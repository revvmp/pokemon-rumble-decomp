# Progress

## Current status

Roadmap Phases 1 to 3 are complete and Phase 4 is active. `python configure.py` and `ninja` rebuild `main.dol` byte for byte from dtk split objects plus one reconstructed source unit, and `tools/verify/verify.py` accepts it against the frozen manifest `config/targets/wpse-v1.json`. Code progress is measured against the objdiff denominator documented in [matching](MATCHING.md#counting-method): 112 of 3,098,940 code bytes and 2 of 13,465 functions are linked from source, excluding alignment padding entries (F-0011).

| Milestone | State | Evidence or prerequisite |
| --- | --- | --- |
| Repository architecture and workflow | Initialized | `ARCHITECTURE.md`, `PROJECT_RULES.md` |
| Artifact verifier | Implemented | `tools/verify/verify.py` |
| Release identification | Identified | WPSE, USA, TMD version 1; F-0001; retail revision label and acquisition provenance unverified |
| Frozen reference manifests | Frozen | Package `config/targets/wpse-v1-package.json`; executable `config/targets/wpse-v1.json` (`main.dol`) |
| Artifact and section inventory | Complete for current scope | F-0005 extraction; both executables' sections in `re/INVENTORY.md`; boot loader role F-0009 |
| Toolchain identification | Identified per library | RVL_SDK Wii/1.0; MSL and Runtime Wii/1.1 or 1.3 with `-func_align 4`; NW4R Wii/1.0 or 1.1; F-0007 |
| Reproducible game build | Achieved | Two clean builds byte-identical; F-0010 |
| Exact executable match | Achieved for `main.dol` | Verified; nearly all code still comes from original split objects |
| Source reconstruction | Started | 1 of 8,782 units; see ledger |
| Full release match | Pending | Boot loader, data contents, and WAD packaging outside current scope |

## Progress images

The README shows images rendered by `tools/progress/progress.py`, the project's own visualizer. It replaces hosted decomp.dev badges, which the project owner reports are being sunset ([D-0008](DECISIONS.md#d-0008-generate-progress-images-in-repository)). Regenerate and commit the images whenever phase state or measured progress changes:

```powershell
python tools/progress/progress.py render --phases config/progress.json --output-dir docs/images
```

| Image | Source | Shows |
| --- | --- | --- |
| `docs/images/progress-roadmap.svg` | `config/progress.json` | Each [roadmap](ROADMAP.md) phase as done, in progress, or not started |
| `docs/images/progress-code.svg` | objdiff report, when given | Matched code, code linked from source, matched functions, matched data, and per-category code; without a report it states that progress is not measured |
| `docs/images/progress-treemap.svg` | objdiff report, when given | Translation units sized by code bytes and shaded by match percentage in seven bins |

Render with the real report after a successful build:

```powershell
python tools/progress/progress.py render --phases config/progress.json --report build/wpse-v1/report.json --output-dir docs/images
```

Phase states in `config/progress.json` must agree with the milestone table above and the roadmap gates. States must run done, then at most one active, then pending; the tool rejects anything else. The tool refuses a report with zero total code. Per-category rows appear only when progress categories cover all code; today almost every unit is an uncategorized dtk placeholder. The treemap is opt-in with `--treemap` and stays out of the README until real translation-unit splits replace the 8,781 per-function placeholders, which would otherwise render as noise. "Matched" means source compiles to identical code; "linked from source" means the final build uses that source rather than an original fallback object. Only the linked figure approaches source coverage, and it still requires the ledger review described in [matching](MATCHING.md).

Images use a single blue sequential ramp with separate light and dark steps selected through `prefers-color-scheme`, embed hover titles that work when an SVG is opened directly, and contain no timestamps, so identical inputs produce identical bytes.

## Reconstruction ledger

The ledger is kept per area in [ledger](ledger/README.md). It has one linked unit: `Runtime/__init_cpp_exceptions.cpp` in [msl-runtime](ledger/msl-runtime.md). All other code comes from original split objects and counts as fallback.

## Latest session

Replaced the 36 placeholder issues with 507 per-block decomp issues that together list every function in the executable:
- Dumped all functions with their string and call references from a copy of the IDA database. Four parallel surveys then mapped `.text` 0x80052500–0x80292000 into source files and libraries (F-0012).
- The game address range turned out to contain HBM, RevoEX, NCD, CNT, TMC JPEG, FA, MSL printf, and NW4R ef, g3d and snd. Library and game regions were partitioned into blocks of 6–77 functions, mean 26.6, cut at file starts where the survey evidenced them.
- Each issue body lists every function it owns, its classes or reference files, compiler, and done criteria. Superseded issues were closed with links to their replacements. Research issues #25, #29, #35 and #36 remain. Issue #1 was then completed: CI builds and verifies every push and pull request (see [continuous integration](SETUP.md#continuous-integration)).
- Issue #16, the example run, was released unfinished.

New trap: library code inside the game address range. Validation is in [validation](VALIDATION.md#per-block-issue-set-validation).

## Earlier session: take-issue skill

Added the `take-issue` skill (`.claude/skills/take-issue/SKILL.md`) and `tools/collab/claim_issue.py`, which let any contributor tell an LLM to claim an unassigned issue and work it in an isolated worktree. Read-only and rejection paths are validated; see [validation](VALIDATION.md#issue-claim-tool-validation). No reverse engineering findings.

## Earlier session: parallel work setup

Set up parallel work under D-0011:
- Wrote [collaboration](COLLABORATION.md), which covers claims, the unit workflow, shared-file rules, the definition of done, and per-agent worktrees.
- Split the reconstruction ledger into per-area files under `docs/ledger/`, and added a per-issue findings directory and GitHub issue and pull request templates.
- Created 36 GitHub issues with area and kind labels, indexed in [workstreams](WORKSTREAMS.md). Library issues carry anchor ranges and matched reference files from F-0007.
- Validated the worktree procedure end to end.

No new reverse engineering findings. New trap: junction-aware worktree removal.

## Earlier session: Phases 1 to 3

Completed roadmap Phases 1 to 3 and started Phase 4.

- Acquired and pinned dtk 1.8.4, objdiff-cli 3.8.2, binutils 2.42-2, the CodeWarrior compiler bundle `20251118`, sjiswrap 1.2.2, and ninja 1.13.0. This resolved the earlier dtk provenance question: the user-supplied binary is the official x86 asset (F-0006).
- Froze the `main.dol` acceptance manifest and recorded both executables' sections. A delegated pass identified the boot content as a NAND loader (F-0009).
- Measured `.comment` version 15 (F-0008).
- A second delegated pass identified per-library compilers by masked code search across nine compilers (F-0007). I re-ran its two headline comparisons.
- Vendored dtk-template as comment-free files in `tools/build/` (D-0007 accepted) and reorganized `tools/` by category (D-0009).
- Wrote `configure.py`, added the Wii exception runtime split, and reproduced `main.dol` exactly from two clean builds (F-0010).
- Reconstructed and linked `Runtime/__init_cpp_exceptions.cpp`, and documented the objdiff counting method.
- Rendered progress images from the real report.
- Added the source fidelity standard and the source-language decision (D-0010).

New traps are recorded in `AGENTS.md`.

## Earlier session: reference extraction

Pulled `40483bd` (FEAT: Add progress), preserved local extraction changes, and reconciled overlapping documentation with its roadmap and progress tooling. Completed recursive reference extraction: 156 U8 archives, 134 decompressions, 3,420 inventoried files. A fresh second run produced byte-identical outputs for every file. Original WADs and prior extracted contents remain unchanged. See [extraction](re/EXTRACTION.md) and F-0005 for exact commands, tool pins, and validation limits. Native asset formats remain preserved, not converted to editable exports. New traps: 0x40 assets require LZH8 rather than dtk NLZSS, and backslash-bearing U8 names require parent directories to be created before Windows extraction. No source reconstruction or game match is claimed.

Roadmap Phase 1 remains active: executable acceptance manifests, full section/coverage review, and input provenance still need resolution. Code progress remains unmeasured; no fabricated objdiff report was supplied.

## Earlier session: progress images

Added `tools/progress/progress.py` and `config/progress.json` and embedded the roadmap and code progress images in the README; see [progress images](#progress-images) and [D-0008](DECISIONS.md#d-0008-generate-progress-images-in-repository). The code image states that progress is not measured because no objdiff report or denominator exists. Validation is recorded in [validation](VALIDATION.md#progress-image-validation). No reverse engineering findings were produced. New trap recorded in `AGENTS.md`: progress images are committed outputs that go stale unless regenerated.

## Earlier session: Petari study

Studied the Petari (Super Mario Galaxy) decompilation at commit `e0ab141` and recorded its tooling, repository mechanics, and CodeWarrior matching knowledge in [prior art](re/PRIOR_ART.md), with each practice mapped onto this project's rules. Wrote the phased [roadmap](ROADMAP.md) and proposed [D-0007](DECISIONS.md#d-0007-evaluate-the-dtk-template-ecosystem) to adopt the dtk-template ecosystem once the executable is confirmed. Recorded the first reverse engineering finding, [F-0001](re/FINDINGS.md#f-0001-wad-container-metadata): unencrypted WAD header, ticket, and TMD fields of the local WAD, including title ID `WPSE`, title version 1, seven contents, and boot index 6. No content was decrypted or extracted, no tool was acquired, no manifest was fingerprinted, and no code changed. New traps recorded in `AGENTS.md`: vendored dtk-template tooling conflicts with the no-comments rule, and Petari's source-comment conventions must move to documentation.

## Earlier session: documentation rules

Updated `AGENTS.md` to require persistent traps and findings with evidence links, and seeded it with established initialization lessons. Clarified that end-to-end validation is preferred and end-to-end suites are allowed; unit tests, code comments, and docstrings remain prohibited. No new reverse engineering findings or newly observed tooling traps emerged. This session changed documentation only; no game or tooling behavior changed.

## Initialization evidence

Added project architecture, living documentation, reverse engineering records, and exact artifact verification. No new reverse engineering findings were produced. No game match was attempted because the original and reconstructed artifacts are unavailable. See `HANDOFF.md` for the next action.

CLI integration checks passed for matching samples and rejection of mismatches, missing output, changed references, overlapping roots, and repeat fingerprinting. Syntax, configuration, documentation links, original ignore preservation, and comment-free Python were checked. See [validation](VALIDATION.md) for evidence and limits. No unit tests were added.
