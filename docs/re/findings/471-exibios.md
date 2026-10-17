# Issue 471: EXIBios and the OSPlayTime and OSLaunch block

## F-471-1: `EXIBios.c` matches at `-O3` with 16-byte function alignment

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/exi/EXIBios.c` (CC0).

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x802C0AC0`-`0x802C23B8` | `SetExiInterruptMask` through `EXIGetID`, 19 functions; `CompleteTransfer` and `__EXIAttach` are inlined |
| `.data` | `0x803D51D0`-`0x803D5216` | The EXI version string |
| `.bss` | `0x80453710`-`0x804537D0` | `Ecb` |
| `.sdata` | `0x804B8AD0`-`0x804B8AD4` | `__EXIVersion`, global |
| `.sbss` | `0x804B9590`-`0x804B9594` | `IDSerialPort1` |

The unit uses Petari's source with two changes. The version string becomes `<< RVL_SDK - EXI \trelease build: Feb 27 2009 10:02:03 (0x4302_145) >>`, as in the target. `__EXIVersion` loses `static`, because the target symbol is global.

With the library flags (`-O4,p`), all 19 functions differ and score 50% to 89%; the unit totals 68%. No compiler from GC/3.0a3 to Wii/0x4201_127 changes this. Petari builds this file with `-O3` (`cflags_sdk_exi`). With `-O3` every function and data section scores 100%.

`-O3` also drops the function alignment to 4 bytes. The object then comes out 0x60 bytes shorter than its split, and full verification fails at DOL header byte `0x1F`, even though objdiff reports 100%. Adding `-func_align 16` restores the Wii/1.0 default alignment. `ninja` then exits 0 and `tools/verify/verify.py verify` passes. `cflags_sdk_exi` in `configure.py` records both flags.

## F-471-2: Remaining functions of the block

- **`0x802BF1E0`-`0x802BF998`, seven functions:** these belong to `OSPlayTime.c`. That file starts in #470's block with `OSPlayTimeIsLimited` at `0x802BF1C0`, so #470 owns its split.
- **`0x802BF9A0` (`0x134`):** a nibble-table CRC-32. It uses the `0x40`-byte table at `.rodata` `0x80371630`, and only game code calls it (`0x80106D70`, `0x80107CA0`). No reference names it, and nothing shows which unit it belongs to, so it is left unsplit.
- **`0x802BFAE0`-`0x802C09F4`, OSLaunch:** Petari's `OSLaunch.c` reproduces nine of the ten functions at 97.6% to 100% once named: `PackArgs`, `__OSCheckCompanyCode`, `__OSCheckTmdSysVersion`, `__OSGetValidTicketIndex`, `__OSRelaunchTitle`, `__OSLaunchTitle`, `LaunchCommon` and `__OSReturnToMenul`. The tenth, `0x802C08D0`, is `OSLaunchManualViewer`, which Petari lacks.
  - The unit's string pool (`.data` `0x803D4EB0`-`0x803D51D0`) also holds the texts of `__OSLaunchTitlevForSystem`, `__OSLaunchTitlelForSystem` and `OSLaunchPDChannel`. Those functions were dead-stripped, and no reference source for them is available.
  - The pool cannot match without them, so the OSLaunch split is left for a later issue. A CC0 reconstruction of the same SDK file, `InusualZ/mhtri-decomp` `src/OS/OSLaunch.c`, records the same blocker.

## F-471-3: OSCrc and OSLaunch

This supersedes the "left unsplit" and "left for a later issue" notes in F-471-2.

- **`OSCrc.c`, `.text` `0x802BF9A0`-`0x802BFAD4`, `.rodata` `0x80371630`-`0x80371670`.** xbret/xenoblade links `OSCrc.c` between `OSPlayTime.c` and `OSLaunch.c`, which is where this function sits, and its `OSCalcCRC16` uses the same nibble-table structure. The function is `OSCalcCRC32`: initial value `0xFFFFFFFF`, two table lookups per byte, and the result inverted. The table is the reflected CRC-32 nibble table. `length` is signed, because the target tests it with `cmpwi`. `OSCalcCRC16` has no code or table in the target, so it is not reconstructed. The name is medium-high confidence.
- **`OSLaunch.c`.** Petari's source, with two corrections, matches all nine functions:
  - `PackArgs` loads each argument once, as OSExec's copy does.
  - The `__OSGetValidTicketIndex` and `LaunchCommon` diagnostics use lines 407, 803 and 811.
  - `OSLaunchManualViewer(const char*)` is written from the target. It rejects disc applications (panic at line 1411 with an empty message), builds a one-entry `argv` from its argument, picks the region's `HABA`, `HABK` or `HABC` title, and calls `LaunchCommon` with launch code 2.
- **OSLaunch string pool.** Following F-286-3, global functions whose code the linker strips supply the remaining strings, in pool order:
  - `__OSLaunchTitlevForSystem` and `__OSLaunchTitlelForSystem` come from koopthekoopa/wii-ipl (CC0) `OSExec.c`.
  - `OSLaunchShopChannelHelp` (the `/startup?initpage=showHelp` URL) follows the later-SDK function that InusualZ/mhtri-decomp (CC0) records under that guessed name.
  - `OSLaunchPDChannel`'s body is plausible but unverifiable; only its five strings are evidence.

  The pool then matches byte for byte.
- **Boundaries.** The OSLaunch `.data` ends at `0x803D51CA` and `.sdata` at `0x804B8AC9`. The bytes up to `0x803D51D0` and `0x804B8AD0` are padding before EXIBios.

With both units `Matching`, objdiff reports 100% for every function and section, `ninja` exits 0, and `tools/verify/verify.py verify` passes.

## Provenance

`src/RVL_SDK/exi/EXIBios.c` is Petari's CC0 source at commit `e0ab141` with the changes above. The declarations added to `include/revolution/exi.h` (`EXIImmEx`, `EXIDma`, `EXIClearInterrupts`, `EXISetExiCallback`) follow Petari's `libs/RVL_SDK/include/revolution/exi.h`.

## Traps

- **`-O3` drops function alignment to 4 bytes.** A unit built with Petari's `cflags_sdk_exi` (`-O3`) scores 100% in objdiff but links 0x60 bytes short. Add `-func_align 16` to any `-O3` SDK unit whose target functions start on 16-byte boundaries. Evidence: F-471-1.
- **Dead-stripped functions can still block a split.** OSLaunch's text nearly matches, but its string pool keeps the diagnostics of functions the linker removed. Check the whole `.data` pool against the reference before investing in the text. Evidence: F-471-2.
- **Raw newlines inside generated string literals compile to `0x0D`.** A scripted edit left real line breaks inside three `OSReport` literals. CodeWarrior accepted them and emitted carriage returns, and `.data` fell to 99.25%. Check generated literals for `\n` escapes. Evidence: F-471-3.
