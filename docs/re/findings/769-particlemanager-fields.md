# F-769-1: Particle-manager field and post-field reconstruction

Issue #769 owns `nw4r/ef/ef_particlemanager.cpp`. The original unit has 17 retained functions and 8,880 instruction bytes. `ParticleManager::Calc` at `0x80232930` is 5,464 bytes (`0x1558`). The inherited body scored 26.02% and omitted both field and post-field work. The other sixteen functions already matched.

## Reference and target evidence

The algorithm reference is [InusualZ/mhtri-decomp at ac489be692a1273afff8574b2402aecba2ae2b8f](https://github.com/InusualZ/mhtri-decomp/tree/ac489be692a1273afff8574b2402aecba2ae2b8f), specifically `src/ef/ef_particlemanager.cpp`, `src/ef/ef_postfield.cpp`, and `src/ef/ef_pf_calc_particle.h`. Its root license is CC0. These files are under `src/ef/`, rather than the sparse checkout's older `src/nw4r/ef/` tree. The reference has debug assertions, procedural wrapper types, revision differences, and nonmatching notes of its own. Its implementation names, opaque offset scaffolding, assertions, and compiler pragmas were not imported. The reconstruction uses this project's existing C++ classes and the target's Gekko instructions.

The immutable original executable, its split object, and the linked baseline ELF supply the target evidence. Headless IDA on a temporary database copy supplied a reading aid. Its paired-single gaps were checked against `powerpc-eabi-objdump -d -M gekko`; generated pseudocode was not pasted into source.

| Target observation | Reconstructed meaning |
| --- | --- |
| `0x80232EBC` accepts `(u8)(magic + 0x55) <= 1` | Both baked `0xAB` and keyed `0xAC` tracks |
| `0x80232ED0` loads header byte 2 | Direct curve dispatch; the old `u16 >> 4` expression was incorrect for this target |
| `0x80232F68` and `0x80232F88` add byte 1 to particle `+0x20` | Ordinary U8 and F32 destinations are byte offsets into `ParticleParameter` |
| `0x80232F4C` always passes particle `+0x40` | Rotation destination is `mParameter.mRotate`; no old logical-target exclusion |
| `0x80232FA8` copies `0x1C` bytes after the key/range/random/name tables | Field record, curve type 7 |
| `0x80233920` copies `0x24` bytes and optionally evaluates F32 into that copy | Post-field transform, curve type 2 in particle-manager dispatch |
| `0x80233A70` calls `0x80230DA0` | Post-field particle step, including collision and velocity processing |
| `0x80233B60` reads post-field byte `0x48` | Optional bounded wrapping after the post-field step |

The private header `ef_particlemanagerfield.h` is used only by this unit. Its `detail` type and helper names are descriptive, not recovered original spellings. Existing shared resource and particle headers are unchanged. `PostFieldInfo` is an original name supported by the adjacent `crossCheck*` mangled symbols; it remains forward-declared. The descriptive callee name `CalcPostFieldParticle` identifies the proven external call without taking ownership of its implementation or split. Texture and child executors retain their existing interface spellings from `ef_animcurve.h`.

## Resource layouts

`detail::ParticleField` is 28 bytes: space at 0, destination mode at 1, an opaque reserved byte at 2, flags at 3, power at 4, and twenty bytes of kind-specific parameters at 8. The evidenced kinds are gravity 0, speed 1, magnet 2, Newton 3, vortex 4, spin 6, random 7, and tail 8. This does not assign behavior to kind 5.

Gravity and spin use a rotation vector at 8. Magnet uses a point at 8. Newton uses distance at 8 and a point at 12. Vortex uses outer power at 8, distance at 12, and rotation at 16. Random uses diffusion at 8 and a two-byte interval at 12. Unread payload bytes remain opaque reserved storage.

Field space 1 transforms the particle position into emitter space and transforms the resulting force back into manager space. Other position-dependent modes begin with the world-space particle position minus the emitter's world-space origin. Force space 0 transforms from world to manager space, 2 uses the force directly, and 3 first scales by the measured emitter scale. Destination mode 0 adds velocity; mode 1 adds position. Random mode additionally interprets this byte when selecting its power's base velocity.

`detail::ParticlePostFieldTransform` is 36 bytes: scale, rotation, translation. `detail::ParticlePostField` is 112 bytes: that transform, 34 opaque collision-parameter bytes, a two-byte effect-resource index at `0x46`, wrap flags at `0x48`, three reserved bytes, then wrap scale at `0x4C`, rotation at `0x58`, and translation at `0x64`. The resource is selected from the track's name table only when that table exceeds its four-byte header.

The tail field reads `Emitter::mPrevGlobalTranslate` at `0x114`, skips a NaN origin through `__fpclassifyf`, and uses the emitter's current global translation minus that saved translation. The scalar momentum step uses the existing `Particle::AddPosition` method twice, first for the position offset and then for velocity, preserving the target's operation order.

## Current result and remaining work

The source remains `NonMatching`, so the build selects the original unit. The first complete draft scored 91.13% for `Calc`; the current reconstruction scores 94.20%. All sixteen other functions remain 100%. The current `Calc` is 5,532 bytes, 68 bytes longer than the original; its frame is `0x5E0` rather than `0x5C0`. The principal differences are seven matrix-copy spill slots, a spilled post-field flag, register assignment, and instruction ordering. A fuzzy score is not evidence of byte identity.

The `.sdata2` pool is now 72/72 bytes at 100%, versus 16 bytes in the inherited source. The absent `smMtxInv` definition was removed using the already established F-378-1 ownership evidence; only `smDrawMtxPMtoEM` belongs to this unit. No split or build-status changes, original hashes, or compiler flags were made.

After the symbol-name and unused-static cleanup, `python configure.py --map` exited 0, default Ninja exited 0 after deleting only `build/wpse-v1/main.dol`, and explicit frozen verification exited 0 with `PASS main.dol`. `git diff --check` also exited 0. This validates the retained-original build only and earns no new source coverage. Re-run the full workflow after subsequent changes and require full source linking and exact matching before closing #769.

## Diagnostics

Temporary diagnostics are prefixed `769-` outside the repository. Eight matrix-copy spelling/order variants, 48 post-field-local declaration/type variants, eight scalar type variants, twelve helper placement/linkage/result variants, eight copy/zero grouping variants, sixteen register-keyword variants, six direction-matrix helper variants, and six equivalent control-flow variants did not remove the remaining spills. Several are byte-identical after optimization. External matrix-copy calls reduce the score and do not reproduce the native word-copy sequence. These are negative results for this source context, not general compiler rules.

Compiler diagnostics compared Wii/1.1 and 1.3 under performance/size optimization and automatic/deferred inlining, then the other seven installed Wii versions under the existing performance settings. None improved on the current compiler. Size optimization reduced the matching neighbor count from sixteen to eight. Per-function IPA scored 89.64%; per-function IPA plus deferred inlining scored 92.49% and reduced matching neighbors to thirteen. Program IPA emitted an intermediate object without normal diff symbols, so it was not scored. Original build settings were restored after every diagnostic.

## Traps

- **The tracked IDA database has labels without function definitions.** `ida_funcs.get_func(0x80232930)` returned none and decompilation failed with invalid input ranges. On the scratch copy, recreate exactly the proven function extent and wait for analysis before decompiling. Do not change or commit the tracked database.
- **Default PowerPC ISA settings misdecode paired-single instructions.** IDA's server/Altivec configuration treated Gekko PSQ instructions as VSX instructions. On the scratch copy only, `PPC_ISA_SUPPORT=0x0008`, `PPC_TOC=0x804C1C60`, and `PPC_SDA_BASE=0x804BDA20` restore the correct interpretation. These options are documented by the installed `ppc.cfg` and `ida.cfg`. GNU objdump likewise needs `-M gekko`. Hex-Rays still omits two valid paired-single case blocks; inspect native instructions instead of interpreting those gaps as missing target behavior.
- **Register-save helpers need accurate analysis types.** Applying a no-spoils user-call type to the target's save/restore helpers and concrete known callee prototypes made the decompiler preserve the manager argument. This changed scratch analysis types only, never executable bytes.
- **A related reference can expose the right algorithm under another source tree.** Restricting the CC0 Monster Hunter Tri checkout to `src/nw4r/ef` missed its fuller `src/ef` implementation. Inspect the pinned tree before concluding that a public reference has no field calculations; still validate every adopted layout and path against this target.
