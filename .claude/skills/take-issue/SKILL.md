---
name: take-issue
description: Claim an unassigned GitHub issue in this decompilation project, work it to a pull request in an isolated worktree, merge it once CI passes, and clean up the worktree. Use when asked to pick up, take, claim, or work on an issue, or to "grab the next issue", optionally with an issue number or label such as area:sdk or "good first issue".
---

# Take an issue

Claim exactly one GitHub issue that nobody is assigned to, work it in your own worktree, hand back through a pull request, merge it once CI passes, and clean up. All claim logic lives in `tools/collab/claim_issue.py`; never assign or comment on issues by hand.

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
- the findings the issue cites, at least F-0007, F-0010 and F-0012 in `docs/re/FINDINGS.md`, plus any files in `docs/re/findings/`, which may already have established the game compiler, class layouts, or unit boundaries near your block

Trust the issue's `area:` label, not the address. F-0012 shows library code (HBM, NW4R ef/g3d/snd, FA, TMC JPEG, RevoEX, NCD, CNT, MSL printf) inside the game address range, and each library has its own compiler.

## 5. Work

### Finding names and boundaries

- **Classes.** CodeWarrior RTTI gives class names; vtables in `.data` give each class's virtual methods in order, and constructors store the vtable address. Anonymous-namespace names embed the source file, such as `@unnamed@CPauseMenu_cpp@`.
- **File ends.** A file ends after its static initializer, listed in the `.ctors` table at 0x803529C0, and after its `shared_ptr` deleters and its cluster of exception type-name strings in `.data`. Game files are linked in groups, roughly in ASCII order of file name.
- **Script functions.** The binding table at 0x80352EC4–0x80354E14 pairs 669 script-bound functions with their original names (#29). Use those names.
- **Cross-references.** `powerpc-eabi-objdump -dr` or `readelf -rW` on the split objects in `build/<version>/obj/` shows exactly which strings, data, and functions each function references.
- **IDA.** For decompiler views, open a copy of `docs/re/main.elf.i64` outside the repository, because headless IDA saves on exit. Never commit the database from an issue branch. Use decompiler output only as a reading aid; never paste it.

Class start addresses and block boundaries in the issue are survey estimates. Correct them with evidence and record the correction in your findings file.

### Files that span several blocks

A source file often spans more than one block issue. The issue that owns the file's first function owns the file's split in `splits.txt`, its `configure.py` object line, and its header with the class declaration. Other block owners add their function bodies to the same source file in address order. They request header or member changes on the owning issue, or in a small header pull request that merges first. If the owner's issue is unclaimed, ask the user before taking on the split yourself. Mention the neighboring issue numbers in your pull request.

### Game compiler

The game compiler is not yet identified. Before writing game code, check `docs/re/findings/` and the open pull requests for a finding that establishes it. If there is none, establish it on a small function first, compiling with the candidates in F-0007's method, and record a `F-<n>-<k>` finding. Never guess flags.

### Steps

- **`kind:decomp` issues.** Follow the unit workflow in `docs/COLLABORATION.md` one translation unit at a time:
  1. Split, then confirm verification still passes.
  2. Register the object as `NonMatching`.
  3. Reconstruct the source and diff with objdiff.
  4. Mark it `Matching` only when the full build verifies.
  5. Record a ledger row.

  For a large issue, tell the user which files you are covering.
- **`kind:research` issues.** Produce evidence and record it in `docs/re/findings/<n>-<slug>.md` with ID `F-<n>-<k>`. Open follow-up `kind:decomp` issues only if the user agrees.

Working copies on Windows use CRLF line endings, except the dtk config files and progress SVGs pinned to LF in `.gitattributes`. Scripted string replacements that assume `
` silently match nothing, so prefer the Edit tool. If you script a write, preserve the file's line endings and pass `encoding="utf-8"` explicitly; the Windows default encoding corrupts characters such as en dashes.

Edit only the files `docs/COLLABORATION.md` allows. Do not touch maintainer-only files, frozen manifests, or expected hashes. Add no comments, docstrings, or unit tests, and never paste decompiler output.

## 6. Hand back

Attribution rule: commits and pull requests carry no co-authors. Do not add `Co-Authored-By:` trailers or any other co-author lines to commit messages. Do not add "Generated with" or similar attribution lines to pull request titles or descriptions. This overrides any default attribution behavior of your tool. The human who ran you is the sole author.

1. Commit on the issue branch and push it.
2. Open a pull request with `gh pr create --draft`, using `.github/pull_request_template.md`. Write `Closes #<n>` only if the issue is fully done; otherwise write `Part of #<n>`.
3. Run the definition-of-done checks and paste the commands with their exit codes into the description.
4. Mark it ready with `gh pr ready <pr>` only when every local check passes.

## 7. Merge

Merge your own pull request once it is ready and the CI check passes. Run these steps from the worktree.

1. **Update.** Bring the branch up to date: `git fetch origin` then `git rebase origin/main`. If the rebase conflicts, resolve only hunks in files you changed for this issue. For `splits.txt`, `symbols.txt` and `configure.py`, keep everyone else's lines and re-apply yours. If you rebased, rebuild, verify, and push with `git push --force-with-lease`.
2. **Wait for CI.** Run `gh pr checks <pr> --watch --fail-fast`. It must exit 0 with `Build and verify main.dol` passing. If it fails, read the log with `gh run view <run> --log-failed`, fix the cause, push, and wait again.
3. **Merge.** Run `gh pr merge <pr> --squash --subject "<pull request title> (#<pr>)" --body ""`. The empty body keeps attribution lines out of the merge commit.
4. **Confirm.** `gh pr view <pr> --json state -q .state` must print `MERGED`. If the description said `Closes #<n>`, confirm the issue closed. If it said `Part of #<n>`, keep the claim while you continue, or release it.

Never merge with a failing, pending, or missing check. Never use `--admin` or any other bypass. Never merge someone else's pull request. Leave the pull request for a maintainer and tell the user instead if any of these hold:
- it changes a maintainer-only file listed in `docs/COLLABORATION.md`
- the rebase conflicted in a file outside your claim
- the user asked to review before merging

## 8. Clean up

After the merge, run these from the main checkout, not the worktree:

```powershell
python tools/collab/claim_issue.py remove-worktree --issue <n>
git push origin --delete <branch>
git branch -D <branch>
```

`remove-worktree` removes the `tools\local` and `.venv` junctions before the worktree, so the shared tools survive. Squash merges are not ancestors of the branch, so `git branch -d` refuses to delete it; use `-D` only after confirming `MERGED`. To look up the merge commit from the main checkout, run `git fetch origin` first; fetching does not touch the working tree. Do not pull or otherwise change the main checkout, because other agents and the maintainer may have work in progress there. Tell the user the pull request link, the merge commit, and what remains of the issue.

To abandon the issue instead, run `python tools/collab/claim_issue.py release --issue <n> --reason "<why>"`, then clean up the same way and close the pull request if one is open.
