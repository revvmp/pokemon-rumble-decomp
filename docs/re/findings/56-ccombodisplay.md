# CComboDisplay.cpp and shared layout contracts (#56)

## Scope and current status

The original issue contains eighteen functions, 2,108 instruction bytes, at `0x8006DDE0` through `0x8006E6A0`. RTTI identifies the private implementation as `@unnamed@CComboDisplay_cpp@::CComboDisplay` and the public interface as `IComboDisplay`. The complete original inventory is now reconstructed in Matching `game/CComboDisplay.cpp`. The actual pipeline object reproduces all eighteen functions and every instruction target, eleven exception records, and all retained owned data. Source-selected full Ninja and frozen verification exit 0. The earlier shared prerequisites in PRs #1057 and #1061 earned no issue-completion credit by themselves.

An independent scratch producer using the actual Ninja game producer's Wii/1.1 options reproduces all eighteen bodies, every resolved instruction relocation target, and all eleven owned exception records, 616 bytes including indexed code lengths. These are preflight results, not whole-unit acceptance. Outside evidence is `56-reference/56-preflight.json` and `56-reference/56-eh-proof.json`.

## Shared motion and text contracts

| Address | Declaration or behavior | Native evidence |
| --- | --- | --- |
| `0x801D0890` | `shared_ptr<CLayout::CMotionProgram> CLayout::CreateMotion(const char*, Pane*, f32, f32)` | Hidden result, layout, animation name and pane use `r3` through `r6`; frame and rate use `f1` and `f2`. It calls `0x801D1FE0`, then `0x801D0970` when an animation exists. |
| `0x801D0D00` | Named-pane `CreateMotion` overload | #58 independently traces pane-name lookup followed by the raw-pane overload. |
| `0x801D0DB0` | `QueueMotion` with the same argument types | #58 independently traces construction through `0x801D1FE0`, insertion after the first program, removal of subsequent queued programs, and immediate start when the list is empty. |
| `0x801D1040` | Named-pane `QueueMotion` overload | #115 independently traces pane-name lookup followed by `0x801D0DB0`. |
| `0x801CB140` | `void CMotionProgram::Wait()` | #58 independently traces the raw `this` receiver, repeated state-3 test and `YieldFiber`; the existing `IsFinished` predicate also accepts state 2 and remains unchanged. |
| `0x801D1730` | `u16 CLayout::GetMotionLength(const char*)` | Animation lookup followed by SDK `AnimTransform::GetFrameSize()`, whose canonical return is `u16`; the combo factory zero-extends sixteen bits. |
| `0x801D60B0` | `TextBox* CLayout::SetText(const char*, const wchar_t*)` | Calls `strlen` for the pane name, `wcslen` for the text, checks TextBox RTTI, updates text buffers, and returns the last affected text box. |
| `0x8006BD60` | Wide `FormatString` overload returning `std::wstring` | Initializes the twelve-byte wide string, prepares variadic arguments and calls `0x801F5BB0`; the combo caller passes UTF-16 `L"%d"` and a count. |

The motion constructor at `0x801D1FE0` stores the return of `0x801D2180` at program offset `0x0C`. That routine looks up the `map<Pane*, shared_ptr<PaneMP>>`, defaults a null input to the SDK root pane, and returns the mapped `PaneMP*`. The previous `Pane*` member interpretation was incorrect. Changing the member type to `PaneMP*` preserves the 24-byte program layout. Its state at `0x14` is exactly 3 in the combo transition predicate, represented by the additive `IsEnded()` method.

Method spellings for these previously unnamed routines are descriptive inferences. Native argument, result, storage, and ownership evidence determines their contracts.

## Shared layout resources

`GetDefaultFontMap()` at `0x80126280` locks the weak FontMap owner at `0x804B9090`, returning `shared_ptr<CLayout::FontMap>`. The lock may return empty; it is not an unconditional empty-font factory.

`gSharedLayoutArchives` at `0x804B9098` is a pointer to a null-terminated array of constant archive-name pointers. `0x801CABD0` traverses it, and `0x80127CF0` assigns the first mapped value's archive array at offset 20. Only the first word of the existing eight-byte placeholder is proven to be this pointer. This prerequisite neither renames nor resizes that outside symbol; its second word remains unproven. The resource function, variable and header names are descriptive inferences coordinated with #55 and #58.

## Prerequisite consumer validation

The initial same-base control on source head `e4d9541e17d77a30c8bd07369aa7632afc32ccae` enumerated all 716 actual `all_source` inputs through Ninja. Of these, 711 whole objects were byte-identical. CBossPii, CEnemyPii, CMsgBalloon, CNoticeDisplay and SaveData differed only in compiler-generated numeric symbol names inside `.strtab`. Section metadata, every allocated section, raw relocations and symbol attributes were identical after normalizing only names of the form `@` followed by digits. The linked ELF had only the same numeric-name metadata difference; the full frozen DOL was byte-identical. Both before and after states completed configure, explicit `all_source`, plain Ninja and frozen verification with exit 0.

