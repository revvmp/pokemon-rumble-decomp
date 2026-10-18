# Collaboration

This guide lets many people and LLM agents work on the decompilation at once without conflicting. It extends the [project rules](PROJECT_RULES.md); where the two differ for multi-contributor work, this guide and [D-0011](DECISIONS.md#d-0011-multi-contributor-workflow) apply.

## How work is divided

Work is tracked as GitHub issues. Each decomp issue owns a block of about 25 functions in address order, and its body lists every one of them; every function belongs to exactly one issue. [Workstreams](WORKSTREAMS.md) lists them with their regions. Labels give the area and the kind of work:

| Label | Meaning |
| --- | --- |
| `area:sdk` | RVL_SDK libraries, written in C, compiled with Wii/1.0 at default alignment |
| `area:msl-runtime` | MSL_C, CodeWarrior runtime, and MetroTRK, compiled with Wii/1.1 and `-func_align 4` |
| `area:nw4r` | NintendoWare for Revolution, C++, compiler Wii/1.0 or 1.1 unresolved |
| `area:hbm` | Home Button Menu and its private layout library copy |
| `area:middleware` | Other middleware without reference source, such as RevoEX, NCD, and TMC JPEG |
| `area:game` | Game code, C++ |
| `area:infra` | Build, CI, and tooling |
| `kind:decomp` | Split translation units and reconstruct matching source |
| `kind:research` | Produce evidence, names, or boundaries that unblock decomp issues |
| `good first issue` | Small, well-bounded regions with reference source |

Compilers and evidence come from [F-0007](re/FINDINGS.md#f-0007-per-library-compiler-identification), and regions from the executable survey in [F-0012](re/FINDINGS.md#f-0012-full-executable-layout-survey). Block boundaries are estimates, not final unit boundaries. Establishing exact boundaries is part of each issue; when a unit crosses into a neighboring block, coordinate on that issue.

A source file can span several block issues. The issue that owns the file's first function owns the file's split, its `configure.py` object line, and its header. Other block owners add their function bodies to the same source file in address order. Changes to the class declaration go through the owning issue or a small header pull request that merges first.

## Claiming work

Use `tools/collab/claim_issue.py` rather than assigning by hand. It only takes open issues with no assignee and no other active claim comment, re-checks after assigning to detect a simultaneous claim, and posts the claim comment for you.

```powershell
python tools/collab/claim_issue.py list --label "good first issue"
python tools/collab/claim_issue.py claim --next --agent "<agent or person>"
python tools/collab/claim_issue.py claim --issue <n> --agent "<agent or person>"
python tools/collab/claim_issue.py release --issue <n> --reason "<why>" [--blocked]
python tools/collab/claim_issue.py overlap [--base origin/main] [--paths <file> ...] [--issue <n>] [--strict]
python tools/collab/stale.py [--idle 60] [--ready 30]
```

`claim` first lists open pull requests that reference the issue or its neighbors (n-1, n+1), or that mention file or class names from its title. Run `overlap` from your worktree before opening a pull request: it lists other open pull requests whose changed files intersect `git diff --name-only <base>...HEAD`, ignoring files merged automatically by `union` or `decomp-config` unless `--all-files` is given. It warns and exits 0; `--strict` exits 1 on any overlap. `stale.py` is read-only: it lists open pull requests with their idle time, linked issue, and whether they change `src/` or `include/`; flags idle drafts, ready pull requests left unmerged, and docs-only drafts; and lists pairs of open pull requests that change the same files. All three use only the REST API.

Claiming rules:

1. Only open issues with no assignee can be claimed. Claiming assigns you and posts a `Claiming` comment naming the agent session. LLM agents claim through the person who runs them.
2. Each issue has exactly one owner. To let others help with a large library issue, the owner splits it into sub-issues, one per source file or group of files. Each sub-issue is titled after the file, labelled like the parent plus `kind:decomp`, and its body starts with `Part of #<parent>`. Unassigned sub-issues are then claimable like any other.
3. Keep one active claim per contributor until its pull request is open.
4. A claim expires after 14 days without a linked pull request or progress comment. A maintainer may then reassign it.
5. Research issues produce new decomp issues. Open them with the `kind:decomp` label and link them from the research issue.

## Unit workflow

Do these steps in order for each translation unit.

1. **Prepare.** Read root `AGENTS.md`, your issue, F-0007, the relevant [ledger](ledger/README.md), and any findings the issue cites.
2. **Split.** Add the unit to `config/wpse-v1/splits.txt` with every section range it owns (`.text`, `.rodata`, `.data`, `.sdata`, `.sdata2`, `.bss`, `.sbss`, `.ctors`, `extab`, `extabindex`). Build and confirm verification still passes; a correct split never changes output bytes.
3. **Register.** Add `Object(NonMatching, "<path>")` to the matching library in `configure.py`, creating the library entry with the F-0007 compiler and flags if it is missing. Keep libraries and objects in address order.
4. **Reconstruct.** Write `src/<library>/<path>` and headers under `include/<library>/`. Follow the source fidelity standard in `AGENTS.md`: C for RVL_SDK and MSL, C++ elsewhere, original names, no comments.
5. **Diff.** Use objdiff. In the GUI, open the repository root; `objdiff.json` is generated by `configure.py`. The CLI report is `build/wpse-v1/report.json`. For a quick inner loop, `python tools/re/unitdiff.py <unit or source path>` builds only that unit's object and prints per-function match percentages with the first differing instructions in about a second; `--function <name>` prints one function's full diff. It does not link, so it never replaces the full build for split and `Matching` checks.
6. **Accept.** When every function matches, change the object to `Matching`, run a full build, and run verification. Only a passing verification makes the unit done; objdiff alone is not enough.
7. **Record.** Add or update your unit's row in the area ledger. Record recovered names and layouts in [symbols](re/SYMBOLS.md) and new evidence in a findings file.
8. **Submit.** Open a pull request using the template. One issue per pull request, ideally one to a few units.
9. **Merge.** Rebase onto `origin/main`, commit everything, and verify locally: `configure.py`, `ninja`, and `verify.py` all exit 0. If `main` has not moved since, squash-merge with `--match-head-commit` set to the verified commit and an empty merge body; you do not wait for GitHub CI. If `main` moved, repeat. Do not merge if the pull request's CI check has failed or the latest `main` run is red. Then remove your worktree and branch, as in the `take-issue` skill. Resolve conflicts in shared files by keeping both sides. Leave pull requests that touch maintainer-only files, or whose conflicts the skill's rules do not cover, for a maintainer.

Partially matching work is welcome. Keep it `NonMatching` so the build still uses the original object, and record the remaining differences, suspected causes, and decomp.me scratch links in the ledger row.

## Shared files

| File | Rule |
| --- | --- |
| `config/wpse-v1/splits.txt` | Edit only ranges inside your claimed region. dtk rewrites this file during builds; commit only your own hunks and discard unrelated rewrites. |
| Merging config files | `splits.txt`, `symbols.txt` and `configure.py` use the `decomp-config` merge driver (`tools/collab/merge_config.py`, registered by `claim_issue.py worktree`), which merges independent entries automatically during a rebase. |
| `config/wpse-v1/symbols.txt` | Rename or adjust only symbols inside your region. Discard unrelated dtk churn. |
| `configure.py` | Add or change only the object lines for your units, plus a library entry if missing. |
| `include/` shared headers | RVL_SDK headers live in `include/revolution/` and `include/private/`, imported from Petari ([D-0013](DECISIONS.md#d-0013-shared-rvl_sdk-headers-come-from-petari)). Look there first and extend the existing header in place; never add SDK headers under `include/RVL_SDK/`, which holds only forwarders. Changes to types used outside your area go in a separate small pull request that merges first. |
| `config/wpse-v1/symbols.txt` names outside your block | You may rename symbols your units reference, such as callees or data in other blocks, when evidence supports the name, because linking from source needs them. Do not split, resize, or re-type them; that belongs to the owning issue. |
| `docs/ledger/<area>.md` | Add rows for your own units only. |
| New findings | One new file per finding in `docs/re/findings/`, named `<issue>-<slug>.md`, with ID `F-<issue>-<n>`. Do not append to `docs/re/FINDINGS.md`. |
| `AGENTS.md` traps | Maintainer-only. Record traps under a `Traps` heading in your issue's findings file in `docs/re/findings/`; maintainers promote project-wide ones into `AGENTS.md`. |
| `docs/ledger/*.md`, `docs/re/SYMBOLS.md` | Append your rows. `.gitattributes` merges these with git's union driver, so concurrent appends rebase without conflicts. Never rewrite or reorder other contributors' rows. |
| Maintainer-only | `docs/re/main.elf.i64` (binary, unmergeable), `config/progress.json`, `docs/images/`, the status table in `PROGRESS.md`, `HANDOFF.md`, `WORKSTREAMS.md`, the `AGENTS.md` state table, `config/targets/`, `config/wpse-v1/build.sha1`, `config/toolchain.json`, and vendored `tools/build/`. |

After each merge, the merging agent runs `tools/progress/refresh_images.py`, which commits refreshed progress images to `main` only if `main` still matches its verified build. Maintainers update the shared summaries. Contributors never edit either, so they never conflict on them.

## Definition of done

A pull request is ready when all of these hold:

- `python configure.py` and `ninja` succeed from a clean checkout of the branch.
- Local verification passes on the exact commit being merged, after rebasing onto the latest `main`. GitHub CI does not run on pull requests or pushes, so this local verification is the only gate ([continuous integration](SETUP.md#continuous-integration)).
- `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exits 0.
- Every unit marked `Matching` shows 100% in objdiff, and the full DOL still verifies.
- No comments, docstrings, unit tests, or raw decompiler output were added.
- Ledger rows, symbol records, and any findings are updated.
- The description lists the commands run with their results.

## Working with LLM agents

Agents follow the same rules as people. These additions keep parallel agents from interfering:

- **One worktree per agent.** The build writes `build.ninja` and `build/` at the repository root, so two builds in one checkout clobber each other. Give every agent its own worktree and branch:

  ```powershell
  git worktree add ..\rumble-<issue> -b <issue>-<slug>
  New-Item -ItemType Junction -Path ..\rumble-<issue>\tools\local -Target (Resolve-Path tools\local)
  New-Item -ItemType Junction -Path ..\rumble-<issue>\.venv -Target (Resolve-Path .venv)
  ```

  `orig/` is tracked, so worktrees include the originals. The junctions share the pinned binaries without copying them. `claim_issue.py worktree` does all of this for you.
- **Seeded builds.** A cold build compiles every unit and takes minutes. `claim_issue.py worktree` copies the newest verified build from `..\rumble-build-seed` into the new worktree with `tools/collab/build_seed.py apply`. It rewrites the absolute header paths in `.ninja_deps` to the new worktree, backdates tracked files, and touches the files that differ from the seed's commit, so `ninja` rebuilds exactly what changed. Seeds are written only by `build_seed.py store`, which refuses unless `build/wpse-v1/seed-commit` names `HEAD`, the tree is clean, and `ninja -n` has nothing to build; `build_seed.py record` writes that file after a passing verification. The three newest seeds are kept. If no seed is usable, the worktree gets no `build/` and the first build is a full build. Never store a seed from the main checkout while another agent may be building there.
- **Do not wipe `build/`.** `ninja` regenerates `build.ninja` and reruns the split when `configure.py`, `splits.txt`, or `symbols.txt` change, so incremental builds after a rebase are exact. Delete only `build/wpse-v1/main.dol` before building, so a failed link cannot leave an old DOL for `verify.py`.
- **Keep the main checkout current.** The skill, `tools/collab/claim_issue.py`, and the docs are read from the main checkout, so agents run `git pull --ff-only origin main` there before claiming and after merging. They never stash, reset, or commit in it; if the fast-forward fails, they report it and continue.
- **Remove junctions before removing a worktree.** A recursive delete can follow a junction and erase the shared `tools/local` and `.venv`. Remove the junctions first, then the worktree:

  ```powershell
  cmd /c rmdir ..\rumble-<issue>\tools\local
  cmd /c rmdir ..\rumble-<issue>\.venv
  git worktree remove --force ..\rumble-<issue>
  ```
- **Brief.** Give an agent the issue number and tell it to start with root `AGENTS.md`, this guide, and the issue. The issue body states the region, compiler, reference source, and done criteria.
- **Evidence over guesses.** Agents must not invent names, flags, or compiler versions. Unproven names stay in the ledger with a confidence note until evidence supports them.
- **No co-authors.** Commits and pull requests must not list co-authors. Agents must not add `Co-Authored-By:` trailers, "Generated with" lines, or any other attribution to commit messages, pull request titles, or descriptions, even if their tool does so by default.
- **Hand back through the pull request.** The pull request description is the agent's report: units changed, match status per function, commands run with exit codes, and remaining differences.
- **Reference source.** Petari is CC0. Check the license of any other source before reuse, and record provenance in the ledger row.
