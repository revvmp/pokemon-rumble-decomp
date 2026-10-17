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

In a local experiment that was not committed, both units were set to `Matching` and eleven external labels outside this claim were renamed in `symbols.txt`: `fn_802BCF60` `OSWakeupThread`, `fn_802CB440` `__GXIsGPFifoReady`, `fn_802CB520` `__GXCleanGPFifo`, `fn_802CB930` `__GXSetVCD`, `fn_802CB9E0` `__GXCalculateVLim`, `fn_802CC080` `__GXSetVAT`, `fn_802CF270` `__GXSetSUTexRegs`, `fn_802CFB20` `__GXUpdateBPMask`, `fn_802D0A60` `__GXSetProjection`, `fn_802D0DB0` `__GXSetViewport`, `fn_802D0F90` `__GXSetMatrixIndex`. Call order in `__GXSetDirtyState` and `GXAbortFrame` identifies each one. The source-linked DOL then passed full frozen-manifest verification. `__GXIsGPFifoReady` and `__GXCleanGPFifo` have since been named on `main` by the GXFifo unit. The units stay `NonMatching` until the owners of the other nine addresses land the names; then only the `Matching` switch and a rebuild are needed.

## F-476-3: split end padding

The first source-linked attempt ended GXGeometry's `.text` at the next unit's start, `0x802CD440`. The compiled object ends at `0x802CD434`, and the following auto-split object has 4-byte alignment, so everything after moved 0x10 bytes earlier and verification failed at DOL header byte `0x6F`. Ending the split at the last function's end, `0x802CD434`, leaves the padding in the next auto split and verifies. objdiff reports 100% either way. Confidence high.

## F-476-4: external names and source-linked match

On `main` at `e2105cd`, `OSWakeupThread`, `__GXIsGPFifoReady` and `__GXCleanGPFifo` were already named. The remaining eight labels were renamed in `config/wpse-v1/symbols.txt`:

| Address | dtk size | Name | Evidence |
| --- | --- | --- | --- |
| `0x802CB930` | 0xB0 | `__GXSetVCD` | First of the three vertex-format calls in `__GXSetDirtyState`, under the VCD dirty bit; sits in the GXAttr range before `__GXCalculateVLim`, as in Petari `GXAttr.c` |
| `0x802CB9E0` | 0x12C | `__GXCalculateVLim` | Third vertex-format call, under the vertex-limit condition; follows `__GXSetVCD` in GXAttr |
| `0x802CC080` | 0x80 | `__GXSetVAT` | Second vertex-format call, under the VAT dirty bit; last GXAttr routine before the one at `0x802CC600` |
| `0x802CF270` | 0x164 | `__GXSetSUTexRegs` | First call in `__GXSetDirtyState`, under the SU texture-size dirty bit; GXTexture range |
| `0x802CFB20` | 0x4 | `__GXUpdateBPMask` | Second call in `__GXSetDirtyState`; a single `blr`, matching the empty Wii body in Petari `GXTexture.c` |
| `0x802D0A60` | 0x44 | `__GXSetProjection` | Last call in `__GXSetDirtyState`, under the projection dirty bit; GXTransform range |
| `0x802D0DB0` | 0x90 | `__GXSetViewport` | Call under the viewport dirty bit; GXTransform range |
| `0x802D0F90` | 0x88 | `__GXSetMatrixIndex` | The two calls with `GX_VA_PNMTXIDX` and `GX_VA_TEX4MTXIDX` arguments under the matrix-index dirty bits; GXTransform range |

Both units were then set to `Matching` in `configure.py`. After deleting `build/`, `python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exited 0 (`PASS main.dol`). Because the source objects now resolve every branch to these names and the linked DOL is byte-identical, each call target is confirmed. The objdiff report shows `GXMisc` 100% (18 functions) and `GXGeometry` 100% (9 functions). Only the names changed; splits and sizes stay with the GXAttr, GXTexture and GXTransform issues. Confidence high.

## Traps

- **End a split at its last function, not at the next unit.** A GXGeometry split that ended at the next unit's start scored 100% in objdiff, but the source-linked DOL shifted by 0x10 because the next auto split has 4-byte alignment. End `.text` and `.sbss` at the true end of the unit's contents. Evidence: F-476-3.
