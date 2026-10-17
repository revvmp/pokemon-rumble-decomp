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

# F-450-2: The target CharWriter is a later revision than every public reference

`ut_CharWriter.cpp` starts at `0x802983D0` and runs into #451, ending before the `TextWriterBase` functions (`TextWriterBase<char>::GetLineHeight` is at `0x8029A8F0`). Its functions do not match the size profile of any CC0 reference checked: ogws, Petari, zeldaret/ss (commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`), xbret/xenoblade (commit `ea25691b2029394ab777bda7060139e517a13cba`), koopthekoopa/wii-ipl, doldecomp/forecast, and the Wii News Channel. Each of those has a 0x220-byte constructor, a 0xA58-byte `SetupGX`, and one `SetupGXWithColorMapping`; the HBM copy (`nw4hbm`) in Xenoblade has a 0x38C constructor and a 0x950 `SetupGX`.

| Address | Size | Evidence |
| --- | --- | --- |
| `0x802983D0` | `0x380` | constructor; references `mLoadingTexture` (`.bss` `0x804524C8`); initializes a 0x4C-byte object with byte colour fields at 0x8–0x1B and 0x18–0x1B, floats at 0x24–0x34, and words at 0x38 and 0x3C |
| `0x80298750` | `0x40` | destructor |
| `0x80298790` | `0x930` | `SetupGX`: clears the texture cache, calls `0x80299E80` when colour mapping is set, and otherwise inlines four format-specific GX setups |
| `0x802990C0` | `0x9F8` | a second setup routine using `GXSetTevColor` and `GXSetTevKColor`, which calls `0x8029A170` and `0x8029A4E0`; no counterpart in the references |
| `0x80299AC0`, `0x80299B90` | `0xC4`, `0x78` | `SetFontSize(f32, f32)` and `SetFontSize(f32)` by size |
| `0x80299C10`–`0x80299CD0` | `0x60` each | `GetFontWidth`, `GetFontHeight`, `GetFontAscent` |
| `0x80299D30` | `0x150` | probably `UpdateVertexColor` (Xenoblade size 0x150) |
| `0x80299E80` | `0x2EC` | `SetupGXWithColorMapping`-style setup with `GXSetTevColor` |
| `0x8029A170`, `0x8029A4E0` | `0x370`, `0x2AC` | further colour-mapping setups; the first uses `GXSetTevKColorSel` and `GXSetTevKAlphaSel` with three TEV stages |

`Print` and `PrintGlyph` were not found at their reference sizes in this range and may be in another unit or inlined. The unit needs a reconstruction from the disassembly; the SS source is the closest starting point for the shared parts (`SetupGXCommon`, `SetupVertexFormat`, the font metric accessors).
