# Sprint a05: CDlgMsgWin and ButtonMessage

## Units

- `game/CDlgMsgWin.cpp` (`.text` `0x80084D40`-`0x80088C18`, `.rodata` `0x80352B00`-`0x80352B18`) and `game/ButtonMessage.cpp` (`.text` `0x80088C20`-`0x800896D8`) are split and registered `NonMatching`.
- `CDlgMsgWin` is a global class, `IDlgMsgWin` plus `enable_shared_from_this`, with every method defined inside the class body. About 190 of 218 emitted functions have matching instruction streams; the rest differ only in symbol names or emission order.
- `ButtonMessage.cpp` holds `GetButtonMessage`, `operator+(wstring, const wchar_t*)`, `FormatMessage<const wchar_t*>`, the `basic_formatter<wchar_t>` constructor and destructor, the player search used by the Pii dialogs, and out-of-line `CPlayer::GetPersonalData` and `GetPendingPersonalData`.

## Traps

- **Methods defined in a class body change string addressing.** Out-of-line definitions of the same code load a shared `.data` string base into a callee-saved register once; in-class definitions load each string with its own `lis`/`addi`, as the target does. Define game methods inside the class when the target does not CSE string addresses.
- **Emission order is reverse declaration order for in-class virtuals.** `dont_inline on` for the whole file is needed for these dialogs; a user-declared destructor is emitted early, an implicit one late as in the target.
- **A unit whose `.data` holds exception type-name strings cannot become Matching alone.** Other split objects reference `lbl_8039DED8` and similar by symbol name; the compiled object defines anonymous `@N` data, so linking fails with `undefined` until every referencing unit is also built from source.
- **Renaming a target symbol to an MSL mangled name creates collisions.** Two dtk objects that both define the same name stop the link (`multiply-defined`); a Matching unit's weak copy is silently dropped in favor of a dtk strong symbol of the same name. Never give two retained copies the same name.
- **Tiny functions are ambiguous.** Matching 4- and 8-byte template helpers by bytes alone assigns names to the wrong copy; use the caller set.
- **`#pragma scheduling off` before a definition fixes inline getters whose stores precede the second load**; it must precede the definition, not follow it.
- **Header edits that look local change other units.** `std::tr1::shared_ptr(Y*, D)` now calls `enable_from_this(p, p)` and `shared_ptr_holder` is a `compressed_pair<T*, D&>` holder; `basic_string::data()` goes through a const `buffer()`; `CLayout::CMotionProgram::IsEnded(bool)` is written without helper calls.

## Issues 336-346 shift

- TextLayout is `Matching`: `WrapText` needed a shared word-start block reached from three sites (one `goto` label inside the `Empty` case), plus `.data` `0x803C2B88`-`0x803C2BE0` and `.sdata2` `0x804BA570`-`0x804BA580` splits.
- CUIMenu now holds `RunUIMenu`, a file-local `RunMenuList`, `NotifyCallbacks`, `OnMenuAccept` and `OnMenuSelection`; 57 of 61 functions match. The bound argument type in the RTTI strings is `const MenuCallback *`, so the callback list struct is named `MenuCallback` and `CUIMenuCallbacks` derives from it. Free-function `bind` overloads and a `tuple<ph<1>, B, D>` `bound_func` were added to `include/functional`.
- errmsg holds the system message functions (`ShowSystemMessage`, `InitializeSystemMessages`, `SetSystemMessageFonts`, `RegisterSystemMessageCallback`, the fatal loop class and `GetFontMap`/`ResizeFrame`); 34 of 39 match.
- Data splits were added for CUIMenu, errmsg and homebutton, and their `lbl_` and `@` symbols renamed with `tools/re/name_data.py`.

## Traps

