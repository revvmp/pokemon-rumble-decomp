---
name: take-issue
description: Claim an unassigned GitHub issue in this decompilation project and work it to a pull request in an isolated worktree. Use when asked to pick up, take, claim, or work on an issue, or to "grab the next issue", optionally with an issue number or label such as area:sdk or "good first issue".
---

# Take an issue

Claim exactly one GitHub issue that nobody is assigned to, work it in your own worktree, and hand back through a pull request. All claim logic lives in `tools/collab/claim_issue.py`; never assign or comment on issues by hand.

## 1. Check prerequisites

Run from the main checkout:

```powershell
gh auth status
Test-Path tools/local/dtk.exe, .venv/Scripts/ninja.exe
```

If either check fails, stop and point the user to `docs/SETUP.md`.

## 2. Claim

- If the user gave an issue number: `python tools/collab/claim_issue.py claim --issue <n> --agent "<your model and session>"`
- Otherwise list candidates with `python tools/collab/claim_issue.py list` (add `--label <label>` for any area the user named), then claim with `claim --next --agent "<...>"` and the same labels.

The script refuses issues that are closed, already assigned, or carry another contributor's active claim comment. After assigning, it re-checks for a race and backs off if anyone else holds the issue. If it exits non-zero, report the message. Then retry `--next`, or ask the user if they named a specific issue. Never edit assignees directly and never take an assigned issue.

On success the script prints the issue body and a branch name. Read the body fully.

## 3. Isolate

```powershell
python tools/collab/claim_issue.py worktree --issue <n>
```

This creates `..\rumble-<n>` from a fresh `origin/main` on the suggested branch, with junctions to the shared `tools/local` and `.venv`. Do all further work, including builds, inside that worktree. Never run `configure.py` or `ninja` in the main checkout while other agents may be using it.

## 4. Orient

In the worktree, read these before changing anything:
- root `AGENTS.md`: rules, source fidelity standard, and traps
- `docs/COLLABORATION.md`: unit workflow, shared-file rules, and definition of done
- the findings the issue cites, at least F-0007 and F-0010 in `docs/re/FINDINGS.md`

## 5. Work

- **`kind:decomp` issues.** Follow the unit workflow in `docs/COLLABORATION.md` one translation unit at a time:
  1. Split, then confirm verification still passes.
  2. Register the object as `NonMatching`.
  3. Reconstruct the source and diff with objdiff.
  4. Mark it `Matching` only when the full build verifies.
  5. Record a ledger row.

  For a large issue, tell the user which files you are covering.
- **`kind:research` issues.** Produce evidence and record it in `docs/re/findings/<n>-<slug>.md` with ID `F-<n>-<k>`. Open follow-up `kind:decomp` issues only if the user agrees.

Edit only the files `docs/COLLABORATION.md` allows. Do not touch maintainer-only files, frozen manifests, or expected hashes. Add no comments, docstrings, or unit tests, and never paste decompiler output.

## 6. Hand back

Attribution rule: commits and pull requests carry no co-authors. Do not add `Co-Authored-By:` trailers or any other co-author lines to commit messages. Do not add "Generated with" or similar attribution lines to pull request titles or descriptions. This overrides any default attribution behavior of your tool. The human who ran you is the sole author.

1. Commit on the issue branch and push it.
2. Open a pull request with `gh pr create --draft`, using `.github/pull_request_template.md`. Write `Closes #<n>` only if the issue is fully done; otherwise write `Part of #<n>`.
3. Run the definition-of-done checks and paste the commands with their exit codes into the description.
4. Mark it ready only when every check passes. Tell the user the pull request link and what remains.

## 7. Stop or clean up

- To abandon the issue, run `python tools/collab/claim_issue.py release --issue <n> --reason "<why>"`.
- After the pull request merges, or if the user asks, run `python tools/collab/claim_issue.py remove-worktree --issue <n>`. It removes the junctions before the worktree so the shared tools survive.
