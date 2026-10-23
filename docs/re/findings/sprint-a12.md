# Sprint a12 findings (CPiiTex, PiiPropData, CPiiListFilterSort)

## Traps

- **A `#pragma dont_inline on` placed mid-file also applies to template instantiations.** CodeWarrior instantiates templates at the end of the translation unit, so the pragma state at end of file decides inlining of every template body. PiiPropData's constructor stayed 4% until `#pragma dont_inline reset` was added at the end of the file; the vector constructor then inlined as in the original (95.97%). Functions defined after the `on` still call their (now out-of-line) helpers, which is what the original Set/GetWazaLevels show.
- **Renaming a placeholder to a template name that already exists elsewhere breaks the link or the DOL.** `dtor_800F5FD0` is a second copy of `__dt__Q23std39__vec_deleter<Us,...>` (also at 0x8007B2A0); extab relocations in this unit point at the first copy. Naming both identically either failed with multiply-defined or, with `scope:weak`/`scope:local`, changed extab relocations and the DOL hash. Leave such duplicate copies with a placeholder name.
- **Placeholder-to-name pairing.** `unitdiff.py --json` lists relocation targets for matched functions. Aligning instruction mnemonics of a target function and its source counterpart and comparing `bl` relocation names gives the source name for each `fn_`/`dtor_` callee (scratch script, not committed). Apply the renames only when each placeholder maps to a single name.
- **Never grep unitdiff output for `rror`.** Mangled `length_error` matches; use `Error:` with a leading `#`.
- **`std::pair<s16, s16>` copies use per-field `lha`/`sth`; the original copies the pair as one word.** A plain struct of two `s16` (`LevelMove`) reproduces the word copy.
- **CW folds `x >= 1 && x <= 100` into an unsigned range check; the original did not.** GetWazaLevels keeps a two-compare form in the original; no source spelling found yet that avoids the fold (all four orderings tried).
- **Vector iterator loops that cache the end:** `for (it = begin(), end = end(); ...)` and `it != end()` produce different spills. UpdateCamera needed `it != mPiis.end()` re-evaluated each iteration, while HasPrefixedItem needed a raw-pointer pair declared as `first`, `end = first + size()`.
- **Class-template helper names pair by call relocations only for callers that exist in your object.** Extab-only and vtable-only functions (tuple and `bound_func` destructors, `function_imp` virtuals) have no `bl`; pair them by size and address order against your object's same-class functions.
- **A bound `function` argument that the target passes as `D` by value needs the `bind` parameter to be `D`, not `const D&`.** The copy then becomes an argument temporary created after the member-function-pointer temporary, which fixes stack slot order (ShowTypeFilterMenu, ShowPrefixFilterMenu).
- **`bound_func::operator()(A&)` must build `tuple<A&>` and resolve the placeholder through `resolve_bound_argument`.** Passing the argument straight through omits the target's `stw r4, 8(r1)` spill (function_imp operator() went from 77% to 100%).
- **`x >= 0 && x < N` on a signed `int` folds to an unsigned compare; the nested form `if (x >= 0) { if (x < N) {...} }` keeps both compares.** Used for ShowPrefixFilterMenu.
- **A dynamic_cast target is found from the RTTI symbol in the vtable data, not from the member being called.** PreviewTypeFilter calls CUIList methods but casts to CUIMenu (`__RTTI__7CUIMenu`).
- **Menu sub-functions that are in the `dont_inline on` state call `__dt__` and `HasItemWithPrefix`; the others inline shared_ptr releases.** Toggle `#pragma dont_inline on` and `reset` per function group in address order.
- **`fn_800E0840` in CPii is `std::swap<std::vector<SD_PiiPersonalData::Data>>`, not `operator=`.** `std::swap(rOut, records)` reproduces ExportPersonalData at 100%.
- **Parallel agents share the scratchpad directory.** Another agent overwrote a helper script named `fd.py`; keep personal tools in a per-agent subdirectory.
- **Tried and rejected:** a user-declared `vector::iterator` copy constructor (Filter 100% to 86%), copying `sort` parameters into locals (sort 91% to 80%), removing the `is_POD<UIMenuItem>` specialization (CPauseMenu push_back 65% to 100% but Run 98% to 87%).
# Sprint a12 findings

## Traps

