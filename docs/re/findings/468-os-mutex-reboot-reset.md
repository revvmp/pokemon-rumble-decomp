# Issue 468: RVL_SDK OSMutex, OSReboot and OSReset

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). Evidence comes from the dtk 1.8.4 split assembly `build/wpse-v1/asm/auto_03_802B9F00_text.s`, GNU binutils 2.42 disassembly of the split and compiled objects, and Wii/1.0 `mwcceppc.exe` with `cflags_sdk`. Reference: Petari `e0ab141914921726a595573dc48d3f13d0922efe`, CC0, `src/RVL_SDK/os/OSMutex.c`, `OSReboot.c` and `OSReset.c`. None of these files carries a file-level notice.

## F-468-1: Unit boundaries

| Unit | `.text` | Data | Evidence |
| --- | --- | --- | --- |
| `OSMemory.c` | `0x802B9760`–`0x802B9F00` | owned by #467 | The first nine functions in this issue's table (`0x802B98A0`–`0x802B9EFC`) are OSMemory routines, and #467 reconstructed them (F-467-1) |
| `OSMutex.c` | `0x802B9F00`–`0x802BA314` | none | `OSInitMutex` through `OSSignalCond`. The split carries `align:16`, as F-467-4 asked |
| `OSReboot.c` | `0x802BA320`–`0x802BA3A4` | `.sbss` `0x804B94E8`–`0x804B94F0` | `__OSReboot` and `OSGetSaveRegion`. `SaveStart` and `SaveEnd` |
| `OSReset.c` | `0x802BA3B0`–`0x802BAE2C` | `.data` `0x803C8AA8`–`0x803C8D10`; `.sdata` `0x804B8A90`–`0x804B8A98`; `.sbss` `0x804B94F0`–`0x804B9500` | It starts at `OSRegisterShutdownFunction` and ends at `OSResetSystem`. `.data` is the string pool starting at `"OSReset.c"`. `.sdata` is the empty panic message. `.sbss` holds `bootThisDol`, `__OSIsReturnToIdle` and `ShutdownFunctionQueue`. The next function, `0x802BAE30`, belongs to #469 |

Confidence high. All three units compile to the target's sizes, and every function is identical with relocations masked.

## F-468-2: Names and differences from Petari

Names come from Petari or from identical call sites: `__OSUnlockAllMutex`, `OSTryLockMutex`, `__OSReboot`, `OSGetSaveRegion`, `__OSCallShutdownFunctions`, `__OSGetDiscState`, `OSShutdownSystem`, `OSRestart`, `OSReturnToDataManager`, `OSReturnToSetting`, `__OSReturnToMenuForError` and `__OSHotResetForError`. `OSInitCond` (`0x802BA220`, tail call to `OSInitThreadQueue`), `OSWaitCond` (`0x802BA230`) and `OSSignalCond` (`0x802BA310`, tail call to `OSWakeupThread`) are absent from Petari. They are the public SDK condition-variable routines, and their bodies follow the mutex code's queue and priority handling. Confidence high.

This revision differs from Petari in these ways:

- `__OSGetDiscState` takes Petari's non-RMG branch.
- `OSRestart` discards the result of `OSEnableScheduler`.
- `OSReturnToSetting` takes a signed `int` setting, because the switch compiles to `cmpwi` with no zero extension.
- `__OSReturnToMenu` (local) is newer than Petari's. After writing the state flags it sets the MEM1 arena to `0x81280000`–`0x812F0000` and initializes ESP. It allocates `OSRoundUp32B(sizeof(ESTicketView))` (`0xE0`) bytes from the MEM1 arena and reads the disc ticket view. If play time is limited and none remains, it writes the expired flag. A failure of ESP or of the allocation falls back to `__OSReturnToMenuForError`, which is inlined twice.
- The panic lines are `0x37F`, `0x391`, `0x3CB`, `0x3F2`, `0x40A` and `0x4A1`.
- The string pool also keeps `OSSetBootDol()`'s message, although that function is dead-stripped. The source keeps `OSSetBootDol` as a global function to reproduce it, as F-286-3 found for CNT.

External names added to `symbols.txt` for the link, from their call roles in this unit and Petari's matching call sites, with high confidence: `OSGetCurrentThread` (`0x802BBD90`), `__OSPromoteThread` (`0x802BC0D0`), `__OSGetEffectivePriority` (`0x802BBEE0`), `__OSBootDol` (`0x802B6AF0`), `SCGetIdleMode` (`0x802E9720`), `__OSShutdownToSBY` (`0x802BDCC0`), `OSGetAppType` (`0x802B2F30`), `__OSRelaunchTitle` (`0x802C0040`), `__OSReturnToMenul` (`0x802C07C0`) and `__OSNextPartitionType` (`.sbss` `0x804B94A0`). Their splits and sizes are unchanged.

## F-468-3: Link layout

`OSReset.c`'s code ends at `0x802BAE2C`. The following auto-split object at `0x802BAE30` (#469) has inferred 4-byte alignment, so `fn_802BAE30` gets `align:16` in `symbols.txt` to keep its original start (F-494-2, F-286-3). With that, and with the three units `Matching`, `ninja` exits 0 and `tools/verify/verify.py verify` passes.

`include/revolution/os/OSMutex.h` now includes `OSThread.h` instead of redefining `OSThreadQueue`, and it declares `OSCond` and the condition and try-lock routines. `OSThread.h` gains `__OSPromoteThread` and `__OSGetEffectivePriority`.

## F-468-4: Block completion

The nine `OSMemory.c` functions at the start of this block (`0x802B98A0`–`0x802B9EFC`) now link from source as `Matching` (F-467-6). Every function from `0x802B98A0` to `0x802BAE2C` is therefore in a `Matching`, source-linked unit: `OSMemory.c`, `OSMutex.c`, `OSReboot.c` and `OSReset.c`. `OSReset.c` ends inside this block, so no unit runs into #469. The full build and the frozen-manifest verifier pass.

## Traps

No new reusable trap. The dead-function string pool (F-286-3) and successor alignment (F-467-4) both applied again.
