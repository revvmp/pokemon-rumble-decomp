# CDlgPiiList.cpp unit

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #82 owns the first function of `CDlgPiiList.cpp`; the file continues through the blocks of #83 to #91.

## F-82-1: Unit boundaries

The original file name comes from the anonymous-namespace class names `@unnamed@CDlgPiiList_cpp@::CDlgPiiList` and `@unnamed@CDlgPiiList_cpp@::CDlgPiiListImpl` in the RTTI strings at `0x8039E208` and `0x8039F0F0`.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x800102D0`–`0x80012CD8` | First and last records indexed by functions in the `.text` range |
| `extabindex` | `0x800463B0`–`0x800467E8` | All 12-byte entries whose function lies in the `.text` range |
| `.text` | `0x800896E0`–`0x80091354` | Starts with the `CDlgPiiList` factory, which constructs the 20-byte object with vtable `0x8039E1DC` and the `shared_ptr_deleter<CDlgPiiList>` control block. Ends after the destructor at `0x80091290`, the last function referenced by this file's vtables, `extabindex` entries or callers. See F-82-2 for the code from `0x80091360` |
| `.data` | `0x8039DF80`–`0x8039F518` | Starts with the `shared_ptr_deleter<CDlgPiiList>` vtable directly after the `NIntl::basic_formatter` exception-name cluster that ends the previous file. Ends after the `PIISPEC_ANM`, `LIST_ANM`, `PLAYERID_ANM`, `HELP_ANM` strings and their pointer table; `0x8039F518` is the `shared_ptr_deleter<CDlgPiiSelector>` vtable |
| `.sdata` | `0x804B6360`–`0x804B64A8` | `0x804B6360` and `0x804B6364` are read only by `0x8008BA70` and `0x8008C0E0`; RTTI records follow through the `default_delete` records ending at `0x804B64A8`. `0x804B64A8` is read only by `0x80094DA0` in the next file |
| `.sbss` | `0x804B8FD8`–`0x804B8FE0` | Read only by `0x8008E3B0` |
| `.sdata2` | `0x804B9DE0`–`0x804B9DF0` | Read only by `0x8008E3B0` and `0x8008D7F0`; neighbors belong to `0x80085DE0` and `0x800952B0` |

No `.ctors` entry falls in the range, so the file has no static initializer. The unit is registered `NonMatching`; the build still links the original bytes, and verification passes.

The class hierarchy from the RTTI base lists in `.sdata` and `.data`:

| Class | RTTI | Bases (offset) |
| --- | --- | --- |
| `@unnamed@CDlgPiiList_cpp@::CDlgPiiList` | `0x804B63D0` | `enable_shared_from_this<CDlgPiiList>` (4), `IDlgPiiList` : `IDlg` : `ICalcDraw` : `ICalc` (0) |
| `IDlgPiiList` | `0x804B6418` | `IDlg`, `ICalcDraw`, `ICalc` |
| `CUIPiiList` | `0x804B63E0` | `CUIList` : `BUILayout` : `BUI` (0), `enable_shared_from_this<BUI>` (4) |
| `CUIList` | `0x804B63F0` | `BUILayout`, `BUI`, `enable_shared_from_this<BUI>` |
| `BUILayout` | `0x804B63F8` | `BUI`, `enable_shared_from_this<BUI>` |

The file also emits `shared_ptr_deleter` records for `CUIPiiListItem`, `CUIPiiSpec`, `CUIPiiList`, `CUIPlayerID`, `IDisconMsg::CDisable`, `CPpuEngine`, `CUIManager`, `BUILayout`, `std::tr1::function<void (BUI *)>`, `CDlgPiiListImpl` and `char[]`, and `std::tr1::function` implementations binding `CDlgPiiListImpl` members with no argument, `const CPiiListFilterSort *`, and `BUI *`.

## F-82-2: Unit end and first matched functions

The range `0x80091360`–`0x80092EB0` is not part of this file. None of its functions is named by a `CDlgPiiList.cpp` vtable or called from `0x800896E0`–`0x80091354`; its callers are in `CDlgPiiSelector` code. Its exception records, `extab` `0x80012CD8`–`0x80012F68` and `extabindex` `0x800467E8`–`0x80046AD0`, follow this file's and are left to the automatic split with the code until the owning unit is identified. The split now ends at `0x80091354`, and objdiff counts 130 functions.

The source declares the anonymous-namespace `CDlgPiiList` with its original base order, `IDlgPiiList` at 0 and `enable_shared_from_this<CDlgPiiList>` at 4. Its only data member is `shared_ptr<CDlgPiiListImpl> mImpl` at 0xC, which gives the 0x14-byte object the factory allocates. `IDlgPiiList` adds `GetType` and `Show` after `IDlg`'s `IsClosable` and `Close`, matching vtable `0x8039E1DC`: `Calc` `0x800899E0`, `CalcMtx` `0x80089970`, `CalcView` `0x80089900`, `Draw` `0x80089890`, `IsClosable` `0x8008EE70` (returns false), `Close` `0x8008A370`, `GetType` `0x8008EE40` and `Show` `0x80089C20`. `Show(player, type)` is the dialog entry used by the script command `ShowDlgPiiList`.

Seven functions compile identically with the game profile and are renamed for objdiff: `IDlgPiiList::Create` `0x800896E0`, `~IDlgPiiList` `0x80089850`, `~CDlgPiiList` `0x8008FFD0`, `~move_ptr<CDlgPiiList>` `0x8008F5D0`, and the `shared_ptr_deleter<CDlgPiiList>` `get_deleter` `0x8008F070`, `dispose` `0x8008F0C0` and destructor `0x80090080`. The factory proves the TR1 constructor shape added to `include/memory`: the new pointer is held in a `Metrowerks::move_ptr` until the deleter is allocated, which produces the cleanup region and the zero store after the held pointer, and then `enable_shared_from_this` receives a weak reference. `weak_ptr::lock` (`0x80090A50`) and `shared_from_this` (`0x80089B50`) also compile identically from these headers in a scratch file, as does the TR1 `bad_weak_ptr` throw path; they are not yet emitted here because their callers need the `CDlgPiiListImpl` layout.

The remaining functions need the implementation object (`0x408` bytes), whose members include `CPiiListFilterSort` (#173), `CFiber` (#307), `CUIManager` (#327/#328), `CUIPiiSpec` (#217), `CUIPlayerID` (#218), `CUIList` and the `IDlgMsgWin` interface (#73 to #81). Their headers do not exist yet.

## F-82-3: Implementation object and remaining dependencies

`Show` allocates `CDlgPiiListImpl` (RTTI name `@unnamed@CDlgPiiList_cpp@::CDlgPiiListImpl`, 0x408 bytes) and constructs every member inline. The table records the observed layout; member names are not yet chosen.

| Offset | Size | Contents | Evidence |
| --- | --- | --- | --- |
| 0x0 | 4 | Player index, the first `Show` argument | Stored from `r27`; passed to the script manager's slot 0x64 |
| 0x4 | 4 | List type, the second `Show` argument | Returned by `GetType` `0x8008EE40`; type 1 adds the player's second list |
| 0x8 | 4 | Result, initially 0 | `Close` stores 1 |
| 0xC, 0xD | 1 each | Flags, initially false | |
| 0x10 | 0xC | `vector<shared_ptr<CUIPiiListItem> >` | Filled by `0x8008A5C0` from a list of Pii data |
| 0x1C | 0x20 | `CPiiListFilterSort` by value | Constructor `0x800E5D00`, destructor `0x800E5D20` |
| 0x3C | 8 | `shared_ptr<CUIManager>` | `new` 0x54, constructor `0x801C9680` |
| 0x44 | 8 | `shared_ptr<BUILayout>`, root layout | `new` 0x134, constructor `0x801C9EE0` |
| 0x4C | 8 | `shared_ptr<CUIPiiSpec>` | `new` 0x188, constructor `0x8011DCF0` |
| 0x54 | 8 | `shared_ptr<CUIPiiList>` | `new` 0x180, `CUIList` constructor `0x801DA120`, then vtable `0x8039E2A8` |
| 0x5C | 8 | `shared_ptr<CUIPlayerID>` | `new` 0x13C, constructor `0x8011F800` |
| 0x64 to 0x7C | 4 × 8 | Four `shared_ptr<BUILayout>` | Bound to the help, title, remote-memory and saving animations |
| 0x84 | 8 | `shared_ptr<IDlgMsgWin>`, initially empty | Forwarded `Calc`, `CalcMtx`, `CalcView` and `Draw` when set |
| 0x90 | 0x370 | `CFiber` by value | Inline constructor, then `0x8019E110`; destructor `0x8019E180` |
| 0x400 | 1 | Flag, initially false | |

`CFiber` (RTTI `CFiber`, vtable `0x803BE468` with only the destructor) is fully observable from #307's code: state flags at 0 (bit 0 started, bit 1 finished, bit 2 cancelled, bit 3 running), the stack as `shared_ptr<unsigned char>` with a `void (*)(void*) throw()` deleter at 4 (`Start` allocates it 32-byte aligned through `0x801B10D0`), two `function<void()>` hooks at 0xC and 0x18 that the yield routine `0x8019E5A0` calls before suspending and after resuming, an `OSMutex` at 0x24, two `OSCond` at 0x3C and 0x44, an 8-byte-aligned `OSThread` at 0x50, and the vtable pointer at 0x368. `Start` is `0x8019E320`, the resume routine `0x8019E460`, and the current fiber is the static word `0x804B9120`. The dialog starts its fiber with `CDlgPiiListImpl::Run` (`0x8008C0E0`) and a 0x8000-byte stack.

`CPiiListFilterSort` (#173) is only partly observable. Its constructor zeroes 0x0, 0x10 and the `shared_ptr` at 0x14, and stores -1 at 0x1C; 0x4 to 0xF are left uninitialized. The filter routine `0x800E5DB0` reads the filter kind at 0 (1 and 2 test bit `[4]` of two Pii attribute masks, 3 compares `[8]`, 4 compares or excludes by `[0xC]`), and the sort routine `0x800E62F0` indexes a seven-entry comparator table by 0x10. Apart from the constructor and destructor, no access to the `shared_ptr` at 0x14 or the word at 0x1C was found in the `0x800E5620`–`0x800ECBE0` filter, sort and menu routines or in the `CDlgPiiList` and `CDlgPiiSelector` code, so their types and names are not yet recoverable. Every `CDlgPiiListImpl` function, and every `CDlgPiiList` virtual except the seven already matched, depends on that layout.

The rest of the file instantiates MSL TR1 machinery whose names come from RTTI at `0x8039E6E0`, `0x8039EC70`, `0x8039EE58` and `0x8039F1C0`: `std::tr1::function`, `detail::function_base<void (*)(...)>`, `detail::function_imp<Signature, detail::bound_func<void, detail::mem_fn_N<...>, tuple<...> >, bool, bool>`, the ten-element `tuple` padded with `nat`, and `placeholders::detail::ph<1>`. A `function<void()>` stores its implementation pointer at 0 with an 8-byte buffer at 4; bound member functions are heap allocated through the out-of-line constructor at `0x8008FC50`, which first applies the variadic emptiness test at `0x80070700`. The implementation virtuals are destructor, `clone` (`new` copy), clone-into-buffer (placement copy) and the call, which dispatches through `__ptmf_scall`. Show and the layout routine also use `vector`, `wstring` concatenation (`0x8008FD30`) and `list` iteration. None of these library templates exists in the repository yet. The same dependency blocks #73 to #81 ([F-73-3](73-cdlgmsgwin.md#f-73-3-every-function-depends-on-the-games-standard-library)).

The automatic code at `0x80091360`–`0x80092EB0` (#88 to #91) builds `CUIPiiListItem` vectors from a Pii set and list for `CDlgPiiSelector`. It calls the vector destructor `0x8008A450` and the `shared_ptr` destructor `0x8008F4B0` that `CDlgPiiList.cpp` emits, which places it in a later translation unit. It has no data, strings or RTTI, so it has no recoverable file name, and it depends on the same `set`, `list` and `vector` templates.

## Traps

- **The next file starts with its factory, not its constructor.** `0x80092EB0` looks like a free helper of the dialog list, but it constructs and destroys the `CDlgPiiSelector` object. Do not end any split before it at the survey estimate `0x80092F6C`.
- **Code between two files is not automatically the earlier file's.** The first split ran to `0x80092EB0` and verified, but nothing in `CDlgPiiList.cpp` references `0x80091360`–`0x80092EB0`. A split that only verifies does not prove ownership; check references before extending a unit to the next known start (F-82-2).
- **Byte-identical template instances need reference evidence before renaming.** Every `~shared_ptr<T>` is the same 0x84 bytes. `0x8008F540` was first renamed `~shared_ptr<CDlgPiiList>` because the comparison matched, but exception tables in more than twenty other files reference it, which an anonymous-namespace type cannot explain. Check `extab` and call references to identify the instance before renaming it; the rename was reverted.
- **A by-value member can block a whole class.** `CPiiListFilterSort` sits by value inside `CDlgPiiListImpl`, and no access to two of its members has been found, so no evidence names them. Check by-value member layouts before planning a reconstruction around the class that contains them (F-82-3).
- **ICalcDraw slot names are fixed on main.** Slots 2 and 3 are `CalcMtx` and `CalcView` (#906); overrides in this file must use those names.
