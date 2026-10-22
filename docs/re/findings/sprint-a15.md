# Sprint a15: script bindings units

## Traps

- **Bindings keep the base engine parameter.** A `CPpuEngine*` local changes register allocation; take `NPpu::CEngine* pEngine` and cast at each `PopWorldObj`/`GetWorld` call (moveTo, isInKeepOutArea, camera and world-object bindings).
- **A for loop with a pointer and a counter matches the original element scan.** `CountMergedIds` matched only with the pointer declared before the counter and `++pSlice, ++count` in the increment.
- **String-pop-first bindings stay about 4% off.** Ordinary play, request, requestNext and getMotionLength bindings of camera, light set, null object and model units (16 functions) use `lwzux` and compute the Slice address late in the original. Tried without effect: member `PopString`, free `PopValue`, returned-by-value `Pop`, `CValue(mStack[...])` temporaries, pointer-returning `Pop`, copy-initialised locals.
- **The world-object bindings share one .sdata2 pool from 0x804BA280.** Splitting the pool across units made every float load differ by symbol; one unit `ScriptWorldObj.cpp` covers 0x8014B030-0x8014FB10.
- **Auto-pairing callee names can duplicate a global name.** Two addresses given the same mangled name fail the original-object link (`multiply-defined`); check duplicates in symbols.txt after bulk renames.
- **File-local statics that share a global name elsewhere need `scope:local`** in symbols.txt (`Format` overloads in ScriptMessages).
- **`std::list<BWorldObj*>` iterators are returned in r3 in the original.** The repo `include/list` returns them through a hidden pointer, leaving PushObjectIds, MergeObjectIds and the `std::set<int>` and list helpers at 0x8014DA60-0x8014E5F0 unmatched.

## Tail wave (agent tW)

Flipped `CTextWorldObjStat` and `CPiiInfoWorldObjStat` to Matching. Both clip at plus or minus 1.2, not 1.0.

### Traps

- **A linker-stripped literal leaves a permanent objdiff offset.** The inline `CLayout::CreateMotion(const char*)` pools `0.0f` and `1.0f` in every unit that uses it. The linker drops the weak copy and its unreferenced `1.0f`, so the original `.sdata2` lacks it and later literals sit 4 bytes lower. The function stays at 99.8% in objdiff while the DOL verifies. Flip such a unit on full verification.
- **Interface RTTI must carry its mangled name in the owning original object.** `IWorldObjStat` has no vtable in the DOL; its RTTI `0x804B69C8` is in `CHPBar`'s fallback object. While it was a `lbl_` name, a source unit's weak `__RTTI__13IWorldObjStat` and type-name string were kept, growing `.data` and `.sdata` by 0x20 each.
- **A unit's `.data` split must start at its first object, not at the preceding padding.** `CPiiInfoWorldObjStat` and `CUIMenuSoundCallback` started 4 bytes early, offsetting every data relocation. Moving the start exposed a `gap_` symbol overflowing the previous unit's object; widen the previous unit's last string symbol to the boundary (`lbl_803A8B3C` to 0x30).
- **Merged `lbl_` data blobs hide literal matches.** Splitting a target symbol at the compiled object's boundaries (only where bytes agree) and renaming `lbl_` literals to the compiled `@NNNN` or `@STRING@` names fixed about thirty functions in these units. Scripts: scratchpad `tw/splitdata.py` and `tw/strren.py`.
- **An if-else chain of character ranges becomes a binary search whose bodies follow source order.** `ConvertCase` matched only with the full-width range tested before the Cyrillic ranges, in the order full-width, upper Cyrillic, lower Cyrillic.
- **A non-const accessor in a const function means the member is `mutable` or cast.** The DOL has no const `vector::operator[]`; `PiiPropData::GetWazaLevels` calls the non-const one, matched with `mutable mSpecies`.

### Unsolved