- **Read vtable words from the DOL to name template clones.** `function_imp` and `shared_ptr_deleter` virtual members are only reached through vtables, so `name_callees.py` cannot name them. The vtable symbol address gives the dtor, clone, clone-into-buffer, call and `get_deleter`/`dispose` addresses in order; the RTTI name string next to it names the instantiation and its bound function type.
- **Type names in RTTI strings give the real parameter types.** `bound_func<void, void (*)(BUI *, bool *, const MenuCallback *), ...>` fixed both the callback signatures and the class name before any code was written.
- **`std::min` argument order and operand temporaries decide stack slots.** Swapping to `std::min(a, b)` with `b < a ? b : a` semantics flipped compares, and passing expressions instead of named locals created the temporaries the target spills.
- **`layouts.swap(mLayouts)` versus `mLayouts.swap(layouts)`** changes load order of the three swapped fields; match the load order of the target.
- **`cntlzw`/`srwi 5` materialising a null test** came from `bool noInput = !pInput; if (noInput) break;` inside a `for (;;)` that re-tests every iteration; `pInput == nullptr` and `pInput != nullptr` fold into a plain compare.
- **A stack-passed `bool` parameter is loaded with `lbz`.** The target loads the last parameter of `RunMenuList` with `lwz` and passes it on without normalising; `int`, `unsigned`, `long`, `const` and `volatile` variants all differ, so the real parameter type is still unknown.
- **Two temporaries of the same type swap slots between sites.** `GXColor` in `DrawHomeButtonNotice` and the `random_access_iterator_tag` in the first `CUIList::Load` string constructor keep the wrong order after every declaration reordering tried.
- **Hoisting a base-position `switch` helps stack layout.** Writing `pTitle->GetBasePositionH()` and `GetBasePositionV()` as `switch` statements moved `CUIMenu::UpdateSelection` from 96% to 99.4%.
- **Several `PaneRectangle` helper variants stay at 88.7%.** Reordering `position`, `size` and the base-position reads in `GetPaneRectangle` changes only instruction scheduling; none reached the target prologue order.

## Long-tail shift (tB)

## Traps

- **A user copy constructor on `default_delete<T[]>` regresses dozens of units.** It fixed one CDlgPiiSelector helper but changed about 40 other units and broke the link; keep the implicit one and leave that helper unmatched.
- **Moving a file-scope string table before its first use changes its addressing.** Defining `gAreaPaneNames` above the CNoticeDisplay classes made CodeWarrior address it from the pool base; keep it at the end of the file.
- **Inline flag setters split into add and remove helpers.** `CPiiPersonalData::SetFlag` and `BWorldObj::SetStateFlag` call separate 16-byte `AddFlag`/`RemoveFlag` helpers in the target; the split in `symbols.txt` must match before callers reach 100%.
- **Placeholder deleter and destructor clones are best named from vtable slots.** Comparing original and rebuilt vtable relocations gives the permutation of `get_deleter`, `dispose` and destructor clones directly.
- **A duplicate weak vtable leaves zeros in the original.** CGameMap's `.data` holds 0xB4 zero bytes where its weak `__vt__8IGameMap` would go, because the linker kept the CGameEnding copy and zero-filled this one. Naming the earlier copy (`__vt__8IGameMap` at `0x803A1D00`) in `symbols.txt` reproduces this; a source-built unit otherwise keeps a full vtable there.
- **`vector<unsigned long>` stores through `void*`.** CGameMap's exception table points at `__vec_split_buffer<void*>`'s destructor, so `__vec_integer` derives from `__vec_constructor<void*, allocator<void*>, true>`. Its `push_back(void* const&)` adds the inline level the length-error slots need, and the vector converts by value. `__vec_constructor<unsigned short, A, true>` gets a forwarding `push_back` for the same reason (CDlgCollection).
- **Instantiation order of `shared_ptr<T>` classes follows include order.** A by-value declaration instantiates the class, and the deferred `~shared_ptr`/`operator=` group comes out in reverse. CGameMap needed the headers in alphabetical order, with `<memory>` and `<string>` first so that the exception vtables precede `ICalc`. `shared_ptr<CDividedGround>` had to be first, so `CDividedGround.h` declares a `static Create(u32)`. That declaration has no evidence of its own.
- **CNoticeDisplay is unresolved.** The target emits `__ct<T>`/`__as<T>` for the four notice classes in the deferred group at the end, with each `__rf<T>` after that class's `Run`. Ours emits them right after `Show`, so the `function_imp` and deleter vtables come before the string pools and the `Init` pool offsets move by 0xE4. Moving or removing `#pragma dont_inline` did not change the order.

