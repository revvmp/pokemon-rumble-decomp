# Matched-data reporting and source-selection audit

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

## Verified baseline

The 2026-11-05 audit reads the current `build/wpse-v1/report.json` rather than treating the earlier source-completion headline as proof of the requested Matched Data percentage. The report is 4,561,010 bytes with SHA-256 `c88be97be32c594219052939c0a613ccc4c82eec59cd4ca6f0942421a29ef54e`, identical to the report recorded in [main executable acceptance](main-source-completion-20261104.md).

| Report measure | Observed value |
| --- | ---: |
| `matched_data` | 1,245,840 bytes |
| `total_data` | 1,788,360 bytes |
| `matched_data_percent` | 69.663826% |
| Remaining matched-data gap | 542,520 bytes |
| `complete_data` | 1,788,360 bytes |
| `complete_data_percent` | 100% |

Source selection and object comparison are distinct measurements. The frozen full DOL match establishes artifact identity, while the current pre-link object report still leaves the literal Matched Data goal unfinished. Neither the report nor its numerator, denominator, or displayed percentage was changed by this audit.

## Renderer and native input checks

`tools/progress/progress.py` renders Matched Data directly from `matched_data / total_data`. An in-memory invocation of its existing render functions, using the current report and `config/progress.json`, produces exactly the committed bytes of both SVG files:

| Image | SHA-256 |
| --- | --- |
| `docs/images/progress-code.svg` | `b61826eecce0ebc44ff7f9127d397d88e437da0c2a36ab868f67527fefafe68a` |
| `docs/images/progress-roadmap.svg` | `384eac111e364d58c2c2b8d6f8b1fa2f4ae294d1b7bc873136704794bea0a731` |

The code image correctly displays 69.66% Matched Data. There is no stale-image or renderer defect explaining the gap.

`build/wpse-v1/main_link.json` contains 897 logical groups and 902 raw native linker inputs. Every selected path lies under the source-output tree. Direct full-file comparisons of each selected object with its corresponding `main_link/unit_NNNN.o` alias find zero differences across all 902 pairs. This independently confirms the existing source-selection claim without converting it into matched-data credit.

## Remaining function-count padding

The report's function arrays and aggregate measures both contain 13,462 entries. Exactly one entry begins with `pad_`: `main/nw4r/ef/ef_resource` contains `pad_03_802347B4_text`, size twelve, at `0x802347B4`. Both original and rebuilt DOLs contain twelve zero bytes at that address, file offset `0x1E4A74`.

The `ef_resource.cpp` split begins at `0x802347B4`; its first actual function begins at `0x802347C0`. The compiled source object's `.text` has native alignment sixteen and begins with the real `Resource::GetInstance` instruction sequence, rather than the twelve zeros. This is an alignment entry still attached to a source-owned split, distinct from the 470 standalone padding objects previously removed.

The renderer already excludes `pad_` entries and correctly displays 13,461 real functions. The earlier documentation's description of all 13,462 report entries as real CPU functions is therefore one too high. No renderer modification is justified. Any later split/counting correction must preserve the independently verified native alignment and full frozen artifact, and must not claim extra reconstructed instructions. This audit does not modify the split or denominator.

## CPpuCode local string identity

The existing target symbol at `.data:0x803AD110`, size twelve, is named `@STRING@Load__8CPpuCodeFPCcb`. The original split `CPpuEnv.o` places it at `.data+0x3938`, containing `script.arc/` followed by a null terminator. The current compiler output places the identical twelve bytes at `.data+0x3BA0`, named `@STRING@Load__8CPpuCodeFPCci`.

The accepted final `main.elf` places the compiler's `FPCci` symbol at exactly `0x803AD110`, with size twelve and identical content. `include/game/CPpuEnv.h` declares `CPpuCode::Load(const char*, int)`, consistent with the `i` suffix. The old `b` suffix is a stale inferred identity. Renaming only that target symbol to `@STRING@Load__8CPpuCodeFPCci`, preserving its address and extent, is supported by the final address, bytes, compiler symbol, and declared signature. This is a proposed symbol correction; ordinary rebuild/report and frozen verification remain the coordinator's acceptance steps.

