# Issue 167: CPauseMenu.cpp split

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf` and the dtk split assembly. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). No decompiler output was used.

## F-167-1: unit boundaries

The source file name is original: the factory's allocation-failure message uses `"CPauseMenu.cpp"` (`.data` `0x803A7658`) with `"%s(%d): not enough memory\n"`, and RTTI names `@unnamed@CPauseMenu_cpp@::CPauseMenu` and `@unnamed@CPauseMenu_cpp@::CPauseMenuInput`. The whole of #167 is this one file, so #167 owns its split, object line and any header.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x800217CC`–`0x800229AC` | Record of the factory `0x800D83B0`; `0x800217C4` belongs to `0x800D8370` and `0x800229AC` to `0x800DCA10` |
| `extabindex` | `0x80049D28`–`0x80049EC0` | Entries `0x800D83B0` through `0x800DC990`; the next entry is `0x800DCA10` |
| `.text` | `0x800D83B0`–`0x800DCA04` | `0x800D8330` and `0x800D8370` are deleting destructors in vtables `0x803A6F80` and `0x803A6EFC` of the previous file (`CNumericInput` type names end at `0x803A759D`). `0x800DCA10` stores the `CPii` vtable `0x803A84A8` and starts the next file (F-159-3) |
| `.rodata` | `0x80352C10`–`0x80352C60` | Three tables read by the menu routine `0x800DB160` and the fiber body `0x800D9110`; two hold the `"menu"` string pointer. `0x80352BC0` is read by `0x800D6F70`, `0x80352C60` by `0x800DCF40` |
| `.data` | `0x803A75A0`–`0x803A7F98` | The member-function pointer `{0, -1, 0x800D9110}`, layout and message names, the `vector length error` and exception lists, `CPauseMenu.cpp`, motion names, `NIntl::basic_formatter<w>` exception lists, deleter and `function_imp` vtables and names, the three class vtables, type names and base lists, and the `default_delete` names ending with `exception` at `0x803A7F88`. `0x803A7F98` (`5:/pii_1-151_387-492.arc/anim.brres`) belongs to `CPii` |
| `.sdata` | `0x804B6EF0`–`0x804B6F70` | `"menu"` (read through the `.rodata` tables), `black`, `SS`, `PICTURE`, `MAPID`, `begin`, `end`, `fadeout`, `PROMPT`, `SS_ROOT` and six type infos `0x804B6F40`–`0x804B6F68`. `"Cursor"` at `0x804B6EE8` is read by `0x800D6F70`; `0x804B6F70` (`_BATTLE`) starts the `CPii` strings |
| `.sdata2` | `0x804B9FE0`–`0x804BA020` | Read by `0x800D83B0`, `0x800D9110` and `0x800DA610`; `0x804B9FD8` and `0x804B9FDC` are read by `0x800D6F70`, `0x804BA020` by `0x800DCF40` |

