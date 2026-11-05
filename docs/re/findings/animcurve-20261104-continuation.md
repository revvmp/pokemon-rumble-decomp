# Animation-curve continuation, 2026-11-04

## Accepted semantic correction

`detail::InterpolateCurveU8` had the cubic coefficient subtraction reversed. The correct expression is `t * (t * (c0 + c1 - 2.0f))`. The previous expression used `2.0f - (c0 + c1)`. This correction follows the original executable's operand values; it is not a fake match or a claimed exact-function gain.

The frozen `orig/wpse-v1/main.dol` contains `40000000`, or `2.0f`, at virtual address `0x804BA650`, DOL file offset `0x3E65F0`. In the original `AnimCurveExecuteU8` object:

| Function offset | Original instruction | Proven value |
| --- | --- | --- |
| `0x754` | `lfs f8, lbl_804BA650` | `f8 = 2.0f` |
| `0x84C` | `fadds f7, f12, f9` | `f7 = c0 + c1` |
| `0x860` | `fsubs f7, f7, f8` | `f7 = c0 + c1 - 2.0f` |

The coefficient registers come from the two type-bit selections immediately preceding the addition. Their literals are `0.0f` at `0x804BA628` and `1.5f` at `0x804BA654`. The other polynomial constants are `3.0f` at `0x804BA64C` and `-2.0f` at `0x804BA658`. The float interpolation helper already uses the correct subtraction order.

An isolated include overlay containing only this correction preserves all eight function scores, including the three exact functions. The coordinator applied the one-line correction to `include/nw4r/ef/ef_animcurveinternal.h`; acceptance of the entire source unit remains outstanding because five executors are nonexact.

## Texture executor: exact isolated function

`AnimCurveExecuteTexture` now reproduces all 1,492 original function bytes. The direct-compiler output and copied original function both hash to SHA-256 `b6f1ff273d4d2cc6c15ffa1d0bdd27bec99657c617ee5a482d264936d7568778`. All 373 instruction words, including branch displacements, agree. Relocation offsets, kinds, and destinations agree after resolving local data targets to their section offsets. The unit's `.data` and `.sdata2` bytes also agree. The three previously exact functions remain exact; the other four executor scores are unchanged.

The retained source keeps the original C++ signature. It uses an explicit pointer-to-`u32`-to-pointer identity for the local command-list pointer and expands the frame calculation in the caller. The frame calculation names the infinite-loop mask and declares its fitted-period local before the arithmetic. These are documented D-0016 controls, not a claim that this spelling is original source. The pointer identity is lossless for the target's 32-bit pointer ABI.

The honest attempts included ordinary pointer copies, pointer and enum identities, a `void*` reassignment, helper wrappers at several inline depths, parameter and declaration permutations, and the available nearby compiler revisions. The allocator trace explains the remaining obstacle: the incoming pointer copy disappeared before allocation, while the fitted period retained interference degree 29 and was coloured ahead of the seed parameter. The pointer identity preserves a named local once the frame calculation is expanded. Moving the fitted-period declaration earlier changes the simplify ordering enough to assign it the target register. An intermediate candidate had only the seed/tick register pair reversed; the declaration move closes that difference without adding instructions.

This is an exact-function gain, from three to four of the eight functions. The unit remains `NonMatching` while U8, F32, Rotate, and Child are unfinished. Full-build acceptance remains the coordinator's responsibility.

## Baseline and scratch results

Direct native compilation uses the configured Wii/1.1 command, including exceptions and RTTI off. The starting exact functions remain `CreateCurveChild`, `AnimCurveExecuteBakedU8`, and `AnimCurveExecuteBakedF32`. The five starting diagnostic scores are U8 `93.206955`, F32 `94.47577`, Rotate `93.434135`, Texture `99.61126`, and Child `96.30681`.

The current scratch U8 lead has the original 4,832-byte extent, score `97.14239`, six differences after masking register numbers and aligning instruction sequences, and remaining register differences. It combines these investigated source controls:

