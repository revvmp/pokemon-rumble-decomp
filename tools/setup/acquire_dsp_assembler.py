import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import platform
import shutil
import stat
import subprocess
import sys
import tempfile
import urllib.request
import zipfile


ROOT = Path(__file__).resolve().parents[2]
CHUNK_SIZE = 1024 * 1024


def sha256(path):
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(CHUNK_SIZE), b""):
            digest.update(chunk)
    return digest.hexdigest()


def child_path(directory, relative):
    name = PurePosixPath(relative)
    if name.is_absolute() or not name.parts or any(part in (".", "..") or ":" in part or "\\" in part for part in name.parts):
        raise RuntimeError(f"Unsafe relative path: {relative}")
    target = directory.joinpath(*name.parts).resolve()
    if not target.is_relative_to(directory.resolve()):
        raise RuntimeError(f"Path escapes its directory: {relative}")
    return target


def verify_file(path, expected, size=None):
    if not path.is_file():
        raise RuntimeError(f"Missing file: {path}")
    if size is not None and path.stat().st_size != size:
        raise RuntimeError(f"Wrong size at {path}: {path.stat().st_size}, expected {size}")
    actual = sha256(path)
    if actual != expected:
        raise RuntimeError(f"Wrong digest at {path}: {actual}, expected {expected}")


def acquire_file(entry, cache, offline):
    target = child_path(cache, entry["cache"])
    if target.exists():
        verify_file(target, entry["sha256"], entry["size"])
        return target
    if offline:
        raise RuntimeError(f"Offline cache is missing {target}")
    target.parent.mkdir(parents=True, exist_ok=True)
    partial = target.with_name(target.name + ".part")
    print(f"Downloading {entry['url']}", flush=True)
    try:
        with urllib.request.urlopen(entry["url"], timeout=120) as response, partial.open("wb") as output:
            shutil.copyfileobj(response, output, CHUNK_SIZE)
        verify_file(partial, entry["sha256"], entry["size"])
        partial.replace(target)
    finally:
        partial.unlink(missing_ok=True)
    return target


def trim_dsp_core(content):
    notice_end = content.index(b"#pragma once")
    enums_start = content.index(b"enum : u32")
    enums_end = content.index(b"struct DSP_Regs")
    if not notice_end < enums_start < enums_end:
        raise RuntimeError("Unexpected DSPCore.h enum boundaries")
    prefix = b'#pragma once\n#include "Common/CommonTypes.h"\nnamespace DSP {\n'
    return content[:notice_end] + prefix + content[enums_start:enums_end] + b"\n}\n"


def staged_sources(manifest, cache, host_root):
    staged = {}
    for entry in manifest["sources"]:
        source = child_path(cache, entry["cache"])
        content = source.read_bytes()
        transform = entry.get("transform")
        if transform == "dsp_core_enums":
            content = trim_dsp_core(content)
        elif transform is not None:
            raise RuntimeError(f"Unknown source transform: {transform}")
        if entry["target"] in staged:
            raise RuntimeError(f"Duplicate staged input: {entry['target']}")
        staged[entry["target"]] = content
    for entry in manifest["host_sources"]:
        source = child_path(host_root, entry["path"])
        if entry["line_endings"] != "lf":
            raise RuntimeError(f"Unsupported host-source line endings: {source}")
        content = source.read_bytes().replace(b"\r\n", b"\n")
        if hashlib.sha256(content).hexdigest() != entry["sha256"]:
            raise RuntimeError(f"Wrong normalized host-source digest at {source}")
        if entry["target"] in staged:
            raise RuntimeError(f"Duplicate host input: {entry['target']}")
        staged[entry["target"]] = content
    return staged


def extract_compiler(archive, directory, compiler):
    directory.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(archive) as source:
        for member in source.infolist():
            child_path(directory, member.filename.rstrip("/"))
            if stat.S_ISLNK(member.external_attr >> 16):
                raise RuntimeError(f"Archive contains a symbolic link: {member.filename}")
        source.extractall(directory)
    compiler_root = child_path(directory, compiler["root"])
    executable = child_path(compiler_root, compiler["executable"])
    verify_file(executable, compiler["executable_sha256"])
    verify_file(child_path(compiler_root, compiler["license"]), compiler["license_sha256"])
    return executable


def verify_install(manifest, manifest_path, local, staged):
    report = json.loads((local / "build.json").read_text(encoding="utf-8"))
    if report["manifest_sha256"] != sha256(manifest_path):
        raise RuntimeError("Installed DSP assembler was built with a different manifest")
    build_directory = child_path(local, report["build_directory"])
    for relative, content in staged.items():
        verify_file(child_path(build_directory / "source", relative), hashlib.sha256(content).hexdigest(), len(content))
    compiler = manifest["compiler"]
    compiler_root = child_path(build_directory / "compiler", compiler["root"])
    verify_file(child_path(compiler_root, compiler["executable"]), compiler["executable_sha256"])
    verify_file(child_path(compiler_root, compiler["license"]), compiler["license_sha256"])
    binary = manifest["binary"]
    output = child_path(local, binary["install_name"])
    verify_file(output, binary["sha256"], binary["size"])
    print(f"DSP assembler verified: {output}")
    return output


