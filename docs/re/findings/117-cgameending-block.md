# Issue 117: CGameEnding.cpp 2/10 (0x800A5BF0 to 0x800A6444)

Target `wpse-v1`. The block holds 21 functions in the file whose split and object line #116 owns (`game/CGameEnding.cpp`, F-116-1): sixteen weak `IGameMap` defaults and the `CGameEnding` overrides `RunAction`, `Draw`, `CalcView`, `CalcMtx` and `Calc`. The survey title names `CGameMap` and `CGameResult`, which are later files.

## F-117-1: the 21 functions

| Address | Size | Identity | Evidence |
| --- | --- | --- | --- |
| `0x800A5BF0`-`0x800A5CE0` | `0x4`/`0x8` | Sixteen inline defaults of `IGameMap`: `GetCurrentDialog` (`0x800A5BF0`), `GetPostEffect`, `GetNumericInput`, `GetEnemyInfo`, `GetMsgBalloon`, `GetTutorialMsg` (`0x800A5C40`), `GetLayoutManager`, `GetComboDisplay`, `GetBattleRoyalDisplay`, `GetMoneyLifeDisplay`, `GetDlgMsgWin`, `GetDlgInfo`, `GetDlgCollection`, `GetDlgPiiList`, `GetMiniMap`, `GetWorldObjStatDisplay` (`0x800A5CE0`) | Vtable `0x803A1AC8` slot by slot against `include/game/IGameMap.h`; each body is `li r3, 0` or `blr` |
| `0x800A5CF0` | `0xA0` | `CGameEnding::RunAction`, slot 5 | Runs the script `CGameEnding.pkc` (`0x803A1AB8`) through `0x80102220`; see F-117-3 |
| `0x800A5D90` | `0x78` | `CGameEnding::Draw` | Draws the world, then the layouts at 0x20, 0x114 and 0x220 |
| `0x800A5E10` | `0x50` | `CGameEnding::CalcView` | World at 0x10, then the pause menu at 0x18 |
| `0x800A5E60` | `0x50` | `CGameEnding::CalcMtx` | Same two objects |
| `0x800A5EB0` | `0x594` | `CGameEnding::Calc` | Drives the staff roll; see F-117-2 |

The vtable slots of `CGameEnding` (`0x803A1AC8`) confirm the `IGameMap` numbering of `IGameMap.h` for the slots this block touches: slot 5 `RunAction` (`0x800A5CF0`), slot 6 `GetMapDesc` (`0x800A6460`, returns the word at 0xC), slot 7 `GetWorld` (`0x800A6470`, returns the pointer of the `shared_ptr` at 0x10), slots 8 to 19 and 21 to 26 the defaults above, slot 20 `GetPauseMenu` (`0x800A6450`, returns the pointer of the `shared_ptr` at 0x18) and slots 27 to 42 as named in `IGameMap.h`.

## F-117-2: CGameEnding layout and Calc

Anonymous-namespace class `@unnamed@CGameEnding_cpp@::CGameEnding` is 0x310 bytes. The offsets are fixed by the accesses in this block and by F-116-3:

| Offset | Member | Evidence |
| --- | --- | --- |
| 0x0C | `const MapDesc*` | Returned by slot 6 `GetMapDesc` |
| 0x10 | `shared_ptr<BWorld>` | Returned by slot 7 `GetWorld`; `Calc`, `CalcMtx`, `CalcView` and `Draw` call it first |
| 0x18 | `shared_ptr<IPauseMenu>` | Returned by slot 20 `GetPauseMenu`; `CalcMtx` and `CalcView` forward to it. F-116-3 took this second `shared_ptr` for the shadow camera; the slot 20 override and the `ICalcDraw` slots called on it show it is the pause menu, as in `CMultiRanking` (F-159) |
| 0x20 | `CLayout` (0xE8 bytes) | `congratulations`; its motion is the `shared_ptr` at 0x108 |
| 0x108 | `shared_ptr<CLayout::CMotionProgram>` | Reset and the layout hidden once `IsEnded` |
| 0x110 | `int` state | 0 idle, 1 and 2 staff roll running or scrolled |
| 0x114 | `CLayout` | `staff_roll`; its scroll motion is the `shared_ptr` at 0x1FC |
| 0x1FC | `shared_ptr<CMotionProgram>` | `scroll` motion of pane `SCROLL` |
| 0x204 | `int` | Line count of the `LINES` text box |
| 0x208, 0x214 | `list<wstring>` | Line texts and title texts; each node is 0x14 bytes |
| 0x220 | `CLayout` | `next`; its motion is at 0x308 |

`CLayout` is 0xE8 bytes (F-141, F-93-4), which places the layouts at 0x20, 0x114 and 0x220 with the motion pointers at 0x108, 0x1FC and 0x308 and the state at 0x110.

`Calc` sets outline alpha 255 and width 1, calls `Calc` of the world and the layouts at 0x114, 0x220 and 0x20 and, in state 1 or 2 once the scroll motion has ended, pops the first line and title, restarts `scroll` on pane `SCROLL`, joins the remaining texts of both lists into two `wstring`s with newline separators (padding with separators up to the line count), shows them in the `LINES` and `TITLES` text boxes and sets state 2. It then hides `next` and `congratulations` when their motions end.

