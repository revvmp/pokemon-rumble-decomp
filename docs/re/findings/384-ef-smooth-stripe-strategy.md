# F-384-1: NW4R ef smooth stripe strategy, first half

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. #384 (`0x80243040–0x80247A64`) covers the end of the stripe strategy file and the start of the smooth stripe strategy file. The smooth stripe file starts in this range at `0x80245EF0` with the `DrawSmoothStripeStrategy` constructor, so #384 owns its split, object line and header. The file is split and registered as `NonMatching`. All eleven functions in #384's part of it match at 100% in objdiff, and the full DOL passes `tools/verify/verify.py` with the split.

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80245EF0–0x8024AE80` | constructor through the destructor at `0x8024AE40`; `g3d_rescommon.cpp` starts at `0x8024AE80` |
| `.data` | `0x803C5728–0x803C5744` | `DrawSmoothStripeStrategy` vtable, seven slots (`0x1C`, was a `0x38` label); zero padding follows up to `g3d_resfile.cpp` at `0x803C5760` |
| `.bss` | `0x8042F978–0x8042F9A8` | four 0xC vectors read by the later functions (`lbl_8042F99C` was a `0x24` label, now 0xC) |
| `.sbss` | `0x804B92F0–0x804B92F4` | their four guard bytes (`lbl_804B92F3` was 5 bytes, now 1) |
| `.sdata2` | `0x804BAA60–0x804BAA94` | `FLT_EPSILON`, `2π/256`, `0.0f`, `1.0f`, two conversion doubles, `0.5f`, `-1.0f`, `0.01f`, `360.0f`, `256/360` (`lbl_804BAA90` now 4 bytes, ending the split at its last constant) |

The unit continues into #385's range at `0x80247A70`. Those functions (`DrawSmoothStripe_BSpline_ConnectRing` through the destructor) are #385's; the source file has no body for them yet.

## Names

The retail debug maps of riptl/wii-symbols (`RZTP01`, `SUPE01`, names only) list the smooth stripe functions with mangled names and sizes equal to the target's. The CC0 Wii Sports map (kiwi515/ogws_info `RSPE_r1.map.csv`) gives the same order with the weak copies. New names, high confidence where a map gives the mangled name and size:

| Address | Name | Evidence |
| --- | --- | --- |
| `0x80245F30` | `DrawSmoothStripeStrategy::CalcVertex` | RZTP01, SUPE01, 0x3A0 |
| `0x802462D0` | `DrawSmoothStripeStrategy::CalcVertexTube` | Wii Sports order; signature from the source (medium) |
| `0x80246730` | `DrawSection_BSpline` | RZTP01, 0x2CC |
| `0x80246A00` | `DrawSectionEnd_BSpline` | RZTP01, 0x21C |
| `0x80246C20` | `DrawSectionTube_BSpline` | RZTP01, 0xABC |
| `0x802476E0` | `GXEnd` (local weak copy) | Wii Sports order |
| `0x802476F0` | `DrawSmoothStripe_BSpline_ConnectNone` | RZTP01, SUPE01, 0x344 |
| `0x80247A40` | `GetDrawOrder` (const) | RZTP01, SUPE01, 0xC |
| `0x80247A50` | `GetStripeTexmapType` (const) | Wii Sports order; mangling as the stripe strategy's (medium) |
| `0x80247A60` | `Vertex::Vertex()` | Wii Sports order |
| `.data` `0x803C5728` | `__vt__Q34nw4r2ef24DrawSmoothStripeStrategy` | constructor store |

In the stripe part of the range, which this issue does not reconstruct, the same maps and the stripe vtable at `0x803C5708` name `DrawStripeStrategy::DrawTubeSegment` (`0x802434A0`), `DrawStripeTube_ConnectNone`, `_ConnectRing`, `_ConnectEmitter` (`0x80243880`, `0x80243B40`, `0x80243E80`), `DrawStripeTube` (`0x80244240`), `Draw` (`0x802449F0`), `GetCalcAheadFunc` (`0x80245540`) and the destructor (`0x80245EB0`). `0x80243040` is the stripe `CalcVertexTube`, the same size as the smooth one; `0x80245290`–`0x80245C80` are `GetInitialPrevAxis` and the four `CalcAhead_*` functions in Wii Sports order, left unnamed because no map gives their signatures.

## Scope boundary

`ef_drawstripestrategy.cpp` starts at `0x80242820` in #383's range, so #383 owns its split. Its functions from `0x80243040` to `0x80245EB0` (13 of #384's 25) stay unsplit dtk objects until that split exists. They have no public source; the smooth stripe source here is their closest model, since the two files share `CalcVertexTube`, the tube segment writer and the `CalcAhead_*` set with equal sizes.

# F-384-2: Reconstruction detail

The source follows the structure of the CC0 MH Tri decompilation (InusualZ/mhtri-decomp `ac489be692a1273afff8574b2402aecba2ae2b8f`, `src/ef/ef_drawsmoothstripestrategy.cpp`), a debug build in which the helpers are separate functions. Its C names and asserts were not used.

- **The frame helpers return matrices by value.** MH Tri passes an output matrix before `self` to its frame and rotation helpers, which is CodeWarrior's hidden return slot of a member function. Written as inline members `CalcAxisMtx` and `CalcRotateMtx` that `return math::MTX34(...)`, `CalcVertex` matches; with an output-pointer parameter assigned from a temporary, the stack grows by 0x60 and the function scores 78%. `CalcVertexTube` uses `CalcAxisMtx` and builds its own scale matrix.
- **The axis helper takes the ahead vector by reference.** The target keeps its address in a register across both cross products, which a parameter reproduces and a local does not.
- **VEC3 assignment and copy construction compile differently.** Member assignment (`vertex.pos0 = sum`) copies floats; the copy constructor (`Vertex(rPos0, rPos1, t)`) copies words, as the target's weak `Vertex` constructor at `0x80248380` does. The blend helpers default-construct the result and assign each member.
- **The spline weight is `(0.5 + (0.5t² − t), 0.5 + (−t² + t), 0.5t²)`.** Writing `0.5f * t2` twice rather than through a local `halfT2` gives the target's register allocation in `DrawSection_BSpline`; `t` is computed into a local before the call, so the weight helper does not re-evaluate it.
- **The tube writer indexes the trigonometric table.** `pTrig[j]` matches; a walking pointer loads the table address one instruction early.
- **`ConnectNone` increments the texture index before the first `GetNext` and after it in the loop.** Sin and cosine stack slots follow declaration order: `f32 c, s;` in `CalcRotateMtx`, `f32 s, c;` in `CalcVertexTube`.
- The draw flags are `DRAWFLAG_TEXCOORD` (1), `DRAWFLAG_VIEW_AXIS` (8, cross with the context's view axis instead of the particle's previous axis) and `DRAWFLAG_REVERSE` (0x10, ring order). `typeOption2 & 0xC0` equal to `0x40` (`STRIPE_TEXMAP_REPEAT`) makes each particle span one texture repeat; otherwise the coordinate is normalized over the stripe. These enum names are descriptive (low confidence). `AheadContextStripe` adds `mEmitterAxisX` (`+0xBC`, fallback axis) and `mViewAxisZ` (`+0xC8`) to `AheadContext`; further members used by the later functions are not declared yet.

## Traps

- **Out-of-line weak inlines can come back when unrelated helpers become inline members.** `ConnectNone` calls `GetDrawOrder`, `GetStripeTexmapType`, `Vertex()`, `math::FInv`, `GetNumDrawParticle` and `GXEnd` out of line in the target, although `DrawSection_BSpline` inlines `FInv`. With the frame computation written inline in `CalcVertex`, the rebuild inlined all of them into `ConnectNone` (0%). After the frame code moved into the inline members `CalcAxisMtx` and `CalcRotateMtx`, the rebuild called them out of line exactly as the target does, with no pragma. The mechanism is not established; when a target calls trivial inlines out of line, first rebuild the earlier functions of the file with the original helper structure before reaching for `dont_inline`.
- **Return-slot parameters in a debug reference mean return by value.** An output pointer that a debug decompilation passes before `self` is the hidden return slot. Return the object by value.

# F-384-3: complete smooth-stripe iterator consumer probe

The existing canonical smooth-stripe source was independently compiled
without changing its implementations, types, layout or established Wii/1.1
NW4R flags. The baseline reproduces the recorded Origin mismatch:
text99.928696%, Origin97.391304%, with all four data sections100%.
The #383 owner supplied a bounded shared-header proposal: the existing
const GetElderParticle and GetYoungerParticle overloads forward to their
already existing nonconst overloads. No overload, member or signature is
added. The nonconst implementation uses the same list links and reads
without mutating the manager. This is a target-supported equivalent
implementation; original source spelling is not recovered.

With only those two forwarding changes in an external diagnostic include
overlay, the unchanged whole source scores100% in all five allocated
sections. An independent strict audit resolves all513 text relocations and
reproduces every byte of all28 retained functions /20,196 bytes. The28-byte
vtable and52-byte constant payload are exact, including five vtable
relocations; BSS48 and small BSS4 extents are unchanged. The source emits
thirteen additional weak functions whose retained copies belong to other
units. These receive no source-byte or function credit, and diagnostic
comparison does not substitute for an actual source-selected full link.

This differs from the failed F-385-2 probe that added new nonconst
DrawStrategy overloads and changed unrelated overload resolution. It also
preserves main's already corrected GetNumActive implementation; the older
proposal's direct-field hunk is unnecessary. Shared-header publication is
coordinated with the #383 owner, who will prove its existing consumers
before the source-selection gate. The smooth unit remains NonMatching.
#384's fourteen stripe functions remain in the #383-owned complete stripe
unit; its source and interface changes stay with that owner.

## Traps

- A whole-object score can include compiler-emitted weak duplicates that
  are discarded in the real link. Enumerate native retained functions and
  resolve their relocations independently; count only the retained28 here,
  then prove the final ELF/map/source selection and full frozen DOL.
- A const-forward implementation and a newly added nonconst overload are
  different changes. The former preserves the existing overload set;
  the latter can silently change every caller's selected implementation.

# F-384-4: coordinated source-selected acceptance and native helper order

The complete source-selected proof is recorded in [F-383-4](383-ef-stripe-native.md#f-383-4-source-selected-stripe-and-coordinated-smooth-stripe-acceptance). The #384 first-function owner approved the exact `Vertex()` declaration with its unchanged empty inline definition after the first `ConnectNone` body. This preserves every #385 body and all existing source provenance. It is inferred from native weak emission, not claimed as uniquely recovered original source placement.

The actual complete link verifies all 28 retained Smooth functions / 20,196 bytes / 513 text relocations, initialized vtable28 and constants52, BSS48/SBSS4, exact final ELF/map positions and source-only inputs. Together with the #383-owned complete Stripe unit, the strict original #384 audit covers all25 rows /18,856 bytes. The 13 discarded weak copies have no credit; source #385 is already canonical and receives no duplicate reconstruction claim or automatic closure.

## Traps

- Earlier per-function and paired-target proofs did not establish weak constructor placement. The first real source-selected CHECK failed because `Vertex()` appeared before `DrawSectionEnd_BSpline`. Keep retained layout and actual final source-selected addresses as mandatory gates even when every section scores100%.
