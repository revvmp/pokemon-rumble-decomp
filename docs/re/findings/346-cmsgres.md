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
| `0x801EF480` | `ConvertCase(wchar_t, int)`: Latin-1, Latin Extended-A, Greek, Cyrillic and full-width case mapping | 78.14% |
| `0x801EF670` | `towupper` (original, replaces MSL's; called by `__wpformatter`) | 100% |
| `0x801EF680` | `toupper` (original): `ConvertCase` for the European locale, else the MSL table | 100% |

`CollationKey` is a 32-bit union of bit fields: category (3 bits), primary (8), secondary (8), accent (4), case (4) and width (3), with the five `.rodata` tables (ASCII, Latin-1, Greek, full-width, kana) and eight ligature keys after the kana table. Comparison levels 1 to 4 compare category, primary and secondary, then accent, case and width. Method, member and field names are descriptive (medium).

## F-346-3: Message

| Address | Function | Match |
| --- | --- | --- |
| `0x801EF6F0` | `InitMsgResList(u32)` | 99.85% |
| `0x801EF970` | `SetMsgRes(int, const shared_ptr<CMsgRes>&)` | 100% |
| `0x801EFA20` | `GetMessageText(int, const wchar_t*)`: group in the high halfword, index in the low | 100% |
| `0x801EFAD0` | `SetDateMessageIDs(const DateMessageIDs&)` | 71.36% |
| `0x801EFB60` | Date formatter (`0x8011A0FC`) | Not reconstructed |
| `0x801F0480` | Wide-string helper of the formatters | Not reconstructed |
| `0x801F0530` | `SetTimeMessageIDs(const TimeMessageIDs&)` | 51.00% |
| `0x801F0580` | Time formatter (`0x8011A120`) | Not reconstructed |
| `0x801F0DE0` | `__vec_deleter<shared_ptr<CMsgRes>>` destructor | 100% |
| `0x801F0EB0` | `shared_ptr_deleter<vector<...>>::get_deleter` | 100% |
| `0x801F0F00` | `shared_ptr_deleter<vector<...>>::dispose` | 100% |
| `0x801F0FD0` | `shared_ptr<vector<...>>` destructor | 100% |
| `0x801F1060` | `Metrowerks::move_ptr<vector<...>>` destructor | 98.77% |
| `0x801F1150` | `shared_ptr_deleter<vector<...>>` destructor | 100% |
| `0x801F1190` | `__sinit_\Message_cpp` | 100% |

The date and time setters copy 11 and 8 words. A struct copy of an `int` array uses an update-form store (`stwu`); the original's separate `addi` base is produced only by a struct of individually named members, whose names need the formatters. `InitMsgResList` differs only in the stack slots of the inlined `length_error` construction, and the `move_ptr` destructor in two swapped registers; both come from the shared `include/stdexcept` and `include/vector`.

Outside the units, for the relocations: `__vt__Q23std12length_error` `0x80399778`, `__dt__Q23std12length_errorFv` `0x8005A830`, `__vt__Q23std11logic_error` `0x803997B0` and the `shared_ptr_deleter<char, default_delete<char[]>>` vtable `0x803998B4`, by vtable contents and the compiler's names for the same references.

## F-346-4: case-range switches

`ConvertCase` and the range part of `MakeCollationKeys` compile to a balanced compare tree over value ranges, the lowering of a `switch` with GNU case ranges (`case 0xC0 ... 0xFF:`), which CodeWarrior accepts. The original compares with `cmplwi`. Every tested Wii build (1.0, 1.0a, 1.1, 1.3, 1.5, 1.6, 1.7 and `0x4201_127`) lowers such a switch with `cmpwi` and an extra lower-bound test for zero, for `unsigned short`, `wchar_t`, `unsigned int` and `unsigned long` controlling values, with plain or `u`-suffixed or wide-character labels, and with listed instead of ranged cases. An unsigned 32-bit value with few ranges produces a subtract-and-compare chain instead. If-chains do not produce a tree. The source of the unsigned tree is unresolved; these two functions are the remaining structural mismatches in `CMsgRes.cpp`, with the narrow `ConvertEscape` register allocation.

## Remaining work

- `CMsgRes.cpp`: `ConvertEscape`, `MakeCollationKeys` and `ConvertCase` (F-346-4).
- `Message.cpp`: the date and time formatters and their helper, which build `std::basic_string<wchar_t>` and need #327/#328; the named members of the two ID structs; the shared-header differences of `InitMsgResList` and the `move_ptr` destructor.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf`, scans of absolute and small-data references, `extabindex`, `.ctors` and RTTI strings, `tools/re/unitdiff.py`, and scratch compiles with every Wii compiler in `tools/local/compilers`. No decompiler output was used.

## Traps

- **An eight-instruction member function stayed out of line.** That proved a file boundary: under `-ipa file` a same-file caller always inlined it, and neither `dont_inline` nor `auto_inline off` around the caller changed that. Check small out-of-line calls before assuming one file.
- **Case-range switches lower to signed trees.** See F-346-4; an unsigned compare tree is not reproduced by any tested type or compiler.
- **Struct copies depend on the member list.** Copying an `int mIDs[N]` struct gives a `stwu` first store; individually named members give `addi` plus offset stores.
- **A pointer initialized from the same expression twice can matter.** `ConvertEscapeW` matched only with `pDst` loaded from `mpData->mData` again rather than copied from `pSrc`.
- **`python -` with a heredoc hangs Git Bash** even with input redirected from `/dev/null`; run scripts from files.