## Independent inventory-tool review

The independent review of `tools/re/data_match_inventory.py` used its actual CLI against the preserved baseline report with `--json`. It exited zero, retained the report's SHA-256 unchanged, and reconciled all 542,520 lost-credit bytes across 242 units. The section totals are 249,396 `extab`, 155,158 `.data`, 62,420 `.bss`, 55,680 `extabindex`, 9,790 `.sdata`, 8,240 `.rodata`, 1,316 `.sdata2`, and 520 `.sbss` bytes. These are failed whole-section credits, not a claim that every byte in each section differs.

The utility preserves the report's measures, exposes its reported percentage, and never substitutes `complete_data` for `matched_data`. It verifies unit totals against aggregate totals and failed-section sizes against each unit's lost credit. The real `--limit -1` rejection exits two. Tokenizer and AST inspection find no code comments or docstrings. No unit tests were added.

## Independent exception-mapping review

The first independent review of `tools/re/map_exception_symbols.py` executed its CLI for CDlgPiiList, CModel, and PiiPropData, printing the audit to stdout. It exited zero without modifying the shared objdiff configuration. All 96, 104, and 26 target records respectively have matching complete index/table bytes and normalized relocation tuples; no function identities are missing. The source objects contain 158, 141, and 45 records respectively, including additional records for unretained compiler functions. Those surplus records are reported, not counted as additional reconstructed target data.

The mapping mechanism pairs exception-index records through their unique function relocation identity, then pairs each corresponding exception table. Existing function symbol mappings are respected. Audit comparisons retain full record bytes plus every relocation offset, target identity, relocation kind, and addend within each paired record. Differences are reported explicitly. The generated configuration changes comparison symbol identities only: no compiler object, original artifact, or report percentage is rewritten. Tokenizer and AST inspection find no comments or docstrings.

This diagnostic is not a replacement for pinned objdiff output or frozen full-artifact verification. In particular, the remaining pre-link exception-index score limitation is not assigned fabricated credit. Initial review requested complete index-section byte/relocation coverage checks, validation that exception-table targets are nonempty object symbols, and output/input collision rejection before general production integration. The author implemented those checks.

The independent follow-up CLI audit across all 897 units exits zero with no parsing errors. The 169 units containing exception records have 4,837 target records, 4,836 mapped records, 4,836 exact indexes, and 4,823 exact tables. One missing function identity and thirteen table discrepancies remain explicitly reported; the tool does not mask them or claim their bytes match. Source and target index sections must each be completely covered by nonoverlapping twelve-byte object records and exactly their expected relocations. A real invocation with `--output objdiff.json` rejects the input collision with exit one before writing; the shared configuration remains byte-identical. The reviewed revision has SHA-256 `1bad6cd1258a61adde28a9b90a6e32869f2543a2658b44bd9167f28d165084a2`, with zero tokenizer comments and zero AST docstrings.

The review supports using these derived identities to guide ordinary objdiff comparison while retaining the unmodified objects and full acceptance gates. It does not establish 100% Matched Data. A separate reader-reuse caveat was reported: linked executable symbol values are absolute addresses, so any optional EXEC-mode use of the ELF reader's `body` method must subtract the containing section address. The audited exception-mapping CLI consumes relocatable objects and is unaffected by that extension.

## Independent comparator investigation and CLI rejection checks

