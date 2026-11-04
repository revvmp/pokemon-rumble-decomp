# BModelObj final CalcSize function match

Root's new lead was a non-const CalcSize member signature. That source change removes the load-scheduling differences and leaves326 target instructions with15 operand differences: a repeated14-operand FPR swap between absolute minimum Z and extent X, plus one final height multiply operand order. The mutable signature is supported by the observed compiler behavior and requires renaming CalcSize__9BModelObjCFi to CalcSize__9BModelObjFi.

The retained height expression is `(bounds.max.y * mScale.y) / 2.0f`. Division by the exact power-of-two constant compiles to the target multiply operand order and reduces the remaining comparison to14 register operands. Swapping the first product operands instead adds another mismatch; reassociating the multiplication changes code.

Immediately after `f32 extentX = mScale.x * maxX`, the source now reassigns `extentX = static_cast<f32>(extentX)`. The redundant same-type conversion changes allocator/coalescing treatment without adding an instruction, resolving the FPR swap in both inlined CalcExtent sites. This is a D-0016 fake. Unary-plus reassignment and a same-type const-reference conversion also reproduce the target, but the explicit value cast is retained for clarity. Numeric identity arithmetic and aliasing casts were not retained.

## Verification

Direct compilation with the real project headers and configured Wii/1.1 flags yields all88 functions at100%, including all326 CalcSize instructions. The diagnostic reference object differs from the original split only by the planned const-to-mutable symbol rename, performed with objcopy on an ignored scratch copy. It is not a build input or modified reference manifest. The source and header signature are applied; root owns the symbol correction, broad build, and frozen-manifest verification.

Evidence is in ignored tools/local/session/329-bmodel: baseline.json proves the15-operand mutable-signature remainder; half1.json isolates the division expression; extentidentity0.json and applied.json prove88/88 exact. No unit tests were added.

## Rejected probes

Separate double temporaries, reference binding, fabs wrappers, and parameter-order variants preserve the register swap. By-value AABB parameters alter instructions substantially. Changing named extents to double introduces conversion differences. Pointer/integer aliasing of the float forces extra memory traffic. A simple scalar same-type reassignment is sufficient; no shared math helper or compiler flag change is needed.

## Trap

A same-type scalar reassignment can alter coalescing even when separate locals, helper parameters, and declaration permutations fold to unchanged code. Here it restores both copies of an inlined floating-point helper without an extra instruction. This is a verified local workaround, not a general optimizer rule.


## Native emission reconstruction

The initial function-perfect native link differed in function and RTTI order. Original vtable relocations proved the two inferred KDTree node deleter destructor names were exchanged: 0x8005A910 belongs to KDTreeZXNode<2,9>, and 0x8005A950 belongs to KDTreeZXNode<0,9>. Root corrected these names. An independent audit of all owned exception-cleanup references found no further shared-pointer or holder identity mismatch.

The retained source-local RemoveChild<2,9> inline specialization copies the generic body exactly and requests its recursive helper in the target slot. Shared-pointer sizeof probes request the char, root-scale, and model specializations in the required sequence. The include order alloc_ptr, allocator, shared_ptr_holder, memory positions the holder destructor between the move-pointer and allocator cleanup waves. A named-false probe inside the source-local SetRotation inline body requests the error-string and exception destructors without emitted instructions. These timing controls are D-0016 fakes. The BMODELOBJ_EMISSION_LAYOUT gate preserves ordinary header behavior for all other translation units.

For metadata, BWorldObj must be complete before BKDTreeLink and BSoundSource; stdexcept must precede ICalc. CKDTree is included after the complete BModelObj declaration and an explicit RootScale deleter sizeof probe. This gives the target ordering of KDTree node metadata, RootScale deleter metadata, and BModelObj metadata without disturbing any function body or function order.

Real-header direct compilation of the applied candidate reproduces all88 function comparisons and all87 nonempty text entries in exact target order. Evidence: tools/local/session/329-bmodel/realheaders.json and realdata.json. Root completed the final native build with ninja exit0 and frozen-manifest verification PASS; BModelObj is retained as Matching.


The third native link differed at only nine bytes: CalcMtx loaded the correct degree-to-index coefficient from the wrong small-constant offset. The compiler emitted 256.0f/360.0f after the half-scale literal, while the target emits it before the integer-conversion double. Moving the inline transform helper or naming a const float did not change pool order. An unreferenced external BModelDegreesToIndex function, placed before CalcPlayRate, requests the coefficient at the target position. The linker can discard its unused body while CalcMtx retains the shared compiler literal. This is a D-0016 fake helper, with no assembly or copied bytes. Direct comparison still reports88 exact functions and87 ordered target entries; literalfunction2.json records the candidate. Root completed the final native build with ninja exit0 and frozen-manifest verification PASS. Inspection of the final linked ELF confirms BModelDegreesToIndex is absent: its body was dead-stripped, while the shared literal is retained at the target address.
