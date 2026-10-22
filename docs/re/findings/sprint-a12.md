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

- CRemoteMemory: `opt_optimizecontrolflow off` for the file. A file-local `FindValidBlock` selects the read block with early returns, which gives the target's shared `li 3` block. FinishRead has an explicit `case -4`. BeginRead runs with `opt_propagation off` and a local state mask in `SetState`, and reads the connection bit as `(flags >> 6) & 1` from one load. ThreadMain tests the connection result with `result != 0 && result == -1` (linear in the target). Scores: ThreadMain 78.97 to 81.74, BeginRead 87.98 to 95.82, FinishRead 56.93 to 63.44, PrepareWrite 77.19 to 82.24, FinishWrite 44.66 to 51.05. GetPiiBox stays at 65.14; with `global_optimizer off` it reaches 85.80, but that does not combine with the file-wide pragma.
- CPpuEngine: `HandlerList` (derived from `vector<int>`, built from the callback list range) is the local in `SendMessage`; `0x801021A0` is its destructor (now 100%).

### Unsolved

| Unit | Function | % | Cause |
| --- | --- | --- | --- |
| CRemoteMemory | GetPiiBox | 65.14 | Target looks optimizer-off (materialised `(mFlags & mask) == value` with mask and value in registers, frame pointer); `global_optimizer off` gives 85.80 but the list-assign iterator copies and slots still differ |
| CRemoteMemory | BeginRead, FinishRead | 95.82, 63.44 | `0x200 \| state` unfolded for the two composite states; FinishRead keeps `channel * 0x440` in a saved register where the target recomputes it |
| CRemoteMemory | ThreadMain, PrepareWrite, FinishWrite | 81.74, 82.24, 51.05 | ReadBack's tree root is -1 in ours and -2 in the target (the target has an extra `-4` node going to the default body); PrepareWrite's list `!=` is materialised in the target |
| CPpuEngine | SendMessage | 82.94 | The range construction copies 8-byte iterator objects (word plus uninitialised byte) in the target |
| CPiiListFilterSort | CompareBPAscending/Descending, CompareSpeciesName | 99.3-99.6 | Item pointers r5/r3 versus r3/r4. Without locals the items get r3/r4 but B loads first (right-to-left argument evaluation); item locals plus data locals return to r5/r3 |
| CPiiListFilterSort | Sort | 96.16 | `*(comparators + n)`, explicit template arguments and named iterators do not move the `mSortType` load |
| CGameSession | GetSnapshot | 88.24 | Store order of play time, money and count; direct member stores, a separate `now`, a reference alias and local-order variants are worse or unchanged |
