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
