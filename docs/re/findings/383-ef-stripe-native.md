# F-383-2: native stripe ownership and reconstruction probes

## Scope and ownership

The original issue survey is not a source-file boundary. The directional tail at `0x802408C0–0x80241320` belongs to the file whose first function is in #382. That issue is assigned to another contributor; this investigation does not change its source, header, split, or object registration. The existing free, line, and point strategy units retain their established boundaries from F-383-1.

`DrawStripeStrategy` begins with its constructor at `0x80242820`, inside #383. The complete native file continues through its destructor at `0x80245EB0`, ending at `0x80245EF0`, inside #384. #383 therefore owns that whole file's declaration and registration. `DrawSmoothStripeStrategy` starts at `0x80245EF0` and remains outside this claim. No new source registration, matching designation, or progress credit is established by these diagnostic probes.

## Reference evidence

The CC0 Forecast reference at commit `a1f28f8a0d777ebd12953f5c6997ee2decfd07f5` supplies an older ribbon-only implementation. The CC0 News Channel reference at commit `128e3ffbaee9e70bd175c5f66a44fb8430068629` is also ribbon-only and supplies useful member names. Neither implements this target's tube and texture-mapping variants. The CC0 MHTri reference at commit `ac489be692a1273afff8574b2402aecba2ae2b8f` describes the newer tube algorithm in a debug build, but its offset-based helper structures and instruction-control pragmas are reading aids rather than acceptable reconstructed source.

The SUPE01 symbol map independently records the exact 820-byte `DrawStripe` and 1,096-byte `DrawParticle` signatures. This target retains those sizes and native call structure. `DrawStripe` takes the derived `AheadContextStripe`, an integer drawing flag word, and two vector references. `DrawParticle` takes that context and flag word before the particle, ahead callback, three vector references, pivot, and longitudinal texture coordinate. The older reference signatures are different.

## Individual compiler evidence

All comparisons below use the immutable `orig/wpse-v1/main.dol`, Wii/1.1 and the existing `cflags_nw4r` options. Diagnostic files live outside the repository under `383-reference`; no original bytes are embedded in source. The search compares every non-relocation instruction word. It does not by itself establish relocation targets, complete initialized data, actual build selection, or full DOL acceptance.

An ordinary C++ rewrite of `DrawParticle` reproduces all 1,096 bytes of non-relocation instructions at `0x80242BE0`. The rewrite uses the canonical particle and manager types, the existing vector/matrix functions, and `PSSinCosRad`. Its frame crosses the ahead vector with either the particle's previous axis or the context's view Z axis according to flag `8`; degenerate cross products use the emitter X axis. The resulting up axis becomes the particle's new previous axis. Flag `1` controls texture-coordinate writes. These are functional differences from the older ribbon source, rather than arbitrary code-generation edits.

The idiomatic updated `DrawStripe` algorithm produces 796 bytes with ordinary automatic inlining. A bounded `dont_inline` region, also present around that function in the pinned Forecast reference, reproduces the target's complete 820-byte non-relocation stream at `0x80242860`, including the independently emitted `GetStripeTexmapType` and `FInv` helpers. This is a diagnostic result; the final implementation must establish the complete file's original helper emission and calling context before adopting an instruction-control pragma. The empty constructor also matches its native 60-byte stream. Identical empty destructors and return instructions have multiple raw matches and require target-aware relocation and vtable proof before attribution.

The earlier ribbon implementation also reproduces the native ahead-vector bodies at `0x80245610`, `0x80245730`, and `0x802459B0`; its center-connected variant is twelve bytes longer than the target. The native `GetCalcAheadFunc` additionally handles direction value `7` with the same no-design branch as value `5`, which the older source lacks. Its complete callback signatures and emitted targets remain to be proved before proposing shared declarations.

## Free strategy diagnostics

Recompiling the existing free strategy with the canonical headers reproduces three retained functions exactly and leaves its `Draw` body at 2,132 bytes against the native 2,156. Automatic deferred inlining produces the same result. Diagnostic optimization changes to `-O3,p`, `-O2,p`, and disabled peephole optimization do not repair the function and introduce other differences. No compiler option or shared inline definition was changed in the repository on the strength of these failures.

## Traps

- **A newer stripe signature changes real behavior.** The older ribbon's previous-axis output parameter and boolean texture argument cannot simply be renamed to the native integer flags. The target has a view-facing frame branch and a separate texture-mapping mode; reconstruct those observed operations with the canonical types.
- **A raw function search is only a preliminary gate.** The exact `DrawParticle` stream is useful reconstruction evidence, while relocation pairing, complete native unit data, source-selected link inputs, and the frozen full build remain necessary before a matching claim.
- **Diagnostic scripts can overwrite valuable candidates.** The one-shot older-reference preparation script recreates the old ribbon candidate. Preserve the modern candidate separately; do not rerun the older preparation over the newer tube investigation.
