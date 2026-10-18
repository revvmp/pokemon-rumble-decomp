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

Eight functions compile identically with the game profile and are renamed for objdiff: `IDlgPiiList::Create` `0x800896E0`, `~IDlgPiiList` `0x80089850`, `~CDlgPiiList` `0x8008FFD0`, `~shared_ptr<CDlgPiiList>` `0x8008F540`, `~move_ptr<CDlgPiiList>` `0x8008F5D0`, and the `shared_ptr_deleter<CDlgPiiList>` `get_deleter` `0x8008F070`, `dispose` `0x8008F0C0` and destructor `0x80090080`. The factory proves the TR1 constructor shape added to `include/memory`: the new pointer is held in a `Metrowerks::move_ptr` until the deleter is allocated, which produces the cleanup region and the zero store after the held pointer, and then `enable_shared_from_this` receives a weak reference. `weak_ptr::lock` (`0x80090A50`) and `shared_from_this` (`0x80089B50`) also compile identically from these headers in a scratch file, as does the TR1 `bad_weak_ptr` throw path; they are not yet emitted here because their callers need the `CDlgPiiListImpl` layout.

The remaining functions need the implementation object (`0x408` bytes), whose members include `CPiiListFilterSort` (#173), `CFiber` (#307), `CUIManager` (#327/#328), `CUIPiiSpec` (#217), `CUIPlayerID` (#218), `CUIList` and the `IDlgMsgWin` interface (#73 to #81). Their headers do not exist yet.

## Traps

- **The next file starts with its factory, not its constructor.** `0x80092EB0` looks like a free helper of the dialog list, but it constructs and destroys the `CDlgPiiSelector` object. Do not end any split before it at the survey estimate `0x80092F6C`.
- **Code between two files is not automatically the earlier file's.** The first split ran to `0x80092EB0` and verified, but nothing in `CDlgPiiList.cpp` references `0x80091360`–`0x80092EB0`. A split that only verifies does not prove ownership; check references before extending a unit to the next known start (F-82-2).
- **ICalcDraw slot names are fixed on main.** Slots 2 and 3 are `CalcMtx` and `CalcView` (#906); overrides in this file must use those names.
