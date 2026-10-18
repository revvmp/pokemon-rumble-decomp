# F-383-2: native stripe ownership and reconstruction probes

## Scope and ownership

The original issue survey is not a source-file boundary. The directional tail at `0x802408C0–0x80241320` belongs to the file whose first function is in #382. At this investigation's initial claim, that issue was assigned to another contributor; it subsequently became unassigned. This #383 investigation does not change its source, header, split, or object registration. The existing free, line, and point strategy units retain their established boundaries from F-383-1.

`DrawStripeStrategy` begins with its constructor at `0x80242820`, inside #383. The complete native file continues through its destructor at `0x80245EB0`, ending at `0x80245EF0`, inside #384. #383 therefore owns that whole file's declaration and registration. `DrawSmoothStripeStrategy` starts at `0x80245EF0` and remains outside this claim. No new source registration, matching designation, or progress credit is established by these diagnostic probes.

The original #383 table accounts for all 24 functions and 9,976 bytes without treating the neighboring file boundaries as survey boundaries:

| Original inventory group | Functions | Code bytes | Acceptance state |
| --- | --- | --- | --- |
| Directional file tail owned by #382 | 7 | 2,612 | Outside this claim's file ownership |
| Free strategy | 4 | 2,592 | Existing NonMatching unit; `Draw` remains different |
| Line strategy | 3 | 1,576 | Already Matching before this investigation |
| Point strategy | 3 | 1,172 | Already Matching before this investigation |
| First seven retained stripe functions | 7 | 2,024 | Exact individual probe; complete unit acceptance pending |
| Total | 24 | 9,976 | Issue remains incomplete |

The complete stripe file adds fourteen neighboring #384 functions, 11,884 bytes, beyond its seven original #383 rows. Its complete retained inventory is therefore 21 functions and 13,908 bytes. These neighboring functions must be attributed once, rather than counted again when #384's remaining smooth-stripe work is delivered.

## Reference evidence

The CC0 Forecast reference at commit `a1f28f8a0d777ebd12953f5c6997ee2decfd07f5` supplies an older ribbon-only implementation. The CC0 News Channel reference at commit `128e3ffbaee9e70bd175c5f66a44fb8430068629` is also ribbon-only and supplies useful member names. Neither implements this target's tube and texture-mapping variants. The CC0 MHTri reference at commit `ac489be692a1273afff8574b2402aecba2ae2b8f` describes the newer tube algorithm in a debug build, but its offset-based helper structures and instruction-control pragmas are reading aids rather than acceptable reconstructed source.

The SUPE01 symbol map independently records the exact 820-byte `DrawStripe` and 1,096-byte `DrawParticle` signatures. This target retains those sizes and native call structure. `DrawStripe` takes the derived `AheadContextStripe`, an integer drawing flag word, and two vector references. `DrawParticle` takes that context and flag word before the particle, ahead callback, three vector references, pivot, and longitudinal texture coordinate. The older reference signatures are different.

## Individual compiler evidence

All comparisons below use the immutable `orig/wpse-v1/main.dol`, Wii/1.1 and the existing `cflags_nw4r` options. Diagnostic files live outside the repository under `383-reference`; no original bytes are embedded in source. The search compares every non-relocation instruction word. It does not by itself establish relocation targets, complete initialized data, actual build selection, or full DOL acceptance.

An ordinary C++ rewrite of `DrawParticle` reproduces all 1,096 bytes of non-relocation instructions at `0x80242BE0`. The rewrite uses the canonical particle and manager types, the existing vector/matrix functions, and `PSSinCosRad`. Its frame crosses the ahead vector with either the particle's previous axis or the context's view Z axis according to flag `8`; degenerate cross products use the emitter X axis. The resulting up axis becomes the particle's new previous axis. Flag `1` controls texture-coordinate writes. These are functional differences from the older ribbon source, rather than arbitrary code-generation edits.

The idiomatic updated `DrawStripe` algorithm produces 796 bytes with ordinary automatic inlining. A bounded `dont_inline` region, also present around that function in the pinned Forecast reference, reproduces the target's complete 820-byte non-relocation stream at `0x80242860`, including the independently emitted `GetStripeTexmapType` and `FInv` helpers. This is a diagnostic result; the final implementation must establish the complete file's original helper emission and calling context before adopting an instruction-control pragma. The empty constructor also matches its native 60-byte stream. Identical empty destructors and return instructions have multiple raw matches and require target-aware relocation and vtable proof before attribution.

