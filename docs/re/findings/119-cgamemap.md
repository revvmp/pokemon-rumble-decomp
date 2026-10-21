# CGameMap.cpp unit, class layout and first block (#119)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical with the split registered. Issue #119 lists 23 functions in `.text` `0x800A7650`-`0x800A98D4`. Neighbouring blocks: #117 and #118 (`CGameEnding`, preceding), #120 to #122 (the rest of this file), #123 onward (`CGameResult`).

## F-119-1: unit boundaries of `game/CGameMap.cpp`

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80016838`-`0x800186BC` | Records named by the first entry of the unit (`0x800A7760`) and the first entry of `CGameResult` (`0x800AC4B0`) |
| `extabindex` | `0x80047D3C`-`0x80048084` | 71 entries, `0x800A7760` to `0x800AC140` |
| `.text` | `0x800A7650`-`0x800AC4A8` | First function is the constructor (stores the `CGameMap` vtable `0x803A23F8`); last is the sorted-dialog helper `0x800AC140` ending at `0x800AC4A8`; `0x800AC4B0` is the first function to reference `gameresult` (`0x803A2938`) |
| `.data` | `0x803A2150`-`0x803A2938` | From the `5:/stage/` path to the `exception` string; `0x803A2938` starts the `CGameResult` strings |
| `.sdata` | `0x804B6790`-`0x804B67D0` | `%02d`, `EX`, `light`, `t_`, `/`, then the type-info pointers of the deleter classes; `time` at `0x804B67D0` is the next file's |
| `.sdata2` | `0x804B9E70`-`0x804B9E78` | One float, `1.0f`, read by `Initialize` |

- **No `.ctors`, `.bss` or `.sbss`.** The unit has no static initializer; the `.rodata` tables read by `LoadStageResources` (`0x8035B9D0` ranks, `0x8035B9F8` worlds) belong to the stage-name file.
- **Start.** `0x800A7650` is the constructor: it has no exception entry, matching F-116-1's account of the end of `CGameEnding.cpp`.
- **Deleter vtables.** `shared_ptr_deleter<BWorld>` (`0x803A19BC`) and `<CShadowCamera>` (`0x803A1A30`) already exist in `CGameEnding.cpp`'s `.data`; this unit references them but defines only `<CGameMap>` (`0x803A21D4`) and `<CDividedGround>` (`0x803A2374`). `-ipa file` merges the identical weak vtables across files.

## F-119-2: `@unnamed@CGameMap_cpp@::CGameMap` layout

RTTI `0x803A24AC`; base `IGameMap` (which holds `ICalcDraw` at 0 and `enable_shared_from_this<IGameMap>` at 4). Size `0xD8` (`new` of 216 bytes in `IGameMap::Create`). The class is declared in `src/game/CGameMap.cpp`, in an anonymous namespace, as `CGameEnding` and `CDlgPiiSelector` are; the interface is `include/game/IGameMap.h`.

| Offset | Member | Type | Evidence |
| --- | --- | --- | --- |
| `0xC` | `mParam` | `IGameMap::Param` | Copied from the constructor argument; `~Param` at `0x800A7760` is the exception cleanup of the constructor and of `Create` |
| `0xC` | `mParam.mMapDesc` | `MapDesc*` | `GetMapDesc` (`0x800A8010`) returns it |
| `0x10` | `mParam.mEnvironment` | `shared_ptr<NPpu::CEnvironment>` | Passed to `CPpuEngine::SetDefaultEnvironment` by `Calc` and `RunAction` |
| `0x18` | `mPausing` | `bool` | Starts true; `SetPausing` (`0x800AA420`) stores it, `IsPausing` returns it when no selector is shown |
| `0x1C` | `mCurrentDialog` | `weak_ptr<IDlg>` | `GetCurrentDialog` locks it; `RegisterDialog` and `UnregisterDialog` assign and clear it |
| `0x24` | `mTimerFrame` | `int` | `StartTimer`, `AddTimer`, `GetTimerFrame` |
| `0x28` | `mTimerStep` | `int` | Added to `mTimerFrame` by `Calc` while not pausing; `StopTimer` zeroes it, `IsTimerWorking` tests it |
| `0x2C` | `mHitstopFrames` | `int` | `Hitstop` keeps the larger value; `Calc` decrements it |
| `0x30` | `mModelObjsHidden` | `bool` | Set by `DisappearModelObjs`, cleared by `AppearModelObjs`; `CalcView` and `Draw` skip the stat display while set |
| `0x34` | `mHiddenObjIDs` | `vector<u32>` | Object handles; `AppearModelObjs` passes each to `BWorld::GetObject` and calls virtual slot 8 with true |
| `0x40`-`0xD0` | 19 `shared_ptr<I...>` | see below | One 8-byte pair each |

The 19 shared pointers, with the getter slot that returns each (the vtable getters `0x800AA4A0`-`0x800AA5C0`):

| Offset | Member | Getter slot |
| --- | --- | --- |
| `0x40` | `mWorld` (`BWorld`) | 7 `GetWorld` |
| `0x48` | `mPostEffect` | 25 |
| `0x50` | `mWorldObjStatDisplay` | 8 |
| `0x58` | `mMiniMap` | 9 |
| `0x60` | `mDlgPiiSelector` | 10 |
| `0x68` | `mDlgPiiList` | 11 |
| `0x70` | `mDlgCollection` | 12 |
| `0x78` | `mDlgInfo` | 13 |
| `0x80` | `mMoneyLifeDisplay` | 16 |
| `0x88` | `mBattleRoyalDisplay` | 17 |
| `0x90` | `mComboDisplay` | 18 |
| `0x98` | `mNoticeDisplay` | 15 |
| `0xA0` | `mLayoutManager` | 19 |
| `0xA8` | `mDlgMsgWin` | 14 |
| `0xB0` | `mPauseMenu` | 20 |
| `0xB8` | `mTutorialMsg` | 21 |
| `0xC0` | `mMsgBalloon` | 22 |
| `0xC8` | `mEnemyInfo` | 23 |
| `0xD0` | `mNumericInput` | 24 |

Slot numbers count the vtable entries after the two header words (`0x803A2400` is slot 0). The getter bodies read these offsets directly, which fixes the pairing of every `IGameMap` getter slot with its member; the names in `IGameMap.h` agree with all 19. The remaining slots: 26 `GetCurrentDialog` (`0x800AA430`), 27 `RegisterDialog` (`0x800A99E0`), 28 `UnregisterDialog` (`0x800A9B60`), 29 `SetPausing`, 30 `IsPausing` (`0x800A7F70`), 31 `Hitstop`, 32 `IsHitstopping` (world flag `0x8`), 33 `EnterEvent` (`BWorld::RequestPause`), 34 `ExitEvent` (`RequestResume`), 35 `IsInEvent` (world flag `0x40`), 36 `StartTimer`, 37 `AddTimer` (also calls slot 7 of the `IBattleRoyalDisplay`, vtable offset `0x24`), 38 `StopTimer`, 39 `IsTimerWorking`, 40 `GetTimerFrame`, 41 `DisappearModelObjs`, 42 `AppearModelObjs`.

Every object in the table derives `ICalcDraw`: `Calc`, `CalcMtx`, `CalcView` and `Draw` call vtable offsets `0xC`, `0x10`, `0x14` and `0x18` on all of them. The ten interfaces that had no header (`IWorldObjStatDisplay`, `IMiniMap`, `IDlgCollection`, `IDlgInfo`, `IMoneyLifeDisplay`, `IBattleRoyalDisplay`, `ILayoutManager`, `ITutorialMsg`, `INumericInput`, `IPostEffect`) now have one that states only that base and the creator the constructor uses.

## F-119-3: the 23 functions of #119

100% means objdiff 100% against the object from `splits.txt`.

| Address | Size | Identity | Match |
| --- | --- | --- | --- |
| `0x800A7650` | `0x110` | `CGameMap::CGameMap(const Param&)` | 100% |
| `0x800A7760` | `0x8C` | `IGameMap::Param::~Param()` (deleting form, constructor cleanup) | 100% |
| `0x800A77F0` | `0x7C` | `vector<unsigned long>::~vector()` (constructor cleanup) | 100% |
| `0x800A7870` | `0x40C` | `CGameMap::~CGameMap()` | 98.8%: the target stores the `CGameMap` vtable pointer before destroying members, the rebuild does not |
| `0x800A7C80` | `0x2DC` | `CGameMap::Calc()` | 100% |
| `0x800A7F60` | `0x8` | `shared_ptr<BWorld>::operator*() const` | 100% |
| `0x800A7F70` | `0x94` | `CGameMap::IsPausing()` | 100% |
| `0x800A8010` | `0x8` | `CGameMap::GetMapDesc() const` | 100% |
| `0x800A8020` | `0x1D4` | `CGameMap::CalcMtx()` | 100% |
| `0x800A8200` | `0x1E0` | `CGameMap::CalcView()` | 100% |
| `0x800A83E0` | `0x1CC` | `CGameMap::Draw()` | 100% |
| `0x800A85B0` | `0xC4` | `GetStageNumber(MapDesc*)` | 100% |
| `0x800A8680` | `0x58C` | `CGameMap::Initialize()` | 100% |
| `0x800A8C10` | `0x8` | `shared_ptr<BWorld>::get() const` | 100% |
| `0x800A8C20` | `0x28` | `shared_ptr<BWorldObj>` converting constructor from `shared_ptr<CDividedGround>` | 100% |
| `0x800A8C50` | `0xDC` | `shared_ptr<CDividedGround>::shared_ptr(CDividedGround*)` | 100% |
| `0x800A8D30` | `0x28` | `shared_ptr<BWorldCamera>` converting constructor from `shared_ptr<CShadowCamera>` | 100% |
| `0x800A8D60` | `0x124` | `shared_ptr<CShadowCamera>::reset(CShadowCamera*)` | 100% |
| `0x800A8E90` | `0x124` | `shared_ptr<BWorld>::reset(BWorld*)` | 100% |
| `0x800A8FC0` | `0x10` | `shared_ptr<CShadowCamera>::shared_ptr()` | 100% |
| `0x800A8FD0` | `0x8` | `shared_ptr<CShadowCamera>::operator*() const` | 100% |
| `0x800A8FE0` | `0x8` | `shared_ptr<CDividedGround>::operator*() const` | 100% |
| `0x800A8FF0` | `0x8E4` | `CGameMap::LoadStageResources()` | 85%: same calls and control flow, different stack slots (F-119-5) |

Also at 100% outside the block, because the constructor, `Initialize` and the factory emit them: `IGameMap::Create` (`0x800AA140`, builds the object and calls `Initialize`) and the `shared_ptr<I...>::operator=` and destructor pairs of the 19 member types between `0x800AA6D0` and `0x800ABBB0`, renamed from `fn_` and `dtor_`. #120 to #122 own the rest.

`IGameMap::Create` is called from `0x800BCFA0` with a hidden result and a stack `Param` holding the map descriptor from `IGameSession` slot 14 `GetMapDesc` and a copy of the caller's environment `shared_ptr`.

## F-119-4: what the rebuild needed

- **`vector<unsigned long>` has four destructor levels, not three.** The pointer vector in `include/vector` already routes through `__vec_pointer`; the target `0x800A77F0` shows the same extra level for `unsigned long`. A `vector<unsigned long>` specialization over `__vec_integer` reproduces it (100%).
- **A member holding the constructor argument.** `~CGameMap` destroys one object at `0xC` whose `shared_ptr` sits at `+4` and releases it with the strong count (`release`), which identified `mParam` as the whole `Param` and `0x800A7760` as `~Param`.
- **`IDlgPiiList::Create` takes a bool.** The call in `Initialize` passes `1` in `r4`, and `0x800896E0` ignores it; the declaration is `Create(bool)` with an unnamed parameter in `CDlgPiiList.cpp`. The symbol is `Create__11IDlgPiiListFb`.
- **Creators.** The `Create` functions declared in `include/game/I*.h` and the `symbols.txt` renames of `0x8006B470`, `0x800776B0`, `0x8007E5D0`, `0x800C1220`, `0x800C26D0`, `0x800C4960`, `0x800D6790`, `0x800D83B0`, `0x800FD500`, `0x8011AB90` and `0x80121340` follow the signatures the call sites pass (`f1` is `1.0f` for the mini map; the pause menu takes the map descriptor and two bools). Names are descriptive, medium confidence: `CEnemyInfo::Create`, `CreateComboDisplay`, `IMsgBalloon::Create`, `IDlgPiiSelector::Create`, `IDlgMsgWin::Create` and `CreateNoticeDisplay` were already established.
- **`BeginHoldCollection` and `EndHoldCollection` on `CPlayer`.** `0x800F8540` sets bit 1 of the player's flags word at `+4` and caches a child pointer at `+0x6C`; `0x800F85A0` clears that bit. `Calc` calls the first when hitstop starts and the second after the world update; `Initialize` calls the second for every player. Medium confidence for the names.
- **`IsBattleRoyalFirstPart`** (`0x801251C0`) returns true for `mMap < 3` (signed); name from the CPauseMenu unit (F-167). `GetStageNumber` reads parameter 0 of the map descriptor's `CPpuRegister` (`MapDesc` `+4`, `GetValue` `0x80111D20`); for mode 2 world 7 it first redirects to the descriptor of the mirrored map.
- **`CShadowCamera::Setup`** (`0x8010C5A0`) stores the level count at `0x68` and the alpha mode at `0xF0` and allocates the three capture textures; `CDividedGround::Load` (`0x80072340`) is F-61's ground-loading entry point.

## F-119-5: what blocks the rest

- **`LoadStageResources` (`0x800A8FF0`, `0x8E4` bytes) stack layout.** The reconstruction makes the same calls in the same order (world and rank names from `gStageWorlds[mStage]` and `gStageRanks[mArea]`, `FormatString("%02d")`, three `light...` names, `LoadFile` of `5:/stage/<world>/<nn>.arc/stage.brres`, then `LoadResourceFile` of `<world>/common.arc/stage.brres`, `ResFile::GetResAnmScn`, `BWorld::LoadLightScene` `0x8005EB70`, `CShadowCamera::SelectLight`) and matches 85%. The target allocates ten distinct 12-byte slots for the `operator+` temporaries (frame `0x230`, saves `r23`-`r31`); the rebuild shares slots between statements (frame `0x1A0`, saves `r24`-`r31`). Naming the intermediate strings, nesting them in blocks, binding references to `world` and `rank`, and building the path inside the call did not change the frame. `lights[i].assign(...)` is required: `operator=` produces a different copy.
- **`~CGameMap` vtable store.** Making the destructor explicit, out-of-line, `inline`, or calling a virtual in its body did not make CodeWarrior store the vtable pointer first. `CGameEnding`'s destructor shows the same inlined base destructors without the store, so the trigger is something specific to `CGameMap`; the remaining difference is three instructions.
- **`Initialize` and the save slot.** The stores to `SlotData` `+0x16`, `+0x17`, `+0x12FA` and `+0x12FB` only match when the destination bytes are signed: `SlotData::Data::mFlags` is `s8[4]` and the two bytes after `mLeadPiiMonsno` (the padding before `mLeadPiiIdLow`) are `s8 mLastArea[2]`, indexed by mode 0 and 1. A `switch (mode)` reproduces the branch layout. The type change of `mFlags` leaves every other unit's objdiff counts and the DOL verification unchanged.
- **Slots 31-42 and the rest of the class** (`Hitstop`, `AddTimer`, `RunAction`, the dialog stack, `DisappearModelObjs`, `AppearModelObjs`) are declared in `CGameMap.cpp` but defined by #120 to #122. `AddTimer` needs slot 7 of `IBattleRoyalDisplay`, whose three own virtuals are unnamed.

## Method

GNU objdump 2.42 of `build/wpse-v1/main.elf`, Python scans of `extabindex`, vtables, `.data` and `.sdata` references and `r13`/`r2` offsets, the dtk-generated assembly, and `tools/re/unitdiff.py`. No decompiler output was used.

## Traps

- **A weak function can be the constructor's cleanup of a parameter object.** `0x800A7760` first looked like `~enable_shared_from_this` and `0x800A77F0` like a generic vector destructor. Both are referenced only from `extab` cleanup actions of the constructor, so they are the destructors of the `Param` argument copy and of the vector member; identify them from the member the cleanup action destroys.
- **A standalone destructor with one fewer `beq` means a missing base level.** Count the `this != 0` branches between the prologue and the first load: each inlined base destructor adds one. `vector<unsigned long>` needed the same extra level as `vector<T*>`.
- **`cntlzw` after a bool test is `!x` stored into a bit field.** `GetStageNumber` writes `index.mMode = !IsBattleRoyalFirstPart(index)` and `index.mMap = (index.mMap - 1) % 2 + 1`; the `xor` and `subf` pair is the signed remainder.
- **By-value `MapIndex` arguments live at two stack slots.** The by-value copy and the `const MapIndex&` temporary swap places with the order the temporaries are created; a named local declared early did not reproduce the target order.
- **Another agent's unit may already have renamed a symbol you need.** `0x80125180` arrived as `IsStage7EvenMap` on a rebase; run `git pull --rebase` before renaming callees in `symbols.txt`.
