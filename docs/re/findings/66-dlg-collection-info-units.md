# F-66-1: CDlgCollection.cpp and CDlgInfo.cpp unit boundaries

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issues #66 to #72 list 189 functions between `0x800776B0` and `0x80084DDC` under "CDlgCollection, CDlgInfo". They form two translation units, both recovered from RTTI names that embed the source file: `@unnamed@CDlgCollection_cpp@` and `@unnamed@CDlgInfo_cpp@`.

## Section ranges

| Unit | `extab` | `extabindex` | `.text` | `.data` | `.sdata` | `.sdata2` |
| --- | --- | --- | --- | --- | --- | --- |
| `game/CDlgCollection.cpp` | `0x8000CD9C–0x8000E5D4` | `0x800455AC–0x80045A2C` (96 entries) | `0x800776B0–0x8007E5C4` | `0x8039BFE8–0x8039D060` | `0x804B5F30–0x804B60B0` | none |
| `game/CDlgInfo.cpp` | `0x8000E5D4–0x8000FBC8` | `0x80045A2C–0x80045D68` (69 entries) | `0x8007E5D0–0x80084D34` | `0x8039D060–0x8039DB40` | `0x804B60B0–0x804B61A8` | `0x804B9DC8–0x804B9DD0` |

Neither unit has `.ctors`, `.bss`, `.sbss` or `.rodata` objects. Both are split and registered `NonMatching` in the `Game` library; the full DOL still verifies.

## Boundary evidence

- **CDlgCollection start.** `0x800776B0` allocates a 20-byte object whose vtable is `0x8039C220`, the `CDlgCollection` vtable, and returns it as a shared pointer with `enable_shared_from_this` hookup. Its only caller is `0x800A8AF4`. The preceding unit's last `extabindex` entry (`0x80077660`, `extab` `0x8000CD94`) and its last `.sdata` type-info object (`default_delete<CPpuEngine>` at `0x804B5F28`) end the previous unit.
- **CDlgCollection end and CDlgInfo start.** `0x8007E5D0` is the matching `CDlgInfo` factory, called only from `0x800A8B18`, the call after the `CDlgCollection` factory. The two weak destructors `0x8007E430` and `0x8007E500` before it are referenced only by `CDlgCollection` unwind entries, and their `extabindex` entries precede `0x8007E5D0`'s. `.text` ends at the last function end, `0x8007E5C4`.
- **Data split.** `CDlgCollection` data starts with its own `shared_ptr_deleter` vtable at `0x8039BFE8` and ends with the `bad_weak_ptr` `what()` string at `0x8039D050`. `CDlgInfo` data starts at `0x8039D060` with a four-entry string table pointing at `CAPTION`, `COLON`, `VALUE` and `UNIT` (`0x804B60B0`–`0x804B60C8`), which also begin its `.sdata`.
- **CDlgInfo end.** `0x80084D40` allocates 288 bytes and constructs an object with vtable `0x8039DBC0` (`CDlgMsgWin`), so it starts `CDlgMsgWin.cpp`, which is not one of these units even though #72's table lists it. `CDlgInfo` `.text` ends at `0x80084D34`, `.data` at `0x8039DB40` (the `CDlgMsgWin` deleter vtable), and `.sdata` at `0x804B61A8` (the `CDlgMsgWin` deleter type info).

## Class inventory

Type-info base lists (name pointer, then base/offset pairs) give these hierarchies:

| Class | Bases |
| --- | --- |
| `IDlg` | `ICalcDraw`, `ICalc` |
| `IDlgCollection`, `IDlgInfo` | `IDlg` |
| `CDlgCollection` (anonymous namespace) | `IDlgCollection` at 0, `std::tr1::enable_shared_from_this<CDlgCollection>` at 4 |
| `CDlgInfo` (anonymous namespace) | `IDlgInfo` at 0, `std::tr1::enable_shared_from_this<CDlgInfo>` at 4 |
| `CDlgCollectionItem`, `CDlgInfoItem` | `IUIListItem` |
| `CDlgCollectionItemModel`, `CDlgInfoItemLine` | none (polymorphic roots) |

