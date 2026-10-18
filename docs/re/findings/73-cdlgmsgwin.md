# CDlgMsgWin and the formatter helper unit (#73–#81)

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issues #73 to #81 together cover `.text` `0x80084DE0`–`0x800896D8`. This finding establishes the translation units in that range, the `CDlgMsgWin` class hierarchy and layout, and why no function in it can be reconstructed as matching source yet. No split, source unit or symbol is added; no bytes are credited.

## F-73-1: Unit boundaries

The range holds two translation units. Neither starts where the issue survey estimated.

| Unit | `extab` | `extabindex` | `.text` | `.data` | `.sdata` | `.sdata2` |
| --- | --- | --- | --- | --- | --- | --- |
| `CDlgMsgWin.cpp` | `0x8000FBC8`–`0x80010218` | `0x80045D68`–`0x80046368` | `0x80084D40`–`0x80088C18` | `0x8039DB40`–`0x8039DED8` | `0x804B61A8`–`0x804B6358` | `0x804B9DD0`–`0x804B9DE0` |
| Formatter helper unit, file name unknown | `0x80010218`–`0x800102D0` | `0x80046368`–`0x800463B0` | `0x80088C20`–`0x800896D8` | `0x8039DED8`–`0x8039DF80` | `0x804B6358`–`0x804B6360` | none observed |

