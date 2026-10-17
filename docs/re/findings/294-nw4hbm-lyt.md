# Issue 294: HBM nw4hbm::lyt, block 2

## F-294-1: lyt_bounding, lyt_common, lyt_drawInfo and lyt_group

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). #293 matched this block's first five functions (`0x80187300`–`0x80187520`), which end `lyt_arcResourceAccessor.cpp`; see [F-293-1](293-nw4hbm-lyt.md#f-293-1-lyt_animationcpp-and-lyt_arcresourceaccessorcpp). Four more units follow:

| File | `.text` | Other sections |
| --- | --- | --- |
| `RVL_SDK/hbm/nw4hbm/lyt/lyt_bounding.cpp` | `0x80187520`–`0x80187694` (6 functions) | `.ctors` `0x803529F8`–`0x803529FC`, `.data` `0x803BB630`–`0x803BB718`, `.bss` `0x803E9BC0`–`0x803E9BC4` |
| `RVL_SDK/hbm/nw4hbm/lyt/lyt_common.cpp` | `0x801876A0`–`0x80188530` (16 functions) | `.rodata` `0x80366E58`–`0x80366E68`, `.bss` `0x803E9BC8`–`0x803E9BF0` |
| `RVL_SDK/hbm/nw4hbm/lyt/lyt_drawInfo.cpp` | `0x80188530`–`0x801885F0` (2 functions) | `.rodata` `0x80366E68`–`0x80366E70`, `.data` `0x803BB718`–`0x803BB748` |
| `RVL_SDK/hbm/nw4hbm/lyt/lyt_group.cpp` | `0x801885F0`–`0x8018893C` (5 functions) | `.data` `0x803BB748`–`0x803BB770` |

The function lists, sizes and order are identical to CC0 xenoblade's `config/jp/symbols.txt`, which gives the names. The code follows CC0 Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, `src/hbm/nw4hbm/lyt`). `0x80188940` starts `lyt_layout.cpp` (`SetTagProcessorImpl`), which runs into #295's block.

Evidence and header corrections:

- `Bounding::typeInfo` (`0x803E9BC0`) is 4 bytes; dtk's guess was 8. The `.ctors` entry at `0x803529F8` is the unit's static initializer, which sets it to `&Pane::typeInfo`. The weak `ut::Color` destructor at `0x80187630` stays in this unit in the linked DOL.
- `Group` stores `mName[16]` at `0x18` and `mbUserAllocated` at `0x28`. The constructor copies 16 bytes and clears the byte at `0x28`. The imported header's 17-byte name and explicit padding were wrong. `Group::Init` and `Group::AppendPane` have no out-of-line copies, because both are inlined into the constructor.
- `lyt_common.h` gains `detail::DrawLine` and `detail::InitGXTexObjFromTPL` at namespace scope. It drops a misplaced `TexCoordAry::DrawLine` and the unused `TexCoordAry::SetCoord`. `EqualsResName` becomes `EqualsPaneName`, the target symbol name.
- `revolution/gx/GXTexture.h` gains `GXInitTexObjUserData`.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with all four units `Matching`. objdiff reports all 29 functions and every `.text` and `.rodata` section at 100%. `lyt_bounding.cpp` `.data` scores 37% and `lyt_common.cpp` `.bss` scores 50% in objdiff. The cause is weak RTTI records and the compiler-named local static `texCoords` and its guard. The linked DOL is byte-identical.

Outside names used for linking come from the `Bounding` vtable (`0x803BB630`) and its calls. These are the `nw4hbm::lyt::Pane` constructor (`0x8018C850`), destructor (`0x8018C970`), `GetVtxPos` (`0x8018D740`), and the virtual functions at `0x8018CBD0`–`0x8018D820`, in vtable order. They are listed in [symbols](../SYMBOLS.md#hbm-layout-common-units-294). `ut::detail::LinkListImpl::Erase(Iterator)` (`0x80193810`) is called from `GroupContainer`'s destructor.
