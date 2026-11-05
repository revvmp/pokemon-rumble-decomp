# Exception-symbol identity mappings

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

The 2026-11-05 Matched Data continuation begins from a complete source-selected main executable but an object-level data score below 100%. Source selection and this requested diagnostic are distinct. `tools/re/map_exception_symbols.py` reconstructs exception-record correspondence without changing source objects, original objects, executable bytes or report numerators.

## Identity evidence

Each 12-byte `extabindex` record contains two `R_PPC_ADDR32` relocations: its function at offset zero and its exception table at offset eight. The tool pairs unique function identities between the original split and compiler output, then maps their index and table symbols. It requires zero relocation addends, valid object/function kinds, complete index-byte coverage and complete index-relocation coverage. Duplicate function identities, overlapping records, conflicting mappings and ambiguous mapped names fail.

The original records use inferred names such as `@etb_80010218`; the compiler emits local names such as `@33350`. These are distinct names for the same evidenced exception record. Existing objdiff explicit `symbol_mappings` express that correspondence without renaming either object. Additional unretained compiler records remain present and visible.

The audit separately compares every mapped record's complete raw bytes and relocation tuples, using the mapped symbol identities. Differences remain reported and mapped so the actual comparator can expose them; correspondence is not a declaration of byte identity. Output paths cannot overwrite the input configuration or any original/compiler object, including hardlink aliases. The separate generated configuration uses absolute object paths. The shared configuration is never rewritten by this tool.

## Measured experiment

An initial audit of the accepted source checkpoint examined 169 exception-bearing units. It found 4,837 original index records and 8,025 compiler records, mapped 4,836 original records, and independently proved all 4,836 mapped index records exact in both raw bytes and relocation semantics. There were 4,823 fully exact table records. One original `CGameStage` destructor identity was still unnamed; thirteen table records retained reported raw or relocation differences, including two Gecko runtime size-20 versus size-18 padding differences.

Three initial examples prove that retained symbol identity can recover correct comparison despite additional compiler records:

| Unit | Original/source records | Exact raw table bodies | Mapped extab score |
| --- | ---: | ---: | ---: |
| CDlgPiiList | 96/158 | 96 | 100% |
| CModel | 104/141 | 104 | 100% |
| PiiPropData | 26/45 | 26 | 100% |

The pinned objdiff CLI generated a real isolated full report from these mappings: matched data increased from 1,245,840 to 1,538,384 of the same 1,788,360 bytes, or 86.02206%. All 13,462 functions and 3,078,460 code bytes remained exact. These are diagnostic measurements before the coordinator's parallel metadata corrections, not a claim that the production report or milestone was already accepted.

Scratch configurations, complete audits and unmodified objdiff output are under `tools/local/data-acceptance-map/`. The audit's input SHA-256 records tie anonymous compiler names to the exact object revisions. Source changes that alter compiler-generated names require regenerated correspondence.

Real CLI rejection checks returned exit 1 for an unknown unit, output targeting `objdiff.json`, output targeting a compiler object and colliding audit/config outputs. Hashes of the protected input configuration and compiler object remained unchanged. No unit tests were introduced.

## Remaining relocation-comparison limitation

Pinned objdiff v3.8.2's [`data.rs`](https://github.com/encounter/objdiff/blob/v3.8.2/objdiff-core/src/diff/data.rs) `reloc_eq` does not receive the explicit symbol mapping. For defined targets, it accepts two compiler-generated names, equal name/addend, or equal pre-link target address in equal-named sections. Thus a mapped index's pointer to its mapped exception table can still compare unequal when preceding unretained tables move that table inside the compiler object.

The concrete CDlgPiiList record `@eti_80046398` maps to compiler `@34128`. Both complete 12-byte records are `000000000000010800000000`; both use exactly the same function identity and the mapped exception-table identity. The original table starts at extab offset 80 and the compiler table at offset 96. Pinned objdiff gives the index 80%, despite the independent complete raw/relocation proof. This is a comparator identity limitation, not evidence of a wrong unwind action.

