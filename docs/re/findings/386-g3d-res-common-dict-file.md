# F-386-1: NW4R g3d resource common, dictionary and file units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #386 owns `.text` `0x8024AE80–0x8024B724`, the first 19 functions of NW4R g3d. They begin three translation units, `g3d_rescommon.cpp`, `g3d_resdict.cpp` and `g3d_resfile.cpp`, all of which are now `Matching`. `g3d_resfile.cpp` continues through all of #387 (`0x8024B730–0x8024C584`) and the first two functions of #388 (`0x8024C590`, `0x8024C6B0`), so this change reconstructs those functions too.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/res/g3d_rescommon.cpp` | `.text` | `0x8024AE80–0x8024B024` | `ResName::operator==`, `detail::ResWriteBPCmd` (two overloads), `ResWriteCPCmd`, `ResWriteXFCmd`, `ResWriteSSMask` |
| `nw4r/g3d/res/g3d_resdict.cpp` | `.text` | `0x8024B030–0x8024B3B0` | `ResDic::Get` (two overloads), `operator[](const char*)`, `Find`, `operator[](ResName)`, `GetIndex` |
| `nw4r/g3d/res/g3d_resfile.cpp` | `.text` | `0x8024B3B0–0x8024CFFC` | 20 `GetRes*` accessors, eight `HasRes*` predicates, `Bind`, `Init`, `Terminate`, `CheckRevision` |
| | `.data` | `0x803C5760–0x803C58C0` | eleven 32-byte aligned `ResNameData27` group names, `"3DModels(NW4R)"` to `"External"` |

The first g3d function is `0x8024AE80`; the function before it (`0x8024AE40`) is a deleting destructor referenced from a vtable at `0x803C5728`, which precedes the g3d data, and belongs to the preceding block. The unit order follows the zeldaret/ss link order (`g3d_rescommon`, `g3d_resdict`, `g3d_resfile`, `g3d_resmdl`). `g3d_resfile.cpp` ends with `ResFile::CheckRevision` (0x94C bytes, the same size as in the Mario Kart Wii symbol map); `0x8024D000` begins the next unit in #388. The group-name data is followed at `0x803C58C0` by an object referenced from `0x8024ED00`, outside this unit.

objdiff reports 100% for all 44 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1). A new `NW4R.g3d` library entry holds the three objects.

## Sources

- `g3d_rescommon.cpp` and `g3d_resdict.cpp` follow the CC0 zeldaret/ss sources at commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`, which equal CC0 ogws at commit `27fa94a593096eed7442efc4e70981fd52025b9c` apart from formatting.
- `g3d_resfile.cpp` follows ogws's function order, which groups the `GetRes*NumEntries` functions after all accessors. That order agrees with the target, where the kept predicates sit after `GetResAnmScn(const char*)`. ss places `GetResMdlNumEntries` next to `GetResMdl`.
- The g3d headers are the zeldaret/ss `include/nw4r/g3d` resource headers, stripped of comments by `tools/vendor/vendor_strip.py`. Only the include closure of `g3d_resfile.h` is kept: 25 resource headers plus `g3d_state.h`, `g3d_camera.h` and `g3d_fog.h`. ss's revision constants match this target better than ogws's (see below).

## Differences from the references

The 2009 target differs from both references in these ways:

- `ResDic` has a lookup taking an explicit name length (`0x8024B270`). It returns the entry's data like `operator[](const char*)`, but skips the null-name test and the `strlen` call, and passes its arguments straight to `Get(const char*, u32)`. Game code at `0x800DDB9C` and `0x800DDD04` calls the `ResFile` wrappers with the data pointer and length of a game string object. Its original name is unknown; it is named `ResDic::Find` (low confidence).
- `ResFile` has `GetResPltt(const char*, u32)` (`0x8024B4C0`) and `GetResTex(const char*, u32)` (`0x8024B630`), which use that lookup. The overload names are descriptive.
- Eight 0x4C-byte functions at `0x8024BE00–0x8024C07C` return whether the top-level dictionary contains the group for models, AnmChr, AnmVis, AnmClr, AnmTexPat, AnmTexSrt, AnmShp and AnmScn, in that order. Game code at `0x801C7744` and `0x801C78A0–0x801C78F0` uses them to build an animation-type mask. They are named `ResFile::HasResMdl` to `HasResAnmScn` (low confidence).
- `ResTex::CheckRevision` (`0x802510F0`) and `ResPltt::CheckRevision` (`0x80250F40`) are out-of-line and accept revision 1 or 3. The headers declare them without a body and set `REVISION` to 3.
- `ResAnmVis::REVISION` is 4, not ss's 3. The other revisions checked inline by `CheckRevision` match ss: `ResMdl` 11, `ResAnmChr` 5, `ResAnmClr` 4, `ResAnmTexPat` 4, `ResAnmTexSrt` 5, `ResAnmShp` 4, `ResAnmScn` 5.

Functions absent from the target were stripped by the linker: `GetResMdl(u32)`, `GetResPltt(const char*)`, `GetResPltt(int)`, `GetResPltt(u32)`, the `u32` index overloads, `GetResAnmScn(int)`, all ten `GetRes*NumEntries`, and `Release`. The source keeps the ones the references define, because `-ipa file` inlines `GetRes*NumEntries` into `Bind`, `Init`, `Terminate` and `CheckRevision`. `GetExternalData` is absent from the target and from ogws, but the `"External"` name remains in `.data`. As in ogws, the source keeps it with `#pragma force_active`.

