# F-448-1: NW4R ut ResFontBase unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. `nw4r/ut/ut_ResFontBase.cpp` begins with `ResFontBase::ResFontBase` at `0x80294C20`, the last function of #447 ([F-447-1](447-ut-romfont.md)), covers all 27 functions of #448 (`0x80294C80–0x80295464`), and ends with `GetGlyphFromIndex` at `0x80295470`, the first function of #449. This completes #447 and #448.

## Unit and boundaries

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80294C20–0x80295578` | 29 functions, constructor through `GetGlyphFromIndex` |
| `.data` | `0x803C6AE8–0x803C6B58` | default reader pointer constant used by the inline `Font` constructor, `ResFontBase` vtable (`0x803C6AF8`, 0x60 bytes) |

The next function, `0x80295580`, and the next vtable, `0x803C6B58`, belong to the following unit, expected to be `ut_ResFont.cpp` in #449. The `.text` split ends at the last function's end and leaves the alignment padding to that unit.

`ut_ResFontBase` is `Matching`: objdiff reports 100% for all 29 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Differences from the 2006 reference

The source starts from ogws `src/nw4r/ut/ut_ResFontBase.cpp` at commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73` (CC0-1.0). Petari `src/nw4r/ut/ut_ResFontBase.cpp` at commit `e0ab141914921726a595573dc48d3f13d0922efe` (CC0) supplies `RemoveResourceBuffer` (`0x80294CD0`) and `HasGlyph` (`0x80295110`). The target adds a one-entry glyph lookup cache that neither reference has:

- The constructor stores a halfword 0 at offset 0x18 and `0xFFFF` at offset 0x1A, after `mResource` (0x10) and `mFontInfo` (0x14). The class is 0x1C bytes.
- `FindGlyphIndex(u16)` (`0x802952A0`, 0xA0 bytes) first compares the character with the halfword at 0x18 and, if equal, returns the halfword at 0x1A. Otherwise it stores the character at 0x18, searches the code maps, stores the result (or `0xFFFF`) at 0x1A, and returns it. It is a `const` member, so both fields are `mutable`.
- Every caller inlines the cached form: `SetAlternateChar`, `GetCharWidths`, `GetGlyph`, `HasGlyph`, and `GetGlyphIndex`.

The fields are named `mLastCharCode` and `mLastGlyphIndex` from this behavior (medium confidence; no original name is embedded in the executable).

`GetCharWidthsFromIndex(const FontWidth*, u16)` has no out-of-line copy in the target. The compiled object emits one after `GetCharWidthsFromIndex(u16)`, but nothing references it and the linker removes it, so the link matches.

## Headers

`include/nw4r/ut/ut_ResFontBase.h` comes from the same ogws commit with comments removed. It adds `RemoveResourceBuffer`, `HasGlyph`, and the two cache members. The resource structures (`FontInformation`, `FontTextureGlyph`, `FontWidth`, `FontCodeMap`) keep the ogws layout; every offset the unit reads agrees with it.

## Traps

- **Function order in `nm` output can mislead.** A relocation-masked search listed functions in ELF symbol-table order, which put `FindGlyphIndex(const FontCodeMap*, u16)` and `GetGlyphFromIndex` out of place. `powerpc-eabi-nm -n` showed the object emits them in definition order, matching the target. Check addresses, not symbol-table order, before reordering definitions.
