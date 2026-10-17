---
name: take-issue
description: Claim an unassigned GitHub issue in this decompilation project, work it to a pull request in an isolated worktree, merge it once verified locally against the latest main, and clean up the worktree. Use when asked to pick up, take, claim, or work on an issue, or to "grab the next issue", optionally with an issue number or label such as area:sdk or "good first issue".
---

# Take an issue

Claim exactly one GitHub issue that nobody is assigned to, work it in your own worktree, hand back through a pull request, merge it once it verifies locally against the latest `main`, and clean up. All claim logic lives in `tools/collab/claim_issue.py`; never assign or comment on issues by hand.

## 1. Check prerequisites

Run from the main checkout:

```powershell
gh auth status
Test-Path tools/local/dtk.exe, .venv/Scripts/ninja.exe
```

If either check fails, stop and point the user to `docs/SETUP.md`.

Then bring the main checkout up to date, so this skill, `tools/collab/claim_issue.py`, `AGENTS.md`, and the docs you read are current:

```powershell
git pull --ff-only
```

Pull only with `--ff-only`, and only while the main checkout is on `main`. It never creates a merge commit, and git refuses rather than overwrite uncommitted changes. If the pull fails, because of local changes, another branch, or another agent's pull holding `.git/index.lock`, do not stash, reset, or commit anything in the main checkout. Retry once after a short wait for a lock; otherwise tell the user and continue. Your worktree is created from a fresh `origin/main` either way. If the pull changed this skill file, re-read it before continuing.

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

### Headers and outside symbols

- **Headers.** RVL_SDK headers already exist in `include/revolution/` and `include/private/`, imported from Petari. Include and extend those rather than writing new ones, and never add files under `include/RVL_SDK/`, which only forwards to the new paths.
- **Outside symbols.** If your unit calls or references symbols in another block, you may give them evidenced names in `symbols.txt` so the link resolves. Leave their splits, sizes, and types to the owning issue.

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
` silently match nothing, so prefer your agent's file-editing tool (Edit in Claude Code, `apply_patch` in Codex). If you script a write, preserve the file's line endings and pass `encoding="utf-8"` explicitly; the Windows default encoding corrupts characters such as en dashes. Do not use PowerShell `Set-Content -Encoding utf8` or `Out-File`, which add a byte-order mark.

Edit only the files `docs/COLLABORATION.md` allows. Do not edit `AGENTS.md`; record any traps you hit under a `Traps` heading in your findings file `docs/re/findings/<n>-<slug>.md`. Do not touch maintainer-only files, frozen manifests, or expected hashes. Add no comments, docstrings, or unit tests, and never paste decompiler output.

## 6. Hand back

Attribution rule: commits and pull requests carry no co-authors. Do not add `Co-Authored-By:` trailers or any other co-author lines to commit messages. Do not add "Generated with" or similar attribution lines to pull request titles or descriptions. This overrides any default attribution behavior of your tool. The human who ran you is the sole author.

1. Commit on the issue branch and push it.
2. Open a pull request with `gh pr create --draft`, using `.github/pull_request_template.md`. Write `Closes #<n>` only if the issue is fully done; otherwise write `Part of #<n>`.
3. Run the definition-of-done checks and paste the commands with their exit codes into the description.
4. Mark it ready with `gh pr ready <pr>` only when every local check passes.

Keep temporary scripts and notes outside the repository in files named with your issue number, such as `rebase-535.py`. Agents may share a scratch directory, and a generic name like `resolve.py` gets overwritten by another agent.

## 7. Merge

Merge your own pull request as soon as you have verified it locally against the latest `main`. GitHub does not build or verify pull requests; local verification is the only gate. You run the digest-pinned tools, `configure.py`, `ninja`, and `verify.py`. After a rebase onto `origin/main`, your branch head has exactly the files the squash merge will produce, so verifying it locally tests the real merge result. Other agents merge at the same time, so this step is a loop. Run it from the worktree.