- **Inlined and out-of-line copies of one helper can coexist.** CStatistic::SetWazaUsed inlines the file-local ExpandTo (unrolled zero fill visible) while Set calls it; write the same helper and let the inliner decide. Use `oldSize <= index` to get the target compare order.
- **CStatistic::SetWazaUsed clears from the wrong vector.** The target stores `mValues[word] & ~mask` into mWazaBits when clearing; reproduce it as written.
- **CStatistic::Add's saturating negative path is a std::max against INT_MAX**, with the limit as an addressable local.
- **COyaInfo region 0x800ED1B0-0x800F2580** is a 20 KB block of inlined string/tree containers with EH cleanup; no source exists and it is not split. Not attempted.
- **CPiiCollectionBox::Create** needs an inline constructor `mEntries.resize(496)`; vector operator= (Save/RestoreSnapshot, CStatistic vector assign) is about 90 to 99 percent because of register allocation in include/vector.

## Tail wave (a12, second shift)

### Fixes

- **`reverse_iterator` keeps a dereference temporary.** CPiiBox::Create's copies of an uninitialized word next to each reverse iterator are MSL's `mutable Iterator deref_tmp_` member, declared before `current_`; `operator*` assigns it and decrements it, and `operator!=` compares `base()` through `const&` parameters. `vector` gained `rbegin`/`rend` returning `reverse_iterator<const_iterator>`, with iterator traits on `const_iterator`. No other source used `reverse_iterator`; the full report kept every other unit's count.
- **A reverse list walk is a `const_iterator` copy decremented before use.** `for (it = end(); it != begin(); --it) { const_iterator current = it; --current; ... }` gives the target's register order in resetmgr `Update` (iterator above the element) and errmsg `Present`/`ShowSystemMessage`; the `iterator` copy and `reverse_iterator` forms do not.
- **resetmgr appends callbacks.** `RegisterResetCallback` uses `push_back` (the target inserts before the list sentinel) of a local entry whose `mFunction` is assigned before `mIgnoreMask`; the constructor-built temporary kept a `this` register.
- **`text = a + b` versus `text.assign(a + b)`.** In CTitleSession `RefreshSlots` the swapped iterator-tag slots came from one inline level too few; `operator=` adds it.
- **A temporary argument instead of a scoped local fixes the frame order.** errmsg `ShowSystemMessage` passes `GetFontMap(created)` straight to `LoadFromMemory`.
- Compiler-generated `@NNNN` labels can be named in `symbols.txt` from unitdiff rows (a variant of `name_data.py` that accepts `@` names on the source side). Their numbers drift whenever an included header changes; constants still compare by value.

- **errmsg's `.data` ended 0xB0 bytes early.** The `default_delete<NResetMgr::CDisable>`, `<NErrMsg::@unnamed@errmsg_cpp@::Work>` and `<CRomFont>` names at 0x803C2020 belong to errmsg; homebutton's `.data` starts at `sNormalModes` (0x803C20D0). Both units stay NonMatching and the DOL still verifies.
- **homebutton used invented NResetMgr names.** Its local declarations (`RequestMenu`, `RequestReset`, `IsPowerOffRequested`, ...) did not exist; the target calls `RequestReturnToMenu`, `RequestRestart(0)`, `RequestLaunchManualViewer` (with the `"lmv"` address as its `u32`), `IsRestartRequested`, `CancelRestart` and `IsShutdownRequested` from `NResetMgr.h`.
- UpdateHomeButton (96.4% to 99.65%): the controller loop increments `i` last; `canOpen` is set by `if (!sDisableCount && IsEnabled())`; the state check keeps the loaded `sWaitingToOpen` and tests `!waiting && sWaitingToOpen` after the loop; the aspect test is a `bool` with the `!wide` branch first; the stick deltas are computed `dy` first; `moved = padMoved | stickMoved` gives the `or.`; the pointer copy is per axis. Left: the `waiting` register, the merged zero constant for `use_devtype` and the `false` argument, and the divisor's float register.

### Unsolved

