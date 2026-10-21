# Issue 146: CMsgBalloon warp setup, formatter and string helpers

Target `wpse-v1`, checked with `tools/re/unitdiff.py src/game/CMsgBalloon.cpp`, objdump of the linked `build/wpse-v1/main.elf`, a full `ninja` and `tools/verify/verify.py`. No decompiler output was used.

## F-146-1: the 32 functions of 0x800C7EF0-0x800C88C8

Result: every function of the block compares 100% in objdiff apart from relocation names of callees and literals that other issues own or that need the whole file (`SetupBattleWarp` 99.27%, `SetupStageWarp` 99.56%, `NIntl::basic_formatter<wchar_t>::operator%(const wchar_t*)` 99.62%, `parse` 99.48%, `basic_string<wchar_t>::buffer` 99.21%, `operator+=` 95.00%; the other 26 are 100.00%). Confidence high. Full `ninja` links and `verify.py` passes; per-function scores of every other unit are unchanged against the baseline `report.json`.

| Address | Function |
| --- | --- |
| `0x800C7EF0`-`0x800C7F30` | `CLayout::GetViewRect`, `ut::Rect::GetWidth`, `GetHeight`, `lyt::Pane::SetTranslate`, `CPiiPersonalData::GetHP` (inline accessors declared in F-145-4) |
| `0x800C7F40`-`0x800C8020` | `list<Entry>::iterator` default constructor, assignment, `operator->` (`0x800C7F60`), `operator*` (`0x800C7FE0`), `operator++`, `operator!=`; `shared_ptr<BWorldObj>` bool conversion and `operator->`; `BWorldObj::IsVisible` and `GetStateFlags`; `Entry::ClearFlags` |
| `0x800C8030` | `CMsgBalloon::SetupBattleWarp(StageID)` (descriptive name, medium) |
| `0x800C83C0`, `0x800C83D0` | `CBattleRoyalRecord::GetClearTime`, `shared_ptr<CBattleRoyalRecord>::operator->` |
| `0x800C83E0`, `0x800C83F0` | `ceil(float)` and `ceilf(float)` |
| `0x800C8420`, `0x800C8460`, `0x800C84A0` | `NIntl::basic_formatter<wchar_t>`: `operator%(const wchar_t*)`, `str()`, `parse()` |
| `0x800C8650`, `0x800C8690` | `basic_string<wchar_t>::operator[]` and its private buffer accessor |
| `0x800C86E0` | `isdigit(int)` |
| `0x800C8720`, `0x800C8740`, `0x800C8750` | `vector<wstring>::size`, `operator[]` and the `__vec_constructor` layer below it |
| `0x800C8730` | `basic_string<wchar_t>::operator+=(const basic_string&)` |
| `0x800C8760` | `CMsgBalloon::SetupStageWarp(StageID)` (descriptive name, medium) |

### SetupBattleWarp and SetupStageWarp

Both fill the `mWarpLayout` panes `BATTLE`, `KNOWN`, `UNKNOWN`, `EXTRA` and `CLEAR`. `SetupStageWarp` loads the stage models for kinds other than 2 through `0x800C88D0` (declared as `SetupStageModels`, owned by the next block), asks `GetStageClearCount`, and shows `KNOWN` or `UNKNOWN` (kind not 2) or `EXTRA` (kind 2) plus `CLEAR` with the clear count when it is positive.

`SetupBattleWarp` has two cases. For kind 2 it shows `UNKNOWN` and, from a progress value (9 once the clear count exceeds 1, else twice the signed byte `mStatus[18]` of the current save slot plus `IsGameFlagSet(30)` minus 1), writes the best record `{1}-{2}` (progress divided by 2, progress modulo 2 plus 1; message `0x600CC`). Otherwise it reads the record's clear time through `IGameSession` slot 4 (`GetBattleRoyalData`) and `0x8006A820` (`GetRecord(kind, rank)`, a `shared_ptr` copy), shows `BATTLE` or `UNKNOWN`, and for a cleared stage writes the clear count (`0x600C7`) and the time formatted by message `0x70010` (`{1}'{2}"{3}`) from minutes, seconds and hundredths of `ceil(100 * time)`, the last two with the `%02d` specification.

### The formatter

`NIntl::basic_formatter<wchar_t>` is 0x34 bytes: the format string at 0, a `vector<wstring>` of arguments at 0xC, the pending format specification at 0x18, the result at 0x24 and a parsed flag at 0x30. Three nested empty exception classes (`InvalidFormatException`, `TooFewArgsException`, `TooManyArgsException`, type names at `0x803A5160`, `0x803A5198`, `0x803A51D0`) are thrown by value from `parse`, which copies the format into the result, replacing each `{n}` (single digit, n at least 1) by argument n and throwing when an argument is missing or unused. `operator%(const wchar_t*)` stores the specification that the next `operator%<int>` (`0x801F34E0`, in another unit) consumes. The constructor `0x80089330`, destructor `0x800893F0`, `operator%<T>` and `c_str()` (`0x8008D4E0`) are defined in other units; `str()` and `parse()` are the copies in this one. `include/NIntl/basic_formatter.h` declares them; `include/game/MessageFormat.h` holds the one-argument and two-argument helpers behind `0x800CB5F0` and `0x800CB570` (`FormatMessageString`, descriptive name, medium), which return a copy of `str()`.

### Library changes