The file has no `.bss` or `.ctors` entry. `.sbss` `0x804B9098` is shared with many earlier files, and `.rodata` `0x8035B9C0`–`0x8035BA58` sits among objects read by several other files; neither is owned here. With the split and a `NonMatching` object line, `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## F-167-2: classes

`CPauseMenu` derives from `IPauseMenu`, which derives from `ICalcDraw`. `IPauseMenu`'s vtable `0x803A7C18` holds its destructor `0x800D90D0`, a pure `Calc`, the `ICalcDraw` defaults `0x80063DC0` and `0x80063DD0`, a pure `Draw` and two pure slots 5 and 6.

| Slot | `CPauseMenu` (`0x803A7B24`) |
| --- | --- |
| 0 | Destructor `0x800DC770` |
| 1 | `Calc` `0x800DC2A0` |
| 2 | `CalcMtx` `0x800DC230` |
| 3 | `CalcView` `0x800DC1C0` |
| 4 | `Draw` `0x800DC120` |
| 5 | `0x800DC340`, returns the `bool` at 0x2C |
| 6 | `0x800DA610` |

The factory allocates 0x5B8 bytes and constructs the object inline. Layout from the destructor and the overrides:

| Offset | Member | Evidence |
| --- | --- | --- |
| 0x4, 0xC | `shared_ptr` to `ICalcDraw`-shaped objects | Every override forwards to their slots 1 to 4 |
| 0x14, 0x1C, 0x24 | `shared_ptr` | Released by the destructor |
| 0x2C | `bool` | Slot 5 |
| 0x30 | `CFiber` | `Calc` calls `Resume`; destroyed with `~CFiber`; started by the factory with a `function<void()>` binding `0x800D9110` and stack size 0x10000 |
| 0x3A0 | `shared_ptr` to an `ICalcDraw`-shaped object | Tested before each forward |
| 0x3A8 | `CaptureTexture` | `Draw` calls `0x801C4C20` with `false` when the `bool` at 0x3D5 is set, then clears it |
| 0x3D8, 0x4C0 | Layout objects (`0x801CB1C0` constructor, `0x801CC070` destructor, 0xE8 bytes) | `Calc` calls slot 1 of 0x3D8 when `0x801D07B0` returns true; `Draw` calls its slot 4 |

`CPauseMenuInput` implements `IUIInput` (vtable `0x803A7B8C`). Each slot calls `0x80124A50`, which returns the same input object at `0x803E9088` as `GetUIInput()`, and forwards to the same slot; `GetHold`, `GetTrig`, `GetRelease` and `GetRepeat` also set bit 0x10 when bit 0x100 is set. The seven functions `0x800DC350`–`0x800DC4B0` lie in reverse slot order, so they are inline in the class (F-735-1). The fiber body `0x800D9110` stores this vtable.

## F-167-3: functions of the block

| Address | Role |
| --- | --- |
| `0x800D83B0` | Factory: `new` of `CPauseMenu`, two `CUIManager` (0x54, `0x801C9680`) and three `BUILayout` (0x134, `0x801C9EE0`) objects inlined with their `shared_ptr` control blocks, message texts through `GetMessageText`, `GetSaveData(-1)`, wide `basic_string::replace`, the two formatter helpers, then `CFiber::Start` |
| `0x800D90D0` | `~IPauseMenu` |
| `0x800D9110` | Fiber body, a `CPauseMenu` member (strings `begin`, `end`) |
| `0x800D9810`, `0x800D9890`, `0x800D9D70`, `0x800D9FB0`, `0x800DA1E0` | MSL `vector` instances with 8-byte elements: deleter destructor, `push_back` (`vector length error`) and destructors used by exception actions. `0x800D9810`, `0x800D9890` and `0x800D9D70` are also called from `CPiiListFilterSort.cpp` (`0x800E7E6C`, `0x800E93B4`, `0x800E9630`), so they are weak copies first emitted here |
| `0x800DA2D0` | Fade-out routine (`fadeout`), called by the fiber body |
| `0x800DA610` | Slot 6 |
| `0x800DB130` | Returns a copy of the `shared_ptr` at 0x20 of its argument |
| `0x800DB160` | Menu routine (`menu`), reads `.rodata` `0x80352C10` and `0x80352C20` |
| `0x800DB790`, `0x800DBC50` | Wide-string formatting helpers using `NIntl::basic_formatter<wchar_t>` |
| `0x800DC120`–`0x800DC340` | `Draw`, `CalcView`, `CalcMtx`, `Calc`, slot 5 |
| `0x800DC350`–`0x800DC4B0` | `CPauseMenuInput` slots 6 to 0 |
| `0x800DC4E0`, `0x800DC550`, `0x800DC5B0`, `0x800DC910` | `function_imp` of the bound member (vtable `0x803A78E0`): clone, clone into storage, call through `__ptmf_scall`, deleting destructor |
| `0x800DC5E0`, `0x800DC630`, `0x800DC950` | `shared_ptr_deleter<CPauseMenu>` (vtable `0x803A782C`): `get_deleter` by type-name `strcmp`, `dispose` through the virtual destructor, deleting destructor |
| `0x800DC660` | Deleting `shared_ptr` destructor with a `throw()` pad |
| `0x800DC6F0` | `move_ptr<CPauseMenu>` destructor |
| `0x800DC770` | `CPauseMenu` destructor |
| `0x800DC990` | Destructor of a holder that frees the pointer at 0 and clears the word at 4 |

## Blockers

The file is compiled with inlining: the destructor and overrides inline `shared_ptr` access and release, and the factory inlines `CUIManager` and `BUILayout` construction. Every function of the file is emitted only through the factory, either directly or through the vtables and the member pointer it references, so no function can be reconstructed and compared before the factory compiles. The factory needs complete `BUI`, `BUILayout`, `CUIManager` and `CLayout` declarations (#327, #328, #890), MSL wide `basic_string`, and `NIntl::basic_formatter<wchar_t>`. The fiber body and slot 6 also need the `CLayout` motion interface and `IGameSession` slots. The unit stays `NonMatching` with the original object; none of the 37 functions is reconstructed.

## Traps

- **An entire block can be unreachable without its factory.** Anonymous-namespace classes whose virtuals are inline are emitted only when the factory constructs them. Check what references a function before planning a partial reconstruction; here that is every function of the block.
