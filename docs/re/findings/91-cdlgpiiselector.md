# CDlgPiiSelector.cpp unit (#91)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #91 owns the first function of `CDlgPiiSelector.cpp`, its factory at `0x80092EB0`. The file continues through the blocks of #92 to #112.

## F-91-1: Unit boundaries

The file name comes from the anonymous-namespace class names in the RTTI strings, `@unnamed@CDlgPiiSelector_cpp@::CDlgPiiSelector`, `::CDataPerPlayer`, `::IDlgPiiSelectorInternal` and `::CDlgPiiSelector::MultiPlayerData`.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80012F68`–`0x80014E20` | Records indexed by the first and last functions of the `.text` range; the helper unit before ends at `0x80012F68` (F-82-3), and `0x80014E20` is the first record of `0x8009FD40` |
| `extabindex` | `0x80046AD0`–`0x8004794C` | All 12-byte entries whose function lies in the `.text` range |
| `.text` | `0x80092EB0`–`0x8009FD38` | Starts with the factory, which allocates the 0x40-byte object, calls the constructor `0x80092F70` and uses `0x8009BDD0` for cleanup. Ends after the vector helpers `0x8009FC80`–`0x8009FD30`, which follow the `shared_ptr_deleter` destructors `0x8009F9E0`–`0x8009FC20`. `0x8009FD40` is the out-of-line `CDropPii` constructor, which stores vtable `0x803A1030`, so it starts the next file |
| `.data` | `0x8039F518`–`0x803A1020` | Starts with the `shared_ptr_deleter<CDlgPiiSelector>` vtable and ends with the `default_delete<CDataPerPlayer>` name strings. `0x803A1020` (`minimap_drop`) is read by `0x8009FE50` in the next file |
| `.sdata` | `0x804B64A8`–`0x804B6598` | `0x804B64A8` is read by `0x80094DA0`. The range ends with the type records read by the selector's `get_deleter` functions `0x8009B870`, `0x8009E920`, `0x8009E9C0`, `0x8009EA60` and `0x8009EB00`. `0x804B6598` is the `CDropPii` type record named by vtable `0x803A1030` |
| `.sbss` | `0x804B8FE0`–`0x804B8FE8` | Read only by `0x800955C0` and `0x80096E20` |
| `.sdata2` | `0x804B9DF0`–`0x804B9E20` | Read only by selector functions `0x800952B0` to `0x8009A0E0`; `0x804B9E20` belongs to `0x800A13B0` and later |

No `.ctors`, `.rodata` or `.bss` object is referenced by the range. The unit is registered `NonMatching`; the build still links the original bytes and verification passes. objdiff counts 530 functions. The split agrees with the survey boundary between #112 and #113.

The vtables in `.data` give the class hierarchy:

| Class | Vtable | Notes |
| --- | --- | --- |
| `@unnamed@CDlgPiiSelector_cpp@::CDlgPiiSelector` | `0x8039F9A8` | 14 slots: the seven `ICalcDraw` and `IDlg` slots, then seven selector virtuals |
| `@unnamed@CDlgPiiSelector_cpp@::IDlgPiiSelectorInternal` | `0x8039FAE0` | Abstract, 14 slots, adding two to `IDlgPiiSelector`; destructor `0x800930D0` |
| `IDlgPiiSelector` | `0x8039FD20` | Abstract public interface, 12 slots (five after `IDlg`); destructor `0x80093070` |
| `@unnamed@CDlgPiiSelector_cpp@::CDataPerPlayer` | `0x8039FAA0` | Destructor only, `0x8009AC30` |

The file also emits `shared_ptr_deleter` records for `CDlgPiiSelector`, `CItem`, `CPii`, `CDataPerPlayer` and `CDlgPiiSelector::MultiPlayerData`, and `std::tr1::function` implementations that bind members with no argument, `const CPiiListFilterSort *` and `BUI *`, the same set as `CDlgPiiList.cpp`. Reconstruction depends on the same library templates recorded in [F-82-3](82-cdlgpiilist.md#f-82-3-implementation-object-and-remaining-dependencies).

## Traps

- **The issue range is not the file range.** #91's listed range ends at `0x80092F6C`, inside this file. Every function from `0x80092630` to `0x80092EB0` belongs to the unnamed helper unit before it (F-82-3), and the factory is the only function of this file in #91.
