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

# F-382-2: NW4R ef directional strategy

`ef_drawdirectionalstrategy.cpp` starts at `0x8023EFA0` with the `DrawDirectionalStrategy` constructor and is now split and registered as `NonMatching`. The split is verified: the full DOL still passes `tools/verify/verify.py`.

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x8023EFA0–0x80241320` | constructor through the destructor at `0x802412E0`; `DrawFreeStrategy` starts at `0x80241320` |
| `.rodata` | `0x8036C5D0–0x8036C630` | the quad and cross-quad corner arrays, `0x30` each |
| `.data` | `0x803C56A0–0x803C56BC` | `DrawDirectionalStrategy` vtable, seven slots (`0x1C`, was a `0x20` label) |
| `.sdata` | `0x804B8920–0x804B8928` | texture-coordinate table (`lbl_804B8920` now 8 bytes) |
| `.sdata2` | `0x804BA9B8–0x804BA9E0` | `0.0f`, `FLT_EPSILON`, `1.0f`, `100.0f`, `0.5f`, `2π/256`, two conversion doubles |

The unit crosses into #383's range at `0x802408C0`. #382 owns the split, the object line and the header, because it owns the first function. The functions from `0x802408C0` are #383's: the weak `MTX34` twelve-float constructor, `CalcLocalTransform`'s out-of-line copy, `DrawDirectionalBillboard` (`0x80240B10`), `InitGraphics` (`0x80241150`), `GetCalcAheadFunc` (`0x80241210`), `CalcAhead_NoDesign` (`0x802412B0`) and the destructor.

## Names

The CC0 Wii Sports map (kiwi515/ogws_info `RSPE_r1.map.csv`) lists the unit's functions in the same order: constructor, two `ef::DrawQuad` overloads, `Draw`, `DrawDirectional`, the weak copies, `ef::CalcRotate`, `ef::CalcLocalTransform`, `DrawDirectionalBillboard`, `InitGraphics`, `GetCalcAheadFunc` and `CalcAhead_NoDesign`. These names replace `fn_8023EFE0`, `fn_8023F120` (`math::VEC3Transform`), `fn_80240340` (`math::MTX34Mult`), `fn_80240380` (`DrawStrategyImpl::Normalize`), `fn_80240B10`, `fn_80241150`, `fn_80241210`, `fn_802412B0` and `fn_802412E0`. Both `DrawQuad` overloads are file-local (`scope:local`); `ef_drawfreestrategy.cpp` has its own matrix `DrawQuad` at `0x80241360`. `CalcAhead_NoDesign` takes the unused `Particle*`, as `CalcAhead_EmitterDesign` does.

## Status

Five functions match at 100%. The constructor, the matrix `DrawQuad` and `Draw` are in #382's range. `InitGraphics` and the implicit destructor are in #383's range, but the source defines them because `Draw` inlines `InitGraphics`. `Draw` inlines `InitGraphics` and calls `DrawDirectionalBillboard` when `(typeOption2 & 3) == 1`, otherwise `DrawDirectional`. The reference structure is forecast's older `Draw`, with three changes from MH Tri's newer `draw_world`. The view matrix is the inlined `GetViewMtxCooked`. When `typeDir` is `DIR_7`, the emitter axes come from the inverse particle-manager matrix of the `AheadContext`. The speed stretch is computed before `SetupGP`.

`DrawDirectional` scores 96.8%. Every other instruction agrees, but the target lowers all of its switches as binary searches: the inlined `GetViewMtxCooked`, `Draw_GetSizeY`, `CalcRotate` and `CalcLocalTransform`. It also sets the texture flag with a branch. The rebuild uses compare chains and a branchless flag. The out-of-line copies of the same inlines in this unit use compare chains in the target too. This is the problem F-383 reports for `DrawFreeStrategy::Draw`. Both functions inline `CalcRotate`.

Probes in scratch, all reverted:

- `#pragma global_optimizer off` and `#pragma optimizewithasm off` both produce the binary searches (9 of the target's 10 `bge`), but they also change the schedule around the inlined paired-single `asm`, which the target does optimize. `opt_dead_assignments off` produces 3. No other `opt_*` pragma, `-inline` mode, `-ipa` mode or optimization level changes the switches.
- Wii/1.0 to 1.7 keep the compare chains (1.3 to 1.7 give 3 `bge`). GC/3.0a3 and 3.0a5.2 give 7 but score 87.7%.
- The switch form does not change when the function grows by up to 250 extra branches, 100 extra locals, 16 extra stack aggregates, or 8 extra switches. It does not change when `CalcRotate` and `CalcLocalTransform` are defined after `DrawDirectional`. Five more inlined `Normalize` calls stop the inlining of everything that follows.

The texture flag is written as `if (mNumTexmap > 0) { texCoord = 1; } else { texCoord = 0; }`, as in the free strategy. That form gives the target's branch whenever the optimizer leaves the switches alone. `int texCoord = mNumTexmap > 0` always compiles branchless.

The weak copies in #382's range, at `0x8023F120` and `0x80240170–0x802408B4`, and the vector `DrawQuad` at `0x8023F160` exist only for `DrawDirectionalBillboard`, which calls them out of line. They cannot be produced until that function is written. The source already defines the vector `DrawQuad`.

## Traps

- **Binary-search switches can mark a partly optimized function.** When a target lowers a small inlined switch as a binary search but the same inline's out-of-line copy uses a compare chain, the function was compiled with less global optimization than the rebuild. Check `#pragma global_optimizer off` in scratch to confirm the form before rewriting the switch. The cause in this library is still unknown.
- **Weak copies can belong to a function in another issue.** Out-of-line copies of header inlines are emitted only for the callers that do not inline them. Find those callers before you assign the copies' addresses to an issue's done criteria.
