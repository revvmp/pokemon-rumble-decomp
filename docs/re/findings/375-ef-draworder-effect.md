# F-375-1: NW4R ef draw order and effect units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #375 owns `.text` `0x80225820–0x80226954`. This change reconstructs the first two NW4R ef translation units, `ef_draworder.cpp` and `ef_effect.cpp`, which cover the first 18 of the issue's 25 functions. The remaining seven functions start `ef_effectsystem.cpp`, which continues into #376.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/ef/ef_draworder.cpp` | `.text` | `0x80225820–0x80225940` | `DrawOrder::Draw`, `Add`, `Remove` |
| | `.data` | `0x803C52D0–0x803C52E8` | `DrawOrder` vtable (0x14) and padding |
| `nw4r/ef/ef_effect.cpp` | `.text` | `0x80225940–0x80226544` | 15 `Effect` functions, including the out-of-line inline `CreateEmitter(ResEmitter, u8, u16)` |
| | `.data` | `0x803C52E8–0x803C5308` | `Effect` vtable (0x20) |
| | `.sdata2` | `0x804BA5E0–0x804BA5E8` | `0.0f` and padding |

The order follows the CC0 ogws link order (`ef_draworder`, `ef_effect`, `ef_effectsystem`). `ef_effectsystem.cpp` begins at `0x80226550` with `EffectSystem::~EffectSystem`; the `NW4R - EF` version string at `0x803C5308` follows the effect vtable and belongs to it. `DrawOrderBase::GetPMList`, `Effect::GetNumEmitter`, `GetEmitter`, and the two for-each functions are absent from the target; the linker removed them, so the source defines only the functions the target contains plus the inline `GetPMList`.

Both units are `Matching`: objdiff reports 100% for all 18 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 (F-25-1) with `cflags_nw4r`, which is `cflags_base` with `-fp_contract off` and `-ipa file`, matching the ogws NW4R flags.

## Differences from the 2006 reference

