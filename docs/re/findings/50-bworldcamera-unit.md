# F-50-1: BWorldCamera.cpp unit split

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #50 covers `0x80066460`–`0x80068610`, which is exactly one source unit, `game/BWorldCamera.cpp`. The unit is split and registered `NonMatching`; the full DOL still verifies.

| Section | Range |
| --- | --- |
| `extab` | `0x80009DDC`–`0x8000A26C` |
| `extabindex` | `0x80044AB4`–`0x80044C04` |
| `.text` | `0x80066460`–`0x80068610` |
| `.data` | `0x80399F28`–`0x8039A160` |
| `.sdata` | `0x804B5B70`–`0x804B5B98` |
| `.sdata2` | `0x804B9CE8`–`0x804B9D10` |

## Evidence

- `0x80066460` stores the `BWorldCamera` vtable (`.data` `0x80399FC8`, 0x4C bytes, RTTI `.sdata` `0x804B5B78`) and calls the `BWorldObj` constructor at `0x8006A100`. RTTI bases: `BWorldObj`, `ICalc`, and `std::tr1::enable_shared_from_this<BWorldObj>` at offset 4.
- The previous unit, `BWorld.cpp`, ends at `0x80066454`, and its `.data` ends at `0x80399F28` ([F-46-1](46-bworld-unit-boundaries.md)). The first `BWorldCamera.cpp` data object is the `shared_ptr_deleter<BWorldCamera::CMotionProgram, default_delete<...>>` vtable at `0x80399F28`, used by `0x80067910`. The data ends after the `std::detail::default_delete<BWorldCamera::CMotionProgram>` type name (ending `0x8039A15A`). The next object, the `BWorldLightSet::CMotionProgram` deleter vtable at `0x8039A160`, belongs to `BWorldLightSet.cpp`.
- `.sdata` holds the `CMotionProgram` deleter type info (`0x804B5B70`), `BWorldCamera` type info (`0x804B5B78`), the first copy of the `std::out_of_range` type info (`0x804B5B80`, used by the vtable at `0x8039A040`), two bytes used by `0x80067F70` (`0x804B5B88`, `0x804B5B89`), and the `default_delete<BWorldCamera::CMotionProgram>` type info (`0x804B5B90`). `.sdata2` constants `0x804B9CE8`–`0x804B9D0C` are referenced only from this unit. The unit has no `.bss`, `.sbss` or `.ctors` contribution.
- `0x80067F70` is a `basic_string` template instance. It throws `std::out_of_range("basic_string: out_of_range")` and `std::length_error("basic_string: length_error")` (`.data` `0x8039A078`–`0x8039A11F`).

## Blocker

Reconstruction needs MSL C++ `std::tr1::shared_ptr`, `weak_ptr`, `enable_shared_from_this` and `basic_string` templates that compile to the original instances. It also needs the `BWorldObj`, `ICalc` and `BWorldCamera::CMotionProgram` class declarations. None of these exist in the repository yet. The shared containers are being built under #53 and #327, and `ICalc` under PR #906. Writing parallel STL headers here would duplicate that work, so source is deferred until those land.

## Method

Python reading the linked `main.elf` (byte-identical to the original DOL) for vtables, RTTI, `extabindex`, and `lis`/`addi`, `r13` and `r2` references. No decompiler output was used.

## Traps

- **Weak library objects mark the first user, not the owner.** The `std::out_of_range` vtable and type info sit inside this unit's data because it is the first unit to use them. Later units reference those same addresses, so they are not a file boundary.
