# CDlgPiiSelector CDataPerPlayer block (#95)

Target `wpse-v1`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #95 lists 27 functions in `.text` `0x80095160`-`0x800962E4`. All belong to `game/CDlgPiiSelector.cpp` ([F-91-1](91-cdlgpiiselector.md)), compiled without inlining ([F-92-1](92-cdlgpiiselector-construction.md#f-92-1-the-file-is-compiled-without-inlining)). [F-95-3](#f-95-3-reconstruction-of-the-block) reconstructs the block. F-95-1 and F-95-2 record what each function is and the blockers that held it back.

## F-95-1: the block is the construction path of CDataPerPlayer

`CDataPerPlayer` is in the anonymous namespace, so its members are emitted only when a function in the unit uses them. Only two call sites reach this block through `bl`: `ShowMultiPlayerEntrance` (`0x80094DA0`, #94) and `Show` (`0x8009AAC0`, #110). Both are not yet reconstructed ([F-94-3](94-cdlgpiiselector-entrance-block.md#f-94-3-what-blocks-the-other-19-functions)).

| Address | Function | Evidence |
| --- | --- | --- |
| `0x80095160`, `0x80095170` | `shared_ptr` default constructors for the members of `MultiPlayerData` at 0x170 and 0x178 | Store two zero words; called from `0x800950F0` |
| `0x80095180` | Assignment of a `shared_ptr<BWorldObj>` made by `shared_from_this` | Calls `0x800951D0` and `0x8009CEC0`; a `BWorldObj` helper that links into this range, not `CDlgPiiSelector` code |
| `0x800951D0` | `enable_shared_from_this<BWorldObj>::shared_from_this()` | Already named |
| `0x800951E0` | `shared_ptr` copy constructor | Copies two words and calls `retain` (`0x80085350`) when the count pointer is not null |
| `0x80095230`, `0x80095260`, `0x80095270` | `std::min` result dereference and its empty argument-check helpers | `0x80095230` calls the empty pair and loads the `int` through the returned reference; consumed by `ShowMultiPlayerEntrance` for `mPlayerNum` |
| `0x80095280` | `std::min<int>(const int&, const int&)` | Already named |
| `0x800952A0` | `CLayout` load wrapper | Passes a zero final argument to `0x801CCBD0`; called with `dlgpiisel_confirm` (`.data` `0x8039F5DC`) and the default font map |
| `0x800952B0` | `CDataPerPlayer::CDataPerPlayer(CDlgPiiSelector*, int, int)` | Stores the three arguments at 0, 4 and 8, sets the vtable `0x8039FAA0` at 0x448, then constructs, in order: `0x14` (`0x80092480`), `0x20` (`0x800E5D00`), two `VEC2` (`0x40`, `0x48`, from `0x80095480` with the `.sdata2` constant at `r2 - 0x7E70`), `bool`s at 0x50 to 0x52, `shared_ptr` words at 0x54, 0x5C, 0x64, 0x6C to 0x74, three small-data words at 0x78 to 0x80, and `new` objects held by shared pointers at 0x84 (`CUIManager`, 0x54 bytes), 0x8C and 0x94 (`BUILayout`, 0x134), 0x9C (`CUIPiiSpec`, 0x188), 0xA4 (a 0x180-byte class with vtable `0x8039E2A8` derived from `0x801DA120`), 0xAC (a 0x13C-byte class from `0x8011F800`), a shared pointer at 0xB4, words at 0xC4 to 0xD0, and the `CFiber` at 0xD8. Allocated with `new(0x450)` by `ShowMultiPlayerEntrance` and by `Show` |
| `0x80095480` | `nw4r::math::VEC2::VEC2(float, float)` | Stores `f1` at 0 and `f2` at 4 |
| `0x80095490`, `0x800954A0`, `0x800954F0`, `0x80095590`, `0x800955A0` | `shared_ptr` or `weak_ptr` default constructors | Store two zero words; `0x800955A0` takes a second argument, zero at both call sites |
| `0x800954B0` | Constructor of the 0x180-byte member at 0xA4 | Calls `0x801DA120`, then stores the vtable `0x8039E2A8` |
| `0x80095500` | `CFiber::CFiber()` | The header's inline `CFiber() { Initialize(); }`: vtable `0x803BE468` is `__vt__6CFiber`, two `std::function` words at +0xC and +0x18, calls `CFiber::Initialize` (`0x8019E110`) with an exception-specification cleanup using `__unexpected` (`0x80341A60`) |
| `0x800955B0`, `0x800955C0` | `CDataPerPlayer` setup, a thunk then the 0xB74-byte body | Called after each `CDataPerPlayer` is stored. Builds the `BUILayout` tree (root, four children with `AttachChild`), the wide text with `DPL_ROOT` and `HELP`, message ids from `0x60000`, per-player text from `CPlayer::GetPii()->GetPersonalData()`, formats pane names with `FormatString`, positions panes with `nw4r::math::VEC3`/`VEC2` and `MTX34` transforms, then starts the `CFiber` at 0xD8 with a `std::tr1::function<void()>` (`CFiber::Start`, stack size `0x10000`) |
| `0x80096140` | `shared_ptr<BUI>` constructor from `shared_ptr<BUILayout>` | Already named |
| `0x80096190` | `shared_ptr<BUILayout>` copy constructor | Already named |
| `0x800961E0`, `0x80096230`, `0x80096280` | Three further 0x4C-byte `shared_ptr` conversion or copy constructors | Called by `0x800955C0` between `AttachChild` calls; target types not recovered |
| `0x800962D0` | 0x14-byte helper | Called twice by `0x800955C0`; not recovered |

## F-95-2: what blocks reconstruction

- **No emitter.** Every function above is reached only through `ShowMultiPlayerEntrance` or `Show`, which need `IGameSession` slots 22 and 40, `IDlgMsgWin` slots 11, 21 and 22 and `CDataPerPlayer` as a complete class ([F-94-3](94-cdlgpiiselector-entrance-block.md#f-94-3-what-blocks-the-other-19-functions)). The `CDataPerPlayer` ctor and `0x800955C0` cannot be compared in this unit before those exist.
- **Layout.** `CDataPerPlayer` is 0x450 bytes. Members at 0x14, 0x20, 0x9C (`CUIPiiSpec`), 0xA4, 0xAC and 0xB4 need their classes; `CUIManager` and `BUILayout` have headers, `CPiiListFilterSort` (#173) and the UI framework (#327/#328) are open.
- **Size.** `0x800955C0` alone is 0xB74 bytes with over 100 calls into UI, string and math helpers whose names belong to the UI framework blocks.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf` and the symbols in `config/wpse-v1/symbols.txt`. No decompiler output was used, and no source or symbol was changed.

## Traps

- **A block full of tiny `shared_ptr` constructors is not independent.** The five zero-store constructors at `0x80095490`-`0x800955A0` look like reusable weak functions but are separate instantiations for different member types. Do not give them one name; resolve each by the member offset in `0x800952B0` (F-95-1).
- **Anonymous-namespace classes are emitted only through their users.** Defining `CDataPerPlayer` alone produces nothing in the object; a caller in the unit must exist first (F-95-1).

## F-95-3: reconstruction of the block

All 27 functions of `0x80095160`-`0x800962E4` are reconstructed in `src/game/CDlgPiiSelector.cpp` and compare at 100% in `tools/re/unitdiff.py`, apart from relocation-name and pool-addend differences (the string pool of the partial unit differs from the original's, so `addi rX, r31, imm` offsets into `lbl_8039F4D0` differ, and local `@NNNN` labels stand for `lbl_` data). They need emitters, so the callers are reconstructed with them: `ShowMultiPlayerEntrance` (`0x80094DA0`, #94), the implicit `MultiPlayerData` constructor (`0x800950F0`, #94) and `Show` (`0x8009AAC0`, #110) match the same way. Callees in neighbouring blocks (the `bind` helpers and `wstring::clear` of #96, `std::tr1::shared_ptr` members) are emitted as a side effect; `bind<...OnSelectionChanged>`, `bind<...Run>` and `wstring::clear` do not yet compare at 100% and belong to #96.

`CDataPerPlayer::Setup(bool background)` (`0x800955C0`, 0xB74 bytes) builds the layout tree. State 1 is the pause-menu variant: the layouts `dlgpiiselt_root`, `dlgpiilist_root`, `piispec2`, `dlgpiilist_list`, `playerid` and `dlgpiilist_help` under `DPL_ROOT`, with a help text built from five `GetButtonMessage` strings. Other states load `dlgpiisel_root`, `dlgpiisel_p_root`, `piispec`, `dlgpiisel_list` and, for state 0 with exactly one joined player or state 2, `dlgpiisel_help`, attach the player layout under `P<n>` and `HELP<n>`, and clear the leave text when the player's Pii is fainted. It then registers `OnSelectionChanged` and `OnAccept` on the `CUIPiiList` callback owners (`std::tr1::bind(..., this, placeholders::_1)`), shows the list, hides spec, list and player id, samples the panes `INSIDE` and `OUTSIDE` into the two `VEC2` at 0x48 and 0x40 through `CLayout::NormalizeViewPosition` (`0x801D6280`) and starts the `CFiber` with `bind(&CDataPerPlayer::Run, this)` and stack size 0x8000. `0x800955B0` is `Setup()` calling `Setup(false)`.

Corrections found by this work:

| Item | Correction | Evidence |
| --- | --- | --- |
| `CFiber` constructor | `0x80095500` is `CFiber(const char*)`, which ignores its argument; `CDataPerPlayer` passes `"CDataPerPlayer"` (`0x8039F60C`) | The caller loads that string into `r4` and the callee overwrites `r4` |
| Bound members | `OnSelectionChanged` takes `BUI*` (renamed from the `Fv` spelling of F-96), and `0x800967F0` is `OnAccept(BUI*)`, which stores 3 in the word at +0xC. The two ptm triples in `.data` are `{0, -1, fn}` pointer-to-member constants, not `VEC3` | RTTI strings `function_imp<void (*)(BUI *), ... mem_fn_1<void, CDataPerPlayer, BUI *>, tuple<CDataPerPlayer *, ph<1>, ...>>` and `function_imp<void (*)(), ... mem_fn_0<void, CDataPerPlayer>, ...>` |
| `IGameSession` slot 40 | Returns the `IDisconMsg*`, not a pending map change; it was `GetPendingMapChange`, now `GetDisconMsg`. `IDisconMsg::CDisable` (4 bytes) holds it and calls `AdjustSuppressCount(1)`, and `MultiPlayerData` + 0x178 is a `shared_ptr<IDisconMsg::CDisable>` | `0x8008E360` stores its argument and calls vtable slot 7 of it with 1; slot 7 of `IDisconMsg` is `AdjustSuppressCount`; `0x800BABC0` is `lwz r3, 0xC70(r3)` |
| `CDataPerPlayer` layout | `0xBC` to `0xC0` are five `bool` flags cleared by `Setup`, not a `VEC2`; `0x78`, `0x7C` and `0x80` are `const char*` initialised to the empty string at `0x804B64CC`; `0x54` is a `weak_ptr` (destructor `0x8009BAC0`), `0x5C` and `0x64` are `weak_ptr`s of another type (`0x8009B970`). Their element types are not recovered, so `weak_ptr<IUIListItem>` and `weak_ptr<BUI>` are placeholders that fix the two constructor shapes | `Setup` stores five zero bytes; constructor `0x800952B0`; destructor `0x8009AC30` |
| `shared_ptr(const weak_ptr&)` | Takes a second `restrict_to` pointer argument defaulted to null, as the converting constructor does; `include/memory` declared one parameter | `0x800951D0` is `li r5, 0; b shared_ptr(weak)` |
| `std::tr1::function<R(A)>` | Constructors take the same defaulted `unused_argument*` as `function<R()>` | `0x8009C620` is called with `r5 = 0` |
| `compressed_pair` | `first` and `second` are forwarding members, not `using` declarations; `list::size()` calls a wrapper (`0x80095260`) that tail-calls the storage accessor (`0x80095270`) | Target bytes of `0x80095230`-`0x80095270` |
| `placeholders::_1` | A non-const static (`.sbss` `0x804B8FE0`), reloaded before every `bind`; the const `extern` was hoisted into a callee-saved register | `lbz r6, lbl_804B8FE0@sda21` before each of the three `bind` calls |

New or extended declarations: `GetButtonMessage` (`0x80088C20`, formats the controller button symbol into a message; `include/game/ButtonMessage.h`), `CUIPlayerID::Setup` (`0x8011F8B0`), `CUIPiiSpec` and `CUIPlayerID` constructors (padded to 0x188 bytes for `CUIPiiSpec`), `CUIList::GetSelectionCallbacks`, `GetAcceptCallbacks` and `CCallbackListOwner::Add` (`0x80078FB0`), `CLayout::NormalizeViewPosition` (`0x801D6280`, descriptive name), a three-argument `CLayout::Load`, `nw4r::lyt::Pane::GetGlobalMtx`, `CPlayer::GetPersonalData`, `CPiiBox::GetPersonalDataList` (`0x800915E0`) and `IDisconMsg::CDisable`. The names of `GetButtonMessage`, `NormalizeViewPosition`, `OnAccept`, `RequestClose` (`0x8009AC00`, stores 5 in the word at +0xC) and `Run` are descriptive, medium to low confidence. `sMaxPlayerNum` is the `int` 4 at `.sdata` `0x804B64A8` that `ShowMultiPlayerEntrance` clamps the Pii box size against.

## Traps (#95)

- **A constructor argument can be ignored by the callee.** `__ct__6CFiberFv` and the `CDataPerPlayer` call site differed only by a string load into `r4`; the original is a one-argument constructor that discards the name. Look for an extra `lis`/`addi` before a call that the callee overwrites (F-95-3).
- **A const extern hides reloads.** `placeholders::_1` declared `extern const` made CodeWarrior keep the byte in a callee-saved register across calls; the original reloads it, so it is a mutable static (F-95-3).
- **`if (a && b)` with a temporary in `b` adds a flag register.** `pPii != nullptr && pPii->GetPersonalData()->IsFainted()` compiled to two extra `li`; nested `if`s reproduced the original (F-95-3).
- **Do not run `cmd /c mklink` from the Bash tool.** It started an interactive `cmd` that never exited; use PowerShell `New-Item -ItemType Junction` (F-95-3).