| Unit | Function | % | Cause |
| --- | --- | --- | --- |
| CTitleSession | `Run` | 99.92 | `.data` order: the target emits the CUIManager and CTitleSession deleter vtables before `Run`'s literal pool and the BUILayout deleter after the `basic_string` pools; ours puts all three deleters after the pools. Making `shared_ptr(Y*)` inline or in-class only reorders them. |
| CPauseMenu | `Create`, `Run`, `FormatMessage` x2 | 99.9 | same `.data` order problem: the `<u8, void (*)(void*)>` deleter vtable comes after `function_base` in the target and first in ours |
| CPauseMenu | `LeaveStage` | 98.73 | `GetHubIndex` bitfield packing order; aggregate, member-wise and by-value forms tried |
| CPauseMenu | `TakeScreenshot` | 99.77 | register allocation of the work buffer pointer |
| errmsg | `GetPaneRectangle` | 88.70 | the first three instructions are scheduled above `stwu` in ours; local order, const and reference forms tried |
| main | `HandleFileError` | 96.61 | `UsesAlternateErrorMessage` range checks: the combined `||` form folds into the switch's range compare, the two-`if` form lays the false path inline |
| homebutton | `InitializeHomeButtonInfo`, `UpdateHomeButton` | 98.57, 99.65 | parameters get the lowest non-volatile registers in ours, the highest in the target |
| CShadowCamera | `SetupShadowModel`, `SetupShadowMaterial` | 95.20, 99.88 | argument load scheduling around the by-value `GXColor` copies |
| CPii | `swap<vector<Data>>`, bad_function_call `what`/dtor | 97.70, missing | see sprint-tZ; the bad_function_call weak functions come from a throw site the linker dropped |

### Tail wave: flip-ready units

- **CUIMenuSoundCallback, CUIPiiListItem and ScriptCamera are Matching.** Only CUIMenuSoundCallback's `.data` blocked the link. The unit lacked the `shared_ptr_deleter<function<void(BUI*)>>` RTTI that the target emits right after the bind member-pointer table, together with the `shared_ptr_deleter_common` and `default_delete<function<void(BUI*)>>` names at the end. The constructor still calls `Add` out of line, but the unit has to instantiate `Add`'s body, so it defines the inline `CCallbackListOwner::Add` after `OnScroll`, as CDlgCollection and CDlgPiiList do. Including `CUIMenuItem.h` before `CUIMenu.h` puts the CUIMenu RTTI base list ahead of the CUIMenuItem name, as in the target.
- **Duplicate weak vtables leave zeroed space in the target.** In CUIList and CUIMenuSoundCallback, 0x14 or 0x18 zero bytes come before an RTTI name string where the target object had a weak vtable whose winning copy is in an earlier unit. Treat such a zero run as a vtable this unit must also instantiate, not as padding.
- **ut_ArchiveFontBase is not a regression.** `ConstructOpAnalyzeGLGR` has been at 99.29% since #748 (see 806-archive-font-registers). It is a pure register permutation: the target gives `glyphsPerSheet` r31, `sizeAdjustTable` r30 and the adjustment-table pointer r22, while ours gives the pointer r31. Swapping the `numSheet` and `glyphsPerSheet` declarations changes nothing.

| Unit | Function | % | Cause |
| --- | --- | --- | --- |
| CBattleRoyalDisplay | `FormatString` position | 100 (order) | The target emits the wide `FormatString` in the second drain, after the round-1 templates and before the implicit `~CBattleRoyalDisplay`. A strong definition, an inline one, an in-class friend of a plain class, and a friend of a class template completed at parse time all put it right after `Calc` or earlier. Remaining hypothesis: the body becomes available only after `Calc`'s call-graph node is created (sprint-ida rule 10), for example a friend in a class template completed during the round-1 walk. |

## Final tail wave (a12, third shift): CRemoteMemory, CPpuEngine and others

### Traps

