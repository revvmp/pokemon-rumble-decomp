# Animation-curve tail investigation

## Baseline and scope

The configured Wii/1.1 flags use exceptions and RTTI off. Direct compilation reproduces three exact functions: CreateCurveChild and both baked executors. The five remaining scores are U8 93.206955, F32 94.47577, Rotate 93.434135, Texture 99.61126, and Child 96.30681. No source or header edits from this investigation have been retained. All experiments use private direct-compiler output under tools/local/session/329-animcurve.

## Texture allocation

Texture has matching instruction structure but 29 differing register operands. The first group is a saved-register rotation: target header r31, particle r24, tick r25; source header r24, particle r25, tick r31. ra_trace confirms the named pHeader variable coalesces into the pCmdList parameter node (virtual r32). pHeader itself has degree zero. The target assignment is consistent with a separately retained header local, but that source shape is not established. Explicit register and const qualifiers on the local pointer also leave the result unchanged. A second group swaps the integer-path period and loop-count registers r6/r7.

Rejected probes include 31 combinations of scalar/pointer identity reassignment; pointer/integer roundtrips; signed and unsigned 64 address casts; a union view; const-reference binding; mutable header view; pointer-walk initialization; top-level const/register/restrict parameter qualifiers; scoped signed/unsigned enum address and zero-offset views; and whole-body inline helpers with six parameter orders, one/two forwarding depths, and by-reference input variants. None improves the baseline. One forwarding depth drops to 91.32708; two depths reach 97.93565. Explicitly naming the loop-count load at several integer widths changes scheduling and does not resolve the swap.

## Child lead

A source-local copy of CalcFrameStep taking mutable AnimCurveHeader* improves Child to 97.489044 without changing the other seven scores. It restores the target's second processFlag reload rather than reusing the earlier cached value. This removes five structural differences. The result is scratch mutableframe.cpp and mutableframe.json. Applying the analogous mutable signature to CalcTime instead regresses U8/F32/Rotate; do not extrapolate the local finding to every curve helper.

The second frame calculation receives tick+1. Reassociating the unsigned return expression from period*(loop+1)-tick to (period*(loop+1)-1)-(tick-1) improves Child further to 97.731255 by reproducing the target's subtract-one ordering after inlining. This identity remains a scratch probe, not an accepted fake. The remaining structural differences are concentrated in the second frame calculation's integer-to-float conversion slots and scheduling. Named next-tick variables, preincrement forms, an extra forwarding level around either/both frame calls, and 24 explicit float-local declaration permutations did not resolve them.

Duplicating the frame helper under a second name or using distinct template specializations leaves the 97.731255 candidate unchanged. An optional bool-pointer end output with nullptr for the second calculation regresses both Texture and Child.

Widening CalcTime's frameLength from u16 to u32, s32, int, unsigned int, or const u32 did not improve the three large executors. It should not be confused with the accepted baked-executor widening described in sprint-a17.

## Evidence and limits

329-animcurve.py compiles the baseline; dedicated helper, identity, header, period, enum, frame, floatorder, timewidth, and mutabletime scripts reproduce the experiments. trace.txt records actual compiler register allocation. No native source-link trial is warranted because five functions remain nonexact. No unit tests, assembly, object rewriting, or frozen-hash changes were used.
