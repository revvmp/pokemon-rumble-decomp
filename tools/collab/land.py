import argparse
import json
import re
import subprocess
import sys
import time
from pathlib import Path

import build_seed


REPO = "repos/{owner}/{repo}"
VERSION = "wpse-v1"
MANIFEST = Path("config") / "targets" / f"{VERSION}.json"
BUILD_DIR = Path("build") / VERSION
NINJA_LOG = Path("build") / "land-ninja.log"
NO_BUILD_INPUTS = ("docs/",)
LOCK_RETRIES = 6
CLOSING = r"\b(close[sd]?|fix(e[sd])?|resolve[sd]?)\s*:?\s+#{number}\b"


class Stop(Exception):
    pass


def run(*args, cwd, check=True, capture=True):
    result = subprocess.run([str(arg) for arg in args], cwd=cwd, capture_output=capture, text=True, encoding="utf-8", errors="replace")
    if check and result.returncode != 0:
        detail = (result.stderr or result.stdout or "").strip() if capture else ""
        raise Stop(f"{' '.join(str(arg) for arg in args[:4])} exited {result.returncode}{': ' + detail if detail else ''}")
    return result


def git(root, *args, check=True):
    return run("git", *args, cwd=root, check=check)


def gh(root, *args, check=True):
    return run("gh", *args, cwd=root, check=check)


def step(text):
    print(f"== {text}", flush=True)


def main_checkout(path):
    common = Path(git(path, "rev-parse", "--path-format=absolute", "--git-common-dir").stdout.strip())
    return common.parent


def resolve_worktree(issue):
    here = build_seed.toplevel(Path.cwd())
    main = main_checkout(here)
    if here.resolve() != main.resolve():
        return here, main, True
    tree = main.parent / f"rumble-{issue}"
    if not (tree / ".git").exists():
        raise Stop(f"No worktree at {tree}; run land.py from the issue worktree or create it with claim_issue.py worktree")
    return tree, main, False


def current_branch(root):
    result = git(root, "symbolic-ref", "--quiet", "--short", "HEAD", check=False)
    if result.returncode != 0:
        raise Stop("HEAD is detached; check out the issue branch first")
    return result.stdout.strip()


def rebase_in_progress(root):
    gitdir = Path(git(root, "rev-parse", "--path-format=absolute", "--git-dir").stdout.strip())
    return (gitdir / "rebase-merge").exists() or (gitdir / "rebase-apply").exists()


def fetch(root):
    for attempt in range(LOCK_RETRIES):
        result = git(root, "fetch", "--quiet", "origin", "main", check=False)
        if result.returncode == 0:
            return
        if "lock" not in result.stderr or attempt == LOCK_RETRIES - 1:
            raise Stop(f"git fetch failed: {result.stderr.strip()}")
        time.sleep(3)


def rev(root, name):
    return git(root, "rev-parse", name).stdout.strip()


def contains_main(root):
    return git(root, "merge-base", "--is-ancestor", "origin/main", "HEAD", check=False).returncode == 0


def rebase(root):
    if contains_main(root):
        print("Branch already contains origin/main.")
        return
    result = git(root, "rebase", "origin/main", check=False)
    if result.returncode != 0:
        conflicted = git(root, "diff", "--name-only", "--diff-filter=U").stdout.split()
        listing = "".join(f"\n  {name}" for name in conflicted) or "\n  (see git status)"
        raise Stop(f"Rebase stopped on conflicts:{listing}\nResolve them as the take-issue skill describes, run git rebase --continue, then rerun land.py.")
    print(f"Rebased onto origin/main {rev(root, 'origin/main')[:10]}.")


def require_clean(root):
    status = git(root, "status", "--porcelain", "--untracked-files=all").stdout.strip()
    if status:
        raise Stop(f"The worktree has uncommitted or untracked files; commit or delete them first:\n{status}")


def changed_since(root, commit):
    return [name for name in git(root, "diff", "--name-only", commit, "HEAD").stdout.splitlines() if name]


def only_non_inputs(names):
    return all(name.startswith(NO_BUILD_INPUTS) for name in names)


def timed(label, action):
    start = time.monotonic()
    result = action()
    print(f"{label}: exit {result.returncode} in {time.monotonic() - start:.1f} s", flush=True)
    return result


def build(root):
    (root / BUILD_DIR / "main.dol").unlink(missing_ok=True)
    configure = timed("configure.py", lambda: run(sys.executable, "configure.py", cwd=root, check=False))
    if configure.returncode != 0:
        raise Stop(f"configure.py failed:\n{configure.stdout}{configure.stderr}")
    log = root / NINJA_LOG
    log.parent.mkdir(parents=True, exist_ok=True)
    print(f"ninja output goes to {log}", flush=True)
    with log.open("w", encoding="utf-8") as stream:
        ninja = timed("ninja", lambda: subprocess.run([str(build_seed.ninja_path(root))], cwd=root, stdout=stream, stderr=subprocess.STDOUT))
    if ninja.returncode != 0:
        tail = "\n".join(log.read_text(encoding="utf-8", errors="replace").splitlines()[-40:])
        raise Stop(f"ninja failed; full output in {log}:\n{tail}")


