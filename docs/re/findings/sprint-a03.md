# Sprint a03: CDividedGround and CEffect

Issues 61-65 (`game/CDividedGround.cpp`) and 113-114 (`game/CEffect.cpp`).

## State

- `CDividedGround.cpp`: every function of the unit is reconstructed. All but `LoadAttr`, the `list::insert<iterator>` instantiation and cosmetic relocation-name differences (`LoadPkc`, `BuildScene`) are exact. The unit stays `NonMatching`.
- `CEffect.cpp`: `FreeHeap` and `GarbageCollection` became exact. `InitializeEffect`, `AllocEffectGroupID`, `FreeEffectGroupID`, `CalcMtx` and `CreateEmitter` remain below 100%.

## Traps

- **Search loops that fall through to a fallback need a `goto`.** `for (...) { if (found) break; } if (it == end) {...}` re-evaluates `!=` through an out-of-line call and adds a stack temporary. The target jumps from the loop body straight past the fallback; `goto found` reproduces `Load` (100%) and `BuildScene`.
- **Empty-class tag temporaries decide stack slots in string code.** `random_access_iterator_tag()` passed by value creates two one-byte slots; their order depends on whether the tag is a named local. A named local in `replace(pos, count, const string&)`, `replace(..., ForwardIterator)` and a separately named `replace_tagged` for `append(const char*)` reproduced `LoadPkc`, `LoadAttr`, `BWorldLightSet::CreateMotion` together. `operator=` and `assign(const string&)` need different bodies: the out-of-line `assign` (symbol `assign__...`) uses `replace(0, n, p, p + size, forward_iterator_tag())`, the inlined `operator=` uses the tagged `replace`.
- **`OnLeaveWorld` takes the world.** The target passes `r4` through to `BModelObj::OnLeaveWorld`; `BWorld` calls it with `this`. All overrides were changed to `OnLeaveWorld(BWorld*)` and their symbols renamed (`...FP6BWorld`).
- **A reference symbol can carry the wrong signature.** `CModel::Load` was imported as `Q36CModel10LoadParams`; the compiler produces `Q26CModel10LoadParams`. The string range constructor needs a third pointer argument (`li r6, 0`) in the caller; modelled as `const Allocator* = 0`.
- **Tiny identical functions cannot be named by fingerprint.** `lwz r3,0(r3); blr` and `blr` exist many times (`get`, `operator*`, `GetModel`). Name them from the call sites of a nearly matching caller instead (`bl` pairs at equal instruction indexes after sequence alignment).
- **`GetModel` has a const and a non-const overload.** `BuildScene` calls the non-const one for `SetPriorityDrawXlu` and `SetMtx` and the const one (reached through a const reference) for `PushBack`.
- **Zero-byte stack stores are value-initialised empty-class temporaries.** `li r0,0; stb r0,N(r31)` comes from a temporary of an empty class without a constructor, such as `Metrowerks::int2type<0>()` or the `Size()` default argument of `alloc_ptr`. The target `list::insert(pos, first, last)` is `insert_dispatch(position, first, last, int2type<0>())`, which builds an `__list_imp<T, Allocator> temp(first, last, alloc())` and splices it: that gave `insert<iterator>` 100% (frame 0x90, flags at 0x8, 0xc and 0x10).
- **Commutative float multiplication order is not controllable from source.** `x * 0.5f` versus `0.5f * x`, and `a * (b / c)` versus `b / c * a`, compile identically; the `fmuls` operand swap in `CalcMtx` and `CreateEmitter` cannot be changed this way.
- **Compiler version and flags do not matter for the remaining CEffect functions.** Wii/1.0 to 1.5 give identical output; `-O4,s`, `-inline deferred`, `-opt nopeephole` and `-opt noschedule` make it worse.
- **Out-of-line `string::assign(const string&)` and inlined `operator=` need different tag locals.** `assign` goes through `replace_tagged(pos, n, const string&)` (named `random_access_iterator_tag` local, tag slot at 8 and copy at 0xc); `operator=` keeps the unnamed temporary through `assign_copy`. Mixing them shifts `Load` and `LoadPkc`.
- **`LoadAttr` first `file && file->IsValid()` check stays at 99.27%.** `!file || !file->IsValid()`, a `bool valid` temporary, a separately declared `file` and a positive `goto loaded` all give the same or worse output; the second check already matches.
- **`AllocEffectGroupID` needs a materialised `false` at stack slot 8.** `const bool&` constructor parameters, `bool()` defaults, an `Allocator` default argument and a local copy of `x` do not reproduce it.

