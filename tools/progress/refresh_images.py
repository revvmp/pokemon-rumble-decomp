import argparse
import os
import subprocess
import sys
import tempfile
import time
from pathlib import Path


IMAGE_DIR = "docs/images"
MESSAGE = "DOCS: refresh progress images"


def git(*args, cwd, env=None, check=True):
    result = subprocess.run(["git", *args], cwd=cwd, capture_output=True, text=True, encoding="utf-8", env=env)
    if check and result.returncode != 0:
        raise RuntimeError(f"git {' '.join(args[:3])} failed: {result.stderr.strip()}")
    return result


def main():
    parser = argparse.ArgumentParser(description="Render progress images from a verified local build and commit them to main if main still matches that build.")
    parser.add_argument("--build-dir", type=Path, default=Path("build/wpse-v1"))
    parser.add_argument("--remote", default="origin")
    parser.add_argument("--branch", default="main")
    parser.add_argument("--dry-run", action="store_true", help="render and report, but do not push")
    parser.add_argument("--min-interval", type=float, default=60, metavar="MINUTES", help="skip unless the last image refresh on the branch is at least this old (default: 60; 0 always refreshes)")
    args = parser.parse_args()

    root = Path(git("rev-parse", "--show-toplevel", cwd=Path.cwd()).stdout.strip())
    report = (root / args.build_dir / "report.json").resolve()
    if not report.is_file():
        print(f"ERROR: {report} is missing; build and verify first", file=sys.stderr)
        return 1

    remote_ref = f"{args.remote}/{args.branch}"
    git("fetch", "--quiet", args.remote, args.branch, cwd=root)
    moved = [name for name in git("diff", "--name-only", "HEAD", remote_ref, cwd=root).stdout.splitlines() if name and not name.startswith(f"{IMAGE_DIR}/")]
    if moved:
        print(f"{remote_ref} differs from this build in {len(moved)} file(s); the next merge refreshes the images.")
        return 0
    last = git("log", "-1", "--format=%ct", f"--grep=^{MESSAGE}$", remote_ref, cwd=root).stdout.strip()
    age = time.time() - int(last) if last else None
    if age is not None and age < args.min_interval * 60:
        print(f"The last image refresh on {remote_ref} is {int(age // 60)} minute(s) old, under --min-interval {args.min_interval}; skipping.")
        return 0

    with tempfile.TemporaryDirectory() as scratch:
        scratch = Path(scratch)
        output = scratch / "images"
        render = subprocess.run([sys.executable, str(root / "tools/progress/progress.py"), "render", "--phases", str(root / "config/progress.json"), "--report", str(report), "--output-dir", str(output)], cwd=root)
        if render.returncode != 0:
            return render.returncode

        env = dict(os.environ, GIT_INDEX_FILE=str(scratch / "index"))
        base = git("rev-parse", remote_ref, cwd=root).stdout.strip()
        git("read-tree", base, cwd=root, env=env)
        for image in sorted(output.glob("*.svg")):
            path = f"{IMAGE_DIR}/{image.name}"
            blob = git("hash-object", "-w", "--path", path, str(image), cwd=root).stdout.strip()
            git("update-index", "--add", "--cacheinfo", f"100644,{blob},{path}", cwd=root, env=env)
        tree = git("write-tree", cwd=root, env=env).stdout.strip()
        if tree == git("rev-parse", f"{base}^{{tree}}", cwd=root).stdout.strip():
            print("Progress images are already current.")
            return 0
        commit = git("commit-tree", tree, "-p", base, "-m", MESSAGE, cwd=root).stdout.strip()

    if args.dry_run:
        print(f"Would push {commit[:10]} onto {remote_ref} ({base[:10]}).")
        return 0
    push = git("push", "--quiet", args.remote, f"{commit}:refs/heads/{args.branch}", cwd=root, check=False)
    if push.returncode != 0:
        print(f"{remote_ref} moved during the refresh; the next merge refreshes the images.")
        return 0
    print(f"Pushed {commit[:10]} {MESSAGE}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