- **`#pragma opt_optimizecontrolflow off` keeps switches as binary trees.** With the global optimizer on, CodeWarrior Wii/1.1 rewrites a switch with fewer than about eight cases into a linear compare chain (or a `cmplwi` range check). This happens in every Wii compiler version and with every `-O`, `-ipa` and `-inline` flag. GameCube compilers and `global_optimizer off` keep the backend's binary tree instead. The undocumented pragma `opt_optimizecontrolflow off` keeps the tree and leaves every other optimization on. It also keeps redundant compares and does not merge branch targets. At file scope it is right for CRemoteMemory (Update stays 100%, every switch function improves). It is wrong for NPpuVM and ef_animcurve (both drop). In ef_drawfreestrategy `Draw` it raised 97.50 to 98.39 and in ef_drawdirectionalstrategy `DrawDirectional` 97.53 to 98.10, but other functions in those units dropped. Try it per function with `#pragma push`/`pop` wherever a target has `cmpwi; beq; bge` trees for a short switch. Other `opt_*` names in the compiler binary (`opt_foldconstants`, `opt_propagation`, `opt_rebuildconditionals`, ...) are accepted, and `opt_propagation off` works inside push/pop.
- **`global_optimizer off` adds a frame pointer only when a block-scope local exists.** Any local declared in a nested scope, including a `for` init or an inlined `case {}` block, gives `mr r31, r1`. Inline-function top-scope locals do not. A function with no frame pointer in the target can still be optimizer-off if all its locals are at function scope.
- **Arguments that are constant expressions fold even through two inline levels.** `SetState(static_cast<Flag>(A | B))` folds into the callee's `state | C` at parse time. Only a plain enumerator argument folds differently in the target (BeginRead's third `SetState` loads `0x208`, the first two load `0x200` and OR the state). No source form was found for the unfolded pair.
- **A vector-like destructor with one extra `beq` can be a class derived from the vector.** CPpuEngine `SendMessage`'s extab lists `DESTROYBASE vector<int>` and a local whose destructor is `dtor_801021A0`. That local is a struct derived from `vector<int>` (`HandlerList`), not a POD vector with a deeper MSL chain. Adding a `__vec_constructor<T,A,true> : <T,A,false>` level to all POD vectors is wrong: the target's own `vector<int>` destructor keeps the short chain.
- **A materialised `||` with the result preset to false (`li 0; beq set; bne end; set: li 1`) is `bool r = false; if (a || b) r = true; return r;`.** NPpuVM `BitwiseOp` inlines its integer test in this shape. Changing `CValue::IsInt` this way breaks Slice and ScriptGame, which use the plain `||` shape, so a separate helper is needed. That helper gained only 0.1% (register reuse of the loaded type still differs), so it was not kept.

### Fixes

- CRemoteMemory: `opt_optimizecontrolflow off` for the file. A file-local `FindValidBlock` selects the read block with early returns, which gives the target's shared `li 3` block. FinishRead has an explicit `case -4`. BeginRead runs with `opt_propagation off` and a local state mask in `SetState`, and reads the connection bit as `(flags >> 6) & 1` from one load. ThreadMain tests the connection result with `result != 0 && result == -1` (linear in the target). Scores: ThreadMain 78.97 to 81.74, BeginRead 87.98 to 95.82, FinishRead 56.93 to 63.44, PrepareWrite 77.19 to 82.24, FinishWrite 44.66 to 51.05. GetPiiBox 65.14 to 85.80 with `global_optimizer off` in a push/pop region (it combines with the file-wide pragma). Per-function `opt_common_subs off` or `opt_propagation off` add 2 to 4 points to PrepareWrite and FinishRead; not kept.
- CPpuEngine: `HandlerList` (derived from `vector<int>`, built from the callback list range) is the local in `SendMessage`; `0x801021A0` is its destructor (now 100%).

### Unsolved

| Unit | Function | % | Cause |
| --- | --- | --- | --- |
| CRemoteMemory | GetPiiBox | 85.80 | Optimizer-off in the target (materialised `(mFlags & mask) == value` with mask and value in registers, frame pointer); the list-assign iterator copies and stack slots still differ |
| CRemoteMemory | BeginRead, FinishRead | 95.82, 63.44 | `0x200 \| state` unfolded for the two composite states; FinishRead keeps `channel * 0x440` in a saved register where the target recomputes it |
| CRemoteMemory | ThreadMain, PrepareWrite, FinishWrite | 81.74, 82.24, 51.05 | ReadBack's tree root is -1 in ours and -2 in the target (the target has an extra `-4` node going to the default body); PrepareWrite's list `!=` is materialised in the target |
| CPpuEngine | SendMessage | 82.94 | The range construction copies 8-byte iterator objects (word plus uninitialised byte) in the target |
| CPiiListFilterSort | CompareBPAscending/Descending, CompareSpeciesName | 99.3-99.6 | Item pointers r5/r3 versus r3/r4. Without locals the items get r3/r4 but B loads first (right-to-left argument evaluation); item locals plus data locals return to r5/r3 |
| CPiiListFilterSort | Sort | 96.16 | `*(comparators + n)`, explicit template arguments and named iterators do not move the `mSortType` load |
| CGameSession | GetSnapshot | 88.24 | Store order of play time, money and count; direct member stores, a separate `now`, a reference alias and local-order variants are worse or unchanged |

