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

## Remaining work in #375

`ef_effectsystem.cpp` starts at `0x80226550` and runs to about `0x80226EFC`, where `Emitter::Emitter` (`0x80226F00`, matched by ogws) begins in #376. Its first seven functions are in #375: `0x80226550` destructor, `0x802265E0` `GetInstance`, `0x802265F0` `Initialize`, `0x80226700` `ActivityList::ActivityList`, `0x80226750` `Closing`, `0x802267C0`, `0x80226830`. The 2009 `EffectSystem` differs from ogws: it keeps a per-group array whose count sits at offset `0xC014`, so only the `ActivityList` constructor matches the ogws source unchanged.

## Traps

- **An unreferenced `.text` symbol in a split object is not proof of a link gap.** The compiled `ef_effect.o` contains weak `ReferencedObject::SendClosing` and `DestroyFunc` between strong functions, plus a weak `ReferencedObject` vtable. The target has neither, but the link still matches because the linker removes unreferenced weak copies. Check the linked result rather than padding these out.
- **Branch targets show where code sits relative to a condition.** In `Effect::Calc`, a single misplaced `li` came from putting the vector copy inside the status test. Both branches of the test targeted the copy's first load, so the copy is unconditional.
