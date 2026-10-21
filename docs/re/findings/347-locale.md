# Region, language and locale, and the argument formatter (#347)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #347 lists 21 functions in `.text` `0x801F11C0`Ã¢â‚¬â€œ`0x801F6060`, directly after `Message.cpp` ([F-346-1](346-cmsgres.md#f-346-1-two-units)) and before TMC JPEG `tmcjpeg.c`.

## F-347-1: initial two-unit survey (superseded by F-347-4)

The block holds two translation units. Neither embeds a source-file string; both file names are descriptive (medium confidence).

| Unit | Section | Range | Evidence |
| --- | --- | --- | --- |
| `Locale.cpp` | `extab` | `0x80043768`Ã¢â‚¬â€œ`0x80043798` | Six 8-byte records, `0x801F11E0` to `0x801F16F0` |
| | `extabindex` | `0x80052344`Ã¢â‚¬â€œ`0x8005238C` | Six entries; `0x8005238C` indexes `0x801F18E0` |
| | `.text` | `0x801F11C0`Ã¢â‚¬â€œ`0x801F18D8` | After `__sinit_\Message_cpp` |
| | `.data` | `0x803C3F78`Ã¢â‚¬â€œ`0x803C4160` | Region and locale name tables and five jump tables |
| | `.sdata` | `0x804B8760`Ã¢â‚¬â€œ`0x804B8810` | Name strings, the three locale override strings and the four cached values; `0x804B880C` is padding |
| | `.sbss` | `0x804B9200`Ã¢â‚¬â€œ`0x804B9208` | The two cached locale pointers |
| Formatter unit | `extab` | `0x80043798`Ã¢â‚¬â€œ`0x80044150` | |
| | `extabindex` | `0x8005238C`Ã¢â‚¬â€œ`0x80052434` | `0x80052434` indexes MSL `0x8033FD0C` |
| | `.text` | `0x801F18E0`Ã¢â‚¬â€œ`0x801F6060` | Ends before `tmcjpeg.c` |
| | `.data` | `0x803C4160`Ã¢â‚¬â€œ`0x803C4258` | `"vector length error"`, `basic_string` error strings and exception lists; `"NO NAME"` at `0x803C4258` is read from `0x80207298` |
| | `.sdata` | `0x804B8810`Ã¢â‚¬â€œ`0x804B884A` | Default format strings `"%d"`, `"%f"`, `"%s"`, `"%ls"`, `L"%d"`, `L"%lu"`, `L"%f"`, `L"%s"`, `L"%ls"` and the two `char()` bytes of `basic_string<char>::replace` |

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
| `0x801F18E0`Ã¢â‚¬â€œ`0x801F2DE0` | `char` instances for `int` (`"%d"`), `float` (`"%f"`), `const char*` (`"%s"`) and `const wchar_t*` (`"%ls"`); format through `0x800721A0` |
| `0x801F34E0`Ã¢â‚¬â€œ`0x801F5120` | `wchar_t` instances for `int`, `unsigned long` (`L"%lu"`), `float`, `const char*` and `const wchar_t*`; format through `0x8006BD60` |
| `0x801F5830`, `0x801F5940` | `vector<basic_string<char>>` and `vector<basic_string<wchar_t>>` destructors |
| `0x801F5A50`, `0x801F5BB0` | `vsnprintf` and `vswprintf` into a `basic_string`: count with `__CountPrintf` or `__wCountPrintf`, resize, print; called by `0x800721A0` and `0x8006BD60` |
| `0x801F5D10` | MSL `basic_string<char>::replace(size_type, size_type, size_type, char)`, used by the resize |