The earlier ribbon implementation also reproduces the native ahead-vector bodies at `0x80245610`, `0x80245730`, and `0x802459B0`; its center-connected variant is twelve bytes longer than the target. The native `GetCalcAheadFunc` additionally handles direction value `7` with the same no-design branch as value `5`, which the older source lacks. Its complete callback signatures and emitted targets remain to be proved before proposing shared declarations.

### Expanded complete-file probe

The subsequent idiomatic candidate contains all retained strategy operations, including tube vertex calculation, tube segments, the three connection modes, tube dispatch, the main draw, initial-axis calculation, and all four ahead callbacks. The pinned Wii Sports identifier map, `kiwi515/ogws_info` commit `bcf62e22f9e7fc1d75acbb24bd8368b03cba1c81`, supplies the member names `CalcVertexTube`, `DrawTubeSegment`, `GetInitialPrevAxis`, and the callback names. Its `GXEnd` entry also corrects the older reference's inferred `EndStripe` member wrapper: the native four-byte helper is the existing SDK `GXEnd`.

With the explicitly diagnostic bounded inlining region around `DrawStripe`, 19 of the complete unit's 21 retained functions reproduce 11,392 native code bytes. The outside audit independently decodes and checks 212 text relocations for those exact bodies, all five vtable targets, and all 80 initialized bytes: the 28-byte vtable at `0x803C5708` and 52-byte constant pool at `0x804BAA28`. `DrawStripeTube` has the correct 1,964-byte size but differing register allocation. The origin-connected ahead callback remains 564 bytes against the target's 552. Neither function is accepted, and the complete file has not undergone source-selected build verification.

The target-proven complete nested context adds emitter X axis at `0xBC`, view Z axis at `0xC8`, and a trigonometric-table pointer at `0xD4`, making it `0xD8`. The pointer is initialized to null before the constructor body transforms the emitter axis. That initialization order restores the native 2,208-byte main `Draw`. Its initial-axis calculation reproduces all 684 bytes when every return uses the same named local vector; an early direct member return prevents the compiler's named return optimization and adds 48 bytes.

Two bounded shared-interface corrections are independently evidenced, but remain outside-only proposals pending consumer verification. First, native `GetNumDrawParticle` at `0x80242BA0` reads the existing typed `ActivityList::mNumActive` field at manager offset `0x50`, whereas the current canonical helper calls `GetNumActive`, which reads the list's object count at `0x40`. These counts differ during closing transitions. The News Channel reference exposes both accessors; its `GetNumActiveCount` name for the latter field is explicitly inferred rather than established as an original symbol. Second, native and mapped `CalcAhead_NoDesign` has the unused third `Particle*` argument, also present in that reference. The current canonical two-argument declaration emits a distinct weak symbol when its address is taken. The paired native target is `0x802412B0`; a corrected declaration must coalesce with that existing callback rather than retain a duplicate body under the wrong signature.

Changing both shared list-walker manager arguments to mutable pointers shortens the origin callback to the correct size, but leaves nine instruction differences and breaks the other three previously exact callbacks. This is not an accepted shared-header correction. Natural flat and pair-array table forms and signed/unsigned private flush-helper parameter variants leave the tube dispatcher's register permutation unresolved. A complete Wii/1.3 compiler control loses three previously exact routines, retaining only 16 of 21 exact bodies; its origin callback grows to 572 bytes. The candidate therefore retains the established Wii/1.1 compiler and the canonical const walkers, rather than adopting a flag or overload workaround.

The tube dispatcher was subsequently resolved by restoring the resource-accessor structure evidenced in the MHTri debug reference. A single inline accessor for the tube division count is used consistently by all three connection paths and the dispatcher. Its inferred descriptive name is `GetStripeTubeDivide`; no claim is made that the original spelling was recovered. This additional inline level preserves the already exact connection bodies and restores all 1,964 dispatcher bytes, without a compiler flag, pragma, new storage, or competing type. The resulting current probe has 20 exact functions, 13,356 bytes, 360 independently paired text relocations, five exact vtable targets and the same complete 80 initialized bytes. Only the origin-connected ahead callback remains unaccepted. The earlier 19-function result and failed controls above describe the preceding candidate, not a second source-credit contribution.

### Complete native probe and existing consumers

The origin callback is resolved by forwarding the two existing const `GetElderParticle` and `GetYoungerParticle` implementations to their existing non-const implementations. Their signatures do not change, no overload or new helper is added, and the implementations continue to read the manager without mutating it. This avoids duplicating the link macro in the const implementations and retains the original shared operation through another inline level. Unlike changing the list walkers' manager parameter types, it preserves all three previously exact callbacks and restores all 552 origin-callback bytes.

