# F-381-1: NW4R ef cylinder, torus and cube emitter forms

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. #381 opens with the last three emitter-form units, which follow `ef_sphere.cpp` (F-380-1). All three are `Matching`: objdiff reports 100% for all five functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/ef/ef_cylinder.cpp` | `.text` | `0x80238270–0x80238A20` | `EmitterFormCylinder::EmissionSub`, `Emission` |
| | `.data` / `.sdata2` | `0x803C5628–0x803C5638` / `0x804BA890–0x804BA8D4` | vtable / constants |
| `nw4r/ef/ef_torus.cpp` | `.text` | `0x80238A20–0x8023905C` | `EmitterFormTorus::Emission` |
| | `.data` / `.sdata2` | `0x803C5638–0x803C5648` / `0x804BA8D8–0x804BA920` | vtable / constants |
| `nw4r/ef/ef_cube.cpp` | `.text` | `0x80239060–0x8023AC3C` | `EmitterFormCube::EmissionSub`, `Emission` |
| | `.data` / `.sdata2` | `0x803C5648–0x803C5658` / `0x804BA920–0x804BA958` | vtable / constants |

The cylinder `.sdata2` split ends at its last constant (`0x804BA8D4`); dtk had inferred an 8-byte symbol at `0x804BA8D0`, now 4 bytes. `ef_drawstrategybuilder.cpp` follows at `0x8023AC40`.

## Reference

The sources follow `src/nw4r/ef/` of CC0 `Etrainquandt17/Whippy` (commit `8ee40c841475db862231f37b8cb64196b613cc17`) without code changes, with the substitutions of F-380-1 (`math::FMod`, `NW4R_MATH_FLT_EPSILON`) and `tanf` for `std::tanf`; `math.h` gains an inline `tanf`. `ef_util.h` gains the reference's `Max` template.

## Recovered detail

`ef::FAbs(f32)` must forward to a separate `f64` overload that applies `__fabs`. The single-level form of F-380-1, `static_cast<f32>(__fabs(x))`, produces the same instructions in the disc and sphere units but not in `EmitterFormCube::Emission` (97.3%): there the target hoists `FAbs(baseY)` out of the inner loops and keeps `baseY` in a stack slot, which only the two-level inline form reproduces. This matches the Whippy definition, which calls a `double` wrapper around `__fabs`. All earlier units still match with the two-level form.

## Traps

- **Inline nesting depth changes CodeWarrior loop optimization.** Two `FAbs` definitions that expand to the same `fabs`/`frsp` pair matched every small function but differed in one large loop nest by register allocation, one stack slot and four extra `fabs`. When a large function differs only by register numbering and invariant hoisting, compare the inline call structure of the helpers it uses with the reference, not just their expansions.
