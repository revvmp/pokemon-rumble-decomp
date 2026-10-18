# CDlgPiiList.cpp unit

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #82 owns the first function of `CDlgPiiList.cpp`; the file continues through the blocks of #83 to #91.

## F-82-1: Unit boundaries

The original file name comes from the anonymous-namespace class names `@unnamed@CDlgPiiList_cpp@::CDlgPiiList` and `@unnamed@CDlgPiiList_cpp@::CDlgPiiListImpl` in the RTTI strings at `0x8039E208` and `0x8039F0F0`.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x800102D0`–`0x80012F68` | First and last records indexed by functions in the `.text` range |
| `extabindex` | `0x800463B0`–`0x80046AD0` | All 12-byte entries whose function lies in the `.text` range |
| `.text` | `0x800896E0`–`0x80092EB0` | Starts with the `CDlgPiiList` factory, which constructs the 20-byte object with vtable `0x8039E1DC` and the `shared_ptr_deleter<CDlgPiiList>` control block; ends after `0x80092E70`. The function at `0x80092EB0` constructs the 64-byte object of `0x80092F70` and destroys it through `0x8009BDD0`, the `CDlgPiiSelector` factory pattern, so it starts the next file |
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

## Traps

- **The next file starts with its factory, not its constructor.** `0x80092EB0` looks like a free helper of the dialog list, but it constructs and destroys the `CDlgPiiSelector` object, and its `extabindex` entry is the first of that file. End the `.text` split at `0x80092EB0`, not at the survey estimate `0x80092F6C`.
