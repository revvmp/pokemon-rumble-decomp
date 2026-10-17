# F-447-1: NW4R ut RomFont unit and the revised Font interface

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. `nw4r/ut/ut_RomFont.cpp` begins with `RomFont::RomFont` at `0x80294540`, the last function of #446, and covers the first 25 functions of #447 (`0x802945A0–0x80294C20`). It completes #446. The remaining #447 function, `0x80294C20`, is the `ResFontBase` constructor: it stores the `Font` vtable, a second `CharStrmReader` pointer constant (`0x803C6AE8`), and the vtable at `0x803C6AF8`, then sets fields 0x10 and 0x14 to null, 0x18 to 0, and 0x1A to `0xFFFF`. It opens `ut_ResFontBase.cpp`, which continues into #448's range.

## Unit and boundaries

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80294540–0x80294C20` | 26 functions, constructor through `GetEncoding` |
| `.data` | `0x803C6A18–0x803C6AE8` | default reader pointer constant, `RomFont` vtable (`0x803C6A28`), `Font` vtable (`0x803C6A88`) |
| `.sdata` | `0x804B89F8–0x804B8A00` | `RomFont::mFontEncode` (`u16`, `0xFFFF`) and padding |

The `.sdata` split ends at `0x804B8A00`, past the two-byte object, because dtk rejects an automatic split that starts at the unaligned `0x804B89FA`.

`ut_RomFont` is `Matching`: objdiff reports 100% for all 26 functions and for its data, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Differences from the 2006 reference

The source starts from ogws `src/nw4r/ut/ut_RomFont.cpp` at commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73` (CC0-1.0); Petari's `RomFont.h` (CC0) supplies the later `Unload` and `HasGlyph` declarations. Changes:

- `Unload` (`0x80294700`) follows `Load`. It returns the font buffer and clears `mFontHeader`.
- `HasGlyph` (`0x80294B30`) is a virtual between `GetGlyph` and `GetEncoding`. It applies the ogws per-encoding character tests and returns false for other encodings. `HandleUndefinedChar` becomes `HasGlyph(ch) ? ch : mAlternateChar`, which calls through the vtable at offset 0x58. `SetAlternateChar`, `GetCharWidth`, and `GetGlyph` inline it. `MakeCharPtr` and `HandleUndefinedChar` remain in the source, but their out-of-line copies are unreferenced and the linker removed them.
- The character tests must place the constant first (`0x20 <= ch && ch <= 0xFF`). The ogws form, `ch >= 0x20 && ch <= 0xFF`, gets the compiler's range-check rewrite (`addi`, `clrlwi`, single `cmplwi`); the constant-first form keeps the two separate comparisons the target uses. The SJIS half-width range ends at `0xDF` (the ogws `VERSION_RSPE01_01` form).

## Font vtable

The target `Font` vtable is 0x60 bytes, four more than both the ogws interface and Xenoblade's `__vt__Q34nw4r2ut4Font` (0x5C). Comparing the `RomFont` vtable with the `Load` virtual-call offsets (`GetCellWidth` +0x28, `GetMaxCharWidth` +0x2C, `GetEncoding` +0x5C) shows two additions to `ut_Font.h`:

- `HasGlyph` at +0x58, a pure virtual, as in Petari.
- A non-pure virtual at +0x20, directly after `GetBaselinePos`. Every `Font`-derived vtable in the executable (nine references, including `0x803C6A48`) points it to `0x8012F070`, a 0x10-byte body that tail-calls the +0x1C slot (`GetBaselinePos`) and passes `r4` through. The `PairFont` code at `0x80297A70` calls it with a character code after testing `HasGlyph` on each font. It is therefore modeled as `virtual int GetBaselinePos(u16 ch) const { return GetBaselinePos(); }`. No original name is available, so this name is low confidence.

`fn_8012F070` is renamed `GetBaselinePos__Q34nw4r2ut4FontCFUs` so the linker discards the weak copy emitted by this unit.

## Traps

- **Constant-first comparisons change code generation.** `ch >= 0x20 && ch <= 0xFF` and `0x20 <= ch && ch <= 0xFF` compile differently under Wii/1.1: only the first gets the subtract-and-compare range rewrite. When a target shows two separate `cmplwi` tests where the reference compiles to `addi`/`cmplwi`, swap the operand order before restructuring the code.
- **A short `.sdata` object can need its padding.** dtk refuses an automatic split at an unaligned address. End the split at the next aligned boundary (here `0x804B8A00`) when the unit's last object ends mid-word.
