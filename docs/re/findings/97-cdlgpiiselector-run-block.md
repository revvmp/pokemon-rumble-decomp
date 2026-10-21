# CDlgPiiSelector CDataPerPlayer fiber block (#97)

Target `wpse-v1`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #97 lists 23 functions in `.text` `0x80096960`-`0x80097794`. All belong to `game/CDlgPiiSelector.cpp` ([F-91-1](91-cdlgpiiselector.md)), compiled without inlining ([F-92-1](92-cdlgpiiselector-construction.md#f-92-1-the-file-is-compiled-without-inlining)). [F-97-3](#f-97-3-reconstruction-of-the-block) reconstructs the block: every function compares at 100% in `tools/re/unitdiff.py` apart from the pool label in `RunSelection` (99.95%, see below).

## F-97-1: identities of the 23 functions

| Address | Size | Function | Evidence |
| --- | --- | --- | --- |
| `0x80096960`, `0x80096970` | 0x4 | `VEC3::operator const Vec*() const` and `operator Vec*()` | Already named; emitted by the `Transform` helper |
| `0x80096980` | 0x8 | `nw4r::lyt::Pane::GetGlobalMtx() const` | Already named |
| `0x80096990` | 0x10 | `nw4r::math::VEC3::VEC3(float, float, float)` | Already named |
| `0x800969A0`, `0x800969D0` | 0x30, 0x14 | `VEC2::operator=` and `_VEC2::operator=` | Three-word and two-word `lfs`/`stfs` pairs; used by the `mReservedPosition` stores in `Setup` |
| `0x800969F0` | 0x78 | `std::tr1::bind(void (CDataPerPlayer::*)(), CDataPerPlayer*)` | Called by `Setup` before `CFiber::Start`; builds a `tuple`, a `mem_fn_0` and the `bound_func` |
| `0x80096A70` | 0x48 | `bound_func<void, mem_fn_0<void, CDataPerPlayer>, tuple<CDataPerPlayer*>>` constructor | Copies the 12-byte pointer-to-member and the 4-byte tuple through their implicit copy constructors (`0x80096AC0`, `0x80096AE0`) |
| `0x80096AC0`, `0x80096AE0` | 0x1C, 0xC | `mem_fn_0` and one-element `tuple` copy constructors | Word-for-word copies |
| `0x80096AF0` | 0x3C | `std::tr1::mem_fn(void (C::*)())` | Spills the pointer-to-member to a stack temporary and forwards it to `0x80096B30`; the two-argument `mem_fn` overload already exists |
| `0x80096B30` | 0x1C | `mem_fn_0(Function)` constructor | The member-initializer copy stores word 1 before word 0 |
| `0x80096B50` | 0xC | `tuple<CDataPerPlayer*>(CDataPerPlayer* const&)` constructor | Takes its argument by reference: `bind` stores the pointer to a stack slot first |
| `0x80096B60` | 0x60 | `CDataPerPlayer::Run()` | Fiber entry. States 0 and 1 call `YieldFiberStarted`, then the dialog flow at `0x80096BC0`; state 2 calls the entrance flow `0x8009A420`; `SetOpen(false)` runs afterwards |
| `0x80096BC0` | 0x1A0 | `CDataPerPlayer::RunDialog()` | Waits for the remote memory, stores the controller (`0xC4`) and its disconnect serial (`0xC8`), fills `mItems` with `0x800928A0`, opens and runs the selection loop, then plays the hide motions and waits for the list's |
| `0x80096D60` | 0x9C | `CDataPerPlayer::WaitForRemoteMemory()` | For controller types 1 to 3 (Wii Remote, Classic, Nunchaku) calls `CRemoteMemory::WaitUntilIdle(true)` on `GetRemoteMemory(GetChannel())` (`IGameSession` slot 26, offset 0x68) |
| `0x80096E00` | 0x18 | `CDataPerPlayer::SetOpen(bool)` | Stores the byte at `0x50`, then tail-calls slot 13 (offset 0x3C) of the owner, which is `UpdateDlgOpenState` |
| `0x80096E20` | 0x73C | `CDataPerPlayer::RunSelection()` | The selection loop; see [F-97-2](#f-97-2-the-selection-loop) |
| `0x80097560` | 0x9C | `CDataPerPlayer::UpdateList(bool)` | Calls `CPiiListFilterSort::Filter` (`0x800E5DB0`) with a converted copy of the list pointer, `mItems` and the player (-1 in state 2); selects index 0 when asked |
| `0x80097600` | 0x4C | `shared_ptr<CUIList>(const shared_ptr<CUIPiiList>&)` | Converting copy constructor, called with `r5 = 0` |
| `0x80097650` | 0x4 | `CUIPiiList::SelectIndex(int)` | Tail call to `CUIList::SetSelectedIndex` (`0x801DB1F0`); called only with 0, while the direct `CUIList` call is used with a variable index |
| `0x80097660` | 0x120 | `CDataPerPlayer::SelectEntrancePii()` | Asks the owner for the entrance `CPiiPersonalData` of the player's controller (slot 12) and selects the matching list index, else index 0 |
| `0x80097780` | 0x14 | `shared_ptr<CPiiPersonalData>::operator bool_type` | Already named |

## F-97-2: the selection loop

`RunSelection` sets the silent flag (`0x51`), loads the list, plays the show motions for the spec, player id, help and list layouts (the helper `0x80097820`, which marks the layout visible and starts a `CreateMotion` whose name depends on the layout and direction), waits for the list's motion, hands the keyboard focus to the list (`CUIManager` `0x801C9850`), clears the silent flag and loops once per frame (`YieldFiber`).

Each iteration reads the player's controller and handles, in order:

- a disconnected controller: sets mode 4 unless the current map is the even stage 7 map (`0x80092E70`, which calls `IsStage7EvenMap` on the map index);
- an unplugged or disabled controller: sets mode 4 in state 2;
- otherwise, unless the input-disabled byte (`0x52`) is set, a trigger of mask `0x90` (cancel; plays sound 90 and, unless the map is the even stage 7 map or the player's Pii is fainted, clears the entrance Pii and sets mode 4), mask `0x800` (mode 6, the favourite toggle), mask `0x100` in state 1 (mode 7, the sort menu) or forwarding the input to the `CUIManager` (`0x801C9D60`).

The loop then takes the mode (`0x80097B00` reads the word at `0xC` and clears it) and dispatches 5 (close request: adds one to the statistic key -99, sound 41, return), 4 (sound 41, return unless the even stage 7 map), 7 (`CPiiListFilterSort` sort menu at `0x800E6FA0` with a `function<void(const CPiiListFilterSort*)>` bound to `OnSortChanged`, `0x80097CC0`, which calls `UpdateList(true)`), 6 (toggles the selected item's favourite flag, sound 9 or 90, and stores the new state in the byte at `0x10`, which `OnSelectionChanged` follows) and 3 (accept; `0x80097CD0` returns 2 to cancel with sound 41, 0 to proceed). A proceeding accept applies the choice (`0x80097F40`); in state 2 it processes the entrance result (`0x80098EF0`), and when the word at `0xD0` is not 2 it shows the list and loops again. Otherwise it runs the Pii swap sequence (`0x80099400`, `0x800999F0`, `0x8009A0E0` with a 0x24-byte stack record holding a `shared_ptr<CPiiPersonalData>` and two `VEC3`), plays the hide motions for the spec, list, player id and help layouts and returns after `0x800990F0`.

## F-97-3: reconstruction of the block

All 23 functions are reconstructed in `src/game/CDlgPiiSelector.cpp` and compare at 100% in `tools/re/unitdiff.py`. Only `RunSelection` is at 99.95%: the local label `@NNNN` that stands for the pointer-to-member constant at `.data` `0x8039F6FC` differs from `lbl_8039F6FC` because the partial unit's `.data` is smaller. The full build and `verify.py` still pass with the original objects linked.

The following declarations are shared and were added for this block. A per-function comparison of `report.json` against a fresh baseline build of `origin/main` shows no regression in any unit; the only changed names are the three renamed auto units for `0x80113760`, `0x801C9850` and `0x801C9D60`.

| File | Change | Evidence |
| --- | --- | --- |
| `include/tuple` | The one-element constructor takes `const T0&` (or `T0&` for a reference element) through `detail::tuple_argument` | `bind` spills the object pointer to the stack and passes its address to `0x80096B50`; the `tuple<A&>` instance used by `bound_func::operator()` still needs a reference |
| `include/functional` | `mem_fn(R (C::*)())` and the zero-argument `bind` calls it | `0x80096AF0` |
| `include/memory` | `shared_ptr(detail::unused_argument*)` and `operator==` between two `shared_ptr` | `0x80097AF0` stores two zero words but is called with `r4 = 0` from this block, `ScriptBinding_method_setGenerator` and `CPiiListFilterSort`, so the null literal converts through a pointer parameter; `0x800977C0` compares the `get()` words |
| `include/game/CUIPiiList.h` | `SelectIndex(int)` | `0x80097650` |
| `include/game/CUIManager.h` | `SetFocusedUI(BUI*)` (`0x801C9850`) and `DispatchFocusedInput(const IUIInput*)` (`0x801C9D60`) | The second name is from [F-328](328-encoding-converter.md); the first pairs with the existing `GetFocusedUI` and `mFocusedUI` |
| `include/game/CStatistic.h` | `Add(int, int)` (`0x80113760`) | Called with -99 and 1 by `RunSelection` and with the same signature shape by the swap sequence; descriptive name, low confidence |
| `include/game/CPiiListFilterSort.h` | `Filter(shared_ptr<CUIList>, vector&, int)` (`0x800E5DB0`) and `ShowSortMenu(shared_ptr<CUIManager>, int, vector&, function<void(const CPiiListFilterSort*)>)` (`0x800E6FA0`) | [F-173-2](173-cpiilistfiltersort.md#f-173-2-functions-of-this-block); `Filter` takes a base-class list pointer because the call site uses the converting copy constructor (`r5 = 0`) and the destructor of `shared_ptr<CUIList>` (`0x8007C170`) |

The `CDataPerPlayer` members renamed or added: the byte at `0x50` is `mOpen`, `0x52` is `mInputDisabled`, `0xBC` to `0xBF` are `mSpecShown`, `mListShown`, `mPlayerIDShown` and `mHelpShown` (the byte at `0xBC + index` is read and written by the motion helper `0x80097820`), and the word at `0xD0` is `mEntranceResult`. The names `RunDialog`, `RunSelection`, `WaitForRemoteMemory`, `SetOpen`, `TakeMode`, `UpdateList`, `SelectEntrancePii`, `PlayMotion`, `ValidateAccept`, `ApplyAccept`, `ProcessEntranceAccept`, `FinishAccept`, `CanSwap`, `PrepareSwap`, `ExecuteSwap`, `SwapRequest`, `LoadItems` and `IsCurrentMapStage7Even` are descriptive, medium to low confidence. The members and free functions that live in the following blocks (`PlayMotion`, `TakeMode`, `OnSortChanged`, `ValidateAccept`, `ApplyAccept`, `ProcessEntranceAccept`, `FinishAccept`, `CanSwap`, `PrepareSwap`, `ExecuteSwap`, `RunEntrance`, `LoadItems`, `IsCurrentMapStage7Even`) are declared only; their symbols in `symbols.txt` carry the mangled names of those declarations so the unit compares, and the owners of #98 to #110 define them. `symbols.txt` also gives the vector iterator helpers `0x8008C090`, `0x8008C0C0`, `0x8008E6C0`, `0x8008E7B0`, `0x8008E7C0`, `0x800977A0` and `0x800977C0`, `get` `0x80097A30`, the `bind`, `function` and `shared_ptr<CUIManager>` copy used by the sort menu call (`0x80097B70`, `0x8009C700`, `0x8008EF90`, `0x80097B20`), the null `shared_ptr` (`0x80097AF0`) and `SwapRequest` (`0x80099950`, `0x80099990`) their mangled names.

## Traps

- **A control flow with a temporary in a condition needs a goto-shaped exit.** `if (pPii == nullptr || !pPii->GetPersonalData()->IsFainted())`, a nested bool, and `&&` with an empty `if` all compile to `li` flag materialisation or to different register numbers (96.9% to 98.4%). The original jumps from the null test straight into the body and from the fainted test over it. `do { ... break; ... } while (false)` reproduces it exactly (100%); a `goto` label does the same.
- **`switch` and `if` chains differ.** The mode dispatch with cases 5, 4, 7, 6, 3 matches only as a `switch` in that order: an `else if` chain returns before the other branches and reorders the blocks. The same holds for the two-way accept result.
- **A plain-looking `shared_ptr` constructor with an extra register can be another constructor.** `0x80097AF0` is called with `r4 = 0` and ignores it; the default constructor `0x80094CE0` takes no second argument. Passing the literal `0` to a `const shared_ptr&` parameter selects it.
- **Two identical 12-byte functions are not interchangeable.** `0x800977A0` is both the vector iterator's pointer constructor and its copy constructor by instruction stream. The call site decides: `iterator it = v.begin()` here calls the copy constructor, so pairing by instruction signature alone chose the wrong name once.
- **`nm` output prefixes undefined symbols with `U`.** Taking the last whitespace-separated field is not enough when a name contains no spaces but the script splits on the first; strip the type letter before writing a name to `symbols.txt` (18 lines were written as `U name = ...` and caught by `git diff`).

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf`, a `bl` scan of its full disassembly for callers, and Wii/1.1 with `cflags_game` through `ninja` and `tools/re/unitdiff.py`. Target symbols were renamed to the object's mangled names by comparing instruction streams with relocation names removed. IDA was not used; no decompiler output was used.
