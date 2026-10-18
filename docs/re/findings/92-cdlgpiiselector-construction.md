# CDlgPiiSelector construction block (#92)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #92 lists 23 functions in `.text` `0x80092F70`–`0x80093534`. All of them belong to `game/CDlgPiiSelector.cpp`, whose split, object line and first function (the factory at `0x80092EB0`) belong to #91 ([F-91-1](91-cdlgpiiselector.md)).

## F-92-1: the file is compiled without inlining

Every call in `CDlgPiiSelector.cpp` goes out of line, including one-instruction accessors:

- `shared_ptr<MultiPlayerData>::operator bool` (`0x800936C0`, five instructions) is called from `0x80094100`, `0x80094130`, `0x80094C00`, `0x80094CF0` and `0x80094DA0`.
- `shared_ptr::operator->` (`0x80093540`, `0x800936E0`) is called 38 times.
- The `move_ptr` destructor (`0x8009C250`) calls the compressed-pair `first()` twice, `second()` (`0x8009C340`) and `default_delete::operator()` (`0x8009C2E0`) instead of inlining them.
- Even the user-declared empty `default_delete` constructor (`0x80093470`, a single `blr`) is a call.

The template members instantiated at the end of the unit do not inline either, unlike `BWorld.cpp`, where a `#pragma dont_inline` region covers only the constructor ([F-43-1](43-bworld-ctor.md)). The whole unit therefore needs inlining off. A `#pragma dont_inline on` at the top of the source, never reset, reproduces it; a per-object `-inline off` would be equivalent and is not distinguished here. `IDlg()` (`0x80084E90`), whose kept weak copy is in the `CDlgMsgWin` region ([F-73](73-cdlgmsgwin.md)), also calls `ICalcDraw()` out of line, so at least one earlier file was built the same way.

## F-92-2: identities of the 23 functions

Names are the CodeWarrior mangled names that the reconstruction below produces; class names come from the RTTI strings at `0x8039B62C`–`0x8039D07C` ([F-91-1](91-cdlgpiiselector.md)). They are now in `config/wpse-v1/symbols.txt`, except `0x800931F0`, whose member name is unknown.

| Address | Function | Evidence |
| --- | --- | --- |
| `0x80092F70` | `CDlgPiiSelector::CDlgPiiSelector()` | Called by the factory after `new(0x40)`; stores vtable `0x8039F9A8` |
| `0x80092FF0` | `IDlgPiiSelectorInternal::IDlgPiiSelectorInternal()` | Stores vtable `0x8039FAE0` |
| `0x80093030` | `IDlgPiiSelector::IDlgPiiSelector()` | Calls `IDlg()` `0x80084E90`, stores vtable `0x8039FD20` |
| `0x80093070` | `IDlgPiiSelector::~IDlgPiiSelector()` | Slot 0 of `0x8039FD20`; calls `~IDlg` `0x80077820` |
| `0x800930D0` | `IDlgPiiSelectorInternal::~IDlgPiiSelectorInternal()` | Slot 0 of `0x8039FAE0` |
| `0x80093130` | `enable_shared_from_this<CDlgPiiSelector>()` | Constructor's base at 4 |
| `0x80093160` | `weak_ptr<CDlgPiiSelector>()` | Zeroes both words |
| `0x80093170` | `~enable_shared_from_this<CDlgPiiSelector>()` | Constructor cleanup; calls `~weak_ptr` `0x8009B910` |
| `0x800931D0` | `shared_ptr<CDataPerPlayer>()` | Element constructor passed to `__construct_array` for the four slots at 0xC |
| `0x800931E0` | `shared_ptr<CDlgPiiSelector::MultiPlayerData>()` | Member at 0x2C |
| `0x800931F0` | Empty non-virtual `CDlgPiiSelector` member taking the factory's `bool` | Called through `operator->` (`0x80093540`) with the factory argument; name unknown |
| `0x80093200` | `shared_ptr<IDlgPiiSelector>(const shared_ptr<CDlgPiiSelector>&)` | The factory's return conversion; its third argument is a null pointer |
| `0x80093250` | `shared_ptr<CDlgPiiSelector>(CDlgPiiSelector*)` | `move_ptr` guard, `new(0x10)` control block, `enable_from_this` |
| `0x80093300` | `shared_ptr<CDlgPiiSelector>::enable_from_this` | Weak reference into `enable_shared_from_this` at offset 4 |
| `0x80093360` | `weak_ptr<CDlgPiiSelector>(CDlgPiiSelector*, shared_ptr_deleter_common*)` | Calls `attach_weak` `0x800854B0` |
| `0x800933A0` | `weak_ptr<CDlgPiiSelector>::swap` | |
| `0x800933D0` | `Metrowerks::move_ptr<CDlgPiiSelector>(CDlgPiiSelector*)` | Third argument is the address of a zeroed `nat` |
| `0x80093400` | `Metrowerks::compressed_pair<CDlgPiiSelector*, default_delete<CDlgPiiSelector> >(CDlgPiiSelector*)` | Called by both `move_ptr` and the control block |
| `0x80093430` | Compressed-pair storage constructor, second type empty | Default-constructs the empty base, then stores the pointer |
| `0x80093470` | `default_delete<CDlgPiiSelector>()` | Empty |
| `0x80093480` | `shared_ptr_deleter<CDlgPiiSelector, default_delete<CDlgPiiSelector> >(CDlgPiiSelector*)` | Calls `shared_ptr_deleter_common()` `0x80085600`, stores vtable `0x8039F518`, then the compressed pair at 0xC |
| `0x800934E0` | `move_ptr<CDlgPiiSelector>::release()` | |
| `0x80093530` | Compressed-pair storage `first()` | Returns `this`; the only `first()` level, so `compressed_pair` exposes the storage accessor directly |