## Final tail wave (CPiiListFilterSort, CModel)

CPiiListFilterSort 60 to 64 of 71 functions at 100% (CompareBPAscending, CompareBPDescending, CompareSpeciesName, ShowPrefixFilterMenu). CModel 152 to 154 of 158 (`~CModel`, `Load`).

### Traps

- **Reading a value before the pointers it comes from fixes a parameter-register swap.** The BP comparators loaded the item pointers into r5/r3 instead of r3/r4. Reading `int bpA = rA->GetPPD()->GetBP(); int bpB = ...` before the `pA`/`pB` data pointers gives r3/r4. The inlined copy in CompareSpeciesName also needed the creation-time comparison through the `const T&` `Compare` template instead of the by-value `CompareValue`.
- **Wrapping a whole inline body flips the frame-slot order of its exception objects.** `ScnMdlSimpleData::Reset` forwarding to an inline `Release()` that holds `mModel.reset()` and the vector swap reverses the two `__unexpected` slots and fixes the registers in CModel `Load` and `~CModel`. Wrappers around only one of the two statements, which earlier attempts tried, do not.
- **`0 <= x && x < n` keeps two signed compares.** In ShowPrefixFilterMenu, nested `if (choice >= 0) { if (choice < 44)` was folded into `cmplwi 0x2b`.

### Unsolved

| Unit | Function | % | Cause |
| --- | --- | --- | --- |
| CPiiListFilterSort | Sort | 96.16 | The target loads `mSortType` before storing `begin()` into its argument temporary. A helper template, named iterators, unsigned or long `mSortType` and a `const` array did not change this. |
| CPiiListFilterSort | `sort<..., RPF>` (shared_ptr), both `sort<UIMenuItem>` | 97.78, 99.37 | The 0x66666667 constant, `first` and `last` get different non-volatile registers. The same template matches for LevelMove and pointer elements. Non-const, reinterpret and out-of-class forms of the UIMenuItem conversion operator did not change this. CGameResult has the same 97.78 instance. |
| CPiiListFilterSort | ShowTypeFilterMenu | 99.93 | `li 1` and `kind` swap r3/r4 in `1 << kind`. Mask type, a caller-computed mask, `const int&`, an item parameter, a `TypeBit` helper, a `kind` local and `mask <<= kind` did not change this. |
| CModel | TPartialAnm<1> QueueMotion/AppendMotion/Update | 99.4-99.6 | The `IsIdle` value temporaries are swapped. `IsReplaceable` written as `if`, a local, a `&&`/`||` mix, `!= false` or a folded `Main &&` term, and `IsEmpty`/`IsLastFinished`/`IsFinished` helpers did not change this. |
| CModel | Update<0> | 99.80 | Moving `advanced = true` before `pop_front` fixes the node register (r27) but puts `li r30, 1` before the pop. The target sets it after the `mAnimObj.reset()` block. `advanced |= true` did not change this. |

### Follow-up attempts (no change, all reverted)

- **`sort` instances:** a `__sort_ref` trait that collapses `TCompare&` in the recursion and the inner calls left every score unchanged. A user-declared `vector::iterator` copy constructor regressed the unit from 63 to 59 functions.
- **`Sort`:** a `Comparator` local before the call scored 74%. Reading `mSortType` again after the call changed nothing.
- **ShowTypeFilterMenu:**
  - Swapping the helper's parameter order, a `begin()` local before the mask, and `mask & G()` versus `G() & mask` did not move the registers. A `const_iterator`, an `end` local and a `>> kind` test all scored worse.
  - `(1 << kind)` written inside the loop gives `li r3, 1` but shuffles r4 to r6 (99.74%).
- **IsIdle:** a size local (`u32` or `int`) scores worse (94-98%). A dummy local and an explicit `!= 0` changed nothing.
- **Update:** moving `pop_front`, the start frame or `mAnimObj.reset()` into inline helpers scored worse or the same.

## Register-allocation wave (a12)

Diagnoses use `tools/re/ra_trace.py` and the rules in [sprint-ra](sprint-ra.md).

### Fixes

