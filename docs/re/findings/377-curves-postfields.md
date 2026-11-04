# F-377-1: Effect curve and post-field boundaries

The initial post-field candidate status below is historical: PR #1004 accepted that entire unit from source; see [F-768-1](768-postfield-particle.md). F-377-2 records the current animation candidate.

Issue #377 owns the first retained functions of two translation units. The earlier survey boundary through `0x8022F4F4` divides the post-field unit. The preceding four emitter functions already belong to the verified emitter unit from #737.

| Unit | Native sections | Retained functions |
| --- | --- | --- |
| `nw4r/ef/ef_animcurve.cpp` | `.text` `802290A0-8022DFAC`; `.data` `803C5378-803C5578`; `.sdata2` `804BA628-804BA660` | Eight, all owned by #377 |
| `nw4r/ef/ef_postfield.cpp` | `.text` `8022DFB0-80231640`; `.sdata2` `804BA660-804BA698` | Fourteen: nine from #377, four from #767, one from #768 |

Both splits pass the real configure/Ninja/frozen-manifest pipeline without changing `main.dol`. The animation unit remains an original-only split; no placeholder implementation or source credit is introduced. Its two 256-byte tables precede the particle vtable. The post-field unit is registered `NonMatching` and still selects its original object.

## Evidence and provenance

The target is the immutable `wpse-v1` executable. Gekko disassembly, split-object relocation records, native data words, and the verified ELF establish the boundaries and algorithms. `0x8022D910` and `0x8022DC60` are baked U8 and F32 curve executors; they are not post-field geometry routines. Their channel lookup references belong to the animation unit. The sphere classifier at `0x8022DFB0` starts the post-field unit. The last post-field particle step ends exactly at the first particle method, `0x80231640`.

