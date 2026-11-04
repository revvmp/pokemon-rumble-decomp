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


## CPlayerPii follow-up

The next coordinated source ownership was CPlayerPii.cpp, whose current direct baseline is 253/254 functions exact. GetShadowName is 88.85714%. The precise target schedule is LR store, first name-table load, mpPlayer load, then the other three table loads. The source schedules the first name-table load before the LR store and mpPlayer after all four table loads.

No CPlayerPii source change was retained. Same-type, const-removing, integer-identity, and volatile pointer casts of the player access do not change the schedule. Named player locals before or after the array also do not help, including the pointer-to-integer identity technique that fixed CModel. Mutable arrays remain worse at 79.85714%; explicit individual element copies from a static table are 67.14286%, regardless of the source position of the player load. Inline aggregate helper experiments add a call under dont_inline, while removing that pragma also inlines GetPlayerNo and changes the intended function structure.

Scheduler experiments confirmed the prior result: 604 and 7450 reach 90.47619% but retain the incorrect LR-store position; 603, 601, and disabled scheduling are worse. These were scratch-only probes, not retained compiler settings. A scheduling model remains selected after a plain `scheduling on`, so isolated experiments must restore the actual model (750 here), rather than treating `on` as a model reset. This observation explains unrelated template score changes in the first scratch sweep; no tracked file was changed.

Scratch direct-compile JSON and candidates are in ignored tools/local/session/329-player. The existing deque emission-order blocker is unchanged. No new matching or link claim is made for this unit.


## CMsgRes follow-up

CMsgRes.cpp was the next assigned source. The direct baseline has only ConvertEscape unmatched at 96.57971%; the constructor is exact. Its remaining differences are entirely in the preheader instruction schedule: the target shifts the SJIS comparison result and completes the locale address before copying pSrc to pDst. The loop body already matches.

No source change is retained. Same-type and integer-identity pointer casts, source/destination reassignment, explicit scalar casts, and alternate boolean representations do not reach an exact match. Explicit locale locals with helper arguments change register allocation substantially. Supplying the locale expression directly to the same helper preserves the original result; adding a cast to that expression changes allocation but does not fix it.

Moving the whole loop into a small inline helper is a new useful diagnostic: selected scalar/reference parameter forms reproduce the entire target instruction sequence, with only register operands differing. The best measured version reaches 98.115944%. This is not an accepted gain because it replaces the simpler source with many register differences. In this candidate, the caller retains its pointer/flag locals, passes pSrc by reference, passes the remaining four by value, and passes digit and the loop counter by reference. The inline helper still uses the existing byte conversion operations. The resulting register mapping differs across destination, SJIS flag, end pointer, trail flag, digit, counter, accumulator, and locale component.

All 120 orders of the five pointer/flag parameters give the same result. Reference/value combinations, moving digit/counter/accumulator outside the helper, boolean and integer parameter types, and helper initialization of selected preheader locals were also tested without a 100% result. Some initialization-only probes used a destination before its delayed source initialization; those were diagnostic scratch failures and must not be retained. None of these experiments changed tracked source or shared headers.

The whole-loop diagnostic and score reports are in ignored tools/local/session/329-msg, especially extended12.cpp and extended12.json. The original baseline remains the main source.


## Main emission-order follow-up

The root agent supplied tools/local/session/main-order-next/1.cpp, with all 145 target functions exact and a SequenceMatcher common-order score of 104. All following work remained scratch-only: main.cpp, shared headers, symbols, and build configuration were not modified.

The best source-only candidate is tools/local/session/329-main/deferweakfont.cpp. It preserves all 145 exact function comparisons and has a true longest common subsequence of 124/145 target functions. The initial candidate's true LCS is 105; SequenceMatcher is a heuristic and undercounted this case. The direct runner329-main.py now computes the dynamic-programming LCS. All experiments use the original split object, exact game compiler flags, and the scratch filename main.cpp to preserve anonymous-namespace mangling.

Retained in the scratch candidate, in order of construction:

