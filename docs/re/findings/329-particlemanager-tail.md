# ParticleManager Calc tail

Current investigation is scratch-only. The retained candidate scores 99.963394 for Calc, with all 1366 instructions structurally identical and seven register-operand differences in the post-field name-table address chain. The other sixteen functions remain exact. This is not an accepted source match or a verified source link.

The reproducible candidate consists of `tools/local/session/329-particlemanager/indexorder2.cpp`, private field-header overlay `postaddr2.h`, and emitter-resource header overlay `indexidentity0.h`. The direct runner is `tools/local/session/329-particlemanager.py`; add the scratch include directory before repository includes. Restore both overlays before compiling because exploratory runners replace them.

## Proven improvements

The target computes emitter scale immediately after constructing axisSum, before matrix setup. Hoisting that computation restores the target call placement. Reverse the two final rotation-only matrix declaration/copy blocks. Declare the track index immediately before the post-field pointer to reproduce their saved-register assignment.

The existing HashCurveSeed and NextCurveSeed helpers reproduce the random-field seed sequence. Explicit VEC3Sub and VEC3Scale calls restore paired-single operand order and stack slots; retain the original gravity scale operator.

In the resource accessors, assignment-form same-type identities on the parameter-description size and particle-track byte offset reproduce six operand differences. These changes currently exist only in a scratch shared-header overlay and must be isolated or audited before integration.

The post-field helper advances a byte cursor past key data, converts its address to u32 for the header-size addition, then advances through range and random tables. This restores the exact instruction sequence but leaves the seven operands below. The pointer/integer step is a code-generation accommodation and would require a D-0016 ledger entry if retained.

## Remaining operands

| Calc offset | Target | Candidate |
| --- | --- | --- |
| 0x100c | add r4,r23,r0 | add r7,r23,r0 |
| 0x1010 | lwz r7,20(r23) | lwz r4,20(r23) |
| 0x1014 | addi r0,r4,32 | addi r22,r7,32 |
| 0x1018 | lwz r4,24(r23) | lwz r0,24(r23) |
| 0x101c | add r0,r0,r6 | add r22,r22,r6 |
| 0x1020 | add r22,r0,r7 | add r22,r22,r4 |
| 0x1024 | add r19,r22,r4 | add r19,r22,r0 |

The candidate coalesces the intermediate cursor into the final saved name-table pointer early. The target retains an anonymous r0 accumulator until the final random-table addition.

## Traps and failed approaches

Fully flattening the address expression allows the compiler to move the constant header-size addition, losing instruction identity. Named signed/unsigned integer intermediates, assignment-form identities, enum identities, const offsets, pointer qualifiers, member/free helpers, helper parameter references/order, local single-field structs/arrays, and earlier declaration of the final name-table pointer did not remove the remaining coalescing. A union crossing pointer/integer fields regressed. A separate header-skip helper produces eight differences rather than seven. Output-reference helpers alter wider allocation and regress. Typed-pointer strides for the header-size addition also reassociate the constant.

These outcomes are specific to this address chain and do not invalidate the same-type identity technique elsewhere. No tracked implementation changes or shared builds were performed for this investigation.

The actual register-allocation trace is saved in the same scratch directory as trace.txt. The early saved cursor is anonymous compiler temporary @4821 (virtual r49, degree 35), not a surviving named source local. Replacing the last cursor assignment with a return expression avoids early saved-register coalescing but yields eight volatile-register differences. Named offset identities can score 99.96559 while changing addition grouping and load order; this is not a better structural candidate. The retained 99.963394 package remains preferred.