Independent inspection of the pinned upstream [objdiff 3.8.2 data comparator](https://github.com/encounter/objdiff/blob/v3.8.2/objdiff-core/src/diff/data.rs) confirms that `reloc_eq` does not receive explicit symbol mappings. It checks relocation flags and then existing section/name/address heuristics. The downloaded unmodified source file has SHA-256 `c803623c19336444f79b81deff418ed1c30167651b74e228bd307a1f719d3b35`. Thus mapping an exception-table symbol pairs its own bytes for comparison but does not establish its identity when another data record points to it at a different object-relative offset.

The isolated candidate passes explicit mappings only to finalized data comparisons. When a mapped relocation identity exists, the added branch requires the expected destination name, unchanged relocation flags and addend, and equal defined section names or two undefined symbols. An explicit mismatch returns false rather than falling through to an address heuristic. Automatic candidate pairing and code comparison retain their original behavior. This is a comparison-identity correction, not an exemption for arbitrary differing data.

Build feasibility was independently checked: installed Cargo and Rust are 1.89.0, satisfying the source's Rust 1.88 minimum; Visual Studio 2022 Community provides MSVC 14.41.34120. The isolated dependency lock contains no Git dependencies. The existing production executable retains its pinned SHA-256 `36d229ac6ce74a26b47f42cf86ea8e808d8cd834a54aa62d7ed3f47b765d971b`; this review does not change that pin or select the candidate for production.

Seven actual CLI comparisons use isolated copies of the CDlgPiiList original/source objects under `tools/local/data-tooling-comparator/`. The initial copies reproduce the real mapped `@eti_80046398` record and its mapped `@etb_80010268` target. Intentional mutations affect diagnostic copies only; direct comparisons confirm both original and build objects remain unchanged. The independently exercised candidate executable has SHA-256 `298c0b4f7cf243633bb9f9f965b8053830ea5b28fcd29c1f3fa2196d434baeca`.

| Actual CLI case | Pinned result | Candidate result |
| --- | --- | --- |
| Exact record with explicit index/table mappings | Index 80%, table 100% | Index 100%, table 100% |
| Remove the table identity mapping | Index 80% | Index 80% |
| Change a non-relocation index byte | Index 75% | Index 95%, still rejected as exact |
| Change relocation type | Index 80% | Index 80% |
| Redirect the relocation to a different, unpaired table | Index 80% | Index 80% |
| Change exception-table payload byte | Table 87.5% | Table 87.5% |
| Set a nonzero symbol-relocation addend | Loader rejects, exit one | Loader rejects, exit one |

The positive case demonstrates the missing identity comparison. The six negative cases demonstrate that missing mapping, changed bytes, changed relocation kind, changed target, changed table contents, and invalid addends do not receive an exact result. Diagnostic JSON receipts are `pinned-results.json` and `candidate-results.json` in that isolated directory. No unit tests, input masking, object changes in the production build, or postprocessed report percentages were introduced. Production adoption remains subject to source/tool pinning, reproducibility, full report review, and frozen executable verification.

### Reproducible candidate and acquisition review

The initial repeated-build difference was traced by the author to an absolute path for generated `objdiff.report.rs` inside the Cargo target directory; deterministic PE timestamps alone did not remove that embedded source path. The acquisition helper remaps upstream source, Cargo home, and the generated target directory and passes the native `/Brepro` linker option. An independent full-file comparison of `repro-target-one/release/objdiff-cli.exe` and `repro-target-two/release/objdiff-cli.exe` then finds zero differing bytes. Both files are 9,108,480 bytes with SHA-256 `2ab661ca3a99f7a1cddf6263de69cea724c1c2d413937ae96286a3530f4a66b8`; their PE reproducibility timestamp fields agree as well.

All seven actual CLI cases were independently rerun against that exact reproducible binary. Their results are identical to the table above, including every rejection. `tools/local/data-tooling-comparator/repro-results.json` records the exact candidate hash and results.

The reviewed `tools/setup/acquire_objdiff.py` verifies the upstream archive, Cargo lockfile, patch recipe, every original/patched source digest, exact Cargo/Rust version strings, and final executable size/hash. It uses Cargo's locked dependency mode, builds in a fresh isolated directory, and replaces the destination only through a verified temporary output. Manifest and patch stability are rechecked before publication. No source/object rewrite participates in game reconstruction. The helper's acceptance checks and diagnostic candidate do not by themselves establish production acceptance; coordinator integration and frozen executable verification remain separate.

## Tracked mapping integration review

The first full mapping-file audit reads 16,467 entries across 479 units at SHA-256 `d1eab2ff1dbf96e4b41ffe0d62f61ad0a24493e52a2e56143d04cb9279dbbfbb`. It finds no duplicate JSON keys, unknown units, repeated destination names within a unit, or names missing from either actual ELF symbol table. Every unambiguous pair has equal symbol kinds and section names and nonzero extents.

Six target keys are ambiguous because their names are repeated across different defined sections: OSPlayTime's `@470OSPlayTime0`, NANDCore's `@491NANDCore0`, NANDCheck's `@491NANDCheck0`, and scsystem's `@492scsystem48`, `@492scsystem60`, and `@492scsystem78`. These are real repeated target names, not undefined aliases. They were reported to the coordinator and SDK owner for distinct section-qualified identities or removal of ambiguous mappings; the coordinator subsequently removed all six ambiguous entries. Objdiff resolves mappings by name and can silently skip missing names or section-kind mismatches, so raw dictionary bijection alone does not prove that every mapping is usable.

Review requested and the coordinator implemented rejection of duplicate JSON keys, invalid top-level shapes, unknown unit names, invalid mapping values, and repeated destinations. When a tracked mapping file is configured it is authoritative for every unit, preventing removed entries from surviving through a stale generated `objdiff.json`. The mapping path is an implicit dependency of Ninja's configure edge.

Actual end-to-end configure checks ran in an isolated scratch copy under `tools/local/data-tooling-configure`, using byte-identical copies of the current `configure.py` and `tools/build/project.py` and copied real source/configuration inputs. The normal case exits zero. Unknown units, a top-level list, duplicate keys, repeated destinations, and non-string values each exit one with the intended diagnostic and leave the preceding generated objdiff configuration unchanged. A deliberately stale mapping inserted only in that isolated generated configuration disappears after the tracked unit's mappings are removed and configure runs again. The generated Ninja file contains the mapping-file dependency using Windows path separators. No shared build outputs or tracked mapping inputs were changed by these checks.

The reviewed configure source SHA-256 is `7e0551ca7ec623d118e811627e0e932c441721c8a7b357fbe57ed415ca77ddfc`; the build module is `44ed829a6ab901d7fcb746b9111775144846a12072e68336287705b0cde9594c`. These checks validate configuration ingestion and dependency declaration, not compilation or frozen-artifact acceptance. The existing generation order can still write the Ninja file before later mapping validation fails; rejected invocations must not be treated as completed builds.

## Integrated v1 same-input audit

After normal acquisition and production integration of `3.8.2-rumble-data-mappings-1`, an independent audit snapshots the current objdiff configuration and all 1,794 target/source objects into `tools/local/data-tooling-integrated`. Both the upstream and patched real report CLIs consume those same immutable copies. No source configuration, tracked mapping, symbol/split file, production report, DOL, or original input object changes during the comparison.

| Measure | Official 3.8.2, current mappings | Integrated mapping-aware comparator |
| --- | ---: | ---: |
| Matched data | 1,645,586 | 1,747,988 |
| Data denominator | 1,788,360 | 1,788,360 |
| Matched data percentage | 92.01648% | 97.74251% |
| Matched and source-linked code | 3,078,460 | 3,078,460 |
| Report function entries | 13,462 | 13,462 |

The data improvement is a net 102,402 bytes: 125 units gain 102,458 bytes and one loses 56 bytes. CUIPiiListItem's `.sdata` is the sole data-score regression, caused by the stricter explicit identity path encountering a section-pool-plus-addend relocation form. It remains an openly reported residual requiring a separately proved canonicalization; it is not hidden behind the aggregate gain. Every per-unit function array is exactly identical between reports. All code, data, function, and unit denominators remain unchanged, and there are no code/function regressions.

The official report SHA-256 is `e655909f009dfab7bb1fff931ffbde0cbea1cade5080cdc4ebf1614104de4c84`. The patched report SHA-256 is `0b3b317ddadaedba7004fdb41a7cc57f0aed8205e4fa34c165ba3457b5fbf476`, exactly equal to the production report at this checkpoint. The source objdiff configuration SHA-256 is `2fabffe1b4fb0148747c4ede0683d6f2783f21b252f66ef8c4a03d3d516b1570`. These figures compare the two tools on the same current reconstruction, separating symbol/ownership gains from the comparator fix itself.

The real inventory CLI independently reconciles 40,372 remaining bytes across 23 units: 29,260 `.data`, 8,268 `extab`, 1,856 `.rodata`, 784 `.sdata`, and 204 `extabindex` bytes. Largest whole-section credit gaps are CPpuEnv 14,664, CPpuEngine 3,740, CFileData 2,952, homebutton 2,744, and CTitleSession 2,472 bytes. Detailed unit/section receipts are `audit-summary.json` and `remaining-inventory.json` in the isolated audit directory. A near-100% section still loses its entire section credit; these figures do not imply that every byte differs.

Read-only full-byte comparison independently confirms that the current 4,094,880-byte DOL equals the original and its frozen manifest SHA-256. The literal Matched Data goal remains unfinished at this checkpoint. No denominator adjustment, report rewrite, or substitution of source-selected data credit participates in the gain.

## Independent v2 pooled-relocation review

The v1 CUIPiiListItem regression identifies a second representation of an established relocation: the target points to a mapped data object directly, whereas the compiler points to its containing data pool plus the object's offset. The reviewed v2 branch is restricted to an explicit mapped identity and a true section symbol or a zero-sized unknown `...data.` pool symbol. It requires one uniquely named mapped object, nonzero equal object sizes, an in-object original addend, equal relocation flags, the same pool/mapped section, the same original/mapped section name, and checked equality of the two effective destination addresses. The direct mapped-name path and the entire unmapped comparison path retain their earlier behavior.

Independent byte comparison confirms that both fresh v2 builds are identical, 9,139,712 bytes, with SHA-256 `ee60619f592086703c25c122e9445a898e418da154e8f5300d4d5b4ff98b9c11`. All seven earlier exception CLI controls produce exactly the same results with this binary. A completely unmapped CUIPiiListItem comparison produces identical whole diff JSON under v1 and v2.

Twelve additional real CLI cases use isolated CUIPiiListItem copies under `tools/local/data-tooling-pool-v2/`. Both production objects and the preserved source/target audit copies remain unchanged. The exact RTTI record rises from 50% to 100%, closing its `.sdata` section from 90.47619% to 100%. Every adversarial case remains non-exact or is rejected:

| Control | v2 outcome |
| --- | --- |
| Explicit mapping names a different destination | Record 75% |
| Data-pool relocation addend advances by one byte | Record 75% |
| Relocation type changes | Record 75% |
| Mapped object's size changes | Record 75% |
| Mapped name becomes ambiguous | Record 75% |
| Mapped symbol kind changes | Record 75% |
| Mapped destination moves while preserving its size | Record 75% |
| Referenced data payload byte changes | `.data` 99%; data difference remains visible |
| Mapped symbol moves to an invalid or incompatible section | CLI rejects |

Removing only one pointer's mapping can still yield 100% for this particular valid record: that pointer continues through the unchanged upstream address-equivalence rule, while its other pointer uses the valid v2 mapping. This is not counted as a rejection case. The fully unmapped equivalence check above isolates and confirms unchanged legacy behavior. The same-size wrong-destination control separately proves that destination validation is not merely an incidental size-mismatch rejection.

The exact candidate hash and earlier-case results are recorded in `tools/local/data-tooling-comparator/v2-results.json`; pooled-case receipts are `results.json` and `additional-results.json` in the pooled-control directory. The review finds no bypass of data-byte comparison or relocation type, destination, section, size, or ambiguity checks. Production pinning and complete frozen-build verification remain coordinator acceptance steps.

## Traps

- A source-completion headline does not imply the objdiff Matched Data metric is complete. Read the raw `matched_data`, `complete_data`, and denominator independently before claiming the user's percentage target.
- An aggregate function count can retain an alignment entry inside an otherwise complete source unit. Inspect the actual function list, bytes, and native alignment before treating a difference from the renderer's filtered count as a renderer bug.
- A compiler-local string name can retain an obsolete inferred parameter type. Match the final address, extent, contents, and current compiler identity before renaming; equal strings alone are insufficient ownership evidence.
