# NPpuVM long tail

## F-282-4: BitwiseOp

Target `wpse-v1`, `CEngine::BitwiseOp` at `0x80171CE0`, size `0x488`. The starting source matched 98.75862% under objdiff-cli 3.8.2. The prior honest attempts are recorded in [sprint-a03cx](sprint-a03cx.md#2026-10-22-nppuvm-and-cmodel-follow-up), [sprint-a12](sprint-a12.md), and [sprint-perm](sprint-perm.md).

Separate right and left operand locals, retaining the existing const reference aliases, reproduce the first operand's duplicated wide-string type loads and raise the function to 99.25862%. A mutable-reference type read followed by a const-reference read, with the latter returned through an output argument, reproduces both duplicated reference/type checks. The cached types are initialized on every path that subsequently uses them; the reference path returns before reading an uninitialized left type. No value is changed by the helper.

The final mismatch was the right integer predicate's materialization. A local false-initialized predicate, assigned true when either integer tag matches, reproduces the target. The helper preserves the original conversion of integer/function values, floating values, and other tags.

These constructs are a documented D-0016 fake match. Direct compilation with the exact NPpuVM flags from `build.ninja`, Wii/1.1 `mwcceppc.exe`, and sjiswrap, followed by objdiff against `build/wpse-v1/obj/game/NPpuVM.o`, gives 100% for all 1,160 bytes of `BitwiseOp`. The unit remains NonMatching pending the other six functions and data. Full-build acceptance is performed by the coordinating maintainer.

## F-282-5: CompareOp

`CEngine::CompareOp` at `0x80172170`, size `0x80C`, initially matched 98.85437%. Separate operand locals fix its first conversion but leave the cached type loads below the reference comparison. Applying `ReadType` to both values reproduces those loads at the target positions. Passing the cached types explicitly into local integer and float conversion helpers then preserves their register values through the comparisons, reproducing all 2,060 bytes at 100% in objdiff. Each conversion has the same tag cases and arithmetic as the original `CValue` conversion. This is another D-0016 fake match using explicit caching rather than plausible original source.

## F-282-6: Execute

`CEngine::Execute` at `0x80172980`, size `0xDF8`, lacked only the target's redundant `cmpwi; blt` before a branch to the same loop head. Disabling dead-assignment elimination around this function and assigning zero to the dead `code` local in the argument-check case retains the comparison through the global optimizer. The later backend removes the dead assignment itself. The test must be `GetArgInt(0) >= 0`, so the branch bypassing the assignment is `blt`, matching the target. `code` is unconditionally replaced by the next instruction fetch before any use, so the assignment changes no behavior.

This D-0016 fake match produces all 3,576 function bytes at 100% in the direct objdiff comparison. The simpler empty-body forms, standalone pragma sweep, and dead assignments with normal optimization did not retain the comparison. With the pragma, the original `< 0` condition reaches 99.99441%, with only `bge` versus `blt` remaining; the inverted condition resolves it.

## F-282-7: BinaryOp

`CEngine::BinaryOp` at `0x80171400`, size `0x8D4`, initially matched 94.460175%. The same separate operands and explicit type cache used in F-282-4 and F-282-5 remove its conversion-load differences. Cached null and integer predicates preserve those type registers through the reference cases.

Named pointer locals with compound offset updates reproduce the early pointer loads and destination registers. The integer result tag is stored before reading the two pointers for reference subtraction, reproducing the zero register and subtraction operands. In the reference-add case, a successful right-reference operation breaks explicitly; otherwise execution proceeds to the left-reference check. This reproduces the target's branch to that check when the right pointer is null. The prior else-if spelling skipped it, although the earlier rejection of two references makes the outcomes equivalent.

The final isolated comparison is 100% over all 2,260 bytes. These explicit caches, local ordering and tag-store ordering are recorded as a D-0016 fake match. The helpers retain the original type cases, numeric conversions and pointer operations. BinaryOp joins BitwiseOp, CompareOp and Execute as exact functions; CCode::Load, CEngine's destructor and CollectGarbage remain unfinished.

## Other attempts

An isolated per-function `Execute` sweep over the compiler's `opt_*` switches plus `peephole` and `scheduling` did not improve its 99.77628% baseline without a source change. `opt_optimizecontrolflow off` lowers it to 99.272934%; `peephole off` to 98.13199%; `scheduling off` to 83.76734%. Only the combined change in F-282-6 is retained.

Expanding `CCode::Clear` inside `Load`, explicitly caching header fields before member stores, and caching string allocation results before the header-size check raised `Load` from 88.051285% to 94.02564%, but still changed instruction scheduling and register selection. The experiment was not retained. Changing header integer fields to unsigned long did not solve the aliasing differences and was reverted.

## Traps

No new project-wide trap. Separate operand locals and const aliases must be tested together: the former fixes the first conversion while the latter preserves later type caching. Per-function results do not establish a whole-unit link match.

## F-282-8: Further destructor, load and collection probes

The integrated baseline is now 60/63 real functions exact. The remaining three were investigated in isolated direct compiles under `tools/local/session/282-vm2`; none of the following experiments was retained.

The engine destructor differs only in register allocation and the resulting save schedule. The target keeps the deletion flag in r31, this in r30, the instance tree in r29 and the removed node in r28. The source uses r31/r29/r28/r30. A member inline cleanup wrapper puts this in r30 and improves 95.1031% to 98.0412%, but changes the find loop's volatile registers and still swaps tree/node. Separate iterators, pointer round trips, explicit tree erasure, helper arguments, moving the generic erase definition out of class, and local optimizer pragmas do not close it.

For CCode::Load, expanding Clear and caching the magic before its final pointer stores, caching all four header counters before member stores, and constructing the wide-string pointer through integer arithmetic raises 88.0513% to 94.1197%. Permuting the four cache initializers reaches 94.2137%. Header-reference helpers, restricted pointers, header-value argument helpers, and optimizer switches do not reproduce the remaining load/store scheduling, register assignment and add operand order. This confirms the earlier cache experiment without resolving it.

For CollectGarbage, a pointer-to-u32-to-pointer conversion on the engine yielded by the set iterator reproduces the beginning of the mark loop and raises 89.3% to 90.0429%. Disabling opt_lifetimes together with that conversion reaches 91.1%. The map find expansion still caches its end-node pointer where the target recomputes it; register and scheduling differences continue through both mark paths and the sweep. Other local optimizer toggles regress the candidate.

## F-282-9: Garbage collector const-end and sweep lifetime investigation

A later isolated pass resolves the collector's first 0x2F0 bytes exactly. The pointer-map specialization lacked const end accessors. Adding const end and const end_node implementations in a scratch map header, and calling the const end overload only from MarkValue, prevents the unwanted end-node address from being kept across the find expansion. This is the same confirmed distinction between const and mutable end-node construction observed earlier in CPpuEnv clear. Combining it with the engine-pointer integer round trip and an unsigned global-loop index gives 94.42857% versus 89.3% baseline, with the entire marking phase exact.

The target uses a separate next iterator while sweeping. A next-iterator assignment in the loop condition, advancing next in the body, and assigning current from next at the bottom retains the target redundant loop-header copies when opt_lifetimes is disabled. This reaches 97.71429%. The remaining mismatch is current-node lifetime/coalescing: target keeps current in a volatile register until erasure, then copies it into a saved register; the compiler merges it with erase's saved node for the whole loop. Postincrement, const-reference current values, generic forwarding helpers, explicit iterator copy removal, integer/enum round trips, and const-reference erase signatures did not finish this last boundary.

All new candidates remain scratch-only: tools/local/session/nppu-ppu/markround.cpp/.json and nextcondition2.cpp/.json, with the required const-end additions in nppu-ppu/include/map. No shared header changes are accepted from this pass. The header snapshot map-before-copy.h preserves the useful const-end variant; its added postfix increment probe is not required by the best candidate.

Fresh destructor pointer aliases and erase forwarding probes reproduced the earlier 98.04124% partial result but did not improve it. Header field/cache enum and const variants in Load reached only 94.290596%; volatile member stores did not fix its scheduling. These partial experiments are not retained source gains.

## F-282-10: Exact engine destructor through concrete node lifetime

The engine destructor at 0x80170510 (388 bytes) now compares at 100%, raising NPpuVM to 61/63 exact functions. A named const void pointer inside the pointer-set find specialization preserves the target this register and all volatile find-loop registers, reaching 99.53608%. The remaining tree/node saved-register swap disappears when erase keeps its removed node as the concrete node pointer rather than the base-node pointer. Neither change alone resolves the function.

Both operations are local inline explicit specializations in NPpuVM.cpp. No shared headers change. Find uses the same pointer comparison, and erase retains the same increment, balancing, destruction, deallocation and size update. This is documented D-0016 source shaping: the local specialization structure is intentional and does not claim original template-source recovery. The broader allocator and explicit-delete-cast probes were unnecessary and are not retained.

The exact direct compiler command and objdiff comparison are preserved in tools/local/session/nppu-ppu/dtaccepted.cpp and dtaccepted.json. All previous 60 exact functions remain exact; function-order LCS remains 59/63. The unit remains NonMatching with Load and CollectGarbage unfinished. Coordinating maintainer validation of the integrated full build is pending.


## F-282-11: Collector sweep copy lifetime narrowed to one register swap

A fresh isolated pass preserves the accepted destructor prefix and merges only the prior nextcondition2 collector tail into the current source. Its map overlay starts from the current shared header, retaining BWorld layout guards, and adds only the three const-end accessors needed by F-282-9.

Advancing erase's existing position iterator instead of constructing a separate next iterator removes the unwanted current-node coalescing and reaches 98.814285%. Taking that inner erase iterator by mutable reference additionally restores every volatile register, reaching 99.87143% over the native 1,400 bytes. The outer pointer-map erase still takes its iterator by value, so advancing its inner iterator does not alter the caller's current iterator. All instructions now align. Only nine operands in the character-string erase interchange r29 and r30; the wide-string sweep and entire marking phase are exact.

This is an unfinished scratch lead, not an accepted shared-header or source change. Reproduction uses tools/local/session/nppu-61-probe.py with nppu-61-compile/base.cpp and map-ref.h copied to its include/map overlay; erref2.json records the comparison. Its RA trace ra.txt shows the character tree temporary r43 (degree 24) assigned r29 and removed-node temporary r62 (degree 22) assigned r30; the target wants the opposite. The analogous wide temporaries already match.

No further improvement came from concrete/const/void/integer node views, outer mutable-reference erase, named map/iterator declarations at five scopes, explicit iterator copy/assignment bodies, helper argument permutations and inline depths, splitting erase finalization into a helper, const/restrict string-pointer locals, whole-sweep forwarding, or local optimizer toggles. Passing the node explicitly from a named caller local reintroduced loop-wide coalescing. A named tree pointer inside the outer erase caches the map across the loop and loses required reloads; a volatile view of the global restores those reloads but returns to the same nine-operand swap. Identity rotate intrinsics did not finish the register allocation. No partial candidate was installed in main.


A subsequent fresh Load pass also remained at 94.290596%. Restricting the data/header/this pointers, volatile header reads combined with volatile pointer-member stores, pointer-typed views of the data-size words, integer/pointer storage views, Boolean condition materialization and pointer-add operand/type changes did not improve the prior cached-header candidate. All remain private probes under nppu-61-compile; no Load change was installed.

## F-282-12: Independent collector register-identity pass

The F-282-11 candidate was reproduced in a separate direct-compiler lane, tools/local/session/nppu-fresh, at 99.87143%. Its only nine differing operands still interchange the character-map receiver and removed node between r29 and r30. The accepted main destructor was not edited.

No exact gain resulted from full-range signed/unsigned enum node views, const/volatile pointee views, register storage spelling, node-reference helper parameters, out-of-class erase definitions, a named aggregate erase context, pointer/integer unions, 64-bit pointer-identity conversions, conditional pointer assignments, or explicit receiver/removed-node caller locals. Named caller locals again cause loop-wide node coalescing. Replacing only the character-map erase with a specialization permits isolated receiver experiments, but receiver enum/address views cache the map across the loop; a volatile global-pointer load restores the required reload and returns to the same nine-operand residual. None of these source/header candidates is retained.

The runner nppu-fresh.py starts from nppu-61-compile/base.cpp and map-ref.h, directs both source and object output to its own directory, and saves each source/header/objdiff result. It performs no shared configure or ninja work. The best candidate remains F-282-11 unchanged.

## F-282-13: Load reduced to three scheduling swaps

A fresh private pass reaches 94.871796% for CCode::Load (468 bytes). This score is dominated by instruction-order penalties: every operation, branch, register operand, frame offset, and loop-alignment nop is present. Only three adjacent instruction pairs differ: SUBIS versus the reset store at offsets 0x58/0x5C, and CMPWI versus the newly allocated pointer stores at 0xF0/0xF4 and 0x158/0x15C.

The count block now matches exactly. Separate declarations from initialization: declare wideCount, stringCount, codeSize, then stackSize; initialize wideCount, stackSize, codeSize, then stringCount. The stack value retains the existing HeaderCount enum conversion. Four volatile member lvalue stores preserve the target stack/code/string/wide write order. Merely permuting declarations together with their initializers did not solve the register priority. Aggregate and constructor-mediated count copies, by-value count helpers, and type changes also failed.

Both string-address computations now match. Form the byte offset as a named u32, then assign `address = static_cast<u32>(address + reinterpret_cast<u32>(pData))` before adding sizeof(SHeader) and converting to the string pointer. The explicit same-type cast on the addition is material: a plain compound addition or an identity cast after the completed addition does not reproduce the temporary register identity. The character offset is mCodeSize + pHeader->mWStringDataSize; the wide offset is mCodeSize.

The best source and comparison are tools/local/session/nppu-fresh/loadcompound8.cpp and loadcompound8.json. This is a diagnostic whole-file snapshot derived from an older Load candidate: transfer only CCode::Load, never the entire file, because its collector differs from the current best collector. No main source or header change is retained from this pass.

Further private tests did not resolve the three scheduling pairs: volatile pointer stores; signed/unsigned/enum/const scalar identities; conditional, switch, comma and inline store-helper forms; whole-load inline wrappers; formal restricted pointers; empty inline and empty-destructor boundaries; malloc/free exception attributes; and zero-length memory calls. Lvalue-assignment address identities add arithmetic instructions and are rejected. Disabling scheduling or peephole optimization, or enabling aggressive load/store scheduling, changes many otherwise correct instructions and is rejected. The best candidate uses the existing compiler flags without these pragma experiments.
