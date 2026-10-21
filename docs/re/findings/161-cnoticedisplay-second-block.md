# Issue 161: the second CNoticeDisplay.cpp block

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`, analysed in the dtk 1.8.4 split assembly under `build/wpse-v1/asm/` and compared with `tools/re/unitdiff.py`. No decompiler output was used. The block is `0x800D1030`-`0x800D2BD8` of `game/CNoticeDisplay.cpp`, whose split, object line and header are #159's (F-159-1).

## F-161-1: the 12 functions and what the source reproduces

The issue lists 12 functions: three `Run` fiber bodies, the three constructors, the three `Init` functions and three `shared_ptr` `operator->` helpers. `python tools/re/unitdiff.py game/CNoticeDisplay.cpp` after this change:

| Address | Function | objdiff | Remaining difference |
| --- | --- | --- | --- |
| `0x800D1030` | `CSimpleText::Run` | 99.86% | Literal-pool relocations only |
| `0x800D1270`, `0x800D1780`, `0x800D1BA0` | `shared_ptr<CSimpleText>`, `<CFight>`, `<CLose>` `operator->` | 100% | None |
| `0x800D1280` | `CFight::CFight(int)` | 99.78% | Vtable relocation name |
| `0x800D1340` | `CFight::Init` | 99.27% | `li`/`mr` constant materialisation, see F-161-3 |
| `0x800D15D0` | `CFight::Run` | 99.77% | Literal-pool relocations only |
| `0x800D1790` | `CLose::CLose(int)` | 99.76% | Vtable relocation name |
| `0x800D1840` | `CLose::Init` | 99.72% | Literal-pool relocations only |
| `0x800D19F0` | `CLose::Run` | 99.81% | Literal-pool relocations only |
| `0x800D1BB0` | `CYouWin::CYouWin(int, int, float)` | 99.82% | Vtable relocation name |
| `0x800D1CA0` | `CYouWin::Init` | 97.95% | F-161-3 |

`Run` is the fiber body that `Init` stores with `std::tr1::bind(&Class::Run, this)`; `0x800D1CA0` is the 0xF38-byte `CYouWin::Init`. `CYouWin::Run` is `0x800D2BE0` (#162). The three `Run` functions and the two `Init` functions compile to the original size byte for byte; every row left over is a literal-pool relocation, a vtable name that needs the full `.data` layout of the file, or one of the items in F-161-3.

Names. `CFight`, `CLose` and `CYouWin` are RTTI names. `Init`, `Run`, `StartNotice` and the member names are descriptive (low confidence); `Run` is the body bound to the fiber, hence the name. `fn_800D1030`, `fn_800D15D0` and `fn_800D19F0` are renamed to the mangled `Run__...` spellings in `symbols.txt`.

### Behaviour

- **`CSimpleText::Run` (`0x800D1030`).** Creates the `begin` motion, queues `normal`, appends `end` and yields the fiber once. Type 3 plays sound `0x5C`. Until the `end` motion has ended (`CMotionProgram` state 3) it reads the UI input: with the pause released (`mPauseReleased`), a pressed button sets `mCloseRequested`. A close request restarts the `end` motion when it has not started, then the request is cleared and the fiber yields. At the end it releases the pause (`IGameMap::SetPausing(false)`) if an auto-close count is set and the pause has not been released.
- **`CFight` (`0x800D1280`-`0x800D15D0`).** `Init` loads `event/br_count.arc/event2d.arc` with `LoadFileList`, loads the layout `beginning_count` from the returned entry, sets `AREA` to the stage message and `GRADE` to `FormatAreaName`, and starts the fiber. `Run` creates `begin`, appends `3`, `2`, `1` and `end`, waits for each motion in turn and plays sound `0x5B` after the first three, `0x32E` after the last countdown motion, releases the pause and waits for `end`.
- **`CLose` (`0x800D1790`-`0x800D19F0`).** Constructor stores state 0 and the type. `Init` loads the shared layout `lose`, or `timeup` for type 9. `Run` plays `begin`, `normal`, appends `end`, plays sound `0x5C`, restarts `end` on a state-1 request, and at the end calls `IGameSession` slot 32 with `(1, 0)`.
- **`CYouWin::Init` (`0x800D1CA0`).** Loads the base layouts (the same sequence as `CSimpleText`, so it is `BOther::LoadLayouts`), then `event/br_end.arc/event2d.arc` into a `clear_time` layout and a `prize` layout. It converts the winner parameter to centiseconds (`ceilf(param * 100.0f)`), formats minutes, seconds and hundredths into the message `0x600E4` with `NIntl::basic_formatter<wchar_t>` (`% format_spec(L"%d") % minutes` and so on) and sets `CLEARTIME`, `TIME`, `TIME_SHADOW` and `NEWRECORD`. In mode 2 the area comes from byte `0x12FB` of the save slot (`SlotData::mData.mStatus[19]`, a signed value); unless `0x801251C0` (a map number below 3) rejects it, the best time of `CBattleRoyalBox::GetData(mode, area)` is updated with the winner parameter and the result shows or hides `NEWRECORD`. It then loads the `prize` layout, empties `MONEY` and `MONEY_SHADOW`, shows `AREA`, `STAGE` and `YOUWIN`, hides the two extra layouts and starts the fiber.

## F-161-2: new types and shared declarations

- **`include/game/NIntl.h`.** `NIntl::basic_formatter<CharT>` came from #167 (`CPauseMenu.cpp`) in parallel; this issue extends it. Layout from `CYouWin::Init` and F-347-3: format string `+0x00`, vector of argument strings `+0x0C`, the current conversion specification `+0x18` (`mScratch`), result `+0x24`, formatted flag `+0x30` (size 0x34). `operator%(const format_spec&)` is inline and assigns the specification (`wstring::operator=(const wchar_t*)`, the inline `replace`); `operator%(const int&)` is `0x801F34E0` (numeric arguments are passed by reference, F-347-3; the mangled name is `__md__Q25NIntl18basic_formatter<w>FRCi`, not the `Fi` guess of #167); five sibling instances sit at `0x801F26E0`, `0x801F2DE0`, `0x801F3BF0`, `0x801F4300` and `0x801F4A10`. `str()` is the inline formatting loop. It now has the shape the original code needs: `int c = mFormat[i]` compared with `cmpwi` and copied into a `CharT` only in the literal-character branch, the placeholder tests in `if (bad) { throw } else { ... }`, `isdigit` (the narrow map at offset 8 of the `ctype` component, not the wide map), and `mResult.append(mArgs[index - 1])` rather than `append(begin, end)`. These changes lift `FormatMessage<PCw,PCw>` and `<PCw,PCw,PCw>` in `CPauseMenu.cpp` from 77% to 91.7%.
- **`include/game/CBattleRoyalBox.h`.** `CBattleRoyalData` is the 4-byte `f32` best time (F-52); `UpdateBestTime(f32)` is `0x8006A550`. `CBattleRoyalBox::GetData(int mode, int area)` is `0x8006A820` (a `shared_ptr` copy from `table[mode][area]`, stride 0x28 and 8). `IGameSession` slot 10 returns the box, not a `CBattleRoyalData`; the slot is renamed `GetBattleRoyalBox`.
- **`IGameSession` slot 10.** #167 declared slots 25 to 42; this issue only needs `SetFade(int, int)` (slot 32, called as `(1, 0)`). Slot 10 returns the `CBattleRoyalBox`, so it is renamed `GetBattleRoyalBox`.
- **`CLayout::AppendMotion(const char*)`.** The weak default-pane overload is `0x8008E3B0`, renamed `AppendMotion__7CLayoutFPCc` (scope weak).
- **Names for called functions.** `IsBattleRoyalFirstPart(MapIndex)` `0x801251C0` (named by #167; true when the map number is below 3), `UpdateBestTime` and `GetData` take their compiler spellings in `symbols.txt`; `0x801F34E0` is `operator%(const int&)`.
- **`basic_string`.** `size()` and `data()` test `!is_long()` instead of `is_short()`: the original builds the boolean `x == 0` once (`cntlzw`; `srwi.`) and reuses it across a loop body, and a bool-returning helper makes the compiler branch on the raw bit. The whole DOL still verifies and the `CPauseMenu.cpp` and `SaveData.cpp` function counts are unchanged.
- **`MSL_C/ctype.h` and `math.h`.** `isdigit` (narrow map, `& 8`) and `ceilf` (`(float)ceil(x)`). The formatter calls `isdigit` on a wide character, as the original does.

## F-161-3: what keeps the unit from 100%

The unit stays `NonMatching`: #159, #160, #162 to #164 own the rest of the file and the build links the original object. The leftovers in this block are compiler behaviours not yet explained, not missing functions.

1. **Constant zeros are merged in the source build and not in the original.** `LoadFileList(&entry, path, 0, false, nullptr)` and `LoadResourceFile(path, 0, false)` compile to `li r6,0; li r8,0; ...; mr r7,r6` and `li r5,0; mr r6,r5`. The original has `li r6,0; li r7,0; li r8,0` and `li r5,0; li r6,0` (the other ten `LoadFileList` callers and the `CSimpleText` call, F-160-3). A lab compile outside the repository (same flags, `Wii/1.1`) reproduces the merge in every context that has a `shared_ptr` result with an inline destructor under `-ipa file`, and reproduces `li` only when one of these holds: the callee's `bool` is `int`, `-ipa` is `off`, inlining is `off` or `noauto`, `#pragma peephole off`, `#pragma exceptions off`, the result is a type with an out-of-line destructor, or the result is assigned to an existing `shared_ptr`. None of them keeps the rest of the unit matching (see the Traps). All Wii builds from 1.0RC1 to 1.5 behave the same. The merge is the `-opt peep` constant-copy peephole, so the original's call arguments do not look like equal constants to it.
2. **Frame-address hoists.** The original keeps the address of a local `std::string` short buffer or of the formatter (`addi r24, r31, 0xed`, `addi r25, r31, 0x1e0`, `addi r27, r31, 0x1e2`) in a non-volatile register before a loop or a run of calls and uses `mr`. The source build recomputes `addi r3, r31, off` at each use. The same difference is the `ENEMY_IMG%d` loop of `CSimpleText::Init` (F-160-3). No `#pragma opt_*` and no `-opt` flag changes it.
3. **Register numbers in `CYouWin::Init`.** `mode`, `area`, `newRecord` and `eligible` take different non-volatile registers (`r28`, `r25`, `r24`, `r30` in the original). Any permutation of their declaration order scores lower. The time conversion and the formatter loop are otherwise identical instruction for instruction.
4. **A tag temporary slot.** The `forward_iterator_tag` temporary of the single-character `append` is at stack offset `0x30`, in the original at `0x34`.

