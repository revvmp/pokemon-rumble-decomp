# Issues 184 to 193: CPlayerPii.cpp

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf` and the generated split assembly. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md) plus `#pragma dont_inline on` ([F-181-3](181-cplayer.md#f-181-3-the-unit-is-built-without-inlining)). No decompiler output was used. Issues #184 to #193 (`0x800F8D60` to `0x800FD4F8`) are one source file whose first function is the `CPlayerPii` constructor. This note records the file and the functions of #184, #185, #187 and #188 (`0x800F8D60` to `0x800F9B84` and `0x800FA370` to `0x800FB4D4`) and all of #186 except `GetShadowName`; later sections are added as the other blocks land.

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

- **`deque<VEC3>` is a three-class chain, `__cdeque` a four-class chain.** `deque` : `__deque_ops` : `__deque_deleter`, where `__deque_deleter` owns `start_`, `size_` and the `compressed_pair<buf_type, Allocator>` and has the user destructor `0x800F9C80`. The block-pointer buffer is `__cdeque<T*>` : `__cdeque_pointer<T>` : `__cdeque_imp<void*>` : `__cdeque_buf<void*>`; `__cdeque_buf` owns `data_`, `start_`, `size_`, a `compressed_pair<size_type, Allocator>` and the destructor `0x800FA370`. The PS2 layout in the public `KidWizardOfTheWeb/SR2` header (`__cdeque_deleter_common`, `__cdeque_deleter`, `__cdeque_imp`, `cdeque`; `__deque_deleter_common`, `__deque_deleter`, `__deque_imp`, `deque`) gave the idea; the Wii names and the position of the destructor bodies come from the extab records and call chains here.
- **Constructor chain.** `0x800F8DF0` to `0x800F9060` is `deque()` down to `allocator<void*>()`, in caller-first order. `__cdeque_buf()` is `data_(0), start_(0), size_(0), capacity_(0)`.
- **Destructor chain.** Out-of-class destructor bodies are emitted after the in-class chain, and the implicit destructors above them appear before `~CPlayerPii`, bottom-up: `0x800F9070` to `0x800F92B0`, then `0x800F9310`. `~__cdeque_buf` and `~__deque_deleter` are defined outside their classes and land at `0x800FA370` and `0x800F9C80`.
- **Two deque flavours, chosen by `__builtin_is_pod`.** `deque<WorkType>` in `CSaveManager.cpp` (`WorkType` is an enum, so POD) has one more class level in its constructor and destructor chains (`__deque_container`, `__deque_storage`, then the owner `__deque_imp`), one more `push_back` forwarder, and a single buffer accessor shared by the destructor and `first()`; `deque<VEC3>` (`VEC3` has a copy constructor, so not POD) has `deque`, `__deque_ops` and the owner `__deque_deleter`, `push_back` crosses one forwarder, and the destructor, `allocate_back` and `pop_front` use accessors (`0x800F9D80`, `0x800F9D90`) that differ from `__deque_ops::buf` (`0x800FCD50`). `CW`'s `__builtin_is_pod` is true for the enum and false for `VEC3` (checked with the pinned compiler); the real MSL `Metrowerks::is_POD` is `__builtin_is_pod`, while `include/MSL_CPP/msl_utility.h` still lists types. `include/deque` therefore keeps the `CSaveManager` classes for POD types and adds `__deque_ops` and `__deque_deleter` for the others, selected by `__deque_select<T, Allocator, __builtin_is_pod(T)>`. The class names are descriptive.
- **Non-POD element path.** The unit uses the `int2type<false>` paths (placement construction through the `VEC3` copy constructor, `0x800FAC50`).
- **A temporary buffer has a reference allocator.** `0x800FBF70` builds a `__cdeque_buf` whose pair stores `Allocator&`; its accessors (`0x800FBF20`, `0x800FC060`) and destructor (`0x800FCEA0`) are a second instantiation, `__cdeque_buf<void*, allocator<void*>&>`.

## F-184-4: the ring-buffer layer and its iterators

`__cdeque_buf<void*>` is a ring buffer of block pointers: `data_`, `start_`, `size_` and a `compressed_pair` holding the capacity. The capacity field counts one spare slot, so `capacity()` is `cap() == 0 ? 0 : cap() - 1` (`0x800FA420`); `allocate(n)` stores `n + 1` (`0x800FBEC0` chain, `0x800FCF30`/`0x800FD240`). The growth policy `recommend(n)` (`0x800FA8B0`) returns `cur + max(3 * (cur + 1) / 5, n)` below a third of `max_size()`, `cur + max((cur + 1) / 2, n)` below two thirds, otherwise `max_size()`; `max_size()` is a function-local static `min(alloc().max_size(), numeric_limits<long>::max() / 2) - 1` (`.sbss`). `allocator<void*>::max_size` calls `numeric_limits<unsigned long>::max()` (`0x80087B10`), which is why `include/limits` exists and `MSL_CPP/allocator.h` uses it.

