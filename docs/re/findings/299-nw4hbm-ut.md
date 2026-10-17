# Issue 299: HBM nw4hbm::ut, block 1

## F-299-1: math_triangular, ut_binaryFileFormat, ut_CharStrmReader and ut_CharWriter

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The block `0x80191880`–`0x80192A54` holds four Home Button Menu units. The last one, `ut_CharWriter.cpp`, continues into #300's block.

| File | `.text` | Other sections |
| --- | --- | --- |
| `RVL_SDK/hbm/nw4hbm/math/math_triangular.cpp` | `0x80191880`–`0x80191B30` (3 functions) | `.rodata` `0x80366F00`–`0x80366F24`, `.data` `0x803BBB00`–`0x803BCC18` |
| `RVL_SDK/hbm/nw4hbm/ut/ut_binaryFileFormat.cpp` | `0x80191B30`–`0x80191BA4` (1 function) | none |
| `RVL_SDK/hbm/nw4hbm/ut/ut_CharStrmReader.cpp` | `0x80191BB0`–`0x80191CD0` (4 functions) | none |
| `RVL_SDK/hbm/nw4hbm/ut/ut_CharWriter.cpp` | `0x80191CD0`–`0x801936CC` (26 functions) | `.rodata` `0x80366F28`–`0x80366F48`, `.bss` `0x803E9C18`–`0x803E9C30` |

The unit order and function sizes are identical to CC0 xenoblade's `config/jp/symbols.txt` and `splits.txt`, which give the names. The code follows CC0 Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, `src/hbm/nw4hbm`). `ut_binaryFileFormat.cpp` and `ut_CharStrmReader.cpp` are the repository's matched `nw4r::ut` sources, moved into namespace `nw4hbm`. `0x801936D0` starts `ut_Font.cpp` (`Font::InitReaderFunc`).

Evidence:

- **math_triangular.** The `.data` range holds the 257-entry sine and cosine table and the 33-entry arctangent table, both in an anonymous namespace. The `.rodata` pool is `65536.0f`, `0.0f`, `1.0f`, `32.0f`, `64.0f`, `-1.0f`, `-64.0f`, `128.0f`, `-128.0f`. The `1.0f`, `32.0f`, `-1.0f` and `-64.0f` entries come from `AtanFIdx`, which the linker dead-strips. Writing `AtanFIdx` between `CosFIdx` and `Atan2FIdx` reproduces the pool order without placeholder code. The 4 bytes at `0x80366F24` are padding before the next unit's 8-byte-aligned `.rodata`, so the split ends at `0x80366F24`. `lbl_80366F04` is now 0x20 bytes, in place of 0x24.
- **ut_CharWriter.** `CharWriter` is 0x4C bytes. The layout follows Kirby's Epic Yarn: color mapping at `0x0`, vertex colors at `0x8`, text color at `0x18`, scale at `0x24`, cursor at `0x2C`, texture filter at `0x38`, alpha at `0x42`, the fixed-width flag at `0x43`, fixed width at `0x44` and font at `0x48`. The `.bss` holds the guard byte and storage of `SetupGXCommon`'s local static fog color (`0x803E9C18`, `0x803E9C1C`), then `CharWriter::mLoadingTexture` (`0x803E9C20`, 0x10 bytes). `SetScale`, `ResetColorMapping`, `SetTextColor(Color)`, `EnableLinearFilter`, `SetCursor(f32, f32, f32)`, `LoadTexture`, `UpdateVertexColor` and the static `SetupGX*` helpers are inlined into their callers. The linker dead-strips their out-of-line copies. `SetupGXWithColorMapping` is the only static helper that keeps its own copy.
- New headers: `nw4hbm/ut/ut_CharStrmReader.h`, `nw4hbm/ut/ut_CharWriter.h` and `nw4hbm/ut/ut_Font.h`. The `Font` class carries Kirby's Epic Yarn's pure virtual list, with `u16` character codes, which the `Print__...FUs` mangling requires. `CharWriter` uses the `Font` vtable slots `0xC` (`GetWidth`), `0x10` (`GetHeight`), `0x14` (`GetAscent`), `0x30` (`GetTextureFormat`) and `0x50` (`GetGlyph`), and the unit matches with them. The other slots have not been checked against a target vtable.
- `revolution/gx/GXGeometry.h` gains the SDK inline `GXSetTexCoordGen`, which calls `GXSetTexCoordGen2` with `GX_FALSE` and `GX_PTIDENTITY`.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with all four units `Matching`. objdiff reports all 34 functions and every `.text`, `.rodata`, `.data` and `.bss` section at 100%.

### Traps

- **A constant pool can record functions that the linker removed.** The `math_triangular.cpp` `.rodata` contains constants used by no surviving function. Before inventing placeholder code to order a pool, look for a stripped sibling function from the reference library, here `AtanFIdx`.