| Unit | Function | % | Cause |
| --- | --- | --- | --- |
| CHPBar | `Update` | 99.96 | Temporary flag of `IsJoined() && GetPendingPersonalData()` in r24, original r4 (see sprint-t1) |
| CBattleRoyalBox | `GetData` | 89.33 | Scheduling of the two address computations of `mData[mode][area]` |
| Message | `InitMsgResList` | 99.99 | `vector(n)` tag slots swapped; same in `BWorld` `vector<ObjEntry>(n)`; header variants made it worse |
| CMoneyLifeDisplay | `Create` | 99.30 | `MapDesc` load scheduled after the vtable store |
| CPlayerPii | `GetShadowName` | 88.86 | `mpPlayer` load scheduled after the name table copy |
| BUI | `OnInput` | 91.70 | Neighbour address strength-reduced in ours |
| errmsg | `GetPaneRectangle` | 88.70 | Computation scheduled above `stwu` in ours |
| CUIList | `OnInput` | 99.74 | Original computes the wrap index as `0 - 1` from a register |
| CShadowCamera | `SetupShadowModel` | 95.20 | Stack slots fixed with an inline helper; argument load order remains |
| BModelObj | `CalcSize` | 95.72 | `fabs` register of the minimum z, and scale load order in the unscaled branch |
| CMsgRes | `ConvertEscape`, `MakeCollationKeys` | 94.66, 81.85 | Register allocation |
| CNoticeDisplay | three `Init` | 99.6-99.9 | `.data` order: the original emits the `CNoticeDisplay` deleter vtable before the function literals |
| PiiPropData | constructor, `Reset`, `sort<short>`, `SpeciesMoves::operator=` | 95-99.6 | vector tag slots, one register rotation, one extra tag |
| resetmgr | `Update`, `RegisterResetCallback` | 99.38, 96.15 | Iterator register order |

## Tail wave 2 (agent a15)

Flipped `CMultiRanking` and `CHPBar` to Matching. Matched `CUIList::OnInput`; `CGameEnding::Calc` rose from 99.29% to 99.75%.

### Traps

- **A missing exception-table action means the callee was visible and could not throw.** `IMultiRanking::Create` had 8 extra extab bytes: our build gave call sites of `CPlayer::GetPersonalData` a cleanup action that the original omits. A `throw()` declaration changes nothing. Defining the getters inline in `CPlayer.h` removes the actions even where the call stays out of line, because the compiler sees the body under `-ipa file`. This also fixed the `CHPBar::Update` flag register (r4, because the flag is no longer live across the call) and one `CGameSession` function. Commit a560a5ebd reached the same conclusion in parallel. The weak getter copies in `CDlgPiiList` now score 57% because their loads are scheduled; the earlier `#pragma scheduling off` cannot apply to a header inline.
- **Weak `@STRING@` literals give the definition order of inline functions.** The two `"begin"` literals of `CMultiRanking::RunAction` sit after the `Init` strings in `.sdata`, in reverse creation order. A literal in a non-inline function becomes a local `@NNNN` placed early in `.sdata`, so both calls had to be in inline helpers defined before `Init`. Rename the target labels to the compiled `@STRING@Helper__...` names.
- **Inline helpers fix register and stack-slot parity together.** `RunAction` matched only after its skip loop, ranking motion, player model motion and layout motion each became small inline members. The temporaries of the assignment from `Play` must sit at an even inline depth (`PlayModelMotion` inside `StartPlayerMotion`). `CGameEnding::Calc` needed its scroll block in an inline `ScrollStaffRoll` to reverse its tag slots.
- **When every function matches but the DOL does not, check the function order.** `CHPBar` had all 18 functions at 100% and failed verification until its definitions followed the original order: `Initialize`, `Update`, `SetEnabled`, `CalcView`, `IsVisible`, `Draw`, `SetAnimationState`, `GetType`.
- **Stale `@NNNN` names hide matching literal pools.** The GameUtil `.sdata2` labels named the pool of an earlier compile. Rename them by address to the current object's order.

### Unsolved

| Unit | Function | % | Cause and attempts |
| --- | --- | --- | --- |
| CGameEnding | `Calc` | 99.75 | The node register of the two inline `list::erase` calls is r26, but r25 in the original. Tried `pop_front`, iterator locals, range erase, and moving the erase out of the helper |
| CUIList | `vector<shared_ptr<CLayout>>::~vector` | 95.92 | Ours keeps two dead `beq` from the inlined base destructors; the original copy (EH-only, emitted late) does not. The unit's function order also differs from the original (`Load` before `GetLayoutCount`, `OnInput` before `Calc`) |
| CBattleRoyalBox | `GetData` | 89.33 | The original computes `&mData[mode][area]` twice in different association orders; every indexing form tried fails |
| GameUtil | `InitBgmFxPresets` | 92.23 | The original hoists four more float literals into f0 to f5. Tried `static` helpers and other parameter orders and types |
| CMoneyLifeDisplay | `Create` | 99.30 | The `*pMapDesc` load sits before the vtable store. A by-value `MapIndex` constructor argument reproduces it but adds a stack copy |
| Message | `InitMsgResList` | 99.99 | `vector(n)` tag slots. Tried wrapper-based, explicit-tag and reordered-tag variants of `__vec_constructor(size_type)` |
| CDividedGround | `LoadAttr` | 99.27 | The first `file->IsValid()` reloads the pointer for `mPending`. Tried `!file || !IsValid`, `get()`, `!= nullptr`, a bool local and direct initialisation |
| CDlgMsgWin | `shared_ptr_deleter<char, default_delete<char[]>>` constructor | 62.96 | Scheduling of the empty deleter copy; a user-declared `default_delete<T[]>` constructor makes it worse |
| BUI | `OnInput` | 91.70 | Ours strength-reduces the neighbour address. Tried `for` scope, pointer arithmetic and operand order |
| CUIMenu | `OnInput` | 98.62 | Extra `b` after each `return` in a scope that holds a `shared_ptr`. An inline `SelectIfSelectable` helper dropped the function to 93% |
| CEffect | `InitializeEffect` | 99.62 | Pool count and pointer registers. Tried declaration order, removing the count local, and a member template `AllocArray` |
| CPiiPersonalData | `Import` | 97.08 | Store order of `mRandomSeed` and `mCreationTime` |