## Traps (w7 shift)

- Most "0% missing" template and deleter functions in the Pii dialog units are naming artifacts: the target has an unnamed copy (fn_/dtor_) of a weak function that the source already compiles under its mangled name. Vtable slots (get_deleter/dispose/dtor) and sort/reset callees can be renamed by pairing target and source vtable slot order; names that already exist in symbols.txt (a weak copy in another unit) must stay placeholders or the link reports multiply-defined symbols.
- Renaming `__vt__` of shared_ptr_deleter/BWorldObj list_deleter/CompressedPairStorage placeholders to the names used by Matching units changed the DOL (verify failed even though the build linked). Keep them as placeholders.
- Flipping CDlgCollection to Matching fails to link: sibling NonMatching objects still reference its sdata RTTI (`lbl_804B6058`) and the local-scope `function_base<PFP3BUI_v>` destructor copy (`_8007BF00`). Dialog units sharing weak data must flip together or the shared symbols need real names first.
- **Explicit versus implicit cdeque iterator copy constructor is budget-sensitive.** Removing the user copy constructor of `__cdeque_buf::iterator` makes `distance`, `copy_construct` and `cdeque_imp` copy inline as in the target, but `insert<void_iterator>` (CPlayerPii, CSaveManager) then inlines the copies where the target calls the out-of-line constructor, and the derived `__cdeque_pointer::iterator` needs its own explicit copy constructor. Net result was one function worse, so the header was left unchanged. The target `insert` exceeds the inline budget while `distance` does not.
- **Argument-before-receiver order.** `mMsgWin->OpenMessage(GetMessageText(...))` hoists the shared_ptr dereference before the call; the target evaluates the text first. Binding the text to a local and then calling reproduced `CDlgCollectionImpl::Run` exactly.

## Tail wave (CLayout, CFileData, NPpuVM, CPiiTex, CPpuEngine)

- **`CaptureTexture` is `COffScreenTexture`.** The `shared_ptr_deleter<COffScreenTexture>` dispose (`0x800F74A0`) and the `move_ptr<COffScreenTexture>` destructor (`0x800F7540`) call the destructor at `0x801C4930` directly with the delete flag, so `delete` of a `COffScreenTexture*` is that class's own destructor, not a derived wrapper's. The class, header and unit were renamed (`17COffScreenTexture`); CPiiTex then matched once its functions followed the target order (InitTexture first, UpdateCamera last) and is `Matching`.
- **A flag forwarded raw needs `int` all the way.** `CPpuEngine::Load(const char*, int, ...)` passes `mode` to `LoadScriptCode` with no bool conversion, and `LoadScriptCode` passes it on to `CPpuCode::Load` and `LoadLocalizedFile(..., int background, ...)`. Both now take `int wait`.
- **A specialised `reallocate` dropped value-initialised tags.** `CPpuEngine.h` carried an `is_POD<MTX34>` specialisation and a hand-written `__vec_deleter<MTX34>::reallocate`; the target uses the generic path, whose `allocate(n, pod_type())` and `move_construct_to_end(..., pod_type())` create two one-byte tag slots. Removing both specialisations matched LoadPlacement with no regressions. The node name is read as `reinterpret_cast<const char*>(node.name + reinterpret_cast<u32>(&node))` (integer add, offset operand first).
- **Data labels of NonMatching units.** `lbl_` constants used by otherwise identical functions were renamed to the build's `@NNNN` names (CLayout, CFileData, NPpuVM, CPpuEngine). report.json already scored these functions 100%; only unitdiff changes.

