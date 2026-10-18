# CDlgPiiList.cpp template instances (#85)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. The range `0x8008EE70`–`0x8008FD24` lies in `game/CDlgPiiList.cpp`, split and described in [F-82-1 to F-82-3](82-cdlgpiilist.md).

## F-85-1: Two shared_ptr destructor instances identified

Every `std::tr1::shared_ptr<T>` destructor compiles to the same 0x84 bytes, so the type of each instance comes from its references, not its code. The exception tables that reference each one:

| Address | Referenced by | Instance |
| --- | --- | --- |
| `0x8008F0F0` | Only the `CDlgPiiList` destructor `0x8008FFD0` (`extab` `0x80012090`) | `~shared_ptr<CDlgPiiListImpl>`, the `mImpl` member |
| `0x8008F180` | Only `Calc` `0x800899E0`, `shared_from_this` `0x80089B50`, `Show` `0x80089C20` and `Close` `0x8008A370` | `~shared_ptr<CDlgPiiList>`, the reference `shared_from_this` returns |

Both types live in the file's anonymous namespace, which explains why no other file references either address. Inside `shared_from_this`, the destructor cleans up the locked `shared_ptr<CDlgPiiList>`. The existing `src/game/CDlgPiiList.cpp` already emits both instances, and both compile identically to the originals. They are renamed in `symbols.txt`, which brings objdiff to 9/130 functions.

The other 0x84-byte destructors in the range (`0x8008F210`, `0x8008F300`, `0x8008F390`, `0x8008F420`, `0x8008F4B0`, `0x8008F540`) are referenced from many other files, so they are instances for shared types. This file only emits them first. Their types still need to be identified from those callers.

The rest of the range (`IsClosable` `0x8008EE70`, the `CUIPiiList` overrides `0x8008EE80` and `0x8008EEA0`, the `function_base` destructor `0x8008F030`, five out-of-line `shared_ptr<T>(T*)` constructors at `0x8008F7F0`–`0x8008FB70`, and the `function<void()>` constructor `0x8008FC50`) needs the `CDlgPiiList` vtable, the `CUIList` layout, or the TR1 `function` template, all blocked as recorded in F-82-3.

## Traps

- **A vtable is emitted only with its first non-inline virtual.** `IsClosable` is inline in the class, but `CDlgPiiList`'s vtable, and with it `IsClosable` `0x8008EE70`, is not emitted while `Calc` is declared out of line and left undefined. Define the virtuals inline before expecting their slots in the object.