## Final tail wave (tY)

ScriptPii reached 100% in every function. Data-label naming (`@NNNN` labels, RTTI, vtables) raised SaveData, CPiiListFilterSort, CDlgPiiSelector, CDlgInfo, CDlgCollection, CGameResult and ScriptWorldObj. Their remaining misses are register, frame-slot or inlining problems.

| Unit | Function | Match | Cause |
| --- | --- | --- | --- |
| ScriptPii | (unit) | all 100% | `NewPlayerPii` extab range entry `(0x6c, 0x0004, 0x78)` against the target's `(0x6c, 0x0000, 0x78)`. The code is identical. Changing the if/else form, a direct-init `data`, the `IsJoined` test form or `throw()` on `CPlayer::GetPersonalData` did not change it. The unit stays NonMatching. |
| ScriptWorld | `SetStageDescTarget` | 99.71% | world/rank r30/r29 swap; see sprint-a17cx |
| ScriptGame | `MakeMapID`, `IncMapClearedNum` | 96.5%, 99.5% | non-volatile order; see sprint-a13 and sprint-a17cx |
| CDlgCollection | `CDlgCollectionItem` ctor | 94.7% | the inlined `vector<u16>` copy CSEs `memmove`'s `(pLast - pFirst) * 2` with the outer count. The target recomputes it from the pointers. |
| CDlgCollection | `Calc` | 99.6% | loop index and `pImpl` registers (sprint-r1 open case) |
| CGameResult | `Calc` | 95.9% | the frame slots of every local are in reverse creation order. A camera helper inline or a block scope did not help. |
| SaveData | `append`/`DeserializeVector<SlotData>` | 0%, 7.6% | the target inlines nothing for `SlotData` (it calls `cap()`, `construct_at_end` and the member vector copy ctors out of line). Ours inlines all of it. |
| ScriptWorldObj | `tree_begin_holder<less<int>>` ctor | 8.3% | the target calls an out-of-line `less<int>` constructor. Adding `less() {}` matches it but loses 6 functions elsewhere (COyaInfo, CMsgBalloon). |
| ScriptWorldObj | `set<int>::insert` | 94.4% | the target passes `li r5, 0`, which suggests MSL's defaulted `restrict_to` parameter on `pair`'s converting constructor. Not tried, because `<utility>` lacks `restrict_to`. |

### Traps

- **`name_callees.py` can name a template clone after the wrong type.** It named ScriptPii's `0x80154450` `DynamicCast<CPii>`, but its `IsKindOf` tests mask `0x100`, which is CEnemyPii. Its `scope:local` also broke the DOL. Check the `IsKindOf` mask before accepting a `DynamicCast` name.
- **`IsKindOf` copies are file-local.** ScriptPii keeps its own `IsKindOf<CPlayerPii>` and `IsKindOf<CEnemyPii>` while ScriptWorldObj has global ones, so the template is `static` in `BWorldObj.h`. Without that, a Matching ScriptPii loses 0xA0 bytes of `.text` to weak dedupe. No other unit changed.
- **The pointer-list `push_back` is a forwarding member.** ScriptPii calls `__list_imp<T*>::push_back`, which tail-branches to the `end()`/`insert` body (named `append`). Making `list::push_back` forward for all element types lost 35 functions elsewhere. Only the pointer specialisation forwards.
- **Inline members defined in the .cpp before first use are emitted ahead of that function's template instances.** Defining `CPlayerPii::Initialize`/`LoadScript` as `inline` above `NewPlayerPii` put them before `NewPlayerPii`'s template instances, as in the target.

## Tail wave (library long tail)

### Fixes