### Open, with what was tried

| Unit | Function | % | Cause and attempts |
| --- | --- | --- | --- |
| CPpuEngine | SendMessage, `dtor_801021A0` | 82.6, 0 | The `vector<int>(list::iterator, list::iterator)` range constructor in the target keeps 8-byte iterator copies (word plus byte) in memory for `distance` and `construct_at_end`; the extra-level `vector<int>` destructor is that local's. Our list iterator is 4 bytes; not modelled. |
| NPpuVM | `CCode::Load` | 88.1 | Target hoists the header loads (magic, sizes) above the member stores; a `const SHeader&` changes nothing. |
| NPpuVM | `~CEngine` | 95.1 | Inlined `set<T*>::erase`: target ranks the tree pointer above the node. Declaring `pNode` before `pNext` in `tree_imp::erase` and a named find result do nothing. |
| CFileData | `map<CNTHandleWithMutex*, weak_ptr>::insert` | 99.9 | The `left`/`leftmost` slots are swapped. `map::insert` calling `tree_.insert_one` directly (one inline level fewer) gives 100% but breaks COyaInfo and l10n_files (DOL fails); swapping the declarations in `insert_one` loses 2 elsewhere. |
| CFileData | `CWorker::CWorker` | 99.8 | Target stores `BThread::mpHead` before the vtable; making `mpHead` non-volatile regresses eight functions. |
| CLayout | `Attach`, `AttachLayout`, `DetachAttachedLayouts` | 99.2-99.3 | The target `set<T*>` insert and copy paths have two more empty-tag slots and one more iterator copy than the hand-written pointer specialisation in `include/set`. |
| CLayout | `GetPaneAlpha` | 98.2 | Inlined recursive copies carry an extra `b`; early-return shapes compile larger (0-87%). |

## Final tail wave (CLayout, CFileData)

Gained: CLayout `Draw` and `GetPaneAlpha`; CFileData `DefaultFileErrorHandler`, `copy`, `construct_at_front`, `construct_at_end(n)` and `resize`. Also improved: map_base `erase` 91.6 to 96.7%, CLayout `FindAnimation` 93.2 to 98.1%, `__ct__7CLayout` 95.5 to 96.9%, and `CalculateAttachedMtx` 93.6 to 94.9%. CGameResult and CUIMenu each gained one function from the shared `map` changes. Both units stay `NonMatching`.

### Traps

- **Slot order can need an inline helper, not a change to declaration order.** In CLayout `Draw`, the 16-byte locals (`rect`, viewport, scissor) were in reverse order, while the 64-byte `Mtx44` and `MTX34` temporaries were already right. Moving the three small objects into an inline helper (`SetupScreen(Mtx44 projection)`) reversed only those three. The projection matrix stays in the caller.
- **A zero-initialised struct followed by member reassignment.** The target stores four integer zeros into the viewport, reloads `x` and `y` as floats, then stores all four members as floats. This source reproduces it: `Viewport viewport = {0, 0, 0, 0}; viewport.SetSize(w, h)`, where `SetSize` calls `Set(x, y, w, h)`. Convert `w` and `h` into locals first. Otherwise the right-to-left argument order converts the height first.
- **An extra `b` after a then-block in recursive inlines.** `GetPaneAlpha` needs `if (paneAlpha != 255) { return ...; } return alpha;` with the pane alpha held in a `u8` local. Without the local, the early-return form changes the recursive inline depth and the function scores 0%.
- **Debug strings can end in a newline.** The CFileData `OSPanic` messages end in a newline escape (`fatal error: NAND_08` followed by `\n`). A missing newline shifts the later strings in the pool by 4 bytes and moves every `addi rX, base, off` that loads them.
- **`std::swap` of a `compressed_pair` spills a temporary.** `map_compact_tree::swap` now calls the member `swap` on each of its three compressed pairs. `comp_` is built from the anchor alone, with no `Compare()` temporary. This fixed the split-buffer helpers for vector of map, and `copy`, without changing any Matching unit.
- **Copy through the derived tree.** `map_compact_red_black_tree` owns the copy constructor: it default-constructs its base and then calls `this->copy(rOther)`, as `red_black_tree` does. The target keeps `this` in a saved register for the base-destructor cleanup.
- **`erase_next` increments a temporary.** `front() = (++iterator(position)).node_;` gives the target's register use. With a named copy, the optimiser stores the leftmost result directly instead.

