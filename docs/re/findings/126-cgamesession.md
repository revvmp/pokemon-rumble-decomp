# CGameSession.cpp unit and its first block (#126)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #126 lists 25 functions in `.text` `0x800B2040`–`0x800B32D4`; the unit spans #126 to #136. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). No decompiler output was used.

## F-126-1: unit boundaries of `game/CGameSession.cpp`

The file is one translation unit with no static initializer. Its split is in `config/wpse-v1/splits.txt`, with the object registered `NonMatching` in `configure.py`; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with the original object linked.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80019A1C`–`0x8001B864` | Records named by the first (`0x800B2040`) and last (`0x800BCAE0`) index entries of the unit |
| `extabindex` | `0x800482A0`–`0x800487EC` | 113 entries; the next entry indexes `0x800BCB40` |
| `.text` | `0x800B2040`–`0x800BCB34` | First function is the constructor; last is the `nw4r::snd::SoundHandle` destructor `0x800BCAE0` |
| `.data` | `0x803A3678`–`0x803A4088` | From the `vector length error` string used by the unit's vector helpers to the `exception` string; `0x803A4088` is the `CGameStage` file's `vector length error` |
| `.sdata` | `0x804B6880`–`0x804B6918` | From the `0` / `9999999` money limits (`0x800B8390`, `0x800B8FC0`) to the `default_delete<CRemoteMemory>` type info; `0x804B6918` is `shared_ptr_deleter<CGameStage>` |
| `.sbss` | `0x804B9000`–`0x804B9008` | The instance pointer written by the constructor (`0x800B2194`) and read by `IGameSession::GetInstance` (`0x800BAD40`); `0x804B9008` is first used at `0x800BD9C0` |
| `.sdata2` | `0x804B9EC8`–`0x804B9EF8` | Constants read by `0x800B9750` onward; `0x804B9EC0` and `0x804B9EC4` belong to the preceding file and `0x804B9EF8` is first read at `0x800BD334` |

- **Start.** `0x800B2040` is the first function that stores the `CGameSession` vtable `0x803A395C`. The preceding functions are the `CGameResult` file (#125).
- **End.** `0x800BCB40` stores the `CGameStage` vtable `0x803A4138`, and `0x800BD410` constructs `CGameStage`; both belong to the next file. The unit therefore ends at `0x800BCB34`, which is also the end of #136.
- **Class names.** The unnamed-namespace RTTI string `@unnamed@CGameSession_cpp@::CGameSession` (`0x803A3A0F`) names the source file. The class is local to the unit, so it lives in `src/game/CGameSession.cpp` and is reached from outside only through `IGameSession::Create` (`0x800BAC40`) and `IGameSession::GetInstance` (`0x800BAD40`).

## F-126-2: `CGameSession` layout

`sizeof` is `0xC78`: the factory allocates `0xC78` bytes (`0x800BAC60`) and the 64-bit members force 8-byte alignment. Base classes are `IGameSession` (`ISession` → `ICalcDraw` → `ICalc`, vptr at 0) and `enable_shared_from_this<ISession>` at 4.

| Offset | Member | Evidence | Confidence |
| --- | --- | --- | --- |
| `0x10` | `s64 mPlayTime` | Added to `OSGetTime() - mStartTime` by `0x800B29A0` and `0x800B9750` | Medium |
| `0x18` | `s64 mStartTime` | Same | Medium |
| `0x20` | `u32 mHeaderValue` | Stored to `SlotData::Data::mHeader[2]` by `0x800B2A50`; copied to `0x24` by `0x800B7E50` | Low |
| `0x24` | `u32 mLoadedHeaderValue` | Written only by that copy | Low |
| `0x28` | `CRemoteMemoryRegistry` | Five-word tree object built by `0x800ED1B0` and destroyed by `0x800EE2B0`; its count at `0x38` is saved into the slot; passed to every `CRemoteMemory` constructor; returned by slot 25 | Medium (type name descriptive) |
| `0x3C` | `shared_ptr<CPiiBox> mTeamPiiBox` | Returned by `GetTeamPiiBox` (slot 22) | Medium |
| `0x44` | `shared_ptr<CPlayer> mPlayers[4]` | `GetPlayer` (slot 23); constructed with `new CPlayer(i)` (`0x74` bytes) | High |
| `0x64` | `shared_ptr<CRemoteMemory> mRemoteMemories[4]` | `GetRemoteMemory` (slot 24); `new CRemoteMemory(i, &registry)` (`0x40` bytes) | High |
| `0x84` | `shared_ptr<CFileData> mSessionFile` | Stored by `0x800B7B34` from the loaded `CGameSession.pkc`, passed to the `.pkc` loaders; returned by slot 26 | Low |
| `0x8C` | `WazaDescData` (`0x2C`) | Constructor `0x8010DDE0`, destructor `0x8010DE20`; returned by slot 6 | Medium |
| `0xB8` | `PiiPropData` (`0x38`) | Constructor `0x800F2630`, destructor `0x800F2B30`; returned by slot 7 | Medium |
| `0xF0` | `shared_ptr<CPiiCollectionBox>` | Slot 8; export `0x800E1CE0` | Medium |
| `0xF8` | `shared_ptr<CBattleRoyalData>` | Slot 10; export `0x8006A860` | Medium |
| `0x100` | `shared_ptr<CStatistic>` | Slot 11; export `0x80116260` | Medium |
| `0x108` | `StageDesc mStageDescs[120]` | `__construct_array` with element size `0x18` and count `0x78`; indexed by slot 13 | High |
| `0xC48` | `MapIndex mMapIndex` | Built as `(0, 1, 0, 1)` by the constructor; stored by `0x800B9B30` | Low |
| `0xC4C` | `shared_ptr<IGameStage> mGameStage` | Slot 20 | Medium |
| `0xC54` / `0xC58` | `int mStageState`, `int mStageParam` | Set by `0x800B9B30` (0, param) and `0x800B9BA0` (1, param) | Low |
| `0xC5C` / `0xC60` | `nw4r::ut::Color mFadeColor`, `mFadeTargetColor` | Constructed as `(0, 0, 0, 255)`; chosen by `0x800B97E0` | Low |
| `0xC64` | `int mFadeFrames` | Defaults to 15 in `0x800B97E0`; slot 33 | Low |
| `0xC68` | `shared_ptr<CSaveSequence> mSaveSequence` | Created lazily by `0x800BA9B0` and `0x800BAA90` before a virtual save call | Low |
| `0xC70` | `int mMode` | The constructor argument; slot 40 | Medium |

Member names are descriptive (low unless stated). The constructor's argument and member initialisation order reproduce the original `0x800B2040` exactly (100%).

## F-126-3: functions

All 25 functions of #126 are reconstructed in `src/game/CGameSession.cpp`; objdiff reports each at 100% except `GetSnapshot` (see Remaining work). Names are the mangled spellings the source produces.

| Address | Function | Match |
| --- | --- | --- |
| `0x800B2040` | `CGameSession::CGameSession(int)` | 100% |
| `0x800B2370` | `MakeMapIndex(int, int, int, int)` | 100% |
| `0x800B2390` | `ISession::~ISession()` | 100% |
| `0x800B2400` | `IGameSession::~IGameSession()` | 100% |
| `0x800B2470`, `0x800B2480` | `shared_ptr<CPlayer>()`, `shared_ptr<CRemoteMemory>()` | 100% |
| `0x800B2490` | `StageDesc::StageDesc()` | 100% |
| `0x800B24E0` | `vector<shared_ptr<MapDesc>>::~vector()` | 100% |
| `0x800B25B0` | `StageDesc::~StageDesc()` | 100% |
| `0x800B26A0` | `shared_ptr<CPlayer>::get()` | 100% |
| `0x800B26B0` | `CGameSession::~CGameSession()` | 100% |
| `0x800B28F0` | `CGameSession::InitStageDescs()` | 100% |
| `0x800B29A0` | `CGameSession::GetSnapshot(SessionSnapshot*)` | 82.43% |
| `0x800B2A50` | `CGameSession::SaveToSlot()` | 100% |
| `0x800B2FE0`–`0x800B3360` | default constructors and destructors of `vector<SD_*>` and the four `SD_*` constructors | 100% |
| `0x800B30D0` | `CPiiPersonalData::GetCreationTime() const` | 100% |
| `0x800B9750` | `CGameSession::GetPlayTime()` (slot 30, belongs to #133; written to confirm the 64-bit tick arithmetic) | 100% |

`SaveToSlot` writes the session state into the current slot (`GetSaveData(-1)`): play time, remote-memory count, header word, elapsed seconds since boot, the `SD_PiiCollectionData`, `SD_PiiPersonalData`, `SD_BattleRoyalData` and three `SD_StatisticValue` vectors, and the creation time (the 64-bit unique identifier at `CPiiPersonalData` + `0x40`) and species of the first non-null lead Pii among the four players. Each vector is built element by element from the `Data` records of a `SessionSnapshot`, then swapped into the slot. Its callers are the two save entry points `0x800BA9B0` and `0x800BAA90`; `0x800B5670` (`LoadFromSlot`, #129) is the inverse and is called by the constructor.

## F-126-4: `IGameSession` slots 25 to 42

`IGameSession`'s vtable (`0x803A3BC8`) has 43 slots, all pure after the `ICalcDraw` slots. [F-91-2](91-cdlgpiiselector.md#f-91-2-factory-and-session-interface) declared slots 6 to 24. The `CGameSession` vtable `0x803A395C` names the implementations of the rest, which `include/game/IGameSession.h` now declares so that the class is concrete. The names are descriptive and low confidence; the owning issues should rename them.

| Slot | Implementation | Declared as | Evidence |
| --- | --- | --- | --- |
| 25 | `0x800B91A0` | `GetRemoteMemoryRegistry()` | Returns `this + 0x28` |
| 26 | `0x800B91B0` | `GetSessionFile()` | Returns the shared pointer at `0x84` by value |
| 27 | `0x800B91E0` | `CheckAddPersonalData(int, const shared_ptr<CPiiPersonalData>&)` | Returns 4 when the player already holds the data, 5 when slot 28 refuses it |
| 28 | `0x800B9340` | `CanSelectPersonalData(int, ...)` | Compares the data with every other joined player's lead and sub data |
| 29 | `0x800B94A0` | `IsPersonalDataSelected(int, ...)` | Walks the lists reached from the players and the team box |
| 30 | `0x800B9750` | `GetPlayTime()` | `mPlayTime` plus elapsed ticks, converted to seconds as a float |
| 31 | `0x800BAD60` | `GetStageParam()` | Returns `0xC58` |
| 32 | `0x800B97E0` | `SetFade(int, int)` | Chooses the colours at `0xC5C`/`0xC60` and the frame count at `0xC64` (default 15) |
| 33 | `0x800B98F0` | `GetFadeFrames()` | Returns `0xC64` |
| 34 | `0x800B9B30` | `StartStage(const MapIndex&, int, int)` | Stores the index (last byte 1) and parameter, forwards to the stage's slot 10 |
| 35 | `0x800B9BA0` | `RestartStage(int, int)` | Stores the parameter, forwards to the stage's slot 10, sets state 1 |
| 36, 37 | `0x800BA9B0`, `0x800BAA90` | `BeginSave()`, `BeginSaveAndExit()` | Create `mSaveSequence` if empty, call `SaveToSlot`, then slot 6 or 7 of the sequence |
| 38 | `0x800BAB70` | `ContinueSave()` | Tail call of slot 8 of the sequence |
| 39 | `0x800BAB90` | `IsSaveFinished()` | Returns 1 without a sequence, otherwise slot 10 |
| 40 | `0x800BABC0` | `GetMode()` | Returns `0xC70` |
| 41 | `0x800BABD0` | `HasPlayerWithId(u32)` | Compares a word of the first four players' data |
| 42 | `0x800B8F20` | `RegisterTeamPiiBoxCollection()` | Adds every team-box entry to the collection box |

## F-126-5: save-slot and container types

- **`SlotData::Data` tail.** The old `u8 mStatus[0x20]` is replaced by evidenced members: `mRegistryCount` (`Data` offset `0x12E0`, written from the snapshot), `mElapsedSeconds` (`0x12E4`, `OSTicksToSeconds(OSGetTime())`), twelve reserved bytes, the high word of the lead Pii identifier (`0x12F4`), its species halfword (`0x12F8`) and the low word (`0x12FC`). The halves are stored one by one because the identifier is split around the species.
- **`SessionSnapshot`.** A local struct of 16 bytes (`s64` play time, header word, registry count) followed by six `std::vector` of `SD_*::Data` in `SlotData` order. Its constructor is `0x800B3530` and its destructor `0x800B3680`.
- **`StageDesc`** is `MapIndex mIndex` (4), a `DescParamTable` (an 8-byte object with out-of-line constructor `0x801119B0`, destructor `0x801119C0`, `Reset` `0x80112530` and an indexer `0x80111D20` that `ScriptBinding_GetStageDescParam` calls) and a `vector<shared_ptr<MapDesc>>` at `0xC`. The earlier `int mParams[3]` was wrong; only `mMapDescs` was used.
- **`std::vector` iterator.** The original `vector<T>::iterator` is a class, not `T*`: `SaveToSlot` calls its copy constructor (`0x800B3A10`), `operator*` (`0x800B3A20`), `operator++` (`0x800B3FC0`) and `operator!=` (`0x800B3510`) out of line. `include/vector` gains `__vec_iterator<T>` with an implicit copy constructor (emitted out of line here), plus `reserve`, `push_back` and `swap` for non-POD element types; every other game unit keeps its objdiff percentage.

## Remaining work

- `GetSnapshot` (`0x800B29A0`) is 82.43%. The instruction sequence is the same; the original allocates `r0`, `r6` and `r5` for the start time and the 64-bit difference and delays the four stores behind the loads of the next call's arguments. Over 1,500 source variants (statement order and 64-bit expression forms, locals for the loaded fields and the box pointer, setter helpers, `Set`-style argument orders, `optimization_level`, `scheduling` and `peephole` pragmas) reached at most 88.8%. The same arithmetic in `GetPlayTime` (`0x800B9750`) matches at 100%, so only this function's register assignment differs.
- The out-of-line vector helpers in `0x800B37F0`–`0x800B5650` are emitted by this unit but belong to #127 and #128: `reserve` and `push_back` for each element type are 74% to 97% and `SD_PiiPersonalData::Data::operator=` is 58%.
- The unit stays `NonMatching` until #127 to #136 reconstruct the remaining 197 functions; the original object is linked meanwhile.

## Traps

- **An unreferenced unnamed-namespace class compiles to nothing.** `src/game/CGameSession.cpp` produced an empty object until `IGameSession::Create` instantiated `CGameSession`; members that nothing calls are dropped, so wrap them in `#pragma force_active on` until their callers (`BeginSave`, `LoadFromSlot`) are reconstructed.
- **`#pragma dont_inline on` blocks inlining of callees too.** Wrapping `InitStageDescs` to keep it out of the constructor also stopped `MakeMapIndex` from inlining inside it (a `bl MakeMapIndex__Fiiii` appeared). Write the bitfield assignments out in that function instead.
- **A destructor keeps its own vptr store only if the body calls a member function.** `~CGameSession() { sGameSession = nullptr; }` lost the `stw` of the vtable address that the original keeps; calling a (non-virtual) member that does the assignment restored it exactly.
- **The iterator type decides which vector helpers are out of line.** With `T*` iterators `SaveToSlot` scored 73%; a class iterator (implicit copy constructor, `operator*`, `operator++`, a free `operator!=`) brought it to 100% without any pragma.
- **A condition on an assignment keeps the temporary alive across the test.** `if ((lead = player.GetLeadPersonalData()))` destroys the returned `shared_ptr` after the bool conversion, matching the original; a separate `if (lead)` destroys it first.
- **A narrowing store is converted unless it goes through a local pointer.** `GetSaveData(-1)->mData.mLeadPiiMonsno = monsno;` (an `int` into a `u16` member) inserts `clrlwi` before the call that fetches the slot; `SlotData* pSlot = GetSaveData(-1); pSlot->mData.mLeadPiiMonsno = monsno;` stores the register as is, which is what the original does. Declaring `PiiSpeciesData::GetSpecies` as `u16` breaks `CEnemyInfo.cpp` instead.
- **`__vec_deleter::erase_at_end` allocates registers differently with a local copy.** After `clear()` started passing a local `n`, `~StageDesc` lost its match; copying the argument once more inside the non-POD `erase_at_end` (`size_type count = n;`) restored it without changing any other unit (BWorld's destructor gained 0.2%).
- **A declared-but-undefined inline function is not emitted.** `MakeMapIndex` (`0x800B2370`) only appears once its definition sits under `#pragma force_active on` in the `.cpp`; the header declares it without a body.