The outside `56-land-contract.py` wrapper repeats this control inside the official `land.verify` callback after its original verifier on every merge-loop round. It restores only these three proposed headers to that round's exact common main ancestor, builds and snapshots every Ninja-declared source object and linked artifact, restores the candidate headers, rebuilds and compares again. It rejects instruction, data, relocation, section-metadata or nonnumeric-symbol changes, restores candidate files even after failure, and requires a clean tree and unchanged head before returning. A moving main is handled by the official loop's rebase and another complete control.

## Complete map-interface prerequisite

`IGameMap` has forty-three virtual slots, a 180-byte vtable at `0x803A1D00`, and a twelve-byte base layout. Its bases are the canonical `ICalcDraw` at zero and `enable_shared_from_this<IGameMap>` at four. An actual canonical-header producer reproduces all five pure slots, all thirty-eight concrete method bodies and resolved targets, and the 108-byte destructor at `0x800A5980`. Every native byte read through the reading-aid ELF is checked independently against frozen `orig/wpse-v1/main.dol`. Outside proof is `56-reference/56-map-contract-proof.json`.

| Slots | Descriptive contract | Evidence |
| --- | --- | --- |
| 0–4 | Destructor, Calc, CalcMtx, CalcView, Draw | ICalcDraw hierarchy and original vtable; Calc and Draw are pure. |
| 5 | RunAction | Pure in IGameMap; CGameMap `0x800A98E0` runs the action fiber with the current map/script context. |
| 6 | GetMapDesc | Pure; CGameMap `0x800A8010` returns its descriptor pointer at twelve. MapDesc RTTI at `0x803A3804`, constructor data flow and the first StageID word consumed by map bindings establish the referenced record. |
| 7 | GetWorld | Pure; CGameMap `0x800AA5C0` returns its BWorld owner; world-calculation and effect bindings consume it. |
| 8–14 | GetWorldObjStatDisplay, GetMiniMap, GetDlgPiiSelector, GetDlgPiiList, GetDlgCollection, GetDlgInfo, GetDlgMsgWin | Getter bodies `0x800AA5B0` through `0x800AA550`, script binding families, factory results and interface RTTI. |
| 15–19 | GetNoticeDisplay, GetMoneyLifeDisplay, GetBattleRoyalDisplay, GetComboDisplay, GetLayoutManager | Getter bodies `0x800AA540` through `0x800AA500`; matching named binding families and factory/interface RTTI. |
| 20–25 | GetPauseMenu, GetTutorialMsg, GetMsgBalloon, GetEnemyInfo, GetNumericInput, GetPostEffect | Getter bodies `0x800AA4F0` and `0x800AA4E0` through `0x800AA4A0`; factories in CGameMap initialization and original IPauseMenu/ITutorialMsg/IMsgBalloon/IEnemyInfo/INumericInput/IPostEffect names. |
| 26–28 | GetCurrentDialog, RegisterDialog, UnregisterDialog | `0x800AA430` locks the weak owner at 28 and returns its pointer. `0x800A99E0` accepts a shared owner, closes a different current dialog when possible, and stores a weak owner; `0x800A9B60` clears the matching owner. IDlg's existing Close slot is used. |
| 29–35 | SetPausing, IsPausing, Hitstop, IsHitstopping, EnterEvent, ExitEvent, IsInEvent | Pause/Resume/IsPausing/Hitstop/event bindings and CGameMap overrides. The kept IsPausing default `0x8006E020` belongs to the combo unit. |
| 36–40 | StartTimer, AddTimer, StopTimer, IsTimerWorking, GetTimerFrame | Timer bindings and `0x800AA3A0` through `0x800AA330`; StartTimer stores two signed integers, frame count and per-frame step, whose default is -1 in the binding. |
| 41–42 | DisappearModelObjs, AppearModelObjs | Original binding spellings, `0x800A9C10` and `0x800AA0B0`, with saved model identifiers and visibility changes. |

All optional pointer getters return null in the base. RegisterDialog and the boolean predicates default to false; optional actions are empty. The complete declaration contains no synthetic gap methods or padding. Previously unnamed method spellings and unobservable const qualifications are inferred descriptions; interface class names, ownership, slot order, default behavior and observable argument/result shapes are evidenced. The descriptor getter exposes a const MapDesc reference, while its complete record layout remains outside this prerequisite.

## Inline motion-wrapper prerequisite

The one-name CreateMotion wrapper at `0x8006E240` is an inline header definition. With the canonical Wii/1.1 producer, ordinary automatic inlining keeps its sixteen-byte weak copy after Calc and before ShowComboCount, exactly the native retained order. No forced inlining settings are used. The private combo destructor is implicit; an explicit destructor definition changes retained ordering despite identical body bytes.

