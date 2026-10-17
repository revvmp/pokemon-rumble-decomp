# F-382-1: NW4R ef billboard strategy, second half

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. `ef_drawbillboardstrategy.cpp` (split and first half in [F-381-3](381-ef-emitter-forms-draw-strategies.md#f-381-3-nw4r-ef-billboard-strategy-first-half)) is now complete and `Matching`: objdiff reports 100% for all 13 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. The split is unchanged.

The functions added from `0x8023E0F0`, the start of #382's range, are `DrawDirectionalBillboard`, `InitGraphics`, `GetCalcAheadFunc` and the weak copies of the header-inline `DrawStrategyImpl::CalcAhead_Speed`, `CalcAhead_EmitterCenter`, `CalcAhead_EmitterDesign`, `CalcAhead_Particle` and `CalcAhead_ParticleBoth` (`0x8023ECE0`, formerly `fn_8023ECE0`). The weak copies follow `GetCalcAheadFunc` in the order it references them. The `.sdata` texture-coordinate table `{0, 1, 0, 0, 1, 0, 1, 1}` needs no alignment attribute; it starts its split at `0x804B8900`.

## Reference and version

As in F-381-3, the target is the newer revision also in the CC0 MH Tri decompilation (InusualZ/mhtri-decomp `ac489be692a1273afff8574b2402aecba2ae2b8f`), which gives the expression trees; the public forecast source (CC0 doldecomp/forecast `a1f28f8a0d777ebd12953f5c6997ee2decfd07f5`) gives the older structure.

- `DrawDirectionalBillboard` starts with the inlined `DrawInfo::GetViewMtxCooked`, skips particles that are not `NW4R_EF_LS_ACTIVE`, takes the axis lengths from `ef::MTXColLen`, transforms the ahead vector with `VEC3TransformNormal`, and derives the roll from `math::FSqrt` of its XY length and `math::FInv`, with the zero test on the root. The stretch reads `EmitterDrawSetting::typeOption0` (`+0xB0`).
- The quad origin is `pos + (vx·rc·ox + vy·rs·oy, vx·rs·ox − vy·rc·oy, 0)` with `ox = px − sx·px` and `oy = sy·(stretch + py − 1) − py`. The target schedule needs the three vectors built with the `VEC3` constructor; member assignment scores 98.5%.
- `InitGraphics` calls `DrawStrategyImpl::Initialize`, sets the vertex format, and loads an identity position matrix from a local `MTX34`.
- `GetCalcAheadFunc` reads `typeDir` (`+0xAE`) and maps `AHEAD_BB_PARTICLE_BOTH` (4) to `CalcAhead_ParticleBoth`.

## Recovered detail

- **`InitGraphics` is not auto-inlined.** All three draw functions call it out of line. Without an assert the body has CodeWarrior's auto-inline complexity of exactly 15, the `inline_max_auto_size` default, so `-inline auto` inlines it into all three and they drop to 90–94%. A release `NW4R_ASSERT_VALID_PTR(pManager)`, which the MH Tri debug build shows at the top of this function, adds one empty statement and keeps it out of line. The directional and point strategies' `InitGraphics` are smaller and are inlined in the target.
- **`CalcAhead_EmitterDesign` takes `Particle*`.** The mangled name has three parameters; the header now declares the unused third one.
- **Elder and younger walks take a `const ParticleManager*`.** `CalcAhead_Particle` and `CalcAhead_ParticleBoth` reload the list offset (`lhz 0x42`) on every pass of the walk. `GetElderDrawParticle` and `GetYoungerDrawParticle` reproduce this when their manager parameter is `const`, using new `const` overloads of `GetElderParticle` and `GetYoungerParticle`; the non-`const` overloads stay, because the target has their weak out-of-line copies (`0x80240400`). `GetNextDrawParticle_EldersFirst` and `GetNextDrawParticle_YoungersFirst` in `ef_drawstrategyimpl.cpp` now simply return these walks, replacing the file-local `Const` helpers of F-381-2 (Whippy's own TODO). That unit still matches at 100%.
- **The texture flag is an `int` below the draw functions.** `DispPolygon` and the three `DispParticle_*` helpers take `int texCoord`. `DrawDirectionalBillboard` keeps an `int` local; with `bool` the flag lands in `r23` instead of `r30` and the function scores 99.6%. `DrawNormalBillboard` and `DrawYBillboard` keep their `bool` locals, which they need, and still match.

## Traps

- **A function at the auto-inline limit can need a release assert.** When a target calls a small same-file function out of line but the rebuild inlines it, test `#pragma inline_max_auto_size` in scratch to measure its complexity. If it sits at the default of 15, the original probably had a statement that compiles to nothing, such as an NW4R release assert. Do not commit the pragma.
- **A flag's declared type moves its register.** `int` and `bool` flags produce the same compare instructions, but they colour differently through two levels of inlining. When only one flag's register differs, sweep the flag's type at each inline level.
