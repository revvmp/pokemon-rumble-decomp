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

## 2026-10-22: CWorldObjStatDisplay source-link acceptance

CWorldObjStatDisplay is now selected as Matching in configure.py. The final changes are confined to that registration, src/game/CWorldObjStatDisplay.cpp, and this appended record. No shared header changes remain, no original hashes changed, and no commits or repository scratch files were created.

### Source changes and emission evidence

- Declare the derived overrides in the order Add, Calc, Draw. Their inherited virtual slots remain unchanged. Define all three out of class with inline, making the compiler emit them from the vtable in reverse declaration order: Draw, Calc, Add. Each method is followed by its requested inline helpers, following sprint-e1 rules 1 and 6.
- Remove the explicitly declared and defined empty derived destructor. The implicit virtual destructor retains member destruction and is generated late, following rule 7.
- Mark the file-local CompareType function inline. Its address is requested by Draw, placing it after the pointer-vector begin/end helpers instead of among the strong definitions.
- Keep the original explicit empty constructor, factory location, allocation expression, includes, and shared headers. Moving the factory, using an implicit constructor, and changing interface-destructor declarations were unnecessary and reverted. No primary-template forward declaration or rule 4a change was needed.

The retained text begins with Create, then Draw and its helpers, Calc and its helpers, and Add. The deferred tail is copy_children at 0x801231F0, get_deleter at 0x801233A0, dispose at 0x801233F0, shared_ptr<CWorldObjStatDisplay> destruction at 0x80123420, move_ptr destruction at 0x801234B0, the by-value comparator sort at 0x80123530, __sort132 at 0x80123920, the reference-comparator sort at 0x80123A60, and the derived destructor at 0x80123E50.

Retained data now has the exact target layout without header reordering:

| Address | Retained object |
| --- | --- |
| 0x803AFAA0 | Display shared_ptr_deleter vtable |
| 0x803AFAB4, 0x803AFB24 | Deleter type name and base descriptor |
| 0x803AFB30, 0x803AFB44 | Vector length-error message and exception type string |
| 0x803AFB80 | CWorldObjStatDisplay vtable |
| 0x803AFBA0, 0x803AFBB8 | Derived type name and base descriptor |
| 0x803AFBD4, 0x803AFBEC | Interface type name and base descriptor |
| 0x803AFC00 | default_delete<CWorldObjStatDisplay> type name |
| 0x804B76F8, 0x804B7700, 0x804B7708 | Deleter, derived-class, and interface RTTI |
| 0x804B7710, 0x804B7711 | Tree insertion boolean constants |
| 0x804B7714, 0x804B7718 | By-value and reference-comparator sort shuffle statics |
| 0x804B7720 | default_delete<CWorldObjStatDisplay> RTTI |

### Validation

python configure.py completed successfully with Matching selected. Ninja stalled without output on both the initial object request and a subsequent single-job DOL request; both were interrupted. The unit was compiled with the exact version and flags read from its generated build.ninja edge. The configured Wii/1.1 linker was then invoked with -fp hardware -nodefaults -lcf build/wpse-v1/ldscript.lcf and a response file generated from the actual main.elf link edge. That response file explicitly selected build/wpse-v1/src/game/CWorldObjStatDisplay.o. main.dol was deleted before linking; the compiler, linker, and dtk elf2dol each exited 0. The existing FORCEACTIVE warnings remained, but the complete output passed exact acceptance.

The requested command returned PASS after the final minimized source changes:

```text
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
```

main.dol SHA-256: ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da.

Live unitdiff reports 48/48 functions at 100%, covering 13,396/13,396 instruction bytes. objdiff-cli report generate refreshed build/wpse-v1/report.json; cnt100.py reports 12,910 before and after, with no per-unit count changes. No shared header changes remain. Full-DOL identity also validates the currently Matching units. git diff --check passed.

### Traps

- An object-order comparison by symbol name can suggest a remaining mismatch even after exact source-link acceptance. The target symbol at 0x80124880 is currently named __dt__20IWorldObjStatDisplayFv, whereas the accepted linked ELF names the retained 64-byte function as the display shared_ptr_deleter destructor. The earlier source copy of the interface destructor is discarded. All DOL bytes, including the relevant vtable pointers, match. Treat the target name as questionable; do not reorder the interface destructor solely to satisfy seq.py. No symbol rename was needed or retained in this session.
- Raw object data contains duplicate weak vtables and RTTI that are discarded at link. Compare retained linked addresses and the frozen DOL before treating those extra object entries or compiler-generated numeric label changes as a data-order failure.

