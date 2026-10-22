# Sprint a02: CLayout, CPauseMenu, ef_animcurve

## Traps

- **Imported names for one-off RTTI and iterator helpers can be swapped or wrong.** CLayout `get_deleter` for `shared_ptr_deleter<PaneMP>` and `<CMotionProgram>` scored 99.74% only because the two `__RTTI__default_delete` symbols at `0x804B8400` and `0x804B8408` carried each other's names. The list-of-`CFileData` iterator helpers at `0x801CE7D0` were named `generic_iterator<0>` although the target loop uses `const_iterator` (the unit scored 99.88% with `const_iterator` source and 95% with `iterator` source). When only the `bl` operand names differ, rename the symbol by address instead of changing the source.
- **Unnamed `fn_`/`dtor_` template helpers can be identified from vtable and extab references.** In `game/CLayout.s` the `.4byte fn_...` slots inside `__vt__...shared_ptr_deleter<X,...>` identify the deleter destructors, and the `Dtor:` actions in an extab comment identify member destructors. This named seven placeholders that `name_callees.py` did not propose.
- **A conditional offset reads differently from an added bool.** `GetMessageText(0x60081 + IsHubMap(), ...)` emitted a bare `cntlzw` result; `IsHubMap() ? 0x60082 : 0x60081` reproduced the `neg/or/srwi` normalisation of the target and moved CPauseMenu `Run` from 98.2% to 99.9%. `stage == 7 ? 0x60088 : 0x60087` improved `LeaveStage` the same way.
- **A shared message formatter differs from a temporary one.** `(basic_formatter(pFormat) % a % b).str()` in CPauseMenu compiled 99.68%; the named-local form in `include/game/MessageFormat.h` with `NIntl/basic_formatter.h` (not `game/NIntl.h`) reaches 99.92% with only data labels left.
- **Hoisting a float computation changes the callee-saved float count.** `TakeScreenshot` needed `maxAspect = maxWidth / maxHeight` directly after `maxHeight` to reproduce the sixth saved float register (`f26`); the shared-pointer test `work.get() == nullptr` on a temporary-initialised pointer added another 2%.
- **A temporary `Rect` argument stops calling its destructor once it is a named local.** `mDrawInfo.SetViewRect(mLayout->GetLayoutRect())` in CLayout `LoadLayout` called `__dt__Rect`; binding the result to a named `Rect` first removed the call (96.3% to 97.9%).
- **Scratch edits to CRLF sources.** Repository sources use CRLF; a Python edit that reads with universal newlines and compares multi-line strings silently fails to match unless the file is normalised first and written back with `\r\n`.
- **A `mutable` member disables const-this load hoisting.** CGameSession `GetEnvironment` loads both `shared_ptr` words before the refcount increment only when it is an inline `const` member and no member (transitively) is `mutable`. PiiPropData's `mutable` vectors blocked it; removing `mutable` (with a `const_cast` in `GetProperty`) matched it.
- **unitdiff is stricter than report.json.** String or data labels at different section offsets score below 100% in unitdiff but 100% in the report; check the report count before chasing them. Oversized target `lbl_` data blobs must be split into per-object symbols for data-flow names to match.
- **auto_00_800046E4_init is MetroTRK `__exception.s`** (`gTRKInterruptVectorTable` to `gTRKInterruptVectorTableEnd`); it needs assembly source support, which the build lacks.
- **Auto-inlining a non-inline function counts only its own statements.** `ResourceFileConfig::SetErrorHandler` (copy, swap, destroy of a `tr1::function`) is auto-inlined into `main` in ours but called in the target. Twelve dead statements in its own body stop the inlining (main 88.8% to 99.4%); enlarging the inline `swap` or copy constructor it calls, defining it before `main`, or growing `main` do not. The original body therefore had more own statements with the same code; the form is unknown.
- **Main's table data was split across two wrong owners.** `gMotorPatterns` (0x803550B8) was defined in ScriptInput, which made a main `.rodata` split a link-order cycle. It belongs to the shared table block before main's rodata; ScriptInput now declares it extern, main owns `.rodata` 0x8035BBA0-0x8035BC88 and defines `gSystemMessageResources`, and the tables it uses must be defined after `InitializeMainMessages` so that function keeps absolute addressing.
- **Extra vector destructor level is per element type.** In the CGameSession `SessionSnapshot` destructor the target has one more null-check level for `vector<SD_BattleRoyalData::Data>` and `vector<SD_PiiPersonalData::Data>` but not for the statistic or collection vectors. An `is_POD` specialization does not add it.
- **A prologue-hoisted load in ours and not in the target means the target is non-const.** `CLayout::NormalizeViewPosition` and the `ShouldRender` loop callback were the reverse cases: making `ShouldRender` const (interface, `CMain` and `CErrMsgFatalLoop`) matched it, and dropping `const` from `NormalizeViewPosition` matched it. `CMotionProgram::GetCurrentFrame` must stay const for its inlined callers in CPii, CMultiRanking and ScriptModel; the one out-of-line copy called from `CModel::UpdateAnimations` is non-const, so it is a separate `GetPlaybackFrame` accessor.
- **A dead load kept in the target means a volatile field.** `CreateFileData` keeps an unused `lbz` of `CFileData::mPending`; declaring it `volatile bool` matched the function with no regressions.
- **Changing `__vec_constructor::operator=` to iterate raw `T*` breaks other units.** The CFileData `vector<map>` assignment calls `copy_range<map*>` and `construct_at_end<map*>`, but the same header change dropped CStatistic, SaveData, CPiiCollectionBox and CUIMenu assignments (13203 to 13192). The selection rule for the map element type is unknown.