### Open, with what was tried

| Unit | Function | % | Cause and attempts |
| --- | --- | --- | --- |
| CFileData | `__as__` for vector of map, plus the missing `copy_range<map*>` and `construct_at_end<map*>` | 99.8 | For this vector only, the target `operator=` passes `T*` iterators, and `copy_range<T*>` loops with `<` one inline level down. Two changes together matched CFileData: passing `data_` (selected on `__vec_trivial_copy`) and adding a `U*` overload to a helper. They broke CDlgPiiSelector, whose `copy_range<list iterator>` gains an inline level, as well as SaveData and CUIMenu. Reverted. |
| CFileData, CLayout | `map::insert` (CFileData), `FindAnimation` | 99.9, 98.1 | The `left` and `leftmost` slots are swapped. An `insert_one` forwarder in `map_compact_tree_adapter` fixes the slot order in both, but changes BWorld `RemoveOldestEffect` (`map<u32, CEffect*>`). The extra level seems to apply only when the key or the value is not POD. Reverted, because the net gain was zero. |
| CFileData | `OpenFileStream` | 97.4 | When `AcquireCNTHandle` returns null, the target destroys the handle on a separate path: `bne` branches to the body and the null case falls through to the destructor. Not reproduced. |
| CFileData | `UpdateResourceFiles` | 99.9 | The `RemoveExpiredFiles` iterator is in r26 in the target and r28 in ours. Declaration order, assignment after declaration and a `for` loop do not fix it. |
| CLayout | `StopMotion`, `ReplaceMotions`, `FindPaneMP`, `AdvancePaneMotions`, `Attach*`, `DetachAttachedLayouts` | 91-99.3 | In the inlined `UpdateAttachedLayouts` and map `find`, the target keeps every iterator copy and the pane key in memory. The standalone `UpdateAttachedLayouts` matches. A user copy constructor on `set<T*>::iterator` and named locals in `set<T*>::begin()` and `end()` change nothing. |
| CLayout | `CalculateAttachedMtx` | 94.9 | The target does not fold `root == root` from the inlined `GetLayoutAlpha`, and it reloads the attach pane's visibility. Ours keeps both in registers. |
| CLayout | `MakeArchiveList(const char* const*)`, `CInitializeException` (array overload) | 94.4, 98.1 | The target loads `*pArchives` after the list is initialised. Pointer, index and `while` loop forms do not move the load. |
| CLayout | `__ct__7CLayout` | 96.9 | `GetDefaultPixelScale()` creates one extra 8-byte `VEC2` temporary. Returning the static directly fixes the slot, but then the copy uses integer loads. |

## Final tail wave (a03, fresh eyes)

CDlgInfo gained `~nw4r::ut::Rect` and the global `~vector<shared_ptr<CLayout>>` copy (70 to 72 of 75). `CDlgInfoItemLine::Update` rose from 98.84% to 99.88%.

### Fixes

