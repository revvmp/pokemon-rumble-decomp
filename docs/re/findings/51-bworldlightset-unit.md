# F-51-1: BWorldLightSet.cpp unit split

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #51 covers `0x80068610`–`0x8006A100`, which is exactly one source unit, `game/BWorldLightSet.cpp`. The unit is split and registered `NonMatching`; the full DOL still verifies.

| Section | Range |
| --- | --- |
| `extab` | `0x8000A26C`–`0x8000A6DC` |
| `extabindex` | `0x80044C04`–`0x80044D00` |
| `.text` | `0x80068610`–`0x8006A100` |
| `.data` | `0x8039A160`–`0x8039A2B8` |
| `.sdata` | `0x804B5B98`–`0x804B5BB0` |
| `.sdata2` | `0x804B9D10`–`0x804B9D28` |

## Evidence

- `0x80068610` stores the `BWorldLightSet` vtable (`.data` `0x8039A208`, 0x48 bytes, RTTI `.sdata` `0x804B5BA0`) and calls the `BWorldObj` constructor at `0x8006A100`. RTTI bases: `BWorldObj`, `ICalc`, and `std::tr1::enable_shared_from_this<BWorldObj>`.
- The previous unit, `BWorldCamera.cpp`, ends at `.text` `0x80068610` and `.data` `0x8039A160` ([F-50-1](50-bworldcamera-unit.md)). This unit's data begins with the `shared_ptr_deleter<BWorldLightSet::CMotionProgram, default_delete<...>>` vtable at `0x8039A160`. Its entries are the destructor `0x8006A0C0`, `get_deleter` `0x80069E80` and `dispose` `0x80069ED0`. The data then holds the `BWorldLightSet` and `enable_shared_from_this<BWorldObj>` vtables and the `default_delete<BWorldLightSet::CMotionProgram>` type name, which ends at `0x8039A2B8`. That address is the `BWorldObj` vtable, used by the `BWorldObj` constructor `0x8006A100`, which starts #52's unit.
- `.sdata` holds the deleter type info (`0x804B5B98`), the `BWorldLightSet` type info (`0x804B5BA0`) and the `default_delete` type info (`0x804B5BA8`, used by `0x80069E80`). `.sdata2` `0x804B9D10`–`0x804B9D20` is used only by `0x80068B50`, `0x800696A0` and `0x80069D80`. There is no `.bss`, `.sbss` or `.ctors` contribution.
- The only foreign data reference is the `shared_ptr_deleter<nw4r::g3d::AnmScnRes, ...>` vtable at `0x80399A88`, which is `BWorld.cpp`'s weak first copy.

## Blocker

Reconstruction needs the same MSL C++ `tr1::shared_ptr` and `enable_shared_from_this` templates as `BWorldCamera.cpp` (#53, #327), plus the `ICalc` and `BWorldObj` declarations (PR #906 for `ICalc`). Source is deferred until those land.

## Method

Python reading the linked `main.elf` (byte-identical to the original DOL) for vtables, RTTI, `extabindex`, and `lis`/`addi`, `r13` and `r2` references. No decompiler output was used.

## Traps

No new trap beyond [F-50-1](50-bworldcamera-unit.md#traps).
