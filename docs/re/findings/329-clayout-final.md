# CLayout final matching pass

## 2026-11-04: four function matches

Target: `wpse-v1`, unchanged original `main.dol`. Compiler: the configured Wii/1.1 compiler and `cflags_game`. The initial direct object comparison had ten unmatched functions. The retained changes reduce that to six, with no losses among previously matched functions. The coordinating agent's full `ninja` build and frozen-manifest SHA verification passed with the first two changes on 2026-11-04. The two attach-function gains below still require the next full validation. CLayout remains NonMatching because six functions and its source link are unfinished.

### MakeArchiveList array overload

At `0x801CABD0`, the reconstructed `const char* const*` parameter allowed the compiler to load the first array entry before initializing the result list. The target initializes the list first. Changing the anonymous helper to `const char**` restores that ordering; incrementing the parameter directly instead of a separate `pName` local also restores the order of two register moves. The function improves from 94.35185% to 100%.

The anonymous helper's name and signature were inferred, not recovered original symbols. Its symbol is now `MakeArchiveList__21@unnamed@CLayout_cpp@FPPCc`. The public exception constructor keeps its existing signature and passes a `const_cast<const char**>`; the helper never writes an array element. The cast is conservatively disclosed in the fake-match ledger.

### CalculateAttachedMtx

The function improves from 96.48707% to 100% using redundant casts in `GetLayoutAlpha` and `GetPaneAlpha`. All other function scores remain unchanged, including the standalone `GetPaneAlpha` at 100%.

The root-pane cast prevents the inlined `root == root` comparison from folding. Casting the attachment access through `static_cast<CLayout*>(this)` and `static_cast<CAttach*>` prevents reuse across recursive inline levels. The casts must cover both the attached-pane read and the parent-layout read. With only the pane read cast, all instructions exist but six GPR operands differ: the cached attachment pointer occupies r5 instead of r3. Casting the parent read too restores the target reload and register allocation.

