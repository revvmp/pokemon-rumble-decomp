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

# F-449-3: Accessor-based GLGR reconstruction

The remaining `ConstructOpAnalyzeGLGR` body now uses the existing `FontGlyphGroupsAcs` interface from the pinned CC0 Wii News Channel source recorded above. This removes the hand-expanded glyph-group offset calculations and the declaration ordering inherited from the forecast reconstruction. No header, ABI, split, symbol, compiler setting, or matching status changes.

With Wii/1.1 and the existing `cflags_nw4r`, the function remains 0x3C0 bytes. `objdiff-cli` reports 99.291664%, up from 98.75%; the other 24 functions and `.data` remain 100%. A direct comparison against the immutable DOL at `0x80295FD0` finds 32 differing non-relocation instruction words, down from 52. This comparison retains function-relative branch offsets and masks only the object's actual relocation words. The differing words assign registers differently; this is not an exact match, and the unit remains `NonMatching`.

The target keeps `pFileTop` in `r25`, `bodySize` in `r26`, and the block header in `r19`. The first remaining difference is the block-end destination at function offset `0xD4`: target `r24`, reconstruction `r28`. At `0x1B8` the target consumes that block end, then reuses `r24` for `numSheet` at `0x1BC`. Later live values also differ: target glyph-group data `r29`, glyphs per sheet `r31`, adjustment-table size `r30`, sheet-flag stride `r28`, and adjustment-table pointer `r22`. The new source assigns those values `r30`, `r24`, `r22`, `r29`, and `r31`, respectively. These observations locate the remaining register allocation problem; they do not establish its cause.

## Investigations that did not establish a solution

- Wii compiler versions 1.0RC1, 1.0a, 1.0, 1.1, 1.3, 1.5, and 1.7 generate identical code for both GLGR source forms and preserve the other 24 matches. GameCube 3.0a5.2 changes eight additional functions and shortens GLGR by four bytes.
- Scalar type combinations, equivalent allocation/alignment expressions, count-read order, loop scopes and counters, and inlined stream-copy local declarations did not improve the accessor version. Reproducing the reference's nested target-buffer struct, mutable temporary stream pointers, and one-element trailing glyph-group array also left the same 32 words different. None of those header changes is retained.
- Hoisting and permuting outer local declarations reached 30 differing words, but retained artificial declaration ordering; the clearer 32-word accessor version is kept. This does not justify another blind declaration-order search.
- Alternative optimization and inlining flags either leave the same result or regress already matching functions. In particular, disabling common-subexpression elimination reduces GLGR's word count but breaks nine functions. No flag change is retained.

The full build still selects the original `ut_ArchiveFontBase` object. A passing frozen-DOL verification therefore proves that this partial reconstruction preserves the build, not that the GLGR function is reconstructed exactly. Issue #449 remains unfinished and gains no source-coverage credit from this change.

## Traps

- **A smaller register diff can regress the unit.** Disabling common-subexpression elimination reduced GLGR's differing-word count while breaking nine functions. Compare every function in the object before treating a compiler experiment as evidence for a setting change.
