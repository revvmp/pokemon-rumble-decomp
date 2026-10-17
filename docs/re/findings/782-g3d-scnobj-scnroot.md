# F-782-1: NW4R g3d `ScnObj` and `ScnRoot` units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #782 owns `g3d_scnobj.cpp` and `g3d_scnroot.cpp`, 75 functions from `0x80267D40` to `0x8026AF9C`.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/g3d_scnobj.cpp` | `.text` | `0x80267D40–0x802695CC` | 37 functions of `ScnObj`, `ScnLeaf` and `ScnGroup` |
| | `.rodata` | `0x8036C960–0x8036C990` | `TYPE_NAME` of `ScnObj`, `ScnLeaf` and `ScnGroup` |
| | `.data` | `0x803C5ED0–0x803C5F94` | two switch tables, the `ScnGroup` vtable (0x40) and the `ScnObj` vtable (0x34) |
| | `.sbss` | `0x804B9318–0x804B931C` | `gpCullingFrustum` |
| | `.sdata2` | `0x804BAD28–0x804BAD30` | two float constants |
| `nw4r/g3d/g3d_scnroot.cpp` | `.text` | `0x802695D0–0x8026AF9C` | 38 functions of `ScnRoot` and `ScnObjGather`, the four anonymous-namespace sort predicates, and the weak MSL `std::sort` and `std::__sort132` instances |
| | `.rodata` | `0x8036C990–0x8036C9A0` | `TYPE_NAME` of `ScnRoot` |
| | `.data` | `0x803C5F98–0x803C6008` | vtables of `ScnObjGather` (0x2C) and `ScnRoot` (0x40) |
| | `.sdata` | `0x804B8980–0x804B8990` | two float constants and the two `sort` instances' `shuffle` statics |
| | `.sdata2` | `0x804BAD30–0x804BAD34` | one float constant |

objdiff reports 100% code for all 75 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

The linker removed members nothing references. In `g3d_scnobj.cpp`: `SetMtx` (pointer form), `GetMtx`, `SetPriorityDrawOpa`, `DisableScnObjCallbackTiming`, the three `ScnGroup_G3DPROC_CALC_*` helpers, the weak `IScnObjCallback` members, and the whole weak `ScnLeaf` vtable with its `IsDerivedFrom`, `GetTypeName`, `GetTypeObj` and destructor. In `g3d_scnroot.cpp`: `GetCurrentCamera`, `GetFog`, `UpdateFrame`, the weak `IScnObjGather` destructor and vtable, and the `static const` limits of the `sort` instances. `LightSetting`'s weak destructor resolves to the copy already named in `g3d_state.cpp`'s range.

## Source

Both files are CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee`, stripped of comments, with their includes replaced. `include/nw4r/g3d/g3d_scnobj.h` and `g3d_scnroot.h` are ss's headers from the same commit, stripped of comments; `g3d_scnobj.h` also includes `math_geometry.h` for `math::FRUSTUM`. Two members kept by this executable are missing from ss:

- `ScnGroup::Construct` (`0x802688E0`) follows CC0 xbret/xenoblade `libs/nw4r/src/g3d/g3d_scnobj.cpp`, rewritten in the style of ss's `ScnRoot::Construct`, with one `align4` over the object and its child array, as the target's `addi r0, r6, 0xEB; clrrwi r0, r0, 2` shows.
- `ScnRoot::ForceDrawMode` (`0x80269FA0`) is in no public reference. When forcing, it stores the mode and sets `SCNROOTFLAG_FORCE_RESMDLDRAWMODE`; otherwise it clears the flag, through the existing `SetScnRootFlag`. It compiles to the target bytes.

`include/algorithm` now carries ss's MSL `find`, `move`, `swap`, `min_element`, `__selection_sort`, `__sort132` and `sort` beside `min` and `max`. It includes a new minimal `include/iterator` with the iterator tags, `iterator_traits` and `distance` from ss's MSL `iterator`. The `sort` and `__sort132` instances in `g3d_scnroot.cpp` match only with MSL's exact algorithm.

## Symbol corrections

- `SetBoundingVolume` (`0x80268300`) is 0x84 bytes. dtk had split its unreachable trailing `blr` off as a 4-byte `fn_80268380`, which is removed.
- `0x80268270` is `SetPriorityDrawXlu`, not `Opa`: it stores to offset `0xD1`. Both are 0x24 bytes.
- `0x80269590` and `0x802695C0` are `ScnObj::GetTypeName` and `GetTypeObj`; the same-sized `ScnLeaf` members are stripped.

## Outside symbols

`Fog::Fog(FogData *)` (`0x80270A90`), `Fog::Init` (`0x80270AA0`) and `Fog::SetFogRangeAdjParam` (`0x80270B70`) are named for the link from their call sites in `Construct` and `SetGlbSettings`.

## Traps

- **dtk can split dead code into a fake function.** An unreachable `blr` after a tail call became a 4-byte `fn_` symbol, so the real function looked 4 bytes short. When a compiled function is exactly 4 bytes longer than the target symbol and the next symbol is a lone `blr`, merge them.
- **Same-sized neighbours need a field check.** A size-only alignment chose `SetPriorityDrawOpa` and `ScnLeaf::GetTypeName` where the target has `SetPriorityDrawXlu` and `ScnObj::GetTypeName`. objdiff showed 99.9% for the first; the second failed only at the link. Check stored offsets and type-name references.
- **Data referenced by later fallback units needs names.** The link failed until the type names, vtables and `gpCullingFrustum` used by units not yet split were renamed in `symbols.txt`.
