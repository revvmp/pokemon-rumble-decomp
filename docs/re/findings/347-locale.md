# Region, language and locale, and the argument formatter (#347)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #347 lists 21 functions in `.text` `0x801F11C0`–`0x801F6060`, directly after `Message.cpp` ([F-346-1](346-cmsgres.md#f-346-1-two-units)) and before TMC JPEG `tmcjpeg.c`.

## F-347-1: two units

The block holds two translation units. Neither embeds a source-file string; both file names are descriptive (medium confidence).

| Unit | Section | Range | Evidence |
| --- | --- | --- | --- |
| `Locale.cpp` | `extab` | `0x80043768`–`0x80043798` | Six 8-byte records, `0x801F11E0` to `0x801F16F0` |
| | `extabindex` | `0x80052344`–`0x8005238C` | Six entries; `0x8005238C` indexes `0x801F18E0` |
| | `.text` | `0x801F11C0`–`0x801F18D8` | After `__sinit_\Message_cpp` |
| | `.data` | `0x803C3F78`–`0x803C4160` | Region and locale name tables and five jump tables |
| | `.sdata` | `0x804B8760`–`0x804B8810` | Name strings, the three locale override strings and the four cached values; `0x804B880C` is padding |
| | `.sbss` | `0x804B9200`–`0x804B9208` | The two cached locale pointers |
| Formatter unit | `extab` | `0x80043798`–`0x80044150` | |
| | `extabindex` | `0x8005238C`–`0x80052434` | `0x80052434` indexes MSL `0x8033FD0C` |
| | `.text` | `0x801F18E0`–`0x801F6060` | Ends before `tmcjpeg.c` |
| | `.data` | `0x803C4160`–`0x803C4258` | `"vector length error"`, `basic_string` error strings and exception lists; `"NO NAME"` at `0x803C4258` is read from `0x80207298` |
| | `.sdata` | `0x804B8810`–`0x804B884A` | Default format strings `"%d"`, `"%f"`, `"%s"`, `"%ls"`, `L"%d"`, `L"%lu"`, `L"%f"`, `L"%s"`, `L"%ls"` and the two `char()` bytes of `basic_string<char>::replace` |

The boundary at `0x801F18E0` rests on data: the locale `.sdata` ends with four bytes of padding at `0x804B880C` before the 8-byte-aligned `"%d"`, which one unit would not insert before a string, and the formatter shares no data or callee with the locale functions. No `.ctors` entry falls in the block.

`Locale.cpp` is `Matching`; the formatter unit is not split.

## F-347-2: Locale

| Address | Function | Match |
| --- | --- | --- |
| `0x801F11C0` | `GetRegionName(Region)` | 100% |
| `0x801F11E0` | `GetConsoleRegion()`: from `VIGetTvFormat` and, for NTSC, `OSGetFontEncode` | 100% |
| `0x801F12A0` | `GetGameRegion()`: from the fourth game-code character | 100% |
| `0x801F1340` | `GetConsoleLang()`: cached `SCGetLanguage` | 100% |
| `0x801F1380` | `GetGameLang()`: from the game region, with `SCGetLanguage` for Europe and the all-region build | 100% |
| `0x801F1520` | `GetGameLocale()` | 100% |
| `0x801F16F0` | `GetConsoleLocale()` | 100% |

The four region and language getters are named after the script bindings that call them: `ScriptBinding_GetConsoleRegion` (`0x801421B0`), `ScriptBinding_GetGameRegion`, `ScriptBinding_GetConsoleLang` and `ScriptBinding_GetGameLang`. The other names are descriptive (medium). `Region` follows the name table: `JPN`, `USA`, `EUR`, `AUS`, `BRA`, `TWN`, `KOR`, `CHN`, `HKG`, `ASI`, `LTN`, `SAF`, `ALL`, with 13 as the not-yet-detected value. The order differs from the SC product areas (`CHN` is 7). Language values are the SDK `SC_LANG_*` constants; the locale is `ja_JP` to `ko_KR` by language, with `en_US` for English in the USA and Brazil, `en_AU` in Australia and `fr_CA` for French in the USA. Both locale functions take the game region, replaced by the console region for the all-region build.

