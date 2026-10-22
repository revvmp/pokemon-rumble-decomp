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
