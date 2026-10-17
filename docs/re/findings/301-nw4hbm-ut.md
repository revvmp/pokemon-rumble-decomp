# Issue 301: HBM nw4hbm::ut, block 3

## F-301-1: ut_ResFontBase

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The whole block is one unit:

| File | `.text` | Other sections |
| --- | --- | --- |
| `RVL_SDK/hbm/nw4hbm/ut/ut_ResFontBase.cpp` | `0x80193D50`–`0x80194398` (24 functions) | `.data` `0x803BCD28`–`0x803BCDE8` |

The function list and sizes are identical to CC0 xenoblade's `config/jp/symbols.txt` (`ea25691b`), which names all 24 functions, and its `splits.txt` gives the same `.text` and `.data` extents. The source follows xenoblade's `ut_ResFontBase.cpp`. `0x801943A0` starts `ut_TagProcessorBase.cpp` (#302).

Evidence:

- **No glyph cache.** Unlike this game's `nw4r::ut::ResFontBase` ([F-448-1](448-ut-resfontbase.md)), the HBM copy has no one-entry lookup cache. The constructor stores only `mResource` (`0x10`) and `mFontInfo` (`0x14`), and `FindGlyphIndex` searches the code maps on every call. The class is 0x18 bytes. #300's header carried the two cache members; they are removed.
- **Inlining.** `GetGlyphIndex`, `FindGlyphIndex(u16)` and both `GetCharWidthsFromIndex` overloads are inlined into their callers, and the linker strips their out-of-line copies, as it does `RemoveResourceBuffer`. `FindGlyphIndex(const FontCodeMap*, u16)` (`0x801941C0`) and `GetGlyphFromIndex` (`0x80194290`) keep their own copies.
- **`.data`.** The default reader pointer-to-member of the inline `Font` constructor (`0x803BCD28`, 0x10 bytes), the `ResFontBase` vtable (`0x803BCD38`) and the weak `Font` vtable (`0x803BCD90`). The RTTI records both vtables use are #300's copies in `ut_ResFont.cpp`; this object's duplicates are dropped at link. objdiff scores `.data` at 91.7% for that reason only.
- **Weak `Font` destructor.** The `Font` vtable points to `0x8018F1A0`, a 0x40-byte destructor in #297's block, the first linked copy. Named `fn_8018F1A0`, it did not replace this object's weak copy, which stayed in `.text` and shifted the rest of the unit by 0x40. Naming it `__dt__Q36nw4hbm2ut4FontFv`, as xenoblade does, lets the linker drop the duplicate.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with the unit `Matching`. objdiff reports all 24 functions and `.text` at 100%.

### Traps

- **A weak function's first copy must carry its real name.** The compiled object's weak `Font::~Font` was linked in addition to the fallback copy named `fn_8018F1A0`, shifting the unit's later functions by 0x40. When a vtable in your object points into another block, give that function its mangled name in `symbols.txt`.