The upstream source file was retained in ignored scratch storage with SHA-256 `c803623c19336444f79b81deff418ed1c30167651b74e228bd307a1f719d3b35`. The scoped correction below has now passed independent CLI rejection checks and reproducible tool acquisition. The coordinator owns production configuration and the full-build acceptance gate.

## Traps

- Trigger: source-linked data is 100%, but Matched Data remains lower. Lesson: use the requested raw `matched_data` field; `complete_data` measures source selection and cannot substitute for it.
- Trigger: extab is exact after explicit identity mapping, while extabindex stays near 80%. Lesson: inspect mapped pointer relocation identity and pre-link target offsets; pinned objdiff data relocation comparison ignores the mapping. Do not invent original compiler-local names or suppress relocation checks to hide this limitation.

## Comparator correction and reproducible acquisition: revision 1

The tracked `tools/setup/objdiff_data_mappings.patch.json` recipe applies a minimal change to pinned objdiff v3.8.2. Only finalized data-symbol and data-section comparisons receive the explicit mapping. Automatic symbol-candidate selection and all code comparisons retain the upstream behavior. When a relocation target has an explicit mapping, equality requires exactly the mapped target name, equal relocation type, equal addend, and equal defined section names or two undefined targets. Without an explicit mapping, the upstream comparison path is unchanged. Raw payload comparison, symbol sizes and progress arithmetic are unchanged.

An independent reviewer ran the actual old and new CLI on isolated copied objects. The exact mapped index increased from 80% to 100%. Removing its table mapping kept it at 80%. Changing an index payload byte yielded 95%; changing the relocation type or referring to the wrong target yielded 80%. Corrupting the table payload kept that table at 87.5%. A nonzero relocation addend was rejected by the object loader. An unmapped frozen-unit control produced identical complete report JSON with the original and modified tools. See [independent review](data-metric-tooling-20261105.md).

A same-input full diagnostic report measured 1,573,092 matched data bytes with upstream objdiff and 1,626,468 with the correction: 53,376 additional bytes across 116 units, with no data-score decreases or code regressions. Concurrent coordinator metadata work means these numbers are a recorded comparison checkpoint, not the final project total.

`tools/setup/acquire_objdiff.py` verifies the pinned upstream ZIP, original source-file hashes, patched source-file hashes, Cargo.lock, Rust/Cargo version strings and actual compiler binary digests, then builds in a fresh ignored directory. It uses `cargo build --release --locked -p objdiff-cli`; offline rebuilding adds `--offline`. Source, Cargo cache and generated target paths are remapped, and the native PE linker receives `/Brepro`. The helper publishes the executable atomically only after its size and SHA-256 match the manifest, and verifies that the manifest and recipe did not change during the build. Downloaded upstream sources and their licenses remain in ignored local storage; no commented upstream source is vendored.

| Input/output | SHA-256 |
| --- | --- |
| Upstream v3.8.2 source ZIP, 876,422 bytes | `4017277e35eac479f32e9e87ae5d31e86fb28dca4348d5a376e9d5ff8c67215d` |
| LF patch recipe, 3,373 bytes | `16bdf3fc4295c5110cbcfb049aab4d2584e835e1efbea2137a0bf6743af27962` |
| Cargo.lock | `defbe0c1ea748ed310da66ac7e04192abd137baebe22d7e1b539da993c9c74c5` |
| rustc 1.89.0 executable | `8500ff87da599c6b6257ef91f43eeddd1ad61cc9603fde8fd9bbf805943d76c4` |
| cargo 1.89.0 executable | `d953858241c1a0aee28e712681dde68e59946375ecabfc3bead2a152e6d7ec2b` |
| Revision 1 objdiff executable, 9,108,480 bytes | `2ab661ca3a99f7a1cddf6263de69cea724c1c2d413937ae96286a3530f4a66b8` |

