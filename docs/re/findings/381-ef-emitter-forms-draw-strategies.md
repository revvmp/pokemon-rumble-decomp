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

# F-381-2: NW4R ef draw strategy builder and DrawStrategyImpl

Two more units follow the emitter forms. Both are `Matching`: objdiff reports 100% for all 17 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`.

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/ef/ef_drawstrategybuilder.cpp` | `.text` | `0x8023AC40–0x8023ADE0` | `DrawStrategyBuilder::Create` |
| | `.data` | `0x803C5658–0x803C5668` | `DrawStrategyBuilder` vtable |
| | `.bss` | `0x8042F2F8–0x8042F918` | seven function-local strategies, 0xE0 each |
| | `.sbss` | `0x804B92E8–0x804B92F0` | their guard bytes and padding |
| `nw4r/ef/ef_drawstrategyimpl.cpp` | `.text` | `0x8023ADE0–0x8023CDF8` | 16 functions, from the constructor to `__sinit_\ef_drawstrategyimpl_cpp` |
| | `.ctors` | `0x80352A48–0x80352A4C` | static initializer entry |
| | `.data` | `0x803C5668–0x803C5680` | `DrawStrategyImpl` vtable |
| | `.bss` | `0x8042F918–0x8042F978` | `mXUnitVec`, `mYUnitVec`, `mZUnitVec`, `mZeroVec`, `mIdentityMtx` |
| | `.sdata2` | `0x804BA958–0x804BA984` | constants |

dtk rejected an `.sbss` end at the last guard byte (`0x804B92EF`) for alignment, so the builder split ends at `0x804B92F0`. dtk had merged `mXUnitVec` and `mYUnitVec` into one 0x18-byte symbol; they are now two 0xC symbols. `lbl_804BA980` is now 4 bytes so the `.sdata2` split can end at the last constant. The billboard strategy (`ef_drawbillboardstrategy.cpp`) starts at `0x8023CE00`.

## Reference

The sources follow Whippy `src/nw4r/ef/ef_drawstrategybuilder.cpp` and `ef_drawstrategyimpl.cpp`. `_InitTexture` and `_SetupACmp` are inlined into their callers; the linker removes the out-of-line copies. Differences from the Whippy source, each confirmed by the target:

- `GXSetFog` takes the colour last. The target passes the four floats in `f1`–`f4` and the colour through a stack pointer in `r4`, matching the project's RVL_SDK prototype `GXSetFog(GXFogType, f32, f32, f32, f32, GXColor)`. `DrawInfo::GetFog` is inlined.
- `GXSetZMode` takes `(flags & FLAG) != 0` for both flags. The reference's `? TRUE : FALSE` form computes the third argument before the first.
- `AheadContext::AheadContext` ignores the result of `MTX34Inv`. The reference falls back to `MTX34Identity`, which the target lacks.
- `GXInitTexObjCI` takes a `GXCITexFmt` in the project's SDK header.
- `Particle::Draw_GetTextureScaleT` and `Draw_GetTextureTransT` test the reverse flags as `reverse >= TEX_REVERSE_T && reverse <= TEX_REVERSE_ST` and negate with `-scale`, as in the Whippy header. The S variants keep the two-comparison form. The particle units still match.

## Headers

New headers `drawstrategy/ef_drawstrategyimpl.h` (from Whippy without comments) and one header per strategy class. The vtables at `0x803C5680–0x803C5760` fix each strategy's virtual layout: destructor, `Draw`, the two inherited particle-order getters, and `GetCalcAheadFunc` for the billboard, directional, stripe and smooth-stripe strategies. Unlike the Whippy header, `DrawSmoothStripeStrategy` does not override the particle-order getters. Shared-header additions are declarations only: `GXGetVtxDesc` and `GXGetVtxAttrFmt` in `GXGet.h`, and the vertex writers `GXNormal3u8`, `GXNormal3u16`, `GXColor3u8`, `GXTexCoord2s8`, the five `GXTexCoord1` forms, `GXPosition1x16`, `GXNormal1x16`, `GXTexCoord1x16`, `GXColor1x8` and `GXColor1x16` in `GXVert.h`. `EmitterDrawSetting::DirType` gains `DIR_7`, which `AheadContext` treats like `DIR_NO_DESIGN`; the reference name is kept because no behavior distinguishes it yet (low confidence).

