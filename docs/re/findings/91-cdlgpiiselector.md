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

## F-91-2: Factory and session interface

`src/game/CDlgPiiSelector.cpp` now holds the factory `IDlgPiiSelector::Create(bool)` (`0x80092EB0`) and, through it, the 23 functions of #92 ([F-92-2](92-cdlgpiiselector-construction.md#f-92-2-identities-of-the-23-functions)). With `#pragma dont_inline on` and `cflags_game`, objdiff shows 31 of the unit's 530 functions at 100%. The extra seven are out-of-line template members that later blocks own: `shared_ptr<CDlgPiiSelector>::operator->` `0x80093540`, `~weak_ptr<CDlgPiiSelector>` `0x8009B910`, `~shared_ptr<CDataPerPlayer>` `0x8009BCF0`, `~shared_ptr<CDlgPiiSelector>` `0x8009BDD0`, `~move_ptr<CDlgPiiSelector>` `0x8009C250`, `default_delete::operator()` `0x8009C2E0` and the storage `second()` `0x8009C340`. The unit stays `NonMatching` and links the original object.

The factory waits on each player's remote memory before it allocates the dialog. For players 0 to 3 it calls the getter `0x800BAD40`, then slot 24 of the returned object, then `0x801081E0` with the factory's `bool`. `0x801081E0` yields the fiber while bit 0 of the object's first word is set. Its `bool` argument is never read.

### Session interfaces

The getter returns the `.sbss` word `0x804B9000`, which the `CGameSession` constructor `0x800B2040` sets. The RTTI base list of `IGameSession` (`0x804B68D8`) names `ISession`, `ICalcDraw`, `ICalc` and `enable_shared_from_this<ISession>` at offset 4. The `ISession` vtable `0x803A3CB0` adds one pure slot to `ICalcDraw`'s five. `ITitleSession` (`0x803AE8C0`) adds none, and `IGameSession` (`0x803A3BC8`) adds slots 6 to 41. `0x8012DCD0` is the session loop. It creates the title session (`0x80119020`), calls slot 5, releases it, then creates the game session (`0x800BAC40`) and calls slot 5 again. Slot 5 is therefore the session's main routine, named `Run`. `CTitleSession`'s slot 5 (`0x80119460`) and `CGameSession`'s (`0x800B7AA0`) are both long loops.

Slots 6 to 24 of `IGameSession` are declared in `include/game/IGameSession.h`, which is enough for the factory's slot 24 call. Each name comes from a `CGameSession` implementation in vtable `0x803A395C` and its callers. The binding names come from [F-29-1](29-script-bindings.md).

| Slot | Implementation | Name | Evidence | Confidence |
| --- | --- | --- | --- | --- |
| 6 | `0x800B81E0` | `GetWazaDescData` | Returns member 0x8C; called by `GetWazaDesc`, `SetWazaDesc` and `InitWazaDescs`; `Run` reloads it next to `WazaDescData.pkc` | Medium |
| 7 | `0x800B81F0` | `GetPiiPropData` | Returns member 0xB8; called by `InitPiiProps` and `method_pprGet`/`pprSet`; `PiiPropData.pkc` | Medium |
| 8 | `0x800B8200` | `GetPiiCollectionBox` | Returns the word at 0xF0; called by `method_setCollectionDefeated`, `method_pprGetCollectionType` and `method_setCollectionAppeared`; the `CPiiCollectionBox` type is inferred | Low |
| 9 | `0x800B8230` | `GetCollectionType(u16)` | Asks the slot 8 object through `0x800E1C10`, the same call `method_pprGetCollectionType` makes, and returns 3 when a player holds the monsno; called by `IsNewMonsno` | Medium |
| 10 | `0x800B8210` | `GetBattleRoyalData` | Returns the word at 0xF8; called by `GetBattleRoyalClearSec`; `SD_BattleRoyalData` is in this file's data | Low |
| 11 | `0x800B8220` | `GetStatistic` | Returns the word at 0x100; called by `GetStatValue`, `AddStatValue` and `SetWazaUsed`; `CStatistic.pkc` | Medium |
| 12 | `0x800B8390` | `GetWazaName(int)` | Returns a `wstring` by value, built from a slot 6 entry through the message lookup `0x801EFA20`, or an empty string when the index is out of range | Low |
| 13 | `0x800B84C0` | `GetStageDesc(const MapIndex&)` | Returns an element of the 24-byte array at 0x108, indexed by three bytes of the 4-byte map index; called by `IsStageCleared`, `GetStageDescParam` and `SetStageDescParams` | Medium |
| 14 | `0x800B8500` | `GetMapDesc(const MapIndex&)` | Indexes a `shared_ptr<MapDesc>` vector reached through slot 13; called by `GetMapDescParam` and `GetMapDescEnemy` | Medium |
| 15 | `0x800B8570` | `CreateMapDesc(const MapIndex&)` | Returns slot 14's result when set, otherwise allocates a 0x38-byte `MapDesc` and stores it; called by `SetMapDescParams` and `InitMapDescEnemies` | Medium |
| 16 | `0x800B8D80` | `MovePlayerPiisToTeamPiiBox` | Adds every player's Piis to the team box (`0x800E0310`, which `AddPPDToTeamPiiBox` uses), counts them and returns the count; no callers found | Low |
| 17 | `0x800B8FB0` | `GetMoney` | Returns the word at 0x20; called by `GetMoney` | High |
| 18 | `0x800B8FC0` | `SetMoney(int)` | Stores the clamped value at 0x20; called by `SetMoney` | High |
| 19 | `0x800B8FF0` | `AddMoney(int)` | Calls slot 18 with the saturated sum and, for gains, records statistic -98; called by `AddMoney` | High |
| 20 | `0x800B9160` | `GetGameStage` | Returns the word at 0xC4C, set from `0x800BD410`, which constructs `CGameStage` (vtable `0x803A4138`); `Calc`, `CalcMtx` and `CalcView` forward to it | Medium |
| 21 | `0x800B90C0` | `IsAllPiiBoxCapacityFull` | Adds the counts of the enabled players' boxes to the team box count and compares the sum with the team box capacity; called by `IsAllPiiBoxCapacityFull` | Medium |
| 22 | `0x800B9170` | `GetTeamPiiBox` | Returns the word at 0x3C; called by `GetTeamPiiBoxPPD`, `GetTeamPiiBoxSize` and `AddPPDToTeamPiiBox`; `CPiiBox` is inferred from the deleter names | Medium |
| 23 | `0x800B9180` | `GetPlayer(int)` | Returns element `player` of `shared_ptr<CPlayer>[4]` at 0x44; the `CPlayer` deleter is in this file; used by all pad and player bindings | High |
| 24 | `0x800B9190` | `GetRemoteMemory(int)` | Returns element `player` of `shared_ptr<CRemoteMemory>[4]` at 0x64; the `CRemoteMemory` deleter is in this file; called by `CheckRemoteMemoryFatalError` | High |

