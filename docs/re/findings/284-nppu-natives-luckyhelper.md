# Issue 284: NPpu script VM, fourth block

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). The block `0x80175810`–`0x801770F4` covers the rest of `game/NPpuLibrary.cpp` ([F-283-1](283-nppu-library.md#f-283-1-the-second-nppu-file-starts-at-0x80174d70)) and a separate last game file.

## F-284-1: the last two functions form their own unit

`0x801768E0` and `0x80176C60` do not touch the script engine. Their only caller is `ScriptBinding_GetLuckyNumber` (`0x80139770`), which builds four base-10000 digits from a 64-bit value, scrambles them with `0x801768E0`, and computes a check value with `0x80176C60` and the 100-byte permutation table at `.rodata` `0x80366760`. That table is the only `.rodata` object between the game code and RVL_SDK `cnt.c` (`.rodata` `0x803667C8`), so it belongs to the unit linked directly before the RevoEX and CNT libraries, and a natives file is unlikely to define it. The two functions are therefore split as a third file, `game/luckyhelper.cpp`; the name follows the issue survey's `luckyhelper` label (low confidence; no string in the executable names it, and a lowercase name is consistent with ASCII link order after `NPpuLibrary.cpp`).

| Unit | Section | Range |
| --- | --- | --- |
| `game/NPpuLibrary.cpp` | `extab`, `extabindex`, `.text` | `0x80034EBC`–`0x8003500C`, `0x8004FF14`–`0x80050034`, `0x80174D70`–`0x801768D4` |
| `game/luckyhelper.cpp` | `extab` | `0x8003500C`–`0x8003501C` (entries of `0x801768E0`, `0x80176C60`) |
| `game/luckyhelper.cpp` | `extabindex` | `0x80050034`–`0x8005004C` |
| `game/luckyhelper.cpp` | `.text` | `0x801768E0`–`0x801770F4`; RevoEX `NETVersion.c` follows |
| `game/luckyhelper.cpp` | `.rodata` | `0x80366760`–`0x803667C8`: `gLuckyNumberTable`, 100 bytes, a permutation of 0–99, then padding to the next unit's 8-byte alignment |

Both functions use one shuffle round four times with the keys 66339863, 93740587, 22531951 and 39631509: the low pair of digits `d0 + d1 * 10000` is multiplied by one key and offset by the next, the high pair by the next key and offset by the first, both modulo 10^8 in 64-bit arithmetic, and the four digits are rebuilt from the two results. `ScrambleLuckyNumber(u16*)` works in place; `CalcLuckyNumberCheck(const u16*, const u8*)` shuffles a register copy and then folds the digits through the table (`table[c / 100] * 100 + table[c % 100]`, adding the next digit modulo 10000). Function and table names are descriptive (medium confidence for behaviour).

`game/luckyhelper.cpp` is linked as `Matching`: both functions and the extab entries are 100% in objdiff, and `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## F-284-2: the sixteen natives of `NPpuLibrary.cpp`

The F-29 binding table names the natives; they carry the C++ mangling of the earlier natives (`__FPQ24NPpu7CEngine`). Outside the block, for the link, `symbols.txt` now names `0x80174550` `NPpu::CEngine::CollectGarbage(int, int)` (its second argument is always 0 and never read), the MSL instances `operator+(const basic_string&, const basic_string&)` for `char` (`0x80072830`) and `wchar_t` (`0x8008FD30`), the out-of-line `basic_string` destructors used by the exception tables (`0x80067F00` `char`, `0x8006BAD0` `wchar_t`), and MSL `strtof` (`0x803472F8`) and `wcstof` (`0x80348BAC`), which both return a float narrowed with `frsp` and take the end pointer as their second argument.

| Address | Native | Behaviour | Match |
| --- | --- | --- | --- |
| `0x80175810` | `Sprintf` | `Format`s the arguments and pushes a collected string, `""` when there are none | 100% |
| `0x801759F0`–`0x80175D10` | `Sin`, `Cos`, `Atan`, `Log`, `Exp`, `_Sqrt` | Double MSL function on `PopFloat`, pushed as a float | 100% |
| `0x80175DB0` | `CreateTimer` | Allocates an `OSTime` with `OSAlloc` and pushes its address as an int | 100% |
| `0x80175E10` | `ResetTimer` | Stores `OSGetTime()` | 100% |
| `0x80175EB0` | `GetTime` | Pushes `OSTicksToMicroseconds` of the elapsed ticks | 100% |
| `0x80175F80` | `GetPlatform` | Pushes 3 | 100% |
| `0x80175FB0`, `0x801760F0` | `ParseInt`, `ParseReal` | One `switch` over all value types; strings go through `CEngine::ParseInt`/`ParseWInt` or `strtof`/`wcstof` | 100% |
| `0x80176270` | `String` | Concatenates `ToStdString`/`ToStdWString` of every argument with `operator+` | 74.2% |
| `0x801766C0` | `GC` | `CollectGarbage(PopInt(), 0)` | 100% |
| `0x80176750` | `Dump` | Converts each argument; the output was compiled out and leaves empty character loops | 100% |

Remaining difference in `String`: both are `basic_string` shapes from `include/string`, owned by #327. The inlined `operator=` of the original tests the destination's long flag once and reads the source's data and size under a single long-flag test (an experimental rewrite reached 88.4%), and the inlined `basic_string(const char*)` keeps the length and the address of the result in callee-saved registers, as in `ToStdString` ([F-283-3](283-nppu-library.md#f-283-3-reconstruction-state)). `include/string` was only extended with the `operator+` declaration.

`NPpuLibrary.cpp` stays `NonMatching` because of `String` and the two #283 `Format` functions.

## Method

`tools/re/unitdiff.py` with Wii/1.1 and `cflags_game`, the generated assembly under `build/wpse-v1/asm/`, and a Python reader of the DOL for the `.rodata` table. No decompiler output was used.

## Traps

- **Union member types decide whether a pushed temporary stays in memory.** Setting the payload of a pushed `CValue` through the `const char*` union member stored the temporary to the stack before copying it; storing it through the `int` member lets the copy fold into the `stwux`/`stw` push, as in the original.
- **A shared epilogue `li r3, 0` means no early return.** `Sprintf` matched only after the empty-argument case became an `if`/`else` branch; `String` keeps a separate `li r3, 0` in its early path and does return early.
- **Consecutive case pairs compile to range tests.** `subi r0, rX, 2; cmplwi r0, 1; ble` in `ParseInt`, `ParseReal` and `Dump` is a `switch` with `case 2: case 3:`, not an `IsString()` test, which compiles to two `cmpwi`.
- **A character test without `extsb` reads an unsigned byte.** `Dump`'s empty loop over a `const char*` needs an unsigned comparison; `*p` and `*p != '\0'` add `extsb` in place of the original loop-alignment `nop`.
- **Store order follows the source within an inlined helper.** The shuffle round matched only with the low pair's digits stored before the high pair's.
