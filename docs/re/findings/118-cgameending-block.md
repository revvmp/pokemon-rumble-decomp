# Issue 118: CGameEnding.cpp 3/10 (0x800A6450 to 0x800A7650)

Target `wpse-v1`. The block holds 30 functions in `game/CGameEnding.cpp` (F-116-1, F-117-1): the three getters already named by #117, the nine `IGameEnding` implementations, `CGameEnding::~CGameEnding`, and the weak instances the factory `0x800A4EF0` causes. The `.text` end `0x800A7650` is the `CGameMap` constructor, so the block ends at `0x800A7650` as the issue estimated.

## F-118-1: the 30 functions

| Address | Size | Identity | Evidence |
| --- | --- | --- | --- |
| `0x800A6450`-`0x800A6470` | `0x8` | `GetPauseMenu`, `GetMapDesc`, `GetWorld` | Named by F-117-1 |
| `0x800A6480` | `0x8` | `IsNextVisible`, slot 51 | `CLayout::IsVisible` (`0x801D07B0`) of the layout at 0x220 |
| `0x800A6490` | `0xC` | `StopNext`, slot 50 | `CLayout::StopMotion` (`0x801D13A0`) of the layout at 0x220 with the file's `spDefaultMotionPane` (`.sbss` `0x804B8FF8`) |
| `0x800A64A0` | `0x11C` | `ShowNext`, slot 49 | `CreateMotion("begin")` discarded, `AppendMotion("loop")` discarded, `AppendMotion("end")` stored at 0x308, then `SetVisible(true)` |
| `0x800A65C0` | `0x3C` | `HideStaffRoll`, slot 47 | Hides the layout at 0x114, state 0 |
| `0x800A6600` | `0x44` | `IsStaffRollFinished`, slot 48 | `!(state == 1 \|\| (state == 2 && !mLineTexts.empty()))` |
| `0x800A6650` | `0xD4` | `StartStaffRoll`, slot 46 | Shows the layout at 0x114, `CreateMotion("in")` stored at 0x1FC, hides pane `SCROLL`, state 1 |
| `0x800A6730` | `0x5F0` | `ParseStaffRollText(const wchar_t*)`, slot 45 | See F-118-2 |
| `0x800A6D20` | `0xC` | `StopCongratulations`, slot 44 | `StopMotion` of the layout at 0x20 |
| `0x800A6D30` | `0xBC` | `ShowCongratulations`, slot 43 | `CreateMotion("normal")` stored at 0x108, then `SetVisible(true)` |
| `0x800A6DF0` | `0x8` | `IGameMap::GetPauseMenu` default | The inherited `return nullptr` weak instance; `CGameEnding` overrides it with `0x800A6450` |
| `0x800A6E00`-`0x800A6F50` | `0x4C`/`0x24` | `get_deleter` and `dispose` of `shared_ptr_deleter` for `CGameEnding`, `BWorld`, `CShadowCamera` | Compare the deleter type-name string and call slot 2 of the owned pointer |
| `0x800A6F80`, `0x800A7010`, `0x800A70A0` | `0x84` | `~shared_ptr` of `CGameEnding`, `IPauseMenu`, `BWorld` | Factory extab: `0x7010` and `0x70A0` destroy the members at 0x18 and 0x10 |
| `0x800A7130`, `0x800A71B0`, `0x800A7230` | `0x74` | `~move_ptr` of `CGameEnding`, `BWorld`, `CShadowCamera` | Locals 0x58, 0x38 and 0x40 of the factory, in construction order |
| `0x800A72B0` | `0x60` | `~alloc_ptr` of the `list<wstring>` node holder | Locals 0x90 and 0x98 around the `push_back`s of the factory |
| `0x800A7310` | `0x274` | `CGameEnding::~CGameEnding` deleting destructor | Destroys 0x308, `CLayout` 0x220, both lists inline, 0x1FC, 0x114, 0x108, 0x20, 0x18, 0x10 and the weak count at 4 |
| `0x800A7590`, `0x800A75D0`, `0x800A7610` | `0x40` | `~shared_ptr_deleter` for `CShadowCamera`, `BWorld`, `CGameEnding` | Vtables `0x803A1A30`, `0x803A19BC`, `0x803A1908` (F-116-1) |

