# Issue 173: CPiiListFilterSort.cpp

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). The block `0x800E5620`–`0x800E7A44` is the start of the file that defines `CPiiListFilterSort`; the file continues through the blocks of #174 and part of #175.

## F-173-1: unit boundaries

No string names the file. `game/CPiiListFilterSort.cpp` follows the only class whose RTTI and `function_imp` type names are confined to it (`CPiiListFilterSort`, `CPiiListFilterSort::FilterType`); the name is descriptive, medium confidence.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80023F74`–`0x800250BC` | First record indexes `0x800E5A50`; the record at `0x80023F6C` belongs to `0x800E5470` of the previous file, and `0x800250BC` to `0x800EC3D0` of the next |
| `extabindex` | `0x8004A2A4`–`0x8004A4CC` | Entries for `0x800E5A50` through `0x800EC320` |
| `.text` | `0x800E5620`–`0x800EC3D0` | `0x800E55F0`, `0x800E5600` and `0x800E5610` are slots of the `CPiiInfoWorldObjStat` vtable `0x803A8B80`, so the previous file ends there. `0x800EC3D0` is the `CPiiPersonalData` constructor (it stores `OSGetTime()` at 0x40 and the rare-colour loop on 0x38 and 0x48), followed by its weak-pointer cleanup and destructor; it starts the next file |
| `.rodata` | `0x80352CA0`–`0x80352E90` | Comparator table `0x80352CA0` (sort routine), menu tables `0x80352CC0` and `0x80352CD8` (menu routine) through `0x80352E70` (`0x800EB30C`); `0x80352C60` belongs to `0x800DCF40` and `0x80352E90` to `0x800F9BC0` |
| `.data` | `0x803A8BC8`–`0x803A9088` | Member-pointer constants, the `!std::exception!!std::tr1::bad_function_call!!` list, the two `function_imp` vtables and names binding `CPiiListFilterSort` members, and the `CUIMenuItem` base list. It follows the `CPiiInfoWorldObjStat` base list and ends before `CPiiPersonalData_init.pkc`, which `0x800ECE50` reads |
| `.sdata` | `0x804B7090`–`0x804B70D0` | Four `sort` shuffle counters (-4), the string `menu`, and the type info of the two `function_imp`s, `CUIMenu` and `CUIMenuItem`. `0x804B7088` is `CPiiInfoWorldObjStat`'s type info; `0x804B70D0` is read by `0x800EC7F0` in the next file |

No `.ctors` entry points into the range. `0x804B9098` (`.sbss`) is shared with many earlier files and is not owned here. The unit is registered `NonMatching`; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## F-173-2: functions of this block

| Address | Role |
| --- | --- |
| `0x800E5620`–`0x800E5BD0` | Seven comparators of `shared_ptr<CUIPiiListItem>`, in table order `0x800E5620`, `0x800E58A0`, `0x800E5980`, `0x800E5810`, `0x800E5780`, `0x800E5A50`, `0x800E5BD0` |
| `0x800E5CE0`, `0x800E5CF0` | Inline `CUIPiiListItem` getters of the masks at 0x14 and 0x18 |
| `0x800E5D00` | `CPiiListFilterSort` constructor: zero at 0, 0x10 and the `shared_ptr` at 0x14, -1 at 0x1C |
| `0x800E5D20` | `CPiiListFilterSort` destructor: releases the `shared_ptr` at 0x14 |
| `0x800E5DB0` | Filter: sorts (`0x800E62F0`), clears the output vector of the list object (+0x148), then copies matching items as `shared_ptr` to the base class; the kinds are 1 type mask 0x14, 2 move-type mask 0x18 (both `1 << [4]`), 3 `CPiiPersonalData` prefix (0x34) equal to `[8]`, 4 `CPiiPersonalData` 0x48 equal (or, when `[0xC]` is set, unequal) to word 0x10 of the object returned by `IGameSession` slot 25, which the script binding `GetOyaID` also reads |
| `0x800E61E0`–`0x800E6250`, `0x800E6F70`–`0x800E6F90` | Out-of-line inline helpers: list-object vector member, getters at 0x10 and 0x48, `shared_ptr` converting copy, `shared_ptr` get, vector `clear`, `size`, empty `shared_ptr`, element address |
| `0x800E62F0` | Sort: copies the seven-entry table to the stack, calls `sort` with the entry at 0x10; for kind 0 with a player index it moves the entries matching the player's current Pii to the front |
| `0x800E6570`, `0x800E69C0`, `0x800E6E10` | `std::sort<It, Compare>`, `std::sort<It, Compare&>` and `__sort132<Compare&, It>` from `include/algorithm`, with the counters `0x804B7090` and `0x804B7094` |
| `0x800E6FA0` | Sort menu (string `menu`), fully inlined list and `shared_ptr` destructors, calls `0x800E7BA0` and `0x800EB200` |

The comparators share one key chain. `Compare(a, b)` yields `a < b ? -1 : b < a`; every stage returns early when it is non-zero, and the last stage is a plain comparison of the word at 0xC.

| Table index | Keys (all of `CPiiPersonalData`) | Direction |
| --- | --- | --- |
| 0 | `FLAG_SPEC_HIDDEN` (0x40000), `FLAG_FAVORITE` (0x2), then the index 1 chain | Flags negated and tested `< 0` |
| 1 | BP (0x64), creation time (0x40, `OSTime`), word 0xC | Descending (`> 0`) |
| 2 | Same | Ascending (`< 0`) |
| 3 | Creation time, word 0xC | Descending |
| 4 | Creation time, word 0xC | Ascending |
| 5 | Species name (`0x801250B0` on the pointer at 0x10, compared with `0x801EF360` unless the pointers are equal), then the index 1 chain | Name `< 0` |
| 6 | Unsigned pointer at 0x10 (species order), then the index 1 chain | Pointer `< 0` |

A scratch reconstruction of index 1 with early returns and `return pB->mID < pA->mID` reproduces every instruction except the registers of the two item pointers (`r5`/`r3` against the original `r3`/`r4`). Local references, inline accessors, pointer and helper-function forms did not change that allocation.

## F-173-3: layouts recovered for the dependencies

`CPiiPersonalData` (0x80 bytes, `new` in `0x800ECBE0`) derives from `enable_shared_from_this<CPiiPersonalData>`. Names come from the F-29 script bindings that read or write each field: flags word 0x8 (`BossFlag` 0x80000000, `BossHideHP` 0x40000000, `Selling` 0x100000, `NewlyDisclosed` 0x80000, `SpecHidden` 0x40000, `InWiiRemote` 0x20000, and 0x2, which `CUIPiiListItem` shows as the `FAVORITE` pane), 0xC an identifier passed to the factory and copied by `0x800EE980` (unnamed), 0x10 the species data pointer, 0x14 `Sex`, 0x18 `WazaID[2]`, 0x20 `Level`, 0x24 `BonusMaxHP`, 0x28 `BonusAP`, 0x2C `BonusDP`, 0x30 `BonusSPD`, 0x34 `Prefix`, 0x38 `Rnd`, 0x40 creation time, 0x48 `OyaID`, 0x4C the register object of `ppdGetRegister`/`ppdSetRegister` (constructor `0x801119B0`), 0x54 `MaxHP`, 0x58 `AP`, 0x5C `DP`, 0x60 `SPD`, 0x64 `BP`, 0x68 `CappedLevel`, 0x6C `HP`, 0x70 and 0x74 the damage and heal totals, 0x78 `Price`, 0x7C `GroupNo`.

`CUIPiiListItem` (0x1C bytes, constructor `0x8011D7E0`, vtable `0x803AF3B8`, base `IUIListItem`): flags at 4 (initially 1; bit 0x4 for `InWiiRemote`), `shared_ptr<CPiiPersonalData>` at 8, the player index at 0x10, the mask of the species' two types at 0x14 and the mask of its two moves' types at 0x18.

## F-173-4: complete CPiiListFilterSort layout

`CPiiListFilterSort` is a 0x20-byte object with no vtable and no base class; the constructor stores no vtable pointer and the destructor is non-virtual. The header is `include/game/CPiiListFilterSort.h`, so other units can embed it by value (`CDlgPiiListImpl` at 0x1C, `CDataPerPlayer`).

| Offset | Size | Member | Evidence | Confidence |
| --- | --- | --- | --- | --- |
| 0x00 | 4 | `int mFilterType` | Constructor zero; the filter `0x800E5DB0` switches on 1 to 4 and treats other values as no filter. RTTI names the nested enum `CPiiListFilterSort::FilterType`; enumerator names are descriptive | type and offset high, names medium |
| 0x04 | 4 | `int mTypeIndex` | Kinds 1 and 2 compute `1 << [4]` and test the item masks at 0x14 and 0x18; not initialized by the constructor | offset high, name medium |
| 0x08 | 4 | `int mPrefix` | Kind 3 compares it with the `Prefix` (0x34) of the item's `CPiiPersonalData`; not initialized | offset high, name medium |
| 0x0C | 4 | `int mInvertOriginalTrainer` | Kind 4 keeps items whose 0x48 equals word 0x10 of the `IGameSession` slot 25 object when zero, and unequal items otherwise; not initialized | offset high, name medium |
| 0x10 | 4 | `int mSortType` | Constructor zero; the sort `0x800E62F0` indexes the seven-entry comparator table by it and tests it against zero for the favourites-first move | high |
| 0x14 | 8 | `shared_ptr<CPiiPersonalData> mpSelectedPii` | Constructor zeroes both words in this order after 0x10; the destructor releases the control block at 0x18 | size and offsets high; the pointee type is unproven because no code reads or writes it |
| 0x1C | 4 | `int mPlayerIndex` | Constructor stores -1; no access in the filter, sort, menu routine or the two dialog units. The sort takes its player index as a parameter instead | size high, name low |

The constructor stores in the order `mFilterType`, `mSortType`, the `shared_ptr` words, `mPlayerIndex`, which declaration order with initializers for the three scalars reproduces; the constructor (32 bytes) and destructor (140 bytes) are at 100% in objdiff. `CPiiListFilterSort::Filter` and `Sort` take the list object, so they are not methods of the 0x20 bytes alone: `0x800E62F0` reads only `this`+0x10, and `0x800E5DB0` reads `this`+0, 4, 8 and 0xC.

## Remaining work

`src/game/CPiiListFilterSort.cpp` defines the constructor and destructor ([F-173-4](#f-173-4-complete-cpiilistfiltersort-layout)). Every other function of the block needs at least one of: the `CPiiPersonalData` and `CUIPiiListItem` headers with the unnamed fields above (owners of `0x800EC3D0` and `0x8011D7E0`), `IUIListItem` (#890), the type of `CPiiListFilterSort` 0x14 and a use for 0x1C (no access found in `0x800E5620`–`0x800EC3D0`, `CDlgPiiList.cpp` or `CDlgPiiSelector.cpp`), `IGameSession` slot 25, and the MSL vector iterator. The filter and sort call the iterator's dereference, increment and inequality out of line (`0x8008E6C0`, `0x8008E7B0`, `0x8008E7C0`), and `sort` receives iterators by address, so this unit's `vector` iterator is a class; `include/vector` declares it as `T*`. Changing it is a shared-header change.

## Traps

- **A default `shared_ptr` member sits between two scalar stores.** The constructor stores 0x0, 0x10, 0x14, 0x18, 0x1C in that order, which declaration order plus initializers for the scalars reproduces. Assigning the scalars in the body put the `shared_ptr` zeroing first and scored 99.5%.
- **A weak `~shared_ptr<CPiiPersonalData>` appears next to the matched destructor.** objdiff lists it as only in source; the target inlines it, so a `Matching` promotion of this file must keep the weak copy out of the link (other units already emit it).

- **One file mixes inlining modes.** The filter and sort call two-instruction accessors out of line (`dont_inline`), while the comparators, `sort` and the menu routine inline `shared_ptr` and list code. Do not apply one pragma to the whole file.
- **CodeWarrior returns one-word classes in `r3`.** A `begin()` returning a one-pointer iterator class compiles to `lwz r3, 0(r3); blr`, so a returned value in `r3` does not prove a raw-pointer iterator.
- **A file can end at a constructor.** `0x800EC3D0` sits between survey blocks #175 and #176 but is the first function of the `CPiiPersonalData` file; its data begins at `CPiiPersonalData_init.pkc`.

## 2026-11-04 final-tail compiler controls

Fresh direct compiler probes leave the five remaining functions unresolved. ShowTypeFilterMenu has the same six operand differences documented in sprint-ra: the constant one and loaded kind exchange r3/r4 at its two mask shifts. Eighteen scoped identity/dead-assignment forms, nine volatile/reference/restrict parameter variants, and twelve scalar wrapper constructor/destructor/conversion combinations did not improve the baseline 99.93343% strict comparison. Applying dead-assignment suppression to the whole caller causes extensive extra operations and is rejected.

Sort's remaining differences are concentrated in argument-setup scheduling before its sort call. Explicit index locals, volatile index reads, cached iterators, and comparator references/pointers all regress; six inline helper parameter-order variants also regress. The two menu sort instances and the shared-pointer sort retain the known constant/pointer register allocation differences. Twelve explicit signed multiply-high remainder rewrites with a named divisor multiplier produce substantially worse code (roughly 87�89%) and are rejected.

No tracked source or shared-header change resulted. Ignored reproducers and result JSON are under `tools/local/session/filter-61-*`; the temporary algorithm overlay is removed after its sweep. All original behavior and previously exact functions remain unchanged in main.

## Exact type-menu mask register allocation

A distinct local TypeMaskBit enum between the integer kind and its shift fixes both mask calculations in ShowTypeFilterMenu. The target keeps the constant one in r3 and the loaded bit index/result in r4; an int temporary instead coalesces the constant with the result and exchanges r3/r4. Casting to the existing FilterType enum did not help, while the dedicated local enum reproduces every instruction operand. This is intentional D-0016 source shaping, not evidence of an original enum name. The integer value and shift operation remain unchanged for all menu kinds. No shared header is modified.

Fresh direct compilation preserves every other function comparison. menu-accepted.cpp/json under tools/local/session/filter-ppu records only one remaining objdiff discrepancy in the menu: the target inferred name lbl_804B9028 versus the source std::tr1::placeholders::_1 relocation. The coordinating maintainer owns the alias audit and full native validation. Scratch enum, integer-identity and helper-parameter-order experiments are in filter-ppu-mask.py.

### Follow-up sort probes

The remaining Sort argument block differs only in scheduling: target loads mSortType immediately after begin returns, before storing the returned iterator, whereas current source stores the iterator and forms all argument addresses before loading mSortType. Fresh enum-index locals and casted member views, restrict-qualified local pointers, comparator-table const/volatile qualifiers, and iterator temporary constructors/reference casts did not match. Scheduling off regresses broadly. A dedicated enum in the generic sort distance/remainder expressions also leaves all sort-instance comparisons unchanged. All these variants remain scratch-only under tools/local/session/filter-ppu.

The previous inline-helper argument-order probes were misleading: Sort is compiled under dont_inline on, and even explicitly inline helper functions become real calls there. Disassembly confirms this for GetComparator and SortItems. Their changed comparison percentages therefore do not establish a useful inline lifetime change. No sort or shared-header edit is retained.

The coordinating maintainer subsequently rebuilt the native report successfully: ShowTypeFilterMenu compares at 100% with the configured symbol mapping. The raw objdiff placeholder-name discrepancy does not require a symbol edit. The complete unit remains NonMatching because its four other tails are unresolved.
