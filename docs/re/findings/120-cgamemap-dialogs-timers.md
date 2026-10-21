# Issue 120: CGameMap.cpp 5/10 (0x800A98E0 to 0x800AA4E8)

Target `wpse-v1`. The block lies inside `game/CGameMap.cpp` (F-119-1), whose split, class layout and first 23 functions came from #119. Neighbouring blocks: #119 (preceding), #121 and #122 (following, same file).

## F-120-1: the 25 functions

| Address | Size | Identity | Match |
| --- | --- | --- | --- |
| `0x800A98E0` | `0xF8` | `CGameMap::RunAction()` | 100% |
| `0x800A99E0` | `0x17C` | `CGameMap::RegisterDialog(const shared_ptr<IDlg>&)` | 90.95% |
| `0x800A9B60` | `0xB0` | `CGameMap::UnregisterDialog(const shared_ptr<IDlg>&)` | 100% |
| `0x800A9C10` | `0x49C` | `CGameMap::DisappearModelObjs()` | 98.4% |
| `0x800AA0B0` | `0x8C` | `CGameMap::AppearModelObjs()` | 99.1% |
| `0x800AA140` | `0x168` | `IGameMap::Create` | 100% (#119) |
| `0x800AA2B0`, `0x800AA2F0` | `0x40` | `GetCurrentGameMap()`, `GetActiveGameMap()` | 100% |
| `0x800AA330`-`0x800AA3E0` | small | `GetTimerFrame`, `IsTimerWorking`, `StopTimer`, `AddTimer`, `StartTimer`, `IsInEvent`, `ExitEvent`, `EnterEvent`, `IsHitstopping` | 100% |
| `0x800AA3F0`, `0x800AA420`, `0x800AA430` | | `Hitstop`, `SetPausing`, `GetCurrentDialog` | 100% |
| `0x800AA4A0`-`0x800AA4E0` | `0x8` | `GetPostEffect`, `GetNumericInput`, `GetEnemyInfo`, `GetMsgBalloon`, `GetTutorialMsg` | 100% |

Evidence for the shapes:

- **`RunAction`.** Sets the default PPU environment and stage target as `Calc` does, locks the ground weak pointer at `BWorld` `+0x50`, tests the `CDividedGround` type bit (`1 << 15`) and calls `LoadPkc(1, <empty environment>, nullptr)`. The temporary empty `shared_ptr` is created after the ground lookup.
- **Dialog stack.** `RegisterDialog` rejects an empty argument, accepts the dialog already registered, and otherwise asks the current dialog to `Close()` (vtable offset `0x20`, not `IsClosable` at `0x1C`); only a successful close lets the new dialog replace it. `UnregisterDialog` clears the weak pointer only when the argument is the current dialog. Comparisons use `.get()`; the `shared_ptr` `operator==` adds two boolean conversions the target does not have.
- **Model objects.** `DisappearModelObjs` sets `mModelObjsHidden`, collects the objects of types `CPii`, `CItem` and `1 << 16` (mask `0x10840`), and for each visible one stores its id in `mHiddenObjIDs` and calls `SetVisible(false)` (slot at `0x28`). `AppearModelObjs` shows every remembered id still in the world and clears the list.
- **Inline slots.** `AddTimer`, `Hitstop` and `GetCurrentDialog` are defined in the class body: the unit's function order (reverse declaration order of inline virtuals) places them between their neighbours, which an out-of-line definition would not. `AddTimer` adds to `mTimerFrame` and, for a positive amount, calls vtable offset `0x24` of the battle royal display, which is `Finish` in the current `IBattleRoyalDisplay.h` (low confidence for that name; the call count matches only the one virtual).
- **`BWorld::GetGround`.** The target locks the weak pointer; the previous `shared_ptr<BWorldObj>(mGround).get()` added the `bad_weak_ptr` path. With `mGround.lock().get()` the `ScriptCamera` copy of `GetGround` rises from 99.75% to 100% and `RunAction` matches.

## F-120-2: what blocks the last three

- **`DisappearModelObjs`.** Instruction stream and branch layout agree. The three inlined `length_error` temporaries of `push_back` get stack slots in a different order (first and second swapped, `0x50`/`0x38` against `0x38`/`0x50`), and a few registers follow. Reordering the branches of `push_back_realloc` did not change it.
- **`AppearModelObjs`.** The same loop in the target holds the vector size in `r4` and the data pointer in `r3`; mine swaps them. Declaring the pointer inside an `if`, moving `clear()` after the flag store, and writing `end()` as `begin() + size()` (85%) did not help.
- **`RegisterDialog`.** The null test of the argument is scheduled before the prologue stores in the rebuild and after them in the target. `!rDialog`, `get() == nullptr` (93.7%), a `bool` local and an `else` form did not reproduce it.

## Traps

- **PowerShell aliases `r` to `Invoke-History`.** A helper function named `R` in a PowerShell script never ran and the edit was lost silently; do scripted edits in Python.
- **`shared_ptr::operator==` in this MSL adds two boolean conversions.** Comparing raw `get()` results is what the target does in `UnregisterDialog` and `RegisterDialog`.
- **Check the virtual offset before naming an `IDlg` call.** `Close` is `0x20`; `IsClosable` is `0x1C`.