- **An empty destructor is emitted for a destroyed temporary, not for a named local.** `GetTextWidth` returns `ceil(pText->GetTextDrawRect().GetWidth())`. The temporary `Rect` makes CodeWarrior emit the weak `~Rect` after `Update`, as in the target, although no code calls it. A named `Rect rect = ...` local is elided. No `{ return; }` body or header change is needed, so CTutorialMsg, CLayout and ut_TextWriterBase are untouched.
- **The duplicate `~vector<shared_ptr<CLayout>>` belongs to CDlgInfo.** CUIList's extab points at the CDlgInfo copy (`0x80080370`), so that copy is the linked global. The CUIList copy (`0x801DC380`) is the unreferenced duplicate and is now the `scope:local` one in `symbols.txt`. CUIList loses one paired function in objdiff (30 of 31), because its source emits the global name. Only a file-local copy could give CUIList its own one.
- **`Update` registers.** The line index is declared before `count` and reused for the trailing `SetPaneRectangle` loop. `totalWidth` is computed before `columns[0] = mRectangles[0]`. The half width is `-totalWidth / 2.0f`.

### SaveData `SlotData` chain, measured

The target's whole call structure can be reproduced, but only with a change to the shared `<vector>` that breaks CStatistic. The results:

- **Three changes together give the target sizes.** With all three, `DeserializeVector<SlotData>` is 608 bytes, `append` 164, `append_realloc` 2832, `can_append` 268, `cap` 8 and `construct_at_end` 1004. All of these equal the target:
  1. `SlotData::Data` with scalar members: `mFlags` as four `s8`, `mLastArea` as two `s8`, and the union replaced by `u32, u32, u16, u8, u8, u32, u16, s8, s8`. The target's `Data` copy (`fn_8016B130`) copies exactly these widths.
  2. About six dummy statements in the `__vec_constructor` copy constructor. These stand in for a real, still unknown cost in the `SlotData` copy level.
  3. `erase_at_end` dispatching on `has_trivial_dtor` instead of `pod_type`.
- **Item 3 is the only one that touches other units, and it breaks the DOL.** It makes the `SD_*` vector destructors cheap enough for `append_realloc`, compiled standalone, to stay under budget. The target inlines everything there: 56 calls. Ours, without item 3, calls 16 helpers. But CStatistic `Set` (`vector<SD_StatisticValue::Data>`) then stops being a rule-3 poison, and the target's out-of-line `erase_tail<int2type<0>>` copies for the five `SD_*` types disappear. Dispatching in `clear()` instead, or giving it a forwarding layer, regresses about 40 units. The cost of the vector destructor path is load-bearing project-wide.
- **The target's `Data` copy is serialised.** It loads and stores one member at a time in `r0` and has no stack frame. Ours loads every trailing member first, uses r23 to r31 and needs `_savegpr`. `#pragma scheduling off` does not change this. Nested member structs are the untested hypothesis.
- **Tools.** Explicitly instantiating `__vec_deleter<SlotData>::append_realloc(unsigned long)` at the end of `SaveData.cpp` shows whether a helper is poisoned when compiled standalone. Remove the instantiation afterwards.

### Unsolved

| Unit | Function | % | Cause and attempts |
| --- | --- | --- | --- |
| CDlgInfo | `Update` | 99.88 | `hasText` is r24 and `hasColumns` r29 in the target; ours are swapped. Tried: declaration order, if/else, ternary, nested `if`, a late declaration of `hasText`, and calling `HasText()` twice (75%). |
| CDlgInfo | `do_replace<wchar_t>` | 97.4 | The target recomputes `last - first` for the `memmove` after `wmemcpy`; ours CSEs it with `distance`, which needs one more saved register. Tried: `static_cast` on either side, and `distance()` in the `memmove`. |
| CDlgInfo | `CDlgInfoItem` ctor | 99.0 | `Page` copy: the slots of the frame tags and saved registers. |
| CDlgCollection | `CDlgCollectionItem` ctor | 94.7 | The target keeps `first`, `last` and the byte difference in registers and recomputes the `memmove` size. Tried: `n * sizeof(T)`, `memmove` first, `size_t` and `size_type` casts, `begin()`/`end()`, `distance`, and an explicit copy constructor on the `unsigned short` specialisation. |
| ScriptWorldObj | `IsKindOf<CPii>` (local copy), `tree_begin_holder<less<int>>` | missing, 8.3 | The target has a file-local `IsKindOf` reached through `DynamicCast` and a global one for the direct `method_is*` calls. ScriptPii uses only `DynamicCast`, and its copies are all local. A user `less() {}` turns empty-class copies into byte copies (COyaInfo 26 to 21). |
| SaveData | 13 functions | | See above. |

