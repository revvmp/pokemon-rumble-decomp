# U8 animation-curve integer continuation, 2026-11-04

The private `selector_u8_product_2` candidate raises `AnimCurveExecuteU8` from 98.54967% to 99.7351%. It retains the original 4,832-byte extent and all 1,208 instruction positions. Register-masked comparison has no differences, and every floating-point instruction is exact. There are still 64 integer instruction-word differences, so this is an unfinished source candidate and earns no exact-function or source-link credit. A second exploratory candidate, `selector_u8_type_half`, uses a u16 interpolation-type parameter and reaches 99.7558% with 55 integer instruction-word differences; its exact original internal parameter type is unknown.

The source and object directories are under `tools/local/tail-20261104/nw4r_ef_ef_animcurve`. Each final candidate includes `integer-evidence.json` with counts and its source hash. The original private object is `animcurve-original.o` in that directory. Copy only the helper/function region beginning at `inline u32 HashCurveExperiment` and ending immediately before `AnimCurveExecuteF32`; these snapshots predate the accepted Child implementation. The native compiler wrapper is `tools/local/tail-20261104/animcurve_probe.py`. Every investigation uses a distinct `selector_u8_*` directory and the private original, never a shared build output. The animation-curve owner combines the candidate with the current five accepted functions.

## Retained private improvements

The final two-random-key branch declares its surviving values in this order: `rA`, `hashA`, `rB`, `hashB`, `pRangeB`, `pRangeA`. Their assignments remain at their original operations. This restores both late hash scheduling groups, as well as the two range pointers and the second hash register. The `selector_u8_tail_decl0` score is 99.05629%, with no remaining masked instruction differences.

Declaring the final branch's `vA` before its channel loop gives the value the target r0 and moves the channel bit to r10. Declaring scale before base, while leaving the loads in their original order, restores the random-byte operand registers. The exact-key branch similarly declares scale and base before the range pointer and bit. In the final branch, named `randomMode = pHeader->processFlag & 4` is reused for the second seed update after the two second-range byte declarations. It reproduces the original single flag read and cached r17 value; the old source already eliminated its repeated read through common-subexpression analysis.

A named `const u8* rangeAddress` followed by `rangeAddress = static_cast<const u8*>(rangeAddress)` before adding the four-byte record prefix preserves the original address temporary separately from the final range pointer. Merely introducing the local, without its reassignment, does not. The scoped controls restore all five affected range-address constructions without adding operations.

The local `FindByteCurveKeys` helper is the existing `detail::FindCurveKeys` body with its incoming lower bound named `lower` and a local `s32 lo = lower` declared before `mid`. This restores the key-search mid/lower registers r5/r6. Declaring `lo` after `mid` loses the correction; a caller-owned mutable lower bound also works but is unnecessary here.

The first exact-key branch can use its value pointer directly and read the flag ten bytes before it through `*(pValues - 10)`. Removing its redundant separately named record pointer restores the common base/value register without changing any address or read. Together these controls yield the `selector_u8_record_1` candidate.

These source-shape controls are prospective D-0016 accommodations. They are not accepted fake matches until the function and full unit receive the required exact evidence.

## Remaining interpolation operands and rejected directions

Every remaining integer mismatch lies inside the four interpolation expansions: the start/type register choice and, in the random-input branches, the register shared by the returned byte and its source input. Every other integer path, including key search, range selection, hash generation, cached flags, and channel loops, is exact.

The intermediate `selector_u8_record_1` candidate had 56 float differences: normalized time used f12 and coefficient c0 used f13, while the target uses f13 and f12 respectively; four smoothstep products also had their operands exchanged. These are now resolved. Writing normalization as `f32 t = (f32)tick; t = t / 65536.0f;` restores all 52 exchanged time/coefficient operands. The compound assignment `t /= 65536.0f` does not produce the same allocation. Reading the existing wrapper field explicitly as `w.value * (f32)(int)(e - s)` restores the four smoothstep product operand orders. A separate scalar product with compound multiplication or an ordinary inline two-float multiply helper also works, but the field read is the smallest retained control. The remaining `CurveFloat` definitions are unchanged.

The private probes cover parameter/declaration order, named input and return locals, const/mutable references, narrow/wide parameter views, same-type casts, short enums, scalar classes, output forwarding, in-place interpolation, and named return joins. No complete improvement resulted. Widening only the interpolation type to u16 reduces the final remaining integer differences from 64 to 55, but changes the input cycle in another expansion. Both final views are retained privately; neither is exact. In-place interpolation removes required byte masks and changes branch/alignment bytes, so its superficially close score is not an acceptable replacement.

Scalar float declaration moves, coefficient forwarding helpers, and return-reference variations of `CurveFloat` optimize identically. Giving normalized time caller-owned output-reference storage creates a held f31 live range and increases the float mismatches from 56 to 184. Changing the float wrapper's constructor to take a const reference has a similar regression. Extra byte classes and bitfield views either introduce masked instruction changes or fail to improve operand allocation.

## Traps

- An ordinary pointer local can still coalesce with its destination. In this function the same-type reassignment, rather than the initializer cast, preserves the already-evidenced temporary address register.
- An instruction-sequence match is not byte identity. The retained U8 candidate has no register-masked or floating-point differences but still differs in integer register operands; it remains unfinished.
- Full reassignment and compound assignment are not interchangeable allocation controls. Splitting float conversion from division with `t = t / 65536.0f` fixes 52 operands here, while `t /= 65536.0f` does not.
- A mutable output reference can extend a floating-point live range substantially. Here it causes f31 allocation and widespread operand changes even though the instruction count remains 1,208.
