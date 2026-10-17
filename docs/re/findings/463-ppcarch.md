# Issue 463: RVL_SDK PPCArch

## F-463-1: PPCArch unit boundaries and source

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The dtk 1.8.4 split assembly in `build/wpse-v1/asm/auto_03_802ABD4C_text.s` and `auto_07_80399380_data.s` shows 23 functions from `PPCMfmsr` at `0x802B1800` to `PPCMthid4`, which ends at `0x802B1A10`, where `__OSFPRInit` (OS) begins. The only data reference is `PPCMthid4`'s `OSReport` string at `0x803C7280`, 0x37 bytes plus one padding byte, followed at `0x803C72B8` by the RVL_SDK OS build banner. The unit therefore owns `.text` `0x802B1800`–`0x802B1A10` and `.data` `0x803C7280`–`0x803C72B8`, matching the issue's estimated block exactly.

Function order follows Petari's CC0 `src/RVL_SDK/base/PPCArch.c` (commit `e0ab141`). `fn_802B1830` (`mtspr HID0, r3`) is `PPCMthid0`, called by `PPCDisableSpeculation`, and `fn_802B1870` (`sc`) is `PPCSync`. Petari's `PPCMfwpar` would sit between `PPCMthid2` and `PPCMtwpar`; the original has no code there, and keeping it in the source still links byte-identically, so the linker dead-stripped it. Interpretation: the original source contained it; confidence medium.

Compiled with Wii/1.0 and Petari's RVL_SDK flags (`cflags_sdk` in `configure.py`), all 23 functions match in objdiff 3.8.2 and `tools/verify/verify.py verify` passes with the unit `Matching`.

## F-463-2: CodeWarrior inline-assembler SPR names

Wii/1.0 `mwcceppc.exe` rejects `WPAR` and `HID4` as SPR names (error 33067) and encodes `HID2` as SPR 1011, not the Broadway HID2 at 920. A one-line test `mfspr r3, HID2` assembled to `7C 73 FA A6` (`mfspr r3, 1011`), confirmed with GNU objdump 2.42. `HID0`, `L2CR`, `MMCR0`, `MMCR1`, and `PMC1`–`PMC4` encode correctly. Use numeric SPRs for HID2 (`0x398`), WPAR (`0x399`), and HID4 (`0x3F3`). Confidence high.
