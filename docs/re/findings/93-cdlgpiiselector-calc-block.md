# CDlgPiiSelector update and draw block (#93)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #93 lists 27 functions in `.text` `0x80093540`–`0x80093F64`. All of them belong to `game/CDlgPiiSelector.cpp`, whose split, object line and first function belong to #91 ([F-91-1](91-cdlgpiiselector.md)). The file is compiled without inlining ([F-92-1](92-cdlgpiiselector-construction.md#f-92-1-the-file-is-compiled-without-inlining)).

## F-93-1: identities of the 27 functions

Slots are counted from the first function word of a CodeWarrior vtable, so slot `n` is at byte offset `8 + 4n`. The selector vtable `0x8039F9A8` is listed in [F-92-2](92-cdlgpiiselector-construction.md#f-92-2-identities-of-the-23-functions).

| Address | Function | Evidence |
| --- | --- | --- |
| `0x80093540` | `shared_ptr<CDlgPiiSelector>::operator->` | Already matched by #91 |
| `0x80093550` | `CDlgPiiSelector::Draw` | Slot 4 of `0x8039F9A8`. Returns early when slot 5 of the object from `IGameMap` slot 15 is true. Otherwise draws each set `CDataPerPlayer`, then the `MultiPlayerData` layout at 0x88 (slot 4) and the shared pointer at 0x170 (slot 4) |
| `0x80093630` | `IGameMap` slot 15, inline default returning a null pointer | Word `0x803A1D44` of the `IGameMap` vtable `0x803A1D00` ([F-116-2](116-cgameending-unit.md#f-116-2-class-hierarchy)). Every caller of slot 15 is a `NoticeDisplay` script binding, and `ScriptBinding_IsNoticeDisplayShown` returns slot 5 of the result, so slot 15 returns the map's `INoticeDisplay` (RTTI string `INoticeDisplay`) and Draw skips while the notice display is shown. `CGameMap` overrides it with `0x800AA540` (member 0x98) |
| `0x80093640` | `shared_ptr<CDataPerPlayer>` boolean conversion | Five-instruction test of the pointer word, called with `mDataPerPlayer[i]` |
| `0x80093660` | `CDataPerPlayer::Draw` | Calls slot 4 through the shared pointer at 0x84 |
| `0x800936A0` | `shared_ptr<CUIManager>::operator->` | The `CDataPerPlayer` constructor `0x800952B0` stores `new(0x54)` built by `0x801C9680`, the `CUIManager` constructor ([F-82](82-cdlgpiilist.md), #890) |
| `0x800936B0` | `shared_ptr<CDataPerPlayer>::operator->` | |
| `0x800936C0` | `shared_ptr<MultiPlayerData>` boolean conversion | Called with the member at 0x2C |
| `0x800936E0` | `shared_ptr<MultiPlayerData>::operator->` | |
| `0x800936F0` | `shared_ptr<IDlgMsgWin>::operator->` | `ShowMultiPlayerEntrance` (`0x80094DA0`) assigns the result of the `CDlgMsgWin` factory `0x80084D40` ([F-73-1](73-cdlgmsgwin.md)) to `MultiPlayerData` + 0x170 through `0x8009BE60` |
| `0x80093700` | `CDlgPiiSelector::CalcView` | Slot 3; each `CDataPerPlayer`, then slot 3 of the dialog at 0x170 |
| `0x800937A0` | `CDataPerPlayer::CalcView` | Slot 3 of the `CUIManager` |
| `0x800937E0` | `CDlgPiiSelector::CalcMtx` | Slot 2; same shape |
| `0x80093880` | `CDataPerPlayer::CalcMtx` | Slot 2 of the `CUIManager` |
| `0x800938C0` | `CDlgPiiSelector::Calc` | Slot 1. Calls `0x80093A10` when `MultiPlayerData` is set. For each set `CDataPerPlayer` it calls `0x80093DB0`; when `0x80093F80` (#94) then returns true it resets the pointer (`0x8009BD80`), clears `mIsPausing` and calls `IGameMap` slot 29 with `false` if it was set, and calls slot 13 (`UpdateDlgOpenState`). Then, while `MultiPlayerData` is set, it calls `0x80094240`, then slot 1 of the layout at 0x88 and of the dialog at 0x170 |
| `0x80093A10` | `CDlgPiiSelector::CalcEntrance` | Calls `0x80093A70` when `MultiPlayerData::mState` is 0 or 1, through two separate cases |
| `0x80093A70` | `CDlgPiiSelector::AssignControllers` | Builds a `CControllerList` on the stack, then for players 0 to 3 passes the list entry, or null when the player index is not below `MultiPlayerData` + 0x84, to `0x800F81C0` on `IGameSession` slot 23 (`GetPlayer`). `ShowMultiPlayerEntrance` also calls it after creating the players' `CDataPerPlayer`s |
| `0x80093B20` | `CControllerList::CControllerList()` | Clears the count and four pointers |
| `0x80093B40` | `CControllerList::Collect()` | Takes the connected GameCube pads first, then fills from `TYPE_CLASSIC`, `TYPE_WII_REMOTE` and `TYPE_NUNCHAKU` until full, and falls back to Wii Remote 0 when nothing is connected |
| `0x80093C50` | `CControllerList::AddType(IController::Type)` | Adds channels 0 to 3 of one type; true once the list is full |
| `0x80093CD0` | `CControllerList::Add(IController*)` | True when full; ignores null and unconnected controllers (`IsConnected`, slot 0); stores into the first empty slot. The loop-alignment `nop` of [F-735-1](735-game-compiler.md) |
| `0x80093DA0` | `CControllerList::Get(int)` | |
| `0x80093DB0` | `CDataPerPlayer::Calc` | When the state word at 4 is 2, compares the player's controller (`CPlayer` + 0xC, read through the weak accessor `0x800859B0`) and its `GetDisconnectSerial` (slot 12) with the words at 0xC4 and 0xC8 and sets the `bool` at 0xCC on a change; while that flag is clear it updates the `CUIPlayerID` at 0xAC through `0x8011F9C0`. Then it calls slot 13 of the `CUIPiiList` at 0xA4, looks up its current entry (`0x801DB250`), and when one exists passes an object built from it (through `0x8008FE10`, `0x8008BA60`, `0x8008BA50` and `0x80089680`) to slot 13 of the `CUIPiiSpec` at 0x9C with the player index at 8. Finally it calls `Calc` of the `CUIManager` and resumes the `CFiber` at 0xD8 (`0x8019E460`) |
| `0x80093F20` | `CDataPerPlayer` accessor of the `bool` at 0xCC | Inline copy emitted after its only caller |
| `0x80093F30` | `shared_ptr<CUIPlayerID>::operator->` | Member 0xAC, `new(0x13C)` built by `0x8011F800` |
| `0x80093F40` | `shared_ptr<CUIPiiList>::operator->` | Member 0xA4, `new(0x180)` built by the `CUIList` constructor and vtable `0x8039E2A8` |
| `0x80093F50` | Boolean conversion of the shared pointer returned by `0x801DB250` | Destroyed through `0x8007C440`; the element type is not recovered |

`CControllerList`, `CalcEntrance`, `AssignControllers`, `Collect` and `AddType` are descriptive names with low confidence; the class has no vtable and no RTTI. `0x80124A20` is declared as the free function `GetController(IController::Type, int)`. It returns entry `[type][channel]` of the 4 × 4 `IController*` table `0x803B3030`, whose initializer `0x801248C0` stores channels 0 to 3 in each entry and whose update `0x80124980` calls `IController::Update` (slot 13) on all sixteen. The name is descriptive, medium confidence. `0x800F81C0` is `CPlayer::SetController(IController*)`: it stores the word at 0xC, which the pad bindings and `0x800859B0` read. That name is descriptive, high confidence.

The `CDataPerPlayer` layout, from its constructor `0x800952B0`: three constructor arguments at 0, 4 and 8 (owner, state, player index), and the same UI members as `CDlgPiiListImpl` ([F-82](82-cdlgpiilist.md)): `shared_ptr<CUIManager>` at 0x84, two `shared_ptr<BUILayout>` at 0x8C and 0x94, `shared_ptr<CUIPiiSpec>` at 0x9C, `shared_ptr<CUIPiiList>` at 0xA4, `shared_ptr<CUIPlayerID>` at 0xAC, a shared pointer at 0xB4, words at 0xC4, 0xC8 and 0xD0, the `bool` at 0xCC, and a `CFiber` at 0xD8 whose vtable pointer is at 0x440. Its own vtable pointer is at 0x448.

`MultiPlayerData` is 0x180 bytes (constructor `0x800950F0`): the state word at 0, `shared_ptr<CPiiPersonalData>[4][4]` at 4 indexed by controller type and channel (`SetEntrancePPD`), the player count at 0x84 (set by `ShowMultiPlayerEntrance` from a value clamped by `0x80095280` against the constant at `0x804B64A8`), a `CLayout` by value at 0x88 (constructor `0x801CB1C0`; RTTI `CLayout`, vtable `0x803C0AF4`, derived from `ICalcDraw` with destructor `0x801CC070`, `Calc` `0x801D62F0` and `Draw` `0x801D63B0`), `shared_ptr<IDlgMsgWin>` at 0x170 and a shared pointer at 0x178.

## F-93-2: reconstruction of eight functions

`src/game/CDlgPiiSelector.cpp` now declares `CControllerList`, `MultiPlayerData` up to the player count, and `CalcEntrance` and `AssignControllers` as inline members of `CDlgPiiSelector`, with `include/game/CPlayer.h` and `include/game/ControllerTable.h` for the two outside functions. `config/wpse-v1/symbols.txt` gives the eight functions and the two outside functions the mangled names the source produces.

These members are not emitted yet. Everything in the file is in the anonymous namespace, and its only callers are `Calc` and `ShowMultiPlayerEntrance`, so the compiler drops them while those virtuals are undefined, and the object, the objdiff report (31/530 at 100%) and the linked DOL are unchanged. To check them, a scratch build added a call to `CalcEntrance` from the factory. All eight functions then compiled to the original instructions at `0x80093A10`, `0x80093A70`, `0x80093B20`, `0x80093B40`, `0x80093C50`, `0x80093CD0`, `0x80093DA0` and `0x800936E0`, apart from relocation words. They were also emitted in the original depth-first order: `CalcEntrance`, `operator->`, `AssignControllers`, the constructor, `Collect`, `AddType`, `Add`, `Get`. The scratch call is not committed. Two shape details matter. The constructor must clear the count before a four-pass loop over the pointers. `Collect` must store each pad pointer before testing it, and clear the slot again when the pad is not connected.

## F-93-3: what blocks the rest of the block

The remaining 19 functions are emitted only from `Draw`, `CalcView`, `CalcMtx` and `Calc`, and each of those needs types that other issues own:

- `CUIManager` and the other UI members of `CDataPerPlayer` (#327 and #328). `CDataPerPlayer::Draw`, `CalcView`, `CalcMtx` and `Calc` call virtuals through them.
- `CLayout` (#327 and #328), held by value at `MultiPlayerData` + 0x88. Its 0xE4 bytes of members also fix the offset of the dialog at 0x170.
- `IDlgMsgWin` (#72 and #73), which has no header yet. `IGameMap` (#116 and the `CGameMap.cpp` blocks) through slot 29, and `INoticeDisplay` for `Draw`. The binding scan below names only some of the 25 `IGameMap` slots from 5 to 29.
- `0x80093F80`, `0x80093F70` and `0x80094240` are in #94, and `0x8009BD80` (`reset`) is in a later block.

A scan of every caller of the map getter `0x800AA2B0` names some `IGameMap` slots. It reads the `lwz r12, X(r12)` that follows the getter before `r3` is reloaded. Slot 7 is called by `EnableCalcWorld`, `IsCalcWorldEnabled` and the effect finalizers, and `CGameMap` returns its `BWorld` (member 0x40). Slot 9 is called by `ShowMiniMap`, slot 10 by the selector's entrance bindings ([F-91-2](91-cdlgpiiselector.md#f-91-2-factory-and-session-interface)), slot 15 by the notice-display bindings, slot 16 by the money and life display bindings, slot 17 by `ShowBRDisplay` and `HideBRDisplay`, slot 19 by the layout bindings (`NewLayout`, `method_layout*`), and slots 36 to 40 by the timer bindings. `Pause` and `Resume` call slot 29 with `true` and `false`, and `IsPausing` calls slot 30, both through a second getter `0x800AA2F0`. Slots 5, 6, 8, 11 to 14, 18 and 20 to 28 have no binding evidence yet. No `IGameMap` header is added here.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf` and the rebuilt object, the generated assembly under `build/wpse-v1/asm/`, Wii/1.1 with `cflags_game` through `ninja` and `tools/re/unitdiff.py`, and a Python word comparison that skips relocated and branch words (`93-cmp.py` and `93-mapslots.py` in the session scratch directory). No decompiler output was used.

## Traps

- **Unreferenced members of an anonymous-namespace class are not emitted.** Defining `CalcEntrance` and `AssignControllers` out of line added nothing to the object, because they have internal linkage and their only callers (`Calc`, `ShowMultiPlayerEntrance`) are not yet defined. To test such a function, reference it temporarily from an emitted function, and do not commit that reference. Check `nm` for the symbol before you trust an unchanged objdiff count.
- **An inline virtual of an interface can sit in another file's block.** `0x80093630` sits between `Draw` and its helpers, but it is the `IGameMap` slot 15 default. This file's copy is kept because `CDlgPiiSelector.cpp` links before `CGameEnding.cpp` and `CGameMap.cpp`. Check the word against the interface vtables before you assign it to the selector.
- **`ninja` targets on Windows use backslash paths.** `ninja build/wpse-v1/src/game/CDlgPiiSelector.o` reported `no work to do` for a changed source. `ninja 'build\wpse-v1\src\game\CDlgPiiSelector.o'` builds it.
