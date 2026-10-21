# CMsgRes and message text units (#346)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #346 lists 29 functions in `.text` `0x801EE470`–`0x801F11B8`, directly after MSL `wprintf.c`.

## F-346-1: two units

The block holds two translation units. Neither embeds a source-file string, so both file names are descriptive (medium confidence): `game/CMsgRes.cpp` after the RTTI class `CMsgRes` named by the deleter strings `0x803B1F84` and `0x803C3BC0`, and `game/Message.cpp` for the message-list and date and time formatting functions.

| Unit | Section | Range | Evidence |
| --- | --- | --- | --- |
| `CMsgRes.cpp` | `extab` | `0x800434B8`–`0x80043500` | Records of `0x801EE470` to `toupper`; `0x800434B0` belongs to `vswprintf` |
| | `extabindex` | `0x8005223C`–`0x800522A8` | First record names `0x801EE470`, last `toupper` `0x801EF680` |
| | `.text` | `0x801EE470`–`0x801EF6EC` | Starts after the `wprintf.c` padding at `0x801EE468`; ends at `toupper` |
| | `.rodata` | `0x803670A8`–`0x803677E8` | Collation tables read only by `0x801EEAC0`; `0x803677E8` is the TMC JPEG banner |
| | `.data` | `0x803C35D0`–`0x803C3A28` | Six case-mapping wide strings read through the `.sdata` pointers; `0x803C3480` is the `__wpformatter` jump table |
| | `.sdata` | `0x804B86A0`–`0x804B86D8` | `"INF1"`, `"DAT1"`, `"STR1"`, the text locale (initially 1) and the six table pointers; `0x804B8698` and `0x804B869C` are used by `__wpformatter` |
| `Message.cpp` | `extab` | `0x80043500`–`0x80043768` | Records of `0x801EF6F0` to `0x801F1150`; `0x80043768` belongs to `0x801F11E0` |
| | `extabindex` | `0x800522A8`–`0x80052344` | |
| | `.text` | `0x801EF6F0`–`0x801F11B8` | Ends with the static initializer `0x801F1190` (`.ctors` `0x80352A34`) |
| | `.ctors` | `0x80352A34`–`0x80352A38` | |
| | `.data` | `0x803C3A28`–`0x803C3F78` | `"vector length error"`, exception lists, date and time message IDs, their format strings and the `shared_ptr_deleter` vtable, RTTI names and `std::exception` strings; `0x803C3F78` is the region-string table read by `0x801F11C0` |
| | `.bss` | `0x803EAF98`–`0x803EAFA8` | `__register_global_object` node of the static initializer |
| | `.sdata` | `0x804B86D8`–`0x804B8760` | Wide month and weekday strings, `L"AM"`, `L"PM"`, RTTI objects; `0x804B8760` (`"JPN"`) belongs to the next file |
| | `.sbss` | `0x804B91F8`–`0x804B9200` | The `shared_ptr` message-resource list |

The split is in one file only if `CMsgRes::GetMessage` (`0x801EEAA0`, eight instructions) could stay out of line in `0x801EFA20`. Built in one file under `-ipa file -inline auto`, the call is always inlined, and `#pragma dont_inline` or `#pragma auto_inline off` around the caller does not prevent it. With the caller in a second file, `GetMessageText` matches at 100%. The boundary between `toupper` and `0x801EF6F0` is therefore a file boundary. The collation and case functions may be a further file, but no data or code evidence separates them, so they stay in `CMsgRes.cpp`.

