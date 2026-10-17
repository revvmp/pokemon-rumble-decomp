# F-449-1: NW4R ut ResFont unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #449 owns `.text` `0x80295470–0x80297594`. Its first function, `ResFontBase::GetGlyphFromIndex` at `0x80295470`, closes `ut_ResFontBase.cpp` ([F-448-1](448-ut-resfontbase.md)). The next five functions form `ut_ResFont.cpp`.

## Unit and boundaries

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80295580–0x8029588C` | constructor, destructor, `SetResource`, `RemoveResource`, `Rebuild` |
| `.data` | `0x803C6B58–0x803C6BB8` | `ResFont` vtable, 0x60 bytes |

The unit is `Matching`: objdiff reports 100% for all five functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

The source is ogws `src/nw4r/ut/ut_ResFont.cpp` at commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73` (CC0-1.0), unchanged except for `RemoveResource` (`0x80295770`), a four-byte tail branch to `ResFontBase::RemoveResourceBuffer`. Its declaration follows Petari `ResFont.h` at commit `e0ab141914921726a595573dc48d3f13d0922efe` (CC0). The ogws placeholder `SIGNATURE_GLGR` is renamed `SIGNATURE_GLYPHGROUP`, after the glyph-group block that `ArchiveFontBase` reads. `include/nw4r/types_nw4r.h` gains the ogws `FOURCC` macro.

# F-449-2: NW4R ut ArchiveFontBase and ArchiveFont units

The rest of #449 is the archive (`.brfna`) font, which streams a compressed font into a caller buffer and loads only the selected glyph groups.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/ut/ut_ArchiveFontBase.cpp` | `.text` | `0x80295890–0x80296DC8` | 25 functions: constructor, destructor, `GetCharWidths`, `HasGlyph`, `SetResourceBuffer`, `RemoveResourceBuffer`, `AdjustIndex`, `IncludeName`, `IsValidResource`, 13 `ConstructOp*` handlers, and `CachedStreamReader::Init`, `Attach`, `RequestData` |
| | `.data` | `0x803C6BB8–0x803C6C18` | `ArchiveFontBase` vtable, 0x60 bytes |
| `nw4r/ut/ut_ArchiveFont.cpp` | `.text` | `0x80296DD0–0x80297710` | constructor, destructor, `GetRequireBufferSize`, `StreamingConstruct`, `Construct`, `Destroy`, `GetGlyph`, `GetGlyphFromIndex` |
| | `.data` | `0x803C6C18–0x803C6CB0` | 13-entry `StreamingConstruct` jump table, `ArchiveFont` vtable |

`ut_ArchiveFont.cpp` begins in #449 and ends in #450's estimated range: `GetGlyph` (`0x802975A0`) and `GetGlyphFromIndex` (`0x802975F0`) are its last two functions. The next function, `0x80297710`, stores the `Font` vtable, a reader constant (`0x803C6CB0`), and the vtable at `0x803C6CC0`; it is the constructor of the following unit (`PairFont`, from F-447-1's call evidence).

`ut_ArchiveFont` is `Matching`: objdiff reports 100% for all eight functions and its data, and the source-linked `main.dol` passes `tools/verify/verify.py`. `ut_ArchiveFontBase` is `NonMatching`: 24 of its 25 functions and its data score 100%. `ConstructOpAnalyzeGLGR` (`0x80295FD0`, 0x3C0 bytes) has the target's size and instruction sequence but a different assignment of callee-saved registers (98.75%). The target keeps the file header in `r25`, the body size in `r26`, `pBlockEnd` and then `numSheet` in `r24`, the glyph-group block in `r29`, and the sheet flag stride in `r28`; the reconstruction assigns `r27`, `r28`, `r30`, `r26`, and `r25`. A hill-climbing search over about 1,100 declaration orders of the function's locals reduced the difference from 104 to 52 differing instructions but did not remove it, so the remaining cause is probably the expression or scope structure, not declaration order alone. Compiler and flags are those of F-25-1.

## Sources and names

Function and class names come from the symbol maps of two CC0 decompilations that contain the same library revision: doldecomp/forecast (commit `a1f28f8a0d777ebd12953f5c6997ee2decfd07f5`) and hotlandsoftware/wii-news-channel (commit `128e3ffbaee9e70bd175c5f66a44fb8430068629`). Both place `ArchiveFontBase` in `nw4r::ut::detail` (the news-channel map spells `Q44nw4r2ut6detail15ArchiveFontBase`, as does Xenoblade's). The sources here start from those two CC0 reconstructions and differ from them as follows:

- `ArchiveFontBase` overrides `HasGlyph` (`0x802959E0`): it returns false unless `FindGlyphIndex` finds the character and `AdjustIndex` maps its sheet to a loaded one. Neither reference has this override; it follows the Petari `HasGlyph` interface.
- `mGlyphIndexAdjustArray` is at offset 0x1C, after the `ResFontBase` glyph cache of F-448-1, not 0x18.
- `IncludeName` scans back over spaces with `while (pPrev > pNameList && *pPrev == ' ')`. Other loop shapes give the same instructions with a different entry branch.
- `IsValidResource` is inlined into `ConstructOpAnalyzeGLGR`, as in the target.

`ArchiveFont::GetRequireBufferSize` (0x404 bytes, 0x400 in the news channel) uses the news-channel `FontGlyphGroupsAcs` accessor unchanged; the size difference comes from the compiler and matches as written. `InitStreamingConstruct` is inline. Callees renamed for linking: `math::CntBit1` (`0x802A34A0`, 0x58 bytes, called once by `GetRequireBufferSize` on the merged sheet bits, size equal to both references' `CntBit1`), `strchr` (`0x80345F38`) and `strstr` (`0x80345F68`), identified by their arguments in `IncludeName` (`','` and the group name).

## Header changes

- `include/string.h` declares `strlen`, `strchr`, and `strstr`.
- `include/Runtime/__mem.h` includes `stddef.h` instead of `string.h`. `__mem.c` defines a weak `strlen` after switching from `.init` to `.text`; CodeWarrior places a function in the section active at its first declaration, so a `strlen` prototype from `string.h` moved the function into `.init` and shifted the DOL. Only `__mem.c` includes this header, and its object is unchanged.
- `include/nw4r/math/math_arithmetic.h` declares `CntBit1`.
- `ResFontBase` helper functions and members become `protected` so derived fonts can use them.

## Traps

- **A prototype can move a function into another code section.** Declaring `strlen` in `string.h` put `__mem.c`'s weak `strlen` into `.init`, because the declaration preceded `#pragma section code_type ".text"`. Check `.init` size first when an unrelated header change shifts the whole DOL.
- **Masking branch targets hides loop-shape differences.** A diff script that replaced every branch target with a placeholder reported `IncludeName` as identical while objdiff showed one mismatched entry branch. Compare function-relative branch offsets.
