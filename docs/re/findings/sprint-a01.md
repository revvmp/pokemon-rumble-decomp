# Sprint a01: BModelObj and BWorld

## Traps

- **Duplicate template instantiations cannot share a name with a split object symbol.** Naming `fn_8005A990` and friends after the `shared_ptr_deleter<char, default_delete<char[]>>` members made the link fail with multiply-defined symbols against CDlgPiiList.o, whose compiled copy is weak. The original binary keeps one copy per unit, so leave those symbols under placeholder names until the owning unit is `Matching`.
- **`std::less<shared_ptr>` in a tree needs unsigned colour comparisons.** `tree_node_base::color()` returning the `__tree_color` enum compiled `cmpwi`; the target uses `cmplwi`. Returning `unsigned` and accepting `unsigned` in `set_color` made `balance_insert` 100 percent.
- **`__pointer_plus_bit` assignment from another `__pointer_plus_bit` copies the colour bit.** `y->parent_ = x->parent_` selects the implicit copy assignment. The rotations must assign `pointer(x->parent_)` to keep the destination colour.
- **Inline depth of a recursive template pair depends on instantiation order.** `KDTreeZXNode<2,9>::OnLinkRemoved` is 0x4F4 bytes and `<0,9>` is 0x254 in the target. Whichever `OnLinkRemoved` is instantiated first is inlined deeper, and the other node type's `RemoveChild` is emitted out of line (`<2,9>` in the target). Instantiating `<0,9>` first, as the insertion loop does, reverses the roles.
- **Arguments are evaluated right to left.** Passing two inlined `Quantize` calls as call arguments loads the second one first; keep the four cell computations as separate statements.
- **Named locals and expression temporaries live in different frame regions.** A `VEC3` difference compared against `VEC3(0, 0, 0)` without a named variable lands at the bottom of the frame, as in `CalcSize`.
- **BModelObj shares draw layer type with IWorldObjStat.** The reference symbol map called the draw layer set element `IDrawLayer`; renaming those five symbols to `IWorldObjStat` matched the set destructor and made `AddDrawLayer` 100 percent.
