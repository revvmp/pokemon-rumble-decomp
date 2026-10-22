# Sprint a12 findings (CPiiTex, PiiPropData, CPiiListFilterSort)

## Traps

- **A `#pragma dont_inline on` placed mid-file also applies to template instantiations.** CodeWarrior instantiates templates at the end of the translation unit, so the pragma state at end of file decides inlining of every template body. PiiPropData's constructor stayed 4% until `#pragma dont_inline reset` was added at the end of the file; the vector constructor then inlined as in the original (95.97%). Functions defined after the `on` still call their (now out-of-line) helpers, which is what the original Set/GetWazaLevels show.
- **Renaming a placeholder to a template name that already exists elsewhere breaks the link or the DOL.** `dtor_800F5FD0` is a second copy of `__dt__Q23std39__vec_deleter<Us,...>` (also at 0x8007B2A0); extab relocations in this unit point at the first copy. Naming both identically either failed with multiply-defined or, with `scope:weak`/`scope:local`, changed extab relocations and the DOL hash. Leave such duplicate copies with a placeholder name.
- **Placeholder-to-name pairing.** `unitdiff.py --json` lists relocation targets for matched functions. Aligning instruction mnemonics of a target function and its source counterpart and comparing `bl` relocation names gives the source name for each `fn_`/`dtor_` callee (scratch script, not committed). Apply the renames only when each placeholder maps to a single name.
- **Never grep unitdiff output for `rror`.** Mangled `length_error` matches; use `Error:` with a leading `#`.
- **`std::pair<s16, s16>` copies use per-field `lha`/`sth`; the original copies the pair as one word.** A plain struct of two `s16` (`LevelMove`) reproduces the word copy.
- **CW folds `x >= 1 && x <= 100` into an unsigned range check; the original did not.** GetWazaLevels keeps a two-compare form in the original; no source spelling found yet that avoids the fold (all four orderings tried).
- **Vector iterator loops that cache the end:** `for (it = begin(), end = end(); ...)` and `it != end()` produce different spills. UpdateCamera needed `it != mPiis.end()` re-evaluated each iteration, while HasPrefixedItem needed a raw-pointer pair declared as `first`, `end = first + size()`.
- **Class-template helper names pair by call relocations only for callers that exist in your object.** Extab-only and vtable-only functions (tuple and `bound_func` destructors, `function_imp` virtuals) have no `bl`; pair them by size and address order against your object's same-class functions.
- **A bound `function` argument that the target passes as `D` by value needs the `bind` parameter to be `D`, not `const D&`.** The copy then becomes an argument temporary created after the member-function-pointer temporary, which fixes stack slot order (ShowTypeFilterMenu, ShowPrefixFilterMenu).
- **`bound_func::operator()(A&)` must build `tuple<A&>` and resolve the placeholder through `resolve_bound_argument`.** Passing the argument straight through omits the target's `stw r4, 8(r1)` spill (function_imp operator() went from 77% to 100%).
- **`x >= 0 && x < N` on a signed `int` folds to an unsigned compare; the nested form `if (x >= 0) { if (x < N) {...} }` keeps both compares.** Used for ShowPrefixFilterMenu.
- **A dynamic_cast target is found from the RTTI symbol in the vtable data, not from the member being called.** PreviewTypeFilter calls CUIList methods but casts to CUIMenu (`__RTTI__7CUIMenu`).
- **Menu sub-functions that are in the `dont_inline on` state call `__dt__` and `HasItemWithPrefix`; the others inline shared_ptr releases.** Toggle `#pragma dont_inline on` and `reset` per function group in address order.
- **`fn_800E0840` in CPii is `std::swap<std::vector<SD_PiiPersonalData::Data>>`, not `operator=`.** `std::swap(rOut, records)` reproduces ExportPersonalData at 100%.
- **Parallel agents share the scratchpad directory.** Another agent overwrote a helper script named `fd.py`; keep personal tools in a per-agent subdirectory.
- **Tried and rejected:** a user-declared `vector::iterator` copy constructor (Filter 100% to 86%), copying `sort` parameters into locals (sort 91% to 80%), removing the `is_POD<UIMenuItem>` specialization (CPauseMenu push_back 65% to 100% but Run 98% to 87%).
# Sprint a12 findings

## Traps

- **Inlined and out-of-line copies of one helper can coexist.** CStatistic::SetWazaUsed inlines the file-local ExpandTo (unrolled zero fill visible) while Set calls it; write the same helper and let the inliner decide. Use `oldSize <= index` to get the target compare order.
- **CStatistic::SetWazaUsed clears from the wrong vector.** The target stores `mValues[word] & ~mask` into mWazaBits when clearing; reproduce it as written.
- **CStatistic::Add's saturating negative path is a std::max against INT_MAX**, with the limit as an addressable local.
- **COyaInfo region 0x800ED1B0-0x800F2580** is a 20 KB block of inlined string/tree containers with EH cleanup; no source exists and it is not split. Not attempted.
- **CPiiCollectionBox::Create** needs an inline constructor `mEntries.resize(496)`; vector operator= (Save/RestoreSnapshot, CStatistic vector assign) is about 90 to 99 percent because of register allocation in include/vector.
