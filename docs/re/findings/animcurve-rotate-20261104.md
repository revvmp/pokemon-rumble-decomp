# Rotation-curve continuation, 2026-11-04

## Scope and current evidence

`AnimCurveExecuteRotate` is investigated in private source/object directories. The canonical animation-curve source belongs to the animation-curve coordinator; none of the candidates below is installed or accepted for source linkage. The copied original unit has SHA-256 `214d84b4511c8f10d599c044bd912044f85afc5800d11630d30f2ce1c07e24d2`.

The current private candidate, `tools/local/tail-20261104/smallgame/AnimCurveRotate/rotate_float_access_1/ef_animcurve.cpp`, compiles with the configured Wii/1.1 command to the original 4,312-byte function extent. Its diagnostic objdiff score is `99.8423`. All 1,078 instructions have the original opcode, order, branch shape, and floating-point register operands. Twenty-eight instructions still differ in general-purpose register operands. This is an unfinished function, not a match or an accepted fake.

The private compiler wrapper removes `-MMD`, preserves the unit's exceptions-off and RTTI-off flags, and writes complete objdiff JSON beside every candidate. `rotate_probe.py` freezes the original object outside the shared build tree. `rotate_trace.py` invokes the configured compiler with read-only Frida hooks; the coordinator remains the only writer of the shared build and configuration.

## Timing and key setup

The starting private lead from the animation-curve investigator scored `95.34137`. It used particle/curve/random/loop hash term order, key flags read through their value pointers, and a pointer-to-`u32`-to-pointer command-list identity. These later source controls restore the entire timing prefix and key setup:

- A source-local timing helper writes the process flag through a caller-owned `u32&`. The helper retains the original timing arithmetic and branch behavior.
- Its nested fitting helper writes its period through another caller-owned `u32&`. Declaring that period immediately after the command-list identity removes an anonymous degree-29 node which otherwise takes the header's register.
- The incoming tick is renamed and copied to a named `u32` before the process-flag declaration. The period, mask, key/range/random pointers, and setup locals retain a measured declaration order. This reproduces tick `r31`, seed `r30`, target `r29`, header `r28`, mask `r26`, keys `r25`, ranges `r24`, random table `r23`, and process flag `r22`.
- Cached record flags are explicit `u8` locals declared before interpolation time, with their assignments at the original loads. Naming those cached values allows `rstride`, `stride`, `pKeyData`, `divL`, `divH`, `end`, `pairBytes`, `idx`, `keyLo`, and `keyHi` to take the target setup registers.

`rotate_period_ref_1` first made every instruction through function offset `0x344` exact. `rotate_setup_named_2_0` removed the remaining masked setup differences, and `rotate_saved_restore_15` restored the saved-register assignments while preserving that instruction sequence.

## Range addresses and float allocation

A range address is formed as a byte pointer, explicitly reassigned to its own type, then advanced by four bytes and converted to `CurveRandomRange*`. This preserves the intermediate address's lifetime where a single expression does not. The sign-byte lookup uses a `u32` address view to reproduce the original `lbzx` operand order. An ordinary `bool` for the first random-record condition supplies its target `r21`. These are measured local source-shape controls, not recovered original spelling.

Declaring each range's scale before its base, while retaining the original assignment/load order, fixes the base/scale register pairs. The final double-random loop declares `scaleA`, `baseA`, `scaleB`, `baseB`, `vB`, then `vA` before their assignments. This keeps `vA` in the allocator's high-degree set; the earlier source lost three neighbors before its simplify visit.

The difficult remaining floating-point rotation was the unsigned-conversion bias, final-loop `vA`, and interpolation time. The target assigns them `f0`, `f1`, and `f2`. A returned one-float class with an explicit fieldwise copy constructor moves time from an ordinary named node to a machine-created node appended after instruction selection. That alone gives `vA=f0`, `time=f1`, and `bias=f8`, reaching `99.20686`.

The animation-curve investigator's read-only graph trace identified why the bias still lost priority. Its initial degree is 39, but earlier low-degree neighbors are removed during the ascending simplify scan. Those include the interpolation helper's simple-case weight, polynomial intermediates, coefficients, and caller range values. Wrapping only the polynomial intermediates is insufficient. Wrapping the simple-case weight `w` together with polynomial `s`, `u`, and `v` keeps enough neighbors present when the bias is visited. This restores bias `f0`, `vA` `f1`, and time `f2`, raising Rotate to `99.80519`.

The wrapper's implicit float conversion reverses four commutative multiplication operand pairs. Accessing `w.value` directly in `w.value * (b - a)` restores them without undoing the allocation control. `rotate_float_access_1` consequently has every floating-point operand exact and reaches `99.8423`. The arithmetic, literals, conversion widths, and original instruction count remain unchanged. A final byte/relocation audit and full source-link verification are still required once the remaining integer operands are solved.

## Remaining integer differences

Four differences at offsets `0x714`, `0x724`, `0x72C`, and `0x730` exchange `r4` and `r12` between the second-key byte offset and the first random-flag negation. The pre-allocation trace places the second flag load before the first flag's OR; final scheduling moves the load after that OR. The extra interference exists during allocation even though the final instruction order agrees. Moving the first condition calculation before the second flag load changes cached-flag priorities and regresses unrelated setup instructions; it is not an accepted fix.

The other 24 differences are in the final two-random-key branch. The target assigns the particle multiplier, loop multiplier, curve multiplier, and loaded curve seed to `r22`, `r25`, `r27`, and `r31`. The candidate assigns `r31`, `r22`, `r25`, and `r27`. The subsequent hash-byte shuffle and range-modulo scratch values inherit that rotation. The constants' generated nodes have degrees below 29, so their descending virtual-register order controls this choice.

## Negative controls and traps

- Ordinary time locals, references, direct aggregate initialization, same-type reassignment, and whole-tail helper wrapping do not reproduce the successful returned-class control. An explicit fieldwise copy constructor on the returned class is material; trivial aggregate copies add stack operations.
- Wrapping caller base/scale locals in converting classes, including explicit fieldwise assignment and returned-class factories, leaves their values as ordinary inline temporaries and does not solve the conversion bias. Wrapping the final `vB` adds two native instructions. The successful control concerns the interpolation helper's specific low-degree neighbors.
- Wrapping `s`, `u`, and `v` alone leaves the bias below the simplify threshold. Adding `w` is decisive. Do not infer success from initial degree alone; inspect working degree at the first simplify visit.
- Accessing a wrapped float through its conversion operator can alter a multiplication's operand order even when every register allocation is correct. Direct field access preserves the original pair in this case.
- Unused float-conversion pairs do not reproduce Child's conversion-slot fix here. Whole-tail wrappers, equivalent double intermediates, and inlining the time expression at every call add or reorder instructions.
- All 24 simple final-hash term orders were checked; the original particle/curve/random/loop order remains best. Named literal locals, caller-owned literal references, pointer/enum literal identities, multiplication operand permutations, parameter widths, and argument-order permutations either fold away or regress the candidate. None closes the remaining literal-register cycle.

The scripts `rotate_setup_order.py`, `rotate_time_refs.py`, `rotate_saved_order.py`, `rotate_period_ref.py`, `rotate_cached_values.py`, `rotate_setup_named.py`, `rotate_saved_restore.py`, `rotate_range_identity.py`, `rotate_rand_flag.py`, `rotate_time_return.py`, `rotate_interpolation_more.py`, and `rotate_float_access.py` reproduce the successful sequence. Additional negative-control scripts and complete outputs remain in the same ignored private tree.
