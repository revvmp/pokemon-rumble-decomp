---
name: take-issue
description: Claim an unassigned GitHub issue in this decompilation project, work in an isolated worktree, deliver a verified pull request, resolve rebase conflicts, merge it once verified locally against the latest main, and clean up the worktree. Use when asked to pick up, take, claim, or work on an issue, optionally by number or area label.
---

# Take an issue

Run `git pull --ff-only origin main` in the main checkout first, so you read the current procedure; if it fails, tell the user and continue. Then read and follow the complete [shared take-issue procedure](../../../.claude/skills/take-issue/SKILL.md) before running any claim or worktree command. That file is the authoritative workflow for both Claude and Codex; do not maintain a second copy here.

Resolve the procedure's repository paths from the checkout root. Use the user's issue number or labels as the procedure directs, and identify the agent as Codex with the actual model and session identity when available. Do not invent an identity.

Preserve every rule in the shared procedure:

- claim exclusions, and `release --blocked` when the remaining work lacks evidence or a reference
- file ownership, including files that span several blocks
- verification gates
- attribution rules
- the merge loop: rebase onto `origin/main`, confirm `git status --porcelain` is empty, verify locally with `configure.py`, `ninja`, and `verify.py`, confirm `main` has not moved, then merge with `--match-head-commit` set to the verified commit; GitHub CI no longer runs, so local verification is the only gate
- the `decomp-config` merge driver for `splits.txt`, `symbols.txt` and `configure.py`, with by-hand resolution only for the entries it marks
- the rebase conflict rules: during a rebase, `--ours` is `main` and `--theirs` is your commit; keep both sides in shared files
- union-merged tables: ledger files and `docs/re/SYMBOLS.md` merge automatically; check afterwards for duplicated or interleaved rows
- shared headers: use and extend the Petari-derived RVL_SDK headers in `include/revolution/` and `include/private/` (D-0013); never add files under `include/RVL_SDK/`, which only forwards; on an add/add header conflict, keep `main`'s version and add only what you still need
- outside symbols: you may name callees or data in other blocks in `symbols.txt` so your link resolves, but leave their splits, sizes, and types to the owning issue
- traps: do not edit `AGENTS.md`; record traps under a `Traps` heading in your findings file `docs/re/findings/<n>-<slug>.md`
- file hygiene on Windows: CRLF working copies, explicit UTF-8 without a byte-order mark, and scratch files named with your issue number
- progress images: right after merging, run `python tools/progress/refresh_images.py` from the worktree before cleanup
- inner loop and overlap: iterate with `python tools/re/unitdiff.py <unit>`, reserve full `ninja` for split and `Matching` checks, and run `python tools/collab/claim_issue.py overlap` before opening a pull request
- REST fallbacks (`gh api .../pulls`, `.../merge` with `sha`) when the shared GraphQL quota is exhausted
- junction-aware cleanup
- keeping the local main checkout current with `git pull --ff-only origin main` before claiming and after merging, without ever stashing, resetting, or committing there

Loading this skill to inspect or edit it does not request an issue claim.
