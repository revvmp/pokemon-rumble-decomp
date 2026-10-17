import argparse
import json
import os
import re
import subprocess
import sys
from datetime import datetime, timedelta, timezone
from pathlib import Path


CLAIM_PREFIX = "Claiming"
RELEASE_PREFIX = "Releasing claim"
BLOCKED_LABEL = "blocked"
CLAIM_EXPIRY = timedelta(days=14)
REPO_ROOT = Path(__file__).resolve().parents[2]
SHARED_LINKS = (Path("tools") / "local", Path(".venv"))


def gh(*args):
    result = subprocess.run(["gh", *args], cwd=REPO_ROOT, capture_output=True, text=True, encoding="utf-8")
    if result.returncode != 0:
        raise RuntimeError(f"gh {' '.join(args[:3])} failed: {result.stderr.strip()}")
    return result.stdout


def git(*args, cwd=REPO_ROOT):
    result = subprocess.run(["git", *args], cwd=cwd, capture_output=True, text=True, encoding="utf-8")
    if result.returncode != 0:
        raise RuntimeError(f"git {' '.join(args[:3])} failed: {result.stderr.strip()}")
    return result.stdout


def current_user():
    return gh("api", "user", "--jq", ".login").strip()


def fetch_issue(number):
    fields = "number,title,state,assignees,labels,body,comments,url"
    return json.loads(gh("issue", "view", str(number), "--json", fields))


def parse_time(text):
    return datetime.fromisoformat(text.replace("Z", "+00:00"))


def active_claims(issue):
    claims = {}
    for comment in issue.get("comments", []):
        author = (comment.get("author") or {}).get("login", "")
        body = comment.get("body", "").strip()
        created = parse_time(comment["createdAt"])
        if body.startswith(CLAIM_PREFIX):
            claims[author] = created
        elif body.startswith(RELEASE_PREFIX):
            claims.pop(author, None)
    now = datetime.now(timezone.utc)
    return {author: created for author, created in claims.items() if now - created < CLAIM_EXPIRY}


def assignee_logins(issue):
    return sorted(person["login"] for person in issue.get("assignees", []))


def blocking_reason(issue, user):
    if issue["state"] != "OPEN":
        return f"issue #{issue['number']} is {issue['state'].lower()}"
    assignees = assignee_logins(issue)
    if assignees:
        return f"issue #{issue['number']} is assigned to {', '.join(assignees)}"
    others = sorted(author for author in active_claims(issue) if author != user)
    if others:
        return f"issue #{issue['number']} has an active claim comment from {', '.join(others)}"
    return None


def label_names(issue):
    return [label["name"] for label in issue.get("labels", [])]


def open_unassigned(labels):
    query = "no:assignee" if BLOCKED_LABEL in labels else f"no:assignee -label:{BLOCKED_LABEL}"
    search = ["issue", "list", "--state", "open", "--search", query, "--limit", "500", "--json", "number,title,labels"]
    for label in labels:
        search += ["--label", label]
    issues = json.loads(gh(*search))
    issues.sort(key=lambda issue: ("good first issue" not in label_names(issue), issue["number"]))
    return issues


def print_issue_line(issue):
    print(f"#{issue['number']:<4} {issue['title']}  [{', '.join(label_names(issue))}]")


def list_command(args):
    issues = open_unassigned(args.label)
    for issue in issues:
        print_issue_line(issue)
    print(f"{len(issues)} open unassigned issue(s)")
    return 0


def choose_issue(args, user):
    if args.issue is not None:
        issue = fetch_issue(args.issue)
        reason = blocking_reason(issue, user)
        if reason:
            raise ValueError(f"Cannot claim: {reason}")
        return issue
    for candidate in open_unassigned(args.label):
        issue = fetch_issue(candidate["number"])
        if blocking_reason(issue, user) is None:
            return issue
    raise ValueError("No claimable issue matches the filters")


def claim_command(args):
    user = current_user()
    issue = choose_issue(args, user)
    number = issue["number"]
    if args.dry_run:
        print(f"Would claim #{number} for {user}: {issue['title']}")
        return 0
    gh("issue", "edit", str(number), "--add-assignee", "@me")
    confirmed = fetch_issue(number)
    if BLOCKED_LABEL in label_names(confirmed):
        gh("issue", "edit", str(number), "--remove-label", BLOCKED_LABEL)
    if assignee_logins(confirmed) != [user]:
        gh("issue", "edit", str(number), "--remove-assignee", "@me")
        raise RuntimeError(f"Lost the claim race on #{number}: assignees are now {', '.join(assignee_logins(confirmed))}; unassigned self")
    agent = f" Agent: {args.agent}." if args.agent else ""
    gh("issue", "comment", str(number), "--body", f"{CLAIM_PREFIX}.{agent}")
    confirmed = fetch_issue(number)
    rivals = sorted(author for author in active_claims(confirmed) if author != user)
    if rivals:
        gh("issue", "edit", str(number), "--remove-assignee", "@me")
        gh("issue", "comment", str(number), "--body", f"{RELEASE_PREFIX}: concurrent claim by {', '.join(rivals)}.")
        raise RuntimeError(f"Concurrent claim on #{number} by {', '.join(rivals)}; released")
    print(f"Claimed #{number} for {user}: {confirmed['url']}")
    print(f"Branch suggestion: {branch_name(confirmed)}")
    print()
    print(confirmed["body"])
    return 0