- Declare the explicit shared_ptr<CMsgRes> raw-pointer constructor specialization before the includes and define it after InitializeMainMessages. Its generic body is unchanged. This puts the copy constructor and arrow operator ahead of it, exactly matching the first eleven target functions.
- Supply source-local full function_base specializations for the file-error, layout-failure, and HBM-close signatures. They retain identical virtual declarations and empty destructor bodies, but defer the destructor definitions to the required region. Member-only destructor specializations with separate declarations and later definitions fail as redefinitions in this compiler.
- Expand ResourceFileConfig's existing header into the candidate, add an explicit destructor declaration, and define its unchanged generated behavior later. Its function remains exact.
- Defer the CDisableHBM and CDisablePadRumble shared-pointer raw-pointer constructors and the void()/HBM-close function wrapper constructor specializations. Bodies are copied from the generic implementation. The late definitions are interleaved with the HBM-close base destructor.
- Defer the four font shared-pointer constructors (PairFont, CResFont, CArchiveFontWithBuf, FontMap) and the three converting Font assignments. Arrange their unchanged definitions in the target wave.
- Move RunSession's source definition before CMain's destructor. This is a real one-function LCS improvement that the earlier SequenceMatcher score hid.
- Insert a named-false map operator[] probe immediately before the first dereference of the shared FontMap. This requests the map accessor before the shared-pointer dereference without emitting instructions. All target functions remain exact.
- CResFont's constructor and destructor match both code and desired emission timing when they are compiler-generated. The candidate expands its header locally and removes both explicit declarations and both empty source definitions. This is a plausible-source correction: explicit empty bodies reproduce instructions but request the functions too early.
- Defer the FontMap weak-pointer converting constructor specialization until after LoadFonts. Its unchanged generic body moves to the correct wave.

A separate scratch include/string override marks the generic basic_string::rfind definition inline. It moves rfind into the immediate helper wave and adds one correctly ordered function without any code regressions. The root agent requested keeping this scratch-only until a complete candidate exists; a macro-scoped main-only header switch is acceptable for a future validation. The override is under329-main/include/string and is not enabled by the normal runner.

Rejected attempts include all 24 nonvirtual CMain declaration orders, inline markers on its methods, moving whole bodies into the class, sizeof shared-pointer probes, early named-false destructor probes, separate ordinary template-member specialization declarations/definitions, and forced-inline pointer helpers. Moving the LoadFonts body into the class worsens ordering. A full local shared_ptr<Font> class specialization preserves all 145 function bodies but changes destructor timing and is not retained. Late ordinary StrapReminder/SaveManager pointer helpers can occupy the desired places, but replace known library symbol identities; they are not retained. Explicit instantiations and extern-template combinations do not solve the remaining accessor waves.

### Emission-measurement trap

SequenceMatcher matching blocks are not a longest common subsequence. In this main.cpp investigation it reported the same104 score before and after correcting RunSession/destructor order, while dynamic programming proved105 versus106. Use a real LCS for progress comparisons and still require complete ordered equality and a frozen-SHA build before claiming a source-unit link match.

### Complete function order candidate

The subsequent candidate is now applied to src/game/main.cpp with narrowly scoped MAIN_EMISSION_LAYOUT switches in include/memory, include/string, and include/functional. Direct compilation proves all 145 target functions exact and their complete relative order identical (applied.json in tools/local/session/329-main). Full source-link acceptance remains pending; this is not yet a frozen-SHA match.

Additional mechanisms completing the order are deferred generic shared_ptr arrow bodies with three explicit accessor specializations, split sizeof type probes controlling the reverse template wave, inline generic string rfind, deferred four-argument function wrapper destruction, moving the complete CResFont definition before CArchiveFontWithBuf, and a source-local FontMap pair specialization whose default constructor and destructor definitions are deferred. The map find_leaf specialization retains the generic body. These are explicit emission-layout fake matches, not recovered original source organization. All ordinary library callers retain their original headers because each shared switch is gated by MAIN_EMISSION_LAYOUT.

Object data remains a separate blocker. The target .data is 5664 bytes versus the candidate 5654, with function_base and CFiber metadata ordering differences; the source also emits additional weak functions and metadata that require linker dead stripping. Changing full function_base class specializations to member-only destructor specializations plus scratch-header deferred generic destructors preserves all 145 functions/order but does not repair RTTI ordering. An early CFiber deleter sizeof probe moves its metadata toward the target but regresses one common function's order, so neither experiment is retained. The unresolved link reference at 0x804B7950 is proven to be __RTTI__17IGameLoopCallback by its relocation to the original IGameLoopCallback name string at 0x803B5FEC.

### Metadata ordering correction

The first native link exposed a previously misnamed destructor: 0x801309F0 is shared_ptr<ISession>, not shared_ptr<IGameSession>. The target's real IGameSession specialization is at 0x800BC050. Changing the early main type probe to ISession restores the exact function order against the corrected symbols; raw byte scoring had hidden the wrong identity because these destructors have identical bodies.

The applied datareview candidate retains all 145 exact functions and their complete order, and now reproduces the original normalized .data bytes through offset 0x161A; only six trailing alignment bytes are absent from the object. Its original 264-byte .sdata prefix also matches after relocation masking. Complete-link verification remains required.

