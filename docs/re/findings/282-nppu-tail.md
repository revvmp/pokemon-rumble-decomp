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
