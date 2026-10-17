# F-388-1: NW4R g3d `ResMdl` unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #388 owns `.text` `0x8024C590–0x8024D704`. Its first two functions, `ResFile::Terminate` and `ResFile::CheckRevision`, end `g3d_resfile.cpp` and were matched with #386 ([F-386-1](386-g3d-res-common-dict-file.md)). The remaining 22 functions begin `g3d_resmdl.cpp`, which continues into #389 and is now `Matching`.

## Unit and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/res/g3d_resmdl.cpp` | `.text` | `0x8024D000–0x8024E0F8` | 29 `ResMdl` accessors, `Bind`, `Init`, `Terminate` |

The unit has no data. `ResMdl::Terminate` (`0x8024E010`, 0xE8 bytes) is its last function. The next function, `0x8024E100`, returns a shape's position array through `ResMdl::GetResVtxPos(int)` and begins `g3d_resshp.cpp` in #389. objdiff reports 100% for all 32 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source and differences

The source follows CC0 zeldaret/ss `src/nw4r/g3d/res/g3d_resmdl.cpp` at commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`. The target matches the ss revision rather than ogws: it has the fur accessors, and `GetResVtxFurPos(int)` (`0x8024D660`) reads offset 0x2C. `ResMdl::Init` initializes the fur vector and fur position arrays after the texture coordinates, as in ss.

- The target begins with an 0x20-byte function that returns the `ResFile` at `toResFileData` (offset 0xC). ogws's `ResAnmTexPat::GetParent` has the same body, so it is named `ResMdl::GetParent` (medium confidence).
- `ResMdl::IsOpaque` and the `DrawEnumerator` class from ss are absent. The unit has no `"DrawOpa"` string; the `.sdata` copies of `"DrawOpa"` and `"DrawXlu"` at `0x804B8998`–`0x804B89C0` are referenced from code at `0x8025ECB0` onward. The source and `g3d_resmdl.h` omit them.
- The functions absent from the target were stripped by the linker: `GetResVtxTexCoordNumEntries`, `GetResVtxFurPosNumEntries`, `GetResVtxFurVec(int)`, `GetResVtxFurVecNumEntries`, `GetResShp(const char*)`, the two `ResTexPlttInfoOffset` accessors, and `Release`. The source keeps them, because `-ipa file` inlines the count functions into `Init`.

## Outside symbols

These callees were renamed in `symbols.txt` so the unit links, from the matching call order: `ResShp::Init` `0x8024E280`, `ResShp::Terminate` `0x8024E610`, `ResMat::Bind` `0x802506E0`, `ResMat::Init` `0x802507B0`, and the `DCStore(bool)` members of `ResVtxPos` `0x80250E60`, `ResVtxNrm` `0x80250E80`, `ResVtxClr` `0x80250EA0`, `ResVtxTexCoord` `0x80250EC0`, `ResVtxFurVec` `0x80250EE0` and `ResVtxFurPos` `0x80250F00`. The inline `Init` of each vertex array calls its `DCStore(false)`. Splits and sizes are unchanged.

## Traps

None new. The g3d trap from F-386-1 applies again: `g3d_resmdl.cpp` starts in #388 and ends inside #389.