The metadata correction relocates each full function_base specialization's complete definition immediately before its corresponding function_imp sizeof probe. An additional CFiber deleter probe follows the CMsgRes probe. GameLoop.h must be included after ut_ResFont.h but before the local CResFont complete definition, placing IGameLoopCallback metadata between CResFont and ResFont. For the void() function_base, HomeButton's inline SetOpenCallback previously forced early instantiation: the source expands the unchanged HomeButton class interface, defers only this setter's identical inline body, forward-declares the specialized base before that class, and completes the base before its function_imp probe. This fixes the final displaced RTTI block without changing any target instructions. No extra shared-header changes were required.

The initial CFiber probe regression was measured against the wrong IGameSession destructor identity; after the symbol correction, the same metadata probe preserves all function order. The complete-definition placement is verified for this compiler/unit; merely moving an empty destructor definition did not move its class metadata.

### Main full-link acceptance

Root enabled main.cpp as Matching and ran the actual shared build after datareview was applied. Ninja exited 0 and the frozen-manifest verifier returned PASS. All 145 functions, their identities/order, linked metadata, exception tables, and executable layout are therefore accepted together. The root retained the Matching selector. This supersedes the pending-link qualifications above for the applied candidate; scratch alternatives remain unaccepted.

### CModel renewed allocator probes

After main.cpp's accepted source link, another focused CModel pass tested output-reference and output-pointer inline helpers, two-field local aggregate state, dependent initialization of the two boolean states, and all six orders of helper parameters carrying the list and initial true/false values. None improved the remaining shared inner/outer boolean register swap. Aggregate bool/int/unsigned fields reduce to the same code; unsigned-char fields add normalization. Passing the initial inner false value from the caller changes its scheduling; restoring conditional initialization reproduces the original mismatch. No CModel source or header change is retained. Evidence is in ignored 329-model/renew*, agg*, out*, and params* reports.

### CLayout renewed stack-layout probes

A source-local explicit set<CLayout*>::begin specialization reproduces the earlier closest StopMotion result (99.907486%) without changing shared headers. Along with the map-find result cast, all remaining mismatches are stack offsets. Moving the child-loop condition into one or two reference-taking inline helpers changes those offsets slightly; the best reaches99.911896%, still without a match. Passing the iterator by value adds copies, and naming or caching the end iterator changes control flow and regresses many already exact inline callers. No CLayout edit is retained from this pass. Scratch evidence is in 329-experiments/renewlayoutspec, layoutcond*, and layoutend*.

### CModel fresh qualifier and identity pass

The fresh main-only pass reproduced the three remaining scores: QueueMotion<true> 99.40945%, AppendMotion<true> 99.63415%, and Update<true> 99.83957%. AppendMotion differs at exactly six register operands: the target outer boolean uses r6 and inner boolean r7, while source uses r7 and r6. The allocation trace assigns the inner virtual r44 before outer r42, with degrees 11 and 12 respectively. No instruction operation differs in this function.

Source-local IsReplaceable specializations tested same-type reassignments, integer and pointer identity round trips, explicit bool/int/pointer states, lvalue identity helpers, scalar arrays and aggregate fields, logical/ternary equivalents, and one to three levels of separate inner/outer inline helpers. These either preserve the allocation or regress. Reinterpreting boolean storage through pointers forces stack storage. Bitfields introduce extra operations. Source-label block permutations change the emitted control-flow layout; a small diagnostic sample was stopped after confirming that regression. No candidate was retained.

Scratch const/non-const IsIdle and IsReplaceable headers confirm that a mutable storage view restores QueueMotion's missing reload (99.88189%), as previously observed, but does not fix the boolean registers. Const front access changes additional operations. Source and shared headers remain unchanged by this pass. Compiler/objdiff evidence and runners are under ignored tools/local/session/model-fresh through model-fresh9; model-fresh/ra.txt records the allocator input and coloring. These are private direct compiler comparisons, not a full-link acceptance claim.

### CLayout ReplaceMotions acceptance candidate: exact iterator slots

The fresh main-only investigation matched ReplaceMotions at 100% with no regressions among previously exact functions. AdvancePaneMotions improves from 96.283066% to 96.55291% as a consequence of the same RebindAnimations helper. StopMotion, FindAnimation, FindPaneMP, and DetachAttachedLayouts retain their previous scores. The formatted source was applied and recompiled directly with the configured Wii/1.1 game flags against build/wpse-v1/obj/game/CLayout.o. Full-build acceptance remains with the root coordinator; this unit is still NonMatching because five functions and its source link remain unfinished.

