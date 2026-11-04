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

## F-141-4: complete typed MoneyLife producer and remaining native context

The complete ordinary producer now exists in `src/game/CMoneyLifeDisplay.cpp` with its anonymous class in `include/game/detail/CMoneyLifeDisplay.h`. It remains `NonMatching`. The factory is its sole retained entry point; there are no emission drivers or retention directives. The class size is 0x5C. The phase member at +8 occupies 0x1C bytes, ending at +0x24; the earlier 0x24-byte member-size claim in F-141-2 is superseded. Its actual type is the existing `CDisplayPhaseCtrl`. The display-mode enum and member/method spellings are inferred from behavior; the class and anonymous source-unit spelling come from RTTI.

| Original entry | Complete typed producer | Native bytes | Result |
| --- | --- | --- | --- |
| `800C4960` | `IMoneyLifeDisplay::Create` | 2384 | Six instruction words differ |
| `800C52B0` | `IMoneyLifeDisplay` destructor | 64 | Code and actual targets exact |
| `800C52F0` | `CMoneyLifeDisplay::Hide` | 24 | Exact |
| `800C5310` | `CMoneyLifeDisplay::Show` | 24 | Exact |
| `800C5330` | `CMoneyLifeDisplay::SetForceVisible` | 24 | Exact |
| `800C5350` | `CMoneyLifeDisplay::Draw` | 88 | Exact |
| `800C53B0` | `CMoneyLifeDisplay::Calc` | 832 | Exact |
| `800C56F0` | shared control-block `get_deleter` | 76 | Exact |
| `800C5740` | shared control-block `dispose` | 36 | Exact |
| `800C5770` | shared-owner destructor | 132 | Exact |
| `800C5800` | raw-allocation guard destructor | 116 | Exact |
| `800C5880` | implicit `CMoneyLifeDisplay` destructor | 260 | Exact |
| `800C5990` | shared control-block destructor | 64 | Exact |

All actual target bindings were checked against the frozen DOL, including all literal payloads, virtual-table slots and cleanup calls. The 12 nonfactory functions total 1740 exact instruction bytes. The nine owned exception records total 1572 exact bytes; the two borrowed guard records add 16 exact bytes. All three virtual tables total 100 exact bytes at their native positions. These are producer-level results, not a Matching acceptance claim.

The full source-selected link preserves all 13 function addresses, every section address and extent, all exception tables and indices, and every foreign section byte. Its remaining differences are 22 text bytes in the factory's six words and 85 data bytes. Factory +0x40..+0x4C load the map-index word after the vtable store rather than before it. Factory +0x820 and +0x824 exchange the `0x4330` conversion constant load and unsigned-short return narrowing. The two data blocks are the meaningful `ICalcDraw` and `shared_ptr_deleter_common` RTTI name records: native `ICalcDraw` begins at data +0x2D4, while the producer places the common control-block name first at +0x2CC and `ICalcDraw` at +0x314. No padding, forced flags, fabricated retention or raw executable data was added to conceal these failures.

The original #141 inventory remains 23 functions. Its other ten entries are the CMsgBalloon continuation documented in F-141-3 (`800C59D0`, `800C5A60`, `800C5AE0`, `800C5B00`, `800C5B10`, `800C5BC0`, `800C5BD0`, `800C5C10`, `800C5C30`, `800C5CC0`). That source is independently owned by its current whole-unit contributor. This checkpoint neither replaces that source nor claims its acceptance or issue closure.

### Native interface and ownership corrections

The final three pure virtual slots of `IMoneyLifeDisplay` are implemented by the typed producer and preserve all existing inherited slots. The original 0x44-byte data symbol at `803A4F4C` contains a 0x1C-byte base descriptor followed by the 0x28-byte `IMoneyLifeDisplay` vtable at `803A4F68`; the corrected symbols separate those actual objects.