## F-117-3: RunAction needs IGameSession slots 25 to 32

`RunAction` calls `IGameSession` slot 32 (offset 0x88) with `(0, -1)` and slot 26 (offset 0x70) to fetch a `shared_ptr` that it passes to `0x80102220("CGameEnding.pkc", 1, ...)`. `include/game/IGameSession.h` declares slots through 24. The `CGameSession` vtable `0x803A395C` gives:

| Slot | Implementation | Behaviour |
| --- | --- | --- |
| 25 | `0x800B91A0` | Returns `this + 0x28` |
| 26 | `0x800B91B0` | Copies the `shared_ptr` at `+0x84` (retaining the count) to the result; the environment handed to `0x80102220` |
| 27, 28 | `0x800B91E0`, `0x800B9340` | Take an index and a `shared_ptr`; not read further |
| 29 | `0x800B94A0` | Calls slot 24 (`GetPlayer`) and tests a byte at +0x10 |
| 30 | `0x800B9750` | Seconds as `f32` from `OSGetTime` and the words at +0x10 to +0x1C |
| 31 | `0x800BAD60` | Returns the word at +0xC58 (read by `CMultiRanking`, F-159) |
| 32 | `0x800B97E0` | Stores a fade colour quad at +0xC60 (type 0 clear, 1 black, 2 white) and a frame count at +0xC64 (15 when negative) |
| 33 | `0x800B98F0` | Returns the frame count at +0xC64 |

The unevidenced names of slots 25 and 27 to 31 block `RunAction`; it is declared but not defined. `0x80102220` is a free function that executes a script with a `CPpuEngine` (the `new(0xC4)` of F-195) and is not yet named.

## F-117-4: what matches, and why the source emits nothing

`src/game/CGameEnding.cpp` declares the class, the five overrides and the inline getters. A temporary exported function that constructed `CGameEnding` (the stand-in for the factory `0x800A4EF0`, #116) made the compiler emit the vtable and every weak function. In that build `tools/re/unitdiff.py` reported 35 of 66 functions at 100% and `Calc` at 98.15%:

- all sixteen `IGameMap` defaults of this block, `Draw`, `CalcView`, `CalcMtx`, the three getters `0x800A6450` to `0x800A6470`, and the other `IGameMap` defaults emitted here;
- the destructors `IGameMap::~IGameMap` (`0x800A5980`) and `IGameEnding::~IGameEnding` (`0x800A59F0`) of #116, once `IGameMap` defines its destructor inline.

The stand-in was removed before committing, because it would be an invented factory. Without a constructor use CodeWarrior emits no vtable, so the committed source compiles to an empty object and the unit report shows 0 of 66 until #116 adds the factory. Once the factory constructs `CGameEnding`, this file's functions appear at the percentages above.

`Calc` differs from the target in three ways, all unexplained: the six `forward_iterator_tag` temporaries of the inlined `wstring::operator+=` sit at mirrored stack offsets (the target allocates them upward in code order, this build downward); the loop counter and the two list iterators take `r28`, `r30`, `r29` instead of `r30`, `r29`, `r28`; and the two `__unexpected` landing pads of the motion assignment swap their stack offsets (0x88 and 0xA0). Reordering declarations changed none of them.

`include/string` gains `append` and `operator+=`. The target needs `size()` first, then the by-value tag copy, then `wcslen` for the literal overload, and the short/long split of `assign(const basic_string&)` for the string overload. Other formulations tried (a named tag, `begin()` and `end()` calls) gave a different instruction order.

## F-117-5: IGameEnding

The nine pure slots of `IGameEnding` (slots 43 to 51) are implemented in #118's range: `0x800A6D30`, `0x800A6D20`, `0x800A6730`, `0x800A6650`, `0x800A65C0`, `0x800A6600`, `0x800A64A0`, `0x800A6490`, `0x800A6480`. `include/game/IGameEnding.h` names them from their bodies, with low confidence: `ShowCongratulations` (creates the motion on the layout at 0x20 and shows it), `LocalizeCongratulations` and `LocalizeNext` (tail calls of `0x801D13A0` with the layout and one `.sbss` word; the callee is unidentified), `BuildStaffRollText` (the 0x5F0-byte function that allocates and fills the two `wstring` lists), `StartStaffRoll` (shows the layout at 0x114, starts a motion, hides pane `SCROLL`, sets state 1), `HideStaffRoll` (hides it, state 0), `IsStaffRollFinished` (true unless state 1 or 2 with lines left), `ShowNext` (creates the motion at 0x308 and shows layout 0x220) and `IsNextVisible` (`0x801D07B0` on the layout at 0x220). Rename them when #118 reconstructs the bodies.

## Traps

- **An unused anonymous-namespace class compiles to nothing.** CodeWarrior emits the vtable, and with it every inline virtual, only when a constructor is used. Declaring undefined virtuals `inline`, as in `CDlgPiiSelector.cpp`, does not change that. To measure a class before its factory exists, add an exported function that constructs it, read `unitdiff.py`, then remove it.
- **A missing `IGameMap` destructor body hides two weak functions.** `IGameMap` declared `~IGameMap()` without a body, so `__dt__8IGameMapFv` and `__dt__11IGameEndingFv` could never be emitted. They are inline in the original.