def release_command(args):
    user = current_user()
    issue = fetch_issue(args.issue)
    if user not in assignee_logins(issue) and user not in active_claims(issue):
        raise ValueError(f"#{args.issue} is not claimed by {user}")
    gh("issue", "edit", str(args.issue), "--remove-assignee", "@me")
    if args.blocked:
        gh("issue", "edit", str(args.issue), "--add-label", BLOCKED_LABEL)
    reason = f" {args.reason}" if args.reason else ""
    gh("issue", "comment", str(args.issue), "--body", f"{RELEASE_PREFIX}.{reason}")
    print(f"Released #{args.issue}")
    return 0


def branch_name(issue):
    slug = re.sub(r"[^a-z0-9]+", "-", issue["title"].lower()).strip("-")
    return f"{issue['number']}-{slug[:40].rstrip('-')}"


def worktree_path(number):
    return REPO_ROOT.parent / f"rumble-{number}"


def make_link(link, target):
    if os.name == "nt":
        result = subprocess.run(["cmd", "/c", "mklink", "/J", str(link), str(target)], capture_output=True, text=True)
        if result.returncode != 0:
            raise RuntimeError(f"Could not create junction {link}: {result.stdout.strip()} {result.stderr.strip()}")
    else:
        link.symlink_to(target, target_is_directory=True)


def remove_link(link):
    if os.name == "nt":
        subprocess.run(["cmd", "/c", "rmdir", str(link)], check=True, capture_output=True)
    else:
        link.unlink()


def worktree_command(args):
    user = current_user()
    issue = fetch_issue(args.issue)
    if user not in assignee_logins(issue):
        raise ValueError(f"Claim #{args.issue} before creating its worktree")
    path = worktree_path(args.issue)
    if path.exists():
        raise ValueError(f"Worktree path already exists: {path}")
    for shared in SHARED_LINKS:
        if not (REPO_ROOT / shared).is_dir():
            raise ValueError(f"Missing shared directory {shared}; follow docs/SETUP.md first")
    branch = branch_name(issue)
    git("fetch", "origin", "main")
    git("worktree", "add", str(path), "-b", branch, "origin/main")
    for shared in SHARED_LINKS:
        make_link(path / shared, REPO_ROOT / shared)
    print(f"Worktree {path} on branch {branch}")
    return 0


def remove_worktree_command(args):
    path = worktree_path(args.issue)
    if not path.exists():
        raise ValueError(f"No worktree at {path}")
    for shared in SHARED_LINKS:
        link = path / shared
        if link.exists() or link.is_symlink():
            remove_link(link)
    git("worktree", "remove", "--force", str(path))
    for shared in SHARED_LINKS:
        if not (REPO_ROOT / shared).is_dir():
            raise RuntimeError(f"Shared directory {shared} is missing after removal")
    print(f"Removed worktree {path}; branch kept")
    return 0


def main():
    parser = argparse.ArgumentParser(description="Claim GitHub issues and prepare isolated worktrees for parallel work.")
    commands = parser.add_subparsers(dest="command", required=True)
    listing = commands.add_parser("list", help="List open issues with no assignee")
    listing.add_argument("--label", action="append", default=[])
    listing.set_defaults(action=list_command)
    claim = commands.add_parser("claim", help="Assign an unassigned issue to yourself and post a claim comment")
    target = claim.add_mutually_exclusive_group(required=True)
    target.add_argument("--issue", type=int)
    target.add_argument("--next", action="store_true")
    claim.add_argument("--label", action="append", default=[])
    claim.add_argument("--agent", help="Name of the LLM agent or session doing the work")
    claim.add_argument("--dry-run", action="store_true")
    claim.set_defaults(action=claim_command)
    release = commands.add_parser("release", help="Unassign yourself and post a release comment")
    release.add_argument("--issue", type=int, required=True)
    release.add_argument("--reason")
    release.add_argument("--blocked", action="store_true", help=f"label the issue {BLOCKED_LABEL!r} so list and --next skip it until someone claims it by number")
    release.set_defaults(action=release_command)
    tree = commands.add_parser("worktree", help="Create ../rumble-<issue> with shared tool junctions")
    tree.add_argument("--issue", type=int, required=True)
    tree.set_defaults(action=worktree_command)
    untree = commands.add_parser("remove-worktree", help="Remove junctions, then the worktree")
    untree.add_argument("--issue", type=int, required=True)
    untree.set_defaults(action=remove_worktree_command)
    args = parser.parse_args()
    try:
        return args.action(args)
    except (OSError, ValueError, RuntimeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
