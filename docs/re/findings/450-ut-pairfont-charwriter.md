# F-450-1: NW4R ut PairFont unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #450 owns `.text` `0x802975A0–0x8029A16C`. Its first two functions, `ArchiveFont::GetGlyph` and `GetGlyphFromIndex`, close `ut_ArchiveFont.cpp` ([F-449-2](449-ut-resfont-archivefont.md)). The next 23 functions form `ut_PairFont.cpp`.

## Unit and boundaries

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80297710–0x802983C4` | constructor, destructor, and the 21 `Font` virtuals |
| `.data` | `0x803C6CB0–0x803C6D20` | default reader pointer constant used by the inline `Font` constructor, `PairFont` vtable (`0x803C6CC0`, 0x60 bytes) |

The unit is `Matching`: objdiff reports 100% for all 23 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`). The following function, `0x802983D0`, begins `ut_CharWriter.cpp`.

## Source

The source starts from `src/nw4r/ut/ut_PairFont.cpp` in the CC0 Etrainquandt17/Whippy decompilation (commit `ef8403b7150210639ad3a0683cd3f86e6d03dee5`), which that project wrote from disassembly. Its header there was a stub, so `include/nw4r/ut/ut_PairFont.h` is new: `mFirst` at 0x10, `mSecond` at 0x14, and `mAlternateWithFirst` (`bool`) at 0x18, as the constructor stores them; the class is 0x1C bytes. The vtable at `0x803C6CC0` fixes the virtual order.

The one addition is the override of the `Font` virtual at offset 0x20 (`0x80297A70`, F-447-1's `GetBaselinePos(u16)`), which selects a font with the same `HasGlyph` fallback as `GetCharWidth`, `GetCharWidths`, and `GetGlyph`. This override is the call evidence F-447-1 cited; the member name remains low confidence.
