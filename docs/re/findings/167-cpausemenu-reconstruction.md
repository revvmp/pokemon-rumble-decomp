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
| `vector<UIMenuItem>::push_back` | `0x800D9890` | 0x4D4 | 80.1% | 176 rows |
| `~CUIMenuCallbacks`, `~CUICallbackLists`, `BUI::CCallbackListOwner::~` | `0x800D9D70`, `0x800D9FB0`, `0x800DA1E0` | 0x234, 0x230, 0xF0 | 100% | 0 |
| `CPauseMenu::LeaveStage` | `0x800DA2D0` | 0x340 | 98.4% | 21 rows |
| `CPauseMenu::TakeScreenshot` | `0x800DA610` | 0xB14 | 82.5% | 428 rows |
| `CaptureTexture::GetBufferOwner` | `0x800DB130` | 0x28 | 100% | 0 |
| `CPauseMenu::RunRemoteMemoryMenu` | `0x800DB160` | 0x62C | 99.9% | 5 rows |
| `FormatMessage<const wchar_t*, const wchar_t*>`, `<..., ..., ...>` | `0x800DB790`, `0x800DBC50` | 0x4BC, 0x4C8 | 74% | about 200 rows each |
| `Draw`, `CalcView`, `CalcMtx`, `Calc`, `IsOpen` | `0x800DC120`-`0x800DC340` | | 100% | 0 |
| `CPauseMenuInput` slots 6 to 0 | `0x800DC350`-`0x800DC4B0` | | 100% | 0 |
| `function_imp` clone, clone into storage, call, deleting destructor | `0x800DC4E0`, `0x800DC550`, `0x800DC5B0`, `0x800DC910` | | 100% | 0 |
| `shared_ptr_deleter<CPauseMenu>` `get_deleter`, `dispose`, deleting destructor | `0x800DC5E0`, `0x800DC630`, `0x800DC950` | | 100% | 0 |
| `shared_ptr<CPauseMenu>::~`, `move_ptr<CPauseMenu>::~` | `0x800DC660`, `0x800DC6F0` | | 100% | 0 |
| `~CPauseMenu` | `0x800DC770` | 0x194 | 100% | 0 |
| `__vec_constructor<UIMenuItem>::~` | `0x800DC990` | 0x74 | 85.5% | 8 rows |

29 of the 37 functions have no instruction difference. The eight that differ are `Run`, `push_back`, `LeaveStage`, `TakeScreenshot`, `RunRemoteMemoryMenu`, the two `FormatMessage` instances and the `__vec_constructor` destructor. Remaining causes, in order of understanding:

- **`Run` (358 rows).** The original keeps the four constant `UIMenuItem` values and the base `UIMenuParam` in stack memory and reloads them each pass, while the reconstruction keeps them in `r21`-`r30`; the frame is `0x180` against `0x110`. The register pressure that spills them in the original has not been reproduced.
- **`TakeScreenshot` (428 rows).** The float geometry and the second `CaptureTexture` follow the original structure; the divergence starts in the register assignment of the size computation.
- **`FormatMessage` (about 200 rows each).** The loop is structurally identical. The original materialises `rep_.l.is_long_ == 0` with `cntlzw`/`srwi.` once per iteration, keeps the short-data address `this + 2` in a register across the loop and tests that flag at every `data()` and `size()`. The reconstruction branches on the raw bit and recomputes the address. Wrapping the test in an inline `is_short()` did not change this.
- **`push_back` (176 rows).** The fast path matches the repository's `int2type<true>` path (`++size; back() = x`), but the original reallocation copies the old elements with the element loop of `construct_at_front(..., int2type<false>)`, which the repository ties to a non-POD element. `UIMenuItem` is declared POD in `CUIMenu.h` so the fast path matches. With the non-POD path the fast path gets a placement-new null check that the original lacks. The vector header has no combination of the two.
- **`LeaveStage`, `RunRemoteMemoryMenu`.** Register numbers of a few temporaries and the frame slot of two exception-specification records.
- **`__vec_constructor<UIMenuItem>::~`.** The original stores `size = 0` before the base destructor body; adding that destructor to `__vec_constructor` would also change the vector-of-pointer destructors that other units link.

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