Source forms that the code requires:

- **Early returns.** Each getter starts `lwz r3; cmp; beq body; b epilogue`, which is `if (cached) return cached;`. An `if (!cached) { ... }` block gives `bne` past the body.
- **The game-code switch is a separate inline function.** Written inside `GetGameRegion`, the getter was never inlined into its four callers; with the switch in an inline helper that returns per case, it is inlined everywhere, as in the original.
- **The European language check is a `switch`.** `case SC_LANG_ENGLISH:` to `case SC_LANG_ITALIAN:` gives the original `subi`, `cmplwi 4`; a two-sided `if` gives two compares. `SC_LANG_*` are `unsigned` macros, so `sGameLang > SC_LANG_ITALIAN` compares with `cmplwi`.
- **Locale override strings are `char` arrays.** String literals, in the functions, in an inline helper or in a macro, are emitted after the file's initialized statics; the original has `en_US`, `en_AU` and `fr_CA` before them. `const char* const` objects are not folded and load through `lwz`; `const char[]` arrays go to `.rodata`. Non-`const` `static char` arrays reproduce `.sdata` exactly.

## F-347-3: formatter unit

A two-character-type argument formatter, used by script `Format` (`fn_801424D0`) and game code. Its object holds a format string at 0x0, a `vector<basic_string>` of formatted arguments at 0xC, the current conversion specification string at 0x18 and a flag byte at 0x30. Each `operator%`-style member formats one argument with the current specification, or a default one when it is empty, appends the result to the vector (with the inlined `vector` growth and `length_error`), clears the specification and returns `*this`.

| Address | Function |
| --- | --- |
| `0x801F18E0`–`0x801F2DE0` | `char` instances for `int` (`"%d"`), `float` (`"%f"`), `const char*` (`"%s"`) and `const wchar_t*` (`"%ls"`); format through `0x800721A0` |
| `0x801F34E0`–`0x801F5120` | `wchar_t` instances for `int`, `unsigned long` (`L"%lu"`), `float`, `const char*` and `const wchar_t*`; format through `0x8006BD60` |
| `0x801F5830`, `0x801F5940` | `vector<basic_string<char>>` and `vector<basic_string<wchar_t>>` destructors |
| `0x801F5A50`, `0x801F5BB0` | `vsnprintf` and `vswprintf` into a `basic_string`: count with `__CountPrintf` or `__wCountPrintf`, resize, print; called by `0x800721A0` and `0x8006BD60` |
| `0x801F5D10` | MSL `basic_string<char>::replace(size_type, size_type, size_type, char)`, used by the resize |

Numeric and pointer arguments are passed by reference and by value respectively. These functions need the MSL `basic_string` copy, resize and fill-replace members, which `include/string` does not yet provide (the same blocker as the `Message.cpp` formatters, #327 and #328), so the unit is left unsplit.

## Remaining work

- The formatter unit: split at the F-347-1 ranges once `include/string` provides the `basic_string` members, then reconstruct the template and its instances.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf`, scans of absolute and small-data references, `extabindex`, `.ctors` and callers, `tools/re/unitdiff.py`, the normal build and `tools/verify/verify.py`. No decompiler output was used.

## Traps

- **String literals follow the initialized statics.** Under `-ipa file`, literals used in function bodies were placed after every initialized file-scope variable, wherever the function or an inline helper was defined. Data whose literals precede the statics needs named array objects.
- **A getter containing a jump-table switch was not auto-inlined.** Moving the switch into an inline helper made the getter small enough to inline.
- **`python -` with a heredoc hangs Git Bash** even with input redirected; run scripts from files.
