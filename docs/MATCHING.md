# Exact matching and completion

## Accepted main executable scope

The 2026-11-05 main.dol acceptance satisfies the gates below for the frozen executable scope, with D-0015/D-0016 controls and the two explicit D-0017 firmware exceptions disclosed. Matched and source-selected data are both 1,788,343/1,788,343 bytes (100%); matched and source-linked code are both 3,078,460/3,078,460 reported bytes. All 897 units are complete. The 13,462 exact report function entries include one twelve-byte zero-alignment entry, so 13,461 are real functions. Embedded programs build from symbolic source. [Data acceptance](re/findings/matched-data-completion-20261105.md) records two complete clean builds, exact hashes, the narrowly corrected data comparator and seventeen-byte native-alignment denominator correction. The earlier [source acceptance](re/findings/main-source-completion-20261104.md) preserves original-language and asset provenance limits. Boot-loader and WAD packaging remain outside this claim.

## Reference contract

Each acceptance artifact has a byte size and SHA-256 captured from an identified original. The verifier validates the reference and reconstruction against those values and directly compares both files. Changed originals, missing artifacts, unsealed manifests, and mismatched builds fail.

The verifier rejects overlapping reference and output roots, links in artifact paths, and reference/output files that are the same underlying file. Do not run builds or extraction while verification is reading their files. A manifest change during verification also fails.

Never strip headers, normalize padding, ignore sections, or patch differences during acceptance. Narrower comparisons are diagnostics and must be labeled partial. See [the reverse engineering workflow](re/WORKFLOW.md) for mismatch investigation.

## Scope and progress

The manifest is an explicit acceptance list. Extra files in either root are not checked. Review it against [the content inventory](re/INVENTORY.md). Matching a single executable does not establish a full game or package match.

| Measure | Required evidence |
| --- | --- |
| Reconstructed functions or bytes | Actual source implementations and an established counting denominator |
| Matching functions or bytes | Local binary identity and a recorded comparison method |
| Remaining fallbacks | Explicit list of assembly, original objects, blobs, and unresolved implementations |
| Artifact match | Passing exact verification for a named frozen manifest |
| Full release match | Complete inventory coverage, including packaging when claimed |

Do not publish percentages before the denominator is known. Runtime behavior and compilation alone cannot prove exact identity. Hash equality cannot prove source reconstruction: copying originals into the output directory can satisfy byte comparison and must never count as decompilation progress.

## Counting method

Code progress is measured by objdiff-cli 3.8.2 through `ninja` (writes `build/<target>/report.json`) over the units defined by dtk 1.8.4's analysis of `config/<target>/symbols.txt` and `splits.txt`. For `wpse-v1` the first clean build measured 3,098,940 code bytes, 1,789,960 data bytes, and 8,782 units, of which 8,781 are dtk auto-generated placeholders. objdiff reports 17,034 functions, but 3,569 of those are zero-filled `pad_` alignment entries ([F-0011](re/FINDINGS.md#f-0011-objdiff-function-count-includes-alignment-padding)); the real function denominator is 13,465. The denominator can shift slightly as symbol boundaries are corrected; report the figures from the current report rather than reusing old numbers.

The 2026-11-04 five-function checkpoint measures 3,078,460 code bytes and 1,788,360 data bytes across 897 units, with 13,462 real function entries. All 470 standalone code-padding entries are now excluded through independently verified native alignment skips, removing 3,792 bytes without changing either instruction-credit numerator. Twenty data-padding objects plus sixteen bytes after the recovered IdleThread remove 375 data bytes. [The alignment ledger and full-link evidence](re/findings/native-alignment-padding.md) and [data evidence](re/findings/anonymous-data-20261104.md) preserve the accounting trail.

"Matched" counts code whose compiled source is identical to the original split object. "Linked" (objdiff `complete_code`) counts code in units marked `Matching` that the final build links from source; it is the figure that approaches source coverage. objdiff fuzzy percentages ignore inter-function alignment padding; a 100% unit can still break the full link, so a unit is `Matching` only when the full build also passes `tools/verify/verify.py`.

## Completion gates

1. Release identity and provenance are documented and reference fingerprints frozen.
2. All executable modules and data are inventoried, with section layouts and address mappings where applicable.
3. Tool versions, binary digests, flags, linker order, and environment are pinned with reproducible acquisition and build commands.
4. All executable code in the claimed scope is reconstructed from source. Undeclared assembly or binary fallbacks block completion. Original-assembly units follow D-0015; the two explicitly approved embedded-firmware language exceptions follow D-0017 and must remain disclosed. Source selection alone does not establish the provenance of an embedded instruction array.
5. Asset reconstruction and local original-data dependencies are documented. No undisclosed original executable dependency remains.
6. A clean build passes exact verification for every required artifact. Repeat a clean build when establishing reproducibility and after toolchain or determinism changes.
7. A whole-game package claim requires exact final package bytes too. Unresolved metadata, signing, encryption, and container details remain blockers; they do not justify weakening the claim.
8. Progress, findings, handoff, and `AGENTS.md` agree with the evidence.

No unit tests are used. Prefer end-to-end tests that execute real tooling or the complete build-to-verification workflow, checking exit status and produced artifacts. Include relevant rejection paths. Tool checks and documented runtime observations supplement these workflows; they do not replace exact byte comparisons.