- CMsgRes `ConvertEscape` 95.57% to 96.40%: the hex digit in the `\x` loop is a named local declared before `pEnd`. Named locals are coloured in declaration order, so the digit now takes r11 before `pEnd` takes r12, and `isTrailByte` opens r31, as in the target. Declaring `i` before the `value = 0` initialisation (`int value; int i = 0; for (value = 0; ...)`) also reorders the two `li` and gives 96.47%; not kept. All 120 orders of the five leading declarations were tried; none beats 96.47%. The rest is the preheader schedule (`srwi` and the locale `addi` before `mr r4`) and the escape byte: an `int escape` gives the target's single register but renumbers the function (92.56%).

### Diagnosed, not fixed

- CPauseMenu `TakeScreenshot`: the swapped registers are `pMode` (temporary r102) and the `fbWidth` and `efbHeight` loads (temporaries r105, r106). The target colours `pMode` first, then `efbHeight`, then `fbWidth`, so the two loads must be variables (named locals, or inline variables with the height created first). Declared locals are deleted by the IR in every form tried (top-of-function declarations, either order).
- CRemoteMemory `PrepareWrite`: generation order is channel multiply, pool base, block index; the target needs multiply, index, base (the base is coloured first and gets r0). Pointer-arithmetic and commuted index spellings generate the same order.
- BWorld `RemoveObject`: `pNext` and `pPrev` are inline return variables of `GetNext` and `GetPrev`; the first expanded is coloured first. The target computes `pNext` first but colours `pPrev` first. A spelled-out ternary for `pNext` with `GetPrev` for `pPrev` reuses the `mNext` load (97.73%).
- NPpuVM `~CEngine`: the in-charge flag and `this` have degree K (29) and wait; the node (a merged copy chain) is coloured second, before `this` and the tree. The target colours flag, `this`, tree, node.
- CShadowCamera `SetupShadowModel`: inline-call arguments are generated right to left (`IsTextureAlpha`, `GetIndex`, the `color` copy, `GetTexture`, `mat`). The target order is the `color` copy, the `mat` copy, then texture, index and alpha left to right, which is what plain member reads give: direct `mTextures[mTextureLevels - 1]`, `mIndex` and `mUseTextureAlpha` (with the members made public) reproduce the order but not the `mat` store position (91.86%). `mIndex` is private to BWorldCamera, so this was not kept.
- CShadowCamera `SetupShadowMaterial`: the `postMtx` temporary gets r3 because `addi r3` (the `GetTexObj` argument) comes after it in the pre-allocation schedule. The six orders of `second`, `postMtx` and `map` do not move it (best 99.88%).
- CPiiPersonalData `Import`: the target stores `mRandomSeed` before `mCreationTime` but loads the low creation word before `mFlags`. Moving the seed assignment first fixes the store order and loses the hoisted load (95.45%). All 24 orders of the last four assignments, a `lo | hi << 32` getter, a `u64` cast and an `s64` local are no better than the current 97.08%.
- CPiiListFilterSort `Sort`: the pre-allocation order is the store of the `begin()` result, the iterator addresses, then the `mSortType` load. The target hoists the load above the store, so in the target the two accesses do not alias.
- homebutton `UpdateHomeButton`: the `false` argument of `InitializeHomeButtonInfo` is merged with the `use_devtype = 0` constant before scheduling; the target keeps two `li`. `WPAD_DEV_CORE` changes nothing.
- BWorld `Calc` and `CalcMtx`, CPlayerPii `GetShadowName`, CPauseMenu `LeaveStage`, main `HandleFileError`, NPpuVM `CCode::Load`, and the CNoticeDisplay and homebutton `.data` layouts are unchanged; see the earlier tables.

## Fake-match wave (a12)

### Fixes

- main `HandleFileError`: the inlined `UsesAlternateErrorMessage` needs `drive < 2 || number >= 512` with `int number = drive;`. With the same variable on both sides, the two compares fold into one unsigned range compare. Separate `if`s lay out two `li 0` blocks. A different variable keeps two signed compares that share the target's single false block (fake, see [the ledger](../FAKEMATCHES.md)).
- main `main`, 88.83% to 99.85%: the original calls `ResourceFileConfig::SetErrorHandler` out of line.
  - `__declspec(noinline)` on its definition does that without touching other inlining.
  - `#pragma auto_inline off` and `dont_inline` around the definition also stop `main` from inlining the `function` constructors (78.55%).
  - The remaining difference is `li r4,1000 ... mr r5,r4` for `InitializeEffect(500, 1000, 1000, ...)`: the target merges an equal `u32` constant. Locals, `register`, `u16` and `s16` casts, inline wrappers, pointers and `opt_propagation off` did not reproduce it.
