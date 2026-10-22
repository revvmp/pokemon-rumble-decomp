# Sort-unit sprint

## 2026-10-22: sort users and source-selection gates

Work stayed in the current checkout. No commits, target-manifest changes, assembly, code comments, or repository scratch files were added. Only this findings document records the session.

The requested Matching flips remain incomplete. All five units remain NonMatching. The final frozen-manifest verification passes using those original-object fallbacks; this is not source-only acceptance for these units.

### Retained changes

- Named the four pointer-sort shuffle objects at `0x804B6A68`, `0x804B6A6C`, `0x804B7714`, and `0x804B7718` with the exact emitted `@LOCAL@sort<...>@shuffle@2` names. Each new name was checked for an existing symbol definition before replacement. Both pointer sorts in each of CMiniMap and CWorldObjStatDisplay now score 100% in live unitdiff.
- Corrected the display's accessors at `0x801222D0` and `0x801223B0` to const `set::begin()` and `set::end()` (`CFv`), instead of duplicate local definitions of their non-const names. Calc now borrows a const DrawLayerSet pointer, while Add retains its non-const access. All 48 target functions, totaling 13,396 instruction bytes, score 100% in live unitdiff.
- Named display length-error strings at `0x803AFB30` and `0x803AFB44` as emitted `@20530` and `@20531`, and tree insertion bytes at `0x804B7710` and `0x804B7711` as `@8445` and `@8446`. These were the remaining live relocation-name mismatches in reserve, push_back, and insert. Compiler-generated numeric names depend on the source context.
- Narrowed the display's data split from `0x803AFAA0`?`0x803B3030` to `0x803AFAA0`?`0x803AFC38`, and its sdata split from `0x804B76F8`?`0x804B77D8` to `0x804B76F8`?`0x804B7728`. The last display data string is its default-delete RTTI name at `0x803AFC00`; `0x803AFC38` begins `item_bakuhatu_star`. At `0x804B7728`, the motor-pattern globals begin. These neighboring objects now remain in automatic fallback splits. No bytes or original hashes changed.
- Reworked CMiniMap::CalcView using a subtraction temporary for the camera line, separate plane-normal and distance assignments, a named layout Size, and an object-position copy followed by scalar x/z subtraction. Matrix/local declaration order follows the investigated stack ordering. The function retains the target's 2,464-byte size. Live unitdiff improved from 87.12% to 93.59%; report fuzzy matching improved from 87.28247% to 93.74675%. Stack placement and instruction scheduling still differ.
- Restored `mProperties.Clear()` in PiiPropData::Reset immediately after the species-vector swap. The original calls `Clear__11MapPiiTableFv` at function offset `0x424`; the initial source omitted it. Live unitdiff improved from 94.16% to 96.19%, and report matching from 94.20318% to 96.234924%. Remaining constructor/reset differences include signed-short vector copying.

### Matching trial and validation

CWorldObjStatDisplay was temporarily selected as Matching only after live unitdiff reached 48/48 functions at 100%. main.dol was deleted before linking. The initial link failed because the oversized data split removed effect-table strings and motor-pattern globals. After narrowing the split, the configured linker and elf2dol commands succeeded, but frozen verification failed at file offset `0xD16A7` (address `0x801213E7`). The linked DOL had the correct total size, but function emission order and retained data order differed. Source ordering and implicit-destructor experiments did not produce acceptance and were reverted. The Matching registration was reverted; configure.py has no retained change.

`python configure.py` succeeded. Ninja stalled without output and was interrupted; compilation, linking, and elf2dol used the configured Ninja commands directly. The link response file was generated from the actual configured link edge. All five source units were recompiled after reverting shared-header experiments. The final linker returned 0 after deleting main.dol. The exact command below returned PASS:

```text
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
```

`objdiff-cli report generate -o build/wpse-v1/report.json` refreshed the report. Its count of `fuzzy_match_percent == 100` increased from 12,870 to 12,872. Comparing the (unit, function-name) sets found zero lost perfect functions; the two gains are the corrected const set accessors. Automatic data splits increased the unit count from 1,472 to 1,474, without claiming additional reconstructed source.

### Traps and remaining work

