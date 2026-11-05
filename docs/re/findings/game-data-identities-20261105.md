# Remaining game data identities, 2026-11-05

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

## Result and scope

The homebutton, CUIPiiSpec and CWorldObjStatDisplay object-data mismatches come from six incorrectly inferred destructor identities, despite instruction-exact destructor bodies. The original vtable pointers identify the retained specialization. Correcting those names and their exception-index/table pairings makes all three logical units 100% code and data in the isolated objdiff report. No source, section extent, original byte, or counting denominator changes are involved.

CUIMenuSoundCallback is separate: every real symbol already compares exactly, while two explicit leading padding objects are counted against its .data and .sdata. Their complete eight-byte native-alignment proof is handed to the coordinator's consolidated padding ledger. Full shared-build integration and frozen verification remain the coordinator's acceptance step.

## Destructor evidence

| Original function address | Correct retained destructor |
| --- | --- |
| 0x801E5C00 | homebutton InitHomeButtonInfoData shared_ptr_deleter |
| 0x801E5C40 | homebutton InitSoundData shared_ptr_deleter |
| 0x801E5C80 | CHBMParam shared_ptr_deleter |
| 0x8011F780 | CUIStar shared_ptr_deleter |
| 0x8011F7C0 | CUIType shared_ptr_deleter |
| 0x80124880 | CWorldObjStatDisplay shared_ptr_deleter |

For every row, the original DOL is checked against frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`, the original vtable's destructor pointer is read directly, and the accepted final ELF places the source specialization at that exact address. The source and target raw 64-byte function bodies are identical. The private evidence packet records each original vtable name, pointer address, decoded function address, source object digest and exact symbol-line replacement.

The three homebutton identities were shifted by one: a local duplicate name for an earlier byte-deleter destructor occupied 0x801E5C00, followed by two incorrectly named homebutton deleters. The accepted source emits the actual three retained class specializations in precisely those original positions. The byte deleter's real earlier global definition at 0x800BE770 remains unchanged. CUIStar and CUIType were a direct name swap. CWorldObjStatDisplay's last retained helper was misidentified as IWorldObjStatDisplay's destructor; the earlier source interface-destructor copy is discarded. The latter was already suspected in [the earlier source-link findings](sprint-cx.md).

Corresponding exception mappings must change together. Their old pairings followed the old incorrect function names, so changing only the function names makes extabindex comparisons worse despite fixing the vtables. Pairing each original exception-index/table record with the newly identified source function restores exact comparisons.

## Isolated validation

Private handoff directory: `tools/local/data-match-20261105/game-bte-lane/`.

- `game-destructor-identities.json`: six exact production symbol-line replacements, source digests and original-vtable evidence.
- `game-destructor-exception-mappings.json`: twelve replacement mappings for six exception-index/table pairs.
- `objdiff.json` and `report.json`: isolated CLI configuration and result using metadata-only target copies and unchanged source objects.

The scratch target copies change only symbol names and binding metadata. They are diagnostic files and never selected as build inputs. The production correction is the original-symbol metadata and comparison mapping. No instructions or original data are patched.

| Unit | Matching functions | Matched data |
| --- | ---: | ---: |
| homebutton | 51/51 | 9476/9476 |
| CUIPiiSpec | 19/19 | 3420/3420 |
| CWorldObjStatDisplay | 48/48 | 4096/4096 |

The actual objdiff CLI exits zero. All code and data sections in those three reports are 100%. Full frozen-artifact verification remains required after the coordinator integrates the changes.

## CUIMenuSoundCallback native padding

| Section | Original range | Bytes | Following source alignment |
| --- | --- | ---: | ---: |
| .data | 0x803AEF4C to 0x803AEF50 | 4 | 8 |
| .sdata | 0x804B7574 to 0x804B7578 | 4 | 8 |

Both ranges are zero in the independently fingerprinted original. The following source object is `build/wpse-v1/src/game/CUIMenuSoundCallback.o`; its actual ELF .data and .sdata alignments are eight. Rounding each range start up to that alignment produces the original next data address. Its measured SHA-256 is `b662a19458888ac9915b2ce0dff9a815a455a12f3b5def066f1f4b8d6342c314`.

`menu-callback-padding.json` preserves those measurements. Both current symbols are explicitly named `pad_` objects, unlike the automatically hidden `gap_` symbols inside the same unit. Setting only their sizes to zero in an isolated diagnostic does not improve the report; the proposed correction is the established native-alignment split treatment, subject to full-link validation and the consolidated ledger. No fabricated source storage is needed.

## Traps

This applies the already documented destructor-identity trap: identical instruction bodies can hide incorrectly named destructors. Original vtable pointers, retained source ownership and exception-record consumers establish the identity; source reordering would obscure the cause. Keep the function identity and exception pair mappings synchronized.