- CMsgRes `ConvertEscape`, 96.40% to 96.58%: the escape byte is `signed char`, because the target stores the sign-extended register. The rest is the preheader schedule: the target places `srwi` and the locale `addi` before `mr r4`. The following did not fix it: all 360 orders of the leading declarations, the `scheduling` models, and the opt pragmas listed below.
- homebutton `.data` now matches the original byte for byte: `#include <memory>` must come first, which creates `shared_ptr_deleter_common` before `bad_function_call`.
- homebutton `.sdata2` now matches too:
  - The wide-mode width scale is `0.9558499f` (0x3F74B294); the old literal rounded to 0x3F74B2A4.
  - `MoveClassicCursor` must use literal steps, because `const float` locals are also emitted as `@LOCAL@` constants.
- CPauseMenu `.data` now matches the original except for trailing padding:
  - `#include <stdexcept>` first gives the exception and interface RTTI order.
  - A `sizeof` probe of `shared_ptr_deleter<u8, void (*)(void*) throw()>` placed right before `IPauseMenu::Create` puts that deleter vtable after `function_base`.
  - The probe's type must be deduced from `GameMemory::Free`, because that is the only way to get the exception specification. A probe that spells out `void (*)(void*)` creates a different class.

### Tools and traps

- **Compile scratch copies outside `build/`.**
  - Run the unit's `mwcceppc` command on a copy that keeps the original file name, because the anonymous namespace is mangled from it.
  - Compare sections with `objcopy -O binary -j .data`.
  - No ninja run is needed, so this works while another build is running. It also shows section order, which objdiff's fuzzy `.data` percentage hides.
- **`mwcceppc` accepts many undocumented `#pragma opt_*` switches.** A string scan of the executable lists `opt_defineargorder`, `opt_repositioncode`, `opt_rotateloops`, `opt_rebuildconditionals`, `c99_alias_by_type` and others.
  - Applied around single functions, none of them fixed a function in this wave, and most changed nothing.
  - `opt_prelinearize off` crashes the compiler, and `opt_pointer_analysis on` fails with error 10184.
- **A stopped background task leaves its child processes running.** Stopping a bash task left `python` sweep processes that kept rewriting sources. Find them with `Get-CimInstance Win32_Process` and stop them before landing.
- **`sed -i` converts CRLF sources to LF** in Git Bash; restore CRLF before committing.

### Unsolved (diagnosed this wave)

| Unit | Function or section | % | Cause |
| --- | --- | --- | --- |
| CPlayerPii | `GetShadowName` | 88.86 | Scheduling only. `#pragma scheduling 604` or `7450` fixes the body (90.48%) but moves the `lr` save one slot later, and non-const names hoist the load above the prologue. The unit also has 63 misplaced weak deque functions. |
| CPauseMenu | `.rodata` | - | The original emits `ShowMenu`'s `UIMenuParam` (`@9061`) after `RunRemoteMemoryMenu`'s two tables. Moving `ShowMenu` out of the class, after `RunRemoteMemoryMenu`, fixes `.rodata` but moves the `vector length error` strings in `.data`. |
| CNoticeDisplay | `.data` | - | Defining `gAreaPaneNames` where the `extern` declaration is moves the first difference from 0xAC to 0xD0. The `shared_ptr_deleter<CNoticeDisplay>` vtable must come before the strings of `LoadLayouts`, and the `function_imp` and deleter vtables must come after the exception RTTI. `sizeof` probes, unused inline constructions and non-`inline` `StartNotice` definitions did not move them. |
| homebutton | `UpdateHomeButton` | 99.65 | The stick divisor and `dy` get f1/f4 instead of f0/f1. The `waiting` register and the merged zero also differ. Six spellings of `dy` and `dx` were tried. |
| BWorld | `Calc`, `CalcMtx` | 99.70, 99.37 | The target's first `states \|= 2` is `li r0,0; ori r30,r0,2`. `states = states \| 2`, `+=`, `^=`, a ternary and a zero local do not produce it. |
| NPpuVM | `CCode::Load` | 88.05 (90.71 with header locals) | The target hoists every `pHeader` load above the stores to `this`, as if they cannot alias. `__restrict`, `c99_alias_by_type` and the pointer-analysis pragmas did not reproduce this. |
