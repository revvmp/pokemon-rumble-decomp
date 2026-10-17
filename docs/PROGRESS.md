# Progress

## Current status

Target `wpse-v1` has identified USA/TMD-version-1 metadata, a frozen package fingerprint, identified main and boot DOLs, and a recursively extracted reference tree. Authenticity and retail revision naming remain unverified. No game toolchain, reconstructed function, or matching game build has been established. Source coverage has no known denominator and is not expressed as a percentage.

| Milestone | State | Evidence or prerequisite |
| --- | --- | --- |
| Repository architecture and workflow | Initialized | `ARCHITECTURE.md`, `PROJECT_RULES.md` |
| Artifact verifier | Implemented | `tools/verify.py` |
| Release identification | Metadata identified | WPSE, USA, TMD version 1; F-0001 |
| Frozen reference manifest | Package frozen | `config/targets/wpse-v1-package.json`; executable acceptance scope pending |
| Artifact and section inventory | Reference extraction complete for recognized containers | 3,420 files; F-0005; executable coverage and native-resource internals pending |
| Toolchain identification | Pending | Candidates listed in `re/TOOLCHAIN.md`; none confirmed |
| Reproducible game build | Pending | Compiler and linker not selected |
| Source reconstruction | Pending | No reconstructed functions |
| Exact executable match | Pending | Requires real original and rebuilt artifacts |
| Full release match | Pending | Requires complete content and packaging coverage |

## Progress images

The README shows images rendered by `tools/progress.py`, the project's own visualizer. It replaces hosted decomp.dev badges, which the project owner reports are being sunset ([D-0008](DECISIONS.md#d-0008-generate-progress-images-in-repository)). Regenerate and commit the images whenever phase state or measured progress changes:

```powershell
python tools/progress.py render --phases config/progress.json --output-dir docs/images
```

| Image | Source | Shows |
| --- | --- | --- |
| `docs/images/progress-roadmap.svg` | `config/progress.json` | Each [roadmap](ROADMAP.md) phase as done, in progress, or not started |
| `docs/images/progress-code.svg` | objdiff report, when given | Matched code, code linked from source, matched functions, matched data, and per-category code; without a report it states that progress is not measured |
| `docs/images/progress-treemap.svg` | objdiff report, when given | Translation units sized by code bytes and shaded by match percentage in seven bins |

Phase states in `config/progress.json` must agree with the milestone table above and the roadmap gates. States must run done, then at most one active, then pending; the tool rejects anything else. Pass `--report build/<target>/report.json` only after roadmap Phase 4 documents the objdiff denominator in [matching](MATCHING.md); the tool refuses a report with zero total code. Add the treemap to the README at that point. "Matched" means source compiles to identical code; "linked from source" means the final build uses that source rather than an original fallback object. Only the linked figure approaches source coverage, and it still requires the ledger review described in [matching](MATCHING.md).

Images use a single blue sequential ramp with separate light and dark steps selected through `prefers-color-scheme`, embed hover titles that work when an SVG is opened directly, and contain no timestamps, so identical inputs produce identical bytes.

## Reconstruction ledger

No entries yet. For each unit, record target, artifact, address range, source path, confidence, implementation status (`Matching`, `NonMatching`, or `Equivalent`), match evidence, fallback dependencies, and for non-matching units the suspected cause and any decomp.me scratch link. These belong here, not in source comments. Establish function-count and code-size denominators before reporting coverage.

## Latest session

Pulled `40483bd` (FEAT: Add progress), preserved local extraction changes, and reconciled overlapping documentation with its roadmap and progress tooling. Completed recursive reference extraction: 156 U8 archives, 134 decompressions, 3,420 inventoried files. A fresh second run produced byte-identical outputs for every file. Original WADs and prior extracted contents remain unchanged. See [extraction](re/EXTRACTION.md) and F-0005 for exact commands, tool pins, and validation limits. Native asset formats remain preserved, not converted to editable exports. New traps: 0x40 assets require LZH8 rather than dtk NLZSS, and backslash-bearing U8 names require parent directories to be created before Windows extraction. No source reconstruction or game match is claimed.

Roadmap Phase 1 remains active: executable acceptance manifests, full section/coverage review, and input provenance still need resolution. Code progress remains unmeasured; no fabricated objdiff report was supplied.

## Earlier session: progress images

Added `tools/progress.py` and `config/progress.json` and embedded the roadmap and code progress images in the README; see [progress images](#progress-images) and [D-0008](DECISIONS.md#d-0008-generate-progress-images-in-repository). The code image states that progress is not measured because no objdiff report or denominator exists. Validation is recorded in [validation](VALIDATION.md#progress-image-validation). No reverse engineering findings were produced. New trap recorded in `AGENTS.md`: progress images are committed outputs that go stale unless regenerated.

## Earlier session: Petari study

Studied the Petari (Super Mario Galaxy) decompilation at commit `e0ab141` and recorded its tooling, repository mechanics, and CodeWarrior matching knowledge in [prior art](re/PRIOR_ART.md), with each practice mapped onto this project's rules. Wrote the phased [roadmap](ROADMAP.md) and proposed [D-0007](DECISIONS.md#d-0007-evaluate-the-dtk-template-ecosystem) to adopt the dtk-template ecosystem once the executable is confirmed. Recorded the first reverse engineering finding, [F-0001](re/FINDINGS.md#f-0001-wad-container-metadata): unencrypted WAD header, ticket, and TMD fields of the local WAD, including title ID `WPSE`, title version 1, seven contents, and boot index 6. No content was decrypted or extracted, no tool was acquired, no manifest was fingerprinted, and no code changed. New traps recorded in `AGENTS.md`: vendored dtk-template tooling conflicts with the no-comments rule, and Petari's source-comment conventions must move to documentation.

## Earlier session: documentation rules

Updated `AGENTS.md` to require persistent traps and findings with evidence links, and seeded it with established initialization lessons. Clarified that end-to-end validation is preferred and end-to-end suites are allowed; unit tests, code comments, and docstrings remain prohibited. No new reverse engineering findings or newly observed tooling traps emerged. This session changed documentation only; no game or tooling behavior changed.

## Initialization evidence

Added project architecture, living documentation, reverse engineering records, and exact artifact verification. No new reverse engineering findings were produced. No game match was attempted because the original and reconstructed artifacts are unavailable. See `HANDOFF.md` for the next action.

CLI integration checks passed for matching samples and rejection of mismatches, missing output, changed references, overlapping roots, and repeat fingerprinting. Syntax, configuration, documentation links, original ignore preservation, and comment-free Python were checked. See [validation](VALIDATION.md) for evidence and limits. No unit tests were added.
