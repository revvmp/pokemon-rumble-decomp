# ScriptWorldObj final helpers

## F-251-2: comparator construction and duplicate type predicate

The last three function gaps were the 48-byte `tree_begin_holder<less<int>>` constructor, its unnamed four-byte callee at `0x8014E130`, and the second 68-byte `IsKindOf<CPii, BWorldObj>` body. The complete source object initially matched 237/240 functions.

The four-byte callee is the empty `std::less<int>` default constructor. A unit-local specialization of `less<int>` supplies that constructor and preserves the original integer comparison operator. Defining it under a scoped `dont_inline on` pragma retains the call in the tree holder, matching both bodies. This avoids the regressions in other units documented for a project-wide comparator change in [sprint-a03](sprint-a03.md) and [sprint-a05](sprint-a05.md). The target symbol at `0x8014E130` is now `__ct__Q23std7less<i>Fv`.

The target contains a local predicate at `0x8014CF80` called by `DynamicCast<CPii>`, and another predicate at `0x8014D8F0` called by the direct script type query. The source previously emitted only one. A local `IsPiiForDynamicCast` predicate and a unit-local inline specialization of `DynamicCast<CPii, BWorldObj>` retain the first independent body. The helper tests exactly the same null condition and type bit as the template; the generic direct-query body remains unchanged. The earlier local target symbol is named for this helper, retaining local scope. The second symbol retains its template name.

Both changes are D-0016 fake matches. The compiler and source-organization differences that originally created the two predicate copies are not reconstructed. The comparator specialization is likewise a local code-generation accommodation, not a claim about the original standard-library header.

Direct Wii/1.1 compilation with the unit's generated flags and diagnostic symbol-table renames gives 240/240 functions at 100%. No shared header changed. The first coordinated Matching trial linked successfully but failed full SHA verification; the unit remains NonMatching pending function/data emission ordering. The 240 matching functions form a 224-function common ordered subsequence with the reference, so individual function identity alone is insufficient.

## Traps

A constructor-specific `__declspec(noinline)` was rejected by this compiler. A scoped `dont_inline on` around the local specialization retains the empty constructor and its call without changing shared headers. The duplicate type predicate must have independent source identity; changing static linkage on the existing template alone cannot create both copies.
