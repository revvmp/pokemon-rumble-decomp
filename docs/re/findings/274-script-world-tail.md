# ScriptWorld final function

## F-274-3: SetStageDescTarget

The 276-byte binding at `0x8015D1D0` previously matched 99.710144%. Four instruction words exchanged the world/rank values between r30 and r29. The detailed earlier attempts are in [sprint-a03](sprint-a03.md), [sprint-a13](sprint-a13.md), [sprint-a17cx](sprint-a17cx.md), and [register allocation](sprint-ra.md).

Disabling dead-assignment elimination for this binding preserves the pop results as named variables and produces the target registers. The remaining mismatch under this pragma was the final integer-zero push: the existing `PushNull`/`PushInt` helper chain retains a named zero and selects `stwx` plus `add`, instead of the target's `stwux`.

An additive inline `CEngine::PushZeroInt` helper writes the integer type and zero payload directly through a reference to `mStack[mSP++]`. It performs the same integer-zero push as `PushNull`, while avoiding the intermediate argument and temporary value that cause the register difference. Only this binding uses the new helper; the existing methods are unchanged. Together with function-scoped `opt_dead_assignments off`, the final direct compiler comparison matches all 37 ScriptWorld functions, including all 276 bytes of this binding. Both reference and pointer forms of the direct push match; the reference form is retained.

This is a D-0016 fake match: the extra helper and optimizer control recover code generation, not independently recovered original source. A full coordinated build verifies integration before any source-link status change. The unit's function/data emission ordering remains a separate acceptance requirement.

## Additional attempts

Six permutations of scalar fields in a local struct, reference aliases to that struct, and a union overlay all retained the baseline mismatch. Integer wrapper classes, with and without empty destructors and const conversion operators, also retained it. Semantically equivalent high-bit changes and masks on the popped values did not help. With dead assignments disabled, equivalent forms of the pushed zero still produced the 97.10145% result; reading the known-zero map field regressed further. None of these unsuccessful experiments was retained.

## Traps

A pragma that fixes nonvolatile argument registers can change the treatment of a later inline constant. Inspect the final push too; preserving the pop registers alone does not establish a match. The verified workaround here is a direct typed integer-zero push with the same stack update semantics.

## Coordinated acceptance

After applying the helper and scoped pragma, the coordinating maintainer rebuilt with normal includes and verified the full DOL SHA. A subsequent trial selecting `game/ScriptWorld.cpp` as Matching also completed the full build and exact SHA verification successfully. The unit is accepted as source-linked; the earlier ordering caveat is resolved for ScriptWorld by that end-to-end result. Direct compilation of the applied source independently confirmed all 37 functions at 100%.