The compressed-pair storage class name `CompressedPairStorage` is the repository's independently authored name ([F-327-4](327-compressed-pair.md)), not an original one; low confidence for that component of the two symbol names. The converting constructor's mangled tail `Pv_Pv` comes from the `restrict_to<...>::type*` parameter in this reconstruction; the original parameter type is not recoverable from the bytes.

`CDlgPiiSelector` is 0x40 bytes: vptr at 0, `enable_shared_from_this<CDlgPiiSelector>` at 4, `shared_ptr<CDataPerPlayer>[4]` at 0xC, `shared_ptr<MultiPlayerData>` at 0x2C, two `bool`s at 0x34 and 0x35 and two words at 0x38 and 0x3C, all cleared by the constructor. Its vtable `0x8039F9A8` holds the destructor `0x8009E060`, `Calc` `0x800938C0`, `CalcMtx` `0x800937E0`, `CalcView` `0x80093700`, `Draw` `0x80093550`, `IsClosable` `0x8009AC20` and `Close` `0x8009AC10` (both return false), then seven selector virtuals. The script bindings name two of them by use: `ShowDlgPiiSelector` (`0x8013A200`) calls slot 8 with a player index and a `bool`, and `IsDlgPiiSelectorShown` (`0x8013A300`) returns slot 7 for a player index. Slot 9 (`0x80094100`) returns whether the `MultiPlayerData` pointer is set.

## F-92-3: library shapes needed by a no-inline unit

A scratch `CDlgPiiSelector.cpp` (not committed) with `#pragma dont_inline on` compiled with `cflags_game`. Its 23 block functions and the #91 factory matched the original instruction for instruction. objdiff showed 100% on 13 functions, and on the other 11 the only differences were relocation names of symbols outside the block (vtables, `IDlg()`, `~IDlg`, `~weak_ptr`, `~move_ptr`, `retain`, `attach_weak`, the deleter base constructor and the three still-unnamed callees of the factory). The emission order also matched, with the out-of-line inline copies following their first referencing function. That needed these changes to the shared TR1 headers. With inlining on they generate the same code, so every existing unit is unchanged:

- `default_delete` has a user-declared empty constructor; the original calls it (`0x80093470`).
- `compressed_pair` has a one-argument constructor, and the second-empty storage has a matching one that default-constructs the empty base. `compressed_pair` exposes the storage's `first`/`second` through using-declarations, because the original has one accessor level, not two.
- `shared_ptr_deleter` stores `Metrowerks::compressed_pair<T*, D>`, the same type as `move_ptr`'s storage, since both constructors call `0x80093400`. `get_deleter` returns `&second()`, and `dispose` calls `second()(first())`. This replaces the private `shared_ptr_storage` helper.
- `move_ptr` stores `compressed_pair<T*, D>`, and its destructor is defined outside the class. The original emits `~move_ptr` at `0x8009C250`, among the late template members, not after the shared-pointer constructor that calls it.
- The `shared_ptr` converting constructor takes a defaulted `restrict_to<is_convertible<Y*, T*>::value>::type*` argument, which accounts for the `li r5, 0` at every conversion call.

With these headers on main, `configure.py`, `ninja` and `verify.py` exit 0. The objdiff report is unchanged against the baseline: 16,663 functions with identical match percentages, 6,021 of them at 100%.

## F-92-4: what blocks committing the source

The factory's first loop calls, for players 0 to 3, the static getter `0x800BAD40` (returns the `.sbss` word `0x804B9000`, set by `0x800B2040` in the `CGameSession` file). It then makes a virtual call through slot 24 (offset 0x68) with the player index, and passes the result and the factory's `bool` to `0x801081E0`. That routine yields the fiber until bit 0 of the object's first word clears. The getter's object is an `IGameSession`: its vtable `0x803A3BC8` is the `ICalcDraw` slots followed by about 37 pure virtuals. Every function in this block is a weak copy emitted only because the factory or the constructor it calls references it. Committing any of them therefore needs `IGameSession` declared with at least the 20 pure virtuals before slot 24, and none of those has a name yet. That interface belongs to the `CGameSession.cpp` owner (#126). The `IDlgPiiSelector` header also needs names for selector slots 9 to 13, which belong to #93 and later. The source stays uncommitted, and the unit remains `NonMatching` with the original object.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf`, Python reading `orig/wpse-v1/main.dol` for vtables and RTTI, Wii/1.1 with `cflags_game` through `tools/re/unitdiff.py`, and a before-and-after comparison of `build/wpse-v1/report.json`. No decompiler output was used.

## Traps

- **A whole file can be built without inlining.** If one-instruction accessors such as `operator->` and `operator bool` are calls throughout a unit, including inside late template instances, inlining is off for the whole file, not just one function. The library headers must then have the original MSL helper structure (compressed-pair accessors, the `default_delete` constructor, out-of-class `~move_ptr`), because every helper becomes a visible call.
- **A weak-only block cannot be committed without its referencing function.** Every function of #92 is emitted only through the factory, so its source is blocked on the factory's callees (`IGameSession`), even though all 23 functions compile identically.
