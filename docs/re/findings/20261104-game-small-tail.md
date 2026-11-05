# Game small-tail continuation, 2026-11-04

This pass owns `CPlayerPii.cpp`, `CMsgRes.cpp`, `CShadowCamera.cpp`, and `CDlgCollection.cpp`. Experiments use the configured native compiler directly and complete objdiff JSON in isolated `tools/local/tail-20261104/smallgame/` directories. Shared configuration, headers, full builds, and frozen-artifact verification remain with the coordinator.

## SetupShadowMaterial exact function

`SetupShadowMaterial` at `0x8010D140` now compares at 100%, retaining its 1,252-byte size. A fresh control reproduces the prior 99.8754% result. The complete unit moves from 15/17 to 16/17 exact real functions; every other function score and the matched function order are unchanged. `SetupShadowModel` remains 95.20192% at 416 bytes. This is function-comparison evidence, not completed source linkage for the unit.

Two local D-0016 controls resolve the five remaining instruction rows:

- A trivial `ShadowMaterialColor : GXColor` constructor copies the four-byte value through a `u32` view. Passing its temporary to `GXSetTevKColor` puts the argument address before the receiver address, matching the target offsets `+0x210` and `+0x218`.
- Immediately after the second-stage declaration, a `void*` local retains `&texObj` through `pTexObj = static_cast<void*>(pTexObj)`. Casting it back only for `GetTexObj` makes the receiver address live at the right point: the post-matrix subtraction uses `r5`, and the receiver address precedes the final matrix addition at offsets `+0x18C`, `+0x194`, and `+0x198`.

These are documented fake-match constructs, not recovered original source. Typed `ResTexObj*` identities and reassignments do not reproduce the second result. Moving the void-pointer declaration before the second-stage declaration gives 99.92971%, while placing it immediately afterward gives 100%.

The first color experiment used the existing `nw4r::ut::Color`. It reproduced the same exact instructions but emitted an additional 64-byte weak destructor, 24 exception bytes, and 12 exception-index bytes. It was replaced by the trivial local copy type before acceptance. In the retained version, every object section has the same size as the control. Raw section comparison changes only `.text` and compiler-generated names in `.strtab`; `extab`, `extabindex`, all data sections, all relocation sections, and the symbol table are byte-identical to the control. The `.text` difference consists of eleven bytes in the five corrected instructions.

Reproduction is `python tools/local/tail-20261104/smallgame_shadow_accept.py`. The final source SHA-256 at this checkpoint is `aa7db07872d1845d18e64bbb19896dcdc595211d86321b7317eefbedc22dcdf2`. `material_control_original/` and `material_accepted/` contain the full source snapshots, compiler logs, objects, and objdiff JSON. `material_acceptance.json` records the no-regression and section checks. The discovery runners are `smallgame_shadow_receiver.py` and `smallgame_shadow_trivialcolor.py`.

## Remaining probes

`CDlgCollection::Calc` remains 99.66725%. A Frida allocation trace confirms that the only instruction differences are the model pointer in `r29` instead of `r26` and the loop's byte offset in `r26` instead of `r29`. The target's normal and rare form-mask values already use the correct `r28` and `r27`. Lifting the model pointer into callers, explicit byte-offset loops, full-range enum offsets, model references, wrapper storage, inline accessors, and surviving pointer identities did not produce the target ordering. A same-type model-pointer reassignment moves several additional values and scores 99.45709%; the successful void-pointer control from shadow material does not transfer to this loop. `smallgame/collection-trace.txt` and the `smallgame_collection*.py` runners preserve the new evidence.

`CMsgRes::ConvertEscape` remains 96.57971%. Its loop body is unchanged; locale-address and SJIS-result scheduling in the preheader remain different. Local locale accessor helpers, extra inline depth, integer/void identity forms, and scheduling-factor controls did not solve it. These complement the prior exhaustive whole-loop helper probes in [the CLayout continuation](329-clayout-final.md#cmsgres-follow-up).

`CPlayerPii::GetShadowName` remains 88.85714%. Additional scheduling-factor, aggressive load/store scheduling, and prologue-helper controls leave the existing 84-byte result unchanged. Prior array, aggregate, ABI-view, and compiler-family investigations remain in [the player tail findings](player-shadow-tail.md).

`CShadowCamera::SetupShadowModel` still has the argument-copy/field-load scheduling described in [the shadow findings](207-cshadowcamera.md#post-window-lifetime-probes). Fresh private reference-return getter headers and a reference index local reproduce the existing 97.40385% diagnostic. The remaining material-copy store occurs too early, allowing the texture multiply to reuse `r0` instead of `r5`. Typed color construction, copy/reference parameter forms, and additional material/index temporaries do not close it. No getter or shared header changes are retained.

## Traps

- An existing wrapper with an empty user-defined destructor can match all instructions while adding exception metadata and a weak function. Compare complete object sections before retaining a scheduling-only success. The trivial local color wrapper preserves the control's metadata in this case.
- A pointer identity's static type and declaration position can affect scheduling even when typed-pointer identities are optimized away. The successful void-pointer control is local evidence; its direct transfer to collection failed.