- **puh_msc_blk (now Matching).** `uhf_msc_blk_pread`/`pwrite` clamp the block count with `if (count > max_block) num_block = max_block; else num_block = count;`. Assigning `max_block` first and overriding it gave the `r26`/`r27` swap (99.4%).
- **snd_SeqSound (now Matching).** The target stores both volatile flags before `mTempSpecialHandle`. Clearing them in a comma expression inside the `mTempSpecialHandle` initializer reproduces the order (zeldaret/ss uses the same device and marks it fake).
- **ut_TextWriterBase, 77 to 79 of 81.** `Font::GetCharStrmReader` is an in-class NRV body marked `__attribute__((never_inline))` (as in zeldaret/ss). Every caller in this unit and `lyt_textBox` calls it out of line, but the inliners inline everything else around it. `~CharStrmReader() { return; }` makes CodeWarrior call the destructor in over-budget `PrintToDisplayListImpl`; an empty `{}` body is elided and the weak destructor is never emitted.
- **TMC JPEG setup.** `((h % 4 != 0) + h / 4) * 4` raises both setup functions from 94.75% to 99.83%; one `add` operand order remains (F-764-1).

### Unsolved (unit, function, match, cause)

| Unit | Function | Match | Cause |
| --- | --- | --- | --- |
| ut_TextWriterBase | `PrintToDisplayListImpl<c/w>` | 98.5% | `posX`/`posZ` take `f22`/`f23` swapped; the target loads `posZ` after the `GetBaselinePos` call. Declaration orders tried change the constant registers instead. |
| hbm lyt_window | `DrawFrame` | 92.5% | scheduling in the first quad only; local orders, `Size` copy and split `bool` tried. |
| pdm_mbr | `pdm_mbr_check_master_boot_record` | 89.2% | byte-load scheduling; operand order, casts, dead stores and unrolled loops tried. |
| ef_drawfree / ef_drawdirectional | `Draw` / `DrawDirectional` | 97.5% | see trap below. |
| ef_particlemanager | `Calc` | 94.2% | frame is 0x20 larger and the register assignment differs throughout. |
| ef_animcurve | 7 functions | 93-99.7% | Baked: frame length takes `r4` instead of `r8`; Texture: parameter registers. |
| auto_03_8027C8B8 | `IOStream::GetOffsetAlign` | n/a | weak copy owned by `snd_MemorySoundArchive.cpp` (F-424-2); needs a dead-stripped first function that calls it, for which there is no evidence. |

### Traps

- **The ef "partly unoptimized" functions are not compiler or flag differences.** Every Wii compiler, `-O2`/`-O3`, each `-opt no*` and each `opt_*` pragma give the same chained switch and branch-free flag. Only `global_optimizer off` gives the target's binary switch tree, but it also adds a frame pointer and stops constant folding. `ef_drawbillboardstrategy` inlines the same `GetViewMtxCooked` switch as a chain, so the cause is in the source of the two draw strategies. Function size, local count, struct returns, later-defined inlines and inline asm do not trigger it.
- **`try.py`-style text replacement replaces every occurrence.** PrintImpl and PrintToDisplayListImpl share statements; a whole-file replace silently changed PrintImpl. Replace by index.

## Final tail wave (a05, second shift)

CGameResult `Create` and `RunAction` reached 100% (73 of 75). CDlgInfo gained `Load` and `Show` (70 of 75). SaveData gained `WriteBanner` and `FormatWide<unsigned long>` (87 of 100).

### Fixes