The initial ReplaceMotions mismatch contained one extra iterator-copy store, an extra stack slot, and consequent frame offsets. The map iterator assigned by RebindAnimations must retain its own storage without materializing the begin() return temporary. A local LayoutMapStorage view names the three observed words: count at 0, root pointer at 4, and first-node pointer at 8. Copy-assigning the existing iterator from an iterator view of the last word emits the required one store. The view follows map_compact_tree's allocator/count, allocator/root, comparator/first-node compressed pairs and the zero-storage adapters in include/map. It reads the first node; it does not mutate map storage.

That change alone reached 99.97369%, leaving exactly six stack-slot offsets. The target has two retained four-byte iterator slots in each descendant begin/end adapter, in addition to the returned const iterator. BeginLayoutDescendants and EndLayoutDescendants now declare the result iterator before a separate source iterator, write the source node through a volatile-qualified reference, set the result node from the same cached node pointer, and return the layout-compatible const-iterator view. The explicit volatile assignment retains the otherwise dead source slot; reverse declaration order puts both slots at the target offsets. Source and result node pointers remain identical. The helpers inline completely.

This is a D-0016 fake match, not a recovered library interface. It includes the map storage alias, set/tree iterator storage views, redundant volatile stack stores, and deliberate local declaration order. Original SDK/library iterator-adapter source is still unknown. Target behavior and the complete ReplaceMotions instruction comparison are preserved; there is no machine-code injection or reference modification.

Rejected approaches in this pass: moving iterator declarations and using native_iterator either removed the required retained slot or changed unrelated frame offsets. Simple begin/end forwarding depths zero through three did not repair the six offsets. Two-iterator aggregate state retained the correct stores but moved them into an eight-byte allocation group, putting the list-allocation temporaries in the wrong positions. Promoting that aggregate into callers and wrapping list push_back did not fix its ordering. Separate ordinary locals optimized one required store away; retaining only the source pointer store through a volatile member reference, and copying from a cached pointer rather than rereading the volatile slot, reproduces the target without extra loads.

Private evidence is under tools/local/session/layout-fresh. raw2-0.json is the six-offset intermediate, separatevol1.json is the first exact function, simple0.json proves the direct volatile member-reference simplification, and base.json was refreshed after applying the formatted main source. Runners layout-fresh-raw2.py, layout-fresh-separatevol.py, layout-fresh-simple.py, and layout-fresh-apply.py preserve the experiments and the narrow source transfer. These ignored diagnostics are not build inputs.


A root follow-up tested30 enum/scalar destination-initialization combinations for CMsgRes ConvertEscape. None improved the96.57971 baseline; no source changes were retained. Unlike the verified Pause/BWorld sites, a typed enum view of the SJIS flag did not change the preheader scheduling. Evidence: ignored tools/local/session/msg-enum.py.

### Clean acceptance and second CLayout gain

The root coordinator subsequently validated the ReplaceMotions change with two clean builds. Both reproduced identical DOL, ELF, and report outputs, and frozen-manifest verification passed. No previously exact function regressed.

The same const-iterator adapter mechanism also matches DetachAttachedLayouts. The private intermediate had exact instructions and registers but three remaining stack offsets: its begin adapter needed two separate local iterator slots rather than the original mutable-return temporary. Generalizing BeginLayoutDescendants and EndLayoutDescendants over the pointer element type and using them for the local children copy resolves all three offsets. This introduces no additional helper bodies, shared-header changes, or set specializations. The D-0016 disclosure above applies to this second caller too.

After applying and freshly compiling the formatted main source, CLayout is 167/171 exact. ReplaceMotions and DetachAttachedLayouts are both 100%; the other scores are unchanged: StopMotion 98.11894%, FindAnimation 98.17969%, FindPaneMP 93.52651%, and AdvancePaneMotions 96.55291%. The second change still awaits central full-build validation. Private generic.json isolates DetachAttachedLayouts as its only score change; layout-fresh-generic.py and layout-fresh-apply-generic.py retain the diagnostic and narrow source transfer.

The unretained StopMotion direction remains promising but incomplete. A source-local set<CLayout*>::begin specialization retaining an additional copy plus a map-find result cast reaches 99.907486%, with only 21 frame-slot offsets different. A matching end specialization changes DetachAttachedLayouts register allocation, but was unnecessary once the generic local adapters solved that function. Neither specialization nor the StopMotion cast is applied. Replacing recursive UpdateAttachedLayouts begin/end calls with the new adapters changes its inlining and regresses previously exact callers, so those calls deliberately retain their existing implementation.

A further75 CMsgRes ConvertEscape pointer/enum address-identity combinations also failed to improve the96.57971 baseline. No source changes were retained. The whole-loop helper caller declaration-order sweep is separate, with diagnostic inputs under msg-helper-orders.