### Traps

- **An experiment helper that runs `git checkout` on a header also reverts your earlier uncommitted edits to it.** Re-apply them and check `git diff --stat` before trusting a full-build comparison.

## CRemoteMemory tail (final wave)

GetPiiBox 85.80% to 100% (CRemoteMemory 14 to 15 of 20). BeginRead 96.51% to 97.27%. The unit stays `NonMatching`.

- **GetPiiBox is not a `global_optimizer off` function.** A greedy pass search from full optimization gave `opt_dead_assignments`, `opt_propagation`, `opt_common_subs`, `opt_strength_reduction`, `opt_scalarize` and `opt_lifetimes` off (96.02%). With these passes off, the list iterator `!=` and `==` compare results become values (`subf; subf; or; srwi.` and `subf; cntlzw; srwi.`) through the unchanged `include/list`, so the header does not need a `bool` temporary.
- **One pass set fits all six partly optimized functions.** The union of the GetPiiBox set and the a05 PrepareWrite set (adding `opt_rebuildconditionals`, `opt_treetransformation` and `opt_generateconditionalassignments`) leaves every function at its best score. All six blocks now use it. Adding any further pass changes nothing.
- **The target `list::assign` dispatches twice.** `assign(const_iterator, const_iterator)` calls `choose_assign(first, last, int2type<0>())`, which calls `do_assign(first, last, iterator_category())`. This reproduces the two zero-stored tags and the copied third tag. Only CRemoteMemory calls `list::assign`, and the full build count was unchanged apart from +1.
- **The extra iterator copies come from const access.** `assign` is called on `const CPiiBox& rBox = mRemoteBox` (const `begin()` gives the target's copies). The final loop needs `PersonalDataList::const_iterator it = PersonalDataList::iterator(mPersonalData.begin())`, which makes three begin copies. The flags chain reads `mFlags` once into a local and tests it through `MatchesFlags(flags, const u32& mask, value)` and `(flags >> 4) & 1`.
- **Removing the non-const `CPiiBox::GetPersonalDataList` breaks other units.** ScriptPiiMotion and two other files need it.

### Unsolved

| Unit | Function | % | Cause and attempts |
| --- | --- | --- | --- |
| CRemoteMemory | BeginRead | 97.27 | The composite `SetState` argument: the target `li r0, 0x200; ori r5, r0, 0x28`, and ours folds `0x228`. A named result in `operator|` gives the unfolded form, but then the compare also uses the register (91.80). `changed` locals, casts and a `FlagSet` class with `opt_scalarize` off were tried. The `li r6, 0` before `li r5, 0x440` argument order did not move with any argument spelling. No further pass helps. |
| CRemoteMemory | FinishRead | 97.46 | The same composite `SetState` form, plus register allocation. |
| CRemoteMemory | ThreadMain | 98.62 | Inlined ReadBack holds `mTransferOffset` in `r8` in the target. An offset local (`u16`, `int`) and the operand order (98.67) were tried. |
| CRemoteMemory | PrepareWrite, FinishWrite | 99.47, 98.19 | Register allocation only. A pointer local, unsigned and `u8` index casts were tried. Removing any pass from the set loses. |