Both units are registered `NonMatching`. `configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with the original objects.

## F-346-2: CMsgRes

`CMsgRes` (0x14 bytes) wraps a `MESGbmg1` resource. Members: `mpHeader` 0x0, `mpInfo` 0x4 (`INF1`), `mpResource` 0x8 (constructor argument), `mpData` 0xC (`DAT1`), `mpString` 0x10 (`STR1`). The file header holds the size at 0x8 and the encoding byte at 0x10 (2 UTF-16, 3 Shift-JIS). `INF1` entries start at block offset 0x10, 8 bytes each, offset first; the group ID is the halfword at block offset 0xC.

| Address | Function | Match |
| --- | --- | --- |
| `0x801EE470` | `CMsgRes(void*, bool)` | 100% |
| `0x801EE4F0` | `~CMsgRes()` | 100% |
| `0x801EE530` | `ConvertEscape()`: in-place `\r`, `\f`, `\xNN`, NUL-split and Shift-JIS lead-byte skipping | 94.59% |
| `0x801EE760` | `ConvertEscapeW()`: the UTF-16 form, adding `\uNNNN` | 100% |
| `0x801EE9A0` | `Setup()`: finds the three blocks | 100% |
| `0x801EEA80` | `GetGroupID() const` | 100% |
| `0x801EEAA0` | `GetMessage(int) const` | 100% |
| `0x801EEAC0` | `MakeCollationKeys(CollationKey*, const wchar_t*, int)` | 81.77% |
| `0x801EEE90` | `CompareCollationKeys(const CollationKey*, const CollationKey*, int)` | 100% |
| `0x801EF360` | `CompareCollation(const wchar_t*, const wchar_t*)`: used by the Pii species-name sort (F-173-2) | 100% |
| `0x801EF470` | `SetTextLocale(int)`: `main` passes 0 for European, 1 Japanese, 3 Korean, 4 and 5 Chinese languages | 100% |
| `0x801EF480` | `ConvertCase(wchar_t, int)`: Latin-1, Latin Extended-A, Greek, Cyrillic and full-width case mapping | 93.34% |
| `0x801EF670` | `towupper` (original, replaces MSL's; called by `__wpformatter`) | 100% |
| `0x801EF680` | `toupper` (original): `ConvertCase` for the European locale, else the MSL table | 100% |

`CollationKey` is a 32-bit union of bit fields: category (3 bits), primary (8), secondary (8), accent (4), case (4) and width (3), with the five `.rodata` tables (ASCII, Latin-1, Greek, full-width, kana) and eight ligature keys after the kana table. Comparison levels 1 to 4 compare category, primary and secondary, then accent, case and width. Method, member and field names are descriptive (medium).

## F-346-3: Message

| Address | Function | Match |
| --- | --- | --- |
| `0x801EF6F0` | `InitMsgResList(u32)` | 99.85% |
| `0x801EF970` | `SetMsgRes(int, const shared_ptr<CMsgRes>&)` | 100% |
| `0x801EFA20` | `GetMessageText(int, const wchar_t*)`: group in the high halfword, index in the low | 100% |
| `0x801EFAD0` | `SetDateMessageIDs(const DateMessageIDs&)` | 100% |
| `0x801EFB60` | `FormatDate(std::wstring&, int year, int month, int day, u32 flags)` | 97.45% |
| `0x801F0480` | `operator+(const std::wstring&, wchar_t)` instance | 100% |
| `0x801F0530` | `SetTimeMessageIDs(const TimeMessageIDs&)` | 100% |
| `0x801F0580` | `FormatTime(std::wstring&, int hour, int minute, int second, u32 flags)` | 97.17% |
| `0x801F0DE0` | `__vec_deleter<shared_ptr<CMsgRes>>` destructor | 100% |
| `0x801F0EB0` | `shared_ptr_deleter<vector<...>>::get_deleter` | 100% |
| `0x801F0F00` | `shared_ptr_deleter<vector<...>>::dispose` | 100% |
| `0x801F0FD0` | `shared_ptr<vector<...>>` destructor | 100% |
| `0x801F1060` | `Metrowerks::move_ptr<vector<...>>` destructor | 100% |
| `0x801F1150` | `shared_ptr_deleter<vector<...>>` destructor | 100% |
| `0x801F1190` | `__sinit_\Message_cpp` | 100% |

The date and time setters copy 11 and 8 words. A struct copy of an `int` array uses an update-form store (`stwu`); the original's separate `addi` base is produced only by a struct of individually named members, whose names need the formatters. `InitMsgResList` differs only in the stack slots of the inlined `length_error` construction, and the `move_ptr` destructor in two swapped registers; both come from the shared `include/stdexcept` and `include/vector`.

Outside the units, for the relocations: `__vt__Q23std12length_error` `0x80399778`, `__dt__Q23std12length_errorFv` `0x8005A830`, `__vt__Q23std11logic_error` `0x803997B0` and the `shared_ptr_deleter<char, default_delete<char[]>>` vtable `0x803998B4`, by vtable contents and the compiler's names for the same references.

## F-346-4: case-range switches

`ConvertCase` and the range part of `MakeCollationKeys` compile to a balanced compare tree over value ranges, the lowering of a `switch` with GNU case ranges (`case 0xC0 ... 0xFF:`), which CodeWarrior accepts. The original compares with `cmplwi`. Every tested Wii build (1.0, 1.0a, 1.1, 1.3, 1.5, 1.6, 1.7 and `0x4201_127`) lowers such a switch with `cmpwi` and an extra lower-bound test for zero, for `unsigned short`, `wchar_t`, `unsigned int` and `unsigned long` controlling values, with plain or `u`-suffixed or wide-character labels, and with listed instead of ranged cases. An unsigned 32-bit value with few ranges produces a subtract-and-compare chain instead. If-chains do not produce a tree. The source of the unsigned tree is unresolved; these two functions are the remaining structural mismatches in `CMsgRes.cpp`, with the narrow `ConvertEscape` register allocation.

## F-346-5: date and time formatters

The only caller of both formatters is the save-data display at `0x8011A0FC` and `0x8011A120`, which passes `OSCalendarTime` fields as `FormatDate(out, year, month, day, 0)` and `FormatTime(out, hour, minute, second, 8)`. `sDateMessageIDs` (`0x803C3A74`, 11 words) and `sTimeMessageIDs` (`0x803C3B60`, 8 words) hold BMG message IDs set by `SetDateMessageIDs` and `SetTimeMessageIDs`; the named members in `include/game/Message.h` follow the order the formatters read them, which also reproduces the `addi` plus offset struct copy of F-346-3 (both setters 100%). Every ID is looked up with the inlined `GetMessageText`; a missing ID falls back to the built-in tables: `sMonthNames` (`"1"` to `"12"`), `sWeekdayNames` (Sunday first, Japanese characters) and the `swprintf` formats, which are string literals in `.data` (`"%1$ls/%2$d"`, `"%1$d/%2$ls/%3$d"`, `"%1$d/%2$d"`, `"%1$d/%2$d/%3$d"`, `"%d:%02d"`, `"%d:%02d:%02d"`).

| Flag | Meaning in `FormatDate` | Meaning in `FormatTime` |
| --- | --- | --- |
| `0x1` | `DATE_NAMED_MONTH` | `TIME_FIRST_SET`: selects the first pair of format IDs |
| `0x2` | `DATE_SHORT_WEEKDAY` | |
| `0x8` | `DATE_NO_YEAR` | `TIME_NO_SECONDS` |
| `0x10` | | `TIME_12_HOUR` |
| `0x20` | `DATE_WEEKDAY`: derives the weekday with `OSCalendarTimeToTicks` and `OSTicksToCalendarTime`, then joins weekday and date in the order given by message `mWeekdayAfterDate` | `TIME_24_HOUR` |
| `0x40` | `DATE_ABBREVIATED` | |

`FormatTime` takes the hour modulo 12 or 24 (message `mHourCycle` decides when neither flag is set), prints `AM` or `PM` from `mAm` and `mPm` for the 12-hour cycle, and joins it with the time in the order given by `mMeridiemAfterTime`. `FormatDate` finishes by upper-casing the first character through the locale table (`_current_locale.ctype_cmpt_ptr->wupper_map_ptr`) with a 256-entry limit. The names of the flags and members are descriptive (medium confidence).

What reproduced the shape, in order of discovery:

- The default text passed to the inlined `GetMessageText` must be a string literal written at the call (`GetMessageText(id, L"%1$d/%2$d")`), not a named array: the literal lands in `.data` at the original offsets (`0x803C3AEC`, `0x803C3B08`, `0x803C3B28`, `0x803C3B3C`, `0x803C3B80`, `0x803C3B90`) and CodeWarrior keeps it in a register across the lookup.
- The suffix text uses `cond ? GetMessageText(id, L"") : L""`, which reproduces the register order of the three string variables; separate `if`/`else` assignments did not.
- The message numbers (`mWeekdayAfterDate`, `mHourCycle`, `mMeridiemAfterTime`) come from one `inline long GetMessageNumber(int)` helper that calls `wcstol(GetMessageText(id, L"0"), nullptr, 10)`; a helper defined late keeps the `L"0"` literal out of the `.sdata` order of the surrounding strings, as in the original (`0x804B8758`, after the deleter RTTI).
- Assigning a concatenation needs `rResult.assign(a + L' ' + b)`: `operator=(const basic_string&)` in `include/string` takes the `assign(begin, end)` route, which does not match the inline `assign(const basic_string&)` of the original.
- The joined strings are `std::wstring(label) + L' ' + text`. The `operator+(const wstring&, wchar_t)` instance at `0x801F0480` matches only with a constructor that initializes the short buffer to one character and keeps the dead long-representation branch (`single_char_init` in `include/string`).
- The capitalization is `c >= 256 ? c : table[c]` returning `int`; `towupper` from `wctype.h` compares both ends and does not match.

Both formatters stay below 100%: the original keeps the address of the first string temporary (the `std::wstring` built from the weekday or AM/PM text) in a non-volatile register across its constructor and the following `operator+` call (`addi r24, r1, 108` before `wcslen`), where this build recomputes the address from `r1` each time. Extab contents, frame sizes and every other instruction already match. Tried without effect: named locals, a reference to the temporary, an inline `ToWide` wrapper (not inlined), a shared inline `JoinTexts` helper (slightly worse), a defaulted allocator argument and a reordered constructor body. `NIntl::basic_formatter` members show the same register pattern in `FormatAreaName`, so the cause may sit in the `basic_string(const charT*)` constructor of the reference library.

`fn_80088DB0` (`0x80088DB0`) is the `operator+(const std::wstring&, const wchar_t*)` instance, named for the link; its unit belongs to the issue that owns that range.

## F-346-4 update: `ConvertCase` as a threshold ladder

A chain of `else if (c < K)` tests on an unsigned 16-bit value is turned by CodeWarrior into the original unsigned `cmplwi` tree (root `0x3D0`, leaves `bgelr` for the gaps), which none of the switch forms in F-346-4 did. `ConvertCase` is now written as that ladder with explicit gap returns and scores 93.34% (it was 78.14%). The remaining difference is structural: each gap `return c;` leaves a dead `blr` after its neighbouring leaf, where the original has one dead `blr` at the end of each case, and the original leaf order puts the `0xFF41` and `0xFF21` leaves before the Cyrillic ones. That layout is what a switch lowering produces, so the original is probably a switch whose unsigned compare tree this compiler never emits. Gap branches written as empty bodies, `break`, `goto` or a shared result variable all change the tree root to `0x380` (82.35%) because the gaps are folded away. A switch over the same ranges with merged case labels stays signed (71.09%).

`MakeCollationKeys` has the same inner tree (root `0x3094`, `cmplwi`, one shared default block), but its default block holds the symbol-key construction, so the ladder form would duplicate it and was not applied.

## Remaining work

- `CMsgRes.cpp`: `ConvertEscape` (94.59%, one extra non-volatile register for the hex-digit character), `MakeCollationKeys` (81.77%) and `ConvertCase` (93.34%), see F-346-4.
- `Message.cpp`: the first-temporary register of `FormatDate` (97.45%) and `FormatTime` (97.17%), and the stack slots of `InitMsgResList` (99.85%) from the shared `stdexcept`/`vector` headers.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf`, scans of absolute and small-data references, `extabindex`, `.ctors` and RTTI strings, `tools/re/unitdiff.py`, and scratch compiles with every Wii compiler in `tools/local/compilers`. No decompiler output was used.

