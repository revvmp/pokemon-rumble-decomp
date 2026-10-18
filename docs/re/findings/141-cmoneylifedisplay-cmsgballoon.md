# Issue 141: CMoneyLifeDisplay.cpp and the start of CMsgBalloon.cpp

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`, analysed in the dtk 1.8.4 split assembly under `build/wpse-v1/asm/` and with `tools/re/unitdiff.py` under `cflags_game`. No decompiler output was used.

## F-141-1: file boundaries in `0x800C4810`–`0x800CD728`

### What precedes `CMoneyLifeDisplay.cpp`

`CMoneyLifeDisplay.cpp` does not start in #140. It starts at #141's first function, `0x800C4960`, so #141 owns its split, object line and any header.

- **`CMiniMap.cpp` ends at `0x800C4810`.** Slot 0 of the `shared_ptr_deleter<CMiniMap>` vtable `0x803A4B74` is the deleter destructor `0x800C47D0` (0x40 bytes), which ends at `0x800C4810`. As in F-159-3 and F-142-1, the deleter destructor is the file's last function.
- **`0x800C4810`–`0x800C495C` are a separate unit.** The three functions build and destroy a 0xF0-byte object with a pointer at 0, an object at 4 built by `0x801CB1C0` (the `CLayout` constructor) and a word at 0xEC: a default constructor `0x800C4810`, a deleting destructor `0x800C4860`, and a constructor `0x800C48C0` that loads a layout and stores the pane found by `"root"`. They are not weak copies: their callers `0x8006C1CC`, `0x800A002C`, `0x800A4ACC` and `0x800A4D64` are earlier in link order, so an inline copy would have been kept there. No `CMoneyLifeDisplay` function calls them.
- **`.sdata` shows an object boundary at `0x804B6AB8`.** `"root"` (5 bytes, read only by `0x800C48C0`) sits at `0x804B6AB8`, although the preceding `"map"` ends at `0x804B6AB4`. Inside `CMiniMap.cpp` the 5-byte `"%02d"` is only 4-aligned (`0x804B6A9C`), so the 8-byte step is a section start, not string alignment.

The helper unit is entirely inside #140's range and is not split here. Its file name is not recovered.

### `CMoneyLifeDisplay.cpp`

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x8001D4F0`–`0x8001DB14` | Record of `0x800C4960`; `0x8001D4B8` belongs to `0x800C48C0` |
| `extabindex` | `0x80048C24`–`0x80048C90` | Entry of `0x800C4960` |
| `.text` | `0x800C4960`–`0x800C59D0` | The factory to the `shared_ptr_deleter<CMoneyLifeDisplay>` destructor `0x800C5990` (F-142-1) |
| `.data` | `0x803A4D00`–`0x803A50E0` | Pane names `money_root`, `life_root`, `moneylife_root` read by the factory; `"minimap_map"` at `0x803A4CF0` belongs to `CMiniMap` |
| `.sdata` | `0x804B6AC0`–`0x804B6B20` | `"life"`, `"LIFE%d"`, `L"%d"`, `"MONEY"`, motion names and three type infos |
| `.sdata2` | `0x804B9F60`–`0x804B9F78` | Read only by `0x800C4960` and `0x800C53B0`; `0x804B9F58` is `CMiniMap`'s |

