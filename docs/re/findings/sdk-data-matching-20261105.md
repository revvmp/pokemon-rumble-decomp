# SDK object-data matching audit, 2026-11-05

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

## Scope and starting evidence

The accepted full executable and source selection do not imply 100% pre-link object data matching. The initial report records 1,245,840 of 1,788,360 data bytes matched (69.663826%). Of these, the 412 RVL_SDK units account for 933,144 of 959,948 matched data bytes. Their remaining 26,804 bytes are object-comparison discrepancies, not evidence that the already verified final executable contains different data.

This audit reads the current compiled ELF objects, original split ELF objects, source declarations, relocation records, frozen main DOL, and ordinary objdiff report. It does not change source bytes, expected hashes, firmware inputs, or selected compiler objects. Configuration proposals are delivered to the coordinator; only that coordinator regenerates original split objects, runs Ninja, and performs final acceptance.

## False data relocations

Two inferred PowerPC pointers are actual non-pointer data:

| Address | Frozen bytes | Actual meaning | Incorrect inferred relocation |
| --- | --- | --- | --- |
| `0x803DD5D4` | `80 1D 0E B0` | Four entries in WPADEncrypt unsigned-byte substitution table `tb` | `CLayout::ReplaceQueuedMotions + 0x20` |
| `0x803DF1FC` | `80 01 01 38` | Little-endian Thumb instructions `lsls r0, r0, #6` and `subs r0, #1` in the WUD controller reset delay | `@etb_80010138` |

WPADEncrypt source `.data` has no relocations. WUD source and original agree on the nine real security-event switch-table relocations; only the spurious firmware relocation differs. The original DOL SHA-256 independently remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The machine-readable receipt is `tools/local/data-match-20261105/sdk-relocations.json`.

## Symbol extents and alignment

Twenty-six SDK sections contain an exact source byte prefix followed only by original zero alignment. Together their tails contain 119 bytes. Five BTE sections are owned by the separate BTE audit; the remaining 21 sections contain 97 zero bytes. Every proposed tail correction has a compiler-object extent and a measured following native-object alignment. The records are `sdk-zero-tails.json` and `sdk-padding-evidence.json` in the same diagnostic directory.

The first proposal shortened split extents and assigned zeros to native alignment. A stronger diagnostic instead adjusts only the incorrect final symbol size while preserving every section byte, section extent, and progress denominator. Isolated copies of target objects received only ELF symbol-size changes, then ordinary objdiff-cli generated an independent report against unchanged source objects. This is diagnostic metadata substitution, not a production build method. Production acceptance must regenerate target objects from corrected `symbols.txt` through dtk.

The 21 symbol-size corrections cover 17 units. Their isolated report retains all 389 exact functions and all 31,448 data bytes in the denominator, raising matched data to 30,760 bytes. Corrected tail sections match, except WPAD small BSS still exposes three unrelated incorrect array extents. HBMFrameController data and DVD BSS also remain separate from the tail corrections. Receipts are `sdk-symbol-size-corrections.json` and `sdk-symbol-only/report.json`. The earlier `sdk-padding-bss.patch` is superseded as a general integration proposal; it must not be applied wholesale.

One production dtk regeneration rejects the dvderror string reduction from sixteen to fifteen bytes: its inferred `gap_07_803D6C7B_data` extends outside the 44-byte section (`48 > 44`). The coordinator reverted this individual correction. Actual source data ends at `0x803D6C7B`; the following dvdFatal source object begins at `0x803D6C80` with native alignment eight. That establishes the five-byte alignment role independently, but a production boundary or explicit-gap solution still needs integration and frozen verification. The isolated size-only report does not establish dtk acceptance.

### Bounded hidden-gap experiment

The actual pinned dtk supports an explicit `hidden` attribute. Private configuration declared the one-byte gaps at `0x803D6C7B` and `0x803A8B6B` with size one, local scope, alignment one and hidden visibility. Full `dtk dol split --no-update -j 4` then exited zero and produced 897 objects without changing either section extent. Objdump independently confirms the generated ELF gap symbols have hidden visibility. This restores valid target generation while retaining the existing denominator.