## Traps

- **An eight-instruction member function stayed out of line.** That proved a file boundary: under `-ipa file` a same-file caller always inlined it, and neither `dont_inline` nor `auto_inline off` around the caller changed that. Check small out-of-line calls before assuming one file.
- **Case-range switches lower to signed trees.** See F-346-4; an unsigned compare tree is not reproduced by any tested type or compiler.
- **Struct copies depend on the member list.** Copying an `int mIDs[N]` struct gives a `stwu` first store; individually named members give `addi` plus offset stores.
- **A pointer initialized from the same expression twice can matter.** `ConvertEscapeW` matched only with `pDst` loaded from `mpData->mData` again rather than copied from `pSrc`.
- **`python -` with a heredoc hangs Git Bash** even with input redirected from `/dev/null`; run scripts from files.
- **A named static array is not the same as a string literal argument.** Passing `sFormat` (a `static wchar_t[]`) to the inlined `GetMessageText` rematerialized the address at the join; the original holds a literal in a register. Write the literal at the call. See F-346-5.
- **A 100% mnemonic match can hide dead `blr` blocks.** The `ConvertCase` ladder with explicit gap returns matches every compare but adds `blr` blocks that change the function size. Check the object size, not only the fuzzy score.
- **Adding a non-const `operator[]` to `std::basic_string` moves unrelated units.** It improved `CPauseMenu` `FormatMessage` by 1.4 points and lowered `CNoticeDisplay` `CYouWin::Init` by 0.34. Use `const_cast<wchar_t*>(str.data())` locally instead.