Its per-translation-unit static `Pane*` is four bytes at eight-byte small-BSS alignment, defaults to null and requires no constructor. A shared-owner candidate instead emitted a 40-byte initializer and destructor-registration storage absent from the original. The four-byte pointer interpretation preserves the native value loads; the complete source-selected link now verifies its four-byte boundary.

On the current 717-source-object inventory at base `23aad9c5fb4edc5f4e790a4e854897c0b55f3b2c`, 712 whole objects remain identical and four differ only in numeric symbol labels. CMsgBalloon gains only the sixteen-byte weak wrapper, a four-byte static pane pointer and eight bytes containing the 0.0f/1.0f literals. All forty-five existing function payloads and symbolic relocation identities, kinds and addends are unchanged. Existing allocated data, exception records and exception index payloads and relocations are unchanged. The full link and frozen verifier exit 0; the DOL is raw-identical and the linked ELF changes only 56 numeric symbol labels. Outside proof is `56-reference/56-inline-consumer-proof.json`.

## F-56-1: Complete source-selected combo display

The public `IComboDisplay` and complete private anonymous `CComboDisplay` preserve the native virtual hierarchy and interface order. The private object is 260 bytes: state at 4, 232-byte CLayout at 8, motion shared owner at 240, count at 248, gauge at 252 and ending-animation frame count at 256. The constructor leaves the last member for Load, which initializes it from the native animation length. Factory and previously unnamed member spellings are descriptive; class names and source basename are original RTTI evidence.

All eighteen original issue addresses and sizes are present in the actual source object, totaling 2,108 instruction bytes. Every relocation is resolved before comparing native instructions; all eleven exception records and indexed code sizes match, totaling 616 bytes. The native reading-aid ELF is independently checked against frozen DOL bytes at every read. `56-unit-native-proof.json`, `56-unit-eh-proof.json` and `56-unit-data-proof.json` under the outside `56-reference` archive record the actual producer proof.

The source-only linker map assigns the entire retained text, exception, data and SDA contribution to CComboDisplay.o. Retained `.data` is 424 bytes, `.sdata` 80, `.sbss` 4 and `.sdata2` 20. All 23 retained data symbols, 497 logical bytes, match after resolving pointers; the 528 allocated bytes include ordinary compiler/linker alignment. Extra canonical inline helpers in the raw object are discarded, rather than credited as reconstructed owned code. The final frozen DOL comparison verifies every allocated byte, including alignment. The source graph selects `src/game/CComboDisplay.o` and excludes its original fallback object.

The owned default-pane pointer at `0x804B8FC8` is four bytes followed by ordinary alignment, and the owned 0.0f at `0x804B9D68` is four bytes. Their previous eight-byte placeholders included following alignment. This correction does not resize the outside archive-pointer placeholder at `0x804B9098`.

Native method emission and data ordering require ordinary inline definitions inside the private class, with getter/setter declarations before ShowComboCount, Calc and Draw. Inherited virtual order stays unchanged. Defining these overrides out of line matched every individual body and EH record but left 48 full-DOL byte differences through RTTI/string ordering and resulting pointers. The implicit destructor and natural inline ordering reproduce the complete DOL under unchanged canonical flags.

`python configure.py --map`, explicit Ninja `all_source`, plain Ninja, frozen verification, `unitdiff game/CComboDisplay.cpp`, and the native/data audit all exit 0. The final official land gate repeats these checks after every rebase and retains an immutable exact-head inventory, object, map, source graph, ELF and DOL outside the repository.

## Traps

- A motion's input pane and stored target have different types. `0x801D2180` accepts `Pane*` but returns the mapped `PaneMP*`; the unchanged pointer width conceals an incorrect declaration from layout checks.
- A matching function body does not establish a complete unit. The combo scratch has exact code and exception records while its full IGameMap interface and default-pane storage boundary still require independent acceptance.
- An eight-byte small-BSS placeholder can include alignment. A raw static pane pointer emits four bytes at eight-byte section alignment and preserves the native load without a constructor; a shared owner instead introduces initialization and destructor-registration code absent from this unit. The complete selected-source link now verifies the four-byte extent and following ordinary alignment.
- This Windows Python uses cp1252 by default. A documentation rewrite without an explicit read encoding converted existing UTF-8 punctuation; the diff caught it before publication. Restore unrelated text from the base and use `read_text(encoding="utf-8")` as well as explicit UTF-8 writes.

- Exact individual bodies and EH can conceal source-induced RTTI ordering differences. Inline overrides inside the class, in natural reverse-emission order, restored this unit's complete data layout; out-of-class inline and out-of-line definitions did not.
