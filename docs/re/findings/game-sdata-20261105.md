# Final game small-data diagnostics

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

The report at 1,747,988 / 1,788,360 matched data bytes (97.74251%) leaves four small-data sections investigated here. All source objects remain unchanged. CDlgMsgWin requires correcting three original symbol boundaries; Message, CPauseMenu, and CUIPiiListItem expose equivalent pooled-data relocation forms that the comparator must recognize explicitly.

## CDlgMsgWin literal boundaries

| Original address | Source literal | Incorrect inferred bytes | Actual bytes |
| --- | --- | ---: | ---: |
| 0x804B6220 | ASCII YESNO and its terminator | 2 | 6 |
| 0x804B6290 | UTF-16 U+3044 U+3044 U+3048 and its terminator | 7 | 8 |
| 0x804B629C | UTF-16 U+306F U+3044 and its terminator | 5 | 6 |

The corresponding existing compiler-style symbols are `@STRING@OpenMessage__10CDlgMsgWinFPCw@2`, `@STRING@OpenYesNo__10CDlgMsgWinFPCw@12`, and `@STRING@OpenYesNo__10CDlgMsgWinFPCw@10`. The source uses the same YESNO pane identifier and the same Japanese Yes/No fallback strings. The latter two need `data:wstring`, not the former narrow-string classification that stopped at the first byte of their two-byte terminators.

Two additional inferred symbols, lbl_804B6222 and lbl_804B6224, incorrectly divide the middle of YESNO into two-byte fragments. A scan of all 897 original split-object relocation sets finds no references to either label. The six-byte compiler literal and frozen executable contents independently establish that they are interior bytes of the first string, so the correction removes those two false boundaries.

The original DOL retains frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Direct reads through its DOL section table yield exactly `5945534e4f00`, `3044304430480000`, and `306f30440000` at the three addresses. These are identical to the full compiled literal payloads. No literal value or source declaration changes.

The packet in `tools/local/game-sdata-20261105/corrections.json` records three before/after symbol lines and two removed interior-symbol lines. Copied configuration passes `dtk dol split --no-update`. Real objdiff against the unchanged source object then reports the entire original 432-byte `.sdata` section at 100%, up from 98.31784%. Its extent and the project data denominator remain unchanged. Full shared-build and frozen-artifact acceptance remain the coordinator's integration gate.

## Pooled-data relocation identities

The other three units' remaining RTTI payloads each contain the expected two pointer relocations. Original split objects reference individual metadata symbols. Compiler objects express the same destinations as a zero-sized `...data.0` section-base symbol plus an addend. The individual target metadata objects already have explicit source symbol mappings and exact contents.

| Owner | Original referenced symbols | Compiler section-base addends | Existing compiler object identities |
| --- | --- | --- | --- |
| Message | lbl_803C3BC0, lbl_803C3CB8 | .data + 408, + 656 | @1860, @1861 |
| CPauseMenu shared-pointer RTTI | lbl_803A7840, lbl_803A78D0 | .data + 672, + 816 | @10225, @10226 |
| CPauseMenu bound-function RTTI | lbl_803A78F8, lbl_803A7A60 | .data + 856, + 1216 | @10227, @10228 |
| CUIPiiListItem RTTI | lbl_803AF3CC, lbl_803AF3DC | .data + 60, + 76 | @5087, @5088 |

Each first destination is the recovered type name; each second is the matching twelve-byte base list. CUIPiiListItem is a minimal example: its original eight-byte RTTI symbol lbl_804B75B0 maps to `__RTTI__14CUIPiiListItem`; its first ADDR32 relocation names lbl_803AF3CC, whose mapped compiler symbol @5087 begins at .data offset 60. The source relocation names `...data.0 + 60`. The second pair is identical in form at offset 76. The payloads and effective destinations agree, but the current comparator reports this record at 50%.

These observations were sent to the comparator owner for an explicit destination-equivalence fix. This audit does not modify the comparator or award the outstanding 320 bytes of section credit. A valid comparison must retain relocation type, section identity, mapped symbol identity, and exact relative offset; arbitrary same-valued or shifted addresses do not establish equivalence.

## Message's remaining weekday literals

The V2 comparator candidate resolves the pooled-pointer cases, making CPauseMenu and CUIPiiListItem `.sdata` exact. Message still scored 92.0354% because three weekday fallback strings had narrow-string inferred lengths. Their source declarations in `sWeekdayNames` are wide U+706B, U+6C34, and U+6728, each with a full two-byte terminator.

| Original symbol | Actual frozen bytes | Inferred/actual size | Compiler identity |
| --- | --- | --- | --- |
| lbl_804B871C | 706b0000 | 3 / 4 | @1609 |
| lbl_804B8720 | 6c340000 | 3 / 4 | @1610 |
| lbl_804B8724 | 67280000 | 3 / 4 | @1611 |

Direct reads through the frozen DOL's section table confirm all three complete payloads. The correction sets each size to four, changes its classification to `data:wstring`, and supplies the explicit mapping above. A separate scratch dtk split exits zero; real V2 objdiff then reports Message's extab, extabindex, ctors, data, and all 136 bytes of sdata at 100%, with no nonexact object symbols. The packet is `tools/local/message-sdata-20261105/corrections.json`; source and all split extents remain unchanged. The coordinator owns production comparator publication and final full-artifact acceptance.

The section-level diff's remaining relocation at `.sdata + 120` initially looked like a different type-name pointer. That presentation compared the original default_delete RTTI with an earlier unretained out_of_range RTTI at the same raw section offset. The same-named default_delete RTTI actually resides at source offset 176 and points to the correct 119-byte type name. Its mapped symbol comparison is exact. No type-name content or comparator acceptance rule needed relaxing for this residual case.

## Traps

- A UTF-16 string terminator occupies two bytes. A narrow-string inferred size can stop one byte early despite the complete source literal and final executable being exact. Check source character width and frozen bytes before changing a literal.
- An inferred label inside a compiler literal need not represent a real object. Check the complete original relocation inventory and contiguous source/frozen contents before removing a false boundary.
- Compiler pooled-data relocations can use a section base plus addend while original split relocations name a constituent object. Diagnose the mapped effective destination; direct symbol-name comparison alone can reject identical data.
- A raw section diff can display a relocation from the wrong neighboring source record after dead stripping. Inspect the mapped individual symbol before diagnosing a source-value mismatch; Message's final errors were three undersized wide literals, not its already exact default_delete RTTI.