- A unit can have every function at 100% and still contain unrelated data in its split. Undefined effect-string symbols exposed this display boundary error. Exact full-DOL verification remains mandatory after correcting boundaries.
- The pointer-vector equality change does not generalize blindly to the primary vector iterator. Testing a by-value friend equality there regressed existing LevelMove sorts and other functions; the shared header was restored. Rewriting the POD const-range constructor to recompute its distance also worsened PiiPropData and was reverted.
- Applying dont_inline around the display methods regressed its factory and Add; that experiment was reverted. Reordering definitions and making destructors implicit did not resolve the full-link mismatch and were also reverted.
- A high fuzzy score can conceal a missing operation: Reset lacked MapPiiTable::Clear. Review inserted/deleted instructions as well as register and relocation differences.
- CGameResult and CPiiListFilterSort were inspected and rebuilt but not modified. Their reference-comparator shared_ptr sorts still score 97.781815%; CPiiListFilterSort also retains a 77.8% bound callback and destructor-layout differences. They are not ready for Matching.
- Next steps: recover the display's natural template/function/data emission order, finish CalcView stack/scheduling recovery, and resolve PiiPropData's vector copy/assignment and short-sort differences. Do not promote a unit on fuzzy scoring alone.

### Final live unitdiff inventory below 100%

These are live `tools/re/unitdiff.py` results against the freshly compiled objects. The report's fuzzy score can differ because live diffs also flag relocation-name differences. Gap/alignment entries are excluded. Every non-perfect target function is listed by its exact name, usable with `--function`.

#### game/CMiniMap

17/20 functions at 100%.

| Function | Live unitdiff |
| --- | ---: |
| `Create__8IMiniMapFP7MapDescP6BWorldfb` | 99.83395% |
| `CalcView__8CMiniMapFv` | 93.59253% |
| `get_deleter__Q43std3tr16detail70shared_ptr_deleter<8CMiniMap,Q33std6detail25default_delete<8CMiniMap>>CFRCQ23std9type_info` | 99.73684% |

#### game/CWorldObjStatDisplay

48/48 functions at 100%.


#### game/PiiPropData

43/48 functions at 100%.

| Function | Live unitdiff |
| --- | ---: |
| `__ct__11PiiPropDataFv` | 95.96947% |
| `Reset__11PiiPropDataFv` | 96.18730% |
| `sort<Q33std30vector<s,Q23std12allocator<s>>8iterator,RQ23std7less<s>>__3stdFQ33std30vector<s,Q23std12allocator<s>>8iteratorQ33std30vector<s,Q23std12allocator<s>>8iteratorRQ23std7less<s>_v` | 96.55556% |
| `push_back__Q23std30vector<s,Q23std12allocator<s>>FRCs` | 99.32639% |
| `__as__Q211PiiPropData12SpeciesMovesFRCQ211PiiPropData12SpeciesMoves` | 97.91389% |

#### game/CGameResult

66/75 functions at 100%.

| Function | Live unitdiff |
| --- | ---: |
| `SetMaterialTexture__25@unnamed@CGameResult_cpp@FPQ34nw4r3g3d6ScnMdlPCcPCcRC9_GXTexObj` | 99.94737% |
| `GetStrongestBP__Fv` | 99.97248% |
| `Create__11IGameResultFPC7MapDescb` | 98.43167% |
| `SetPiiPositions__Q225@unnamed@CGameResult_cpp@11CGameResultFii` | 99.69697% |
| `sort<Q33std112vector<Q33std3tr130shared_ptr<16CPiiPersonalData>,Q23std53allocator<Q33std3tr130shared_ptr<16CPiiPersonalData>>>8iterator,RPFRCQ33std3tr130shared_ptr<16CPiiPersonalData>RCQ33std3tr130shared_ptr<16CPiiPersonalData>_b>__3stdFQ33std112vector<Q33std3tr130shared_ptr<16CPiiPersonalData>,Q23std53allocator<Q33std3tr130shared_ptr<16CPiiPersonalData>>>8iteratorQ33std112vector<Q33std3tr130shared_ptr<16CPiiPersonalData>,Q23std53allocator<Q33std3tr130shared_ptr<16CPiiPersonalData>>>8iteratorRPFRCQ33std3tr130shared_ptr<16CPiiPersonalData>RCQ33std3tr130shared_ptr<16CPiiPersonalData>_b_v` | 97.78181% |
| `RunAction__Q225@unnamed@CGameResult_cpp@11CGameResultFv` | 97.02257% |
| `shared_from_this__Q33std3tr133enable_shared_from_this<7CEffect>Fv` | 99.80769% |
| `Calc__Q225@unnamed@CGameResult_cpp@11CGameResultFv` | 95.87342% |
| `insert_node_at__Q33std6detail143map_compact_tree<Q23std13pair<CPCv,Ul>,Q33std6detail47map_base_value_compare<PCv,Ul,Q23std9less<PCv>>,Q23std32allocator<Q23std13pair<CPCv,Ul>>>FPQ33std6detail14tree_node_basebbRCQ23std13pair<CPCv,Ul>` | 99.82609% |