Hidden visibility does not by itself establish a matched source byte: the independent objdiff report still scores dvderror `.data` at 98.85057%. Declaring a zero-sized boundary symbol is also ineffective: dtk infers its extent back to one byte. The private receipts are `sdk-gap-probe/hidden-report.json` and `sdk-gap-probe/zero-report.json`. These experiments do not justify changing the expected executable, suppressing comparison bytes, or claiming full data matching before the final report verifies it.

## Recovered SDK storage boundaries

DVD low-level BSS has typed, 32-byte-aligned storage whose target inference merged unrelated objects. The source and exact target accesses identify:

| Address | Object | Bytes |
| --- | --- | ---: |
| `0x80459280` | `registerBuf` | 32 |
| `0x804592A0` | `ioVec` | 80 |
| `0x80459300` | `lastTicketError` | 32 |
| `0x80459320` | `coverStatus` | 32 |
| `0x80459340` | `coverRegister` | 32 |
| `0x80459360` | `statusRegister` | 32 |
| `0x80459380` | `controlRegister` | 32 |

The previous 192-byte symbol at `0x804592A0` included the 80-byte vector, sixteen alignment bytes, and three following objects. All remain ordinary source storage. The eight-byte inferred object at `0x803E9BC8` is the one-byte compiler guard for lyt_common's local `texCoords`, followed by seven alignment bytes; the actual coordinate object is 32 bytes at `0x803E9BD0`.

WPAD's `_wpadExtCnt`, `_wpadRumbleCnt`, and `_wpadIsUsedChannel` are four-byte per-controller arrays, although target metadata described each as one byte and three padding bytes. The source declarations and channel-indexed accesses prove the four-byte extent. WUD's final string pool ends one byte before the inferred section end. These proposals are in `sdk-extra-symbol-corrections.json`.

## Additional WPAD closure

After the false relocation is removed, WPADEncrypt's entire original data prefix equals the source. Its final debug string is at source offset `0x1308`, size `0x1F`, ending at `0x1327`; the old `0x1328` inferred encompassing symbol includes one alignment byte. The follow-up changes only that symbol extent.

WPADHIDParser's BSS at `0x8047AC40` contains five consecutive `f32[4]` arrays: `_wpadCalibrationX`, `_wpadCalibrationY`, `_wpadCenterX`, `_wpadCenterY`, and `_wpadRolag`. The original 80-byte inferred symbol merged these exact 16-byte objects. `_wpadExtData` at `0x8047AC90` is `u8[21]`, followed by three alignment bytes. WPADMem's `_wpadGameTitle` at `0x8047ACA8` is `u16[17]`, followed by six alignment bytes. Their exact source definitions, accesses, and compiler symbols establish the boundaries; `sdk-wpad-corrections.json` records the coordinator proposals.

## Retained WUD small data

WUD source `.sdata2` starts with two pointers in the discarded `gWudUnlinkedCallbackMessages` table. Its retained four-byte `Wii` string starts at source offset eight, while target data starts with the string at offset zero. The following default class bytes `00 04 48` form one three-byte compiler literal; target analysis split it into a halfword at `0x804BB5DC` and a byte at `0x804BB5DE` because of the consuming load widths. The original and source relocations prove the latter byte is the same object plus two. This requires truthful symbol grouping and correspondence, not a replacement value or a source initializer change.

## Firmware coverage

The AX DSP, OS DSP initializer, and WUD firmware already build through source generators. AX host storage is `u16 axDspSlave[4096]`; OS storage is `u16 DSPInitCode[]` from the generated include; WUD uses generated typed download/install records. The OS and WUD original-language limits remain exactly the two approved D-0017 exceptions. This audit found no new embedded-code dependency. The WUD false relocation is an analysis error inside already reconstructed Thumb instructions.

## Traps

- Pointer-shaped instruction bytes can survive a source-complete executable build as false original-object relocations. Confirm both the source consumer and the original frozen bytes before blocking the relocation; the WUD pair is Thumb code, not a PowerPC exception pointer.
- A symbol-size-only objdiff diagnostic can pass while dtk rejects regenerated target layout. The dvderror one-byte tail requires actual dtk validation; do not infer production acceptance from edited scratch metadata.
- Channel arrays and static guards can be mis-sized despite identical BSS storage. Use exact source declarations and target accesses to separate data from alignment, preserving the full data denominator whenever possible.