Non-polymorphic classes named only in deleter type names: `CDlgCollectionImpl`, `CDlgCollectionItemModel::Model`, `CDlgInfoImpl`, `Page`. The units bind `CDlgCollectionImpl` and `CDlgInfoImpl` member functions into `std::tr1::function<void ()>` and `std::tr1::function<void (BUI*)>` and hold `shared_ptr`s to `CUIManager`, `BUILayout`, `CUIList`, `CPiiTex` and `function<void (BUI*)>`.

Vtables (after type info and offset):

| Address | Class | Slots |
| --- | --- | --- |
| `0x8039C220` | `CDlgCollection` | `0x8007CDA0` destructor, `0x80077AC0` `Calc`, `0x80077A00` `PrepareDraw`, `0x80077940` `DrawBefore`, `0x80077870` `Draw`, `0x80077860` (shared `IDlg` inline), `0x8007AFE0`, `0x8007AFC0`, `0x80078490` |
| `0x8039C3FC` | `IDlg` (weak) | `0x80077820` destructor, pure `Calc`, `0x80063DC0`, `0x80063DD0`, pure `Draw`, `0x80077860` |
| `0x8039C2F8` | `CDlgCollectionItem` | `0x8007CCC0`, `0x8007B280`, `0x8007B0C0` |
| `0x8039C34C` | `CDlgCollectionItemModel` | `0x8007B0E0` |
| `0x8039C3C8` | `std::tr1::bad_weak_ptr` (weak, also used by `CDlgInfo`) | `0x8007CC80`, `0x8007B290` |
| `0x8039D1F0` | `CDlgInfo` | `0x80083050` destructor, `0x8007F7F0` `Calc`, `0x80063DC0`, `0x80063DD0`, `0x8007F750` `Draw`, `0x80077860`, `0x800813D0`, `0x800813B0`, `0x8007FA30`, `0x8007F650`, `0x8007F570`, `0x8007F290`, `0x8007E810` |
| `0x8039D2C0` | `CDlgInfoItem` | `0x80082ED0`, `0x80081B00`, `0x800814B0` |
| `0x8039D308` | `CDlgInfoItemLine` | `0x800814D0` |

The remaining vtables are `shared_ptr_deleter` instantiations (destructor, `get_deleter`, `dispose`) for every owned type above and two `function_imp` instantiations per unit. Slot names `Calc`, `PrepareDraw`, `DrawBefore` and `Draw` follow the interfaces in PR #906 (F-53-4).

## Source status

No source is committed. A matching unit needs byte-exact `std::tr1` `shared_ptr`, `weak_ptr`, `enable_shared_from_this`, `function`, `bind`/`mem_fn`, `std::vector` and tree-based container instantiations (`CDlgCollection.cpp` holds the `tree::insert length error` string at `0x8039C434`), plus the `BUI`, `BUILayout`, `CUIList`, `CUIManager` and `IUIListItem` interfaces, because each unit emits its own weak copies of those templates. None of these exists in the repository; the BUI and container work is in progress under #327 and #53 (PRs #890, #906, #919). The standard-library code is inlined into game functions, not only emitted as weak templates. The `CDlgCollectionItem` constructor at `0x8007A5C0` copies a wide-character range into a vector and, on overflow, builds `std::length_error("vector length error")` inline: `strlen`, an array allocation, a `shared_ptr_deleter<char[]>` control block (vtable `0x803998B4`), `strcpy` and `__throw`. Matching even one game function therefore needs MSL-exact `stdexcept` and `vector` headers. No public MSL C++ implementation is available to the project, and the independent list and vector reconstructions in F-327-3 do not yet match.

The weak `weak_ptr` destructor at `0x8007DAD0` that BUI's constructor reuses (F-327-1) lies in `CDlgCollection.cpp`.

## Traps

- **An issue table can include the next file's first function.** #72 lists `0x80084D40`, but that function constructs `CDlgMsgWin` and begins `CDlgMsgWin.cpp`. Check what each boundary function constructs before ending a split at the issue's last address.
- **Shared weak functions sit in the first unit that emits them.** `0x80077860` (an `IDlg` inline virtual) and the `bad_weak_ptr` vtable are referenced by later dialog units but live in `CDlgCollection.cpp`. Do not assign them to the unit that references them most.