#### game/CPiiListFilterSort

49/71 functions at 100%.

| Function | Live unitdiff |
| --- | ---: |
| `CompareFavoriteFirst__FRCQ33std3tr128shared_ptr<14CUIPiiListItem>RCQ33std3tr128shared_ptr<14CUIPiiListItem>` | 99.94118% |
| `CompareBPDescending__FRCQ33std3tr128shared_ptr<14CUIPiiListItem>RCQ33std3tr128shared_ptr<14CUIPiiListItem>` | 99.62264% |
| `CompareBPAscending__FRCQ33std3tr128shared_ptr<14CUIPiiListItem>RCQ33std3tr128shared_ptr<14CUIPiiListItem>` | 99.59184% |
| `CompareSpeciesName__FRCQ33std3tr128shared_ptr<14CUIPiiListItem>RCQ33std3tr128shared_ptr<14CUIPiiListItem>` | 99.30851% |
| `CompareSpeciesOrder__FRCQ33std3tr128shared_ptr<14CUIPiiListItem>RCQ33std3tr128shared_ptr<14CUIPiiListItem>` | 96.26865% |
| `Sort__18CPiiListFilterSortFRQ23std108vector<Q33std3tr128shared_ptr<14CUIPiiListItem>,Q23std51allocator<Q33std3tr128shared_ptr<14CUIPiiListItem>>>i` | 96.13291% |
| `sort<Q33std108vector<Q33std3tr128shared_ptr<14CUIPiiListItem>,Q23std51allocator<Q33std3tr128shared_ptr<14CUIPiiListItem>>>8iterator,RPFRCQ33std3tr128shared_ptr<14CUIPiiListItem>RCQ33std3tr128shared_ptr<14CUIPiiListItem>_b>__3stdFQ33std108vector<Q33std3tr128shared_ptr<14CUIPiiListItem>,Q23std51allocator<Q33std3tr128shared_ptr<14CUIPiiListItem>>>8iteratorQ33std108vector<Q33std3tr128shared_ptr<14CUIPiiListItem>,Q23std51allocator<Q33std3tr128shared_ptr<14CUIPiiListItem>>>8iteratorRPFRCQ33std3tr128shared_ptr<14CUIPiiListItem>RCQ33std3tr128shared_ptr<14CUIPiiListItem>_b_v` | 97.78181% |
| `HasMixedOriginalTrainers__FRCQ23std108vector<Q33std3tr128shared_ptr<14CUIPiiListItem>,Q23std51allocator<Q33std3tr128shared_ptr<14CUIPiiListItem>>>` | 98.33334% |
| `ShowTypeFilterMenu__18CPiiListFilterSortFbQ33std3tr124shared_ptr<10CUIManager>iRQ23std108vector<Q33std3tr128shared_ptr<14CUIPiiListItem>,Q23std51allocator<Q33std3tr128shared_ptr<14CUIPiiListItem>>>RCQ33std3tr135function<FPC18CPiiListFilterSort_v>` | 96.51923% |
| `PreviewTypeFilter__18CPiiListFilterSortFP3BUIiRCQ33std3tr135function<FPC18CPiiListFilterSort_v>` | 99.88506% |
| `ShowPrefixFilterMenu__18CPiiListFilterSortFQ33std3tr124shared_ptr<10CUIManager>iRQ23std108vector<Q33std3tr128shared_ptr<14CUIPiiListItem>,Q23std51allocator<Q33std3tr128shared_ptr<14CUIPiiListItem>>>RCQ33std3tr135function<FPC18CPiiListFilterSort_v>` | 98.69768% |
| `sort<Q33std52vector<10UIMenuItem,Q23std23allocator<10UIMenuItem>>8iterator,PFRCPCwRCPCw_b>__3stdFQ33std52vector<10UIMenuItem,Q23std23allocator<10UIMenuItem>>8iteratorQ33std52vector<10UIMenuItem,Q23std23allocator<10UIMenuItem>>8iteratorPFRCPCwRCPCw_b_v` | 99.37283% |
| `sort<Q33std52vector<10UIMenuItem,Q23std23allocator<10UIMenuItem>>8iterator,RPFRCPCwRCPCw_b>__3stdFQ33std52vector<10UIMenuItem,Q23std23allocator<10UIMenuItem>>8iteratorQ33std52vector<10UIMenuItem,Q23std23allocator<10UIMenuItem>>8iteratorRPFRCPCwRCPCw_b_v` | 99.37283% |
| `PreviewPrefixFilter__18CPiiListFilterSortFP3BUIRCQ33std3tr135function<FPC18CPiiListFilterSort_v>` | 99.88506% |
| `ShowOriginalTrainerFilterMenu__18CPiiListFilterSortFQ33std3tr124shared_ptr<10CUIManager>iRCQ33std3tr135function<FPC18CPiiListFilterSort_v>` | 99.93295% |
| `PreviewOriginalTrainerFilter__18CPiiListFilterSortFP3BUIRCQ33std3tr135function<FPC18CPiiListFilterSort_v>` | 99.88506% |
| `ShowSortTypeMenu__18CPiiListFilterSortFQ33std3tr124shared_ptr<10CUIManager>iRCQ33std3tr135function<FPC18CPiiListFilterSort_v>` | 99.93694% |
| `PreviewSortType__18CPiiListFilterSortFP3BUIRCQ33std3tr135function<FPC18CPiiListFilterSort_v>` | 99.88506% |
| `__cl__Q33std3tr135function<FPC18CPiiListFilterSort_v>CFPC18CPiiListFilterSort` | 99.67742% |
| `__cl__Q43std3tr16detail391function_imp<PFP3BUI_v,Q43std3tr16detail343bound_func<v,Q43std3tr16detail90mem_fn_3<v,18CPiiListFilterSort,P3BUI,i,RCQ33std3tr135function<FPC18CPiiListFilterSort_v>>,Q33std3tr1206tuple<P18CPiiListFilterSort,Q53std3tr112placeholders6detail5ph<1>,i,Q33std3tr135function<FPC18CPiiListFilterSort_v>,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat>>,0,1>FP3BUI` | 77.80000% |
| `__dt__Q43std3tr16detail402function_imp<PFP3BUI_v,Q43std3tr16detail354bound_func<v,Q43std3tr16detail88mem_fn_2<v,18CPiiListFilterSort,P3BUI,RCQ33std3tr135function<FPC18CPiiListFilterSort_v>>,Q33std3tr1219tuple<P18CPiiListFilterSort,Q53std3tr112placeholders6detail5ph<1>,Q33std3tr135function<FPC18CPiiListFilterSort_v>,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat>>,0,1>Fv` | 99.93182% |
| `__dt__Q43std3tr16detail391function_imp<PFP3BUI_v,Q43std3tr16detail343bound_func<v,Q43std3tr16detail90mem_fn_3<v,18CPiiListFilterSort,P3BUI,i,RCQ33std3tr135function<FPC18CPiiListFilterSort_v>>,Q33std3tr1206tuple<P18CPiiListFilterSort,Q53std3tr112placeholders6detail5ph<1>,i,Q33std3tr135function<FPC18CPiiListFilterSort_v>,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat,Q33std3tr13nat>>,0,1>Fv` | 99.93182% |

Final verified main.dol SHA-256: `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.