The source follows ogws `src/nw4r/ef/ef_draworder.cpp` and `ef_effect.cpp` at commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73` (CC0-1.0, checked). Most functions match it unchanged. The 2009 target differs in three ways:

- `Effect::CreateEmitter` (both overloads, 0xF4 bytes against 0xE0) first returns null unless the resource's `EmitterDesc::commonFlag` has bit 30 (`CMN_FLAG_RELOCATED`) set. The target reads the word at resource offset 8, which is `commonFlag` immediately after the 8-byte `EmitterResource` header.
- `Effect::Calc` takes a flag word, not a `bool`. Bit 0 (tested with `clrlwi.`) selects the billboard-only pass. Bit 1 skips the preparation loop that resets evaluation status and starts particle-manager calculation. The names `CALC_FLAG_ONLY_BILLBOARD` and `CALC_FLAG_SKIP_PREPARE` describe this behavior; the original names are unknown. The preparation loop also copies a three-float emitter vector at offset 264 to offset 276 for every emitter, regardless of status. The final loop calls `Emitter::CalcGlobalMtx` (`0x80228E40`) on a 48-byte stack matrix and stores its translation column into offset 264. The stack frame grows by 0x30 to hold that matrix.
- An 8-byte accessor at `0x802263D0` returns the head object of the active-emitter list. Game code at `0x800A0FB4`, `0x800A2900`, and `0x800A2984` calls it after `0x80236330` and reads emitter fields from the result. Its original name is unknown; it is named `Effect::GetFirstEmitter` (low confidence).

## Recovered layout

`Emitter` gains two `math::VEC3` members after `mMtxDirty`, at offsets 264 and 276, named `mGlobalTranslate` and `mPrevGlobalTranslate` from their use (low confidence). The header places them before `mMtx`; that position is unverified because no reconstructed function accesses `mMtx` yet. Offsets up to `mCalcRemain` (232) agree with ogws, as shown by `CreateEmitter`. `Effect` keeps the ogws layout: `mActivityList` at 36, its active count at 60, `mFlags` at 80.

## Headers

The NW4R headers come from ogws `include/nw4r` at the same commit, stripped of comments by `tools/vendor/vendor_strip.py`. Only the 24 headers these two units include are kept. Adaptations:

- `types_nw4r.h` includes `revolution/types.h` and `stddef.h` in place of ogws `types.h`.
- The ogws Wii Sports revision conditionals are resolved to its later revision (`VERSION_RSPE01_01`), whose `Effect` virtuals and `FInv` match.
- `ASM` and `DECLTYPE` macros became `asm` and `__decltype__`. Umbrella includes in the ef headers became `ut/ut_list.h` and `math/math_types.h`. `NW4R_MATH_PI` is a literal, and the `CreationQueue` buffer size no longer uses the ogws `MAX` macro.
- The project has no MSL C++ headers, so `math_arithmetic.h` uses `math.h` (`ceil`, `floor`, `fmod`, `modf`) and `math_triangular.h` uses `tan`. These inline wrappers are not used by the two units and remain untested against the target.
- `Effect::Calc`, `Effect::GetFirstEmitter`, the two `Emitter` members, and the `CalcFlag` enum are new, as described above.

The only shared-header change is two declarations, `floor` and `ceil`, in `include/math.h`. The MTX declarations, the C++ `nullptr` definition, and the CodeWarrior fast-cast conversions these headers need arrived on `main` with #599 and #604. `math_arithmetic.h` includes `revolution/os/OSFastCast.h`.

Placeholder names inherited from ogws remain in the imported headers, for example `EMIT_FLAG_15`, `EMIT_FLAG_16`, and `PADDING_0xA3`. They were not renamed without evidence.

## Outside symbols

These callees were renamed in `symbols.txt` so the units link. Names come from the matched call sites and the reference interfaces: `List_Init` `0x80292D00`, `List_Append` `0x80292D20`, `List_Insert` `0x80292D90`, `List_Remove` `0x80292EB0`, `List_GetNext` `0x80292F20`, `List_GetPrev` `0x80292F40`; `EffectSystem::Closing` `0x80226750`; `Emitter::RetireParticleAll` `0x80226FA0`, `CalcGlobalMtx` `0x80228E40`, `SetMtxDirty` `0x80228F90`; `ParticleManager::BeginCalc` `0x802341C0`, `EndCalc` `0x80234230`, `BeginDraw` `0x80234280`, `EndDraw` `0x80234380`; `UtlistToArray` `0x80235320`; `CreationQueue::Execute` `0x80236C20`; `PSMTXIdentity` `0x802C8A80`, `PSMTXCopy` `0x802C8AB0`; and `operator delete` (`__dl__FPv`) `0x801B1580`, called by the deleting `Effect` destructor. Splits and sizes of these symbols are unchanged.

# F-375-2: NW4R ef effect system unit

`nw4r/ef/ef_effectsystem.cpp` completes #375 and begins #376. It is `Matching`: all 15 functions are byte-identical to the target, and the source-linked `main.dol` passes `tools/verify/verify.py`.

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80226550–0x80226EFC` | 14 `EffectSystem` and `ActivityList` functions and `__sinit_\ef_effectsystem_cpp` |
| `.ctors` | `0x80352A38–0x80352A3C` | static initializer entry |
| `.data` | `0x803C5308–0x803C5350` | `NW4R - EF` version string (0x45 bytes) and padding |
| `.bss` | `0x80423190–0x8042F2A8` | `mSortEffectOperatorZDrawInfo` (0xA0), its destructor-chain record (0xC), `instance` (0xC06C) |
| `.sdata` | `0x804B88E8–0x804B88EC` | `NW4R_EF_Version_` pointer |
| `.sbss` | `0x804B92B0–0x804B92C0` | `gBasicDrawOrder`, `gBasicDrawStrategyBuilder`, `gBasicEmitFormBuilder`, `mDisplayVersion` |
| `.sdata2` | `0x804BA5E8–0x804BA5F0` | `0.0f`, `1.0f` |