## Unsolved (tail wave)

| Unit | Function | % | Cause |
| --- | --- | --- | --- |
| CPauseMenu | Run | 99.98 | Four menu item locals take reversed frame slots; no declaration order, const or helper flips the parity |
| CPauseMenu | LeaveStage | 98.73 | Hub `MapIndex` construction evaluates area before mode; seven construction forms tried |
| CPauseMenu | TakeScreenshot | 99.83 | Non-volatile register assignment |
| main | HandleFileError | 96.61 | Inlined `UsesAlternateErrorMessage` range test shares one `false` block in the target; nine forms tried |
| main | main | 88.83 | See the `SetErrorHandler` auto-inlining trap above |
| homebutton | InitializeHomeButton | 99.99 | Two `__unexpected` landing-pad slots for the `spIcon` assignment are swapped |
| homebutton | InitializeHomeButtonInfo, UpdateHomeButton | 98.7, 96.7 | Register assignment of parameters and float temporaries |
| CModel | AddResourceFile | 97.0 | `std::find` over the vector iterator keeps the iterator in memory; the target keeps it in registers |
| CLayout | GetPaneAlpha | 98.2 | Inlined recursive copies carry an extra dead branch the outer copy lacks |
| CLayout | MakeArchiveList, Draw | 94.4, 93.9 | First loop test scheduled before the list stores; frame slot order of rect, viewport and scissor |
| CFileData | copy_range, construct_at_end, construct_at_front, erase | 91-99 | Header-level vector and tree helpers; see the operator= trap |
| NPpuVM | ~CEngine, Load, BitwiseOp, CompareOp, CollectGarbage | 85-98 | Register assignment around inlined `set::erase` and `CValue` type tests |

## Final tail wave: CPiiListFilterSort, CGameSession, CRemoteMemory

Matched: CGameSession `GetEnvironment` (PiiPropData reads its species table through a `const_cast` instead of a `mutable` member) and `RunStage` (player state save and restore loops are inline helpers). Improved: CGameSession `Calc` 93.26 to 99.96, `AddMoney` 96.86 to 99.02; CRemoteMemory `ThreadMain` 75.58 to 78.97, `BeginRead` 85.66 to 87.98; CPiiListFilterSort `Sort` now calls `vector::swap`.

### Traps

- **A `mutable` member anywhere in a contained class breaks other units.** Making `PiiPropData::mSpecies` mutable (for a const lookup) silently undid the CGameSession `GetEnvironment` match from the earlier trap. Use a `const_cast` at the single const access instead.
- **Two loops in one function swap counter and pointer registers until one is an inline helper.** CGameSession `RunStage` matched only with both player loops moved into inline member functions; declaration, loop form and shared-counter variants did not move the registers.
- **Iterators kept in stack slots point to a two-word converting iterator.** In `vector<T*>::assign` from a `set<T*>` the target copies 8-byte iterator objects whose second word is never written. An `__implicit_convert_iterator` with a second pointer-sized member, counted with one by-value level (`__distance` through a `const&` helper) and copied by a by-value `copy_assign`, reproduces it. A `bool` second member copies by bytes instead.
- **A switch node that has the same target as `default` survives only with a separately written body.** In CRemoteMemory `ThreadMain`, `case -1:` with its own copy of the default body keeps the `-1` compare; `case -1: default:` drops it.

### Unsolved

| Unit | Function | % | Cause |
| --- | --- | --- | --- |
| CGameSession | Calc | 99.96 | The two converted iterators are stored in the opposite order; nesting the body in a helper fixes the order but flips every other slot |
| CGameSession | AddMoney | 99.02 | `current` takes r5 instead of r3; inline helper and declaration variants are worse |
| CGameSession | CanAddPPDToPlayerPiiBox | 99.01 | The temporary-constructed flag of the `&&` comparison is kept in a non-volatile register |
| CGameSession | GetSnapshot | 88.24 | 64-bit play-time sum register pairs and store order; ten local and helper forms tried |
| CGameSession | vector and SessionSnapshot destructors | 96.8, 97.7 | Extra null-check level for two element types (see the earlier trap) |
| CPiiListFilterSort | CompareBPAscending, CompareBPDescending, CompareSpeciesName, CompareSpeciesOrder | 96.3-99.6 | The item pointers take r5/r3 instead of r3/r4; pointer, reference, helper and `||` forms tried |
| CPiiListFilterSort | Sort | 96.16 | `mSortType` is loaded after the iterator addresses instead of right after `begin()` |
| CPiiListFilterSort | ShowTypeFilterMenu, ShowPrefixFilterMenu | 96.6, 98.8 | Frame 0x10 smaller and a shift operand swap |
| CRemoteMemory | ThreadMain | 78.97 | The target builds binary trees for three-range switches where ours compares linearly; the retry user data is computed before `OSCancelAlarm` in the target and after it in ours, which keeps `channel` in a saved register |
| CRemoteMemory | SetState callers (BeginRead and others) | 56-88 | the constant ORs on 0x42a and 0x200 stay unfolded in the target; locals, inline operators and out-of-line definitions all fold |
