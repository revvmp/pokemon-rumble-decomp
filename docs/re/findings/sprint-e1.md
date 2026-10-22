# Sprint e1: emission order of weak functions and data

All rules below were measured with the game flags (`Wii/1.1`, `-O4,p -inline auto -ipa file -str reuse`) in sandbox objects and confirmed on real units. Flags (`-ipa off/function/program`, `-inline deferred`, `-str pool/nopool`, `defer_codegen`, `inline_bottom_up`) never reproduced a target order; every fix was in source or headers.

## Landed

- `ScriptMessages` is Matching. `include/memory` defines `shared_ptr(Y*, D)` out of class, so `shared_ptr_deleter<char, default_delete<char[]> >` is instantiated after parsing (from `__error_string`) and its vtable lands after the string pool, as in the target.
- `CNumericInput` is Matching, with four header changes and one source fix:
  - `include/memory` includes `<exception>` before `shared_ptr.h`.
  - `include/string` declares `resize` and `reserve` before the first `replace` overload, and `resize(n, c)` is `inline`.
  - `include/functional` defines the `function<R()>` constructor template and destructor out of class.
  - `CNumericInput.cpp` includes `<memory>`, `CFiber.h` and `<string>` before `INumericInput.h`, defines `Initialize` as an `inline` member after `Run`, and pairs `"T_BT_confirm"` with message 393457 (the earlier source had the two pane names swapped; objdiff still showed 100% because it compares relocations by content).
- `CDisconMsg` is Matching. `include/functional` declares the primary `function` template just before `function<R()>` instead of at the top, so the `function_base` group follows the `function` group in each round and `~function_base`, requested while `~function` is instantiated, joins round 1 (rule 4a). The class declares its members in the rule 6 order with an implicit destructor, `Calc`, `CanShow`, `Run`, `AdjustSuppressCount` and `IsActive` are `inline`, and the message fallback is `gEmptyText` (an `L""` literal added a 4-byte `.sdata` object). `lbl_804B5E08` is `__RTTI__10IDisconMsg`.
- Each change was gated: an order sweep over all 798 units changed no Matching unit, and the 100%-function count in `report.json` did not drop.

## Rules

1. **In-class and `inline` template members are instantiated when first used during parsing. Out-of-class non-`inline` members are instantiated after parsing.** This decides which round (below) a function falls into, and when the classes its body names are instantiated.
2. **Vtables and RTTI of classes created while parsing are emitted in reverse creation order, ahead of the classes created later.** For a non-template class, creation is its definition, so include order changes `.data`: `CNumericInput` needed `exception`, `shared_ptr_deleter_common`, then `logic_error`/`length_error`/`out_of_range`, then `ICalc`/`ICalcDraw`. Classes instantiated after parsing come later, in creation order.
3. **String literals of non-inline functions are pooled when the function is generated, and a shared literal (`-str reuse`) takes its place from the first generated user.** Literals of inline functions become `@STRING@<function>@n` objects. These groups come after the vtables, in reverse definition order of the inline functions, and each group is in reverse creation order. If two strong-pool literals are swapped, check which template that uses them is generated first.
4. **Deferred out-of-class template functions are grouped by class template, last-defined template first.** Within a template, the newest specialization comes first. Within a specialization, members follow the class declaration order. An overload set sits where its name is first declared: the first `replace` declaration fixed `basic_string`'s order, not the 4-argument one. Functions requested while a group is being instantiated join their group if it has not yet been emitted.
4a. **The group position of a class template is the position of its first declaration, including a forward declaration of the primary template.** A partial specialization belongs to its primary template's group. Moving a partial specialization's definition does not move its group; moving the primary's forward declaration does.
5. **A request made while another template is instantiated goes to a later round.** Each round repeats the group order. `CDisconMsg` shows `~function`, the deleter, `shared_ptr` and `move_ptr` in round 1, then `function_imp`, `function_base` and the CLayout/CFiber deleters in round 2.
6. **Inline functions referenced only from a vtable (in-class, or out-of-class with `inline`) are emitted after all strong functions, in reverse class declaration order.** Each one is followed directly by the inline functions it calls, depth first. A virtual call through `this` (`CanShow()` in `Calc`) counts as a call. `CDisconMsg` needs `inline` on `Calc`, `CanShow`, `Run`, `AdjustSuppressCount` and `IsActive`, with the class declaring `IsActive, AdjustSuppressCount, CanShow, Run, Calc, CalcMtx, CalcView, Draw`. This also places its `.data` literals after the `shared_ptr_deleter<CDisconMsg>` vtable, as in the target.
7. **A user-declared destructor is a strong function at its definition. An implicit one is generated late, with the vtable functions.**
8. **`inline` on an out-of-class template member can change code:** it got `find_leaf` inlined into `InsertString`. Check instruction matches after every `inline` change.

## Method

- `scratchpad/e1/var.py <unit> [--src file] [--flags ...] .text .data .sdata` compiles with the unit's exact flags and diffs symbol order against `build/wpse-v1/obj/<unit>.o`, keyed by name or by content for anonymous data. `sweep.py <regex> out.json [base.json]` runs it over every unit and lists the units whose order changed. Use it to gate header edits.
- For data that dtk keeps as one `lbl_` blob, `blob.py <obj> .data` prints string and relocation offsets for a side-by-side diff.
- **Find where a class or member is instantiated** by adding `typedef typename T::probe_x probe;` to its body and compiling. The error names the instantiation point: `global scope` means after parsing, a function name means during parsing.
- Flip test: set the unit to Matching, run ninja (a sha1 CHECK failure still produces the DOL), then run `c1_doldiff.py` for the differing byte runs and `c1_elfcmp.py lo hi` for the first misplaced symbol.

## Open

- **CBattleRoyalDisplay:** an implicit destructor fixes `~CBattleRoyalDisplay`'s position, and `inline` on `Calc` places the deleter RTTI in `.sdata`. Still open: the `std::max` zero constant must come first in `.sdata`, and the wide `FormatString` must be emitted after `move_ptr`. `gEmptyText` can go to `.sdata` with `#pragma explicit_zero_data on` around `wchar_t gEmptyText[1] = L"";` (untested on the link).
- **Duplicate weak copies (ScriptPii, CMsgBalloon):** the original keeps a second copy of some weak templates (`max<int>` at 0x800C5C10, `DynamicCast`/`IsKindOf` in ScriptPii), so the earlier unit's copy did not win there. Untested hypothesis: give the earlier copy a local placeholder name in `symbols.txt` and the later copy the plain global name, so each object resolves to its own.
- **`find_leaf` in tree units (MapPiiTable, CLayoutManager, l10n_files):** the target emits `insert_node_at` and `destroy` for each tree, then `allocator::destroy` and the `alloc_ptr` destructors, and only then `find_leaf` for each tree. Ours emits `find_leaf` inside the tree group. Moving its declaration after `destroy` did not move it, because the `find_leaf<Key>` overload keeps the name's earlier position. `inline` changes the code.

## Traps

- Copying a header back from a scratch file after landing (the land script rebases) silently reverts other agents' newer edits to that header. Regenerate scratch variants from `git show HEAD:<path>` after every rebase.
- `git stash` is shared across worktrees (sprint-a14); keep baselines as files or commits instead.
