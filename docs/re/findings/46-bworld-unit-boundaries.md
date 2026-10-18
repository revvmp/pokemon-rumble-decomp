# F-46-1: BWorld.cpp and its neighboring game units

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Question from issue #46: which translation unit owns the 27 functions in `0x8005F450`–`0x80062974`, and where do the units around it begin and end.

## Result

Every function of #46 belongs to one source unit, `game/BWorld.cpp`, which begins in #43 at `0x8005AC70` and ends in #49 at `0x80066454`. Under [the file ownership rule](../../COLLABORATION.md#how-work-is-divided), #43 owns its split, object line and header; #44 to #49 add their bodies to it in address order. The next three units follow in ASCII file-name order and each begins exactly at an issue boundary:

| Unit | Issues | `.text` | `extab` | `extabindex` | `.data` | `.sdata` | `.sdata2` |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `game/BWorld.cpp` | #43–#49 | `0x8005AC70`–`0x80066454` | `0x800078BC`–`0x80009DDC` | `0x80044514`–`0x80044AB4` | `0x803999B8`–`0x80399F28` | `0x804B5AF0`–`0x804B5B70` | `0x804B9CA8`–`0x804B9CE8` |
| `game/BWorldCamera.cpp` | #50 | `0x80066460`–`0x80068610` | `0x80009DDC`–`0x8000A26C` | `0x80044AB4`–`0x80044C04` | from `0x80399F28` | from `0x804B5B70` | from `0x804B9CE8` |
| `game/BWorldLightSet.cpp` | #51 | `0x80068610`–`0x8006A100` | `0x8000A26C`–`0x8000A6DC` | `0x80044C04`–`0x80044D00` | from `0x8039A160` | from `0x804B5B98` | from `0x804B9D10` |
| `game/BWorldObj.cpp` | #52 | from `0x8006A100` | from `0x8000A6DC` | from `0x80044D00` | from `0x8039A2B8` | not established | from `0x804B9D28` |

`BWorld.cpp` has no `.bss`, `.sbss`, `.rodata` or `.ctors` contribution. The first game static initializer in `.ctors` is `0x8006CD00`, so no file from `0x80052500` to `0x8006CD00` can be delimited by an initializer; the boundaries come from data ownership instead.

## Evidence

- **Class names.** RTTI gives `BWorld` (`.sdata` `0x804B5B30`, name at `0x804B5B28`) with bases `ICalc`, `ICalcDraw` and `std::tr1::enable_shared_from_this<BWorld>` at offset 4; `BWorldCamera` (`0x804B5B78`), `BWorldLightSet` (`0x804B5BA0`) and `BWorldObj` (`0x804B5A88`), the last two deriving from `ICalc` and `std::tr1::enable_shared_from_this<BWorldObj>`, with `BWorldObj` a base of both.
- **Constructors.** `0x8005AC70` stores the `BWorld` vtable (`0x80399DA0`) and is its first function; `0x80066460` stores the `BWorldCamera` vtable (`0x80399FC8`) and calls `0x8006A100`, which stores the `BWorldObj` vtable (`0x8039A2B8`). `0x80068610` stores the `BWorldLightSet` vtable (`0x8039A208`).
- **Start of `BWorld.cpp` data.** The first `.data` object referenced only from `0x8005AC70` onward is the `dummy_capture` string at `0x803999B8`. The preceding `exception` string (`0x803999AC`) is referenced by `0x80058640`, in the previous unit.
- **End of `BWorld.cpp`.** Its `.data` holds, in order, the strings `vector length error` and the exception specification list, then seven `shared_ptr_deleter` vtables with their type names (`CEffect`, `nw4r::g3d::AnmScnRes`, `CKDTree<BModelObj>`, `nw4r::g3d::ScnRoot`, `nw4r::g3d::ScnGroup`, `BWorld::EffectData`, `BWorld::OutlineData`), the `BWorld`, `enable_shared_from_this<BWorld>` and `ICalcDraw` vtables, a second `tree::insert length error` string, and finally the four `default_delete` type names ending at `0x80399F21`. The next object, the `BWorldCamera::CMotionProgram` deleter vtable at `0x80399F28`, is referenced only from `BWorldCamera.cpp`. The `.text` run of deleter destructors, `get_deleter` and `dispose` bodies from `0x80064840` to `0x80066454` belongs to those vtables: for example the `CEffect` deleter vtable lists `0x80066000`, `0x80064980` and `0x800649D0`.
- **Small data.** `BWorld.cpp` references `.sdata` `0x804B5B48`–`0x804B5B68` and owns the seven deleter type-info objects from `0x804B5AF0`, the `BWorld` name and type info, and the `enable_shared_from_this<BWorld>` and `ICalcDraw` type info. Its only foreign `.sdata` reference, `0x804B5AC8` (`void (*)(nw4r::g3d::G3dObj *)` type info), is the earlier unit's copy. Its `.sdata2` constants are `0x804B9CA8`–`0x804B9CE0`; the preceding `0x804B9CA0` is used only by `0x80056B80`. `0x804B8960` and `0x804B910C` are external globals defined elsewhere.
- **Exception tables.** `extabindex` entries are in function order; the first with a function at or above `0x8005AC70` is at `0x80044514`, and the first at or above `0x80066460` at `0x80044AB4`. #46's own functions occupy `extabindex` `0x800446D0`–`0x800447B4` and `extab` `0x80008EEC`–`0x80009668`.

## Contents of #46

| Address | Identity |
| --- | --- |
| `0x8005F740` | `BWorldObj` virtual slot `0x40` (vtable offset), an empty inline body; the copy kept is `BWorld.cpp`'s |
| `0x8005F790` | Vector insertion template instance; it throws `std::length_error("vector length error")` |
| `0x800605C0` | `BWorldObj` virtual slot `0x20`, inline copy |
| `0x80060CC0`–`0x800613F0` | Five factories that allocate a `CEffect` and wrap it in `shared_ptr<CEffect>` (deleter vtable `0x80399A14`) |
| `0x80062200` | `BWorld` virtual slot `0xC`, the first virtual after the destructor |
| `0x80062970` | `BWorldObj` virtual slot `0xC`, inline copy |

Names for the remaining functions are not yet established.

## Consequence for #46 to #49

None of #46 to #49 can split or register a unit of its own: the split, the `Object(NonMatching, "game/BWorld.cpp")` line and `include/game/BWorld.h` belong to #43. A `Matching` mark needs every function from `0x8005AC70` to `0x80066454` reconstructed, across seven issues. Bodies for #46 wait for that split.

## Method

Python reading the linked `main.elf` (byte-identical to the original DOL) for `.ctors`, `extabindex`, vtables, RTTI records and `lis`/`addi`, `r13` and `r2` references, cross-checked against GNU objdump 2.42 disassembly. No decompiler output was used.

## Traps

- **Issue boundaries are not file boundaries here.** #46 to #49 lie inside one unit that starts in #43, while #50, #51 and #52 each start a unit exactly. Check data ownership before splitting at an issue boundary.
- **Inline virtual copies sit mid-file.** Weak copies of `BWorldObj` inline virtuals (`0x8005F740`, `0x800605C0`, `0x80062970`) appear between `BWorld.cpp`'s own functions, not at its end. They do not mark a file boundary.
