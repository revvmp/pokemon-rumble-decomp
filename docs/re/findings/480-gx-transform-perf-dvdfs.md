# Issue 480: RVL_SDK GXTransform, GXPerf and dvdfs

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). Evidence comes from the dtk 1.8.4 split assembly `build/wpse-v1/asm/auto_03_802D0A54_text.s`, GNU binutils 2.42 disassembly of the split and compiled objects, and Wii/1.0 `mwcceppc.exe` with `cflags_sdk`. Reference: Petari `e0ab141914921726a595573dc48d3f13d0922efe`, CC0, `src/RVL_SDK/gx/GXTransform.c`, `src/RVL_SDK/gx/GXPerf.c` and `src/RVL_SDK/dvd/dvdfs.c`. None of these files carries a file-level notice.

## F-480-1: Unit boundaries

| Unit | `.text` | Data | Evidence |
| --- | --- | --- | --- |
| `GXTransform.c` | `0x802D0A60`–`0x802D1018` | `.sdata2` `0x804BB3D8`–`0x804BB3E8` | `__GXSetProjection` through `__GXSetMatrixIndex`. The previous unit, `GXDisplayList.c`, ends at `0x802D0A54` |
| `GXPerf.c` | `0x802D1020`–`0x802D1850` | `.data` `0x803D6728`–`0x803D6810` | `GXSetGPMetric` and `GXClearGPMetric`. `.data` holds the metric switch jump tables |
| `dvdfs.c` | `0x802D1850`–`0x802D1A6C` | `.data` `0x803D6810`–`0x803D6840`; `.sdata` `0x804B8B80`–`0x804B8B90`; `.sbss` `0x804B96F8`–`0x804B9710` | `__DVDFSInit` through `DVDGetFSTLocation`. `.data` is the `DVDRead()` panic message, `.sdata` holds `__DVDLongFileNameFlag` and `"dvdfs.c"`, and `.sbss` holds `__DVDThreadQueue`, `MaxEntryNum`, `FstStringStart`, `FstStart` and `BootInfo`. `dvd.c`'s banner follows at `0x803D6840` |

Confidence high. All three units compile to the target's sizes, every function (17 in GXTransform, 2 in GXPerf, 6 in dvdfs) is identical with relocations masked, and the full DOL verification passes.

## F-480-2: This revision's function sets

The target lacks several Petari functions, and their string data is also absent, so these sources leave them out rather than relying on dead-stripping:

- `GXTransform.c` has no `GXProject`, `GXLoadTexMtxIndx`, `GXGetViewportv`, `GXSetZScaleOffset` or `GXGetScissor`. Its `.sdata2` is 16 bytes, not the 20 that `GXProject`'s constant adds.
- `GXPerf.c` has no `GXReadXfRasMetric`.
- `dvdfs.c` has none of the path routines (`DVDConvertPathToEntrynum`, `DVDOpen`, `entryToPath`, `DVDReadAsyncPrio`, the directory routines). Their panic and warning strings would appear in `.data`, which holds only the `DVDRead()` message (48 bytes). `currentDirectory` goes with them, which leaves `.sbss` at 24 bytes.

Petari lacks two functions that the target has. `GXLoadPosMtxIndx` (`0x802D0C00`) and `GXLoadNrmMtxIndx3x3` (`0x802D0C90`) send indexed XF loads with opcodes `0x20` and `0x28`, counts 12 and 9, and addresses `id * 4` and `0x400 + id * 3`. Their names come from the existing declarations in `include/revolution/gx/GXTransform.h`. Confidence high.

`DVDGetFSTLocation` (`0x802D1A60`) returns `BootInfo->FSTLocation`, which places it in `dvdfs.c` because `BootInfo` is local to the unit.

External names added to `symbols.txt` for the link: `DVDCancel` (`0x802D61E0`). It calls the asynchronous cancel and then sleeps on `__DVDThreadQueue` until the block's state settles, which matches the SDK routine that `DVDClose` calls. Its split and size are unchanged. Confidence high.

## Traps

- **A dvdfs-style unit can be a subset of the reference.** The target `.data` and `.sbss` sizes showed which Petari functions this revision never compiled. Compare each data section's size before assuming that the linker removed the extra functions; see F-480-2.