- **Iterator.** Four pointers in the order `cur_`, `first_` (= `data_`), `begin_` (= `data_ + start_`) and `last_` (= `data_ + capacity`). `++` steps and wraps from `last_` to `first_`; `==` compares `cur_`; `+=` wraps modulo the capacity (`0x800FD3C0`); the difference of two iterators corrects by the capacity when exactly one is before `begin_` (`distance_to`, `0x800FD410`, behind a one-instruction `operator-` forwarder `0x800FB4D0`). `__cdeque_pointer::iterator` forwards each operation through its base; `void_iterator` adds one more forwarder per comparison (`0x800FB420`-`0x800FB470`). Initializing `begin_` from `first_ + start_` rather than from `data_` again matters (`0x800FAB10`).
- **Temporary buffers.** `reserve` (`0x800FA470`), `push_back` (`0x800FA680`) and `insert` build a `__cdeque_buf<void*, allocator<void*>&>` (the allocator member is a reference, `0x800FBF70`), fill it with `construct_at_end` / `construct_at_front`, `clear_moved()` the old contents, and then swap `data_`, `start_`, `size_` and `cap()` one by one with `std::swap`; argument lists are evaluated right to left (`swap(cap(), tmp.cap())` calls `tmp.cap()` first). `push_back` in the full case sets `tmp.start_ = size_` before constructing the new element at `tmp.data_[tmp.start_]`.
- **Const chain.** `deque<VEC3>::operator[]` is const in this build: `ScriptCamera.cpp` reaches `0x80134B30`-`0x80134BD0`, whose last function calls the const `cap()` (`0x800FC080`); `CPlayerPii::GetPositionHistory` therefore returns a const reference and the whole chain (`deque`, `__deque_ops`, `__cdeque`, `__cdeque_pointer`, `__cdeque_buf`) has const `operator[]` overloads.
- **Accessors.** `__deque_ops::buf() const` (`0x800FCD50`) and the non-const `buf()` (`0x800F9D80`) are two different leaves of `CompressedPairStorage::first`; the non-const one serves `~__deque_ops`, `allocate_back`, `pop_front` and the deque iterator, the const one `capacity() const` and `operator[]`.
- **`allocate_back` has no `count` or `limit` local.** With `size_type count = blocks * block_size` and `size_type limit = rBuf.capacity()` the compiler puts `count` in `r26` instead of `r27`; the product is written twice and the capacity compared directly (`0x800F9E70`).

## Source changes

`src/game/CPlayerPii.cpp` is new. `include/game/CPlayerPii.h` holds the typed class; `include/game/CPlayer.h` gains `GetPlayerNo`; `include/game/IGameMap.h` declares `GetActiveGameMap`. `include/deque` adds the non-POD flavour (`__deque_deleter`, `__deque_ops`, `__deque_select`), the `__cdeque_imp` layer, the `__cdeque_buf` constructor, `clear` and destructor, and keeps the POD classes of `CSaveManager.cpp` unchanged. `config/wpse-v1/symbols.txt` names the 56 functions of the range after the mangled names the source produces.

## Acceptance

`configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with `game/CPlayerPii.cpp` as `NonMatching`. Every function of #184, #185, #187 and #188 is 100% in objdiff apart from relocation names of callees in later blocks, which carry the compiled names; #186 differs only in `GetShadowName` (scheduling of the `mpPlayer` load, 88.86%, open). No function of another unit lost its score in `report.json`; the `ScriptCamera` deque functions were renamed to their const forms.

## Traps

- **Duplicate renames link-fail.** Giving two target addresses the same mangled name (two identical 4-byte forwarders or iterator helpers) makes `mwldeppc` report `multiply-defined` in the split object. Check `symbols.txt` for repeated names after bulk renames.
- **Declaration order in a template class does not decide the emission order of its out-of-class members.** The destructor, `push_back`, `pop_front` and `allocate_back` of `__deque_ops` are defined outside the class, but their callee order in the original did not follow the declaration order tried first; per-function matching does not depend on it, the final link will.

- **Renamed placeholders look like regressions in `report.json`.** Renaming an unmatched `fn_` placeholder (for example `0x80097810`) to its mangled name changes the report key; compare scores per address when checking other units.
- **The same wrapper can have different depth in two instantiations.** `push_back` is one forwarder deep for `deque<VEC3>` and two for `deque<WorkType>`, and the constructor chain differs by a class; the cause is the element type's POD-ness, not the unit. Compare the element types before reusing a layer count.