`CDlgPiiList.cpp` (#82) follows at `0x800896E0`; its deleter vtable is at `.data` `0x8039DF80` and its type info at `.sdata` `0x804B6368`.

Evidence:

- **Start of `CDlgMsgWin.cpp`.** The `.data` before `0x8039DB40` ends with the `std::detail::default_delete<@unnamed@CDlgInfo_cpp@::…>` names, the end of `CDlgInfo.cpp`. The first `CDlgMsgWin` object is the deleter vtable `__vt__…shared_ptr_deleter<CDlgMsgWin,…>` at `0x8039DB40`. In `.text`, `0x80084D40` (size 0x9C, called from `0x8007ADAC`, `0x8008C0E0`, `0x8008D100` and `0x8008D2C0`) allocates 0x120 bytes, calls the `CDlgMsgWin` constructor at `0x80084DE0`, wraps the object in a `std::tr1::shared_ptr`, calls the initializer at `0x800851E0` and returns the pointer converted to its interface. It is the unit's only out-of-line function. The preceding function `0x80084C70` is a `vector` of `shared_ptr` deleting destructor referenced only from `0x8007EA20` and `0x80083AE0`, inside `CDlgInfo.cpp`. The unit therefore starts at `0x80084D40`, inside #72, and the first `extabindex` entry is `0x80045D68`.
- **Emission order.** Every other function from `0x80084DE0` to `0x80088C10` is reachable from `0x80084D40` through direct calls, `CDlgMsgWin`'s vtable at `0x8039DBC0`, the `IDlgMsgWin` vtable at `0x8039DC90` or the deleter vtable, and they appear in depth-first order of that call graph. This is the `-ipa file` emission of a class whose methods are all defined inline in its body, the same pattern as `CBattleRoyalDisplay` in F-53-2. Eight functions are not reached by that walk (`0x80084ED0`, `0x80086A30`, `0x80087100`, `0x80087A50`, `0x80087A90`, `0x80087AC0`, `0x80087BE0`, `0x80087C70`). They are template members interleaved with reached ones, and nothing outside the unit calls them.
- **End of `CDlgMsgWin.cpp`.** No function from `0x80088C20` on is reachable from `0x80084D40`. `0x80088C20` is called only from `0x8008ACC0` and `0x800955C0`, and `0x80089500` only from `0x8008BA70`; the constructor at `0x80089330` and destructor at `0x800893F0` have callers in thirteen other units. In `.data`, the `CDlgMsgWin` throw-type names (`!std::exception!!std::logic_error!!std::length_error!!` at `0x8039DD20`, `!std::exception!!std::tr1::bad_weak_ptr!!` at `0x8039DD58`) are followed by its own string literals up to `0x8039DED4`. A second group of throw-type names follows at `0x8039DED8`: `Intl::basic_formatter<wchar_t>::InvalidFormatException`, `TooFewArgsException` and `TooManyArgsException`, which only `0x80088E80` throws. A separate exception-name group marks a separate file.
- **Helper unit.** `0x80088C20`–`0x800896D8` holds six non-inline functions and three inline `shared_ptr` copy helpers (`0x80089650`, `0x80089680`, `0x800896B0`). `0x80089330` constructs a 0x34-byte object holding a `wstring` format string at 0, a `vector<wstring>` of arguments at 0xC, two more `wstring`s at 0x18 and 0x24 and a `bool` at 0x30. `0x80088E80` builds that object on the stack, expands `{n}` placeholders with digit checks through `_current_locale`, and throws the three formatter exceptions. `0x800893F0` is its destructor. `0x80088DB0` concatenates a `wstring` and a wide C string. `0x80088C20` reads an entry through the global at `0x800BAD40` and formats it. `0x80089500` searches four slots of the same global. The original file name is unknown; by link order it sorts between `CDlgMsgWin.cpp` and `CDlgPiiList.cpp`.

## F-73-2: CDlgMsgWin classes and layout

Class names come from CodeWarrior RTTI in `.sdata`:

| Type info | Name | Bases (offset) |
| --- | --- | --- |
| `0x804B5AA0` | `ICalc` | none |
| `0x804B5B40` | `ICalcDraw` | `ICalc` |
| `0x804B5FF0` | `IDlg` | `ICalc`, `ICalcDraw` |
| `0x804B61C0` | `IDlgMsgWin` | `ICalc`, `ICalcDraw`, `IDlg` |
| `0x804B61B8` | `std::tr1::enable_shared_from_this<CDlgMsgWin>` | none |
| `0x804B61B0` | `CDlgMsgWin` | `enable_shared_from_this<CDlgMsgWin>` (4), `ICalc`, `ICalcDraw`, `IDlg`, `IDlgMsgWin` (0) |
| `0x804B61A8` | `std::tr1::detail::shared_ptr_deleter<CDlgMsgWin, std::detail::default_delete<CDlgMsgWin>>` | `shared_ptr_deleter_common` |
| `0x804B61C8` | `std::detail::default_delete<CDlgMsgWin>` | none |

Vtables: `ICalc` at `0x80399E38` (destructor `0x8005B720`, one pure slot, `0x80063DC0`), `IDlg` at `0x8039C3FC` (destructor `0x80077820`, pure, `0x80063DC0`, `0x80063DD0`, pure, `0x80077860`), `IDlgMsgWin` at `0x8039DC90` (the `IDlg` slots and 18 pure slots), `CDlgMsgWin` at `0x8039DBC0` (24 slots) and the deleter at `0x8039DB40`. The constructor chain is `CDlgMsgWin()` `0x80084DE0` → `IDlgMsgWin()` `0x80084E50` → `IDlg()` `0x80084E90` → `ICalcDraw()` `0x8005B710` (emitted in an earlier unit); the inline `ICalc()` is `0x80084ED0`.

`CDlgMsgWin` is 0x120 bytes (factory allocation):

| Offset | Type | Evidence |
| --- | --- | --- |
| 0x000 | vtable pointer | constructor stores `0x8039DBC0` |
| 0x004 | `enable_shared_from_this<CDlgMsgWin>`, a two-word weak pointer | RTTI base offset 4; `0x80084F40`/`0x80084F70` zero both words; destroyed through `0x80087230` (`release_weak`) |
| 0x00C | `bool` | constructor argument; when set, `0x80085F60` and `0x80085B70` register and unregister `shared_from_this()` with the manager returned by `0x800AA2B0` (slots 0x74 and 0x78) |
| 0x010 | layout screen object, 0xE8 bytes | constructed by `0x801CB1C0`, loaded with `"dlgmsgwin"` by `0x801CCBD0`, destroyed by `0x801CC070`; its vtable slot 0xC is called each update and slot 0x18 by `CDlgMsgWin` slot 4 (`0x80085690`) |
| 0x0F8 | `bool` | open flag; set by the open methods, cleared by `0x80085B70` |
| 0x0FC | `vector` of 0x18-byte entries, each with a `wstring` at 0xC | three words; `reserve(4)` in `0x800851E0`; element access `0x80085B60` (`mulli 0x18`); length error `"vector length error"` |
| 0x108 | `int` | cleared on open and close |
| 0x10C | `int` | cursor index; moved by the stick in `0x800856A0`, read by slot `0x80085FE0` |
| 0x110 | `int` | mode 0 to 3; the stick moves the cursor in modes 1 and 2, mode 1 toggles the `YES_CURSOR`/`NO_CURSOR` panes, and `0x80085E60` sets mode 3 |
| 0x114 | two-word animation handle pointer | assigned through `0x80087320`; queried by `0x800859C0`, `0x80085A20`, `0x800859E0` |
| 0x11C | `int` | controller slot, `-1` after initialization; used with the global at `0x800BAD40`, slot 0x64 |

Method names are not recovered; the vtable slots, strings (`FOOTNOTE`, `WITH_IMAGE`, `TEXT_ONLY`, `W_IMG_MSG`, `YES_CURSOR`, `NO_CURSOR`) and sound IDs (`0x8`, `0x9`, `0x54`, `0x5A` through `0x801E7F30`) give behavior only.

## F-73-3: Every function depends on the game's standard library

Of the roughly 230 functions in `0x80084DE0`–`0x800896D8`, about 150 are instantiations of Metrowerks MSL C++ templates, and every game function also inlines MSL code:

- `std::tr1::shared_ptr`, `weak_ptr`, `enable_shared_from_this` and `bad_weak_ptr` (for example the converting copy `0x80085300`, the owning constructor `0x80085360` with `shared_from_this` hookup `0x80085410`, `shared_from_this` through `0x80087550`, the deleter's `get_deleter` and `dispose` at `0x80087160` and `0x800871E0`).
- `std::vector` with its compressed-pair storage layers (constructor chain `0x80084FE0` → `0x80085010` → `0x80085040` → `0x80085090` → `0x800850C0`, `reserve` `0x80085290`, resize helpers `0x80085C50`–`0x80085CB0`, reallocation `0x80086AF0`, length error `0x80088550`).
- `std::wstring` with the short-string layout (long flag in bit 31 of word 0, short data at +2, long size and data at +4 and +8: `0x80085AB0`, `0x80085AF0`, `0x80085B20`), whose out-of-line members `0x80082020` and `0x80082260` were emitted in `CDlgInfo.cpp`.
- `std::string`, `logic_error` and `length_error` constructors used for the thrown exceptions (`0x80087010`, `0x80087070`, `0x80087080`, `0x80087120`).
- `Intl::basic_formatter<wchar_t>` inlined into the helper unit.

Matching any of these units needs independently written implementations of those templates that reproduce MSL's inline structure, emission order and data. The repository has only the shared-pointer control block, the deleter, the allocator and the type-info interfaces (#53, #327, PR #919 for compressed pairs); there is no `vector`, `basic_string`, `shared_ptr`, `weak_ptr`, `enable_shared_from_this` or `Intl::basic_formatter`. The Metrowerks implementation itself is restricted and cannot be imported. The `ICalc`/`ICalcDraw`/`IDlg` interfaces are in review in PR #906 (#53).

A unit cannot be linked as matching function by function: CodeWarrior emits each inline template once per unit in reference order, so the whole of `CDlgMsgWin.cpp` (`0x80084D40`–`0x80088C18`) must be reconstructed at once. Its split belongs to #72, which owns the first function.

## Method

- Tools: dtk split assembly from the normal build (`build/wpse-v1/asm`), GNU objdump 2.42, and scripts kept outside the repository that read `orig/wpse-v1/main.dol`, decode CodeWarrior RTTI records (name pointer, base list of type-info/offset pairs) and walk the call graph from `0x80084D40`.
- `extabindex` entries were read directly from the DOL to fix the exception-table boundaries.
- Decompiler output was not used.

## Traps

- **An issue range can start inside the previous issue's unit.** The survey placed the `CDlgMsgWin` file start at `0x80084DE0`, its constructor, but the unit's only out-of-line function, the factory at `0x80084D40`, sits in the previous issue's block. For an all-inline game class, look for the non-inline caller that allocates the object; it is emitted before the inline methods.
- **A call-graph walk separates units with no static initializer.** Neither unit has a `.ctors` entry. Reachability from the out-of-line function, together with the throw-type name groups in `.data`, located the end of `CDlgMsgWin.cpp` at `0x80088C18` instead of the survey's `0x800896D8`.
