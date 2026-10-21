# Issue 167: CPauseMenu.cpp reconstruction

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the split assembly under `build/wpse-v1/asm`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). This finding supersedes the "Blockers" section of [F-167-3](167-cpausemenu-split.md): the unit now has a complete source, `src/game/CPauseMenu.cpp`, and stays `NonMatching` because eight of its 37 functions still differ. No decompiler output was used.

## F-167-4: emission model of the unit

Link order reproduces the original only when the members are declared and defined as follows.

- `IPauseMenu::Create` is the first function. It inlines `new CPauseMenu` with its constructor and a private `Init`, so no separate constructor or `Init` is emitted.
- `Run`, `LeaveStage`, `TakeScreenshot` and `RunRemoteMemoryMenu` are non-inline members defined after `Create`, in that order, which is the address order `0x800D9110`, `0x800DA2D0`, `0x800DA610`, `0x800DB160`. `TakeScreenshot` is the class's only non-inline virtual, so it is also the key function that makes the `CPauseMenu` vtable appear in this unit.
- The inline virtuals appear in reverse declaration order (F-735-1), so the class declares `~CPauseMenu`, `IsOpen`, `Calc`, `CalcMtx`, `CalcView`, `Draw` in that order. `IsOpen` between the destructor and `Calc` is forced by the emitted order `Draw`, `CalcView`, `CalcMtx`, `Calc`, `IsOpen`, destructor.
- `NIntl::basic_formatter<wchar_t>` users `FormatMessage<...>` are function templates declared before `Create` and defined at the end of the file. Defining them earlier moves their instances directly after `Create`; the original places them after `RunRemoteMemoryMenu`.
- `CPauseMenuInput` is constructed only in `Run`, so its seven `IUIInput` slots (`0x800DC350`-`0x800DC4B0`, reverse slot order) appear once `Run` exists.

## F-167-5: classes, members and evidence