## 2026-10-22: CMsgBalloon emission investigation (not accepted)

CMsgBalloon remains NonMatching. No source, header, symbol, split, or configure.py changes are retained from this investigation. Only this record is appended; no commits were made and no repository scratch files were created. All experiments were reverted before the final successful frozen-manifest verification.

### Current-checkout source-link result

The fresh baseline still reports 436/436 functions at 100% in objdiff report generation, with 166 target names misplaced in the filtered object-order comparison. Its first 218 target names are in the correct relative order. However, the assumption that the current source-selected link differs only in ordering is no longer true: the current map header emits two extra forwarding functions in this dont_inline translation unit.

At native address 0x800C95B0 the target has tree<int,bool>::find_or_insert<int>. The source-selected link instead retains map_base<int,bool>::find_or_insert at 0x800C95B0 and its generic find_or_insert_impl<Tree> at 0x800C95C0, shifting the intended function to 0x800C95D0. Each extra wrapper has four instruction bytes and occupies sixteen aligned bytes. The call chain is map_base::operator[] -> find_or_insert -> find_or_insert_impl<Tree> -> tree::find_or_insert. See include/map around its operator[] and deferred_tree_type overloads. These wrappers are additional to the 436 named target functions, so a 436/436 report does not detect their retained footprint.

Validation procedure and result:

- Temporarily selected Object(Matching, "game/CMsgBalloon.cpp") and ran python configure.py successfully.
- Deleted build/wpse-v1/main.dol before building. The requested Ninja run produced no output and was interrupted; no successful Ninja exit is claimed.
- Compiled CMsgBalloon with its actual mwcc_sjis edge: the configured sjiswrap, Wii/1.1 compiler, and exact generated cflags. The compile exited 0.
- Invoked the configured Wii/1.1 linker with the generated main.elf edge's actual inputs, its -fp hardware -nodefaults flags, and build/wpse-v1/ldscript.lcf. The response file explicitly selected src/game/CMsgBalloon.o. Linker and dtk elf2dol both exited 0, with the existing FORCEACTIVE warnings.
- The requested frozen-manifest verify command returned FAIL, first differing byte 0x1F. Source-selected main.dol was 4,094,912 bytes versus 4,094,880 reference bytes. Its .text size was 0x3004E0 versus 0x3004C0. Failed output SHA-256: d3181368476f97312c0d9930214977d6405a7fc1871f3dd82cc640fd7263070c.
- Restored NonMatching, reran configure.py, deleted main.dol, and repeated the configured linker/elf2dol steps with obj/game/CMsgBalloon.o selected. Frozen verification returned PASS. Accepted SHA-256 remains ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da.
- Refreshed report.json. cnt100.py is 12,941 before and after, with no per-unit count changes. No shared-header candidate passed a complete consumer gate, so none is retained.

### Source-local controls

Moving the existing dont_inline directive before the includes, moving the CMsgBalloon header after the other includes, and forward-declaring Entry inside CMsgBalloon while defining it immediately after the class each left the 166-name ordering discrepancy unchanged. These controls were restored byte-for-byte.

### New vector ownership evidence and rejected diagnostic

The apparent interleaving of __vec_deleter and __vec_constructor may reflect provisional owner names rather than an undocumented compiler scheduling rule. The archived original 2006 MSL vector header places append, append_realloc, growth, sub_push_back, and construct_at_end in __vec_deleter. Its untransformed __vec_constructor is a thin constructor/swap layer. In contrast, this repository puts many of those algorithms in __vec_constructor. This extends the already documented storage-owner corrections in F-152-9; it is not yet proof that every current constructor-owned symbol should be renamed.

Reference: C:/MassStorage/GitHub/198-reference/primary-header-research/alpha2006/CW_RVL/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include/vector, SHA-256 ed11b3e9b051ed59c78e57f853e985012814be2100f676ee6279aca5a508f3ee. Relevant ranges are the deleter declaration around lines 330-650 and the constructor layers around lines 2030-2110. The reference was read, not copied into the repository.

A temporary source/header diagnostic moved the reconstructed append/growth/construction families to the storage base, preserved their bodies, and restored the following declaration/definition contexts:

