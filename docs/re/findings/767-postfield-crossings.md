# F-767-1: Curved-shape crossings and collision dispatch

The four routines in #767 belong to the post-field unit split and owned by #377. [F-377-1](377-curves-postfields.md) and PR #1002 establish the complete `.text` `8022DFB0-80231640` and `.sdata2` `804BA660-804BA698` extents and canonical header. This change appends the four bodies in native order, without changing the split, object registration, or shared header.

| Address | Function | Native instruction bytes | Candidate |
| --- | --- | --- | --- |
| `8022F500` | `crossCheckSphere` | 1600 | 100% |
| `8022FB40` | `crossCheckCylinder` | 2024 | 100% |
| `80230330` | `crossCheckCube` | 1628 | 100% |
| `80230990` | `crossCheck` | 1032 | 100% |

The four bodies total 6,284 bytes. With the nine earlier #377 routines, all thirteen candidates total 11,672 bytes. Only the 2,208-byte #768 particle-step body is still absent. The unit remains `NonMatching`, selects its original object, and earns no source credit until that last routine is accepted with a source-linked full build.

## Evidence and provenance

The algorithm reading reference is [InusualZ/mhtri-decomp ac489be692a1273afff8574b2402aecba2ae2b8f](https://github.com/InusualZ/mhtri-decomp/tree/ac489be692a1273afff8574b2402aecba2ae2b8f), `src/ef/ef_postfield.cpp`, under CC0. As in F-377-1, this revision has procedural wrappers, debug assertions, and code differences. The reconstruction uses this repository's canonical C++ vectors, matrices, particles, emitters, effects, system, and creation queue. No debug compiler flags, offset wrappers, assertions, or decompiler output are imported.

The immutable target, original split object, and verified native ELF provide the instruction, ABI and data evidence. Gekko disassembly establishes the reference differences. All original mangled names already exist in this target's symbol map; no new external names are invented.

Sphere and cylinder solve the segment/surface quadratic, choose the nearest admissible root, and test their caps. Sphere's option selects a hemisphere. Reflection changes velocity and remaining direction, then moves the crossing point slightly away from the surface. Sphere constructs and inverts the normal frame; cylinder reflects in the XZ plane. Cube checks all six faces, keeps tied faces within epsilon for edge/corner reflection, and snaps the hit point to the corresponding unit coordinates.

The dispatcher takes previous position, current position and a velocity vector by value, followed by the killed flag and four output vector pointers. Its canonical declaration's parameter labels were provisional; the mangled reference/value types are correct. The native first reference is previous position and second is current position; the by-value vector is velocity. The four outputs are local hit, transformed hit, resulting position, resulting velocity. The source definition uses those meanings.

When reflection or stopping is enabled, it transforms and scales the resulting velocity and carries the remainder of the step beyond the hit. When neither is enabled, this target still transforms the velocity through the inverse matrix and the current position through the forward matrix. That else branch is absent from the reading reference and is restored from `80230CA0-80230CBC`. Killing copies the transformed hit to the resulting position. Creation flags use `Particle::mCalcRemain` at `0xE2`, not the reference wrapper's lifetime interpretation, and call the canonical creation queue through the manager/emitter/effect/system chain.

## Validation

The standard compiler remains Wii/1.1 with unchanged `cflags_nw4r`. `tools/re/unitdiff.py nw4r/ef/ef_postfield.cpp` reports all thirteen supplied functions at 100%. The independent ELF audit compares their complete unmasked bodies, sizes, paired resolved relocation targets, first 52 native constant bytes, actual fallback graph selection, and complete DOL identity. The build and frozen-manifest verification exit 0. The official landing procedure repeats the build, frozen verification and independent audit on the exact rebased head.

This is partial work on #767: its four bodies are reconstructed, but acceptance requires the whole unit including #768. Do not close #767 from this pull request. Finish #768, select the complete unit from source only after exact verification, and reconcile the original four-row inventory before closing this issue.

## Traps

- A high score with matching function size can still hide register differences. Reuse the quadratic root variable for cap crossings in the sphere rather than introducing two independent cap-time lifetimes; this reproduces its allocation without assembly or compiler changes.
- Express the repeated `-2.0f * dot` terms at the quadratic roots. Introducing a separately live negative-dot local swaps scratch registers in sphere and cylinder. Explicit `math::VEC3Scale` calls preserve the native inline work-register allocation where nested `operator*=` instantiation differs. All of these candidates were checked against exact instruction words, not accepted from score alone.
- The native dispatcher evaluates the option argument once before its shape switch. A small inline `GetTypePosition` helper preserves that interface; spelling six independent option accesses in the caller emits extra loads. The helper itself has no retained native body and earns no extra credit.
- Do not omit the no-reflection/no-stop transformation path merely because it is absent from a nearby revision's source. The target contains two real matrix-vector calls in that else branch.
