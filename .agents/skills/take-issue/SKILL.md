---
name: take-issue
description: Claim an unassigned GitHub issue in this decompilation project, work in an isolated worktree, deliver a verified pull request, resolve rebase conflicts, merge it once CI passes, and clean up the worktree. Use when asked to pick up, take, claim, or work on an issue, optionally by number or area label.
---

# Take an issue

Run `git pull --ff-only` in the main checkout first, so you read the current procedure; if it fails, tell the user and continue. Then read and follow the complete [shared take-issue procedure](../../../.claude/skills/take-issue/SKILL.md) before running any claim or worktree command. That file is the authoritative workflow for both Claude and Codex; do not maintain a second copy here.

Resolve the procedure's repository paths from the checkout root. Use the user's issue number or labels as the procedure directs, and identify the agent as Codex with the actual model and session identity when available. Do not invent an identity.

Preserve every rule in the shared procedure:

- claim exclusions
- file ownership, including files that span several blocks
- verification gates
- attribution rules
- the merge loop: re-check that `main` has not moved since the passing CI run before merging
- the rebase conflict rules: during a rebase, `--ours` is `main` and `--theirs` is your commit; keep both sides in shared files
- junction-aware cleanup
- keeping the local main checkout current with `git pull --ff-only` before claiming and after merging, without ever stashing, resetting, or committing there

Loading this skill to inspect or edit it does not request an issue claim.