- alloc/cap and max_size before the destructor; append before erase_tail; can_append/grow_by and count construction after it.
- All construct_at_end overload declarations in one late group, so the first overload does not accidentally anchor the name earlier.
- Ordinary out-of-class recommend and append_realloc definitions, plus ordinary out-of-class split-buffer construction definitions.
- Tree root accessor before the destructor and find_or_insert declaration before find; string operator+= declarations before append; alloc_ptr allocator accessor after release.

For order diagnosis only, the target-name view mapped the moved constructor-owned methods to their new deleter-owned names. No config symbols or reference objects were changed. The ownership move alone reduced the normalized order discrepancy to 122, restoring the previously unexplained first CPiiTex/BUILayout interleaving. The vector declaration/definition changes reduced it to 68; the additional tree/string/alloc_ptr declaration changes reduced it to 46. These numbers describe normalized name-order experiments, not binary acceptance, not a live cnt100 improvement, and not a completed reconstruction. Remaining clusters include allocator specialization order, shared-pointer member templates relative to FormatMessage, and later split-buffer/deleter helpers. Complete symbol provenance, shared-consumer instruction checks, retained link bindings, and frozen verification are all still required before adopting this direction.

Reproduction helpers and rejected candidate snapshots are outside the repository under the session temporary directory: cx_balloon.py, cx_move_vector.py, cx_vector_outline.py, cx_normal.py, and cx-balloon-baseline/candidate46. The final working tree contains none of the candidate code. Start future work from current source rather than copying those snapshots over newer headers.

### Traps and next work

- Use the actual sjiswrap build edge when compiling CMsgBalloon. Calling mwcceppc directly with its -enc SJIS flags on the UTF-8 source lowers Init and the report count even without a source change. The direct call was corrected; the proper wrapper restores 436/436 and 12,941 globally.
- Filtered target-name order and fuzzy function counts omit additional emitted wrappers. Inspect the source-selected ELF and DOL section sizes before assuming that only ordering remains.
- Do not try to force the apparent vector interleaving solely with primary-template forward declarations until the algorithm owner names have been checked against the original storage/constructor split.
- Next steps are to resolve the map forwarding wrappers without regressing their deferred-tree consumers, then prove or reject the vector owner corrections across current units. The 46-name diagnostic is not safe to retain as a partial fix.


## 2026-10-22: ef_animcurve and CRemoteMemory near-miss investigation

Both requested units remain NonMatching. This session did not finish either unit and made no Matching registration changes. Work stayed in the current checkout, without commits, assembly, added code comments, target-manifest edits, or scratch source files in the repository. Build objects, dependency files, the linker response file, and reports are ordinary configured build outputs.

### Retained source changes

- In both baked curve executors, declare one `u32 loop` before `count` and assign it in the integer and fitted paths. Each live function improves from 99.407585% to 99.739334%, retaining its 844-byte size. This removes ten register-operand differences; eleven remain, principally frame length in r4 instead of r8 and the fitted lifetime temporary in r6 instead of r4. No other ef_animcurve function improves or regresses. The unit remains 1/8 perfect functions.
- Compare the masked CRemoteMemory state as the existing signed `Flag` enum. The two `cmplwi` instructions in Update become the target's `cmpwi`; Update improves from 98.89908% to 100%. No new volatile variables were introduced. The existing volatile temporaries were retained because the target Update actually contains their corresponding stack stores and loads.
- GetPiiBox now calls the evidenced `CreateScriptEnvironment`, instead of `CreatePpuEnvironment`. Its target relocation at function offset 0x12C names the former. It assigns the remote list through an iterator range. The new `list::assign(const_iterator, const_iterator)` overload advances at most the destination size and delegates to the existing `__assign_range`, preserving existing copy-assignment behavior. Live GetPiiBox improves from 45.31% to 65.14%. This overload reconstructs the same-list-iterator interface used here; it does not claim a complete generic input-iterator assign implementation.
- RemoteDataBlock has an inline IsValid method with an early magic rejection followed by CRC equality. FinishRead tests unequal generations first, preserving the previous block-selection semantics while following target branch order, and uses an unsigned active-block index. Live FinishRead improves from 49.80% to 56.07%.
- PrepareWrite tests modes 0 and 1 explicitly before selecting the 536-byte payload transfer; all other modes use the checksum transfer. The target has both comparisons, so the previous `mode > 1` formulation was wrong for negative modes. Live matching changes from 77.40% to 77.19%; the count of perfect functions is unaffected. Register allocation and the WriteBlock switch remain unresolved.

