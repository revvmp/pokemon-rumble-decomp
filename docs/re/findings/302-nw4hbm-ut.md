# Issue 302: HBM nw4hbm::ut, blocks 4 to 6

## F-302-1: ut_TagProcessorBase and ut_TextWriterBase

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The blocks of #302, #303 and #304 (`0x801943A0`–`0x8019A2C4`, 95 functions) hold two units. `ut_TextWriterBase.cpp` starts in #302's block, so #302 owns its split; all three blocks are done here.

| File | `.text` | Other sections |
| --- | --- | --- |
| `RVL_SDK/hbm/nw4hbm/ut/ut_TagProcessorBase.cpp` | `0x801943A0`–`0x80194E58` (12 functions) | `.rodata` `0x80366F48`–`0x80366F50`, `.data` `0x803BCDE8`–`0x803BCE70` |
| `RVL_SDK/hbm/nw4hbm/ut/ut_TextWriterBase.cpp` | `0x80194E60`–`0x8019A2C4` (83 functions) | `.ctors` `0x80352A0C`–`0x80352A10`, `.rodata` `0x80366F50`–`0x80366F68`, `.data` `0x803BCE70`–`0x803BCE78`, `.bss` `0x803E9C30`–`0x803E9C60` |

The function list, sizes and offsets are identical to CC0 xenoblade's `config/jp/symbols.txt` (`ea25691b`) from `TagProcessorBase<char>`'s constructor to `__sinit_\ut_TextWriterBase_cpp`, and its `splits.txt` gives the same section extents. Its names are used for all 95 functions and the data. `0x8019A2D0` starts HBM sound (#305).

Evidence:

- **TagProcessorBase.** The repository's matched `nw4r::ut` source, moved into namespace `nw4hbm`. `ProcessLinefeed` (`0x80194790`) calls `SetCursor(x, y)` where the NW4R copy calls `SetCursorX` and `SetCursorY`. The `.data` holds the `<wchar_t>` vtable and RTTI, then the `<char>` vtable and RTTI.
- **TextWriterBase layout.** The class is 0x60 bytes: `CharWriter` (0x4C), `mCharSpace` `0x4C`, `mLineSpace` `0x50`, `mTabWidth` `0x54`, `mDrawFlag` `0x58`, `mTagProcessor` `0x5C`. The constructor (`0x80194E60`) stores exactly these. Unlike later NW4R, there is no width limit.
- **Function order.** The explicit instantiations of `TextWriterBase<char>` and `<wchar_t>` emit every member out of line, in declaration order, including the one-line accessors (`SetLineSpace` to `GetTagProcessor`, `SetBuffer`, `GetBuffer`). The header declares them in the target's order, which follows CC0 Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, `src/hbm/nw4hbm/ut/ut_TextWriterBase.hpp`). The logic follows the same file.
- **`VSNPrintf` and `StrLen`.** Each instantiation has 4-byte copies that branch to `vsnprintf`, `vswprintf`, `strlen` or `wcslen`. They are static template members that call overloaded file-local helpers.
- **Statics.** `mFormatBufferSize` is in `.data` (256 each). `.bss` holds the two destructor-chain links, `mFormatBuffer` and `mDefaultTagProcessor` for each character type, and the two guard bytes. The static initializer at `0x8019A220` constructs both default processors.
- **Outside name.** `0x801EE3C0` is MSL `vswprintf`: it runs the wide formatter with a string-writing callback and stores a halfword terminator. It is named for the link.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with both units `Matching`. objdiff reports all 95 functions and every `.text`, `.rodata`, `.data` and `.ctors` section at 100%. `ut_TextWriterBase.cpp` `.bss` scores 87.5% because of the compiler-named guard objects. The linked DOL is byte-identical.

### Traps

- **An inline explicit specialization gives no out-of-line copy.** Declaring `TextWriterBase<char>::VSNPrintf` and `StrLen` as inline explicit specializations left the four 4-byte wrappers missing from the object. The explicit instantiation emits a member only when it comes from the primary template, so the members need a template body.
- **Explicit instantiation order follows the class declaration.** Every member of `TextWriterBase` is emitted in the order the class declares them, not the order the source defines them. Order the declarations by the target addresses.