1. **Update.** Run `git fetch origin` then `git rebase origin/main`. Worktrees share refs, so a concurrent fetch by another agent can fail with `cannot lock ref`; wait a few seconds and retry. Resolve any conflicts as described below.
2. **Check that everything is committed.** `git status --porcelain` must print nothing. The merge contains only committed files, so an uncommitted or untracked source file or header can make a local build pass that would fail for everyone else. Commit it or delete it first.
3. **Verify locally.** Run `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`; all must exit 0. Stop at the first non-zero exit. After a failed link, `verify.py` can still pass against the previous `main.dol` left in `build/`. After resolving a conflict in `configure.py`, `splits.txt`, `symbols.txt`, or a shared header, delete `build/` first for a clean build. Record the verified commit with `git rev-parse HEAD`.
4. **Push.** If the rebase changed anything, run `git push --force-with-lease`.
5. **Check that `main` has not moved.** Run `git fetch origin` then `git merge-base --is-ancestor origin/main HEAD`. If it exits non-zero, another pull request merged after your verification; go back to step 1. Each iteration is a rebase and an incremental build, which usually takes seconds.
6. **Merge exactly what you verified.** Run `gh pr merge <pr> --squash --match-head-commit <verified sha> --subject "<pull request title> (#<pr>)" --body ""`. `--match-head-commit` makes GitHub refuse if the pull request head is not the commit you verified. The empty body keeps attribution lines out of the merge commit. If GitHub reports the pull request is not mergeable or out of date, go back to step 1.
7. **Confirm.** `gh pr view <pr> --json state -q .state` must print `MERGED`. If the description said `Closes #<n>`, confirm the issue closed. If it said `Part of #<n>`, keep the claim while you continue, or release it.

If your local tools are unavailable, you cannot verify, so do not merge; leave the pull request ready for a maintainer and tell the user.

### Resolving rebase conflicts

When `git rebase` stops, list the conflicted files with `git status`, resolve each one, `git add` it, and run `git rebase --continue`; repeat until the rebase finishes. If you get lost, `git rebase --abort` returns the branch to its state before the rebase, and you can start again. Resolve by file type:

- **Files only you changed for this issue,** such as your source and private headers: resolve freely.
- **`config/wpse-v1/splits.txt` and `config/wpse-v1/symbols.txt`:** take `main`'s version with `git checkout --ours <file>` (during a rebase, "ours" is `main` and "theirs" is your commit), then re-apply only your own unit's ranges and renames. The rebuild may rewrite these files; keep the rewrite only where it touches your ranges.
- **`configure.py`:** keep every library and object line from `main`. Add your object lines in address order, and reuse an existing library entry rather than creating a second one.
- **Append-style shared files:** ledger files in `docs/ledger/` and `docs/re/SYMBOLS.md`. `.gitattributes` gives these git's union merge, so concurrent appends usually rebase with no conflict. Check afterwards that no row was duplicated or interleaved into another row. If a conflict still appears, keep both sides: their rows and yours, ordered by address where the file is ordered, with no duplicates.
- **Shared headers in `include/`** used by other units: if both sides only added declarations, keep both. If both changed the same declaration differently, stop and leave it for a maintainer.

Never resolve a shared file by taking one side wholesale, which silently drops another contributor's work. After resolving, always rebuild from clean and run verification before pushing. A conflict-free textual merge can still break the link.

Never merge without a passing local verification of the exact head you merge, run after your last rebase onto `origin/main`. Never use `--admin` or any other bypass. Never merge someone else's pull request. Leave the pull request for a maintainer and tell the user instead if any of these hold:
- it changes a maintainer-only file listed in `docs/COLLABORATION.md`
- a conflict falls outside the cases above, or a shared header declaration conflicts
- the full build or verification fails after resolving and you cannot find the cause in your own changes
- the user asked to review before merging

## 8. Refresh progress images and clean up

Right after the merge, run `python tools/progress/refresh_images.py` from the worktree, before removing it. It renders the README progress images from the build you just verified and pushes a `DOCS: refresh progress images` commit to `main` only if `origin/main` still has exactly your verified files. If `main` has moved, or the push races another merge, it prints that and exits 0; the next merge refreshes them. It never touches any checkout. Never commit `docs/images/` any other way.

Then run these from the main checkout, not the worktree:

```powershell
python tools/collab/claim_issue.py remove-worktree --issue <n>
git push origin --delete <branch>
git branch -D <branch>
```

`remove-worktree` removes the `tools\local` and `.venv` junctions before the worktree, so the shared tools survive. Squash merges are not ancestors of the branch, so `git branch -d` refuses to delete it; use `-D` only after confirming `MERGED`. Then run `git pull --ff-only` in the main checkout so it includes your merged work, with the same rules as in step 1: fast-forward only, and never stash, reset, or commit there. If it fails, tell the user. Other agents and the maintainer may have work in progress in the main checkout; never change it in any other way. Tell the user the pull request link, the merge commit, and what remains of the issue.

To abandon the issue instead, run `python tools/collab/claim_issue.py release --issue <n> --reason "<why>"`, then clean up the same way and close the pull request if one is open.