The algorithm reference is [InusualZ/mhtri-decomp at ac489be692a1273afff8574b2402aecba2ae2b8f](https://github.com/InusualZ/mhtri-decomp/tree/ac489be692a1273afff8574b2402aecba2ae2b8f), `src/ef/ef_postfield.cpp` and `src/ef/ef_animcurve.cpp`, under CC0. Its debug assertions, procedural wrapper types, compiler pragmas, offset scaffolding, and implementation names are not imported. The reconstruction uses the canonical C++ math and emitter interfaces. Original mangled classifier/crossing names already present in this target's symbol map are retained; `GetTypePositionSphere` is inferred from its behavior and the other five classifiers, with medium naming confidence. It is not claimed as a recovered string.

## Candidate source

| Address | Function | Bytes | Candidate result |
| --- | --- | --- | --- |
| `8022DFB0` | `GetTypePositionSphere` | 284 | Exact |
| `8022E0D0` | `GetTypePositionCube` | 224 | Exact |
| `8022E1B0` | `GetTypePositionCylinder` | 200 | Exact |
| `8022E280` | `GetTypePositionCircle` | 260 | Exact |
| `8022E390` | `GetTypePositionRectangle` | 232 | Exact |
| `8022E480` | `GetTypePositionPlane` | 168 | Exact |
| `8022E530` | `crossCheckPlane` | 1172 | Exact |
| `8022E9D0` | `crossCheckRectangle` | 1436 | Exact |
| `8022EF70` | `crossCheckCircle` | 1412 | Exact |

All nine compiled bodies, 5,388 instruction bytes, agree byte for byte with their original split-object bodies. All 148 paired text relocations have equal resolved target addresses, including the inline math and the matrix-vector call. The candidate's first 28 constant bytes are exactly the native constant prefix: minus one, one, zero, no-crossing two, float epsilon, one plus one ULP, and one plus two ULPs. This goes beyond a fuzzy score, but is candidate evidence: the object remains unlinked until all fourteen routines and all 56 native constant bytes are reconstructed. No new source coverage is claimed.

`PostFieldInfo` is a 72-byte record at four-byte alignment. The first three vectors occupy `0x00`, `0x0C`, and `0x18`; the particle-step reference establishes their scale/rotation/translation roles. Speed limit is at `0x24`, speed mode at `0x28`, shape at `0x29`, axis/hemisphere options at `0x2A`, collision mode at `0x2B`, flags at `0x2C`, start age at `0x2E`, collision scale at `0x30`, and the existing ten-byte `EmitterInheritSetting` at `0x3C`, followed by alignment. Member spellings are descriptive. The nine routines directly prove the option and flag offsets; the remaining layout follows the referenced particle-step target accesses and is not claimed as original naming.

Sphere/cube/cylinder return 3 outside, 1 on the surface, and 2 inside. Circle/rectangle use 4 for points in their plane outside the shape and 3 for points off the plane. Plane returns 2 above, 3 below, and 1 on it. Crossing routines return the segment parameter on a hit or 2.0 otherwise. Reflection multiplies the selected velocity and direction components by minus one, then nudges the hit coordinate by float epsilon before transforming the hit point.

## Validation and remaining work

`python configure.py --map`, `.venv/Scripts/ninja`, and the frozen-manifest `verify.py verify` command all exit 0. `tools/re/unitdiff.py nw4r/ef/ef_postfield.cpp` reports nine exact functions. An independent temporary ELF audit checks the nine raw bodies, their symbol sizes, paired resolved relocations, the 28-byte data prefix, actual fallback dependency selection, and complete frozen DOL identity; it exits 0. These checks must be repeated on the exact rebased merge head.

#377 remains open: all eight animation routines need source reconstruction, and the post-field unit also needs the five routines owned by #767 and #768 before source selection. This change establishes the owning split and canonical header for those neighboring bodies; it does not implement their functions. Append their definitions in address order to this file through their own claims. Keep this unit `NonMatching` until complete.

## Traps

- If baked executors at `8022D910`/`8022DC60` are treated as geometry, the post-field split starts too early and captures unrelated lookup data. Their byte-mask lookup and channel copies prove they end the animation unit instead.
- The fuller CC0 reference is under `src/ef/`, outside an older sparse `src/nw4r/ef/` checkout. Inspect the pinned tree before concluding implementation evidence is absent.
- A function-scope crossing parameter initialized to 2.0f before the axis switch preserves the native constant order. Initialize that same parameter and overwrite it on a hit; do not insert unused padding data or a second constant object. Returning a mutable initializer directly from the default path instead makes this compiler keep it live across the switch, hoist its load, and change three functions' register allocation. Local const copies add separate data words. All accepted functions retain the established NW4R flags.
- Importing the reference's `peephole off` flag damages eight of the nine candidate functions. It was tested and rejected; the target's reduced debug checks are not evidence for importing debug build flags.

# F-377-2: Complete animation candidate and remaining compiler differences

All eight native animation routines now have C++ candidate bodies in `nw4r/ef/ef_animcurve.cpp`, registered `NonMatching`. The build continues to select its original object. The complete post-field unit from F-377-1 is separately source selected through #1004. Thus thirteen of the original twenty-one #377 functions, 7,040 instruction bytes, are already accepted: four emitter methods/1,652 bytes and nine post-field routines/5,388 bytes. The animation unit's 20,192 instruction bytes remain unaccepted as a whole.

| Native address | Function | Native bytes | Candidate score |
| --- | --- | --- | --- |
| `802290A0` | `AnimCurveExecuteU8` | 4832 | 92.74% |
| `8022A380` | `AnimCurveExecuteF32` | 4044 | 93.41% |
| `8022B350` | `AnimCurveExecuteRotate` | 4312 | 93.05% |
| `8022C430` | `AnimCurveExecuteTexture` | 1492 | 97.09% |
| `8022CA10` | `detail::CreateCurveChild` | 356 | 100% |
| `8022CB80` | `AnimCurveExecuteChild` | 3468 | 91.67% |
| `8022D910` | `AnimCurveExecuteBakedU8` | 844 | 99.27% |
| `8022DC60` | `AnimCurveExecuteBakedF32` | 844 | 99.27% |

These are diagnostic scores, not acceptance. The current candidate text extent is 20,236 bytes, the same as the native extent including alignment; this does not prove any differing body. The independent audit proves the child-creation helper's complete 356-byte body and both paired resolved external call targets exact. Both 256-byte channel tables are byte exact. The other seven routines and the constant pool remain different, so no animation source credit is claimed.

## Source, layouts and behavior

The source is adapted from the CC0 revision and files pinned in F-377-1. Its algorithms are rewritten against the canonical AnimCurveHeader, Particle, ParticleParameter, EmitterInheritSetting, effect/system and creation-queue types. The procedural particle/manager wrappers, raw-offset fake classes, debug pointer checks, reference pragmas and unused texture ramp/latch routines are not imported. No decompiler output or executable bytes are embedded. The two channel tables are reconstructed as population count and highest set bit over each possible byte.

The private header `ef_animcurveinternal.h` declares the binary records and inline evaluation helpers. Value keys have a 12-byte header (frame16, flags8, reserved8, eight interpolation bytes), followed by byte or float values or a random index. A byte key's stride is the header plus enabled-channel bytes rounded to two; float keys use four bytes per enabled channel. Random byte pairs are two bytes, random float pairs eight. Rotation random records append a four-byte sign control after their float pairs. These layouts are descriptive typed views of the native variable-length records.

Texture keys are four bytes: wrap value8, reverse/type8, name index16. A texture key record is 16 bytes: frame16, flags8, nine reserved bytes, and a four-byte key/random-index union. Random texture records are eight bytes: the key, mode8 and three reserved bytes. Child settings are 12 bytes: the existing ten-byte inheritance setting and a two-byte name index. Child key records are 24 bytes: the same twelve-byte record prefix and a twelve-byte setting/random-index union. Key and random tables have a four-byte count/reserved prefix. The existing AnimCurveNameTable supplies four-byte resolved name entries. Field names are descriptive; this does not claim recovered original spellings.

The keyed evaluators resolve a frame and loop index, find the adjacent keys, select direct or randomized endpoints and interpolate enabled channels. U8 interpolation uses 16.16 fixed-point time and clamps random/interpolated results. Float interpolation supports linear, cubic and held values. Rotation also draws a sign choice. Texture curves select one of the three canonical texture slots using header kind bytes `h`, `l` or `p`, resolve the named texture and replace the corresponding wrap/reverse bit groups. The target takes five arguments; the reading reference's extra output parameters have no target equivalent and are omitted.

Child curves walk keys crossed between the current tick and the next tick, including loop and ping-pong boundaries. The retained helper resolves an inline or randomized inheritance setting and queues particle/emitter creation through the actual manager chain. It uses Particle::mCalcRemain at `0xE2`, not the reference wrapper's lifetime label. The two baked routines copy packed enabled channels after resolving clamped, looping, ping-pong or lifetime-fitted frames. They take four arguments, with life in `r6`, as proved by their native callers.

The earlier working hypothesis that `8022CA10` was a texture-ramp helper is rejected: its two native callees are AddParticleCreation and AddEmitterCreation, and the child executor calls it repeatedly. The exact C++ definition confirms its child-creation role. `CreateCurveChild`, the two Baked suffix names and channel-table names are descriptive, low naming confidence. Existing imported public executor spellings remain unchanged.

## Constant ordering and controlled experiments

The native 56-byte pool starts with float zero, four alignment bytes, unsigned and signed conversion biases, then 1.0f, float epsilon, 65536.0f, 3.0f, 2.0f, 1.5f, -2.0f and 255.0f. The 52-byte candidate instead places zero,epsilon, 65536,3,2,1.5,-2,255 first, then the two conversion biases and 1. No fabricated constants, padding words or hash changes are added to repair that ordering.

Making both frame helpers ordinary functions reproduces all 56 constant bytes, but retains unwanted calls and drops the keyed-body scores sharply. Keeping them inline gives the target's inlined algorithms but the wrong constant order. Making all helpers ordinary or static also retains unwanted helper calls. These experiments distinguish a compiler/source-structure problem; neither outcome is accepted. Adding prior ordinary prototypes does not fix it. `-inline auto,deferred` gives the same respective outcomes. `-ipa off` keeps the wrong pool and makes the texture/child bodies worse. All flag experiments were reverted; the candidate retains unchanged Wii/1.1 NW4R flags.

Factoring frame selection into a small baked wrapper makes file-wide analysis inline the wrapper into the U8/F32 keyed entries, unlike the native calls. Explicit inline baked definitions also disappear into their callers. The current ordinary baked bodies keep frame selection in their own bodies and preserve those target calls without compiler pragmas. Reuse of the frame/flag snapshots and a 32-bit enabled-channel count removes several repeated native-absent operations. The key-exact result uses BOOL to avoid an extra byte narrowing; changing final flags between byte/bool/BOOL gives no improvement. An offsetof-based payload-to-record accessor did not improve code and was removed.

Remaining work includes constant ordering, scratch/callee-saved register allocation, loop and pointer lifetimes, and the remaining source differences revealed by exact instruction comparison. The candidate's matching text size is not a reason to dismiss these differences as cosmetic. Complete raw bodies, paired relocations, native positions and a source-selected full DOL are still required.

## Validation

The normal configure/Ninja/frozen-manifest verification pipeline exits 0 with original animation fallback selected. A separate temporary audit requires all eight candidate definitions, compares the one exact helper body and its resolved call targets, verifies all 512 table bytes, confirms the constant section is still different, confirms the real Ninja input selects the original object and excludes the candidate, and compares the complete DOL to the immutable original. This records partial evidence without conflating fallback identity with source acceptance.

## Additional traps

- `nw4r::ef::FAbs(f32)` and `math::FAbs` do not produce the same instructions here. The former uses the double intrinsic followed by float rounding, matching the native `fabs`/`frsp` pair in fractional-key comparison. The paired-single-free math inline omits the rounding instruction. Use the existing EF helper rather than altering shared math headers.
- A matching aggregate text size does not establish matched instructions or constant placement. The candidate reached the native 20,236-byte extent while seven bodies and the constant pool still differed.
- Frame helpers with different inline status can produce opposite pieces of a match: ordinary helpers give the correct pool but wrong calls; inline helpers give the expected expansion but wrong pool. Record both dimensions before accepting a compiler experiment.
- The reading reference's particle wrapper names its `0xE2` field as lifetime. Canonical layout and native child-creation calls prove calcRemain; keep the actual C++ member and its ABI.

# F-377-3: Curve-time APIs explain the native constant order

The later primary reference [doldecomp/forecast at f790c8d49a3e5428a095f84b3794f2fd7d8bc2e5](https://github.com/doldecomp/forecast/blob/f790c8d49a3e5428a095f84b3794f2fd7d8bc2e5/src/nw4r/ef/ef_animcurve.cpp), under its pinned CC0 license, defines the public `AnimCurveCalcTime` and `AnimCurveCalcFrameStep` APIs before the executors. Its source blob is `e86f02c3d42ac3eee69163c3403ecbd940e276ca`. The same APIs occur in the CC0 [wii-news-channel reference at 128e3ffbaee9e70bd175c5f66a44fb8430068629](https://github.com/hotlandsoftware/wii-news-channel/blob/128e3ffbaee9e70bd175c5f66a44fb8430068629/src/nw4r/ef/ef_animcurve.cpp), source blob `c0d40205fa374a7ea5aa71962e035efb846b5eac`. Both pinned root license files were inspected before reuse. The incomplete Rhae implementation was inspected but not reused.

Defining these real APIs before the executors reproduces all 56 native constant bytes, including the conversion biases and their alignment, with unchanged compiler flags. This supersedes F-377-2's unresolved constant-order observation. No filler objects or explicit padding are introduced. The APIs have no entries among this target's eight retained native functions; their candidate bodies receive no source credit. The animation object remains excluded from the real link, so their eventual dead stripping and retained-function positions must still be proved during source acceptance.

The private frame helpers now use the reference's return values and output references rather than pointer-output adapters. Native differences require a lifetime-one guard in floating fitting and 16-bit frame results where this older resource revision narrows to eight bits. Its key records have an eight-byte prefix and packed interpolation bits, whereas this target proves a twelve-byte prefix and eight interpolation bytes; those older layouts are not imported. The child's first end flag is initialized false, as in the reference, before the helper can set it at a final turn boundary. A 32-bit cached step flag avoids several register differences. The float helper retains its byte flag.

At this checkpoint, the two channel tables (512 bytes), full constant section (56 bytes), and child-creation helper (356 instruction bytes and both resolved call targets) are exact. The eight retained candidate functions contain 20,168 instruction bytes versus 20,192 native instruction bytes; individual acceptance is still required. Diagnostic scores are U8 93.21%, F32 94.48%, Rotate 93.43%, Texture 99.61%, Child 96.31%, and both baked executors 99.41%. These percentages do not establish equivalent behavior or matching. Texture and baked differences in the inspected complete diffs are register choices; the keyed and child differences still require instruction and control-flow review.

The normal configure/Ninja/frozen-manifest pipeline and the independent audit are rerun on the merge head. The audit requires all eight retained candidates, the exact helper body and paired targets, all 568 native data bytes, original-only animation dependency selection and immutable complete DOL identity. No animation source credit is added; #377 remains open.

## Traps

- When inline helpers reproduce instructions but reorder constants, check whether real public APIs were discarded before the retained native functions. Here two evidenced curve-time APIs restore the complete pool. Changing inline flags alone had failed; do not insert fake padding or constants to repair the symptom.
- The Forecast reference's step helper only sets its end output on particular terminal paths. Initialize the child's first end flag false in the caller, as the reference does; the second flag is not consumed. Do not assume every output is assigned on every path.
- Changing the step header parameter to mutable through a const cast temporarily raises the child score, but changes alias analysis and still leaves unmatched instructions. The const-cast variant, ordinary/deferred baked wrappers, mutable-header variants and a return-valued key-search rewrite are not retained as evidence of a match.

## Bounded reference controls, 2026-10-20

The current complete unit was rebuilt through `unitdiff.py` on current main. It retains the F-377-3 results: one exact child-creation helper, seven differing native bodies, 512 exact table bytes and 56 exact constant bytes. Texture remains 1,492 native bytes at 99.61%; both baked bodies remain 844 native bytes at 99.41%. These are diagnostic scores, not source acceptance.

Four controls used the already pinned Forecast source to inspect remaining source-structure and local-value differences. Replacing texture's manual resource offsets with the existing typed `SplitCurveResource` helper leaves every reported function score unchanged. Giving the step helper Forecast's 16-bit frame-length local also leaves every score unchanged. Combining that local with Forecast's byte-sized process flag reduces texture to 99.18% without resolving another body. Using Forecast's separate byte wrap/reverse and 16-bit name locals, together with its signed layer index, reduces texture to 98.18%. All controls preserve compiler options and are reverted; none establishes a new canonical correction or source credit.

The remaining texture differences in the complete diff include the saved particle/header/tick register assignment and the step helper's period/loop-count scratch registers. The baked pair shares the same frame-length and loop/period register permutation. Their unchanged instruction count does not excuse those differences. Future work should examine value lifetimes against the native functions rather than repeat these rejected scalar/helper substitutions.

Outside-checkout reproducers are `377-texture-resource-probe.py`, `377-reference-step-types.py` and `377-reference-texture-scalars.py`; complete outputs and the pinned Forecast reading source are in `377-resource-probe/`. Each script restores the original file bytes after its object-only probe. No full link or original-issue completion is claimed by these controls.

## 2026-11-04 Texture and Child scheduling controls

No source change was retained in this bounded continuation. Direct scratch compiles use the configured Wii/1.1 NW4R flags. Texture remains 99.61126%; Child's retained baseline remains 96.30681%.

Texture's differing instructions form a parameter-register cycle plus the two frame-period temporary allocations. Register tracing identifies the second period as anonymous node `@4192`, interference degree 29, colored first into r31. The input tick shares that register; the command pointer consequently takes r24 rather than target r31. Pointer re-expression/type-punning controls, 120 fully-inlined body-wrapper parameter permutations, 36 frame/period/count type combinations, 64 period initialization forms, 24 explicit loop-count caching forms, and selective volatile field-read controls did not improve the baseline. All changed helpers were private scratch clones, so no shared-header regressions were introduced.

Child exposes two target process-flag reloads which the current inline body common-subexpression-eliminates. Qualifying only the processFlag read in a scratch CalcFrameStep clone as volatile restores both target loads and raises Child to 97.489044%. The algebraically equivalent unsigned return `(period * (rLoop + 1) - 1) - (tick - 1)` raises it to 97.731255%, bringing the second-call subtract sequence closer to target. These are artificial partial candidates and were not retained. Pointer-reference parameter variants do not recover an exact body. Remaining differences are predominantly register allocation and a scheduled halfword truncation.

Scratch scripts and traces are in ignored `tools/local/session/animcurve-61-*`. Higher fuzzy scores here do not establish additional matching functions or source-link acceptance.
