# F-449-1: NW4R ut ResFont unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #449 owns `.text` `0x80295470–0x80297594`. Its first function, `ResFontBase::GetGlyphFromIndex` at `0x80295470`, closes `ut_ResFontBase.cpp` ([F-448-1](448-ut-resfontbase.md)). The next five functions form `ut_ResFont.cpp`.

## Unit and boundaries

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80295580–0x8029588C` | constructor, destructor, `SetResource`, `RemoveResource`, `Rebuild` |
| `.data` | `0x803C6B58–0x803C6BB8` | `ResFont` vtable, 0x60 bytes |

The unit is `Matching`: objdiff reports 100% for all five functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

The source is ogws `src/nw4r/ut/ut_ResFont.cpp` at commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73` (CC0-1.0), unchanged except for `RemoveResource` (`0x80295770`), a four-byte tail branch to `ResFontBase::RemoveResourceBuffer`. Its declaration follows Petari `ResFont.h` at commit `e0ab141914921726a595573dc48d3f13d0922efe` (CC0). The ogws placeholder `SIGNATURE_GLGR` is renamed `SIGNATURE_GLYPHGROUP`, after the glyph-group block that `ArchiveFontBase` reads. `include/nw4r/types_nw4r.h` gains the ogws `FOURCC` macro.

## Following units

The rest of #449 holds two classes without public reference source. Names below come from the symbol map in the Xenoblade decompilation's `symbols.txt`, which lists the same NW4R revision; sizes are this target's.

- `ArchiveFontBase` (`nw4r::ut::detail`) from `0x80295890`: constructor, destructor, `GetCharWidths`, `HasGlyph`, `SetResourceBuffer`, `RemoveResourceBuffer`, `AdjustIndex`, `IncludeName`, `IsValidResource`, the `ConstructOp*` stream parser, and `CachedStreamReader` (`Init`, `Attach`, `RequestData`, through `0x80296DC4`). Vtable at `0x803C6BB8`.
- A class derived from it from `0x80296DD0`, with vtable `0x803C6C50` (`GetGlyph` at `0x802975A0`, in #450), a 13-entry jump table at `0x803C6C18`, and functions through `0x80297594`. Its slot layout and function sizes differ from Xenoblade's `PackedFont`, so it is probably `ArchiveFont`.