def verify(root):
    result = timed("verify.py", lambda: run(sys.executable, "tools/verify/verify.py", "verify", "--manifest", MANIFEST, "--reference-dir", Path("orig") / VERSION, "--build-dir", BUILD_DIR, cwd=root, check=False))
    print(result.stdout.strip())
    if result.returncode != 0:
        raise Stop(f"Verification failed:\n{result.stderr.strip()}")


def save_seed(root):
    build_seed.record(root)
    try:
        print(f"Stored build seed {build_seed.store(root, build_seed.default_cache(root)).name[:10]}.")
    except (OSError, ValueError, RuntimeError) as error:
        print(f"WARNING: build seed not stored: {error}")


def remote_head(root, branch):
    line = git(root, "ls-remote", "origin", f"refs/heads/{branch}").stdout.split()
    return line[0] if line else ""


def push(root, branch, dry_run):
    remote = remote_head(root, branch)
    head = rev(root, "HEAD")
    if remote == head:
        print(f"origin/{branch} is already {head[:10]}.")
        return
    if dry_run:
        print(f"Would push {head[:10]} to origin/{branch} (now {remote[:10] or 'absent'}).")
        return
    git(root, "push", "--quiet", f"--force-with-lease=refs/heads/{branch}:{remote}", "origin", f"HEAD:refs/heads/{branch}")
    print(f"Pushed {head[:10]} to origin/{branch}.")


def api_json(root, path, *extra):
    return json.loads(gh(root, "api", path, *extra).stdout or "null")


def find_pr(root, branch, number):
    if number:
        return api_json(root, f"{REPO}/pulls/{number}")
    owner = api_json(root, REPO)["owner"]["login"]
    pulls = api_json(root, f"{REPO}/pulls?state=open&head={owner}:{branch}")
    if not pulls:
        raise Stop(f"No open pull request for branch {branch}; create one first (see the take-issue skill)")
    return pulls[0]


def mark_ready(root, pr, dry_run):
    if not pr.get("draft"):
        return
    if dry_run:
        print(f"Would mark #{pr['number']} ready for review.")
        return
    result = gh(root, "pr", "ready", str(pr["number"]), check=False)
    if result.returncode != 0:
        raise Stop(f"#{pr['number']} is a draft and gh pr ready failed ({result.stderr.strip()}). The REST API cannot undraft; mark it ready in the web UI or retry when the GraphQL quota recovers, then rerun land.py.")
    print(f"Marked #{pr['number']} ready for review.")


def merge(root, pr, sha):
    title = f"{pr['title']} (#{pr['number']})"
    result = gh(root, "api", "-X", "PUT", f"{REPO}/pulls/{pr['number']}/merge", "-f", "merge_method=squash", "-f", f"sha={sha}", "-f", f"commit_title={title}", "-f", "commit_message=", check=False)
    if result.returncode != 0:
        return False, (result.stderr or result.stdout).strip()
    return True, json.loads(result.stdout).get("sha", "")


def report_issue(root, issue, pr):
    body = pr.get("body") or ""
    closing = re.search(CLOSING.format(number=issue), body, re.IGNORECASE) is not None
    for attempt in range(3):
        state = api_json(root, f"{REPO}/issues/{issue}")["state"]
        if not closing or state == "closed":
            break
        time.sleep(3)
    print(f"Issue #{issue} is {state}.")
    if closing and state != "closed":
        print(f"WARNING: the pull request says it closes #{issue}, but the issue is still open.")
    if not closing and state == "closed":
        print(f"WARNING: #{issue} closed although the pull request only references it; reopen it if work remains.")


def refresh_images(root, dry_run):
    step("refresh progress images")
    command = [sys.executable, "tools/progress/refresh_images.py"] + (["--dry-run"] if dry_run else [])
    result = run(*command, cwd=root, check=False, capture=False)
    if result.returncode != 0:
        print(f"WARNING: refresh_images.py exited {result.returncode}")


def cleanup(tree, main, issue, branch, dry_run):
    step("clean up")
    remove = ["python", "tools/collab/claim_issue.py", "remove-worktree", "--issue", str(issue)]
    commands = [remove, ["git", "push", "origin", "--delete", branch], ["git", "branch", "-D", branch]]
    if git(main, "symbolic-ref", "--quiet", "--short", "HEAD", check=False).stdout.strip() == "main":
        commands.append(["git", "pull", "--ff-only", "--quiet", "origin", "main"])
    for command in commands:
        if dry_run:
            print(f"Would run in {main}: {' '.join(command)}")
            continue
        actual = [sys.executable, *command[1:]] if command is remove else command
        result = run(*actual, cwd=main, check=False)
        print(f"{' '.join(command)}: exit {result.returncode}")
        if result.returncode != 0:
            print(f"WARNING: {(result.stderr or result.stdout).strip()}")
            if command is remove:
                print(f"Remove {tree} with claim_issue.py remove-worktree once nothing uses it.")