## Traps

- **A boolean argument form can reorder argument evaluation.** `cond ? TRUE : FALSE` and `cond != 0` produce the same two instructions, but CodeWarrior schedules the ternary arguments in a different order. When only two independent argument computations are swapped before a call, try the comparison form.

# F-381-3: NW4R ef billboard strategy, first half

`ef_drawbillboardstrategy.cpp` starts at `0x8023CE00`, the last four functions of #381. Its later functions, from `DrawDirectionalBillboard` at `0x8023E0F0`, are in #382's range. #381 owns the split and object line because it owns the first function. The unit stays `NonMatching` until #382 adds the rest. The five functions written match at 100% in objdiff, and the full DOL verifies with the split.

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x8023CE00–0x8023EFA0` | constructor, `Draw`, `DrawNormalBillboard`, `DrawYBillboard`, `DrawDirectionalBillboard`, `InitGraphics`, `GetCalcAheadFunc`, weak `CalcAhead_Speed`, `CalcAhead_EmitterCenter`, `CalcAhead_EmitterDesign`, `CalcAhead_Particle`, `CalcAhead_ParticleBoth` (`0x8023ECE0`), destructor (`0x8023EF60`) |
| `.data` | `0x803C5680–0x803C569C` | `DrawBillboardStrategy` vtable, seven slots; four bytes of padding precede the directional vtable |
| `.sdata` | `0x804B8900–0x804B8908` | 32-byte-aligned texture-coordinate table `{0, 1, 0, 0, 1, 0, 1, 1}` |
| `.sdata2` | `0x804BA988–0x804BA9B4` | `0.0f`, `100.0f`, `1.0f`, `FLT_EPSILON`, `2π/256`, two conversion doubles, `0.5f` |

The directional strategy constructor at `0x8023EFA0` starts the next unit. `lbl_804BA9B0` is now 4 bytes so the `.sdata2` split ends at its last constant, and the vtable symbol is 0x1C.

## Reference and version

The public billboard sources (ogws, forecast, wii-news-channel, veryfat-dungeon) are an older revision. This target matches the newer revision also found in Monster Hunter Tri:

- `Draw` sends `ASSIST_BB_NOROLL` (3) to `DrawNormalBillboard`, which then skips the view roll.
- Both draw functions start with the inlined `DrawInfo::GetViewMtxCooked`. Here its projection switch compiles as a compare chain.
- Axis lengths come from `ef::MTXColLen`. The roll uses `math::FSqrt` and `math::FInv`, and its zero test uses the root.
- `DrawYBillboard` skips particles that are not `NW4R_EF_LS_ACTIVE`. `DrawNormalBillboard` does not.
- When the particle has no Z rotation, `DrawNormalBillboard` computes the four corners in an inline paired-single `asm` block. Matrix-multiply registers `f0`–`f12` are fixed, and the other values are eight `register` work variables.

The CC0 MH Tri decompilation (debug build) gives the corner instruction sequence and the expression trees of both quad writers. Its C names and asserts were not used.

## Recovered detail

- The register allocation of the corner block depends on how work variables are reused, not on their names. With one variable per value, CodeWarrior merged the rotation and X-axis registers and scheduled the block differently. Eight work variables, each reused the way the target's registers are, match the schedule and every register.
- The corner array's address register is `r23`, below the rotated branch's vertex temporaries, only when the array is reached through a pointer variable declared with it (`math::VEC3* pVtx = vtx;`) and passed both to the corner block and to the polygon writer. Passing the array directly, or the pointer to only one of them, gives `r29` or perturbs the floating-point registers.
- `DrawYBillboard` needs `f32 rs, rc;` declared in that order. `DrawNormalBillboard` needs `f32 cr, sr;`, which places the cosine at `sp+0x8` and the sine at `sp+0xC`.

## Traps

- **Inline asm work variables are colored by live range.** A CodeWarrior `asm` block with one `register` variable per value can match every instruction and still differ in floating-point registers and schedule. Find which values share each target register and reuse one work variable for each group, as NW4R's own math asm does.
- **A local array's address register follows how the array is referenced.** When only the register holding a hoisted stack address differs, try reaching the array through a pointer variable declared with it.
