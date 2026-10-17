# F-445-1: NW4R ut list, link list, binary header, character reader, and stream units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #445 owns `.text` `0x80292D00–0x80293CC4`. Six of its seven translation units are reconstructed here (F-445-1), covering 24 of its 36 functions plus three `FileStream` functions that continue into #446. F-445-2 adds `ut_TagProcessorBase.cpp`, the remaining 12 functions.

## Units and boundaries

| Unit | Section | Range | Functions |
| --- | --- | --- | --- |
| `nw4r/ut/ut_list.cpp` | `.text` | `0x80292D00–0x80292F5C` | `List_Init`, `List_Append`, `List_Insert`, `List_Remove`, `List_GetNext`, `List_GetPrev` |
| `nw4r/ut/ut_LinkList.cpp` | `.text` | `0x80292F60–0x802930F0` | `LinkListImpl::~LinkListImpl`, `Erase(Iterator)`, `Clear`, `Insert`, `Erase(LinkListNode*)` |
| `nw4r/ut/ut_binaryFileFormat.cpp` | `.text` | `0x802930F0–0x80293164` | `IsValidBinaryFile` |
| `nw4r/ut/ut_CharStrmReader.cpp` | `.text` | `0x80293170–0x80293290` | `ReadNextCharUTF8`, `UTF16`, `CP1252`, `SJIS` |
| `nw4r/ut/ut_IOStream.cpp` | `.text` | `0x80293BF0–0x80293C3C` | `ReadAsync`, `Write`, `WriteAsync`, `IsBusy`, static initializer |
| | `.ctors` | `0x80352A6C–0x80352A70` | initializer entry |
| | `.sbss` | `0x804B93B8–0x804B93C0` | `IOStream::typeInfo` and padding |
| `nw4r/ut/ut_FileStream.cpp` | `.text` | `0x80293C40–0x80293DBC` | `Cancel`, `CancelAsync`, `FilePosition::Skip`, `Append`, `Seek`, static initializer |
| | `.ctors` | `0x80352A70–0x80352A74` | initializer entry |
| | `.sbss` | `0x804B93C0–0x804B93C8` | `FileStream::typeInfo` and padding |

The order follows ogws link order: `ut_list`, `ut_LinkList`, `ut_binaryFileFormat`, `ut_CharStrmReader`, `ut_TagProcessorBase`, `ut_IOStream`, `ut_FileStream`. dtk had already split the two static initializers at `0x80293C30` and `0x80293DB0`, and the `.ctors` table entries at `0x80352A6C` and `0x80352A70` point to them. `FileStream` begins inside #445, so this issue owns its split, although `Append`, `Seek`, and its initializer lie in #446's estimated range.

`List_Prepend`, `List_GetNth`, the three-argument `LinkListImpl::Erase`, and the `IOStream` and `FileStream` vtables and inline virtuals are not in the target; the linker removed them. The sources keep the reference definitions.

All six units are `Matching`: objdiff reports 100% for all 27 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. The `.sbss` ranges include four bytes of padding after each four-byte `typeInfo`, which objdiff counts as unmatched data. Compiler and flags are those of F-25-1 and F-375-1.

## Differences from the 2006 reference

Sources follow ogws `src/nw4r/ut` at commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73` (CC0-1.0). Two code changes and one interface change were needed:

- `List_Remove` matches only when every link is computed with the `NW4R_UT_LIST_GET_LINK` macro form, `(u8*)object + list.offset`. A static helper that returned the same pointer swapped `r5` and `r6`. The ogws source already uses the macro; it differed elsewhere.
- `ReadNextCharSJIS` (0x60 bytes, against 0x70) needs the lead-byte test written as `((0x81 <= ch && ch < 0xA0) || 0xE0 <= ch) ? true : false`. The target evaluates both comparisons into a boolean before branching, without the range-check rewrite that the plain expression gets.
- `IOStream::Write` returns `s32` and the value 0, not `void`. The four inherited `IOStream` slots in derived vtables, for example at `0x803C62D8`, point at four identical `li r3,0; blr` bodies in vtable order: `ReadAsync`, `Write`, `WriteAsync`, `IsBusy`.

## Headers

These ut headers come from the same ogws commit, stripped with `tools/vendor/vendor_strip.py`: `ut_LinkList.h`, `ut_NonCopyable.h`, `ut_binaryFileFormat.h`, `ut_CharStrmReader.h`, `ut_IOStream.h`, `ut_FileStream.h`, `ut_RuntimeTypeInfo.h`, and `ut_algorithm.h`. Adaptations: `DECLTYPE` became `__decltype__`, `ASM` became `asm`, and `IOStream::Write` returns `s32`. `ut_list.h` already exists from #375.

## F-445-2: TagProcessorBase completes the block

`nw4r/ut/ut_TagProcessorBase.cpp` covers `.text` `0x80293290–0x80293BE4`, `.data` `0x803C6960–0x803C6988`, and `.sdata2` `0x804BB0C0–0x804BB0C8`. The text holds the `char` instantiation from `0x80293290` and the `wchar_t` instantiation from `0x80293740`. Each has a constructor, destructor, `Process`, `CalcRect`, `ProcessLinefeed`, and `ProcessTab`. The data holds the `wchar_t` vtable (`0x803C6960`), then the `char` vtable (`0x803C6974`), and the int-to-float conversion double. All 12 functions match the ogws source unchanged, and the unit is `Matching` with full DOL verification.

The target places `ProcessLinefeed` before `ProcessTab` in both instantiations. CodeWarrior emits explicitly instantiated members in class declaration order, not definition order, so the header declares `ProcessLinefeed` first. Callees renamed for linking: `CharWriter::GetFontWidth` `0x80299C10`, `CharWriter::GetFontHeight` `0x80299C70`, `TextWriterBase<char>::GetLineHeight` `0x8029A8F0`, and `TextWriterBase<wchar_t>::GetLineHeight` `0x8029E420`.

Headers added from ogws, stripped: `ut_TagProcessorBase.h`, `ut_TextWriterBase.h`, `ut_CharWriter.h`, `ut_Font.h`, `ut_Color.h`, and `ut_Rect.h`. `ut_TextWriterBase.h` includes `stdio.h` and `wchar.h` and calls the global `vsnprintf` and `vswprintf`. The math umbrella includes became `math/math_types.h`. `include/stdio.h` gained a `vsnprintf` declaration. A new `include/wchar.h` declares `vswprintf`; it relies on the existing `stdarg.h` and `wchar_t.h`.

With this unit, every function in #445 is in a `Matching` translation unit.

## Traps

- **An inline helper can change register allocation where a macro does not.** `List_Remove` produced identical instructions with `r5` and `r6` swapped when the link pointer came from a static inline function. The macro expression reproduced the target. Try the reference's macro form before rewriting logic.
- **Explicit template instantiation follows declaration order.** `TagProcessorBase` member functions were emitted in the class's declaration order. Reorder the declarations, not the definitions, to match function order.
- **Operand order and an explicit ternary change CodeWarrior's boolean code.** `ch >= 0x81` and `0x81 <= ch` compile differently inside a `? true : false` expression.