The resulting complete candidate reproduces every non-relocation instruction in all 21 retained functions, totaling 13,908 bytes. The independent paired audit verifies 369 text relocations, all five vtable pointers, and the complete 80 initialized data bytes. The bounded `DrawStripe` inlining region remains explicitly diagnostic pending review of the complete source emission; these results do not yet establish a source-selected frozen build or new progress credit.

Two direct existing source consumers were recompiled independently with the pinned Wii/1.1 options against both original and proposed headers. All 19 emitted `DrawStrategyImpl` functions and all 15 emitted billboard functions are byte-identical. Every object section's metadata, contents and relocations are unchanged except strictly numeric local `.strtab` labels. This is a focused consumer check, not a substitute for a fresh complete `all_source` baseline and actual frozen build before canonical publication.

### Ordinary complete source and neighboring smooth-stripe evidence

The later canonical smooth-stripe delivery in PR #902 supplies eleven functions from the separate file beginning at `0x80245EF0`. It does not change the directional tail or free-strategy acceptance above, so it does not complete the original #383 inventory. Its own remaining smooth-stripe functions continue into #385.

The complete stripe candidate now uses the evidenced non-static matrix helpers. `CalcAxisMtx` takes the context, flag word, particle, axis vector reference and position reference; both matrix helpers return their matrices by value. The native debug calling convention places the hidden result pointer before the implicit object argument, which distinguishes these members from the older references' static helpers. The virtual ahead-function getter retains the base-context callback type, while the stripe drawing caller explicitly converts it to the derived-context callback type. The four retained stripe callbacks themselves take the base context, consistent with their native declaration evidence.

The final inlining discrepancy was resolved through an actual repeated drawing operation, without an instruction-control pragma. A file-local inline `DrawVertexPair` emits the two transformed edge positions and their optional longitudinal texture coordinates through the existing vector-position helper. This helper structure is also present in canonical smooth-stripe source and the newer debug reference. Factoring those repeated writes out of `DrawParticle` restores the native outlining of `GetNumDrawParticle`, `GetStripeTexmapType`, `FInv` and `GXEnd` in `DrawStripe`, while preserving all other exact routines. Merely correcting the matrix helper declarations or moving their definitions before `DrawStripe` did not restore this emission.

The ordinary source, with no `dont_inline` region or other instruction-control pragma, now reproduces all 21 retained functions and 13,908 code bytes. The independent audit again verifies all 369 text relocation targets, five vtable pointers, and all 80 initialized bytes. The earlier bounded-pragmas result is superseded by this complete ordinary-source probe. Actual source registration, canonical-header consumer verification, source-selected link inputs and frozen full-build acceptance remain pending; no new progress is claimed by this diagnostic result.

Current main already includes the corrected three-argument `CalcAhead_NoDesign` declaration. A later prerequisite must preserve that canonical declaration and propose only the activity-count selection and equivalent const-getter forwarding still absent from main, rather than reintroduce a duplicate signature change.

The ordinary candidate was subsequently compiled against a complete tracked-header snapshot of main `f8602abba0f8cd994902ffe8be11fa6c55ff72c5`, retaining only those two bounded shared changes. All 21 functions, 369 text relocations, five vtable targets and 80 data bytes remain exact. A focused original/proposed compilation of all eight existing strategy source files produces 78 functions: 77 are unchanged, and the sole changed function is the eight-byte weak `GetNumDrawParticle` emitted by the NonMatching smooth-stripe object. Its old bytes `a06400404e800020` decode to `lhz r3,0x40(r4); blr`; the proposed bytes `a06400504e800020` decode to `lhz r3,0x50(r4); blr`. The immutable DOL hash was independently checked before reading native `0x80242BA0`, whose complete eight bytes are exactly the proposed sequence. This intentional correction must be identified separately in the future complete baseline audit; it is not an assertion that every existing NonMatching object remains identical.

The complete ordinary source and owned class declaration are now preserved as a draft proposal. The source is deliberately unregistered while the bounded shared prerequisite and split-only build gate remain pending. It is not marked Matching and does not replace fallback bytes in the current link. The proposed final unit ranges are `.text 0x80242820–0x80245EF0`, `.data 0x803C5708–0x803C5724`, and `.sdata2 0x804BAA28–0x804BAA5C`; padding before the following smooth-stripe unit is outside these last owned objects. These ranges require actual split-only verification before registration and full source-selected acceptance.

## Free strategy diagnostics

Recompiling the existing free strategy with the canonical headers reproduces three retained functions exactly and leaves its `Draw` body at 2,132 bytes against the native 2,156. Automatic deferred inlining produces the same result. Diagnostic optimization changes to `-O3,p`, `-O2,p`, and disabled peephole optimization do not repair the function and introduce other differences. No compiler option or shared inline definition was changed in the repository on the strength of these failures.