### Investigated but not retained

- Widening or making the baked frame count const, reading frameLength directly, moving count declarations, hoisting flags, and rewriting period uses did not finish the baked functions. Extracting the whole baked-time helper also changed the inlining of U8/F32 and regressed those functions; it was reverted. An inline frame-length accessor did not change the remaining allocation.
- Texture parameter-copy permutations and replacing its pointer setup with SplitCurveResource left its 99.61126% score unchanged; those edits were removed. U8, F32, Rotate, Texture, and Child retain their original scores and source.
- Duplicating the WriteBlock -1/default bodies, splitting flag assignments, moving error handling after the switch, and casting switch selectors did not reproduce the target's explicit -1 tree. WriteBlock and FinishWrite retain their initial source. Extracting block selection into another helper made FinishRead's frame larger; that helper was removed.
- A one-pass list assignment implementation worsened GetPiiBox. The retained range overload uses the already-existing assignment helper and was checked by recompiling the configured source set. Large target iterator frames and materialized boolean comparisons remain unresolved; the function is not close to exact source-link acceptance.

### Validation procedure and traps

The initial report counted 12,980 functions with `fuzzy_match_percent == 100`. Unit investigation used `tools/re/unitdiff.py <unit> --function <exact name>` and the JSON/unit summaries. Ninja stalled after an edit and was interrupted, so subsequent investigation invoked the generated compiler command directly and used `unitdiff --no-build`. `python configure.py` succeeded.

An initial full compilation bypassed sjiswrap. After restoring it for the six non-ASCII C++ files, the count was 12,981, but a fresh link failed frozen verification at DOL header byte 0x36 with 192 extra data bytes. This was not accepted. Comparing corresponding data blocks traced the growth to localized SDK DVD strings: `dvdFatal.c` and `dvdDeviceError.c` also contain non-ASCII source. All 865 configured compilation commands were therefore rerun with their exact sjiswrap and post-processing commands, not just the C++ subset.

Verified lesson: a direct-compiler fallback is safe for an ASCII investigation source but must not silently replace sjiswrap across the build. Check C as well as C++ sources; successful compilation and the perfect-function count alone do not establish correct string bytes. No frozen hash was changed.


### Final acceptance and remaining work

All 865 exact configured source compilation commands completed with zero compiler failures. After deleting main.dol, the configured linker and elf2dol both exited 0. The link retained 29 existing FORCEACTIVE warnings. The following command returned PASS:

```text
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
```

Final main.dol SHA-256: `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The refreshed report contains 12,981 perfect functions, up from 12,980. The three temporary encoding-related losses (CDlgMsgWin::OpenYesNo, CMsgBalloon::Init, and CSaveManager::Work) are restored to 100%. Report generation retains its existing exception-table decoding warnings for ScriptLayout::method_layoutSetString and NPpu::ToUnicode.

| Target function | Final live unitdiff | Final report fuzzy match |
| --- | ---: | ---: |
| AnimCurveExecuteU8 | 93.206955% | 93.206955% |
| AnimCurveExecuteF32 | 94.47577% | 94.47577% |
| AnimCurveExecuteRotate | 93.434135% | 93.434135% |
| AnimCurveExecuteTexture | 99.61126% | 99.61126% |
| AnimCurveExecuteChild | 96.31% | 96.31834% |
| AnimCurveExecuteBakedU8 | 99.739334% | 99.739334% |
| AnimCurveExecuteBakedF32 | 99.739334% | 99.739334% |
| CRemoteMemory::Update | 100% | 100% |
| CRemoteMemory::GetPiiBox | 65.14% | 65.135086% |
| CRemoteMemory::FinishRead | 56.07% | 56.64939% |
| CRemoteMemory::PrepareWrite | 77.19% | 78.48855% |
| CRemoteMemory::FinishWrite | 44.60% | 46.19608% |

The signed-state Update baseline was displayed as 98.90%; the earlier extra decimal digits in this session entry are not a separately captured baseline measurement.

CRemoteMemory is now 14/20 live perfect functions; ef_animcurve remains 1/8. Full-DOL PASS validates the current selection of Matching objects and original fallbacks. It does not validate either requested unit linked from source. No promotion was attempted because neither passed the all-functions gate. Remaining work includes ef_animcurve allocation/inlining, CRemoteMemory's state-mask helper and switch structure, list iterator/exception frame fidelity, and data/function emission order. configure.py and config/targets remain unchanged.