The measured host is x86-64 Windows MSVC, Visual C++ 14.41.34120 in Visual Studio 17.11.35312.102. Two fresh source/output directory builds produced identical complete executables. The actual acquisition helper then reproduced the same fingerprint from its pinned archive and LF patch recipe, followed by a successful cached offline check and a second fresh `--offline --rebuild`. Four independent fresh builds therefore agree. Acquisition rejection checks fail without publishing output for a wrong patch hash, wrong archive hash, wrong Rust version and absent offline executable.

The first reproducibility experiment differed by 75 bytes: one generated protobuf Rust source path remained absolute, and its different directory name changed the PE reproducibility/debug fingerprints. Remapping the Cargo target directory, in addition to source and registry paths, eliminated every difference. No executable post-processing was used.

## Revision 2: canonical pooled destinations

Independent review of the integrated first revision identified a stricter-than-upstream score regression in CUIPiiListItem. Its original RTTI record points to two individually named data objects; the compiler expresses the same destinations as `...data.0 + 60` and `...data.0 + 76`. The two retained objects are uniquely mapped, their complete payloads match, and those offsets are exactly their source-object addresses. The [game small-data audit](game-sdata-20261105.md) independently proves all eight affected RTTI referents across CUIPiiListItem, CPauseMenu and Message, including each base list's relocations.

Revision 2 adds only this canonicalization: when an explicitly mapped destination name differs from the compiler relocation target, the compiler target must be a section symbol or a zero-sized unknown `...data.` pool symbol. The mapped destination must resolve uniquely to a nonempty data object of the same size as the original. The original residual addend must be inside that object. The mapped object and pool reference must use the same compiler section, corresponding to the original section name. Checked arithmetic must prove that the pool address plus its addend equals the mapped object address plus the original residual addend. Relocation type remains exact. An arbitrary address equality across shifted layouts, a wrong named object, or an absent explicit identity does not gain this new equivalence.

The frozen 897-unit project snapshot changes only CPauseMenu's 128-byte small-data section and CUIPiiListItem's 56-byte section relative to revision 1. Every per-function record is identical. With all explicit mappings removed from the same frozen snapshot, the complete report JSON is identical between upstream objdiff and revision 2. Message's remaining discrepancy was independently resolved as three four-byte wide strings previously inferred as three-byte objects; no additional comparator relaxation was introduced.

Two fresh revision-2 source/output directory builds reproduce a 9,139,712-byte executable with SHA-256 `ee60619f592086703c25c122e9445a898e418da154e8f5300d4d5b4ff98b9c11`. The actual acquisition helper's fresh offline rebuild from the pinned archive and revision-2 recipe independently reproduces the same binary. The 4,849-byte LF recipe has SHA-256 `3d19aa223c021ef8d04893b7f1db4e3e884b9b3ba8364c879bedeef5c4680e93`; the upstream archive, Cargo.lock, host compiler and deterministic build flags remain unchanged. Production fingerprints are selected by `config/objdiff.json`; earlier revision fingerprints above remain historical evidence.

Independent revision-2 CLI controls use frozen CUIPiiListItem object copies. The positive RTTI record improves from 50% to 100%, and its full small-data section from 90.47619% to 100%. A wrong explicit destination, pool addend plus one, wrong relocation type, changed mapped extent, duplicate mapped name or wrong mapped kind each retains a nonmatching 75% record. An invalid section is rejected by the loader; corrupting referred payload leaves its data section below 100%. A partially removed mapping can still match through the unchanged upstream same-offset fallback for that pointer, so it is not presented as a required failure. The separate fully unmapped 897-unit parity check proves that this fallback was not broadened. Receipts are retained under `tools/local/data-tooling-pool-v2/`; the independent reviewer owns their detailed [acceptance record](data-metric-tooling-20261105.md).

The completed independent review covers twelve pool-control cases and reruns all seven original exception-index controls with the exact reproducible revision-2 binary. The original seven outcomes are unchanged. A same-sized mapped object moved to the wrong address remains nonmatching at 75%; a wrong-section target is rejected even when its extent fits that other section. The entirely unmapped CUIPiiListItem diff JSON is also identical between revisions 1 and 2. No remaining comparator-review blocker was found.
