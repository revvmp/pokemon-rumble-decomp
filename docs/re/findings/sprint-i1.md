# Sprint i1: the Wii/1.1 `-ipa file` inliner budget

Measured with `mwcceppc` Wii/1.1 and the exact `cflags_game` flags on scratch translation units, then confirmed on the real units. The scripts used are throwaway; the results below are reproducible with any repeated inline helper.

## F-i1-1: the rule

Under `-ipa file`, inlining has three parts.

1. **Cost.** Each inline candidate costs its fully flattened body: its own statements plus every callee that it keeps inlined, recursively. A callee with one or two statements costs about 3 units per statement; a larger one costs about 15 units per statement. A call that stays a call costs about 10 units. The caller's own statements are not counted.
2. **Nested levels prune.** When an inline function `G` is being expanded (at any depth below the function being compiled) and the total cost of its children exceeds the budget `L` (about 10000 units, around 222 copies of a three-statement helper), `G` keeps only its *small* children and calls the rest. "Small" is an absolute limit of about 13 to 14 flattened statements: four three-statement helpers are kept, five are called. The limit does not depend on the total or on the order of the calls. `G`'s cost seen by its own caller is then its own statements plus the kept children plus about 10 per call left in place.
3. **The function being compiled is all-or-nothing.** If the total cost of its direct inline callees exceeds `L`, it inlines nothing, not even one-line accessors. A direct callee that is over budget by itself (for example an in-class `Load` with hundreds of statements) is enough. The same callee one level deeper is only pruned (rule 2) and does not poison the caller.

Non-inline functions (defined out of class without `inline`, including out-of-class template members such as `shared_ptr::reset<Y>`) are auto-inlined only when small (about 14 statements or less); larger ones are ordinary calls and cost nothing toward the budget.

What this rules out: `#pragma inline_max_size`, `inline_max_total_size`, `inline_depth`, `inline_bottom_up`, `-inline level=n`, `-inline all|smart|deferred` and the optimisation level do not change any threshold. Definition order and template versus non-template do not matter. Without `-ipa file` (`-ipa function` or `off`) there is no budget at all. Wii/1.0, 1.1, 1.3 and 1.5 behave the same.

## F-i1-2: how to recognise it in a target

A target function that inlines some heavy helpers (member `shared_ptr(new T)` initializers, `reset`, `vector::push_back`, `function` constructors) but calls others of the same kind, while still inlining every small accessor, has an inline helper between it and the called ones. The called helpers sit in the over-budget level, the inlined ones above it. A function that calls even trivial accessors is a rule-3 poison: some direct inline callee is too large.

## F-i1-3: recipe

Move the heavy part of the body into an inline member of the implementation class and call it from the original function. The helper's own level overflows, its large children become calls, and the caller stays under the budget. In-class helpers need no `noinline` or `#pragma auto_inline off`; those hacks can be removed.

| Function | Before | After | Source shape |
| --- | --- | --- | --- |
| `CDlgInfo::Show` | 21.30% | 96.56% | `mImpl->Init(mPages)`; `Init` in `CDlgInfoImpl` holds the layout loading, item loop and fiber start |
| `CDlgCollection::Show` | 31.43% | 94.01% | `pImpl->Init()` in `CDlgCollectionImpl` |
| `CDlgPiiList::Show` | 69.05% | 99.95% | the `CDlgPiiListImpl` constructor is `inline` and its body does the clear, `AppendItems`, `Setup` and fiber start; `AppendItems` and `Setup` are `inline` (they are pruned, which is why the target calls them) |
| `CDlgPiiListImpl::Run` | 24.74% | 94.74% | `Run` keeps `CheckRemoteMemory` (inlined with its `ShowMessage` calls) and calls an inline `RunList` holding the input loop; switch case 1 is written last, matching the target block order |
| `IGameResult::Create` | 68.82% | 98.43% | the constructor keeps the member initializers and `mCapture.Init`; `Create` calls an inline `Load(background)` holding the rest, and `CollectPiis`, `SetPiiPositions` and `UpdatePiiWindow` are `inline` so that `Load` overflows and prunes |

