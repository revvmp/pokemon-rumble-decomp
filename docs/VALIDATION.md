# Initialization validation

Validation ran with Python 3.14.0 on Windows. These results concern repository tooling only. They provide no evidence of an original game hash or reconstructed game match.

## Completed checks

- Parsed `tools/verify/verify.py` and checked that it contains neither comment tokens nor docstrings.
- Parsed both JSON configuration files.
- Checked relative Markdown links resolve to existing files.
- Confirmed `.gitignore` begins with all original content, including its headings and original game filename exclusion.
- Exercised CLI help and confirmed the unconfigured target template fails with exit code `1`.
- Ran the actual fingerprint and verify CLI commands against temporary synthetic artifacts, then removed the temporary directory.
- Confirmed initial fingerprinting succeeds and a second fingerprint attempt refuses to overwrite the recorded values.
- Confirmed matching artifacts succeed and produce a passing JSON report.
- Changed the last byte of a 2,097,408-byte sample and confirmed verification fails with the exact differing offset, beyond the first comparison chunk.
- Confirmed missing output fails.
- Changed both original and output to identical new bytes and confirmed the frozen reference manifest still rejects them.
- Confirmed overlapping reference and build roots fail.
- Checked Git diff whitespace.

The synthetic input was `bytes(range(256))` repeated 8,193 times, stored as separate original and rebuilt files under temporary roots. This was a one-time command-line integration check; no unit tests or test infrastructure were added.

## Remaining validation

The documented Python 3.10 minimum has not been exercised on a separate interpreter. No real game extraction, compilation, linking, runtime check, executable match, or package match has run. Establish those results from the identified original and real reconstruction before changing their progress status.

## Progress image validation

Ran with Python 3.14.2 on Windows. These checks concern repository tooling only.

- Rendered the committed images with the real CLI from `config/progress.json` without a report: exit `0`. Rendered again to a separate directory and confirmed byte-identical output.
- Rendered all three images from a real objdiff `report.json` produced by Petari's public CI (artifact `RMGK01_report`, created 2026-10-15) to confirm the schema, including 64-bit counts serialized as strings and 2,205 units. Output stayed in temporary storage.
- Rendered a temporary copy of that report with match percentages deterministically varied across all seven bins, then viewed screenshots in headless Microsoft Edge in light and dark color schemes to check shading, legends, labels, and overflow.
- Confirmed rejection with exit `1` and no output for two active phases, out-of-order phase states, a report with zero total code, a file lacking report structure, and a missing report. Confirmed missing required arguments exit `2`.
- Confirmed `tools/progress/progress.py` contains no comment tokens or docstrings, and `config/progress.json` parses.

No external or modified report is committed, and no game progress has been measured.

## Recursive extraction and incoming-commit integration

The real `tools/reference/extract_reference.py` workflow and a fresh repeat both exited 0. All 3,420 file outputs compared byte-for-byte; F-0005 and [the extraction guide](re/EXTRACTION.md) record content, archive, wrapper, and native-header checks. Existing-output rejection exited 1 before writes. Extractor syntax and the absence of comments/docstrings were checked without unit tests.

Pulled origin/main commit `40483bd` with `git pull --ff-only origin main` after saving tracked local edits in a named stash. Restored the edits, reconciled five overlapping documentation files, retained the incoming roadmap/progress/prior-art features, and left local changes unstaged. The safety stash named `local extraction work before 40483bd` remains available. No unmerged paths or conflict markers remain; `git diff --check` passed. No commit or push was performed.

Ran `python tools/progress/progress.py render --phases config/progress.json --output-dir docs/images` successfully. The resulting images agree with the incoming images after Windows newline handling: Phase 1 remains active and code progress remains unmeasured. No objdiff report was provided or fabricated.

## Build pipeline validation