Three ordinary helper-structure controls also retain the same 2,132-byte mismatch: extracting the four repeated texture-flag tests in `DrawQuad` into a typed inline helper, declaring `CalcLocalTransform` inline, and declaring `InitGraphics` inline. Each preserves the three other exact native functions. These outside-only controls do not establish a repair and are not proposed source changes.

## Traps

- **A newer stripe signature changes real behavior.** The older ribbon's previous-axis output parameter and boolean texture argument cannot simply be renamed to the native integer flags. The target has a view-facing frame branch and a separate texture-mapping mode; reconstruct those observed operations with the canonical types.
- **A raw function search is only a preliminary gate.** The exact `DrawParticle` stream is useful reconstruction evidence, while relocation pairing, complete native unit data, source-selected link inputs, and the frozen full build remain necessary before a matching claim.
- **Diagnostic scripts can overwrite valuable candidates.** The one-shot older-reference preparation script recreates the old ribbon candidate. Preserve the modern candidate separately; do not rerun the older preparation over the newer tube investigation.
- **The activity counter and list length are different fields.** A matching old helper name can still select the wrong count. Preserve `GetNumActive` semantics for its existing callers and use the target-proven separate activity counter for stripe drawing.
- **Equivalent const getters can expose a different inline call structure.** The duplicated link macro and forwarding versions of the existing const getters preserve the same types and behavior, but differ in this origin callback's loop optimization. The forwarding form also preserves the measured existing consumers. Retain full consumer proof rather than generalizing this observation into a compiler-wide rule.
- **A missing repeated helper can change unrelated outlining.** Direct vertex writes made the stripe dispatcher inline four tiny callees and become 24 bytes shorter. Restoring the evidenced vertex-pair helper gives the exact original emission without a pragma. Diagnose the complete original helper structure before retaining an instruction-control workaround.

# F-383-4: source-selected stripe and coordinated smooth-stripe acceptance

The #383 first-function owner supplies the complete Stripe unit and its fourteen neighboring #384 functions. The claimed #384 owner explicitly authorizes accepting canonical Smooth source and the exact bounded empty-constructor placement correction below. Original #385 remains externally assigned and receives no closure here; none of its existing function bodies changes. The shared Base prerequisite is now canonical through PR #945; its earlier header proof carries no duplicate source-byte credit.

## Split and native source gates

The split-only gate at `cd92339781a95611f30d82af43d2ba49f9011c35` runs actual configure, `all_source`, default Ninja and frozen verification, all exit 0. Only owned boundaries and symbols change. The owned vtable is 28 bytes, not its older 32-byte placeholder; the last float is four bytes, leaving the following unit's alignment outside the split. The following Smooth data and constant sections already have the required eight-byte alignment.

Actual unitdiff then reports all 21 Stripe functions / 13,908 bytes and all three sections 100%; canonical Smooth reports all 28 / 20,196 and all five sections 100%. The first source-selected link correctly fails the frozen CHECK despite those scores. It emits Stripe's local SDK `GXEnd` before `DrawParticle`, and Smooth's weak `Vertex` constructor before `DrawSectionEnd_BSpline`, shifting some native addresses. Verification is not run after that failed Ninja result. Raw instruction identity alone was therefore insufficient.

The measured ordinary-source repair changes only helper declaration/definition placement. Stripe's existing `DrawParticle` is declared inline and defined before `DrawStripe`; Forecast's equivalent older `DispVertex` helper is also inline. The same function body remains intact. Smooth's existing empty `Vertex` constructor is declared in its class and defined inline after the first `ConnectNone` function. The #384 owner reviewed and approved that exact one-line declaration and two-line definition diff, conditional on complete native and actual-link proof. It changes no signature, member, layout, vtable, or #385 function body. These placements are native-emission evidence, not uniquely recovered original source positions.

Both candidates retain every native instruction stream and independently paired target: Stripe 21 / 13,908 / 369 text sites, five vtable sites and 80 initialized bytes; Smooth 28 / 20,196 / 513 text sites, 28-byte vtable, 52 constants, BSS48 and SBSS4. An outside layout gate verifies complete retained order and alignment-aware offsets before another full link. Stripe's seven and Smooth's thirteen extra weak definitions are unretained and receive no credit. Raw object offsets include those discarded copies, so the retained projection and actual linked positions are checked separately. The SDK header, compiler options, function alignment and function instructions are unchanged. A reference-backed `dont_inline` diagnostic did not fix the order and is rejected.