The file has no `.bss`, `.sbss` or `.ctors` entry. `CMsgBalloon.cpp` uses the ranges of F-142-1. With both splits and `NonMatching` object lines, `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## F-141-2: `CMoneyLifeDisplay`

RTTI names `@unnamed@CMoneyLifeDisplay_cpp@::CMoneyLifeDisplay` with base `IMoneyLifeDisplay`, which derives from `ICalcDraw`. The factory `0x800C4960` allocates 0x5C bytes and constructs the object inline.

| Slot | `IMoneyLifeDisplay` (`0x803A4F68`) | `CMoneyLifeDisplay` (`0x803A4EF0`) |
| --- | --- | --- |
| 0 | `0x800C52B0` | `0x800C5880` |
| 1 | pure | `Calc` `0x800C53B0` |
| 2, 3 | `ICalcDraw` defaults | `ICalcDraw` defaults |
| 4 | pure | `Draw` `0x800C5350` |
| 5, 6, 7 | pure | `0x800C5330`, `0x800C5310`, `0x800C52F0` |

| Offset | Member | Evidence |
| --- | --- | --- |
| 0x4 | `int` display mode | From the factory's map index: byte 2 equal to 7 gives 0 when byte 3 is 1, else 2; byte 2 equal to 0 gives 1; otherwise 3. `Draw` and slots 5 to 7 do nothing while it is 0; `Calc` tests bits 1 and 0 |
| 0x8 | 0x24-byte object with a vtable | Constructor `0x80070C20`, destructor `0x80070C50`; `Calc` calls its slot 1, `Draw` tests `0x80070D60`, slots 5 to 7 forward to `0x80070D30`, `0x80070D40` and `0x80070D50` |
| 0x24, 0x28 | `int` shown money, shown life count | Both -1 initially; `Calc` compares 0x24 with `IGameSession` slot 17 (`GetMoney`) and steps 0x28 towards slot 7 of the object returned by `0x800BD5C0` |
| 0x2C | `shared_ptr<CLayout::CMotionProgram>` | `begin` and `end` motions; released when state 3 is reached |
| 0x34 | `shared_ptr<CUIManager>` | `Draw` calls its slot 4 |
| 0x3C | `shared_ptr<BUILayout>` | `MONEY` pane text and `get` motion |
| 0x44 | `shared_ptr<BUILayout>[3]` | Life icons, destroyed with `__destroy_arr` |

The five virtual overrides lie in reverse vtable order after the factory and `~IMoneyLifeDisplay`, so they are inline in the class (F-735-1); `0x800C5770` (`~shared_ptr`) and `0x800C5800` (`~move_ptr`) are exception-action destructors referenced only by the factory's exception table. Every function of the file is therefore emitted only through the factory. Names other than `Calc` and `Draw` are descriptive.

### Blockers

Unlike `CMsgBalloon.cpp`, this file is compiled with inlining. The factory inlines `basic_string::c_str` and the destructor of the `"LIFE%d"` string returned by `0x800721A0`, and `Calc` inlines the wide string of `0x8006BD60`, so both need MSL `basic_string` (#327). The factory also inlines `new BUILayout` (0x134 bytes) and `new CUIManager` (0x54 bytes), which need complete `BUI`, `CUIManager` and `CLayout` layouts (#327, #328, #329 to #335). None of the 13 functions is reconstructed.

## F-141-3: the `CMsgBalloon.cpp` functions of #141

objdiff with `src/game/CMsgBalloon.cpp`:

| Address | Function | Match |
| --- | --- | --- |
| `0x800C59D0` | `GetMapRecord(MapIndex)` | 100% |
| `0x800C5A60` | `IsStageCleared(MapIndex)` | 100% |
| `0x800C5AE0` | `CMapRecord::IsFlagSet(u32) const` | 100% |
| `0x800C5B00` | `vector<shared_ptr<MapDesc> >::size() const` | 100% |
| `0x800C5B10` | `GetStageClearCount(MapIndex)` | 100% |
| `0x800C5BC0` | `CMapRecord::GetClearCount() const` | 100% |
| `0x800C5BD0` | `Clamp<int>(int, int, int)` | 100% |
| `0x800C5C10` | `std::max<int>(const int&, const int&)` | 100% |
| `0x800C5C30` | `IMsgBalloon::Create(bool)` | 100% |
| `0x800C5CC0` | `CMsgBalloon::Init(bool)` | Not reconstructed |

- **`MapIndex` is four signed 8-bit bitfields.** Every reader loads one word and extracts each byte with `extlwi`/`srawi`, and `IsStageCleared` writes byte 3 with a word read, `rlwimi` and a word store. F-91 describes the same 4-byte index for `IGameSession::GetStageDesc`.
- **The index is passed by value.** Each caller copies the word to its stack before every call, including the call through `IGameSession` slot 13. `GetStageDesc` is therefore declared with a by-value `MapIndex`; with `const MapIndex&` the copy disappears.
- **Save records.** `0x80167E10` returns save slot `n` (0x1360 bytes) of the object in `.sbss` `0x804B90D0`, or the current slot for -1 (`GetSaveData`). From offset 0x18 it holds 32-bit records indexed `[3][5][8][10]` by the four index bytes. `IsFlagSet` tests `(value & mask) == mask`; `GetClearCount` is the top 10 bits, clamped to 0 to 999 by its only caller.
- **`GetMapRecord` reads the index before the call.** It loads and extracts all four fields into saved registers before calling `GetSaveData`; indexing the result directly reads them afterwards (25.7%). Copying the fields to locals first reproduces it.
- **`Clamp` uses the by-reference `std` templates.** `0x800C5BD0` passes the addresses of its arguments to `std::max` (`0x800C5C10`, first emitted here and also called from `0x801A0EDC` and `0x801C791C`) and `std::min` (`0x80095280`, kept from an earlier file), unlike the by-value `Min`/`Max` of `BController` (F-735-1).
- **The factory** has the shape of `IDlgPiiSelector::Create` (F-92): `new(0x88)`, the out-of-line constructor `0x800C66F0` (#143), the `shared_ptr` constructor, `operator->`, `Init(flag)` and the converting return.

The four free functions are written with external linkage. Their callers in `CMsgBalloon` code (`0x800C8064`, `0x800C87B4`, `0x800C8940`, `0x800C8AFC`) are not reconstructed, and an internal-linkage definition would not be emitted without them, so the linkage is unproven. `IsStageCleared`, `GetStageClearCount`, `GetSaveData`, `CMapRecord` and the `MapIndex` field names (`mMode`, `mArea`, `mStage`, `mMap`) are descriptive, low confidence; the `mMode` dimension is 3 because the records must end inside the 0x1360-byte slot.

### What blocks `Init`

`Init` (`0x800C5CC0`) calls `basic_string` only out of line (`0x800721A0`, `c_str` `0x80072320`, destructor `0x80067F00`), because the file is compiled without inlining. It is blocked by:

- `new CUIManager` (0x54 bytes) and three `new BUILayout` (0x134 bytes), which need complete `BUI` (0x4C bytes, F-327-1), `CUIManager` and `CLayout` (0xE8 bytes) declarations;
- the `BUILayout` virtual at vtable offset 0x30 (`0x801C9FB0`, forwarding to the `CLayout` loader `0x801CCBD0`), which needs the nine `BUI` virtuals and the first `BUILayout` virtual declared in order;
- the `vector<shared_ptr<BUILayout> >::push_back` layering of F-142-4.

## Traps

- **A unit can sit between a deleter destructor and the next survey class.** `0x800C4810`–`0x800C495C` follow `CMiniMap`'s deleter destructor but are neither `CMiniMap` nor `CMoneyLifeDisplay` code. Check callers and `.sdata` alignment before attaching such functions to either neighbour.
- **A word load with byte extraction means bitfields, not bytes.** A struct of `s8` fields compiles to byte loads; this index is read as one word and written with `rlwimi`, which only bitfields produce.
- **A copied argument before a virtual call means a by-value parameter.** The callee looks the same for `T` and `const T&`; only the caller's stack copy distinguishes them.
