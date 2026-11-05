# Rotation-curve continuation, 2026-11-04

## Scope and current evidence

`AnimCurveExecuteRotate` at `0x8022B350` now reproduces all 4,312 original bytes. Its function SHA-256 is `e3368a5842392b0f088e120b069b51286614f6d7d48d958d7edf24149ca9f14a`. This is an exact-function result; the animation-curve unit still requires its remaining U8/F32 bodies and full source-link acceptance. The copied original unit has SHA-256 `214d84b4511c8f10d599c044bd912044f85afc5800d11630d30f2ce1c07e24d2`.

The final reviewed fragment is `tools/local/tail-20261104/smallgame/AnimCurveRotate/rotate_accepted/rotate_section.cpp`. `rotate_accept.py` installs that fragment into a private copy of the current canonical source, compiles both control and candidate, and writes `acceptance.json` beside the fragment. The canonical owner received this narrow fragment for integration; the coordinator owns normal-build acceptance.

| Audit | Result |
| --- | --- |
| Native function bytes | All 4,312 bytes, including all branch displacements, equal the original |
| Relocations | All 58 relative offsets, types, addends, callees, and literal destinations agree |
| Other seven functions | Raw bytes and relocation records unchanged from canonical control |
| Retained data | Original `.data` 512 bytes and `.sdata2` 56 bytes reproduced exactly |
| Emitted functions | Same eight functions; no additional helper bodies |
| Section changes | `.text` remains 21,740 bytes; only its 930 corrected bytes and 33 generated-name bytes in `.strtab` change |
| Fragment SHA-256 | `7c90b0f9d9ed9fda7f275097a5f8ca969ca428f813f84637cdfe249b92aab542` |
| Complete candidate source SHA-256 | `1c0753f022bdddca9c1319978298333e1b704ec7ea23ef890628ae8098c3c59f` |
| Canonical control source SHA-256 | `1b4060a29a6b692ac65aef1dbb8ff5accce3d470619a7e257d215c711b5bd992` |

The private compiler wrapper removes `-MMD`, preserves the unit's exceptions-off and RTTI-off flags, and writes complete objdiff JSON beside every candidate. `rotate_probe.py` freezes the original object outside the shared build tree. `rotate_trace.py` invokes the configured compiler with read-only Frida hooks; the coordinator remains the only writer of the shared build and configuration.

## Timing and key setup

The starting private lead from the animation-curve investigator scored `95.34137`. It used particle/curve/random/loop hash term order, key flags read through their value pointers, and a pointer-to-`u32`-to-pointer command-list identity. These intermediate source controls restored the entire timing prefix and key setup:

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

The wrapper's implicit float conversion reverses four commutative multiplication operand pairs. Accessing its stored value directly in `w.mValue * (b - a)` restores them without undoing the allocation control. `rotate_float_access_1` consequently had every floating-point operand exact and reached `99.8423`. The final cleanup uses one `RotationCurveValue` class for time and all four interpolation values. The arithmetic, literals, conversion widths, and original instruction count remain unchanged; the raw-byte and relocation audit above verifies the final result.

## Closing the integer differences

Four differences at offsets `0x714`, `0x724`, `0x72C`, and `0x730` exchanged `r4` and `r12` between the second-key byte offset and the first random-flag negation. The pre-allocation trace placed the second flag load before the first flag's OR; final scheduling moved the load after that OR. Moving the condition calculation earlier regressed unrelated setup instructions. Reading the first cached flag as `pRecordA->mFlags`, while retaining the value-pointer-minus-ten view for the second flag, fixes the pair without moving source statements. This reaches `99.860855`.

The last 24 differences were in the final two-random-key branch. The target assigns the particle multiplier, loop multiplier, curve multiplier, and loaded curve seed to `r22`, `r25`, `r27`, and `r31`; the candidate assigned `r31`, `r22`, `r25`, and `r27`. The subsequent hash-byte shuffle and range-modulo scratch values inherited that rotation. Direct literal and hash-expression changes did not solve it.

The decisive clue is when the allocator opens `r22`. In the target, the particle multiplier uses `r22` only twice, and later hash scratch values never reuse it. The candidate's process flag is a degree-31 high-priority node which opens `r22` before all those later expression nodes are colored. The target instead needs `r22` opened by the particle multiplier after the other hash temporaries have already been colored.

