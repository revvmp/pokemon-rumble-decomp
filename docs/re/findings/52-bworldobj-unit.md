# F-52-1: BWorldObj.cpp and the following battle-royal unit

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #52 covers `0x8006A100`–`0x8006B464`. That range holds two source units. The first, `game/BWorldObj.cpp`, is now split and registered `NonMatching`. The second uses `CBattleRoyalBox` and `CBattleRoyalData`, but its file name is not yet evidenced. The full DOL still verifies.

## BWorldObj.cpp

| Section | Range |
| --- | --- |
| `extab` | `0x8000A6DC`–`0x8000A7F4` |
| `extabindex` | `0x80044D00`–`0x80044D48` |
| `.text` | `0x8006A100`–`0x8006A544` |
| `.data` | `0x8039A2B8`–`0x8039A300` |
| `.sdata2` | `0x804B9D28`–`0x804B9D30` |

- **Start.** `0x8006A100` is the `BWorldObj` constructor. It stores the `BWorldObj` vtable (`.data` `0x8039A2B8`, 0x48 bytes, RTTI `.sdata` `0x804B5A88`) and the two constants `0x804B9D28` and `0x804B9D2C`. The `BModelObj`, `BWorldCamera` and `BWorldLightSet` constructors call it, among others (`0x800528E0`, `0x80066460`, `0x80068610`, `0x800D4B50`, `0x801202D0`). The preceding `BWorldLightSet.cpp` ends at `0x8006A100` ([F-51-1](51-bworldlightset-unit.md)).
- **Contents.** The constructor; the deleting destructors at `0x8006A190` and `0x8006A200` (vtable slot 0, releasing the weak reference of `enable_shared_from_this<BWorldObj>` at offset 4 and the weak pointer at 0x1C); the weak-pointer assignment at `0x8006A290`; the shared-from-weak pair at `0x8006A2D0` and `0x8006A370`; three forwarders at `0x8006A480`, `0x8006A490` and `0x8006A4A0` to the member object at 0x24; and two flag operations at `0x8006A4B0` and `0x8006A4F0`, called from `BWorld.cpp`.
- **End.** The unit's only `.data` object is its vtable, ending at `0x8039A300`. Its only `.sdata2` constants are `0x804B9D28` and `0x804B9D2C`. The unit owns no `.sdata`: `BWorldObj`'s type info is the earlier copy at `0x804B5A88`.

## Following unit, name unresolved

| Section | Range |
| --- | --- |
| `extab` | `0x8000A7F4`–`0x8000AAA0` |
| `extabindex` | `0x80044D48`–`0x80044E08` |
| `.text` | `0x8006A550`–`0x8006B464` |
| `.data` | `0x8039A300`–`0x8039A4B8` |
| `.sdata` | `0x804B5BB0`–`0x804B5BD0` |
| `.sdata2` | `0x804B9D30`–`0x804B9D38` |

- Its data starts with `vector length error` and the exception specification list, used by the vector template at `0x8006A860`. Then come the `shared_ptr_deleter<CBattleRoyalBox, ...>` vtable (`0x8039A34C`, used by `0x8006AF10`), the `shared_ptr_deleter<CBattleRoyalData, ...>` vtable (`0x8039A3D0`, used by `0x8006A590` and `0x8006A640`), and the two `default_delete` type names, which end at `0x8039A4B6`. The next object, `battleroyal` at `0x8039A4B8`, is used by `0x8006B470` in #53's `CBattleRoyalDisplay.cpp`.
- `CBattleRoyalData` is a four-byte object holding one float. `0x8006A590` allocates one from a float and wraps it in a `shared_ptr`. `0x8006A550` takes a float and stores it when the stored value is below the constant at `0x804B9D34` or the new value is smaller than the stored value. It returns whether it stored, so it keeps a running minimum. It is called from `0x800D1CA0`.
- Its `.sdata2` constants `0x804B9D30` and `0x804B9D34` are used only by `0x8006A640` and `0x8006A550`. That is why the boundary falls between `0x8006A544` and `0x8006A550`, not at the `BWorldObj` data end alone.
- File-name order puts this unit after `BWorldObj.cpp` and before `CBattleRoyalDisplay.cpp`. No string or RTTI names the file, and `CBattleRoyal.cpp`, `CBattleRoyalBox.cpp` and `CBattleRoyalData.cpp` all fit that order. It is not split here, so no file name is invented.

## Blocker

Both units need the MSL C++ `tr1::shared_ptr`, `weak_ptr` and `enable_shared_from_this` templates (#53, #327). The second also needs `vector`. `BWorldObj.cpp` also needs the `ICalc` declaration (PR #906) and the class of its member at 0x24, whose constructor is `0x801119B0` and destructor `0x801119C0`. Source is deferred until those land.

## Method

Python reading the linked `main.elf` (byte-identical to the original DOL) for vtables, RTTI, `extabindex`, callers, and `lis`/`addi`, `r13` and `r2` references. No decompiler output was used.

## Traps

- **An issue range can hold an unnamed unit.** The block's data ownership shows a second unit with no class vtable and no file-name string. Do not stretch `BWorldObj.cpp` over it just because the issue range continues.
