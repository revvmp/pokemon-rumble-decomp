# CDlgPiiSelector CDataPerPlayer block (#95)

Target `wpse-v1`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #95 lists 27 functions in `.text` `0x80095160`-`0x800962E4`. All belong to `game/CDlgPiiSelector.cpp` ([F-91-1](91-cdlgpiiselector.md)), compiled without inlining ([F-92-1](92-cdlgpiiselector-construction.md#f-92-1-the-file-is-compiled-without-inlining)). No function of this block was reconstructed; this finding records what each is and why none can be emitted yet.

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
