# F-385-1: NW4R ef smooth stripe strategy, second half

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. #385 (`0x80247A70–0x8024AE80`) is the rest of `nw4r/ef/ef_drawsmoothstripestrategy.cpp`, which #384 split and registered ([F-384-1](384-ef-smooth-stripe-strategy.md)). The file ends at `0x8024AE80`, where `g3d_rescommon.cpp` starts; no unit boundary moves. All 17 functions of this range are now in the source file. 16 match at 100% in objdiff; `CalcAhead_ParticleBoth_Origin` scores 97.4%. The unit therefore stays `NonMatching` (27 of 28 functions at 100%, all four data sections at 100%), and the full DOL passes `tools/verify/verify.py`.

## Names

| Address | Name | Evidence |
| --- | --- | --- |
| `0x80247A70` | `DrawSmoothStripe_BSpline_ConnectRing` | already named; RZTP01 mangled name, size 0x418 |
| `0x80247E90` | `DrawSmoothStripe_BSpline_ConnectEmitter` | already named; RZTP01, 0x49C |
| `0x80248330` | `DrawStrategyImpl::GetYoungestDrawParticle` (weak) | already named; Wii Sports order |
| `0x80248380`, `0x802483C0` | `Vertex::Vertex(const VEC3&, const VEC3&, f32)`, `Vertex::operator=` (weak) | already named; RZTP01 |
| `0x80248400`–`0x80249030` | `DrawSmoothStripeTube_BSpline_ConnectNone`, `_ConnectRing`, `_ConnectEmitter`, `DrawSmoothStripeTube` | already named; RZTP01 sizes |
| `0x80249800` | `Draw` | already named; RZTP01, vtable slot 0xC |
| `0x8024A220` | `GetInitialPrevAxis` | Wii Sports order; signature from the call in `Draw` (hidden return slot in `r3`, `this`, setting, context), medium |
| `0x8024A4D0` | `GetCalcAheadFunc` | vtable slot 0x18 at `0x803C5740`, high |
| `0x8024A5A0` | `CalcAhead_Particle_Stripe` | Wii Sports order, `GetCalcAheadFunc` case 3, medium |
| `0x8024A6C0` | `CalcAhead_ParticleBoth_Stripe` | Wii Sports order, case 6 default, medium |
| `0x8024A940` | `CalcAhead_ParticleBoth_Ring` | Wii Sports order, case 6 with ring connection, medium |
| `0x8024AC10` | `CalcAhead_ParticleBoth_Origin` | Wii Sports order, case 6 with emitter connection, medium |
| `0x8024AE40` | `~DrawSmoothStripeStrategy` | vtable slot 0x8, high |

The four `CalcAhead_*` signatures follow `DrawStrategyImpl::CalcAheadFunc`, because `GetCalcAheadFunc` returns them without conversion code; no map gives their mangled names.

The ribbon functions call out-of-line weak `VEC3` members whose retained copies sit in game code. They are renamed in `symbols.txt` so the unit links: `0x80096810` `VEC3::VEC3()` (a `blr`), `0x80098CF0` `VEC3::operator+=`, `0x8009CA00` `VEC3::operator+` and `0x8009CAE0` `VEC3::operator-` (each calls `VEC3Add` or `VEC3Sub` out of line). The Wii Sports map lists `VEC3::operator_-` and `operator_+=` among this file's weak functions. Confidence: high for the bodies, medium for the operator identities.

# F-385-2: Reconstruction detail

The structure follows the CC0 MH Tri debug decompilation (InusualZ/mhtri-decomp `ac489be692a1273afff8574b2402aecba2ae2b8f`, `src/ef/ef_drawsmoothstripestrategy.cpp`), whose helpers are separate functions. Its C names were not used.