- An unsigned word to 64-bit integer to byte conversion preserves byte masks which the compiler otherwise deletes after the saturation checks. A signed intermediate adds unwanted sign-extension instructions.
- Reading key flags through the value pointer's containing record restores the target's flag and pointer scheduling. Computing the same read through the separately named record pointer does not.
- The hash term order `particleSeed`, `curveSeed`, `randomIndex`, `loop` fixes the second random hash's instruction structure. Two short scheduling groups remain around random-range selection and the next hash.
- One additional inline forwarding level around byte interpolation restores the final cubic branch's conversion schedule. Two additional levels reverse that gain; three reproduce it.
- Expanding the timing helpers and ordering their surviving named declarations restores every initial saved-register assignment. Moving the `pValuesA` declaration before the interpolation-time local also restores their volatile-register order. Floating-point constant and value allocation remains unfinished.

These controls are not retained in tracked source, and no exact-match or source-selection credit is claimed for them. They are candidates for continued investigation, not accepted fake matches.

The Child investigation reproduces the earlier mutable-header lead. An output-reference frame helper reaches `97.90427`; changing its argument order does not close it. Its second frame calculation still reverses the two integer-to-float conversion scratch slots relative to the target. A full-width bitfield used during the earlier Texture investigation retained a local but added instructions and was rejected.

Compiler revision probes do not justify a toolchain change. Wii/1.0, 1.1, 1.3, 1.5, and 1.6 produce the same baseline scores under the configured flags; the other installed revisions tested regress at least one function. No configuration, assembly, object code, expected hash, or original input was changed by these experiments.

## Reproduction artifacts

Private scripts and outputs are under `tools/local/tail-20261104/`. `animcurve_literals.py` reads the seven relevant float literals directly from the original DOL. `animcurve_correct_polynomial.py` builds the isolated corrected include overlay. `animcurve_diff.py` prints both register-sensitive and masked instruction comparisons. `animcurve_trace.py` uses the real configured compiler flags with the repository allocator trace hook.

The retained Texture candidate is `nw4r_ef_ef_animcurve/animcurve_texture_accepted/ef_animcurve.cpp`, with direct-compiler `diff.json` and byte/relocation evidence in `evidence.json`; `animcurve_texture_accept.py` reproduces the final cleanup and comparison. The current U8 scratch candidate is `nw4r_ef_ef_animcurve/animcurve_hash_tree_1_13/ef_animcurve.cpp`. The Child candidate is `nw4r_ef_ef_animcurve/animcurve_child_frameoutput_0_0/ef_animcurve.cpp`. The copied original object used during the coordinator's clean builds is `nw4r_ef_ef_animcurve/animcurve-original.o`, SHA-256 `214d84b4511c8f10d599c044bd912044f85afc5800d11630d30f2ce1c07e24d2`.

The current public [ogws animation-curve work](https://github.com/doldecomp/ogws/blob/a807c2c27f65718458f34d436f66769c92d307ce/src/nw4r/ef/ef_animcurve.cpp) was inspected as prior art. That revision uses a different channel loop and leaves four executors undecompiled; its implementation was not imported. Its polynomial agrees with the operand-based correction above.

## Traps

- **Masked register comparisons can conceal reversed subtraction.** U8's incorrect `2.0f - (c0 + c1)` had the same opcode sequence and fuzzy score as the correct expression because both operand registers already differed. Track the values feeding noncommutative operations and verify their relocation-backed literals before treating a remaining operand difference as allocation alone.
- **An identity control depends on the surrounding inline structure.** The `void*` reassignment which retains a computed receiver elsewhere did not repair Texture's original helper-based form. After expanding the frame calculation, several pointer identities preserve the required local, and moving the fitted-period declaration closes the final register pair. Check surviving nodes and simplify order before treating an earlier negative identity experiment as universal.