Ran on Windows with Python 3.14.2, dtk 1.8.4 x86, objdiff-cli 3.8.2, ninja 1.13.0, and CodeWarrior Wii/1.1 linker. Evidence is in [F-0010](re/FINDINGS.md#f-0010-split-build-reproduces-maindol).

- `python configure.py` exit 0. `ninja` with the exception runtime split exit 0, including dtk's `build.sha1` check.
- `tools/verify/verify.py fingerprint` sealed `config/targets/wpse-v1.json` (exit 0), and its digest equals F-0003. `verify` passed (exit 0) with all units split, and again with `Runtime/__init_cpp_exceptions.cpp` linked from source.
- Rejection paths observed during real work: before the runtime split, with 16-byte function alignment, and with dead-stripped table entries, both dtk's check and our verifier failed (exit 1) and reported the first differing offset.
- Two clean builds, each after deleting `build/wpse-v1`, `build.ninja`, and ninja state, produced identical `main.dol`, `main.elf`, and `report.json` SHA-256 values. Both passed verification.
- `tools/vendor/vendor_strip.py` vendored seven dtk-template files. Each output was checked to have the same syntax tree as upstream with docstrings removed, and every Python file under `tools/` contains no comment tokens or docstrings.
- Progress images rendered from the real `build/wpse-v1/report.json` (exit 0) and were inspected in a headless Edge screenshot. `--treemap` without `--report` exits 1 before writing anything.

## Collaboration workflow validation

- Ran the documented worktree procedure for real in a new worktree, `../rumble-worktree-test` on branch `worktree-test`:
  - Created junctions for `tools\local` and `.venv`.
  - Ran `python configure.py` and `ninja`: the build completed, including the `build.sha1` check.
  - Ran `tools/verify/verify.py verify`: `PASS`, exit 0.
- Removed the junctions with `cmd /c rmdir`, then ran `git worktree remove --force` and deleted the branch. Afterward `tools/local/dtk.exe` still hashed to the pinned digest and `.venv` ninja was present.
- Created 36 GitHub issues and nine `area:` and `kind:` labels with a generator script. The script was dry-run first and the output reviewed. Issue #5's rendered body was checked with `gh issue view`.

## Codex skill port validation

The Claude skill inventory contained one skill, `.claude/skills/take-issue/SKILL.md`. Added `.agents/skills/take-issue/SKILL.md` with name and description frontmatter and a relative link to that unchanged shared procedure. Repository placement follows the [official skill documentation](https://developers.openai.com/codex/skills/).

Reviewed the adapter for preservation of the shared claim, isolation, verification, merge, and cleanup workflow. No issue was claimed or GitHub state changed.

Attempted the skill-creator `quick_validate.py` CLI, reference-path check, and `git diff --check`, but shell startup failed with `helper_unknown_error: setup refresh had errors`. A retry failed at the same boundary. These checks and discovery in a fresh Codex session remain unverified. When shell execution is available, run the validator against `.agents/skills/take-issue`, confirm its relative link resolves, and run `git diff --check`. No game build was needed for instruction-only changes. The pre-existing modification to `docs/re/main.elf.i64` was not edited by this session.

## Issue claim tool validation

Ran `tools/collab/claim_issue.py` against the live repository with gh authenticated:
- `list --label "good first issue"` printed the six expected issues, exit 0.
- Dry runs selected the right issues: `claim --next` chose #3, `claim --next --label area:nw4r` chose #23, and `claim --issue 5` chose #5. All exited 0 without changing the tracker.
- These exited 1 with clear messages: claiming non-existent #999, releasing an unclaimed issue, creating a worktree without a claim, and removing a missing worktree.
- `claim` without a target exited 2.

The worktree layout it creates was validated manually in [collaboration workflow validation](#collaboration-workflow-validation). Not yet exercised: the live assign, comment, race check, and release round trip, because it posts on a real issue.

## Continuous integration validation

Validated `.github/workflows/build.yml` and `tools/setup/acquire_tools.py`:
- **Acquisition script, locally.**
  - `--offline` verified the five shared tools and the extracted Wii/1.1 `mwcceppc.exe` and `mwldeppc.exe` digests, exit 0.
  - A run into an empty directory downloaded and verified everything in 12 seconds, exit 0.
  - A toolchain copy with a wrong sjiswrap digest exited 1 with a mismatch message and deleted the file.
- **Clean build, locally.** In a worktree, a clean build with the freshly downloaded tools passed verification. Before `.gitattributes`, it rewrote `config/wpse-v1/symbols.txt` and `splits.txt`; only the line endings changed (CRLF checkout, LF dtk output). After it, no tracked file changed.
- **Passing run.** Run 39014783107 on pull request #132 succeeded on `windows-latest`. Every digest verified, `PASS main.dol` was printed, and no rewrite warnings were raised. The progress steps were skipped because the run was not on `main`.
- **Failing run.** Run 39014932291 on a temporary branch, since deleted, removed `#pragma force_active on` from `src/Runtime/__init_cpp_exceptions.cpp`. The linked `main.dol` failed dtk's `build.sha1` check (`build/wpse-v1/main.dol: FAILED`), the Build step exited 1, Verify was skipped, and the run concluded `failure`.

Not yet exercised: the `main`-only progress image steps, which first run after merge.

## Per-block issue set validation

Validated the replacement of the 36 placeholder issues:
- **Coverage.** The partition script loaded the 13,465 `type:function` entries in `config/wpse-v1/symbols.txt`. It assigned the 13,463 open functions to 507 blocks across 53 regions and failed on any gap or duplicate; neither occurred. The two matched `__init_cpp_exceptions` functions are excluded.
- **Rendering.** Dry runs rendered every body. Issue #37's rendered body, title and labels were checked with `gh issue view`.
- **Posting.** Issues #37–#545 were posted, skipping pull request numbers #132 and #434, with 507 recorded in the resume state. The poster was stopped and restarted once with a shorter delay. On restart it rebuilt its state from existing titles. A `gh issue list` check found no duplicate titles after the restart or at the end.
- **Close-out.** The closing script was dry-run and its comments reviewed. It then closed #2–#24, #26–#28 and #30–#34 with links to their replacements, and labelled #463, #472, #488, #495, #534 and #535 `good first issue`. Afterwards `gh issue list` showed 511 open issues: the 507 blocks plus #25, #29, #35 and #36.


## Source-file issue re-cut validation

Validated the re-cut of unclaimed library issues on 2026-10-17, against `main` at `dd18b4f0`:
- **Eligibility.** Of 111 open issues in `area:sdk`, `area:msl-runtime`, `area:hbm`, `area:nw4r` and `area:middleware`, 77 had no assignee and no active claim comment, using the rules of `tools/collab/claim_issue.py`. Their bodies listed 2,135 functions; 187 were in splits linked as `Matching` in `configure.py`. 32 issues whose unmatched functions had no split, reference span or reference link order evidence were left unchanged (#351, #357–#374, #377, #381–#385, #450–#455, #539).
- **Plan and coverage.** The plan was written to a scratch file before posting. A script checked it: all 1,176 unmatched functions of the 45 issues to supersede were placed, none missing, none extra, none twice, and no source file appeared in two new issues.
- **Posting.** 48 issues were created, #756–#812 excluding other contributors' numbers, deduplicated by title against all existing issues and paced at 4 seconds. GitHub's GraphQL limit was exhausted after 26; the rest were created through the REST API, resuming from the saved state.
- **Race.** #380, #401 and #402 were claimed between planning and closing. The closing script re-checked each issue first and skipped them. The three new issues holding only their functions (#771–#773) were closed as duplicates, and #770 was trimmed to its functions from #379.
- **Close-out.** 42 issues were closed as not planned, each with a comment naming its replacements and the count of its already-matched functions. A final check against the closed issues found 1,088 unmatched functions in 45 open new issues: none missing, none extra, none duplicated, none still listed in an open old issue, and no file in more than one new issue.

## Config merge driver validation

Validated `tools/collab/merge_config.py` against the last 60 commits on `main` that touched each config file. Each consecutive pair (A, B) was replayed as parallel work: B was reconstructed as if branched before A, by reverting A's changes out of B, and then merged onto A. The correct result is B itself.

| File | Pairs | Plain `git merge-file` conflicts | Driver conflicts | Driver results differing from B |
| --- | --- | --- | --- | --- |
| `config/wpse-v1/symbols.txt` | 59 | 6 | 0 | 0 |
| `config/wpse-v1/splits.txt` | 59 | 6 | 0 | 0 |
| `configure.py` | 59 | 6 | 0 | 4, each only a reordering of `Object(...)` lines within one library list |

The `configure.py` reorderings do not affect the build: link order comes from dtk's configuration, which is derived from `splits.txt`. Six real `git rebase` runs in temporary worktrees, with the driver registered through `.gitattributes`, completed without stopping. In every one, `symbols.txt` and `splits.txt` matched B exactly. A first attempt showed the driver must be referenced by absolute path, because older trees being rebased do not contain the tool.

Ledger tables (`docs/ledger/*.md`) moved from union merge to the same driver on 2026-10-17, after union merges brought back a superseded `pf_dir.c` row three times. Rows are keyed by source path, or by unit name when no source exists; hunks containing blank lines or unkeyed text fall back to union. Replaying the real case, a branch from `74cd685c` that adds a row next to `pf_dir.c`, merged onto `aacc0169` where #899 rewrote that row: `git merge-file --union` produced two `pf_dir.c` rows, and the driver produced exactly one, `aacc0169`'s, plus the added row, with no other line differing from `aacc0169`. A real `git rebase` in a scratch repository with the driver registered through `.gitattributes` completed with exit 0, the same single row, and a clean status.

## Unit diff and pull request overlap tool validation

Validated on 2026-10-17 in a worktree off `main` at `fb63a246`, after one full build (`ninja` exit 0, 128 s):
- **`tools/re/unitdiff.py`, matching unit.** `BController` reported `.text` 984/984 at 100% and 19/19 functions at 100% (848/848 code bytes); dtk `gap_` padding symbols are excluded. `game/BController.cpp` and `main/game/BController` resolved to the same unit.
- **Non-matching unit.** `src/TMC_JPEG/tmcjpeg_stream.c` reported `.text` 98.82%, 9/10 functions at 100%, and `TMCJPEGStreamPutBits` at 95.69% with its first six differing rows, starting with the original's `li r11, 0x1` against the rebuilt `lis r11, sBitMask@ha`. `--function TMCJPEGStreamPutBits` printed all rows. `--json` wrote objdiff-cli's JSON unchanged, with `left` and `right` keys.
- **Partial unit.** `CDlgPiiList` listed its 8 diffed functions, summarized 122 target functions not yet in source (30,100 bytes), and listed source-only functions.
- **Errors.** An unknown unit and an unknown `--function` each exited 1 with a message.
- **Timings,** wall clock including Python start-up: after touching the source, 0.77–0.81 s for `BController`, 0.71 s for `tmcjpeg_stream`, and 0.80–1.08 s for `CDlgPiiList`; with the object up to date, 0.23–0.57 s.
- **`claim_issue.py overlap`.** Against 28 open pull requests, `--paths src/RVL_SDK/fa/pf_system.c configure.py --strict` ignored the `decomp-config` file `configure.py`, listed #895 and #915 as sharing `pf_system.c`, and exited 1; without `--strict` it exited 0. `--issue 326` listed #946, #932, #941 and #890 by issue and neighbor references. A branch with no files shared with open pull requests reported none, exit 0. A full run fetching every pull request's files took 4.8 s.
- **`claim` warning.** `claim --issue 359 --dry-run` listed #947 and #935 as referencing neighbor #360 before printing `Would claim`; `claim --issue 343 --dry-run` listed #940 (#344). Dry runs change nothing.
- **`tools/collab/stale.py`.** Listed 28 open pull requests with 18 flagged (idle drafts, ready pull requests unmerged for over 30 minutes, docs-only drafts) and two overlapping pairs, #887 and #895 (51 files) and #895 and #915 (6 files), in 4.7 s. It makes only REST GET requests.

## Incremental and seeded build validation

Validated on 2026-10-17 in scratch worktrees with ninja 1.13.0, timed with `Measure-Command` around `python configure.py` and `ninja`. The machine was shared with other agents' builds, so times vary by about a factor of two.

- **Cold builds.** A fresh worktree at `6201b187` built in 126 s; another at `fb63a246` built in 159 s. Both passed verification and left `git status` clean: dtk no longer rewrites `symbols.txt` on a fresh build, so there is no normalized form to commit.
- **Incremental across config changes.** The `6201b187` build was moved forward twelve commits to `fb63a246`, a range changing `splits.txt` (30 lines), `symbols.txt` (220 lines) and `configure.py` (11 lines). After deleting `build/wpse-v1/main.dol`, `configure.py` and `ninja` took 47 s: `ninja` reran the split, regenerated `build.ninja`, and rebuilt 79 steps. Verification passed, and `main.dol` (`ad1a5989…53da`), `main.elf` (`21cc752e…a727`) and `report.json` (`d4883dcc…1614`) were byte-identical to the clean build at `fb63a246`. Deleting `build/` after config merges is unnecessary.
- **Seed store.** In the clean `fb63a246` worktree, `build_seed.py store` first refused with exit 1 because `build/wpse-v1/seed-commit` did not name `HEAD`. After `build_seed.py record` it copied `build/` (18,438 files, 93 MB) and the ninja state into `..\rumble-build-seed` in 5.6 s.
- **Seeded worktree, forward.** A new worktree at `origin/main` `82571e1d`, one commit later, was seeded by `build_seed.py apply` in 7.4 s. `configure.py` and `ninja` then took 3.1 s (report and progress only), verification passed, and `main.dol` and `main.elf` matched the clean build. Touching `include/game/IController.h` made `ninja -n` schedule `BController.o`, and `ninja -t deps` listed headers under the new worktree, confirming the `.ninja_deps` path rewrite.
- **Seeded worktree, across config changes.** The same seed applied to a new worktree at `6201b187` touched 44 changed files. `ninja` reran the split and `configure.py`, compiled 77 steps, and finished in 91 s against 126 s cold; verification passed with the frozen `main.dol` hash.
- **Failure modes.** `apply` with an empty seed directory, and with a seed whose `.ninja_deps` was corrupt, printed `Build seed not used (...); the first build will be a full build.`, exited 1, and left no `build/`, `.ninja_log`, or `.ninja_deps`. In a seeded worktree, a source change that broke compilation made `ninja` exit 2; with `main.dol` deleted first, verification failed with exit 1 on the missing file instead of passing on a stale DOL.

## Landing tool validation

Validated `tools/collab/land.py` on 2026-10-17 in a scratch worktree `..\rumble-99999` on branch `maint/land-validation`, created from `origin/main~3` with one scratch commit adding a file under `docs/`, and seeded with `build_seed.py apply`. No real issue pull request was touched; `--pr 950`, an already merged pull request, stood in for read-only lookups.

| Command | Exit | Time | Result |
| --- | --- | --- | --- |
| `land.py --issue 99999 --pr 950 --dry-run` | 0 | 137 s | Round 1 rebased onto `dfd85b51`, built (ninja 62 s), verified, stored a seed, and found `main` had moved; round 2 rebased onto `b4205e4a`, built (43 s), verified, and printed the push and squash merge it would make. `refresh_images.py --dry-run` skipped because the branch differs from `main`. |
| same, with an untracked file | 1 | | Stopped before building: uncommitted or untracked files. |
| same, after committing a source change that does not compile | 1 | 2.1 s | `ninja` exited 2; `land.py` printed the log tail and did not run verification. `main.dol` had been deleted first, so no stale DOL remained. |
| same, on a branch from `origin/main~6` editing a line `main` later changed | 1 | | Rebase stopped; `land.py` listed `docs/SETUP.md` and left the rebase in progress for `git rebase --abort`. |
| `land.py --issue 99999 --pr 950 --cleanup`, run inside the worktree | 1 | | Refused, naming the main checkout command. |
| `land.py --issue 99999` (no dry run) | 1 | 60 s | Rebased onto `2402fd31`, built (41.5 s), verified, really pushed `f78bb9b8` to `origin/maint/land-validation` (`git ls-remote` confirmed), then stopped: no open pull request for the branch. |
| `land.py --issue 99999 --pr 950 --dry-run --cleanup`, from the main checkout | 0 | 56 s | Resolved `..\rumble-99999` from the issue number, ran one round (ninja 38 s), and printed the four cleanup commands. |
| simulated docs-only move of `main` (a script wrapping `rebase` and `contains_main`) | 0 | | Round 2 printed `main moved only in docs/; reusing the build verified at 22661eab` and re-ran only `verify.py`. |
| `land.py --issue 99999 --pr 950 --cleanup-only`, from the main checkout | 0 | | `remove-worktree`, remote branch deletion, `git branch -D`, and `git pull --ff-only` each exited 0; the worktree and remote branch were gone and `tools\local\dtk.exe` survived. |

The REST pull request lookup by branch was checked read-only against an open pull request: `find_pr` returned #953 with `draft` true for its branch. The merge call itself, `gh pr ready`, and the image push were not exercised, because no throwaway pull request was available; they use the same REST merge request (`sha` pinned to the verified commit) as the manual fallback in the `take-issue` skill. `refresh_images.py --min-interval` was checked with `--dry-run`: with the last image commit 15 minutes old it printed that it was skipping and exited 0; with `--min-interval 0` and `--min-interval 10` it rendered and reported the images current.

Before these changes, the skill told agents to delete `build/` whenever a config file merged during a rebase; with 93 of 143 recent `main` commits touching one, most rounds were cold builds of 2 to 9 minutes. Each round now costs an incremental build of about 40 to 60 s, dominated by the link, and a new worktree's first build drops from a cold build to the changes since the newest seed (3 s when only headers and docs changed).
## Native source-fragment build validation, 2026-11-04

The ScriptWorldObj partial_sources pipeline was validated with two empty output roots, tools/local/session/clean-20261104 and tools/local/session/clean-repeat-20261104. Each used python configure.py --build-dir followed by the normal ninja entry point. Both ninja runs exited zero; the frozen wpse-v1 verifier passed independently for each. Their main.dol, main.elf and report.json are byte-identical to each other and to the incremental build.

DOL SHA-256: ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da. ELF SHA-256: 8153135205631f75639734ccb02e4f0df68e09afe9e4a762397df0868b7909bd. Report SHA-256: def1b9e61fbd575f489eb7e374cd06e30c878bb63437e8ebf87d5ecf5ecd019d.

The normal default build configuration was restored afterward. This verifies the build mechanism and accepted source-link units;71 unmatched real functions and remaining fallback units still block complete source reconstruction. Original manifests were unchanged.

## Broad-header regression check, 58-function checkpoint

Compared the latest real ninja report with the prior clean-repeat report by original virtual function address, excluding gap and padding names. Both contain13,448 address-bearing real function rows. No previously100% function regressed. The latest fresh build and frozen verifier pass; the remaining real tail is58. This check covers the scoped header changes, signature corrections and added source emission controls retained in this checkpoint.

## Typed data and 53-function checkpoint

The ordinary build links BModelObj, CMiniMap, CPauseMenu and ten additional typed data units. Ninja exits0 and frozen verification passes. The report records3,001,272 matched code bytes,2,552,708 linked code bytes and1,613,548 linked data bytes. The53-function tail and explicit original fallbacks remain blockers to complete source reconstruction. No original hash changed.
