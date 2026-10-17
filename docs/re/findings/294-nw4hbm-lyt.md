# Issue 294: HBM nw4hbm::lyt, block 2

## F-294-1: lyt_bounding, lyt_common, lyt_drawInfo and lyt_group

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). #293 matched this block's first five functions (`0x80187300`–`0x80187520`), which end `lyt_arcResourceAccessor.cpp`; see [F-293-1](293-nw4hbm-lyt.md#f-293-1-lyt_animationcpp-and-lyt_arcresourceaccessorcpp). Four more units follow:

| File | `.text` | Other sections |
| --- | --- | --- |
| `RVL_SDK/hbm/nw4hbm/lyt/lyt_bounding.cpp` | `0x80187520`–`0x80187694` (6 functions) | `.ctors` `0x803529F8`–`0x803529FC`, `.data` `0x803BB630`–`0x803BB718`, `.bss` `0x803E9BC0`–`0x803E9BC4` |
| `RVL_SDK/hbm/nw4hbm/lyt/lyt_common.cpp` | `0x801876A0`–`0x80188530` (16 functions) | `.rodata` `0x80366E58`–`0x80366E68`, `.bss` `0x803E9BC8`–`0x803E9BF0` |
| `RVL_SDK/hbm/nw4hbm/lyt/lyt_drawInfo.cpp` | `0x80188530`–`0x801885F0` (2 functions) | `.rodata` `0x80366E68`–`0x80366E70`, `.data` `0x803BB718`–`0x803BB748` |
| `RVL_SDK/hbm/nw4hbm/lyt/lyt_group.cpp` | `0x801885F0`–`0x8018893C` (5 functions) | `.data` `0x803BB748`–`0x803BB770` |

The function lists, sizes and order are identical to CC0 xenoblade's `config/jp/symbols.txt`, which gives the names. The code follows CC0 Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, `src/hbm/nw4hbm/lyt`). `0x80188940` starts `lyt_layout.cpp` (`SetTagProcessorImpl`), which runs into #295's block; see F-294-2.

Evidence and header corrections:

- `Bounding::typeInfo` (`0x803E9BC0`) is 4 bytes; dtk's guess was 8. The `.ctors` entry at `0x803529F8` is the unit's static initializer, which sets it to `&Pane::typeInfo`. The weak `ut::Color` destructor at `0x80187630` stays in this unit in the linked DOL.
- `Group` stores `mName[16]` at `0x18` and `mbUserAllocated` at `0x28`. The constructor copies 16 bytes and clears the byte at `0x28`. The imported header's 17-byte name and explicit padding were wrong. `Group::Init` and `Group::AppendPane` have no out-of-line copies, because both are inlined into the constructor.
- `lyt_common.h` gains `detail::DrawLine` and `detail::InitGXTexObjFromTPL` at namespace scope. It drops a misplaced `TexCoordAry::DrawLine` and the unused `TexCoordAry::SetCoord`. `EqualsResName` becomes `EqualsPaneName`, the target symbol name.
- `revolution/gx/GXTexture.h` gains `GXInitTexObjUserData`.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with all four units `Matching`. objdiff reports all 29 functions and every `.text` and `.rodata` section at 100%. `lyt_bounding.cpp` `.data` scores 37% and `lyt_common.cpp` `.bss` scores 50% in objdiff. The cause is weak RTTI records and the compiler-named local static `texCoords` and its guard. The linked DOL is byte-identical.

Outside names used for linking come from the `Bounding` vtable (`0x803BB630`) and its calls. These are the `nw4hbm::lyt::Pane` constructor (`0x8018C850`), destructor (`0x8018C970`), `GetVtxPos` (`0x8018D740`), and the virtual functions at `0x8018CBD0`–`0x8018D820`, in vtable order. They are listed in [symbols](../SYMBOLS.md#hbm-layout-common-units-294). `ut::detail::LinkListImpl::Erase(Iterator)` (`0x80193810`) is called from `GroupContainer`'s destructor.

## F-294-2: lyt_layout.cpp

`RVL_SDK/hbm/nw4hbm/lyt/lyt_layout.cpp` covers `.text` `0x80188940`–`0x801895DC` (15 functions), `.rodata` `0x80366E70`–`0x80366E78`, `.data` `0x803BB770`–`0x803BB7C0` and `.bss` `0x803E9BF0`–`0x803E9BF4`. Its first five functions, `SetTagProcessorImpl` to `CreateAnimTransform`, are in #294's block. The last ten, `BindAnimation` (`0x801890B0`) to `BuildPaneObj` (`0x801893E0`), are in #295's block. The source file is one unit, so this issue matches all of it. The names and sizes match xenoblade's symbols, and the code follows Kirby's Epic Yarn.

Evidence and header corrections:

- The `Layout` vtable (`0x803BB770`) has 11 slots, in this order: destructor, `Build`, one `CreateAnimTransform(const void*, ResourceAccessor*)`, `BindAnimation`, `UnbindAnimation`, `UnbindAllAnimation`, `SetAnimationEnable`, `CalculateMtx`, `Draw`, `Animate` and `SetTagProcessor`. The imported header also declared two more `CreateAnimTransform` overloads and `BindAnimationAuto`. Those belong to a later NW4R version, so they were removed.
- `Layout::mspAllocator` (`0x803E9BF0`) is 4 bytes; dtk guessed 8. The `.bss` split ends at the object, and the padding stays with the next unit.
- `SetTagProcessorImpl` is in an anonymous namespace. It inlines its own recursion twice, and `SetTagProcessor` inlines it again. It calls `GetRuntimeTypeInfo` through vtable slot `0xC` and compares the result with `TextBox::typeInfo` (`0x803E9C08`). It then stores the processor at `TextBox` offset `0xF4`.
- The new `include/nw4hbm/lyt/lyt_textBox.h` follows Kirby's Epic Yarn's `TextBox` layout, 0x100 bytes. The `TextBox` vtable (`0x803BB96C`) has 31 slots: `Pane`'s slots, with `GetRuntimeTypeInfo` at `0x8018F350` and the destructor at `0x8018E0C0`, then four `TextBox` virtuals at `0x8018EE60`–`0x8018EFA0`. The order of those four, `AllocStringBuffer`, `FreeStringBuffer` and two `SetString` overloads, comes from the reference header and is not checked here.
- `Pane::PaneList` is now public, so that `SetTagProcessorImpl` can name the child-list iterator. Access control does not change the generated code.
- `BuildPaneObj` constructs `Pane`, `Picture`, `TextBox`, `Window` and `Bounding` through local `CreateObject` templates. The two-argument templates take the `ResBlockSet` by value, which accounts for the copy onto the stack in each case.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with the unit `Matching`. objdiff reports all 15 functions, `.rodata`, `.data` and `.bss` at 100%.

### Traps

- **Wii/1.0a with `-ipa file` inlines a member function defined after its caller when the callee is small enough.** In the first version of `BuildPaneObj`, each case was `return CreateObject<T>(...)`. The compiler inlined it into `Build`, which grew to `0x4CC` against the original `0x304`. The size limit counts source statements before inlining, not emitted code: two extra dead `if` statements in `BuildPaneObj` were enough to stop the inlining. Storing each result in a local `Pane* pPane` before returning it gave the original size, with no change to `BuildPaneObj`'s code. `#pragma inline_max_size`, `inline_max_total_size` and `inline_depth` had no effect. `#pragma dont_inline` changes the whole file under `-ipa file`, because the pragma state at the end of the file applies.