## Header adaptations

- Includes were changed to the project's paths: `revolution/GX.h`, `revolution/MTX.h`, `nw4r/types_nw4r.h`, `nw4r/math/math_types.h`, `nw4r/ut/ut_Color.h` and `nw4r/ut/ut_binaryFileFormat.h`.
- ss's g3d headers rely on forward declarations in its `types_nw4r.h`. The same list is placed at the top of `g3d_rescommon.h`, so the shared `types_nw4r.h` is unchanged.
- The Petari GX headers have no FIFO command constants. `include/nw4r/g3d/platform/g3d_gxhardware.h` provides `GXFifoCmd`, `GX_BP_OPCODE_SHIFT`, `GX_BP_REG_SSMASK`, and the BP, CP and XF command sizes, which the resource layouts use. The values are the ss `GXHardware.h` values that the target's command writers use (0x61, 0x08, 0x10, 0xFE).
- `ALIGN_DECL(32)` became `ATTRIBUTE_ALIGN(32)`.

## Outside symbols

These callees were renamed in `symbols.txt` so the units link: `ResMdl::Bind` `0x8024D960`, `ResMdl::Init` `0x8024DA70`, `ResMdl::Terminate` `0x8024E010`, `ResPltt::DCStore(bool)` `0x80250F20` (the inline `ResPltt::Init` calls it with `false`), `ResPltt::CheckRevision` `0x80250F40`, `ResTex::CheckRevision` `0x802510F0`, `ResTex::Init` `0x80251120`, and `ResAnmTexPat::Bind` `0x80251B90`. Splits and sizes of these symbols are unchanged.

## Traps

- **An issue block can begin a unit that runs through the next two blocks.** `g3d_resfile.cpp` starts at the eighth function of #386 and ends inside #388. Because the issue owning the first function owns the split, check where a file ends before planning, and expect a single change to cover several neighbouring issues.
- **ss's g3d headers are not self-contained.** They compile only after ss's g3d forward declarations, which live in its `types_nw4r.h`. Without them the first error is an undefined `ResMdl` in `g3d_restev.h`, followed by hundreds of cascading errors.
- **A size mismatch among similar functions can reveal a different function.** The eight functions after the accessors were first assumed to be `GetRes*NumEntries`, which compile to 0x70 bytes. The target's 0x4C-byte bodies return only whether the dictionary entry exists.
