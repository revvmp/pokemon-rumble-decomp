# Issue 40: the tail of BModelObj.cpp

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical to `orig/wpse-v1/main.dol`.

## F-40-1: every function of #40 belongs to `game/BModelObj.cpp`

Question: which translation unit owns the 26 functions in `.text` `0x800561B0`–`0x80058B44`, and can #40 split them.

### Result

All 26 functions lie inside one source unit, `game/BModelObj.cpp`, whose first function is the `BModelObj` constructor at `0x800528E0` (#39) and which continues through #41 and #42 to `0x8005AC6C`, where [F-46-1](46-bworld-unit-boundaries.md) places the start of `BWorld.cpp` at `0x8005AC70`. Under [the file ownership rule](../../COLLABORATION.md#how-work-is-divided), #39 owns the split, the `configure.py` object line and `include/game/BModelObj.h`. #40 can only add bodies to that file after #39's split merges. #40 is therefore research only; no split, object line or source was added.

Sections of `BModelObj.cpp` owned by #40's functions:

| Section | Range |
| --- | --- |
| `.text` | `0x800561B0`–`0x80058B44` |
| `extab` | `0x80006F98`–`0x80007328` |
| `extabindex` | `0x80044328`–`0x800443C4` |

The unit's data is shared with the rest of the file. Its `.data` runs from `0x80399440` (the `OUTLINED` string) to `0x803999B8`, its `.sdata` type info from `0x804B5A40` to `0x804B5AD8`, and the only `.sdata2` constant used solely by #40 is `0x804B9CA0`, read by `0x80056B80`.

### Evidence

- **One contiguous `.data` block.** `0x80399440`–`0x803999B8` holds, in order: the `shared_ptr_deleter` vtables and names for `KDTreeZXNode<0, 9>` and `KDTreeZXNode<2, 9>` (entries in #41 at `0x8005A950`, `0x80059E80`, `0x8005A910`), the vtables of `KDTreeZXNode<0, 9>` (`0x80058B50`, #41) and `KDTreeZXNode<2, 9>` (`0x80058650`, #40), the `nw4r::g3d::AnmObjChrRootScale` deleter (#41), the `BModelObj` vtable at `0x80399640`, the type names `BModelObj`, `BSoundSource`, `BKDTreeLink`, `IKDTreeLinkRemoved`, `BWorldObj` and `std::tr1::enable_shared_from_this<BWorldObj>`, the `std::length_error`, `std::logic_error`, `shared_ptr_deleter_common` and `std::exception` vtables, the `tree::insert length error` string, the exception specification list, the `default_delete` type names, and the `exception` string at `0x803999AC` returned by `0x80058640`. The `BModelObj` vtable references functions from #39 (`0x80052F60`, `0x80053A20`), #40 and #41, so #39 to #42 cannot be separate files.
- **No initializer boundary.** The first game `.ctors` entry is `0x8006CD00` (F-46-1), so no file in this range ends at a static initializer.
- **Class.** RTTI `BModelObj` (`.sdata` `0x804B5A68`, base list `0x803996CC`) lists `BKDTreeLink` at offset 0x64, `BSoundSource` at 0x48, `enable_shared_from_this<BWorldObj>` at 4, `ICalc` at 0 and `BWorldObj` at 0. The constructor at `0x800528E0` calls the `BWorldObj` constructor (`0x8006A100`), constructs `BSoundSource` at 0x48 (`0x801E8740`), stores the vtable at 0 and `vtable + 0x48` at 0x60, and makes the `BKDTreeLink` list head at 0x64 point to itself.

### Vtable layout

`__vt__9BModelObj` at `0x80399640` is 0x80 bytes. CodeWarrior places the virtual functions introduced by `BModelObj` after the secondary `BSoundSource` table, not before it: calls through the primary vptr use offsets 0x54 to 0x7C (`0x8005359C` offset 0x54, `0x80055CA4` 0x58, `0x80053AA0` 0x68, `0x80054A2C` 0x6C, `0x8005526C` 0x70), which index past the secondary header.

| Offset | Entry | Owner |
| --- | --- | --- |
| 0x00 | RTTI `BModelObj`, offset 0 | |
| 0x08–0x44 | sixteen `BWorldObj` slots | see below |
| 0x48 | RTTI `BModelObj`, offset -0x48 | |
| 0x50 | `0x8005AA90`, `BSoundSource` destructor thunk (`this - 0x48`, then `0x80052F60`) | #42 |
| 0x54–0x7C | eleven new `BModelObj` virtuals: `0x80053840`, `0x800541B0`, `0x80055200`, `0x80055BF0`, `0x80055CD0`, `0x80054940`, `0x80055140`, `0x80055980`, `0x80056130`, `0x80056350`, `0x800570D0` | #39, last two #40 |

The sixteen `BWorldObj` slots compared with `__vt__9BWorldObj` (`0x8039A2B8`, [F-46-1](46-bworld-unit-boundaries.md)):

| Slot | BModelObj | BWorldObj | Meaning |
| --- | --- | --- | --- |
| 0 | `0x80052F60` | `0x8006A200` | destructor |
| 1 | `0x800579F0` | `0x80062970` | override |
| 2 | `0x80057A60` | `0x80062C70` | override |
| 3 | `0x80053A20` | `0x80069E60` | override |
| 4 | `0x800549C0` | `0x80067EE0` | override |
| 5 | `0x800584B0` | `0x80067ED0` | override |
| 6 | `0x800570A0` | `0x800605C0` | override |
| 7 | `0x80057D00` | `0x80057D00` | inherited inline |
| 8 | `0x80057D20` | `0x80067EA0` | override |
| 9 | `0x80057CF0` | `0x80057CF0` | inherited inline |
| 10 | `0x80057CE0` | `0x80057CE0` | inherited inline |
| 11 | `0x80057CD0` | `0x80057CD0` | inherited inline |
| 12 | `0x800570C0` | `0x80067E90` | override |
| 13 | `0x80057160` | `0x8005F440` | override |
| 14 | `0x80057960` | `0x8005F740` | override |
| 15 | `0x80057CC0` | `0x80057CC0` | inherited inline |

The five shared entries are `BWorldObj` inline virtuals whose surviving weak copies are those of `BModelObj.cpp`. They are emitted at `0x80057CC0`, `0x80057CD0`, `0x80057CE0`, `0x80057CF0`, `0x80057D00`: slots 15, 11, 10, 9, 7, the reverse of their declaration order, which matches the `-ipa file` emission order recorded in [F-735-1](735-game-compiler.md).

### Identities of the 26 functions

Names are descriptive unless marked original. "Original" names are standard-library mangled names fixed by the `std::exception`, `std::logic_error` and `std::length_error` vtables and RTTI; they are now in `config/wpse-v1/symbols.txt`.

| Address | Size | Identity | Evidence |
| --- | --- | --- | --- |
| `0x800561B0` | 0x18 | `BModelObj` non-virtual setter of the word at 0xE8; on change calls `0x800561D0` | callers `0x8009A1DC`, `0x8014D798` |
| `0x800561D0` | 0x178 | applies the 0xE8 mode (0, 2 or other) as destination alpha to every material of the model through `ScnMdl::CopiedMatAccess` and `ResMatPix::GXSetDstAlpha` | callers `0x800535D8`, `0x800561C0`, `0x80057160` |
| `0x80056350` | 0x810 | new virtual at 0x78 | vtable |
| `0x80056B60`, `0x80056B70` | 0x10 each | two-word zeroing constructors, no references in code or data | |
| `0x80056B80` | 0x518 | computes a bounding box into an output `VEC3` triple and calls `ScnObj::SetBoundingVolume` | callers in #39 and `0x8014B690`; `.sdata2` `0x804B9CA0` |
| `0x800570A0` | 0x1C | slot 6, returns the `VEC3` at 0x88 by value | vtable |
| `0x800570C0` | 0x8 | slot 12, returns `this + 0xEC` (a tree container whose header the constructor clears at 0xEC–0xF4) | vtable, constructor |
| `0x800570D0` | 0x90 | new virtual at 0x7C; looks up and inserts into the 0xEC tree (`0x8005A7D0`, `0x800594F0`, #41) | vtable |
| `0x80057160` | 0x7F8 | slot 13 | vtable |
| `0x80057960` | 0x84 | slot 14, calls `G3dObj::DetachFromParent` | vtable |
| `0x800579F0` | 0x6C | slot 1 | vtable |
| `0x80057A60` | 0x258 | slot 2, builds a matrix with `SinCosFIdx` and `PSMTXConcat` and calls `ScnObj::SetMtx` | vtable |
| `0x80057CC0` | 0x4 | `BWorldObj` slot 15, empty inline | both vtables |
| `0x80057CD0` | 0x4 | `BWorldObj` slot 11, empty inline | both vtables |
| `0x80057CE0` | 0x8 | `BWorldObj` slot 10, inline returning 0 | both vtables |
| `0x80057CF0` | 0x8 | `BWorldObj` slot 9, inline returning 0 | both vtables |
| `0x80057D00` | 0x1C | `BWorldObj` slot 7, inline returning the `VEC3` at 0x30 by value | both vtables |
| `0x80057D20` | 0x78C | slot 8 | vtable |
| `0x800584B0` | 0x14 | slot 5, stores a float at 0x2C and sets bit 0 of the flags at 0x70 | vtable |
| `0x800584D0` | 0x88 | weak destructor of an object holding a `shared_ptr` at 0 | caller `0x800F867C`, `extabindex` |
| `0x80058560` | 0x40 | original `std::exception::~exception()` (`__dt__Q23std9exceptionFv`) | vtable `0x80399820`; cleanup action in 366 `extab` words |
| `0x800585A0` | 0x8 | original `std::logic_error::what() const` (`what__Q23std11logic_errorCFv`) | `logic_error` vtable `0x803997B0` and `length_error` vtable `0x80399778` |
| `0x800585B0` | 0x90 | original `std::logic_error::~logic_error()` (`__dt__Q23std11logic_errorFv`) | vtable `0x803997B0` |
| `0x80058640` | 0xC | original `std::exception::what() const` (`what__Q23std9exceptionCFv`), returns `exception` | vtable `0x80399820` |
| `0x80058650` | 0x4F4 | the only virtual of `KDTreeZXNode<2, 9>`, which derives from `IKDTreeLinkRemoved`; not a destructor | vtable `0x80399590`, base list `0x803995B0` |

### Consequence

Reconstructing #40 needs `include/game/BModelObj.h` and the `BModelObj.cpp` split from #39, and the `BWorldObj`, `BSoundSource`, `BKDTreeLink` and `KDTreeZXNode` declarations. A `Matching` mark needs every function from `0x800528E0` to `0x8005AC6C`, across #39 to #42.

### Method

Python reading `build/wpse-v1/main.elf` for vtables, RTTI records, `extabindex` and data pointers, and GNU objdump 2.42 disassembly for `lis`/`addi`, `r13`, `r2` and call references. `configure.py`, `ninja` and `verify.py` exit 0 with the symbol renames. No decompiler output was used.

## Traps

- **New virtuals follow the secondary vtable.** In `BModelObj`, CodeWarrior places the derived class's new virtual functions after the secondary base table, so the primary vtable is not contiguous and the second RTTI header sits in the middle of it. A run of function pointers after a secondary header is not that base's table.
- **The `std::exception` destructor is a game-unit weak copy.** `0x80058560` is referenced by 366 `extab` words across the game; its kept copy is in `BModelObj.cpp`, not MSL.
- **Issue boundaries are not file boundaries.** #39 to #42 are one file; #40 cannot split independently.
