import argparse
import hashlib
import json
import re
import sys
from datetime import datetime, timezone
from pathlib import Path, PurePosixPath


CHUNK_SIZE = 1024 * 1024


def load_manifest(path, sealed):
    raw = path.read_bytes()
    manifest = json.loads(raw)
    if not isinstance(manifest, dict) or type(manifest.get("schema_version")) is not int or manifest["schema_version"] != 1:
        raise ValueError("Expected a manifest object with schema_version 1")
    for field in ("target_id", "title", "region", "revision", "provenance", "scope"):
        if not isinstance(manifest.get(field), str) or not manifest[field].strip():
            raise ValueError(f"Manifest requires nonempty {field}")
    artifacts = manifest.get("artifacts")
    if not isinstance(artifacts, list) or not artifacts:
        raise ValueError("Manifest requires at least one artifact")
    paths = set()
    for artifact in artifacts:
        if not isinstance(artifact, dict):
            raise ValueError("Every artifact must be an object")
        name = artifact.get("path")
        if not isinstance(name, str) or not name or "\\" in name or ":" in name or "\x00" in name:
            raise ValueError(f"Invalid artifact path: {name!r}")
        relative = PurePosixPath(name)
        if relative.is_absolute() or any(part in ("", ".", "..") for part in name.split("/")):
            raise ValueError(f"Artifact path must be relative and normalized: {name}")
        if name.casefold() in paths:
            raise ValueError(f"Duplicate artifact path: {name}")
        paths.add(name.casefold())
        if artifact.get("kind") not in ("executable", "data", "container"):
            raise ValueError(f"Invalid artifact kind: {name}")
        size = artifact.get("size")
        digest = artifact.get("sha256")
        if sealed:
            if type(size) is not int or size < 0:
                raise ValueError(f"Missing or invalid reference size: {name}")
            if not isinstance(digest, str) or not re.fullmatch(r"[0-9a-f]{64}", digest):
                raise ValueError(f"Missing or invalid reference SHA-256: {name}")
        elif size is not None or digest is not None:
            raise ValueError("Fingerprinting refuses manifests with existing fingerprints")
    return manifest, raw


def artifact_path(root, name):
    path = root
    for part in PurePosixPath(name).parts:
        path = path / part
        if path.is_symlink() or (hasattr(path, "is_junction") and path.is_junction()):
            raise ValueError(f"Artifact path contains a link: {path}")
    resolved = path.resolve(strict=True)
    if not resolved.is_relative_to(root) or not resolved.is_file():
        raise ValueError(f"Artifact is not a regular file within its root: {path}")
    return resolved


def fingerprint(path):
    digest = hashlib.sha256()
    size = 0
    with path.open("rb") as stream:
        while chunk := stream.read(CHUNK_SIZE):
            size += len(chunk)
            digest.update(chunk)
    return {"size": size, "sha256": digest.hexdigest()}


def first_difference(reference, rebuilt):
    offset = 0
    with reference.open("rb") as original, rebuilt.open("rb") as output:
        while True:
            left = original.read(CHUNK_SIZE)
            right = output.read(CHUNK_SIZE)
            if left != right:
                for index, (a, b) in enumerate(zip(left, right)):
                    if a != b:
                        return offset + index
                return offset + min(len(left), len(right))
            if not left:
                return None
            offset += len(left)


def record_fingerprints(args):
    manifest, raw = load_manifest(args.manifest, sealed=False)
    root = args.reference_dir.resolve(strict=True)
    if not root.is_dir():
        raise ValueError("Reference root must be a directory")
    if args.manifest.resolve().is_relative_to(root):
        raise ValueError("Manifest must be outside the reference directory")
    for artifact in manifest["artifacts"]:
        artifact.update(fingerprint(artifact_path(root, artifact["path"])))
    if args.manifest.read_bytes() != raw:
        raise ValueError("Manifest changed during fingerprinting; refusing to overwrite")
    args.manifest.write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    for artifact in manifest["artifacts"]:
        print(f"RECORDED {artifact['path']} {artifact['sha256']}")
    print(f"Fingerprinting complete: {args.manifest}. Review and freeze this manifest.")
    return 0