A read-only graph capture confirms that the flag's first simplify visit sees degree 31. Eight low-degree neighbors disappear later in that pass: the normal period, two flag masks, two loop-count loads, `life - 1`, the fitted-period `xoris`, and the fitted-loop product. In collaboration with the animation-curve investigator, caller-owned timing values made enough of those neighbors simplify before the flag. The final retained source keeps only the fitted period, fitted-loop count, and loop-period product as caller locals. The count's explicit same-type reassignment and the product's compound multiplication preserve those named nodes. Their order is `loopPeriod`, `fittingCount`, `fittingPeriod`, followed by the mask and resource pointers.

The process flag returns to an ordinary local `u8` inside `CalcRotationCurveTime`. It is now low-degree but retains inline-variable priority, so it is colored after the hash temporaries and before later inline flags. The original normal period and turning-loop count are also restored as ordinary helper locals/loads. An intermediate caller-owned `u16` turning count reached 100%, but cleanup proved it and the caller normal-period reference unnecessary; neither remains in the final fragment.

## Retained D-0016 controls

The local helpers and one-float value class are documented source-shape controls after the honest variants above failed. Retained controls are the command-list pointer identity; measured declaration order and named incoming tick; fitted-period/count/product output references with same-type count reassignment and compound multiplication; distinct record/value-pointer flag views; byte-pointer range-address reassignment and the integer address view for indexed sign loads; `bool` for the first random condition; the explicit fieldwise float copy constructor; and wrapped interpolation time/weight/polynomial values. The unsigned seed expression uses particle/curve/random/loop order with unchanged modulo-32-bit arithmetic.

No inline assembly, embedded instruction bytes, object rewriting, new original-data dependency, or compiler change participates. All casts rely on the evidenced 32-bit target ABI; the fieldwise float copies preserve the original single-precision arithmetic. The canonical owner and coordinator must retain this entry in the central fake-match ledger when integrating the function.

## Negative controls and traps

- Ordinary time locals, references, direct aggregate initialization, same-type reassignment, and whole-tail helper wrapping do not reproduce the successful returned-class control. An explicit fieldwise copy constructor on the returned class is material; trivial aggregate copies add stack operations.
- Wrapping caller base/scale locals in converting classes, including explicit fieldwise assignment and returned-class factories, leaves their values as ordinary inline temporaries and does not solve the conversion bias. Wrapping the final `vB` adds two native instructions. The successful control concerns the interpolation helper's specific low-degree neighbors.
- Wrapping `s`, `u`, and `v` alone leaves the bias below the simplify threshold. Adding `w` is decisive. Do not infer success from initial degree alone; inspect working degree at the first simplify visit.
- Accessing a wrapped float through its conversion operator can alter a multiplication's operand order even when every register allocation is correct. Direct field access preserves the original pair in this case.
- Unused float-conversion pairs do not reproduce Child's conversion-slot fix here. Whole-tail wrappers, equivalent double intermediates, and inlining the time expression at every call add or reorder instructions.
- All 24 simple final-hash term orders were checked; the original particle/curve/random/loop order remains best. Named literal locals, caller-owned literal references, pointer/enum literal identities, multiplication operand permutations, parameter widths, and argument-order permutations either fold away or regress the candidate. The cause was the timing flag opening a register early, not a hash literal's own expression order.
- A late register cycle can come from the order in which a different branch opens nonvolatile registers. The timing flag is absent from the final hash branch, yet lowering its working degree fixes all 24 hash operands. Inspect the first opening of the discrepant physical register before rewriting already-correct arithmetic.
- Controls which were necessary in an intermediate allocation can become redundant after other lifetimes change. Removing the caller normal-period and turning-count references together preserves the final 100% result and restores the ordinary helper source.

The scripts `rotate_setup_order.py`, `rotate_time_refs.py`, `rotate_saved_order.py`, `rotate_period_ref.py`, `rotate_cached_values.py`, `rotate_setup_named.py`, `rotate_saved_restore.py`, `rotate_range_identity.py`, `rotate_rand_flag.py`, `rotate_time_return.py`, `rotate_interpolation_more.py`, and `rotate_float_access.py` reproduce the initial sequence. `rotate_flag_load_shapes.py`, `rotate_flag_low.py`, `rotate_flag_low_inline.py`, `rotate_turn_count.py`, `rotate_minimize_neighbors.py`, and `rotate_accept.py` reproduce the closure and final audit. `rotate_gpr_graph_hook.js` and the shared `animcurve_graph_sim.py` capture and simulate the working-degree evidence. Additional negative controls and complete outputs remain in the same ignored private tree.
