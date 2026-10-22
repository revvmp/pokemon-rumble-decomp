# Sprint a04 findings

## Traps

- Symptom: a large in-class `Show` (CDlgCollection, CDlgInfo, CDlgPiiList) compiles with every shared_ptr/vector helper called out of line (2344 target bytes versus about 1068), so the match stays near 20 to 30 percent. Compiling with `-pragma "warn_notinlined on"` lists each refused inline. Verified: the refusal is function-wide, not per call. In CDlgInfo, `reset(new Impl)` plus the page loop plus `Fiber::Start(bind(...))` together refuse even the first `if (mImpl)`; removing any one restores inlining. Adding `push_back(shared_ptr<CDlgInfoItem>(new ...))` to the loop moved the refusal count from 3 to 16. `#pragma inline_max_size`, `inline_max_total_size`, `inline_depth` and `-inline level=n` had no effect. `#pragma always_inline on` inlines everything, which is too much. Making `Load`, `Update` and `Run` out-of-class did not fix it. Unresolved; the original source shape that avoids the refusal is unknown.
- Symptom: loop registers differ in Draw, Calc, CalcView and CalcMtx (Collection and Info). The target gives the index the highest callee-saved register and the byte offset the next; ours is reversed. while, pre-increment, reference, hoisted-index and direct-access variants all left the result unchanged.
- `CDlgInfoItem` takes its model `shared_ptr` by value (caller makes a temp copy). `Line` helpers `HasText`/`HasColumns` and `PaneRectangle::GetWidth` reproduce the target's temporaries in `ItemLine::Update`.
- Several shared names (`__dt__5ICalcFv`, `IDlg::IsClosable`, `shared_ptr<CLayout>` helpers) are already defined in other units. Pair-naming by size and match percent must be followed by a link to catch multiply-defined symbols; revert those names to `fn_`/`dtor_`.
- The scratchpad directory is shared between agents; use a private subdirectory for helper scripts.

## Final tail wave (a04)

ScriptPii flipped to Matching. ScriptWorld gained SetupScriptWaza and its five `shared_ptr_deleter<CWaza>` clones (36/37). ScriptWorldObj gained eleven functions (189 of 192 target functions at 100%). CDlgCollection gained its `function_base<void(*)(BUI*)>` destructor.

### Findings

- **The extab omits calls to inline functions that cannot throw.** NewPlayerPii's range at `0x6c` covered the `GetPersonalData` call (return address `0x7c`) in our build but not in the target. Making `CPlayer::GetPersonalData` and `GetPendingPersonalData` header inlines (they were out-of-line in CDlgPiiList.cpp) let the compiler see a body with no calls, so the call no longer needs the `data` cleanup, even though it stays out of line. `throw()` on an out-of-line declaration did not do this. The two getters keep `#pragma scheduling off` around their header definitions; the CDlgPiiList copy constructor of `shared_ptr<CPiiPersonalData>` now instantiates under that pragma and drops to 58.8% (CDlgPiiList is NonMatching).
- **An earlier instantiation moves a template member to reference order.** In ScriptPii the target emits `shared_ptr<CModel>::operator bool_type` right after `method_getNodeY`, before the plain inlines it calls. An in-class `BModelObj::HasModel` that tests `mModel` instantiates it at include time and reproduces that order.
- **Casting the engine once fixes the search helper registers.** ScriptWorldObj's `Search*`/`Count*` helpers match with `CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);` as the first statement and every pop through it. The rectangle half width is `width / 2.0f` (variable first in `fmuls`). The same cast did not move SetStageDescTarget or MakeMapID, alone or with every subset of expanded pops.
- **`pair`'s converting constructor carries MSL's `restrict_to` guard.** `set<int>::insert` passes `li r5, 0` to it. `restrict_to` and `is_convertible` now live in `MSL_CPP/msl_utility.h`; no other function changed.
- **SetupScriptWaza takes the parent by value and `CWaza::Setup` has a third, unused `u32` parameter** (the caller passes `li r6, 0`). The parameter's meaning is unknown.

### Unsolved

| Unit | Function | Match | Cause |
| --- | --- | --- | --- |
| ScriptWorld | `SetStageDescTarget` | 99.71% | world/rank r30/r29; cast local plus all eight pop-expansion subsets tried |
| ScriptGame | `MakeMapID`, `IncMapClearedNum` | 96.5%, 99.5% | register order; four `IncrementClearCount` bodies and a bitfield `CMapRecord` were worse |
| ScriptWorldObj | `tree_begin_holder<less<int>>` ctor, local `IsKindOf<CPii>` | 8.3%, unpaired | the target calls an out-of-line comparator constructor, and keeps two `IsKindOf<CPii>` copies in one object (one local, one global) |
| CDlgCollection | `Calc`, `CDlgCollectionItem` ctor | 99.67%, 94.7% | index-offset/pItem register swap; inlined `vector<u16>` copy CSE (removing the `n` local in `construct_at_end` drops to 91.7%) |
| CDlgInfo | `__dt__nw4r::ut::Rect`, local `vector<shared_ptr<CLayout>>` dtor, 4 near misses | | the target emits an unreferenced `~Rect` after `Update`; inlining `GetTextWidth` into `Update` did not emit it |
| CGameResult | `Create` | 99.94% | the string pool at `0x803A2938` is a section symbol in our object; naming it `...data.0` fixes the `lis` only |
| SaveData, PiiPropData, CGameResult `RunAction`/`Calc` | | | unchanged; see earlier sprint notes |