Earlier honest attempts and related diagnostics are in [sprint-a02](sprint-a02.md#unsolved-after-the-fake-match-wave), [sprint-a03](sprint-a03.md), and [codegen rules](../CODEGEN_RULES.md). These redundant casts are fake matches under D-0016.

### Attach and AttachLayout

`Attach` improves from 99.19036% to 100%, and its inlined copy in `AttachLayout` improves from 99.206085% to 100%. These are the only score changes in this experiment.

The target has two zero-byte stores after set insertion, at frame offsets `0x10` and `0xC` in `Attach`. The reconstructed source omitted those stores and their two four-byte temporary slots, shifting other frame objects by eight bytes. Two value-initialized empty `IteratorConstructionState` temporaries in an inline helper reproduce the stores and frame size. One additional forwarding helper reverses their slot order, giving the exact target sequence. Both helpers inline completely and add no out-of-line functions.

This is a disclosed D-0016 fake. The original tag type or adapter responsible for the stores remains unknown. Earlier reconstruction attempts are recorded in [sprint-a03](sprint-a03.md#open-with-what-was-tried). Nonempty structs were optimized away; volatile-byte variants reproduced the slots but changed scheduling, and were discarded.

### Validation and unsuccessful experiments

`tools/re/unitdiff.py` initially verified the array helper in an isolated worktree. After the owner requested work directly on main, retained edits were transferred to main and experiments used a unique scratch source named `CLayout.cpp`, a separate output object, the exact flags from `build.ninja`, and `objdiff-cli diff -1 <target object> -2 <scratch object>`. The same basename preserves anonymous-namespace symbol spelling. No shared build outputs were written by these experiments.

A 58-case sweep of zero, one, and two optimizer pragmas, with and without the root cast, did not match `CalculateAttachedMtx`. `opt_common_subs off` reached 98.590515% but introduced two extra reloads. Explicit local caches did not remove those reloads. The retained casts need no pragma changes.

An empty inline `throw()` helper in `RebindAnimations` or `StopMotion` changed no code. Adding it to recursive `UpdateAttachedLayouts` changed inlining and regressed multiple functions; it was discarded. No unit tests were added.

### Remaining six functions

`ReplaceMotions`, `StopMotion`, `FindAnimation`, `FindPaneMP`, `AdvancePaneMotions`, and `DetachAttachedLayouts` remain unmatched. The following scratch-only attempts were discarded:

- Wrapping initial `mAnimations.find` in one inline helper raised `FindAnimation` to 99.03125%, but changed string and comparison-temporary frame slots. Named/scoped string forms, extra helper depth, and by-value string forms did not match.
- Explicit casts of the map `find` result and the mutable set `begin` result restore the missing pointer stores in `StopMotion`. Moving the extra set copy inside `set<T*>::begin` reached 99.907486% with matching instructions and registers, but 22 stack offsets still differed. Additional forwarding levels, return-reference temporaries, declaration orders, and begin/end combinations did not recover the frame order. These shared-header experiments also affected other unfinished functions and were not retained.
- Custom map-iterator assignment operators, raw-storage initialization, placement construction, no-op reference helpers, and volatile stores did not reproduce `RebindAnimations`'s retained map iterator while eliminating only its extra temporary.
- A standalone single-pragma sweep over the eight functions unmatched before the attach fix produced no match. Const local set copies and explicit end caching did not match `DetachAttachedLayouts`.

The scratch compiler harness and results are in ignored `tools/local/session/329-experiments`; they are diagnostics, not build inputs.

## Traps

- A redundant cast can change common-subexpression treatment across recursive inline levels without changing the standalone function. Apply casts consistently to related field accesses: the pane-only cast reproduced the reload but left the parent access cached and changed register allocation. The complete pair is required for this unit; this is not a general compiler rule.


## CModel follow-up

After the four CLayout gains, the coordinated main-branch task moved to CModel. The direct compiler baseline had four unmatched functions: QueueMotion<true> 99.40945%, AppendMotion<true> 99.63415%, Update<true> 99.62567%, and Update<false> 99.80488%.

Expanding the first Update pop_front call into the same list implementation operations and reassigning its node through an identity unsigned-long OR forces the named node to retain a distinct allocator value. This moves the node from r30 to the target r27 without changing instructions. Update<false> becomes 100%; Update<true> becomes 99.83957%, with only the shared IsIdle boolean-register swap remaining. The expansion unlinks the node, destroys the shared pointer, deallocates the node with the same node allocator, and decrements the same count. The uintptr-like cast is a deliberate D-0016 fake, relying on this target's 32-bit pointer and unsigned-long representation.

The retained source uses a named node local in the branch. A plain pointer local, erase(begin()), an iterator local, a free inline removal helper, and a pointer-to-integer cast inside that helper all retained the incorrect coalescing with the `advanced` variable. The direct source expansion with pointer-to-integer identity reassignment is necessary in the tested compiler. Declaring the node before or after `advanced`, inside the loop, or directly in the branch yields the same exact result once the reassignment is present. No shared headers changed.

The direct compile used the main build's Wii/1.1 flags and objdiff against the original split. After applying the source, Update<false> is 100%, Update<true> is 99.83957%, and all other function scores are unchanged. Central full-build verification remains the root agent's responsibility. Scratch evidence is under ignored tools/local/session/329-model, especially node2.json, clean3.json, and the applied baseline.json.

Further rejected experiments: bool/int wrapper constructors and inline return helpers produce identical code; compound boolean assignments worsen the conditional shape; named nested bool/int locals are folded to the original register swap; unsigned-char locals add normalization. Identity multiplication adds a multiply; identity bitwise operations are removed. Pointer-shaped boolean results and scalar identity casts also fold back to the original code. An explicit IsReplaceable<true> specialization successfully isolates these experiments within CModel.cpp but does not itself alter code. Explicit Update specializations likewise do not alter allocation.

A separate unretained QueueMotion experiment replaces its second size() expression with `static_cast<TPartialAnm*>(this)->mMotions.size()`. It restores the target reload and improves 99.40945% to 99.88189%; the shared boolean-register swap remains. Retain that cast only if the remaining swap is solved.

### Additional trap

A named pointer may still coalesce with an inline iterator result and then reuse an unrelated variable's dead register. In this specific Update function, a pointer-to-integer identity reassignment in the direct caller prevents that coalescing; applying the same identity operation inside a free inline helper does not. This is a verified local workaround, not a general allocator rule.