The `IGameEnding` slot names of F-117-5 are replaced where the bodies disagreed: slot 44 and slot 50 are `CLayout::StopMotion` on the layouts at 0x20 and 0x220, not localisation calls; slot 45 takes the staff roll text. `0x801D13A0` takes a pane (default `spDefaultMotionPane`), looks up its motion programs in the `std::map<Pane*, shared_ptr<PaneMP>>` at 0xBC, sets the state word of each queued program to 3 (ended), clears the queue and recurses into the child panes, so `StopMotion` is descriptive (medium). `0x801D07B0` returns bit 0 of the flag word, or, when bit 1 is set, the byte at 0xBB of the root pane, so `IsVisible` is descriptive (medium). `0x8008E3B0` is the one-argument `AppendMotion`, not `QueueMotion`; the one-argument `QueueMotion` is `0x800703C0`.

## F-118-2: ParseStaffRollText

The argument is a wide string of lines separated by `L'\n'`. A line starting with U+25C6 is a title: the line list gets `L""` and the title list the line without the marker. Any other line goes to the line list with `L""` in the title list. `Calc` (F-117-2) then shows `LINES` from the line list and `TITLES` from the title list. A default-constructed `wstring` is live for each line and unused.

The loop is one `for (;;)` with an `if` that scans in its `else` branch (`++pEnd`); the scan as a separate `while` produces the bottom-tested shape instead. `include/string` gains the two constructors the function inlines: the range constructor (`reserve(last - first)` then `replace`) and the copy constructor (three-word copy when short, `reserve` and `replace` when long, `init()` only in the long path).

## F-118-3: what matches

With the throwaway emitter of F-117-4 (a temporary exported function that built `shared_ptr<CGameEnding>`, `shared_ptr<BWorld>` and `shared_ptr<CShadowCamera>`; not committed), `tools/re/unitdiff.py game/CGameEnding` reported 58 of 66 functions at 100% and the 30 functions of this block as follows, after the symbols of this block received their mangled names:

- 100%: the eight small `IGameEnding` implementations differ only in string-literal relocation names (`@NNNN` against the split `lbl_` names, 99.65% to 99.89%), an artifact of the partial `.sdata`; `~CGameEnding`, the six deleter `get_deleter`/`dispose`, the three deleters, the three `~move_ptr`, the three `~shared_ptr`, the `~alloc_ptr` and the `GetPauseMenu` default are exact.
- `ParseStaffRollText` 94.3%: the two `std::wstring(L"")` temporaries differ. The target keeps the string's address in `r25` and the length in `r24` (`_savegpr_24`), mine rematerializes the address (`_savegpr_25`). The range constructor temporaries match. Moving the literal constructor body into a helper, naming the temporary, and the implicit `push_back(L"")` all gave the same result.
- `Calc` 98.17% (F-117-4), unchanged.

The committed object is still empty: no code uses `CGameEnding` until the factory `0x800A4EF0` (#116, unclaimed) does, so the unit stays `NonMatching` and the original object stays linked.

## Traps

- **Check the callee before naming a one-argument `CLayout` wrapper.** `0x8008E3B0` and `0x800703C0` have the same shape (`lwz r6, sbss; lfs; lfs; b`) and sit 0x1E000 apart; only the tail-call target (`AppendMotion` against `QueueMotion`) tells them apart.
- **A default argument that names a TU static compiles to an `.sbss` load at the call site.** `CLayout::StopMotion(Pane* = spDefaultMotionPane)` is a single `lwz` before the tail call, which is why `0x800A6490` and `0x800A6D20` read `.sbss` `0x804B8FF8`.
- **A separate `while` scan loop changes block layout.** The same source written as a pre-test `while` followed by the body produced a `b` to the loop test; a single `for (;;)` with the scan in an `if`/`else` reproduced the target.