- **Pooled string references resolve to the symbol that covers the offset.** objdiff turns `addi rX, rBase, off` after a pooled `lis`/`addi` base into a reference to whichever target symbol contains `base + off`. An oversized dtk label (`sVectorTypes__8SlotData` sized 0x7C, `lbl_803A2938`, `lbl_803B76D8`, `lbl_803A2E00`) swallows the strings that follow, so every pooled string load scored as a mismatch. Splitting the label into the compiler's `@NNNN` string objects at the same offsets fixed SaveData `WriteBanner`, `FormatWide` and CGameResult `Create` with no code change.
- **CDlgInfo `sColumnPaneNames` is a writable table and the file does not pool data.** The target keeps the table in `.data` (not `.rodata`) and loads each string with its own `lis`/`addi`; `const char* sColumnPaneNames[4]` plus `#pragma pool_data off` matches `Load` and `Show`.
- **A target `.data` order that places a function's strings after template vtables means the function was emitted late.** CGameResult's `RunAction`, `Draw`, `CalcView`, `CalcMtx` and `Calc` come after the `shared_ptr` member-template instances in the target, in reverse declaration order: they are `inline` (vtable-only) members. Marking them `inline` put the `RunAction` strings at pool offset 0x544 as in the target.
- **`GetBP__25@unnamed@CGameResult_cpp@FP16CPiiPersonalData` was `CPiiPersonalData::GetBP() const`.** The target calls `shared_ptr::operator->` and then the 8-byte getter, out of line because `RunAction` is over budget; it is emitted where `FindStrongestPii` first references it.
- **`Lerp<VEC3>` is a shared header template.** CGameResult calls the copy that CDlgPiiSelector emits, so `Lerp(const T&, const T&, f32)` lives in `MathUtil.h` and the two symbols lost their `@unnamed@CDlgPiiSelector_cpp@` qualifier. CDlgPiiSelector is unchanged.
- **`RunAction` details.** `GetUIInput()` is called inside the wait loop (the loop branches back to the call). The first input test is `GetTrig() | (!canRepeat ? 0 : GetRepeat())` written inline, with `canRepeat` as `atEnd ? false : true`. The advance case is `default:`, so case 0 falls through the compare chain. `steps` is declared before `previous`, and `newShown` before `strongestShown`. The `min` limit is a file-scope `const int` in `.sdata2`, not a `.sdata` temporary.
- **CGameResult `Calc` (95.94 to 96.11%).** The target's VEC3 slots fit `UpdateMirrorCamera()` as one inline level that contains two inline helpers (`GetCameraUp`, `GetCameraTarget`). The remaining difference is `&eye` held in r31 across the second `PSMTXMultVec`.

### Unsolved