## Tail wave 3 (agent a15)

Matched `CUIMenu` `RunMenuList`, and `CPauseMenu` `Create`, `Run` and both `FormatMessage` instances. `CUIMenu::OnInput` rose from 98.62% to 99.89%, `IMoneyLifeDisplay::Create` from 99.30% to 99.63%, and the `CDlgMsgWin` wide string copy constructor now calls the target's `alloc()`.

### Traps

- **A word load of a stack parameter means a 4-byte type.** `RunMenuList` loaded its last stack argument with `lwz` and passed it on unconverted, so the flag is `int` in it and in both `CUIMenu::Load` overloads (symbols renamed from `b` to `i`). `bool` gives `lbz` at offset +3, and an `int` to `bool` conversion adds `neg`/`srwi`.
- **A dead `b` after an inlined destructor's `__unexpected` stub comes from `return` inside the scope.** `CUIMenu::OnInput` lost all three once the early returns became `goto done` to a label before the closing brace.
- **The basic_string copy constructor takes the allocator through the non-const `alloc()`.** The target calls `alloc__...Fv` on the const argument, so `include/string` uses `const_cast<basic_string&>(str).alloc()`. The full report kept every unit's count.
- **`u16` locals keep their conversion late.** `GetMotionLength` returns `u16`; storing it in a `u32` moved the `clrlwi` before the float-conversion constant.
- **objdiff base-register pseudo-relocations reveal oversized labels.** Rows with identical text but different `R_PPC_NONE` targets (`lbl_X` plus addend versus `@N`) are fixed by splitting the target label at the compiled object's boundaries where bytes agree, and by merging adjacent 4-byte `.sdata2` labels that the compiler emits as one 8-byte object (`@N+0x4` in our rows).

### Unsolved

| Unit | Function | % | Attempts |
| --- | --- | --- | --- |
| CBattleRoyalBox | `GetData` | 89.33 | Direct `return mData[mode][area]` gives the target's two address computations but schedules one add after the load (85%); row pointer, row reference, casts and const forms fail |
| CUIMenu | `OnInput` | 99.89 | The `selected % mRowCount` branch uses the saved copy in the target and the call result in ours; candidate, inline helpers, reference and local forms tried |
| CDividedGround | `LoadAttr` | 99.27 | An inline `IsLoaded(const shared_ptr&)` reproduces the inner `IsValid` registers exactly but materialises the helper result; negated, inverted and split forms tried |
| CMoneyLifeDisplay | `Create` | 99.63 | Map index load before the vtable store; mode, `MapIndex&` and `MapDesc&` constructor arguments tried |
| CDlgMsgWin | `shared_ptr_deleter<char, default_delete<char[]>>` constructor | 62.96 | Explicit `D(rDeleter)` and cast forms change nothing |
| CPauseMenu | `LeaveStage` | 98.73 | Zero-then-assign area and mode gives the target's extraction order but the wrong insertion order |
| Message | `InitMsgResList` | 99.99 | A named `pod_type tag = pod_type()` for `construct_at_end` puts both tags in the target slots but stores the second tag early |
| main | `HandleFileError` | 96.61 | The target shares one `return false` block between `drive < 2` and `drive >= 512`; combined, nested, goto and result-variable forms tried |
| BUI | `OnInput` | 91.70 | `mNeighbors[i & 3]` shows the target's unreduced address (98.44%), so the induction must be blocked some other way |
| homebutton | `InitializeHomeButtonInfo` | 98.57 | Moving the body into an inline helper makes the helper level prune and inlines the small callees the target calls |