def verify_artifact(artifact, reference_root, build_root):
    result = {"path": artifact["path"], "kind": artifact["kind"], "status": "FAIL"}
    expected = {"size": artifact["size"], "sha256": artifact["sha256"]}
    result["expected"] = expected
    try:
        reference = artifact_path(reference_root, artifact["path"])
        rebuilt = artifact_path(build_root, artifact["path"])
        if reference.samefile(rebuilt):
            raise ValueError("Reference and rebuilt artifact are the same underlying file")
        result["reference"] = fingerprint(reference)
        result["rebuilt"] = fingerprint(rebuilt)
        result["first_difference_offset"] = first_difference(reference, rebuilt)
        errors = []
        if result["reference"] != expected:
            errors.append("Reference differs from frozen manifest")
        if result["rebuilt"] != expected:
            errors.append("Rebuilt artifact differs from frozen manifest")
        if result["first_difference_offset"] is not None:
            errors.append("Direct byte comparison failed")
        if errors:
            result["error"] = "; ".join(errors)
        else:
            result["status"] = "PASS"
    except (OSError, ValueError, RuntimeError) as error:
        result["error"] = str(error)
    return result


def validate_report_path(args, manifest, build_root):
    if not args.report:
        return None
    report_path = args.report.resolve()
    if not report_path.is_relative_to(build_root):
        raise ValueError("Report must be inside the build directory")
    protected = [args.manifest.resolve()]
    protected.extend((build_root / artifact["path"]).resolve() for artifact in manifest["artifacts"])
    if report_path in protected or (report_path.exists() and report_path.stat().st_nlink > 1):
        raise ValueError("Report would overwrite a manifest, artifact, or hard-linked file")
    return report_path


def verify(args):
    manifest, raw = load_manifest(args.manifest, sealed=True)
    reference_root = args.reference_dir.resolve(strict=True)
    build_root = args.build_dir.resolve(strict=True)
    if not reference_root.is_dir() or not build_root.is_dir():
        raise ValueError("Reference and build roots must be directories")
    if reference_root.is_relative_to(build_root) or build_root.is_relative_to(reference_root):
        raise ValueError("Reference and build directories must be separate and non-overlapping")
    report_path = validate_report_path(args, manifest, build_root)
    results = [verify_artifact(artifact, reference_root, build_root) for artifact in manifest["artifacts"]]
    if args.manifest.read_bytes() != raw:
        raise ValueError("Manifest changed during verification")
    passed = all(result["status"] == "PASS" for result in results)
    report = {
        "schema_version": 1,
        "target_id": manifest["target_id"],
        "scope": manifest["scope"],
        "manifest_sha256": hashlib.sha256(raw).hexdigest(),
        "verified_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if passed else "FAIL",
        "artifacts": results,
    }
    for result in results:
        print(f"{result['status']} {result['path']}")
        if "error" in result:
            print(f"  {result['error']}")
        if result.get("first_difference_offset") is not None:
            print(f"  First differing byte: 0x{result['first_difference_offset']:X}")
    if report_path:
        report_path.parent.mkdir(parents=True, exist_ok=True)
        report_path.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        print(f"Report: {report_path}")
    print(f"{report['status']}: {len(results)} declared artifact(s); scope: {manifest['scope']}")
    return 0 if passed else 1


def main():
    parser = argparse.ArgumentParser(description="Fingerprint original artifacts and verify exact reconstructed bytes.")
    commands = parser.add_subparsers(dest="command", required=True)
    capture = commands.add_parser("fingerprint", help="Seal a reference manifest with no existing fingerprints")
    capture.add_argument("--manifest", type=Path, required=True)
    capture.add_argument("--reference-dir", type=Path, required=True)
    capture.set_defaults(action=record_fingerprints)
    check = commands.add_parser("verify", help="Compare rebuilt artifacts against frozen references")
    check.add_argument("--manifest", type=Path, required=True)
    check.add_argument("--reference-dir", type=Path, required=True)
    check.add_argument("--build-dir", type=Path, required=True)
    check.add_argument("--report", type=Path)
    check.set_defaults(action=verify)
    args = parser.parse_args()
    try:
        return args.action(args)
    except (OSError, ValueError, RuntimeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