The `.data` split must end at the aligned `0x803C5350`; dtk rejects an end at the string's last byte (`0x803C534D`).

## Reference revision

The public ogws and Wii Sports sources are the 2006 revision and do not match. The 2009 target matches the later revision reconstructed in the Kirby's Return to Dream Land decompilations: CC0 `Swiftshine/kdc` (commit `81b52d6a31b6dd671bea0268f36a874477ee2bf6`) and its CC0 fork `Etrainquandt17/Whippy` (commit `8ee40c841475db862231f37b8cb64196b613cc17`), whose `src/nw4r/ef/ef_effectsystem.cpp` already splits `CreateEffect` into the name and resource overloads. Function names and sizes agree with the Mario Kart Wii symbol map in `riptl/wii-symbols` (`RMCE01`): the destructor (0x88), `Initialize` (0x108), `Closing` (0x64), both `CreateEffect` overloads (0x70, 0x124), `RetireEffectAll` (0xE8) and `Draw` (0x84). Only the build banner differs from the Whippy source: `Sep 24 2009 16:53:09 (0x4302_151)`. `EffectSystem::Calc` passes the `Effect::Calc` flag word unchanged, so it is declared `Calc(u32, u32)`.

## Recovered layouts

- `EffectSystem` is 0xC06C bytes: `CreationQueue` at 0x10 (0xC004 bytes), `mMaxGroupID` at 0xC014, `mActivityList` at 0xC018, `mRandom` at 0xC01C, the process camera at 0xC020–0xC060, `mXFFlushSafe` at 0xC064 and the new `mTmpEmitter` at 0xC068, which the constructor clears.
- `CreationQueue` holds 1024 entries of 0x30 bytes. Each entry gains two `math::VEC3` members at 0x18 and 0x24, which the target's `AddParticleCreation` (`0x80236920`) fills when its sixth and seventh pointer arguments are non-null, setting flag bits 0 and 1. `Execute` (`0x80236C20`) passes only the first as the translation. The names `mTranslate` (high confidence, from the reference) and `mExtraTranslate` (low confidence; the target never reads it) are used.
- `EmitterInheritSetting` is 0xC bytes: `alphaFlagPri` and `alphaFlagSec` follow `flag`, as in the reference and as `AddParticleCreation` copies ten bytes.
- `DrawInfo` is 0xA0 bytes, in the new `ef_drawinfo.h`. Its inline constructor runs in the static initializer.

## Headers

New headers `ef_drawinfo.h`, `ef_resource.h` and `drawstrategy/ef_drawstrategybuilder.h` follow the Whippy headers without comments. `ef_drawinfo.h` omits `GetViewMtxCooked`, which needs GX projection declarations this project does not have yet. `DrawStrategyBuilder` and `EmitFormBuilder` keep out-of-line `Create` virtuals, so their vtables (`0x803C5658`, `0x803C55D8`) stay in their own units.

## Traps

- **An unreferenced `.text` symbol in a split object is not proof of a link gap.** The compiled `ef_effect.o` contains weak `ReferencedObject::SendClosing` and `DestroyFunc` between strong functions, plus a weak `ReferencedObject` vtable. The target has neither, but the link still matches because the linker removes unreferenced weak copies. Check the linked result rather than padding these out.
- **Branch targets show where code sits relative to a condition.** In `Effect::Calc`, a single misplaced `li` came from putting the vector copy inside the status test. Both branches of the test targeted the copy's first load, so the copy is unconditional.
- **A whole ef revision exists in CC0 sources.** The 2006 ogws ef did not match `ef_effectsystem.cpp`; the 2011 Kirby reconstructions (`Swiftshine/kdc`, `Etrainquandt17/Whippy`) did, apart from the banner. Compare those first for the remaining ef units, and check object sizes in their headers (`CreationQueue`, `EmitterInheritSetting`) because a single short member shifts every later offset.