def build(manifest, manifest_path, local, archive, staged):
    builds = local / "builds"
    builds.mkdir(parents=True, exist_ok=True)
    directory = Path(tempfile.mkdtemp(prefix="host-", dir=builds)).resolve()
    if not directory.is_relative_to(local):
        raise RuntimeError("Build directory is outside the tool directory")
    source = directory / "source"
    for relative, content in staged.items():
        target = child_path(source, relative)
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(content)
    compiler = extract_compiler(archive, directory / "compiler", manifest["compiler"])
    environment = os.environ.copy()
    environment.update(manifest["environment"])
    environment["ZIG_GLOBAL_CACHE_DIR"] = str(directory / "zig-global-cache")
    environment["ZIG_LOCAL_CACHE_DIR"] = str(directory / "zig-local-cache")
    command = [str(compiler), *manifest["arguments"]]
    print(f"Building pinned DSP assembler in {directory}", flush=True)
    result = subprocess.run(command, cwd=source, env=environment, capture_output=True)
    (directory / "build.log").write_bytes(result.stdout + result.stderr)
    if result.returncode:
        diagnostic = (result.stdout + result.stderr).decode("utf-8", "replace")
        raise RuntimeError(f"Zig exited {result.returncode}:\n{diagnostic}")
    binary = manifest["binary"]
    built = child_path(source, binary["build_name"])
    verify_file(built, binary["sha256"], binary["size"])
    output = child_path(local, binary["install_name"])
    temporary = output.with_name(output.name + ".part")
    shutil.copyfile(built, temporary)
    report = {
        "manifest_sha256": sha256(manifest_path),
        "build_directory": directory.relative_to(local).as_posix(),
        "command": command,
        "compiler_exit": result.returncode,
        "binary_sha256": sha256(built),
        "source_sha256": {relative: hashlib.sha256(content).hexdigest() for relative, content in staged.items()},
    }
    report_path = local / "build.json"
    report_temporary = local / "build.json.part"
    report_temporary.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    publication = [(temporary, output), (report_temporary, report_path)]
    backups = {}
    for _, target in publication:
        if target.exists():
            backup = directory / ("previous-" + target.name)
            shutil.copyfile(target, backup)
            backups[target] = backup
    published = []
    try:
        for staged_path, target in publication:
            staged_path.replace(target)
            published.append(target)
    except OSError as publication_error:
        recovery_errors = []
        for target in reversed(published):
            try:
                if target in backups:
                    shutil.copyfile(backups[target], target)
                else:
                    target.unlink(missing_ok=True)
            except OSError as recovery_error:
                backup = backups.get(target)
                recovery_errors.append(f"{target}: {recovery_error}; retained backup: {backup}")
        if recovery_errors:
            details = "\n".join(recovery_errors)
            raise RuntimeError(f"Publication failed: {publication_error}\nRecovery failed:\n{details}") from publication_error
        raise
    print(f"DSP assembler built: {output}")
    return output


def acquire(manifest_path, local, cache=None, offline=False, rebuild=False, host_root=ROOT):
    if os.name != "nt" or platform.machine().lower() not in ("amd64", "x86_64"):
        raise RuntimeError("The pinned DSP host tool currently supports Windows x86-64")
    if offline and rebuild:
        raise RuntimeError("--offline verifies existing files and cannot be combined with --rebuild")
    manifest_path = manifest_path.resolve()
    local = local.resolve()
    cache = cache.resolve() if cache else local / "downloads"
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    if manifest["schema_version"] != 1 or manifest["host"] != "windows-x86_64":
        raise RuntimeError("Unsupported DSP assembler manifest")
    if not offline:
        local.mkdir(parents=True, exist_ok=True)
        cache.mkdir(parents=True, exist_ok=True)
    for entry in manifest["sources"]:
        acquire_file(entry, cache, offline)
    archive = acquire_file(manifest["compiler"], cache, offline)
    staged = staged_sources(manifest, cache, host_root.resolve())
    if offline or ((local / "build.json").is_file() and not rebuild):
        return verify_install(manifest, manifest_path, local, staged)
    return build(manifest, manifest_path, local, archive, staged)


def main():
    parser = argparse.ArgumentParser(description="Acquire and build the pinned Dolphin DSP assembler with portable Zig.")
    parser.add_argument("--manifest", type=Path, default=ROOT / "config/dsp_assembler.json")
    parser.add_argument("--local", type=Path, default=ROOT / "tools/local/dsp-assembler")
    parser.add_argument("--cache", type=Path)
    parser.add_argument("--offline", action="store_true", help="verify cached inputs and the installed binary without downloading or building")
    parser.add_argument("--rebuild", action="store_true", help="build in a fresh source and compiler directory")
    args = parser.parse_args()
    try:
        acquire(args.manifest, args.local, args.cache, args.offline, args.rebuild)
    except (OSError, RuntimeError, ValueError, KeyError, zipfile.BadZipFile) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