Factory exception record `8001D700` calls `8007DD50`. A real typed `new CUIManager` producer proves that 116-byte function and its eight-byte exception record are the `Metrowerks::move_ptr<CUIManager>` guard destructor. The former name at `8011A990` came from incomplete #217 inference; its contributor confirmed there is no retained independently accepted producer establishing that identity. That older function returns to its address placeholder until its own evidence establishes a type. The corresponding actual BUILayout guard is `8007DDD0`, called by factory exception record `8001D730`, also exact for all 116 code and eight exception bytes. These are borrowed native identities, not new source coverage.

The factory reads the local default motion-pane pointer at `804B9020`. Its actual four-byte zero-initialized symbol belongs to this TU; the old eight-byte extent included four unreferenced bytes. Only `[804B9020,804B9024)` is assigned to this source. No identity or ownership is inferred for `804B9028` from its proximity. The earlier assertion that this TU has no `.sbss` is superseded.

The factory genuinely allocates the life-layout array twice and registers the money layout twice. Both repetitions remain in the source. The idle motion string is `"normal"`, not `"loop"`; comparing only relocation kinds and code size would miss that semantic error.

### Rejected bounded controls

Wii/1.3 reproduces exactly the same six instruction differences as Wii/1.1 with the normal game flags. Genuine unadapted 2006 MSL headers also retain the wrong relative RTTI-name order; this does not establish a new library contract. Pointer/reference map arguments, ordinary inline constructor-definition placement, direct versus named frame conversion, and explicit empty base-destructor declarations did not resolve the remaining differences. A u32 `GetMotionLength` return declaration plus explicit u16 narrowing is also unchanged; the canonical u16 interface remains intact. No shared-header change is proposed from these negative controls.

An explicit empty derived destructor rotates the retained destructor/helper block even when individual bodies are exact. Leaving the derived destructor implicit restores every native function position and complete exception-index order. This is observed for this producer; it is not a general rule that explicit destructors are incorrect.

Reproducible outside artifacts are retained under `C:/MassStorage/GitHub/141-reference`: `141-compile.py`, `141-audit-code.py`, `141-audit-strict.py`, the code/EH/vtable JSON receipts, `141-source-link-proof.json`, the full linked image, and the genuine-primary reading-control object. They preserve the failed source-selected link separately from any successful NonMatching fallback build.

## Traps

- **Exact individual bodies can still have the wrong source order.** This producer's explicit empty derived destructor changes retained placement. Its implicit destructor restores native code and exception-index order without an emission fixture; F-141-4 records the bounded result.
- **A plausible template name can be attached to the wrong earlier function.** The real factory cleanup records identify the CUIManager allocation guard at `8007DD50`, superseding the unaccepted #217 inference at `8011A990`. Validate actual cleanup targets, not only destructor size.
- **String relocation identity is part of the proof.** The idle motion literal must be `"normal"`; a different same-shaped string can leave instruction comparison apparently exact.

- **A unit can sit between a deleter destructor and the next survey class.** `0x800C4810`–`0x800C495C` follow `CMiniMap`'s deleter destructor but are neither `CMiniMap` nor `CMoneyLifeDisplay` code. Check callers and `.sdata` alignment before attaching such functions to either neighbour.
- **A word load with byte extraction means bitfields, not bytes.** A struct of `s8` fields compiles to byte loads; this index is read as one word and written with `rlwimi`, which only bitfields produce.
- **A copied argument before a virtual call means a by-value parameter.** The callee looks the same for `T` and `const T&`; only the caller's stack copy distinguishes them.

## 2026-11-04 scalar snapshot controls

A private scalar packed-index constructor can load the map descriptor before the vptr store, but does not close Create. Reinterpreting the scalar as MapIndex or using a union introduces a stack copy and enlarges the frame; explicit sign-extraction arithmetic avoids that copy but changes extract/vptr scheduling and register choice. Rotation-and-mask extraction reproduces the bitfield instruction selection but does not restore the scheduling. Scalar/reference and 64-bit coercion variants were discarded. Main source and its private header remain unchanged.
