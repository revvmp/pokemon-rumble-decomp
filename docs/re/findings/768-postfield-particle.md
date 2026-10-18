# F-768-1: Source-linked complete post-field unit

Issue #768 owns the final post-field particle step at `80230DA0`, 2,208 bytes. It completes the unit split by #377 and extended by #767. All three claims are held by this same contributor session; as the #377 unit owner, this change also switches that existing object registration to `Matching`. No split, compiler flag or shared header is changed.

| Section | Native extent | Result |
| --- | --- | --- |
| `.text` | `8022DFB0-80231640`, 13968 bytes | Exact, all fourteen retained bodies plus alignment |
| `.sdata2` | `804BA660-804BA698`, 56 bytes | Exact, source linked |

The fourteen routines contain 13,880 C++ instruction bytes: 5,388 from #377, 6,284 from #767, and 2,208 from #768. These bytes are credited once at complete-unit source acceptance. The animation unit from #377 is separate and remains unreconstructed.

## Evidence and source

The immutable target executable, original split object, native ELF/map, and Gekko disassembly supply the ABI, ordering and behavior. The reading reference is [InusualZ/mhtri-decomp ac489be692a1273afff8574b2402aecba2ae2b8f](https://github.com/InusualZ/mhtri-decomp/tree/ac489be692a1273afff8574b2402aecba2ae2b8f), `src/ef/ef_postfield.cpp`, under CC0. Its procedural wrappers and debug flags are not imported. The new body uses existing C++ Particle, ParticleParameter, math, emitter/effect/system and creation-queue interfaces, the #377 PostFieldInfo declaration and the #769 transform interface. No copied executable bytes, inline-assembly reconstruction, comments or unit tests are introduced.

`CalcPostFieldParticle` and `detail::ParticlePostFieldTransform` retain the descriptive spellings introduced by #769. Their ABI is now independently confirmed by a complete matching definition and source-linked caller/callee relocation resolution. They are not represented as recovered original names. Existing `PostFieldInfo`, classifier and crossing spellings remain as recorded in F-377-1/F-767-1.

## Behavior

The function composes a scale/rotation/translation matrix, optionally follows the emitter translation, and applies the manager inverse on the left. It obtains full and rotation-only inverse transforms, classifies the previous point in field space, and updates the particle collision state. The speed mode clamps excess speed through the existing Normalize helper or flags the particle for retirement. Age, collision flags and inside/outside mode can finish processing before segment crossings.

The swept path uses the particle momentum and the sum of the by-value position offset and velocity. It transforms this endpoint and the velocity into field space and permits at most five crossings. Each iteration initializes all four hit/result vectors to zero. A segment below the tiny squared-distance threshold skips the dispatcher. A crossing writes the resulting particle position/velocity, sets collision-status bit 7, and transforms the remaining step for another iteration. With no crossing, the first iteration can still react to a stored inside/outside state transition. Subsequent completion reapplies the speed rule. Exhausting five crossings flags the particle for retirement.

The existing canonical particle members match the target: collision status `0x9D`, velocity `0xA0`, position `0xAC`, momentum `0xC4`, tick `0xDC`, calcRemain `0xE2`. Child creation passes calcRemain; the reference wrapper's lifetime label is not used.

## Acceptance evidence

`python configure.py --map`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exit 0 with the object selected from source. The prior DOL was removed before linking, and Ninja's success was checked before verification. Unitdiff reports 100% for the complete text and constant sections and every one of the fourteen retained functions.

The independent ELF audit compares the complete unmasked text and data sections, every function's size and native position, all 333 paired resolved text relocation targets, the 56 actual linked constant bytes, map contribution ownership by `ef_postfield.o`, and actual Ninja dependency selection of the source object with no original object for this unit. It also reconciles all original #767 and #768 rows and compares the complete rebuilt DOL to the immutable original. The official landing procedure repeats this audit and the frozen verification on the exact rebased merge head.

#768 is complete after that merge gate passes. #767's four routines are accepted within the same complete unit and need their issue inventory closure report. #377 remains open for its eight animation functions; its nine post-field routines are now accepted without double counting.

## Traps

- Reference matrix adapters can take output first; the canonical C++ helper takes output, left and right. Native calls prove rotation precedes scale and the manager inverse multiplies on the left. Swapping operands produced a nearly matching listing with incorrect mathematics and was rejected.
- Assigning a temporary three-argument vector into an earlier variable creates a second stack copy. Construct the emitter translation inside its conditional and declare the three local-space vectors after that scope. This gives the native argument load order, stack lifetimes, 0x1D0 frame and exact 2208-byte body without layout padding or flags.
- The target explicitly clears all four crossing/result vectors every bounce. Default empty vector constructors from the reading reference would omit twelve native stores.
- Fuzzy scores alone did not establish correctness: the initial candidate also reversed two matrix operands. Complete raw section and paired-relocation checks are required before source selection.
