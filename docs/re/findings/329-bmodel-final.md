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
