# BTE data matching audit, 2026-11-05

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

## Status

Read-only inspection of the current report finds 3,184 unmatched BTE data bytes in five sections. The source bytes already reproduce each original section prefix exactly. The discrepancy is 22 bytes of trailing native alignment counted as owned data. No source change or additional reconstruction is justified by this evidence. A symbols-only shared-configuration patch is prepared for the coordinator; full-link acceptance remains pending. The isolated objdiff diagnostic reaches 100% for all five owners without changing any section extent, source byte or denominator.

The overall current object matched-data diagnostic is 1,245,840/1,788,360 bytes (69.663826%), distinct from 100% source-selected data and exact linked main.dol. This audit does not redefine that metric.

## Measured ranges

| Owner | Section | Original start | Target size | Source size | Following source owner | Following alignment |
| --- | --- | --- | ---: | ---: | --- | ---: |
| btu_init | .sdata2 | 0x804BB630 | 8 | 6 | tpl/TPL | 8 |
| gap_utils | .bss | 0x804AD540 | 944 | 940 | bte/hidd_mgmt | 8 |
| hcicmds | .rodata | 0x80398160 | 16 | 10 | bte/hidh_conn | 8 |
| hidh_api | .data | 0x803E2DC0 | 280 | 275 | bte/hidh_conn | 8 |
| l2c_api | .data | 0x803E32C0 | 1936 | 1931 | bte/l2c_csm | 8 |

The four initialized sections have byte-identical target/source prefixes and exclusively zero trailing bytes. The BSS objects are NOBITS: their measured storage extents differ by four bytes. `GAP_Init` is already instruction-exact and clears `sizeof(gap_cb)` from the reconstructed complete `tGAP_CB`, corroborating the 940-byte extent. `BT_BD_ANY` is a six-byte Bluetooth address.

Each following compiled source object's ELF section alignment is eight, independently read from that following object rather than inferred from the preceding owner. Rounding each source end upward to eight reproduces its observed following address. The initial split-boundary proposal was superseded after the coordinator requested a symbol-only diagnostic. Correcting only the five inferred last-symbol sizes is sufficient: objdiff recognizes the remaining original section bytes as padding and reports every section exact. The retained patch leaves all original split extents and denominators unchanged. The following-object alignment evidence still explains why the surplus original bytes are padding rather than missing data.

Private handoff artifacts are `tools/local/data-match-20261105/bte-padding.patch` and `bte-padding-evidence.json`. The patch now contains only symbol-size corrections; `bte-symbol-sizes.patch` is an identical clearer-named copy. The JSON records following-object SHA-256 digests, exact sizes, addresses and alignments. No production configuration was edited by this audit. The coordinator must apply the patch, rebuild the real pipeline, verify frozen main.dol and inspect the regenerated report before acceptance.

## Isolated symbol-only verification

An isolated `objdiff-cli report generate` invocation uses copies of the five target objects with only their ELF symbol-size fields corrected. Original and source section bytes, target section sizes and source objects remain unchanged. These scratch target edits are diagnostic, not selected build inputs; the production fix is the corresponding inferred symbol metadata. The CLI exits zero and writes `tools/local/data-match-20261105/bte-symbol-only/report.json`.

| Owner | Matched data after symbol-size correction | Total data | Result |
| --- | ---: | ---: | --- |
| btu_init | 8 | 8 | 100% |
| gap_utils | 1696 | 1696 | 100% |
| hcicmds | 16 | 16 | 100% |
| hidh_api | 1312 | 1312 | 100% |
| l2c_api | 1936 | 1936 | 100% |

All code comparisons remain 100%. The expected production matched-data increase is 3,184 bytes with no denominator change, subject to normal regeneration and frozen artifact verification by the coordinator.

## WUD source fidelity audit

The eight WUD Thumb source files, `patches.json`, native linker script and `tools/firmware/build_wud.py` remain within the narrow D-0017 exception. The generator assembles real named Thumb routines, links named controller ROM continuations, extracts allocated sections and serializes typed protocol fields. It does not read the original executable or payload as generation input. The 180-byte payload and seven eight-byte replacement blocks are explicit; no new original-language claim is made.