The getter is declared as `IGameSession::GetInstance()`. That spelling is not original and has medium confidence. `0x801081E0` is declared as `CRemoteMemory::WaitUntilIdle(bool)`, a descriptive name with medium confidence. `std::wstring` comes from a new MSL-shaped `include/iosfwd`, which forward-declares `basic_string`.

### Selector interface names

| Slot | Function | Name | Evidence | Confidence |
| --- | --- | --- | --- | --- |
| 7 | `0x80094130` | `IsShown(int)` | Called by `IsDlgPiiSelectorShown` | High |
| 8 | `0x8009AAC0` | `Show(int, bool)` | Called by `ShowDlgPiiSelector` | High |
| 9 | `0x80094100` | `IsMultiPlayerEntranceShown` | Called through `IGameMap` slot 10 by `IsMultiPlayerEntranceShown` | High |
| 10 | `0x80094DA0` | `ShowMultiPlayerEntrance` | Called through `IGameMap` slot 10 by `ShowMultiPlayerEntrance`; allocates the 0x180-byte `MultiPlayerData` | High |
| 11 | `0x80094CF0` | `SetEntrancePPD(IController*, const shared_ptr<CPiiPersonalData>&)` | Stores a shared pointer in `MultiPlayerData` at `4 + type * 32 + channel * 8`, where type and channel are `IController` slots 7 and 8. `0x80096708` passes the player's controller (`CPlayer` + 0xC, also read by `GetPadHold`) and a copied list entry | Medium |
| 12 | `0x80094C00` | `GetEntrancePPD(IController*)` | Returns the same entry by value, or an empty pointer; internal | Medium |
| 13 | `0x80093FA0` | `UpdateDlgOpenState` | Passes `shared_from_this()` to `IGameMap` slot 27 when `0x80094070` finds anything shown, otherwise to slot 28. `CDlgPiiList`'s `Show` and `Close` call the same two slots; internal | Low |

`IGameMap` slot 29 (`0x800AA420`, called by the `Pause` and `Resume` bindings) stores the map's pause flag, and slot 30 reads it (`IsPausing`). The selector members are named after what their code does. `mIsPausing` (0x34) is set when `Show` calls slot 29 with true, and cleared when `Calc` calls it with false. `mInputLocked` (0x35) is changed by `0x80094A90`, which forwards the value to every `CDataPerPlayer`; while it is set, `0x80094240` ignores button input. `mErrorState` (0x38) is the state of the error-message sequence in `0x80094640`. `mError` (0x3C) holds the result of the remote-memory checks `0x800949F0` and `0x80094B40`. All four names have low confidence. `0x800931F0` is the empty member `Init(bool)`, with low confidence.

## Traps

- **The issue range is not the file range.** #91's listed range ends at `0x80092F6C`, inside this file. Every function from `0x80092630` to `0x80092EB0` belongs to the unnamed helper unit before it (F-82-3), and the factory is the only function of this file in #91.
- **A vtable-offset search after a getter can find other objects' calls.** Scanning a few instructions past `lwz r12,48(r12)` for the next slot load reported selector slots 11 to 13 in `0x800DE1E0`, `0x8016CB90` and `0x8016E420`, but those calls go through unrelated objects. Check that the call's `r3` comes from the getter's result before naming a slot from its caller (F-91-2).
