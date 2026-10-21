# Issues 184 to 193: CPlayerPii.cpp

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf` and the generated split assembly. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md) plus `#pragma dont_inline on` ([F-181-3](181-cplayer.md#f-181-3-the-unit-is-built-without-inlining)). No decompiler output was used. Issues #184 to #193 (`0x800F8D60` to `0x800FD4F8`) are one source file whose first function is the `CPlayerPii` constructor. This note records the file and the functions of #184 and #185 (`0x800F8D60` to `0x800F9B84`); later sections are added as the other blocks land.

## F-184-1: unit boundaries

| Section | Range | Evidence |
| --- | --- | --- |
| `.text` | `0x800F8D60`-`0x800FD4F8` | `0x800F8D5C` ends `CPlayer.cpp` (F-181-1); `0x800FD500` is the first function that uses `"5:/post_effect.arc"` |
| `extab` | `0x80026418`-`0x800269D4` | Records of the first function and of `0x800FD490` |
| `extabindex` | `0x8004AA84`-`0x8004B108` | Entries `0x800F8D60` and `0x800FD490`; the next entry is `0x800FD500` |
| `.rodata` | `0x80352E90`-`0x80352EA0` | The table of four paths copied by `GetShadowName` |
| `.data` | `0x803A93C8`-`0x803A9588` | `"minimap_player"`, `"CPlayerPii.pkc"`, the four `"5:/misc.arc/pN.brres"` paths, the `CPlayerPii` vtable `0x803A9448`, its type name and base list, the `deque` length-error strings; `"5:/post_effect.arc"` at `0x803A9588` starts the next file |
| `.sdata` | `0x804B7120`-`0x804B7128` | `__RTTI__10CPlayerPii` |
| `.sbss` | `0x804B9030`-`0x804B9048` | Three function-local statics (a value and a guard byte each) read by the `max_size` functions at `0x800FBDA0`, `0x800FC190` and `0x800FCFA0` |