Numeric and pointer arguments are passed by reference and by value respectively. These functions need the MSL `basic_string` copy, resize and fill-replace members, which `include/string` does not yet provide (the same blocker as the `Message.cpp` formatters, #327 and #328), so the unit is left unsplit.

## Current acceptance

F-347-4 supersedes the unsplit formatter status and the cleanup identities in F-347-3. The original inventory is seven existing Locale functions and fourteen formatter functions, 21 functions and 19,960 instruction bytes in total.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf`, scans of absolute and small-data references, `extabindex`, `.ctors` and callers, `tools/re/unitdiff.py`, the normal build and `tools/verify/verify.py`. No decompiler output was used.

## Traps

- **String literals follow the initialized statics.** Under `-ipa file`, literals used in function bodies were placed after every initialized file-scope variable, wherever the function or an inline helper was defined. Data whose literals precede the statics needs named array objects.
- **A getter containing a jump-table switch was not auto-inlined.** Moving the switch into an inline helper made the getter small enough to inline.
- **`python -` with a heredoc hangs Git Bash** even with input redirected; run scripts from files.


## F-347-4: complete formatter source and corrected boundaries

The actual formatter producer establishes two further translation units, rather than the single formatter unit inferred in F-347-1. `ArgumentFormatter.cpp` and `StringFormat.cpp` are descriptive names; the `NIntl::basic_formatter` class and operator identities follow native caller and template evidence. The existing complete 52-byte class declaration is preserved: format string at 0, argument vector at 12, conversion specification at 24, parsed string at 36 and parsed flag at 48.

| Source | Text | extab | extabindex | Data | Small data |
| --- | --- | --- | --- | --- | --- |
| `ArgumentFormatter.cpp` | `801F18E0Ã¢â‚¬â€œ801F5A44` | `80043798Ã¢â‚¬â€œ800440F0` | `8005238CÃ¢â‚¬â€œ80052410` | `803C4160Ã¢â‚¬â€œ803C41B0` | `804B8810Ã¢â‚¬â€œ804B8848` |
| `StringFormat.cpp` | `801F5A50Ã¢â‚¬â€œ801F6060` | `800440F0Ã¢â‚¬â€œ80044150` | `80052410Ã¢â‚¬â€œ80052434` | `803C41B0Ã¢â‚¬â€œ803C4258` | `804B8848Ã¢â‚¬â€œ804B884A` |

The second boundary is supported by the independent formatter families, their data and exception-table partition, and the complete native source-selected link. Combining them into one source changes fill-replace from the native 848 bytes to 840 by retaining an additional shared literal-pool address. The two zero characters after `804B8848` are real character objects; the following six alignment bytes belong to neither object.

| Address | Function | Instruction bytes |
| --- | --- | ---: |
| `801F18E0` | narrow formatter, integer argument | 1792 |
| `801F1FE0` | narrow formatter, float argument | 1792 |
| `801F26E0` | narrow formatter, narrow string argument | 1788 |
| `801F2DE0` | narrow formatter, wide string argument | 1788 |
| `801F34E0` | wide formatter, integer argument | 1800 |
| `801F3BF0` | wide formatter, unsigned-long argument | 1800 |
| `801F4300` | wide formatter, float argument | 1800 |
| `801F4A10` | wide formatter, narrow string argument | 1796 |
| `801F5120` | wide formatter, wide string argument | 1796 |
| `801F5830` | wide-string temporary vector-buffer destructor | 260 |
| `801F5940` | narrow-string temporary vector-buffer destructor | 260 |
| `801F5A50` | narrow `VFormatString` | 340 |
| `801F5BB0` | wide `VFormatString` | 340 |
| `801F5D10` | narrow string fill-replace | 848 |

The fourteen streams total 18,200 bytes. Their fourteen exception records total 2,488 bytes, including all cleanup relocations. Numeric arguments use const references; string pointers use values. Both `VFormatString` overloads return the count or negative error returned by their counting routine, copy the original `va_list` independently before counting and formatting, and preserve the result on a negative count.

The old two destructor descriptions in F-347-3 were reversed and identified the wrong abstraction. The native exception records require the wide temporary-buffer destructor first, then the narrow one. The bodies happen to be identical, so instructions alone cannot distinguish their placement. Checking the final exception-table pointers is necessary.

The original 2006 MSL headers recovered in the Alpha distribution are a reading aid, not imported source. Their provenance is recorded in `198-reference/primary-header-research/2006-primary-manifest.json` outside the repository. The original string move traits require both trivial allocator copy and trivial allocator destruction. The bounded `__vec_bulk_move<basic_string<...>>` specialization expresses those conditions and preserves the generic POD policy. The same original vector declares its temporary helper before the owning base, names a dependent `helper` typedef in that base and constructs the helper through that alias. Using that actual dependency pattern recovers the wide-before-narrow native cleanup ordering without explicit instantiation or retained drivers. The canonical descriptive helper spelling remains unchanged.

The actual compiled source objects, their code and exception relocations, emitted literals and full source-selected ELF/MAP were checked against the immutable native DOL. The complete source-selected build reproduces every DOL byte, including the cleanup pointers. Existing Locale source selection remains intact; no fallback supplies any of the original 21 functions. Borrowed counting and print routines and the earlier wide-string fill-replace remain existing external dependencies with native identities, rather than newly claimed source coverage.

Outside receipts are retained under `347-reference/347-full-source-positive`, `347-reference/current-controls`, the strict code/exception receipts and `347-reference/347-prerequisite-oracle`. The independent CMsgBalloon consumer also preserves all 436 native functions and all 226 exception records with the exact proposed vector header (`152-reference/font347-vector436/native-proof.json`).

### Additional traps

- Identical cleanup bodies can conceal incorrect semantic placement. Here all instruction and data bytes matched while nine exception cleanup pointers differed by 18 bytes. The final source-selected DOL exposed the mismatch; dependent helper construction recovered the native order.
- Same-body string destructor separation, base destructor definition order, buffer method separation, allocator-reference spelling and helper renaming did not recover this ordering. These diagnostic variants were rejected and are not publication changes.
- A whole-primary-header oracle must put only required game declarations before the original library include directories. Reusing a mutable sparse overlay containing adapted library headers silently ceases to test the genuine headers.
- The bounded prior #141 `-O4,s` compiler-identification control was negative: its factory became 1,520 bytes instead of native 2,384 and Calc became 596 instead of 832. Canonical `-O4,p` remains unchanged; receipts remain in `141-reference/141-O4s-code-proof.json`.

The same-base shared-header control builds all 767 pre-existing actual source objects and all 717 selected Matching objects. It preserves 708 objects byte-for-byte and 43 after numerical-label normalization. Fourteen NonMatching objects have unchanged native scores and preserve every previously exact instruction stream and semantic relocation. Two Matching objects, CPpuRegister and ScriptItem, reorder their discarded temporary-buffer/base cleanup weak definitions and each lose twelve bytes of local alignment. Their native scores and previously exact function instructions and semantic bindings are unchanged. The final selected ELF's allocated sections, complete semantic symbol attributes and relocations, and the frozen DOL are identical. The final linker coalesces those weak definitions to the pre-existing selected owners; their local object offsets do not supply new linked code.
