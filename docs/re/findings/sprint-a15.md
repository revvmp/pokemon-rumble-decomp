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
