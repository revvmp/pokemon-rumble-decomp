# F-382-3: complete directional source investigation

## Ownership and original inventory

The helper claimed the open, unassigned #382 after the faithful partial #383 source draft was published. The earlier contributor had released #382 after partial deliveries #883 and #891. This claim retains those canonical sources and owns the complete directional file beginning at `0x8023EFA0`, including seven neighboring #383 tails through `0x80241320`. It does not replace the first-owner rules with survey boundaries.

The original #382 inventory contains 26 functions and 10,072 code bytes. Nine rows, 3,724 bytes, belong to the previously accepted billboard file. The other seventeen rows, 6,348 bytes, belong to the directional file. Its seven neighboring #383 rows add 2,612 bytes, yielding 24 retained functions and 8,960 bytes for the complete native directional unit. Inherited billboard work and these neighboring tails must each be counted once.

## Ordinary source probe

The existing canonical source supplies `DrawDirectional`, its matrix helpers and both typed `DrawQuad` overloads. The newer view-space algorithm is recovered from the immutable target and the CC0 MH Tri reading aid at commit `ac489be692a1273afff8574b2402aecba2ae2b8f`, `src/ef/ef_drawbillboardstrategy.cpp`. Its C offset structures and instruction-control pragmas are not copied. The separate older Forecast directional source at `a1f28f8a0d777ebd12953f5c6997ee2decfd07f5`, `src/nw4r/ef/drawstrategy/ef_drawdirectionalstrategy.cpp`, and the pinned Whippy interfaces provide naming and revision comparisons.

The outside candidate rewrites the view-space path as the real `DrawDirectionalStrategy::DrawDirectionalBillboard` member, using canonical particle, manager, context, matrix and vector types. It transforms the ahead vector into view space, constructs the perpendicular frame with a previous-axis fallback, applies the emitter-axis scales, and emits the two optional crossing quads. It also restores the getter cases `5` and `7` for no-design direction and `6` for the two-neighbor direction.

With Wii/1.1 and the unchanged `cflags_nw4r` options, ordinary source reproduces the complete 1,588-byte non-relocation instruction stream at `0x80240B10`. The same compilation produces exact raw streams for the 152-byte getter, 868-byte `CalcRotate`, 504-byte `CalcLocalTransform`, and the weak copies needed by the view-space path. The fallback axis uses the existing typed static `mXUnitVec` directly, as the native caller does; routing it through the existing accessor introduces a call absent from the target. No instruction-control pragma is used.

These are individual source probes, not source-selected acceptance. The first independent paired-target audit checks 21 mapped exact bodies / 4,780 bytes and decodes 142 native text relocation targets, including the complete view-space draw. Every repeated target is consistent. Constants and outside weak helper names remain explicitly decoded associations, rather than recovered original spellings. The static CalcRotate identity is not yet included in this name-based audit. Retained-data proof, complete-unit pairing, final native positions and the actual frozen build remain pending. The existing directional registration stays NonMatching; no progress credit is claimed here.

## Remaining differences and interface confidence

`DrawDirectional` remains 3,352 bytes against the native 3,404, retaining the previously documented switch-lowering difference. The ordinary out-of-line `GetViewMtxCooked` is 460 bytes against the native 464. The native helper ends with unconditional `mr r3,r31`, placing the destination matrix pointer in the return register before its epilogue. A diagnostic pointer-return declaration with `return pMtx` reproduces all 464 native instruction bytes and preserves the exact view-space drawing body.

That diagnostic does not establish whether the original source returned a pointer or reference, or conclusively exclude another compiler-level cause for the otherwise unused return register. The pinned Whippy header declares the older interface `void`. No canonical signature change is proposed on this evidence alone. A separate `global_optimizer off` control with the unchanged void interface emits 476 bytes and fails, while changing the draw bodies as well; it is not an accepted compiler option or source pragma.

The reproducible outside tools are `382-compile-complete-probe.py`, `382-view-return-probe.py` and `382-view-optimizer-control.py`, plus `382-paired-complete-probe.py` and its durable outside `paired-text-native.json`. Their inputs and objects are under `382-reference`; none writes source in this worktree.

## Traps

- **A following function can supply earlier weak copies.** Completing the directional view-space caller naturally emits the original earlier helpers. Do not treat those earlier inventory rows as independent missing translation units.
- **A return-register move is evidence, not a complete original C++ signature.** Pointer and reference returns share this ABI. Preserve the distinction between a successful diagnostic and an established canonical interface.
- **A raw stream match does not establish a retained object's final identity.** Pair relocation targets and inspect actual source-selected linked outputs before accepting a unit or crediting neighboring rows.
