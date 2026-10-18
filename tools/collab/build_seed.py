import argparse
import json
import os
import re
import shutil
import struct
import subprocess
import sys
import time
from pathlib import Path


VERSION = "wpse-v1"
NINJA_STATE = (".ninja_log", ".ninja_deps")
RECORD = Path("build") / VERSION / "seed-commit"
SEED_FILE = "seed.json"
KEEP = 3
STALE_COPY = 3600
OLD_MTIME = 978307200
DEPS_HEADER = b"# ninjadeps\n"
DEPS_VERSION = 4
DEPS_FLAG = 0x80000000
ALWAYS_RUN = {"PROGRESS"}


def run(*args, cwd, check=True):
    result = subprocess.run(list(args), cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace")
    if check and result.returncode != 0:
        raise RuntimeError(f"{' '.join(str(arg) for arg in args[:3])} failed ({result.returncode}): {(result.stderr or result.stdout).strip()}")
    return result


def git(root, *args, check=True):
    return run("git", *args, cwd=root, check=check)


def toplevel(path):
    return Path(git(path, "rev-parse", "--show-toplevel").stdout.strip())


def head(root):
    return git(root, "rev-parse", "HEAD").stdout.strip()


def default_cache(root):
    common = Path(git(root, "rev-parse", "--path-format=absolute", "--git-common-dir").stdout.strip())
    return common.parent.parent / "rumble-build-seed"


def ninja_path(root):
    for candidate in (root / ".venv" / "Scripts" / "ninja.exe", root / ".venv" / "bin" / "ninja"):
        if candidate.exists():
            return candidate
    return Path(shutil.which("ninja") or "ninja")


def copy_tree(source, target):
    if os.name == "nt":
        result = subprocess.run(["robocopy", str(source), str(target), "/E", "/COPY:DAT", "/DCOPY:T", "/MT:16", "/R:1", "/W:1", "/NFL", "/NDL", "/NJH", "/NJS", "/NP"], capture_output=True, text=True, errors="replace")
        if result.returncode >= 8:
            raise RuntimeError(f"robocopy {source} failed ({result.returncode}): {result.stdout.strip()[-400:]}")
    else:
        shutil.copytree(source, target, copy_function=shutil.copy2, symlinks=True)


def remove_tree(path):
    if path.exists():
        shutil.rmtree(path)


def clean_tree(root):
    return git(root, "status", "--porcelain", "--untracked-files=all").stdout.strip() == ""


def pending_work(root):
    output = run(str(ninja_path(root)), "-n", cwd=root).stdout
    steps = re.findall(r"^\[\d+/\d+\] (\S+)", output, re.MULTILINE)
    return [step for step in steps if step not in ALWAYS_RUN]


def record(root):
    commit = head(root)
    path = root / RECORD
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(commit + "\n", encoding="utf-8")
    return commit


def recorded_commit(root):
    path = root / RECORD
    return path.read_text(encoding="utf-8").strip() if path.is_file() else None


def seeds(cache):
    found = []
    if not cache.is_dir():
        return found
    for entry in cache.iterdir():
        meta = entry / SEED_FILE
        if entry.name.startswith(".") or not meta.is_file():
            continue
        try:
            data = json.loads(meta.read_text(encoding="utf-8"))
        except (OSError, ValueError):
            continue
        found.append((meta.stat().st_mtime, entry, data))
    found.sort(key=lambda item: item[0], reverse=True)
    return found


def prune(cache):
    for _, entry, _ in seeds(cache)[KEEP:]:
        shutil.rmtree(entry, ignore_errors=True)
    now = time.time()
    for entry in cache.iterdir():
        if entry.name.startswith(".") and now - entry.stat().st_mtime > STALE_COPY:
            shutil.rmtree(entry, ignore_errors=True)


def store(root, cache):
    commit = head(root)
    if recorded_commit(root) != commit:
        raise ValueError(f"{RECORD.as_posix()} does not name HEAD {commit[:10]}; verify this build first")
    if not clean_tree(root):
        raise ValueError("the worktree has uncommitted or untracked files")
    pending = pending_work(root)
    if pending:
        raise ValueError(f"the build is not current with HEAD; ninja would run {len(pending)} step(s)")
    target = cache / commit
    if target.exists():
        os.utime(target / SEED_FILE)
        return target
    cache.mkdir(parents=True, exist_ok=True)
    scratch = cache / f".{commit}.{os.getpid()}"
    remove_tree(scratch)
    try:
        copy_tree(root / "build", scratch / "build")
        for name in NINJA_STATE:
            shutil.copy2(root / name, scratch / name)
        meta = {"commit": commit, "root": root.as_posix(), "stored": int(time.time())}
        (scratch / SEED_FILE).write_text(json.dumps(meta, indent=2) + "\n", encoding="utf-8")
        try:
            os.rename(scratch, target)
        except OSError:
            if not (target / SEED_FILE).is_file():
                raise
    finally:
        remove_tree(scratch)
    prune(cache)
    return target


def rewrite_deps(source, target, old_root, new_root):
    data = source.read_bytes()
    start = len(DEPS_HEADER) + 4
    if not data.startswith(DEPS_HEADER) or struct.unpack_from("<i", data, len(DEPS_HEADER))[0] != DEPS_VERSION:
        raise ValueError(f"{source} is not a version {DEPS_VERSION} ninja deps log")
    old = old_root.rstrip("/") + "/"
    new = new_root.rstrip("/") + "/"
    output = bytearray(data[:start])
    names = set()
    offset = start
    while offset + 4 <= len(data):
        (size,) = struct.unpack_from("<I", data, offset)
        length = size & ~DEPS_FLAG
        payload = data[offset + 4:offset + 4 + length]
        if len(payload) != length:
            break
        offset += 4 + length
        if not size & DEPS_FLAG:
            name = payload[:-4].rstrip(b"\0").decode("utf-8", "surrogateescape")
            if name.lower().startswith(old.lower()):
                name = new + name[len(old):]
            if name in names:
                raise ValueError(f"rewriting {source} duplicates {name}")
            names.add(name)
            raw = name.encode("utf-8", "surrogateescape")
            payload = raw + b"\0" * (-len(raw) % 4) + payload[-4:]
            size = len(payload)
        output += struct.pack("<I", size) + payload
    target.write_bytes(bytes(output))


def tracked_files(root):
    output = git(root, "ls-files", "-z").stdout
    return [name for name in output.split("\0") if name]


def changed_files(root, commit):
    output = git(root, "diff", "--name-only", "--no-renames", "-z", commit, "HEAD").stdout
    return [name for name in output.split("\0") if name]


def set_mtimes(root, names, stamp):
    for name in names:
        path = root / name
        if path.is_file() and not path.is_symlink():
            os.utime(path, stamp)


def choose_seed(root, cache):
    for _, entry, data in seeds(cache):
        commit = data.get("commit", "")
        if git(root, "cat-file", "-e", f"{commit}^{{commit}}", check=False).returncode == 0:
            return entry, data
    return None, None


def discard_seed(root):
    remove_tree(root / "build")
    for name in NINJA_STATE:
        (root / name).unlink(missing_ok=True)


def apply(root, cache):
    if (root / "build").exists() or any((root / name).exists() for name in NINJA_STATE):
        raise ValueError(f"{root} already has build state")
    entry, data = choose_seed(root, cache)
    if entry is None:
        raise ValueError(f"no usable seed in {cache}")
    commit = data["commit"]
    changed = changed_files(root, commit)
    try:
        copy_tree(entry / "build", root / "build")
        shutil.copy2(entry / ".ninja_log", root / ".ninja_log")
        rewrite_deps(entry / ".ninja_deps", root / ".ninja_deps", data["root"], root.as_posix())
        (root / RECORD).unlink(missing_ok=True)
        set_mtimes(root, tracked_files(root), (OLD_MTIME, OLD_MTIME))
        set_mtimes(root, changed, None)
    except Exception:
        discard_seed(root)
        raise
    return commit, len(changed)


def seed_worktree(root, cache=None):
    cache = cache or default_cache(root)
    try:
        commit, count = apply(root, cache)
    except (OSError, ValueError, RuntimeError, KeyError) as error:
        print(f"Build seed not used ({error}); the first build will be a full build.")
        return False
    print(f"Seeded build/ from {commit[:10]}; ninja rebuilds what {count} changed file(s) affect.")
    return True


def main():
    parser = argparse.ArgumentParser(description="Store a verified build as a seed, or seed a fresh worktree's build directory from the newest stored seed.")
    parser.add_argument("command", choices=["record", "store", "apply", "list"])
    parser.add_argument("--cache", type=Path, help="seed directory (default: rumble-build-seed beside the main checkout)")
    args = parser.parse_args()
    try:
        root = toplevel(Path.cwd())
        cache = args.cache or default_cache(root)
        if args.command == "record":
            print(f"Recorded {record(root)[:10]} in {RECORD.as_posix()}")
        elif args.command == "store":
            print(f"Stored seed {store(root, cache)}")
        elif args.command == "apply":
            return 0 if seed_worktree(root, cache) else 1
        else:
            for stamp, entry, data in seeds(cache):
                print(f"{time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(stamp))}  {data.get('commit', '')[:10]}  {data.get('root', '')}")
    except (OSError, ValueError, RuntimeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