The subsequent actual source-selected gate at `81744e707a101a87c362f0f91e3881e4f38fae76`, on main `11c269cc`, passes configure, complete 601-target `all_source`, default Ninja and explicit frozen verification, all exit 0. Strict audits verify every final native function address and size, every owning map contribution, actual source link inputs with fallbacks excluded, both complete initialized payloads, and full DOL identity. This is the first source-selected acceptance checkpoint; later main rebases still require their own current-baseline and exact-head gates.

Evidence: `383-reference/source-split-20261017-183105` for split-only validation; `source-split-20261017-183254` for the rejected first source link; `source-split-20261017-183828` for the passing actual source gate; `retained-layout-probes` for bounded source/header diffs, full native/data comparisons and projected layouts; `stripe-actual-acceptance` for actual Stripe paired/native/map/input proof; and `384-reference/smooth-actual-paired-native.json` plus `384-complete-original-inventory.json` for Smooth and original issue acceptance. Reproducers are `383-audit-stripe-actual.py`, `384-audit-smooth-actual.py` and `384-audit-original-inventory.py`, each taking the actual build directory. They do not modify source or expected hashes.

## Complete original #384 inventory

Every original row is joined by its actual native address, size, compiled source symbol and owning map object. The 25 functions / 18,856 bytes comprise fourteen Stripe functions / 11,884 bytes and eleven Smooth functions / 6,972 bytes. Their containing files are both selected from source and the frozen DOL is exact. This supports completing #384 only after the final merged gate, without duplicate credit for any inherited Smooth body or neighboring #385 work.

| Native address | Bytes | Complete source unit | Function |
| --- | --- | --- | --- |
| `0x80243040` | 1112 | Stripe | `CalcVertexTube` |
| `0x802434a0` | 988 | Stripe | `DrawTubeSegment` |
| `0x80243880` | 696 | Stripe | `DrawStripeTube_ConnectNone` |
| `0x80243b40` | 828 | Stripe | `DrawStripeTube_ConnectRing` |
| `0x80243e80` | 956 | Stripe | `DrawStripeTube_ConnectEmitter` |
| `0x80244240` | 1964 | Stripe | `DrawStripeTube` |
| `0x802449f0` | 2208 | Stripe | `Draw` |
| `0x80245290` | 684 | Stripe | `GetInitialPrevAxis` |
| `0x80245540` | 204 | Stripe | `GetCalcAheadFunc` |
| `0x80245610` | 284 | Stripe | `CalcAhead_Particle_Stripe` |
| `0x80245730` | 632 | Stripe | `CalcAhead_ParticleBoth_Stripe` |
| `0x802459b0` | 712 | Stripe | `CalcAhead_ParticleBoth_Ring` |
| `0x80245c80` | 552 | Stripe | `CalcAhead_ParticleBoth_Origin` |
| `0x80245eb0` | 64 | Stripe | `constructor` |
| `0x80245ef0` | 60 | Smooth stripe | `constructor` |
| `0x80245f30` | 928 | Smooth stripe | `CalcVertex` |
| `0x802462d0` | 1112 | Smooth stripe | `CalcVertexTube` |
| `0x80246730` | 716 | Smooth stripe | `DrawSection_BSpline` |
| `0x80246a00` | 540 | Smooth stripe | `DrawSectionEnd_BSpline` |
| `0x80246c20` | 2748 | Smooth stripe | `DrawSectionTube_BSpline` |
| `0x802476e0` | 4 | Smooth stripe | `GXEnd` |
| `0x802476f0` | 836 | Smooth stripe | `DrawSmoothStripe_BSpline_ConnectNone` |
| `0x80247a40` | 12 | Smooth stripe | `GetDrawOrder` |
| `0x80247a50` | 12 | Smooth stripe | `GetStripeTexmapType` |
| `0x80247a60` | 4 | Smooth stripe | `constructor` |

Original #383 remains incomplete: its containing Directional and Free units still have unresolved large draws. Earlier accepted Line/Point code is inherited, not new work. Stripe contributes 13,908 reconstructed code bytes; Smooth's pre-existing 20,196 source bytes become accepted rather than newly rewritten. Neither discarded weak copies nor PR #945's header correction is counted again.

## Traps

- A 100% unit score can conceal an early helper and wrong linked addresses. Check complete retained layout before linking and every final ELF/map address afterward; keep the failed CHECK as evidence rather than trusting a previous DOL.
- Two local functions can share `GXEnd`. Use name, native address, size and owning map object for Stripe; a global name-to-one-address dictionary can select Smooth's later copy.
- Moving an ordinary inline definition can restore native weak emission without changing function instructions. Prove every retained body, target and final allocation; do not infer a universally correct source position or add a compiler-control workaround.
