# Issue 476: RVL_SDK GXMisc and GXGeometry

## F-476-1: unit boundaries, names, and source

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). Tools: decomp-toolkit 1.8.4, binutils 2.42, objdiff-cli 3.8.2, Wii/1.0 `mwcceppc.exe` with `cflags_sdk` (default 16-byte function alignment, `-inline auto,level=3`, `-ipa file`, `-sdata 8`, `-sdata2 8`).

The 27 functions of the issue form two translation units in the reference order of Petari's CC0 `src/RVL_SDK/gx/GXMisc.c` and `GXGeometry.c` (commit `e0ab141914921726a595573dc48d3f13d0922efe`). They build on the shared GX headers from [F-479-1](479-gx-headers.md), plus Petari's `shortcut_pe_misc.h`, the public `GXManage.h`, `GXCpu2Efb.h`, `GXGeometry.h` and `GXCull.h` declarations, and the private `GXState.h` with dirty-state flags, register indices and internal GX declarations.

| Unit | Section | Range, exclusive end | Evidence |
| --- | --- | --- | --- |
| `RVL_SDK/gx/GXMisc.c` | `.text` | `0x802CC630`–`0x802CCE30` | `GXSetMisc` follows the GXAttr routine at `0x802CC600`; `__GXPEInit` ends at `0x802CCE24` and its 16-byte padding precedes `__GXSetDirtyState` |
| `RVL_SDK/gx/GXMisc.c` | `.sbss` | `0x804B96E0`–`0x804B96F4` | `FinishQueue` (8 bytes), `DrawDone` (1 byte, padded), `DrawDoneCB`, `TokenCB`; reverse declaration order, as CodeWarrior emits statics. `__DVDThreadQueue` begins after 8-byte alignment at `0x804B96F8` |
| `RVL_SDK/gx/GXGeometry.c` | `.text` | `0x802CCE30`–`0x802CD434` | `__GXSetDirtyState` through `__GXSetGenMode`; the GXFrameBuf unit starts at `0x802CD440` |

Neither unit has initialized data. Recovered names, by matching instruction sequences against the reference: `__GXAbort` `0x802CC6B0`, `GXAbortFrame` `0x802CC820`, `GXDrawDone` `0x802CC9E0`, `GXPixModeSync` `0x802CCAB0`, `GXPokeAlphaMode` `0x802CCAE0`, `GXPokeAlphaRead` `0x802CCAF0`, `GXPokeAlphaUpdate` `0x802CCB10`, `GXPokeBlendMode` `0x802CCB30`, `GXPokeColorUpdate` `0x802CCB90`, `GXPokeDstAlpha` `0x802CCBB0`, `GXPokeDither` `0x802CCBD0`, `GXPokeZMode` `0x802CCBF0`, `GXSetDrawSyncCallback` `0x802CCC10`, `GXTokenInterruptHandler` `0x802CCC60` (local), `GXSetDrawDoneCallback` `0x802CCCF0`, `GXFinishInterruptHandler` `0x802CCD40` (local), `__GXSetDirtyState` `0x802CCE30`, `GXBegin` `0x802CD0B0`, `__GXSendFlushPrim` `0x802CD200`, `GXSetLineWidth` `0x802CD2E0`, `GXSetPointSize` `0x802CD320`, `GXEnableTexOffsets` `0x802CD360`, `GXSetCullMode` `0x802CD3A0`, `GXSetCoPlanar` `0x802CD3D0`, `__GXSetGenMode` `0x802CD410`. Confidence high.

`GXFlush`, `GXResetWriteGatherPipe`, `GXSetDrawSync`, `GXSetDrawDone`, `GXWaitDrawDone`, `GXTexModeSync`, `GXPeekARGB`, and `GXPeekZ` have no code in the executable. `GXFlush`, `GXSetDrawDone`, and `GXWaitDrawDone` are inlined into `GXAbortFrame` and `GXDrawDone`; keeping all eight in the source still matches, so the linker dead-stripped them. Interpretation: the original source contained them; confidence medium.

This SDK revision differs from the reference in two places: `GXSetDrawDone` clears `bpSentNot` after `OSRestoreInterrupts`, and `GXSetCoPlanar` clears `bpSentNot` after its two register writes.

## F-476-2: source-link evidence and external names

With the fallback objects, both units score 100% in the objdiff report (GXMisc 18/18 functions, 1,900 code bytes; GXGeometry 9/9 functions, 1,488 code bytes) and `tools/verify/verify.py verify` passes.

In a local experiment that was not committed, both units were set to `Matching` and eleven external labels outside this claim were renamed in `symbols.txt`: `fn_802BCF60` `OSWakeupThread`, `fn_802CB440` `__GXIsGPFifoReady`, `fn_802CB520` `__GXCleanGPFifo`, `fn_802CB930` `__GXSetVCD`, `fn_802CB9E0` `__GXCalculateVLim`, `fn_802CC080` `__GXSetVAT`, `fn_802CF270` `__GXSetSUTexRegs`, `fn_802CFB20` `__GXUpdateBPMask`, `fn_802D0A60` `__GXSetProjection`, `fn_802D0DB0` `__GXSetViewport`, `fn_802D0F90` `__GXSetMatrixIndex`. Call order in `__GXSetDirtyState` and `GXAbortFrame` identifies each one. The source-linked DOL then passed full frozen-manifest verification. The units stay `NonMatching` until the owners of those addresses land the names; then only the `Matching` switch and a rebuild are needed.

## F-476-3: split end padding

The first source-linked attempt ended GXGeometry's `.text` at the next unit's start, `0x802CD440`. The compiled object ends at `0x802CD434`, and the following auto-split object has 4-byte alignment, so everything after moved 0x10 bytes earlier and verification failed at DOL header byte `0x6F`. Ending the split at the last function's end, `0x802CD434`, leaves the padding in the next auto split and verifies. objdiff reports 100% either way. Confidence high.
