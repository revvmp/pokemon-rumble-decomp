# Sprint c1: closing attempts

## Landed

- `tree_imp::erase(iterator)` now carries the erase body itself instead of forwarding to `erase(const_iterator(position))`. The forwarding form needs two iterator stack slots; the target uses one. CLayoutManager `Remove` reaches 100% and no other function regressed (11753 to 11754 functions at 100%).
- `__RTTI__Q33std6detail32default_delete<14CLayoutManager>` is named (0x804B6A48), which fixes `get_deleter` of the CLayoutManager deleter.
- The `.sdata` boundary between CLayoutManager and CMiniMap moved to 0x804B6A50: 0x804B6A50..0x804B6A68 are the CMiniMap, IMiniMap and `shared_ptr_deleter<CMiniMap>` RTTI objects, referenced by CMiniMap's original object. Flipping CLayoutManager with the old boundary fails to link with undefined `lbl_804B6A50`.
- MapPiiTable: `GetIndex` is defined after `GetCell` so the strong function order matches (GetValue, GetCell, GetIndex).

## Traps

- Objdiff percentages ignore data names. A unit whose functions differ only in `lbl_` versus `@nnnn` relocation names is already at 100% functionally; the DOL link decides.
- A flipped unit whose DOL grows by 0x20 or 0x100 bytes has extra extab or text; compare `nm -n -S build/wpse-v1/main.elf` against `symbols.txt` over the unit range to find the first misplaced symbol (the extab symbols `@etb_ADDR` name their original address, so the first one whose address differs marks the shift).
- Weak template emission order is not header definition order and not first-reference order. MapPiiTable: target emits `insert_node_at`, `destroy`, allocator destroy, `alloc_ptr` destructors, then `find_leaf` last; ours emits `find_leaf` first. Moving the `find_leaf` definition or declaration inside `tree.h`, or marking it `inline`, did not change it (`inline` broke the link). CLayoutManager shows the same: allocator destroy, pair destructor and `find_leaf<i>` come after `destroy` in the target, but ours places them earlier. Open.
- ScriptMessages: the whole unit is 100% except `.data`; the `shared_ptr_deleter<char[]>` name and RTTI are emitted before `std::exception` in ours and after the `basic_string` message literals in the target. Same RTTI-order class as CDisconMsg and CColorConst. Open.
- CTextWorldObjStat `CalcView`: the target keeps `-1.0f` in f2 and `position.x` in f3; every spelling tried (local copies, reordered comparisons, `*=`, merged `* 0.5f`) kept f3/f2 swapped or got worse. Open.
