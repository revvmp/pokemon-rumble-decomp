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

## Traps

- **Deleting a statement can change inlining everywhere in a function.** Under `-ipa file` the function being compiled is all-or-nothing against a per-function budget of flattened inline code (F-i1-1). Do not chase single refusals from `warn_notinlined`; find the level structure instead (F-i1-2).
- **Pruned callees look like non-inline functions.** A helper the target calls out of line may still be an inline function that a nested over-budget level pruned. Check whether the same helper is inlined elsewhere in the same target function before declaring it non-inline.
