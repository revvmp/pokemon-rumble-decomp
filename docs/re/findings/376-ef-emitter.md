# F-376-1: NW4R ef emitter unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. `nw4r/ef/ef_emitter.cpp` covers the rest of #376 (`0x80226F00–0x80228A14`) and the first four functions of #377 (`0x80228A20–0x802290A0`). It is `Matching`: objdiff reports 100% for all 20 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80226F00–0x802290A0` | 20 functions, from `Emitter::Emitter` to `Emitter::SetMtxDirty` |
| `.data` | `0x803C5350–0x803C5378` | `Emitter` vtable (0x28) |
| `.sdata2` | `0x804BA5F0–0x804BA624` | float and double constants |

The `.sdata2` split ends at the last constant, `0x804BA624`. dtk had inferred an 8-byte symbol at `0x804BA620`; it is now 4 bytes so the split can end there.

## Reference

The source follows `src/nw4r/ef/ef_emitter.cpp` of CC0 `Etrainquandt17/Whippy` (commit `8ee40c841475db862231f37b8cb64196b613cc17`), the Kirby's Return to Dream Land revision also used for F-375-2. Its code matches this 2009 target without change. The target lacks `Emitter::RetireParticleManager`, `FindParticleManager`, `GetNumParticleManager`, `GetParticleManager`, `ForeachEmitter` and `ForeachParticleManager`. The source omits the last four; the first two remain because `-ipa file` inlines them into their callers and the linker removes the unreferenced copies. The file-local `GetLODratio` stays out of line at `0x80228160`.

## Recovered layouts

- `Emitter`: `mGlobalTranslate` (0x108) and `mPrevGlobalTranslate` (0x114) sit after `mInheritSetting` (0xFC) and before `mMtxDirty` (0x120); `mMtx` is at 0x124. `InitializeDatas` stores 0x120 for `mMtxDirty` and sets both vectors to quiet NaN. This corrects the unverified position in F-375-1.
- `ParticleManager::SetMtxDirty` also clears the static `smMtxInvId` (`.sbss` `0x804B92C0`) when it equals the manager's 16-bit object ID, as `Emitter::SetMtxDirty` inlines it twice.
- `EmitterDesc::commonFlag` bits 10–13, `EmitterParameter::INHERIT_FLAG_PIVOT` (bit 2) and `ParticleManager::FLAG_MTX_INHERIT_PIVOT` (bit 2) are new. `RestructMatrix` uses the pivot flag to apply rotation and scale about the inherited translation when that translation is only partly inherited. The reference names them `UNK`; the names here describe that behavior and are low confidence: `CMN_FLAG_INHERIT_PTCL_PIVOT` (10, copied to the first particle manager), `CMN_FLAG_EMIT_INHERIT_PIVOT` (11, copied to child emitters), and `CMN_FLAG_PTCL_EMIT_INHERIT_SCALE` and `CMN_FLAG_PTCL_EMIT_INHERIT_ROT` (12 and 13, used for emitters created from particles in `CreateEmitterTmp`, where bits 7 and 8 serve `CreateEmitter`).
- `Emitter::CreateEmitter` and `CreateEmitterTmp` take a trailing `math::VEC3*` translation, and `CreateEmitterTmp` returns `bool`. `FindParticleManager` and `RestructMatrix` take the pivot flag.

New header `ef_emitterform.h` declares `EmitterForm`. `ef_util.h` gains the matrix helpers that the emitter calls, named from the reference.

## Outside symbols

Renamed in `symbols.txt` for the link, names from the reference and matched call sites: `AnimCurveExecuteF32` `0x8022A380`, `ParticleManager::RetireParticleAll` `0x802325C0`, `ParticleManager::CalcGlobalMtx` `0x802340E0`, `GetDirMtxY` `0x802353B0`, `MtxGetRotationMtx` `0x802354C0`, `MtxGetRotation` `0x802355E0`, `MtxGetTranslate` `0x80235750`, `MtxGetScale` `0x80235770`, `Normalize` `0x80235C40`, `PSMTX34RotXYZRad` `0x80235F00`, the two `MTX34ScaleNonZero` overloads `0x80236080` and `0x80236100`, `MTX34ScaleInv` `0x80236190`, `MTXColLen` `0x80236240` and `ParticleManager::smMtxInvId` `0x804B92C0`. Splits and sizes are unchanged, except that `smMtxInvId` is now 4 bytes.

## Remaining work in #377

`0x802290A0` (0x12E0 bytes) onward starts the animation-curve code; `AnimCurveExecuteF32` is at `0x8022A380`. Whippy has no `ef_animcurve.cpp`, and `Swiftshine/kdc` marks its copy `NonMatching`.

## Traps

- **Layout guesses from one unit can be wrong in the next.** F-375-1 placed the two `Emitter` vectors after `mMtxDirty` because no function it matched touched that area. The first `ef_emitter.cpp` build then stored `mMtxDirty` at 262 instead of 288. Record unverified positions as such and check the first store to them.
- **A relocation-name diff exaggerates differences.** Local constants (`@3039`) and target labels (`lbl_804BA5F4`) differ by name only. Use objdiff's report, not a raw objdump text diff, to decide which functions still differ.