def land(args):
    tree, main, inside = resolve_worktree(args.issue)
    if args.cleanup and inside:
        raise Stop(f"--cleanup removes this worktree, so run it from the main checkout: cd {main}; python tools/collab/land.py --issue {args.issue} --cleanup")
    if rebase_in_progress(tree):
        raise Stop("A rebase is in progress; finish it with git rebase --continue (or --abort) first")
    branch = current_branch(tree)
    print(f"Landing {branch} from {tree}{' (dry run)' if args.dry_run else ''}")
    verified = None
    for attempt in range(1, args.max_rounds + 1):
        step(f"round {attempt}: update")
        fetch(tree)
        rebase(tree)
        require_clean(tree)
        head = rev(tree, "HEAD")
        if verified and head != verified and only_non_inputs(changed_since(tree, verified)):
            step(f"main moved only in {', '.join(NO_BUILD_INPUTS)}; reusing the build verified at {verified[:10]}")
            verify(tree)
        elif head != verified:
            step("build and verify")
            build(tree)
            verify(tree)
            save_seed(tree)
        verified = head
        print(f"Verified {verified}")
        step("push")
        push(tree, branch, args.dry_run)
        pr = find_pr(tree, branch, args.pr)
        print(f"Pull request #{pr['number']}: {pr['title']}")
        mark_ready(tree, pr, args.dry_run)
        fetch(tree)
        if not contains_main(tree):
            print("origin/main moved after verification; starting another round.")
            continue
        if args.dry_run:
            print(f"Would squash-merge #{pr['number']} at {verified[:10]} as \"{pr['title']} (#{pr['number']})\".")
            refresh_images(tree, True)
            if args.cleanup:
                cleanup(tree, main, args.issue, branch, True)
            return 0
        step("merge")
        merged, detail = merge(tree, pr, verified)
        if not merged:
            fetch(tree)
            if contains_main(tree):
                raise Stop(f"GitHub refused the merge: {detail}")
            print(f"GitHub refused the merge because main moved ({detail}); starting another round.")
            continue
        state = api_json(tree, f"{REPO}/pulls/{pr['number']}")
        if not state.get("merged"):
            raise Stop(f"#{pr['number']} is not merged after the merge call; check it on GitHub")
        print(f"Merged #{pr['number']} as {detail[:10]}.")
        report_issue(tree, args.issue, pr)
        refresh_images(tree, False)
        if args.cleanup:
            cleanup(tree, main, args.issue, branch, False)
        else:
            print(f"Next, from {main}: python tools/collab/land.py --issue {args.issue} --cleanup-only")
        return 0
    raise Stop(f"origin/main kept moving for {args.max_rounds} rounds; rerun land.py")


def cleanup_only(args):
    tree, main, inside = resolve_worktree(args.issue)
    if inside:
        raise Stop(f"Run --cleanup-only from the main checkout: cd {main}")
    branch = current_branch(tree)
    pr = find_pr(tree, branch, args.pr) if args.pr else None
    pulls = [pr] if pr else api_json(tree, f"{REPO}/pulls?state=closed&head={api_json(tree, REPO)['owner']['login']}:{branch}")
    if not any(pull and pull.get("merged_at") for pull in pulls):
        raise Stop(f"No merged pull request for {branch}; release the issue or merge first")
    cleanup(tree, main, args.issue, branch, args.dry_run)
    return 0


def main():
    parser = argparse.ArgumentParser(description="Rebase, build, verify, push, and squash-merge an issue branch, looping until it merges onto the latest main; then refresh progress images and optionally remove the worktree.")
    parser.add_argument("--issue", type=int, required=True)
    parser.add_argument("--pr", type=int, help="pull request number (default: the open pull request for the branch)")
    parser.add_argument("--cleanup", action="store_true", help="after merging, remove the worktree and branches; run from the main checkout")
    parser.add_argument("--cleanup-only", action="store_true", help="only remove the worktree and branches of an already merged pull request")
    parser.add_argument("--dry-run", action="store_true", help="run the local steps (rebase, build, verify) and print the remote actions without pushing, merging, or removing anything")
    parser.add_argument("--max-rounds", type=int, default=10)
    args = parser.parse_args()
    try:
        return cleanup_only(args) if args.cleanup_only else land(args)
    except Stop as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1
    except (OSError, ValueError, RuntimeError, KeyError) as error:
        print(f"ERROR: {type(error).__name__}: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
