# Issue 503: RVL_SDK WPADMem.c and debug_msg.c

## F-503-1: Unit boundaries and sections

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The block `0x803039E0`–`0x803067F4` holds three translation units: WPADMem.c, debug_msg.c and the start of KPAD.c.

| Unit | Section ranges | Evidence |
| --- | --- | --- |
| `RVL_SDK/wpad/WPADMem.c` | `.text` `0x803039E0`–`0x803045FC`; `.bss` `0x8047ACA8`–`0x8047ACD0` | All 13 functions use the memory block in the WPAD control block (`0x998`–`0x9A8`) or the game info at offset 0. `.bss` holds the 17-character game title (0x22 bytes, padded to 0x28) that `WPADSetGameTitleUtf16` writes and `WPADWriteGameData` copies |
| `RVL_SDK/wpad/debug_msg.c` | `.text` `0x80304600`–`0x80304650` | `DEBUGPrint`: an empty variadic function. Only the register save area is left |
| `RVL_SDK/kpad/KPAD.c` (not split here) | `.text` from `0x80304650` | `KPADSetPosParam`-style setters index `0x8047ACD0` by `chan * 0x578`, the KPAD per-channel status. The `.data` version string reads `<< RVL_SDK - KPAD \trelease build: Jun 22 2009 18:32:13 (0x4302_145) >>`. KPAD continues past the block end into #504 (calls and `.bss` up to about `0x803086D0`) |

Both new units compile with Wii/1.0 and `cflags_sdk` at default alignment. They link from source, and full DOL verification passes. The `.bss` split ends at `0x8047ACD0`, where the existing `lbl_8047ACA8` object ends; the six bytes of padding after the title did not shift the link.

## F-503-2: Function names

Public names are high confidence: SDK convention, the WPAD.c callers (`WPADiClearMemBlock`, `WPADWriteExtReg`), and the WPAD control block fields they touch. Static names are medium confidence; they follow robojumper's sdk_2009-12-11 reconstruction (unlicensed; read only as a naming aid).

| Address | Name | Evidence |
| --- | --- | --- |
| `0x803039E0` | `WPADiClearMemBlock` | Already named (F-501-2); clears `memBlock` |
| `0x80303A10` | `__wpadResultCallback` | Clears `memBlock.used` and `memBlock.callback`, then calls the saved callback |
| `0x80303A50` | `__wpadWriteCallback` | Sends the next 16-byte chunk, retries on `WPAD_ERR_BUSY`, otherwise finishes |
| `0x80303BA0` | `__wpadWriteGameDataSub` | Last write of the game info; checks both copies and starts the data write |
| `0x80303CC0` | `__wpadWriteCheck1` | Records the first game info copy's result in `gameInfoErr[0]` |
| `0x80303D00` | `__wpadWriteCheck2` | Same for `gameInfoErr[1]` |
| `0x80303D40` | `WPADWriteGameData` | Fills `gameInfo` (title, game code, `OSGetTime`, type, checksum `+0x55`), then writes it twice at `0x2A` and `0x62` before the data at `addr + 0x9A` |
| `0x803040F0` | `WPADReadGameData` | Requires a valid game info copy (`WPAD_ERR_BROKEN`) and a matching game code (`WPAD_ERR_NOPERM`) |
| `0x80304230` | `WPADReadMemoryAsync` | Status and setup check, then `WPADiSendReadData` |
| `0x80304300` | `WPADWriteMemoryAsync` | Status and setup check, then `WPADiSendWriteData` |
| `0x803043D0` | `WPADGetGameTitleUtf16` | Returns the stored title or `WPAD_ERR_INVALID` |
| `0x80304470` | `WPADSetGameTitleUtf16` | Copies up to 16 UTF-16 units and terminates |
| `0x80304530` | `WPADWriteExtReg` | Already named (F-501-2); `addr | 0x04A40000` |
| `0x80304600` | `DEBUGPrint` | Already named (F-500-2) |

`__wpadSendWriteMemory` is inlined at every use. `WPADGetGameDataTimeStamp`, the face data functions and `WPADReadExtReg` have no copy in the DOL; the linker removed them, so the source omits them.

## F-503-3: Code generation notes

- The memory-access functions need the four locals `success`, `enabled`, `setup` and `status = WPAD_ERR_NONE`. Without `success`, the function is small enough to be auto-inlined into `WPADWriteGameData`, and its status and setup registers swap.
- `WPADWriteGameData` declares `enabled` before the control block pointer, and `WPADReadGameData` declares `status` before `enabled`. Both keep their success path inside `if (!p_wpd->memBlock.used)`, so the busy result is placed last.
- `__wpadWriteGameDataSub` repeats the `gameInfoErr[1]` check inline. Calling `__wpadWriteCheck2` produces the same instructions with the control block and channel registers swapped.

## Source provenance

Written from the target disassembly. Swiftshine/key (CC0) supplied the `WPADiClearMemBlock` and `WPADWriteExtReg` structure; the Petari and pik2wii WPADMem.c files only declare data. robojumper's sdk_2009-12-11 was read only for names; no text was copied.

## Traps

- **An inlining difference can look like a whole-function mismatch.** `WPADWriteGameData` first compiled to 477 instructions against 233, because the smaller `WPADWriteMemoryAsync` was inlined eight times. Check the callees in the target before rewriting the caller.
