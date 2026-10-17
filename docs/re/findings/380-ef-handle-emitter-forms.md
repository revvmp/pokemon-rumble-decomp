# F-380-1: NW4R ef handle, emitter form, creation queue and emitter-form units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. #380 opens with the tail of `ef_util.cpp` (`0x80235CB0`-`0x80236298`), whose first retained function is in #770. [F-770-1](770-effect-utility.md) now reconstructs that full file as Matching. The original work below reconstructed the eight units after it; all are Matching. [F-380-2](380-complete-inventory-audit.md) records the complete issue inventory across all nine source-linked units. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/ef/ef_handle.cpp` | `.text` | `0x802362A0–0x80236370` | `HandleBase` constructor, both assignment operators, `IsValid`, `GetPtr` |
| `nw4r/ef/ef_emitterform.cpp` | `.text` | `0x80236370–0x80236904` | `EmitterForm::CalcVelocity`, `CalcLife` |
| | `.sdata2` | `0x804BA770–0x804BA79C` | constants |
| `nw4r/ef/ef_creationqueue.cpp` | `.text` | `0x80236910–0x80236D54` | constructor, `AddParticleCreation`, `AddEmitterCreation`, `Execute` |
| `nw4r/ef/ef_emform.cpp` | `.text` | `0x80236D60–0x80236E38` | `EmitFormBuilder::Create`, `__sinit_\ef_emform_cpp` |
| | `.ctors` | `0x80352A44–0x80352A48` | static initializer entry |
| | `.data` | `0x803C55D8–0x803C55E4` | `EmitFormBuilder` vtable |
| | `.sbss` | `0x804B92C8–0x804B92E4` | seven file-local emitter-form instances |
| `nw4r/ef/ef_point.cpp` | `.text` | `0x80236E40–0x80237188` | `EmitterFormPoint::Emission` |
| | `.data` / `.sdata2` | `0x803C55E8–0x803C55F4` / `0x804BA7A0–0x804BA7D0` | vtable / constants |
| `nw4r/ef/ef_line.cpp` | `.text` | `0x80237190–0x802374C8` | `EmitterFormLine::Emission` |
| | `.data` / `.sdata2` | `0x803C55F8–0x803C5604` / `0x804BA7D0–0x804BA7F8` | vtable / constants |
| `nw4r/ef/ef_disc.cpp` | `.text` | `0x802374D0–0x80237A04` | `EmitterFormDisc::Emission` |
| | `.data` / `.sdata2` | `0x803C5608–0x803C5614` / `0x804BA7F8–0x804BA840` | vtable / constants |
| `nw4r/ef/ef_sphere.cpp` | `.text` | `0x80237A10–0x80238264` | `EmitterFormSphere::Emission` |
| | `.data` / `.sdata2` | `0x803C5618–0x803C5624` / `0x804BA840–0x804BA890` | vtable / constants |

The `.sdata2` split of `ef_emitterform.cpp` ends at its last constant (`0x804BA79C`); dtk had inferred an 8-byte symbol at `0x804BA798`, now 4 bytes. The last emitter-form instance at `.sbss` `0x804B92E0` is likewise 4 bytes. `ef_cylinder.cpp` follows at `0x80238270` with a file-local helper ahead of `EmitterFormCylinder::Emission` (`0x802386D0`), in #381.

## Reference

All eight sources follow `src/nw4r/ef/` of CC0 `Etrainquandt17/Whippy` (commit `8ee40c841475db862231f37b8cb64196b613cc17`), the Kirby revision of F-375-2. Code matches without change. Differences:

- `HandleBase`'s copy constructor and `HandleBase(LinkedObject*)` are absent from the target; the source omits them.
- The emitter-form sources take `math::FMod`, `cosf` and `NW4R_MATH_FLT_EPSILON` in place of the reference's `std::fmodf`, `std::cosf` and `std::numeric_limits<f32>::epsilon()`; the project has no MSL C++ headers. `math.h` gains an inline `cosf` like its `sqrtf`.
- `ef::FAbs` must be the MSL `std::fabs(float)` form, `static_cast<f32>(__fabs(x))`. The paired-single `math::FAbs` omits the `frsp` after `fabs` that the disc and sphere targets contain, which left both at 99.4%. `ef_util.h` declares it, as the reference does.

## Recovered layouts

The emitter-form vtables at `0x803C55E8–0x803C5658` are laid out point, line, disc, sphere, cylinder, torus, cube, the Whippy link order. The `EmitFormBuilder::Create` switch maps the `EmitFormType` values of the existing header unchanged (disc 0, line 1, cube 5, cylinder 7, sphere 8, point 9, torus 10).

New headers `ef_handle.h` and `emform/ef_point.h`, `ef_line.h`, `ef_disc.h`, `ef_sphere.h`, `ef_cylinder.h`, `ef_torus.h`, `ef_cube.h` follow Whippy without comments. The cylinder and cube headers declare their private `EmissionSub` helpers from the reference; those functions are not yet reconstructed. `Handle<T>::operator=` returns `*this`, which the reference omits.

## Outside symbols

`ef_emform.cpp`'s static initializer stores all seven vtables, so the cylinder, torus and cube vtables (`0x803C5628`, `0x803C5638`, `0x803C5648`) in #381's range are named for the link. Their splits are unchanged.

## Completed utility tail

The formerly blocked `0x80235CB0`-`0x80236298` range is the final eight functions of `ef_util.cpp`, delivered by [PR #885](https://github.com/revvmp/pokemon-rumble-decomp/pull/885) under its first-function owner #770. This includes table-based sine/cosine, Euler direction and matrix construction, both nonzero-scale overloads, inverse scaling, and column length. F-770-1 records the newer CC0 algorithm reference, exact reconstruction, and separate C++/paired-single assembly accounting. No issue functions remain as original-object fallbacks. F-380-2 validates every listed function and its owning sections against the actual build report and frozen full executable.

## Traps

- **Two `FAbs` forms generate different code.** `math::FAbs` is a single `fabs`; the MSL `std::fabs(float)` that the ef reference calls is `fabs` followed by `frsp`. When a function scores just under 100% with only extra `frsp` instructions after `fabs`, use the double-rounded form.

The completion audit also corrected the five single-method vtable objects and `.data` split ends from sixteen bytes to twelve, leaving their four alignment bytes outside each unit. All associated data sections now compare at 100%, while full frozen DOL identity remains unchanged. F-380-2 records this new boundary finding.