There is no `.ctors` entry (`0x803529DC` is `CPpuEngine.cpp`'s). The range spans #184 to #193 and was claimed as one unit. Full DOL verification passes with the split and the original object linked.

## F-184-2: CPlayerPii

`CPlayerPii` derives from `CPii` and is 0x168 bytes. The word at 0x14C is a `CPlayer*`, not a player number as F-235 first assumed: `Initialize` passes it as `this` to `CPlayer::SetPii`, and reads the player number through `CPlayer`'s word at offset 8 (`GetPlayerNo`, `0x800F9510`). The `deque<VEC3>` of positions is at 0x150. The constructor ORs 0x80 into the type, so `CPlayerPii::TYPE` is `1 << 7`.

| Address | Function | Evidence |
| --- | --- | --- |
| `0x800F8D60` | constructor `(CPlayer*, shared_ptr<CPiiPersonalData>, u32)` | the only caller is the allocation in the script binding `NewPlayerPii` |
| `0x800F9310` | destructor | vtable slot 0; the secondary thunk `0x800FD4F0` subtracts 0x48 |
| `0x800F9380` | `Initialize(bool)` | registers the Pii with the player, calls virtual `SetSizeMode(1)` (vtable offset 0x78), creates the `CHPBar` (type `5 - playerNo`, `Initialize(playerNo, ...)`) and the `CMiniMapMarker` (type 5, `"minimap_player"`) |
| `0x800F9A20` | `LoadScript(int, const shared_ptr<CEnvironment>&)` | calls `CPii::LoadScript("CPlayerPii.pkc", ...)` after an empty member that receives `get()`; the empty member is `SetEnvironment` (descriptive, low) |
| `0x800F9AB0` | `Calc` | `CPii::Calc`, then drops the oldest position when the history holds 120, then appends `GetPositionRef()` |
| `0x800F9BC0` | `GetShadowName` | indexes a local copy of the four `"5:/misc.arc/pN.brres"` paths by player number |
| `0x800F9C20` | `IsVisible` | `BWorldObj::IsVisible()` and a map from `0x800AA2F0` that is not pausing (`IGameMap` offset 0x80) |

`0x800AA2F0` is byte-identical to `GetCurrentGameMap` (`0x800AA2B0`) but is a separate function with its own caller; the descriptive name is `GetActiveGameMap` (low). `0x800C7F90` and `0x800C7FC0` are `BWorldObj::IsVisible` and `GetStateFlags`, `0x80097810` is `shared_ptr<CPiiPersonalData>::get`.

`CPlayerPii::GetShadowName` is not yet exact: the scheduling of the `this->mpPlayer` load differs (88.38%). It belongs to #186.

## F-184-3: the MSL deque, constructor and destructor chains

The position history is an MSL `deque<VEC3>`. The unit instantiates the whole template, about 250 functions. The first two blocks fix its class structure.

- **`deque` is a three-class chain, `__cdeque` a four-class chain.** `deque` : `__deque_imp` : `__deque_deleter`, where `__deque_deleter` owns `start_`, `size_` and the `compressed_pair<buf_type, Allocator>` and has the user destructor `0x800F9C80`. The block-pointer buffer is `__cdeque<T*>` : `__cdeque_pointer<T>` : `__cdeque_imp<void*>` : `__cdeque_buf<void*>`; `__cdeque_buf` owns `data_`, `start_`, `size_`, a `compressed_pair<size_type, Allocator>` and the destructor `0x800FA370`. The PS2 layout in the public `KidWizardOfTheWeb/SR2` header (`__cdeque_deleter_common`, `__cdeque_deleter`, `__cdeque_imp`, `cdeque`; `__deque_deleter_common`, `__deque_deleter`, `__deque_imp`, `deque`) gave the idea; the Wii names and the position of the destructor bodies come from the extab records and call chains here.
- **Constructor chain.** `0x800F8DF0` to `0x800F9060` is `deque()` down to `allocator<void*>()`, in caller-first order. `__cdeque_buf()` is `data_(0), start_(0), size_(0), capacity_(0)`.
- **Destructor chain.** Out-of-class destructor bodies are emitted after the in-class chain, and the implicit destructors above them appear before `~CPlayerPii`, bottom-up: `0x800F9070` to `0x800F92B0`, then `0x800F9310`. `~__cdeque_buf` and `~__deque_deleter` are defined outside their classes and land at `0x800FA370` and `0x800F9C80`.
- **`Calc` reaches `append`.** `Calc` calls a one-instruction forwarder that tail-calls the dispatcher with the `int2type<false>` tag, two functions, while `queue::push` in `CSaveManager.cpp` goes through four. `deque::append` is therefore public in `include/deque` and `CPlayerPii::Calc` uses it. `deque::push_back` and `__deque_imp::push_back` keep the `CSaveManager` chain intact. This is a modelling choice: `append` is not a standard member and no other caller exists.
- **Not POD.** `is_POD<VEC3>` is false, so the unit uses the `int2type<false>` paths (placement construction through the `VEC3` copy constructor, `0x800FAC50`).
- **A temporary buffer has a reference allocator.** `0x800FBF70` builds a `__cdeque_buf` whose pair stores `Allocator&`; its accessors (`0x800FBF20`, `0x800FC060`) and destructor (`0x800FCEA0`) are a second instantiation, `__cdeque_buf<void*, allocator<void*>&>`.

## Source changes

`src/game/CPlayerPii.cpp` is new. `include/game/CPlayerPii.h` holds the typed class; `include/game/CPlayer.h` gains `GetPlayerNo`; `include/game/IGameMap.h` declares `GetActiveGameMap`. `include/deque` splits the owner of the data from the algorithms (`__deque_deleter`, `__cdeque_imp`), adds the destructors and `append`. `config/wpse-v1/symbols.txt` names the 56 functions of the range after the mangled names the source produces.

## Acceptance

`configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with `game/CPlayerPii.cpp` as `NonMatching`. All 56 functions of #184 and #185 are 100% in `report.json`, and no function of another unit lost its score.

## Traps

- **Renamed placeholders look like regressions in `report.json`.** Renaming an unmatched `fn_` placeholder (for example `0x80097810`) to its mangled name changes the report key; compare scores per address when checking other units.
- **The same wrapper can have different depth in two units.** `push_back` is one forwarder deep here and two in `CSaveManager.cpp`; do not copy a layer count between instantiations without checking every caller.
