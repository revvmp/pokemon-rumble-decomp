import argparse
import sys
from concurrent.futures import ThreadPoolExecutor
from datetime import datetime, timezone
from itertools import combinations

from claim_issue import REPO, api_items, auto_merged, file_list, linked_issue, open_pulls, parse_time, pull_files_map


CODE_PREFIXES = ("src/", "include/")


def age_text(minutes):
    if minutes < 60:
        return f"{int(minutes)}m"
    if minutes < 24 * 60:
        return f"{int(minutes // 60)}h{int(minutes % 60):02d}m"
    return f"{int(minutes // (24 * 60))}d{int(minutes % (24 * 60) // 60)}h"


def minutes_since(text, now):
    return (now - parse_time(text)).total_seconds() / 60


def ready_since(pull):
    events = api_items(f"{REPO}/issues/{pull['number']}/events?per_page=100")
    ready = [event["created_at"] for event in events if event.get("event") == "ready_for_review"]
    return ready[-1] if ready else pull["created_at"]


def ready_times(pulls):
    ready = [pull for pull in pulls if not pull.get("draft")]
    with ThreadPoolExecutor(max_workers=8) as pool:
        return dict(zip([pull["number"] for pull in ready], pool.map(ready_since, ready)))


def docs_only(files):
    return bool(files) and all(name.startswith("docs/") or name.endswith(".md") for name in files)


def flags_for(pull, files, ready_at, now, args):
    flags = []
    idle = minutes_since(pull["updated_at"], now)
    if pull.get("draft"):
        if idle > args.idle:
            flags.append(f"draft idle {age_text(idle)}")
        if docs_only(files):
            flags.append("docs-only draft")
    elif ready_at and minutes_since(ready_at, now) > args.ready:
        flags.append(f"ready unmerged {age_text(minutes_since(ready_at, now))}")
    return flags


def print_pulls(pulls, files, ready, now, args):
    width = min(max([len(pull["head"]["ref"]) for pull in pulls] + [6]), 44)
    print(f"{'PR':<6}{'state':<7}{'idle':>7}  {'branch':<{width}}  {'issue':<13}{'code':<6}flags")
    flagged = 0
    for pull in pulls:
        names = files[pull["number"]]
        flags = flags_for(pull, names, ready.get(pull["number"]), now, args)
        flagged += bool(flags)
        state = "draft" if pull.get("draft") else "ready"
        code = "yes" if any(name.startswith(CODE_PREFIXES) for name in names) else "no"
        idle = age_text(minutes_since(pull["updated_at"], now))
        print(f"#{pull['number']:<5}{state:<7}{idle:>7}  {pull['head']['ref'][:width]:<{width}}  {linked_issue(pull) or '-':<13}{code:<6}{'; '.join(flags)}")
    print(f"{len(pulls)} open pull request(s), {flagged} flagged")


def print_overlaps(pulls, files, args):
    sets = {number: set(names) for number, names in files.items()}
    if not args.all_files:
        ignored = auto_merged(sorted(set().union(*sets.values())))
        sets = {number: names - ignored for number, names in sets.items()}
    pairs = []
    for left, right in combinations(sorted(sets), 2):
        shared = sorted(sets[left] & sets[right])
        if shared:
            pairs.append((left, right, shared))
    print()
    print("Overlapping pull requests" + ("" if args.all_files else " (ignoring union and decomp-config merged files)") + ":")
    for left, right, shared in pairs:
        print(f"  #{left} and #{right} ({len(shared)} file(s)): {file_list(shared)}")
    if not pairs:
        print("  none")


def main():
    parser = argparse.ArgumentParser(description="List open pull requests with idle, readiness, and file-overlap flags. Read-only.")
    parser.add_argument("--idle", type=float, default=60, help="flag drafts not updated for this many minutes (default 60)")
    parser.add_argument("--ready", type=float, default=30, help="flag ready pull requests unmerged for this many minutes (default 30)")
    parser.add_argument("--all-files", action="store_true", help="count union and decomp-config merged files as overlaps")
    args = parser.parse_args()
    try:
        pulls = sorted(open_pulls(), key=lambda pull: pull["number"])
        files = pull_files_map(pulls)
        ready = ready_times(pulls)
    except (OSError, RuntimeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 1
    now = datetime.now(timezone.utc)
    print_pulls(pulls, files, ready, now, args)
    print_overlaps(pulls, files, args)
    return 0


if __name__ == "__main__":
    sys.exit(main())
