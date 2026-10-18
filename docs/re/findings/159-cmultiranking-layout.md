# Issue 159: CMultiRanking layout and what blocks its source

Target `wpse-v1`, analysed in the dtk 1.8.4 split assembly of `game/CMultiRanking.cpp` (split by #158, F-158-1) and with scratch compiles under `cflags_game`. No decompiler output was used.

## F-159-3: members, virtual slots and emission order

### Members

The factory `0x800CD800` allocates 0x144 bytes and initializes the object inline; `0x800CF650` (slot 0) destroys the members in reverse order. Types come from the deleter vtables and constructors the factory uses:

| Offset | Type | Evidence |
| --- | --- | --- |
| 0x4 | `enable_shared_from_this<IGameMap>` | RTTI base list `0x803A5700`; released last by the destructor |
| 0xC | `int` | First factory argument; returned by slot 6 (`0x800CF410`) and passed to the `CPauseMenu` factory `0x800D83B0` |
| 0x10 | `shared_ptr<BWorld>` | `new BWorld(desc)` (0x90 bytes); returned by slot 7 (`0x800CF420`) |
| 0x18 | `shared_ptr<BWorldCamera>` | 0x68-byte object passed to `BWorld::AddCamera` and `SetCurrentCamera` |
| 0x20 | `shared_ptr<CUIManager>` | Deleter vtable `0x8039C81C`; constructor `0x801C9680` stores the `CUIManager` vtable `0x803C02DC` (bases `BUI`, `ICalcDraw`) |
| 0x28, 0x30 | `shared_ptr<BUILayout>` | Deleter vtable `0x8039C898`; constructor `0x801C9EE0` |
| 0x38 | `shared_ptr<IPauseMenu>` | Assigned from `0x800D83B0`, which builds `@unnamed@CPauseMenu_cpp@::CPauseMenu` (vtable `0x803A7B24`, base `IPauseMenu`, `ICalcDraw`); returned by slot 20 (`0x800CF400`) |
| 0x40 | `shared_ptr<BModelObj>[4]` | `new BModelObj(0x20)` per model name; passed to `GetNodePosition` (F-158-2) |
| 0x60 | `shared_ptr<CPii>[4]` | 0x14C-byte objects from `0x800DCA10`, which stores the `CPii` vtable `0x803A84A8` (base `BModelObj`); `CalcMtx` calls their `SetPosition` |
| 0x80 | `shared_ptr<BUILayout>[4]` | 0x134-byte objects from `0x801C9EE0` |
| 0xA0 | `shared_ptr<CUIPlayerID>[4]` | 0x13C-byte objects from `0x8011F800` (vtable `0x803AF7F0`, `CUIPlayerID`), stored through `0x800CE9F0` |
| 0xC0 | `int[4]` | Per-player value read from the session player record at 0x68 |
| 0xD0 | `shared_ptr<...>[4]` | Element flags at 0, a float pointer at 0x24 and an `int` at 0x2C are read inline by `Calc`; type not recovered |
| 0xF0, 0xF4 | `bool[4]` each | Cleared by the factory; per-player states in `Calc` |
| 0xF8 | `nw4r::math::VEC3[4]` | Offsets added to the node positions in `CalcMtx` |
| 0x128, 0x12C, 0x130 | `int` | Maximum of 0xC0, count of active players, and a frame counter that `Calc` wraps at 90 |
| 0x134 | `nw4r::snd::SoundHandle` | The destructor calls `DetachSound` on it |
| 0x138, 0x13C | `int` | `Calc` counts finished players in 0x13C and compares with 0x138 |
| 0x140, 0x141 | `bool` | 0x140 is whether the `IGameSession` virtual at vtable offset 0x84 returns 1; `Calc`, `CalcMtx`, `CalcView` and `Draw` do nothing while 0x141 is false |

Member names are not recovered; the table is descriptive.

### Virtual slots

The vtable `0x803A563C` has the 43 `IGameMap` slots (F-116-1). `CMultiRanking` overrides slot 0 (`0x800CF650`), the four `ICalcDraw` slots (`Calc` `0x800CF1D0`, `CalcMtx` `0x800CF070`, `CalcView` `0x800CF000`, `Draw` `0x800CEFA0`), the three pure `IGameMap` slots 5 (`0x800CEB20`, a fiber script that plays `begin` and the ranking motions), 6 and 7, and slot 20 (`0x800CF400`). `IMultiRanking` adds no slots. In CMultiRanking the default of slot 15 (`0x80093630`, F-93) is not overridden.

### Emission order

The five overrides from `0x800CEB20` to `0x800CF1D0` lie in the reverse of vtable order, between the factory's weak helpers and the three one-instruction getters. A scratch unit with a factory, three out-of-line virtual overrides defined after it and inline virtuals in the class compiled under `cflags_game` to: factory, the weak helpers it references, the out-of-line overrides in source order, then the inline virtuals, the destructor and the `shared_ptr_deleter` members. The original has the same shape, so the slot 5, `Draw`, `CalcView`, `CalcMtx` and `Calc` overrides are out-of-line and defined in that order after the factory, and slots 6, 7 and 20, the destructor and the deleter members (`get_deleter` `0x800CF430`, `dispose` `0x800CF480`, destructor `0x800CF860`) are inline. Without the factory, an out-of-line override still emits the vtable and every inline virtual, because it is the key function. Confidence medium.

### What blocks the source

- The factory formats `5:/event/ranking.arc/%s.brres` and the `P%d` pane names through the `basic_string` formatter `0x800721A0`; it needs MSL `basic_string` (#327).
- Every override calls through `CUIManager`, `IPauseMenu`, `CPii` or `CUIPlayerID`, which have no headers yet; the `BUI` and `CUIManager` interfaces are being recovered under #327.
- Emitting the vtable needs the 35 inline `IGameMap` slots, none of which has a recovered name (F-116-1).

No `CMultiRanking` source beyond F-158-2 is committed by #159.

## Traps

- **Out-of-line overrides can sit between weak functions.** Under `-ipa file` a function defined after the factory is emitted after the weak helpers the factory references, so a run of overrides after weak code is not evidence that the overrides are inline.