Measured result: `CYouWin::Init` 97.95% (3896 of 3896 bytes), `CFight::Init` 99.27%, every other function of the block at its literal-pool and vtable limit. `python configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## Traps

- **Inline depth decides the stack layout of temporaries.** `CFight::Init`, `CLose::Init` and `CYouWin::Init` match only when their body is one inline level deeper: `Init` returns `StartNotice()`, which holds everything. For the formatter, `StartNotice` (1), the constructor (2) and the string constructor (3) put the constructor and specification temporaries in increasing order, and a `str()` that contains the formatting loop (no separate helper) puts the throw temporaries in decreasing order. Adding or removing a trivial wrapper is the cheapest way to move a whole group of `forward_iterator_tag` and exception temporaries.
- **A throw in an `if`-only branch kills common subexpressions.** `if (bad) throw X;` followed by the rest makes the compiler reload `is_long` and the data pointer because the throw call can fall into the rest. `if (bad) { throw X; } else { rest }` keeps the values in registers and reproduces the original layout (throw block before the continuing block, then `b` to the loop increment).
- **`int c = pattern[i]` and `CharT ch = c`.** The original compares the character with `cmpwi` and stores it to the stack only in the else branch. A `CharT c` local stores it before the comparison and compares with `cmplwi`.
- **`std::string::size()` and `data()` need `!is_long()`.** The original builds the boolean `x == 0` once (`cntlzw`; `srwi.`) and reuses it for every access in a loop body.
- **`#pragma peephole off`, `-O4,s`, `-ipa off`, `-inline noauto` are not the answer.** Each makes the constants separate but breaks many matching functions of this unit (peephole off: 9 of 102 at 100%; `-O4,s`: 10; `-ipa off`: 25; `noauto`: 26). Do not apply them to the unit.
- **A scratch script that restores a backup can silently undo hours of work.** Experiment scripts that write a fixed backup of the source back at the end overwrote the working copy with an old snapshot. Scripts that vary the source must save the file's own content at start and restore that, and the work should be committed first.
- **`iswdigit` is the wrong digit test for the formatter.** With `wint_t` as `int` the wide macro reads `wctype_map_ptr` (offset 0x14); both `CYouWin::Init` and `CPauseMenu.cpp` read offset 8, the narrow `ctype_map_ptr`, so the formatter calls `isdigit`.