| Unit | Function | Match | Cause |
| --- | --- | --- | --- |
| SaveData | `append<SlotData>`, `DeserializeVector<SlotData>`, `Load` | 0%, 7.6%, 70% | With 40 dummy statements in the `__vec_constructor` copy constructor the `SlotData` copy level overflows (the Data copy and vector copies become calls, and `construct_at_end` inlines them like the target's `fn_80169B40`), but `append` still stays under budget because its inlined `append_realloc` keeps the split-buffer helpers as calls. Five statements (sprint-i1) were not enough. Removing the status union and splitting `mFlags`/`mLastArea` into scalar members (the target copies them bytewise) change nothing on their own. |
| CDlgInfo | `__dt__nw4r::ut::Rect` | missing | `~Rect() { return; }` emits it at 100%, but costs CTutorialMsg 3, CLayout 1 and ut_TextWriterBase 2 functions and breaks the DOL. Reverted. |
| CGameResult, CPiiListFilterSort | `sort<..., Compare&>` (shared_ptr by reference) | 97.78% | `first`/`m` versus `j` register pairing; the by-value instantiation is 100%. |
| ScriptGame | `IncMapClearedNum` | 99.49% | record address and value swap r4/r5; a reference local, a pointer local, a `SetClearCount` helper and four body forms did not change it. |
| ScriptWorldObj | `tree_begin_holder<less<int>>` ctor | 8.3% | `: Compare()` in the constructor does not emit a call. CMsgBalloon's `map_value_compare` constructor is a separate 4-byte `blr`, so a user `less()` would conflict there. |

### Traps

- **Python's default text mode rewrites CRLF files as LF.** A replace-and-restore helper that reads without `newline=''` leaves whole-file line-ending changes; check `git status` after experiments.

## Final tail wave: CLayout and CFileData (fresh eyes)

Gained: CFileData `map<CNTHandleWithMutex*, weak_ptr>::insert` (100%), `OpenFileData` (unitdiff call name), `OpenFileStream` 97.4 to 99.89%. CLayout `StopMotion` 90.9 to 98.1%, `AdvancePaneMotions` 91.2 to 95.4%, `ReplaceMotions` 97.8 to 99.5%, `FindPaneMP` 91.9 to 93.5%, `FindAnimation` 98.1 to 98.2%. No unit's 100% count dropped; both units stay `NonMatching`.

### Traps

- **A type-punned reference keeps a variable in memory.** CodeWarrior keeps a local in its stack slot and reloads it when its address is read through a reference of another type. The compact map's pointer-key `find` now reads the key as `reinterpret_cast<const void* const&>(key)` (a `find(Key* const&)` overload, so string keys keep their inline depth). This reproduced the target's `stw r4, 8(r31)` and the per-iteration key reload in `StopMotion` and `FindPaneMP`.
- **`set<T*>::begin()` and `end()` pun the tree iterator.** `typename tree_type::iterator i = mTree.begin(); return reinterpret_cast<iterator&>(i);` gives each call one extra kept object. With a `const_iterator` loop variable this matches the target's copy counts in functions with an inlined `throw()` (four stores for `begin`, three for `end`). Functions without one drop the dead stores, so standalone loops are unaffected.
- **The CLayout set loops use `const_iterator`.** `UpdateAttachedLayouts` (for-init form) and `RebindAnimations` (declared `child`, assigned in the loop) iterate with `std::set<...>::const_iterator`; the out-of-line `operator++` at `0x801D37D0` is the `const_iterator` one and was renamed. Declaring the `UpdateAttachedLayouts` iterator before the loop overflows the inline budget (the recursion inlines one level fewer, 73%).
- **Selective `insert_one` forwarding.** `map_compact_tree_adapter::insert_one` forwards to the base (one more inline level), which fixes the `left`/`leftmost` slot swap of CFileData's map insert. The `map<unsigned long, T*>` specialisation calls `map_compact_tree<...>::insert_one` by qualified name, so BWorld keeps its shallower path. This resolves the conflict recorded in sprint-a03.
- **A null check that jumps to a shared error report destroys its handle on a separate path.** `OpenFileStream` needed `if (!handle) { goto failed; }` with the label on the error report; a nested `if (handle) {...}` shares the destructor tail.

### Open, with what was tried

| Unit | Function | % | Cause and attempts |
| --- | --- | --- | --- |
| CLayout | `StopMotion`, `AdvancePaneMotions` | 98.1, 95.4 | Inlined `UpdateAttachedLayouts` needs one more kept `begin()` object per level, and the map `find` result `it` is stored (target `stw r30, 0x64`). Named returns, by-value conversion constructors, a user copy constructor on `const_iterator`, a conversion operator on `iterator`, a pun in the map `find`, friend comparison operators and a `std::map::find` wrapper either change nothing or break other functions. |
| CLayout | `ReplaceMotions` | 99.5 | Set iterator objects are grouped by inline level differently (target: pun local and return copy together, conversion temporaries apart), and the map `begin()` assignment keeps a temporary the target does not have. |
| CLayout | `DetachAttachedLayouts` | 98.4 | Object counts now match, but `end()` is recomputed and one register shifts after the `begin()` pun (was 99.3). |
| CLayout | `CalculateAttachedMtx` | 94.9 | `GetPaneAlpha(pRoot)`, `mLayout.get()`, `(*mLayout)`, `mAttach->mpPane->IsVisible()` and an nw4r `TestBit` `IsVisible` do not stop the `root == root` fold. |
| CLayout | `__ct__7CLayout` | 96.9 | Binding the result to a reference, a named local, or assigning `.x` alone do not remove the extra `VEC2` copy. |
| CFileData | `WaitUntilLoaded`, `OpenFileStream` | 99.95, 99.89 | `mErrorHandler` arguments: the target gives the error value the lowest register. `int` instead of `long` in the handler type, `long mError`, `GetError()` and `GetPath()` accessors, `ReportError()` and a config reference do not change the allocation. |
| CFileData | `map_base::erase` | 96.7 | The target copies the node into `r3` before the `front()` compare. Seven placements of the iterator copy and increment were tried. |
| CFileData | `reset<CWorker>` | 99.4 | The inlined `PushFinished` in `BThread::Stop` keeps a dead volatile reload of `mpHead`; ours drops it. A `for` tail walk does not change it. |
| CFileData | `UpdateResourceFiles` | 99.9 | Swapping `it` and `end` moves both registers by one; a third variable from the inlined job processing holds `r28`. |
