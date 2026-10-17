# F-390-1: NW4R g3d `ResMat` unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #390 owns `.text` `0x8024F090–0x8024F764`. Its first function, `ResTev::DCStore`, ends `g3d_restev.cpp` and was matched with #389 ([F-389-1](389-g3d-resshp-restev.md)). The rest begins `g3d_resmat.cpp`. That unit runs through all of #391 (`0x8024F770–0x802507D4`) and into #392, and is now `Matching`.

## Unit and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/res/g3d_resmat.cpp` | `.text` | `0x8024F0B0–0x80250AFC` | 53 functions of `ResMat`, `ResMatPix`, `ResMatTevColor`, `ResMatIndMtxAndScale`, `ResMatChan`, `ResMatTexCoordGen`, `ResTexObj`, `ResTlutObj`, `ResTexSrt`, `ResGenMode`, `ResMatMisc`, `ResMatFur` and `ResTexPlttInfo` |
| | `.data` | `0x803C58D0–0x803C5924` | switch jump table (0x54) |
| | `.sdata2` | `0x804BAA98–0x804BAACC` | nine float and double constants |

The last function is `ResTexPlttInfo::Bind` (`0x80250840`, 0x2BC bytes). `0x80250B00` is `ResVtxPos::SetArray`, which begins `g3d_resvtx.cpp` in #392. The `.sdata2` split ends after the last 4-byte constant at `0x804BAAC8`. The symbol there was resized from 8 to 4 bytes, so the split can end at `0x804BAACC`. objdiff reports 100% for all 53 functions and for the data, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

The source is CC0 zeldaret/ss `src/nw4r/g3d/res/g3d_resmat.cpp` at commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`, stripped of comments. That file is identical to the CC0 Kirby's Return to Dreamland copy (Whippy, `ef8403b7`). Every target function has the same size as the ss function at the same position, and all compile to the target bytes unchanged. Changes:

- `ResMat::IsOpaque` is removed from the source and from `g3d_resmat.h`, because it calls `ResMdl::IsOpaque`, which this revision lacks (F-388-1).
- `GX_DUALMTX0` is the SDK's `GX_PTTEXMTX0` (64).
- The linker stripped 30 functions; the source keeps them. Among them are the const `ResMat::GetParent` and `GetResTev`, `ResTexObj::Invalidate`, `ResTlutObj::Invalidate`, `ResTexSrt::GetEffectMtx` and `GetMapMode`, three of the five `ResGenMode` setters, the `ResMatPix` alpha-compare, z-mode and blend-mode accessors, the `ResMatChan` setters, `GXGetTexCoordGen2`, `ResMat::Release` and `ResTexPlttInfo::Release`.

Equal-sized neighbours needed checking. Matching by size alone first named `0x8024F770` `GXSetNumChans` and `0x80250800`/`0x80250820` `GetResTev const`/`GetResMatFur`. The bodies show otherwise. `0x8024F770` stores to byte 2 of `ResGenModeData`, so it is `GXSetNumTevStages`. `0x80250800` and `0x80250820` read `toResMatFurData` (0x34) and `toResUserData` (0x38), so they are `GetResMatFur` and `GetResUserData`.

## GX hardware definitions

The resource layouts and command writers use over a hundred BP, CP and XF register constants. `include/nw4r/g3d/platform/g3d_gxhardware.h` now holds ss's `GXHardware.h` definitions and includes `g3d_gxhardware_bp.h`, `g3d_gxhardware_cp.h` and `g3d_gxhardware_xf.h`. These three files are ss's `GXHardwareBP.h`, `GXHardwareCP.h` and `GXHardwareXF.h` (CC0, same commit), stripped of comments. The write-gather-pipe declaration and the FIFO macros that use it are removed; the project's SDK headers already provide `GXWGFifo`. `GX_BITSET` and `GX_BITGET` come from ss's `GXTypes.h`. The few macros #386 and #389 defined for themselves remain at the end of `g3d_gxhardware.h`. They do not conflict with the imported names. These headers stay under `nw4r/g3d/platform` so the Petari SDK headers (D-0013) are unchanged.

## Outside symbols

These were renamed in `symbols.txt` so the unit links: `ResTex::GetTexObjParam` `0x80250F70` and `ResTex::GetTexObjCIParam` `0x80251030`, in the order `ResTexPlttInfo::Bind` calls them. Splits and sizes of these symbols are unchanged.

## Traps

- **Equal function sizes do not identify a function.** A size-based alignment against a reference can swap neighbours of the same size. Here three were swapped, and objdiff reported 99.8% instead of 100%. Check the field offsets in each body before accepting a name.
