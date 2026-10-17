# F-392-1: NW4R g3d `ResVtx` and `ResTex` units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #392 owns `.text` `0x802507E0–0x80251114`. Its first four functions end `g3d_resmat.cpp` and were matched with #390 and #391 ([F-390-1](390-g3d-resmat.md)). The rest is two complete units, and the second one ends with the first function of #393.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/res/g3d_resvtx.cpp` | `.text` | `0x80250B00–0x80250F18` | 17 functions: `SetArray`, `GetArray`, `CopyTo` and `DCStore` of `ResVtxPos`, `ResVtxNrm` and `ResVtxClr`; `GetArray` and `DCStore` of `ResVtxTexCoord`; `ResVtxFurPos::SetArray(u16)` and `DCStore`; `ResVtxFurVec::DCStore` |
| `nw4r/g3d/res/g3d_restex.cpp` | `.text` | `0x80250F20–0x8025112C` | 6 functions: `ResPltt::DCStore` and `CheckRevision`; `ResTex::GetTexObjParam`, `GetTexObjCIParam`, `CheckRevision` and `Init` |

Neither unit has data. Each `.text` split ends at its last function, leaving the alignment padding to the next object. `ResTex::Init` at `0x80251120` is the first function of #393. It ends `g3d_restex.cpp`, so this change splits it and #393 continues at `0x80251130`. objdiff reports 100% for all 23 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

Both files are CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` `src/nw4r/g3d/res/g3d_resvtx.cpp` and `g3d_restex.cpp`, stripped of comments. Every function compiles to the target bytes unchanged, with one addition. This revision defines `ResPltt::CheckRevision` and `ResTex::CheckRevision` out of line in `g3d_restex.cpp`, between the functions ss places there: `ResPltt`'s after `ResPltt::DCStore` and `ResTex`'s before `ResTex::Init`. Both accept revision 3 or revision 1, so the headers declare `COMPATIBLE_REVISION = 1` beside `REVISION = 3`. ss and ogws define both as inline equality tests against `REVISION`. The target `ResMdl::CheckRevision` remains an inline equality test (F-388-1).

## Traps

- **A unit can end with the first function of the next block issue.** `g3d_restex.cpp` ends with `ResTex::Init` at `0x80251120`, the first function of #393. Before planning a block, read the symbols immediately after its end address.