| Item | Evidence | Confidence |
| --- | --- | --- |
| `IPauseMenu : ICalcDraw`, slot 5 `IsOpen`, slot 6 `TakeScreenshot`, `Create(const MapDesc*, bool hideBackground, bool background)` (the third parameter is a `bool` because it reaches `Load` unchanged; #116 declared it `int fadeIn`, which would add a conversion at every `Load` call) | vtable `0x803A7C18`, callers `0x800A4EF0` and `0x800CD800` pass the word at +0xC of their first object and two true flags | slots high, names medium |
| `CPauseMenu` members: `mBackManager` 0x4 and `mFrontManager` 0xC (`shared_ptr<CUIManager>`), `mBackgroundLayout` 0x14, `mBlackLayout` 0x1C, `mRemoteMemoryLayout` 0x24 (`shared_ptr<BUILayout>`), `mIsOpen` 0x2C, `mFiber` 0x30, `mDialog` 0x3A0, `mCapture` 0x3A8, `mScreenshotBusy` 0x3D4, `mCaptureRequested` 0x3D5, `mHideBackground` 0x3D6, `mScreenshotLayout` 0x3D8, `mCopyrightLayout` 0x4C0, `mScreenshotScale` 0x5A8, `mPictureSize` 0x5B0 | constructor store order, destructor order and every access | high (layout), medium (names) |
| Layout names `pausemenu_bk`, `black`, `remote_memory`, `copyright`, `screenshot`; panes `MAPID`, `COPYRIGHT`, `SS`, `PICTURE`, `PROMPT`, `SS_ROOT`; motions `begin`, `end`, `fadeout`, `prompt_begin`, `prompt_loop`, `prompt_end` | `.data` and `.sdata` strings in first-use order | high |
| `StageID`/`MapIndex` tables: `gStageKindMessageIDs` (`0x8035B9C0`, indexed by the first field), `gStageRanks`, `gStageWorlds` | read by the title text of `Init`; the other two were named by #60 | medium |
| Title formatting: `0x6007A`-`0x6007D` pick the format by kind (field 0) and flag `GetSaveData(-1)->mData.mFlags[1]`; stage 7 of kind 2 uses `mStatus[0x13]` and `IsBattleRoyalFirstPart` | `Init` | medium |
| Menu kinds `MENU_LEAVE` 0, `MENU_CONTROLLERS` 1, `MENU_SCREENSHOT` 2, `MENU_REMOTE_MEMORY` 3, `MENU_RESUME` 0x20000000 | rodata/sdata2 initialisers and the `switch` of `Run` | medium |
| `UIMenuItem {mText, mKind}`, `UIMenuParam` (eight words, `"menu"` layout name), `RunUIMenu(const UIMenuParam&, CUIMenuCallbacks&, IUIInput*, shared_ptr<CUIManager>, shared_ptr<FontMap>, bool)` at `0x801DD850` | argument registers and the `0x801DD850` prologue | signature high, names low |
| `CUICallbackLists` (three `BUI::CCallbackListOwner`, destructor `0x800D9FB0`) and `CUIMenuCallbacks : CUICallbackLists` (adds an `int` and a `bool`, constructor `0x8011D090`, deleting destructor `0x800D9D70`) | the constructor's `DESTROYBASE` action names `0x800D9FB0`; the derived destructor inlines the base | high |
| `IGameSession` slots 30 to 41 | `GetPlayTimeSeconds` 30, `GetMapChangeType` 31, `SetFade` 32, `GetFadeFrames` 33, `RequestMapChange` 34, `RequestSceneChange` 35, `StartMapTransitionOut` 36, `StartMapTransitionIn` 37, `UpdateMapTransition` 38, `IsMapChangeFinished` 39, `GetPendingMapChange` 40, `IsRegisteredID` 41 come from the bodies of the `CGameSession` vtable `0x803A395C` entries; slots 25 to 29 and 42 are named `UnanalyzedSlotNN` because their meaning was not read | names low-medium; slot positions high |
| `IBattleRoyalDisplay` slots 5 to 10 (`Show`, `Hide`, `Finish`, `SetRound`, `GetRoundIndex`, `GetRound`) | `CBattleRoyalDisplay` vtable `0x8039A5A4` bodies | medium |
| `IDlgMsgWin` slots 0x48 to 0x64 (`SetOption`, `SetDefaultOption`, `OpenOptions`, `IsOptionsClosed`, `GetSelectedOption`, `OpenFootnote`, `BeginClose`, `IsClosed`) | `CDlgMsgWin` vtable `0x8039DBC0` bodies | low-medium |
| Outside functions named for the link: `IsGameFlagSet` `0x80165BC0` (tests a bit of a per-slot flag array), `GetPixelAspectRatio()` `0x801B6510` (no-argument overload), `IsBattleRoyalFirstPart` `0x801251C0`, `SaveScreenshotToRemoteMemory` `0x8016CB90`, `OpenRemoteMemoryDialog` `0x8016E420`, `GetBufferOwner` (a `CaptureTexture` member returning a `shared_ptr` copy, emitted here because a function returning a class is never inlined) | bodies and callers | medium |

`TakeScreenshot` renders the `copyright` layout (which contains a `PICTURE` pane) into a second `CaptureTexture`, encodes it twice with `TMCJPEGEncode`/`TMCJPEGEncodeToBuffer` (work area 0x1C00 bytes, quality 400), shows the preview through the `screenshot` layout and saves it through `0x8016CB90`. `LeaveStage` asks for confirmation unless the current map's stage field is 0, plays the `fadeout` motion on the black layout and requests either a scene change (stage 0) or a change to stage 0, map 0 of the current kind and rank.

## F-167-6: match status

Measured with `python tools/re/unitdiff.py CPauseMenu` after renaming the outside symbols in `config/wpse-v1/symbols.txt`. "Real differences" counts instruction rows that differ after ignoring the names of literal-pool symbols (`lbl_804BA000` against `@NNNN`), which objdiff scores as mismatches although the link resolves them identically.

| Function | Address | Size | objdiff | Real differences |
| --- | --- | --- | --- | --- |
| `IPauseMenu::Create` | `0x800D83B0` | 0xD18 | 99.93% | 0 rows |
| `~IPauseMenu` | `0x800D90D0` | 0x40 | 100% | 0 |
| `CPauseMenu::Run` | `0x800D9110` | 0x6F4 | 49.6% | 358 rows |
| `vector<UIMenuItem>::~vector` | `0x800D9810` | 0x78 | 100% | 0 |
| `vector<UIMenuItem>::push_back` | `0x800D9890` | 0x4D4 | 95.9% | frame slots of the exception flags |
| `~CUIMenuCallbacks`, `~CUICallbackLists`, `BUI::CCallbackListOwner::~` | `0x800D9D70`, `0x800D9FB0`, `0x800DA1E0` | 0x234, 0x230, 0xF0 | 100% | 0 |
| `CPauseMenu::LeaveStage` | `0x800DA2D0` | 0x340 | 98.4% | 21 rows |
| `CPauseMenu::TakeScreenshot` | `0x800DA610` | 0xB14 | 82.5% | 428 rows |
| `CaptureTexture::GetBufferOwner` | `0x800DB130` | 0x28 | 100% | 0 |
| `CPauseMenu::RunRemoteMemoryMenu` | `0x800DB160` | 0x62C | 99.99% | 2 rows (slots of two exception-specification records) |
| `FormatMessage<const wchar_t*, const wchar_t*>`, `<..., ..., ...>` | `0x800DB790`, `0x800DBC50` | 0x4BC, 0x4C8 | 74% | about 200 rows each |
| `Draw`, `CalcView`, `CalcMtx`, `Calc`, `IsOpen` | `0x800DC120`-`0x800DC340` | | 100% | 0 |
| `CPauseMenuInput` slots 6 to 0 | `0x800DC350`-`0x800DC4B0` | | 100% | 0 |
| `function_imp` clone, clone into storage, call, deleting destructor | `0x800DC4E0`, `0x800DC550`, `0x800DC5B0`, `0x800DC910` | | 100% | 0 |
| `shared_ptr_deleter<CPauseMenu>` `get_deleter`, `dispose`, deleting destructor | `0x800DC5E0`, `0x800DC630`, `0x800DC950` | | 100% | 0 |
| `shared_ptr<CPauseMenu>::~`, `move_ptr<CPauseMenu>::~` | `0x800DC660`, `0x800DC6F0` | | 100% | 0 |
| `~CPauseMenu` | `0x800DC770` | 0x194 | 100% | 0 |
| `__vec_split_buffer<UIMenuItem>::~` (formerly misnamed `__vec_constructor<UIMenuItem>::~`) | `0x800DC990` | 0x74 | 100% | 0 |

30 of the 37 functions have no instruction difference ([F-167-7](#f-167-7-split-buffer-destructor-and-push_back-reallocation) resolved the destructor). The seven that differ are `Run`, `push_back`, `LeaveStage`, `TakeScreenshot`, `RunRemoteMemoryMenu` and the two `FormatMessage` instances. Remaining causes, in order of understanding:

- **`Run` (358 rows).** The original keeps the four constant `UIMenuItem` values and the base `UIMenuParam` in stack memory and reloads them each pass, while the reconstruction keeps them in `r21`-`r30`; the frame is `0x180` against `0x110`. The register pressure that spills them in the original has not been reproduced.
- **`TakeScreenshot` (428 rows).** The float geometry and the second `CaptureTexture` follow the original structure; the divergence starts in the register assignment of the size computation.
- **`FormatMessage` (about 200 rows each).** The loop is structurally identical. The original materialises `rep_.l.is_long_ == 0` with `cntlzw`/`srwi.` once per iteration, keeps the short-data address `this + 2` in a register across the loop and tests that flag at every `data()` and `size()`. The reconstruction branches on the raw bit and recomputes the address. Wrapping the test in an inline `is_short()` did not change this.
- **`push_back`.** Fixed to 95.9% by F-167-7; what remains is the frame position of the fourteen exception-state flag bytes (the original zeroes two flags before the element loop where the reconstruction zeroes one).
- **`LeaveStage`, `RunRemoteMemoryMenu`.** Register numbers of a few temporaries and the frame slot of two exception-specification records.

## F-167-7: split-buffer destructor and push_back reallocation

- The function at `0x800DC990` is the destructor of `std::__vec_split_buffer<UIMenuItem, allocator<UIMenuItem>>`, not of `__vec_constructor`: the local at frame offset `0x8C` of `push_back` has the five words data, size, capacity, allocator reference and start, and the destructor stores `size = 0` first, as `~__vec_split_buffer` does. Renaming the symbol in `symbols.txt` made it 100%; the previous name came from the reference revision's guess.
- The original reallocation moves the old elements with the element loop (`construct_at_front(..., int2type<false>)`) while the fast path stays POD. `std::__vec_bulk_move<T>` in `include/vector` selects that overload per element type (default `is_POD<T>`), and `CUIMenu.h` specialises it to `false` for `UIMenuItem`. Changing the overload for every vector regressed `BWorld::GetObjects` (94% to 58%) and `CDividedGround` `push_back<const char*>` (98% to 79%), so the trait must stay per type.
- `mDialog->WaitUntilClosed()` without a `pDialog` local moves the polled dialog pointer in `RunRemoteMemoryMenu` to the original register (`r29`).
- The remaining `RunRemoteMemoryMenu` rows swap the frame slots of the first and last `__unexpected` records; the original allocates them in the order reset, temporary, assignment. No reordering of the three `mDialog` statements tried so far changed it.
- `Run` calls `IsHubMap` out of line while the original inlines it everywhere with the `neg`/`or`/`srwi` bool normalisation of the sum `0x60081 + IsHubMap()`; `#pragma always_inline on` around the helper did not change the score.

## Source additions

New headers: `IPauseMenu.h` (extends the stub from #117), `IBattleRoyalDisplay.h`, `CUIMenu.h`. Extended: `IGameSession.h` (slots 25 to 42), `IDlgMsgWin.h` (slots 0x48 to 0x64 and `WaitUntilClosed`), `BUI.h` (`SetVisible`, friend access to the callback owner), `BUILayout.h` (`GetLayout` defined inline), `CLayout.h`, `CaptureTexture.h`, `StageID.h`, `RenderMode.h`, `lyt_pane.h` (scale, size and translate accessors), `string` (copy constructor, `operator[]`, `clear`, iterator `append`), `wchar_t.h` (`wint_t` for C++). Outside symbols were renamed in `symbols.txt` only where the code needs them to link.

## Traps

- **A function returning a class by value is never inlined, but one returning a reference is.** `basic_formatter::str()` returning `std::wstring` stayed a 0x348-byte out-of-line function and `FormatMessage` called it; returning `const std::wstring&` and copying at the call site reproduces the original inlined expansion.
- **`cntlzw`/`srwi.` is the signature of a comparison kept as a `bool`.** `if (x.mStage == 0)` compiles to `srawi.`; an inline helper `bool IsHubMap()` returning the comparison reproduces the original `cntlzw` form. The same holds for `std::wstring` `data()` tests.
- **A by-value struct parameter is passed as a pointer to a caller-made copy.** Double stores of one word at two stack slots identify a by-value `MapIndex` (`RequestMapChange`); a single store with the callee reading the local directly identifies a `const&` (`RunUIMenu`'s parameter block).
- **`CFiber()` is `throw()`.** Without it the inlined `Initialize()` call lacks the original `__unexpected` landing pad; the pad appears only when the constructor is declared `throw()`.
- **`basic_string::operator=` and `assign(const basic_string&)` are different code.** The title `std::wstring` is assigned with `assign`, which reads its own size before the argument's data; `operator=` reads them the other way round.
- **`wint_t` must be `int` in C++.** `iswdigit` compiled with `unsigned short` loses the `li r5,1; cmplwi 0xff; bgt` pattern of the original; `int` reproduces it. C code keeps `unsigned short`.
- **`nw4r/lyt/lyt_texMap.h` must precede the other lyt and ut includes.** Its `lyt_common.h` defines a `RoundUp` macro that breaks `ut_algorithm.h` when included later.
- **The vector element trait decides both the fast path and the reallocation.** There is no setting that reproduces a POD fast path with a loop reallocation (see `push_back` above).
- **objdiff cannot reach 100% for any function that loads a literal.** `Create` has zero instruction differences and scores 99.93% because `lbl_804BA000` and `@14423` are different names; compare with literal names normalised.
- **A symbol named from the reference revision can hide the real class.** `0x800DC990` was labelled `__vec_constructor<...,1>::~` and no source made it match; the extab local layout showed a split buffer. Check the object layout of the local before rewriting vector code.
- **A vector template change is shared by every unit.** Switching the reallocation overload globally regressed two other units; use a per-type trait and compare `report.json` against the seed build.

## F-167-8: inlining collapse in Run, flattened push_back and the ShowMenu helper

Measured with `python tools/re/unitdiff.py CPauseMenu`. Run 49.9% to 98.3%, TakeScreenshot 82.5% to 84.2%, `vector<UIMenuItem>::push_back` 95.9% to 93.9%; the full report.json comparison against a baseline built from the same tree shows no other function changing.

- **Cause of the Run collapse.** `-pragma "warn_notinlined on"` lists the refused inlines. Once a function inlines `vector::push_back` twice, the compiler abandons all inlining in that function: `GetLayout`, `shared_ptr::operator*`, `~vector`, the `CMotionProgram` destructor and `IsHubMap` all became calls in Run, and the original keeps them inline while calling `push_back` out of line. Two inlined `push_back` calls plus the `CUIMenuCallbacks` locals in one function trigger it; fewer locals or one call do not.
- **Making `push_back` a non-candidate.** A non-inline (out-of-class) member whose own statement count is above roughly 15 is never attempted, and nothing else is disturbed. `include/vector` now defines `vector::push_back` out of class with a flattened POD body (fast path, `grow_by`, split-buffer reallocation); the non-POD branch still calls `base::push_back`. In-class definitions, explicit specialisations and `#pragma dont_inline` or `inline_max_size` do not have this effect. Cost: `push_back` itself drops to 93.9% (the original has an extra exception flag byte at frame offset 0x8 and a 0x10 larger frame).
- **Run structure.** The original destroys the callbacks and the item vector on every exit edge, which a `return` from an inlined helper produces; `break`, `switch` with a shared exit and `done` flags merge the cleanup. `CPauseMenu::ShowMenu`, an in-class inline member taking the four constant items and the base parameter block by reference, reproduces it, with the `switch` written `default: return;` first. Items are separate block-scope locals, and `IsStage7EvenMap` takes the map index directly.
- **Remaining Run rows.** Register choices (`_savegpr_24` against 25), the `neg`/`or`/`srwi` normalisation of `0x60081 + IsHubMap()` (bool helper variants, ternaries and casts do not change it), and the pairing of constant items to stack slots.

### Traps

- **Two inlined `vector::push_back` calls switch off all inlining in a large function.** Symptom: every small accessor is a call. Check the refusal list with `warn_notinlined on` in a scratch compile.
- **Dead padding statements count toward auto-inline size.** `if (a) { a = 1; }` repeated eight times rejects an out-of-class function; empty statements do not.
- **Local slot order is not declaration order.** Permuting the declarations of the four constant items did not change the score; the order follows first use.
- **Wrapping `basic_formatter` in a named local, an `=` copy, or constructor-body variants leaves both `FormatMessage` instances at 93.1%.** The retained-address behaviour matches [F-61](61-cdividedground-second-block.md); no source form found.
- **`CUIMenuCallbacks` destructors are inlined by this build but not by the original.** A non-inline destructor with about ten dead statements restores the call but is not plausible source and destabilises `RunRemoteMemoryMenu`; not committed.