The `IGameResult::Create` case shows the counter-intuitive direction: the body had to become larger (three more inline candidates) so that its level exceeded the budget and pruned itself down to what the constructor level could inline.

## Remaining differences

- `CDlgInfo::Show`, `CDlgCollection::Show`: stack slot assignment of the temporaries only.
- `CDlgPiiListImpl::Run`: the target inlines the `bind(&OnSortChanged, this, _1)` call that ours prunes, and its first inlined `ShowMessage` in `CheckRemoteMemory` has one more `YieldFiber` call.
- `IGameResult::Create`: the inlined `G3dObj::DynamicCast<ScnMdl>` branch layout.

## F-i1-4: second shift, applying the rule

A scan of `report.json` plus objdiff call lists (functions below 100% whose `bl` targets differ between target and source, ranked by size times gap) found more real code gaps than inliner cases. Results:

| Function | Before | After | Cause and fix |
| --- | --- | --- | --- |
| `pair<const string, shared_ptr<T>>` tree `destroy` (main, CPpuEnv), `__ct<map>` in main, `GetFontMap` (errmsg) | 0 to 85% | 100% | Pruned levels keep only children of about 14 flattened statements or fewer. The target calls `allocator::destroy` for this pair in pruned levels, ours kept it inline: our `~basic_string` was two statements short. Freeing through `long_data()` and `long_cap()` fixes all four with no regressions. |
| POD `vector::push_back` (CPauseMenu `UIMenuItem`, PiiPropData `short`, CDlgCollection `unsigned short`) and `CPpuEngine::LoadPlacement` | 65 to 81% | 94 to 99% | Not inlining: `push_back_pod` took its capacity from the boolean `grow_by(1)`, so the reallocation constant-folded to `new(0)`. It now uses `recommend(size + 1 - cap)`. |
| `CFileData` `UpdateResourceFiles` | 83% | 99.9% | Missing code: an idle worker (`mCount` and `mYieldDepth` zero) is released and expired cache entries are purged by an inline shared with `ShutdownResourceFiles`. |
| `CModel::TPartialAnm<0>::Update` | 79% | 96% | Missing code: a finished non-main animation pops its motion and resets its animation object. |
| `CModel::Load` | 90% | 97% | Missing `LoadBounds(&mBounds, GetResFile().GetResMdl(0))`. |
| `CDlgPiiListImpl::Run` | 94.7% | 94.9% | The end motion and type switch follow the input loop, which `case 1` leaves with `return`, matching the target block order. |

### Measured gaps that are still open

- `main`: the target calls `ResourceFileConfig::SetErrorHandler`, ours auto-inlines it. Adding statements to its body shows the target body costs about 10 to 15 statements more than ours; `mErrorHandler = rHandler`, an unrolled `swap`, and moving the definition change nothing. The extra cost is probably in the original `function<R(A,B,C,D)>` copy, swap or destructor.
- `SaveData` (a16): `append<SlotData>` in the target is a rule-3 poison (it calls even `cap()`), because `construct_at_end` and `append_realloc` stay expensive. They stay expensive because the `SlotData` copy level overflows in the target, which turns `SlotData::Data`'s implicit copy and the seven vector copy constructors into calls. Ours is just under the budget there: adding about five statements to the `__vec_constructor` copy constructor (in an `if` that never runs) makes the copy level overflow and reproduces the target calls. Dead locals or extra accessor layers in the copy constructor do not count. The real missing cost is still unidentified.
- `CDlgPiiListImpl::Run`: the target keeps `bind(&OnSortChanged, this, _1)` inline in the pruned `RunList` level, so the original `bind` flattens to 14 statements or fewer. Ours is just over the limit. Building `mem_fn_1` directly instead of through `mem_fn` does not change it.

## Traps

