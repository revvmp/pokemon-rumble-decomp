import argparse
import hashlib
import json
import shutil
import subprocess
import sys
import urllib.request
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CHUNK_SIZE = 1024 * 1024


def sha256(path):
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(CHUNK_SIZE), b""):
            digest.update(chunk)
    return digest.hexdigest()


def pinned_tools(toolchain, local):
    driver = toolchain["build_driver"]
    dtk = toolchain["extraction_tools"][0]
    return [
        {"name": "dtk", "url": dtk["source"], "sha256": dtk["sha256"], "target": local / "dtk.exe"},
        {"name": "sjiswrap", "url": driver["sjiswrap"]["source"], "sha256": driver["sjiswrap"]["sha256"], "target": local / "sjiswrap.exe"},
        {"name": "binutils", "url": toolchain["assembler"]["source"], "sha256": toolchain["assembler"]["archive_sha256"], "target": local / "binutils.zip", "extract": local / "binutils"},
        {"name": "compilers", "url": toolchain["compiler"]["bundle"], "sha256": toolchain["compiler"]["bundle_sha256"], "target": local / "compilers.zip", "extract": local / "compilers"},
    ]


def extracted_checks(toolchain, local):
    version = toolchain["compiler"]["runtime_unit_version"]
    return [
        (local / "compilers" / version / "mwcceppc.exe", toolchain["compiler"]["runtime_unit_mwcceppc_sha256"]),
        (local / "compilers" / toolchain["linker"]["version"] / "mwldeppc.exe", toolchain["linker"]["sha256"]),
    ]


def download(url, target):
    partial = target.with_name(target.name + ".part")
    with urllib.request.urlopen(url, timeout=120) as response, partial.open("wb") as stream:
        shutil.copyfileobj(response, stream, CHUNK_SIZE)
    partial.replace(target)


def acquire(tool, offline):
    target = tool["target"]
    downloaded = False
    if target.exists() and sha256(target) == tool["sha256"]:
        print(f"{tool['name']}: cached, digest verified")
    elif offline:
        raise RuntimeError(f"{tool['name']}: missing or wrong digest at {target}")
    else:
        print(f"{tool['name']}: downloading {tool['url']}")
        download(tool["url"], target)
        actual = sha256(target)
        if actual != tool["sha256"]:
            target.unlink()
            raise RuntimeError(f"{tool['name']}: digest {actual} does not match pinned {tool['sha256']}")
        print(f"{tool['name']}: digest verified")
        downloaded = True
    if "extract" in tool and (downloaded or not tool["extract"].exists()):
        destination = tool["extract"]
        if destination.exists():
            shutil.rmtree(destination)
        with zipfile.ZipFile(target) as archive:
            archive.extractall(destination)
        print(f"{tool['name']}: extracted to {destination}")


def main():
    parser = argparse.ArgumentParser(description="Download the pinned build tools into tools/local and verify every digest from config/toolchain.json.")
    parser.add_argument("--toolchain", type=Path, default=ROOT / "config" / "toolchain.json")
    parser.add_argument("--local", type=Path, default=ROOT / "tools" / "local")
    parser.add_argument("--offline", action="store_true", help="verify existing files without downloading")
    args = parser.parse_args()
    toolchain = json.loads(args.toolchain.read_text(encoding="utf-8"))
    args.local.mkdir(parents=True, exist_ok=True)
    try:
        for tool in pinned_tools(toolchain, args.local):
            acquire(tool, args.offline)
        for path, expected in extracted_checks(toolchain, args.local):
            actual = sha256(path) if path.exists() else "missing"
            if actual != expected:
                raise RuntimeError(f"{path}: digest {actual} does not match pinned {expected}")
            print(f"{path.relative_to(args.local)}: digest verified")
        dsp_command = [sys.executable, str(ROOT / "tools/setup/acquire_dsp_assembler.py"), "--manifest", str(ROOT / toolchain["dsp_assembler"]["manifest"]), "--local", str(args.local / "dsp-assembler")]
        if args.offline:
            dsp_command.append("--offline")
        subprocess.run(dsp_command, check=True)
        dsp_binary = args.local / "dsp-assembler" / "dsp-inspect.exe"
        if sha256(dsp_binary) != toolchain["dsp_assembler"]["sha256"]:
            raise RuntimeError(f"{dsp_binary}: digest differs from config/toolchain.json")
    except (OSError, RuntimeError, KeyError, subprocess.CalledProcessError, zipfile.BadZipFile) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