- `std::basic_string` gains `operator[]`, `push_back` and `clear`, and the private `short_data`, `long_data` and `buffer`. `buffer` is `!is_long() ? short_data() : long_data()`; the other order, or an `if`, puts the long case first.
- `vector::operator[]` calls `__vec_constructor::operator[]`, a second layer that the target keeps out of line (`0x800C8740` tail-calls `0x800C8750`).
- `ctype.h` gains `isdigit(int)`: `c < 0 || c > 255` with the narrow `ctype_map_ptr` (offset 8). `iswdigit` and `wctype_map_ptr` produce a different function.
- `math.h` gains `ceilf` and the C++ overload `ceil(float)`; the target calls `ceil` through both.
- `CBattleRoyalData.h` declares `GetRecord(int, int)` and the record's `GetClearTime`; `IsGameFlagSet(int)` (`0x80165BC0`, the native of the `CheckFlag` script binding) is declared in `SaveData.h`.

## F-146-2: CMsgBalloon methods are inline

The numbered `@NNNN` string symbols, the single shared base register and the in-order data layout that F-142-5 could not explain come from compiling the members as out-of-line functions. Strings in `inline` functions are separate weak `@STRING@<function>@<n>` objects, are not merged even when identical, are addressed with their own `lis`/`addi` and are laid out in reverse order of use, which is exactly the target's `.data` and `.sdata` (for example `"CLEAR"` twice in `SetupStageWarp`, and the two `L"%02d"` copies at `0x803A54CC` and `0x803A54D8`). Defining every `CMsgBalloon` member as `inline` in the source reproduces them: `Init` goes from 86.23% to 99.36%, `Calc` from 99.61% to 99.83% and `UpdateBalloon` from 99.44% to 99.63%, and the remaining rows are only literal names. All virtual members are inline, so the class has no key function and the vtable is emitted here; the inline destructor `inline CMsgBalloon::~CMsgBalloon() {}` is what makes `Calc`, `Show`, `Draw` and the rest appear in the object at all.

## F-146-3: statement shapes that matter

- A by-value class parameter (`StageID`) is read through the caller's pointer. Using it through a second type (`MapIndex`) for the record lookup, `GetRecord(rIndex.mMode, rIndex.mArea)`, stops the compiler from reusing the first comparison's `srawi` result and reloads the word; `GetRecord(stageID.mKind, stageID.mRank)` keeps the kind in a callee-saved register. `StageID` and `MapIndex` are the same 32-bit value (`mKind` is the mode, `mRank` the area, `mWorld` the stage).
- The two arguments of `FormatMessageString(msg, progress / 2, progress % 2 + 1)` are temporaries bound to `const int&`; declaring them as locals puts them above the copy of the `MapIndex` instead of below it.
- `formatter % minutes;` then `formatter % L"%02d" % seconds;` then `formatter % L"%02d" % hundredths;` are three statements. One chain, or five statements, differ in where `this` is reloaded.
- `Calc` uses `it->mPosition` and `it->mObject` (`operator->` at `0x800C7F60`) and `&*it` (`operator*` at `0x800C7FE0`).
- In `parse`, `charT c = mFormat[i]` is kept in `r4` for `push_back`; the `{` case comes first, and its validity test is `if (!(...)) throw; else { ... }` because the target jumps from the first throw to the end of the iteration.

## Remaining differences

- Relocation names of `SetupStageModels` (`0x800C88D0`), `GetRecord` (`0x8006A820`), `IsGameFlagSet` (`0x80165BC0`), the formatter members in other units, `FormatMessageString` (`0x800CB570`, `0x800CB5F0`), `basic_string<wchar_t>::operator=(const wchar_t*)` (`0x80086450`), `push_back` (`0x800CA890`), `append` (`0x800CA8F0`) and `clear` (`0x80096310`) keep their `fn_` names; the first three belong to later issues.
- `.data`, `.sdata` and `.sdata2` literal names, and the `mr r4, r31` that follows such a literal in `Calc`.
- The target's `basic_string<wchar_t>::operator=(const wchar_t*)` is a tail call to `assign` (`0x80086F30`). Writing `operator=` as `return assign(s)` reproduces it but moves the stack slot of the iterator tag in inlined users: `BWorldLightSet::CreateMotion` goes from 100% to 99.98% and the DOL no longer verifies. `operator=` keeps its direct `replace`.

## Traps

- **A class defined in one source file has inline members.** If the strings of a function are separate objects, laid out in reverse, addressed one by one with `lis`/`addi` and not merged, the function is an `inline` (weak) function; add `inline` to the definition. Out-of-line functions share one base register and number their strings in order of use.
- **All-inline virtuals need an inline destructor.** With every virtual member `inline` and the destructor only declared, the key function is elsewhere, the vtable is not emitted and none of the virtual members appears in the object.
- **`if (a) throw; rest` and `if (a) throw; else { rest }` differ.** The second jumps to the end of the loop body after `__throw`; the first falls into the code that follows.
- **A header change to `std::string` can break a Matching unit by 0.02%.** The slot of a one-byte empty-class temporary depends on the nesting of the inlined functions, so `BWorldLightSet::CreateMotion` fell to 99.98% and `build.sha1` failed with all other units unchanged. Compare `report.json` per function before pushing.
- **`ceil(float)` is a separate function from `ceil(double)`.** Calling `ceil` on a float reaches `ceil(float)` (`b ceilf`) and then `ceilf` (`bl ceil`, `frsp`); a plain `(int)ceil(x)` on the C function misses both.
- **A shared base register for literals was a symptom.** `Init`, `Calc` and the warp functions showed 86% to 98% until the cause (F-146-2) was found; the earlier note that the compiler "shares one base register" described the effect of out-of-line compilation, not a compiler quirk.