- **Library helpers can be a few statements off the original.** When a pruned level keeps a helper inline that the target calls (or the reverse), test the threshold by adding a never-taken `if (global == 12345) { ... }` with N statements to the helper and rebuilding the whole project. A dead local or `x = x` is folded away before costing; the `if` is not. If the improvements fall only on one container or value type, the gap is in that type's helpers, as with `~basic_string` here. Then find a form with the same code and more statements, such as calling accessors instead of reaching fields directly, and confirm with a full report diff that nothing regresses.
- **Check for constant folding before blaming the inliner.** A reallocation path with `new(0)` and a missing throw meant the source computed its capacity from a `bool`.
- **Deleting a statement can change inlining everywhere in a function.** Under `-ipa file` the function being compiled is all-or-nothing against a per-function budget of flattened inline code (F-i1-1). Do not chase single refusals from `warn_notinlined`; find the level structure instead (F-i1-2).
- **Pruned callees look like non-inline functions.** A helper the target calls out of line may still be an inline function that a nested over-budget level pruned. Check whether the same helper is inlined elsewhere in the same target function before declaring it non-inline.

## F-i1-5: third shift

| Function or unit | Before | After | Cause and fix |
| --- | --- | --- | --- |
| `KDTreeZXNode<2,9>::OnLinkRemoved`, `<0,9>::OnLinkRemoved`, `RemoveChild<2,9>`, `shared_ptr<KDTreeZXNode<2,9>>::reset` (BModelObj) | 0 to 46% | 100% | The recursive `RemoveChild` pair breaks its inline cycle at whichever specialization is instantiated first, and the newest specialization is emitted first (sprint-e1 rule 4). `CKDTree` now names `typedef KDTreeZXNode<0, DEPTH> ZNode;` before `XNode`, so the Z node exists first, `<2,9>` code is emitted first and `RemoveChild<2,9>` is the out-of-line copy. BModelObj 79/88 to 83/88. |
| PiiPropData `0x800F5FD0`, `0x800F6040` | 0%, 82% | 100% | Mislabelled: the 112-byte function is `~__vec_deleter<LevelMove>`, the 116-byte one is `~__vec_split_buffer<LevelMove>` (the `push_back` extab destroys a split-buffer local there). |
| `PiiPropData::GetWazaLevels` | 91.5% | 100% | `1 <= level && level <= 100` and `-5 <= level && level < 0` keep two signed compares; `level >= 1 && level <= 100` is folded into one unsigned range compare. |

SaveData: the vector-copy threshold moved from five to nine dead statements since shift 2 (other header changes in between). Nine statements in `allocate(n, pod_type)` flip it the same way as nine in the copy constructor, so the gap is additive anywhere in the copy chain. Tried without effect: `base(x.alloc())`, an `init` helper, an explicit allocate-and-loop body, a user copy constructor on `CMapRecord`, dissolving the `mStatus` union, scalar `mFlags`/`mLastArea` fields, and two-statement probes in each throw-path helper (`__error_string`, `logic_error`, `length_error`, `exception`, `allocator::allocate`, `allocator::max_size`, `throw_length_error`; together they flip one function). The target's out-of-line `SlotData::Data` copy (`fn_8016B130`) copies `mFlags` and `mLastArea` byte by byte and the MAC address as a word and a half, so those are separate `s8` fields and an array in the original.

Still open: `BModelObj::CalcSize` (91%): the target multiplies `volume * scale` with the volume operand first; source operand order does not change the emitted order, nor do declaration order or a scaling helper.

### Traps

- **Swapping clone names can break the DOL.** `permute_names.py` on CDlgPiiSelector raised objdiff matches but the build checksum failed: other Matching units link to those weak names. Run a full build before committing clone renames in shared template code.
- **Unreferenced inline destructors are not a blocker.** `~tree` and `~red_black_tree` of a `set` member are emitted unreferenced in our objects (every set, even a local one) but absent in the target; the linker strips them, so they appear as "only in source" without affecting the DOL.