- **`GetNumDrawParticle` returns `ActivityList::mNumActive` (`+0x50` of the manager).** The tube functions inline it as `lhz 0x50`; the retained weak copy at `0x80242BA0` reads the same field. `ActivityList::GetNumActive` returned the active list's object count (`+0x40`), so it now returns `mNumActive`. No other source used it.
- **The ribbon functions call their helpers out of line; the tube functions inline them.** With no pragma, the ribbon connect functions call `GetEmitterDrawSetting`, `GetDrawOrder`, `GetNumDrawParticle`, `FInv`, `Vertex()`, the `VEC3` operators and `GetYoungestDrawParticle`, as in the target, and the tube connect functions and `Draw` inline all of them. Both follow from the helper structure #384 established.
- **The tube texture scale tests `!= STRIPE_TEXMAP_REPEAT` first.** With inlined `FInv`, `== REPEAT ? 1.0f : FInv(n)` places the blocks the other way round. The ribbon and ring forms keep `==`.
- **Tube `ConnectNone` increments the texture index before the first `GetNext`**, like ribbon `ConnectNone`. Both ring variants increment after each `GetNext`; both emitter variants increment right after the first vertex.
- **Unused `VEC3` locals.** Ribbon `ConnectRing` and `ConnectEmitter` construct two `VEC3` locals that are never used; the target calls `VEC3()` on them out of line. They are declared as `edge0` and `edge1`.
- **`VertexTube` has a four-argument constructor** used for the shifted end rings of tube `ConnectEmitter`, like `Vertex`'s three-argument one.
- **`DrawSmoothStripeTube` takes `bool flushGP`.** It allocates the `(numTubeDivide + 1)` `Trigonometric` table with `__alloca`, fills it with `math::SinCosDeg`, and draws each side through a file-local `BeginTubeSide`, which sets the cull mode and, when asked, sends an eight-vertex degenerate strip.
- **`Draw` inlines `InitGraphics`, the ribbon dispatch `DrawSmoothStripe`, the `AheadContextStripe` constructor and `CalcAxisMtx`.** The four edge vectors are function-local statics in `.bss` `0x8042F978–0x8042F9A8`, guarded in `.sbss`. The type-option `switch` lists `default` before the tube case; the tube flush flag is a `bool` set by an `if`, which places its computation before the argument setup. The view-mode test uses `typeOption == ASSIST_ST_BILLBOARD` (2); the cross test `ASSIST_ST_CROSS` (1).
- **Field roles.** `typeOption0` is the tube side count, `typeOption1` the curve subdivision count (0 means 1), `typeOption2` bits 0–2 the connection type, bits 3–5 the initial previous-axis choice and bits 6–7 the texture mapping. `typeDir` selects the ahead function (`AHEAD_ST_*`, 6 for both neighbours). `EmitterDrawSetting::FLAG_CLIP_DISABLE` (bit 3) forces the flush strip. The tube flush also tests `EffectSystem::mXFFlushSafe` through the first particle's manager, emitter and effect. `AheadContextStripe::mTrigonometric` is at `+0xD4`. Enum names are descriptive (low confidence).

## Remaining difference

`CalcAhead_ParticleBoth_Origin` (`0x8024AC10`, 0x228) is 97.4%. Its body is identical to the other `ParticleBoth` functions apart from the emitter-centre fallback, and the only difference is in the two inlined neighbour searches: the target loads the list's link offset once before each search loop (`lhz r3` for the elder loop, `lhz r6` again for the younger loop), while the rebuild reloads it inside each loop. `DrawStripeStrategy`'s copy at `0x80245C80` has the same shape. Tested without success, each compiled alone with the unit's flags:

- a local `ParticleManager*` or `const ParticleManager*` (the second merges both initial loads);
- explicit loops through the non-const `GetElderParticle`/`GetYoungerParticle` overloads, alone or mixed with the helpers. An explicit non-const loop hoists its load, but two of them share one load;
- non-const overloads of `GetElderDrawParticle`/`GetYoungerDrawParticle` in `DrawStrategyImpl`. Origin then reaches 98.3%, but the overloads break `GetNextDrawParticle_*`, the billboard `CalcAhead_*` and three functions of this unit, so they were reverted;
- moving the `elderDir` declaration between the searches, or adding functions before or after Origin;
- Wii/1.0, 1.3 and 1.5, which give the rebuild's shape.

The working hypothesis is that each search hoists its load in the original but the two searches are not merged. The source form that produces this is unknown.

## Traps

- **`GetNumActive` read the wrong field.** Out-of-line calls hide a wrong inline body, because the weak copy is stripped at link. When a helper is first seen inlined, compare its offsets with the retained weak copy.
- **Const and non-const overloads of an inline helper compile differently.** In this unit, `GetElderParticle(const ParticleManager*)` keeps the link-offset load inside the search loop, and the non-const overload hoists it. Choose the overload from the target's loop shape before you change the call sites.
- **Do not add overloads to `DrawStrategyImpl` to fix one function.** Callers that pass a non-const manager switch to the new overload and lose their matches.