A second isolated diagnostic corrects WUD's final inferred literal-group size, `lbl_803DFF0C`, from 0x354 to 0x353. This alone restores all 4,520 .data bytes to 100%, producing 13,216/13,224 matched data bytes for WUD. `tools/local/data-match-20261105/wud-symbol-size.patch` records that metadata-only correction, and `wud-symbol-only/report.json` records the successful CLI result. Its source object still includes a discarded eight-byte callback-message pointer table in .sdata2; objdiff explicitly reports this as an eight-byte insertion before the seven retained literal bytes. Changing the inferred grouping of those literals does not fix the extra pointer table and is not proposed. HBMFrameController likewise contains a discarded source vtable before retained RTTI, and a four-byte .rodata alignment tail. These require ownership and retained-output analysis rather than fabricated source padding.

## WUD diagnostic storage correction

The unused two-pointer `gWudUnlinkedCallbackMessages` table existed only to retain two original diagnostic strings. It introduced eight extra .sdata2 bytes which the final linker discarded. Replacing that table with two descriptive static char arrays directly models the retained diagnostic bytes without an extra pointer table. `#pragma force_active` ensures the original strings remain retained by the native linker even though their original callers were stripped. No new runtime behavior or instruction body is introduced.

Two isolated compiler alternatives establish placement: plain char arrays reproduce the initialized string pool, while const char arrays move strings to a different section and break eleven function comparisons. The accepted candidate uses static char arrays under force_active. It compiles with the normal Wii/1.0 flags and, with only the independently proven final .data symbol-size correction, objdiff reports all 86 functions and all 13,224 data bytes exact. Receipt: `tools/local/data-match-20261105/wud-source/forced_char_arrays/report.json`. The target comparison retains original small-data symbol boundaries; the unsuccessful three-byte grouping experiment is not included.

The canonical WUD.c now contains this small storage correction. Complete native-link verification is pending the coordinator; object comparison alone is not acceptance. The firmware assembly and generated protocol data are unchanged.

## Traps

- A whole data section can receive zero matched-data credit when only its trailing alignment differs. Compare exact section extents and the following compiled object's actual alignment before modifying correct source declarations. Correct inferred symbol sizes first: objdiff can recognize unchanged trailing section bytes as padding, preserving the counting denominator.
- A complete linked artifact does not establish 100% pre-link object matched data. Keep the diagnostic measurement and source-selected coverage separate.

## HBM metadata extent audit

Seven more inferred metadata extents include four bytes of native zero alignment each. The read-only final-ELF ownership audit proves each original address equals the retained compiler symbol address, the corrected size equals both source and final-ELF symbol sizes, the retained bytes match exactly, and every relocation has the same kind, relative offset and resolved original address. No source or split edit is needed. `tools/local/data-match-20261105/hbm-symbol-corrections.json` preserves these proofs and source-object SHA-256 digests; `hbm-mappings.json` gives the five anonymous symbol pairings.

| Symbol | Correct size | Meaning |
| --- | ---: | --- |
| lbl_803BA048 | 12 | GroupAnmController RTTI base list |
| lbl_803BA0E8 | 20 | PaneComponent RTTI base list |
| lbl_803BB520 | 12 | AnimTransformBasic RTTI base list |
| __vt__Q36nw4hbm3lyt4Pane | 100 | Pane vtable |
| __vt__Q36nw4hbm3lyt16ResourceAccessor | 20 | ResourceAccessor vtable |
| lbl_803BB9F8 | 20 | TextBox RTTI base list |
| lbl_803BCD28 | 12 | ResFontBase member-function pointer |

The isolated report is `hbm-symbol-only/report.json`. Pointer-backed RTTI comparisons still expose the separately assigned comparator issue: pinned objdiff maps the string symbol itself but does not honor that mapping when comparing a data relocation to it. FrameController demonstrates the difference with identical ADDR32/addend-zero relocations to the paired name string. Renaming only the scratch target name to its compiler name makes the whole unit exact, confirming a comparison-name issue; that diagnostic rename is not a proposed production symbol name. The coordinator owns the comparator correction and full integration acceptance.
