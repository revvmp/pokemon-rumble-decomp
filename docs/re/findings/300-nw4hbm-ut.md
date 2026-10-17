# Issue 300: HBM nw4hbm::ut, block 2

## F-300-1: ut_Font, ut_LinkList, ut_list and ut_ResFont

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). #299 matched the first 20 functions of this block, `0x80192A60`–`0x801936CC`, which end `ut_CharWriter.cpp`; see [F-299-1](299-nw4hbm-ut.md#f-299-1-math_triangular-ut_binaryfileformat-ut_charstrmreader-and-ut_charwriter). Four units follow, and they end at the block's end:

| File | `.text` | Other sections |
| --- | --- | --- |
| `RVL_SDK/hbm/nw4hbm/ut/ut_Font.cpp` | `0x801936D0`–`0x80193774` (1 function) | `.data` `0x803BCC18`–`0x803BCC48` |
| `RVL_SDK/hbm/nw4hbm/ut/ut_LinkList.cpp` | `0x80193780`–`0x801938DC` (4 functions) | none |
| `RVL_SDK/hbm/nw4hbm/ut/ut_list.cpp` | `0x801938E0`–`0x80193A50` (5 functions) | none |
| `RVL_SDK/hbm/nw4hbm/ut/ut_ResFont.cpp` | `0x80193A50`–`0x80193D4C` (4 functions) | `.data` `0x803BCC48`–`0x803BCD28` |

All four sources are the repository's matched `nw4r::ut` sources, moved into namespace `nw4hbm`. The unit order and sizes match CC0 xenoblade's `splits.txt` and `symbols.txt`, which give the names. There are two differences between the versions:

- This target's `LinkListImpl` keeps an out-of-line `Clear` (`0x80193860`, 0x44 bytes), which xenoblade's copy lacks. The two-argument and node `Erase` overloads are inlined, and the linker strips their copies.
- The `ResFontBase` vtable at `0x803BCC48` has no `HasGlyph` slot. `GetEncoding` is at slot `0x54`, the slot `ResFont::SetResource` calls. The new `include/nw4hbm/ut/ut_ResFontBase.h` drops `HasGlyph` for that reason. `ut_list.cpp` keeps `List_Prepend`, `List_Insert` and `List_GetPrev`, which nothing references, and the linker strips them.

The `.data` of `ut_Font.cpp` holds the four pointer-to-member constants of `InitReaderFunc`. The `.data` of `ut_ResFont.cpp` holds the `ResFont` vtable and the RTTI records of `ResFont` (`0x803BCCC8`), `detail::ResFontBase` (`0x803BCD00`) and `Font` (`0x803BCD20`). The `ResFontBase` vtable in the following fallback unit refers to the last two records. Their names had to be added to `symbols.txt` before the link succeeded.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with all four units `Matching`. objdiff reports all 14 functions and every `.text` and `.data` section at 100%.

The `ResFont` vtable refers to 18 `ResFontBase` members in #301's block. These are named for linking: the constructor `0x80193D50`, the destructor `0x80193DA0`, `SetResourceBuffer` `0x80193DE0`, and the virtual functions `GetWidth` (`0x80193DF0`) to `GetEncoding` (`0x801941B0`). The names come from xenoblade, whose function list and sizes are identical.

### Traps

- **RTTI records in a newly owned `.data` range can be referenced from fallback units.** Linking `ut_ResFont.cpp` failed with undefined `lbl_803BCD00` and `lbl_803BCD20`. Those labels are the `ResFontBase` and `Font` RTTI records, which the next unit's vtable refers to. Give the records their `__RTTI__` names in `symbols.txt`.
