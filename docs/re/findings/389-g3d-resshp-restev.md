# F-389-1: NW4R g3d `ResShp` and `ResTev` units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #389 owns `.text` `0x8024D710–0x8024F084`. Its first part, `0x8024D710–0x8024E0F8`, ends `g3d_resmdl.cpp` and was matched with #388 ([F-388-1](388-g3d-resmdl.md)). This change reconstructs the rest: all of `g3d_resshp.cpp` and `g3d_restev.cpp`. The last function of `g3d_restev.cpp` is listed in #390. Both units are `Matching`.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/res/g3d_resshp.cpp` | `.text` | `0x8024E100–0x8024E9C4` | `ResShp::GetResVtxPos`, `GetResVtxNrm`, `GetResVtxClr`, `GetResVtxFurPos`, `Init`, `Terminate`, `CallPrePrimitiveDisplayList`, `CallPrimitiveDisplayList` |
| `nw4r/g3d/res/g3d_restev.cpp` | `.text` | `0x8024E9D0–0x8024F0A8` | `ResTev::GXSetTevAlphaIn`, `GXSetTevAlphaOp`, `GXSetTevKColorSel`, `GXGetTevOrder`, `GXSetTevOrder`, `GXSetTevColorIn`, `GXSetTevColorOp`, `SetNumTevStages`, `CallDisplayList`, `CopyTo`, `DCStore` |
| | `.rodata` | `0x8036C690–0x8036C6F0` | `r2c` (RAS channel to `GXChannelID`, 0x20) and `dlsize` (display-list size per stage count, 0x40) |
| | `.data` | `0x803C58C0–0x803C58D0` | `c2r` (`GXChannelID` to RAS channel, 0x10) |

`ResTev::DCStore` at `0x8024F090` (0x18 bytes) is listed in #390 but ends `g3d_restev.cpp`. objdiff reports 100% for all 19 functions and for the data, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Sources

- `g3d_resshp.cpp` follows CC0 zeldaret/ss at commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`. `GetParent`, `GXSetArray` and `DisableSetArray` are inlined everywhere; the source keeps their definitions. The target has no copy of `GXGetVtxDescv` or `GXGetVtxAttrFmtv`, so the source omits them. `GetResVtxTexCoord(u32)`, `ResShpPrePrim::DCStore` and `ResShp::DCStore` are kept, although the linker stripped them.
- `g3d_restev.cpp` follows the CC0 Kirby's Return to Dreamland decompilation (Etrainquandt17/Whippy, commit `ef8403b7150210639ad3a0683cd3f86e6d03dee5`). Its `ResTev` has the alpha, konstant-color, color-op and stage-count setters that ss and ogws lack. The 2009 target has the same functions in the same order. The getters and setters absent from the target are omitted, except `GXSetTevSwapMode` (see below).

## Evidence that changed the source

- **A stripped function shapes the next one.** `GXSetTevAlphaIn` compiled to the right instructions, but 13 words were out of order. With `GXSetTevSwapMode` defined before it, as in the Kirby source, every word matches. No ordering of its own expressions changed the schedule. `GXSetTevSwapMode` is absent from the target, so the original file defined it and the linker removed it.
- **`GXSetTevOrder` must stay out of line in `SetNumTevStages`.** Without the release-build assertions, `-ipa file` inlines it, and `SetNumTevStages` grows from 0xEC to 0x168 bytes. Two `NW4R_ASSERT` statements, which expand to `((void)0)`, raise its cost enough to keep the call. The Kirby source does the same with two empty statements. The new `include/nw4r/db/db_assert.h` defines the release forms of `NW4R_ASSERT` and `NW4R_ASSERT_VALID_PTR`. The two asserted conditions are plausible stage and coordinate range checks, not recovered text.
- `SetNumTevStages` clears unused stage blocks with `detail::ZeroMemory16ByteBlocks` (`0x8025B610`, paired-single stores of 16 bytes). `CopyTo` uses `detail::Copy32ByteBlocks` (`0x8025B460`).

## Header changes

- `g3d_restev.h` declares the five new setters, `GXSetTevSwapMode` and `SetNumTevStages`. `GXSetTevAlphaIn` takes `GXTevAlphaArg`.
- `include/nw4r/g3d/platform/g3d_cpu.h` comes from ss with its `DC` wrappers and block-copy declarations, plus `ZeroMemory16ByteBlocks`. Its fast-cast helpers are left out: the project's `OSFastCast.h` lacks `OSu8tof32`, `OSSetGQR6` and the GQR type enum.
- `g3d_gxhardware.h` gains the CP array registers and the BP register numbers and field shifts for RAS1_TREF, the TEV color and alpha combiners and TEVKSEL. The values agree with the ss `GXHardwareBP.h` and with the target's immediates.
- `include/revolution/gx/GXDispList.h` gains the SDK inline `GXFastCallDisplayList`, which writes the call-display-list command (0x40), address and size to the write-gather pipe. `ResShp::CallPrePrimitiveDisplayList` inlines that sequence at `0x8024E8D8`.

## Outside symbols

These were renamed in `symbols.txt` so the units link: `GetArray` of `ResVtxPos` `0x80250B40`, `ResVtxNrm` `0x80250C20`, `ResVtxClr` `0x80250D00` and `ResVtxTexCoord` `0x80250DA0`; `detail::Copy32ByteBlocks` `0x8025B460`; and `detail::ZeroMemory16ByteBlocks` `0x8025B610`. Splits and sizes are unchanged.

## Traps

- **With `-ipa file`, a stripped function can change the schedule of the function after it.** If a function has the right instructions in the wrong order and no expression order fixes it, add the neighbouring functions that the reference defines but the linker removed. Here `GXSetTevSwapMode` fixed `GXSetTevAlphaIn`.
- **Release assertions can keep a call out of line.** If `-ipa file` inlines a callee that the target calls, look for the reference's assertions. In release builds they expand to `((void)0)`, but they still raise the inlining cost.
