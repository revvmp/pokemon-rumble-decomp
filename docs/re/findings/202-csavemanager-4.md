# Issue 202: CSaveManager.cpp, fourth block

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #202 lists 32 functions in `.text` `0x80109760`-`0x80109FC4`, the fourth block of the file started in [F-199-1](199-csavemanager.md) and continued in [F-200-1](200-csavemanager-2.md) and [F-201-1](201-csavemanager-3.md). No decompiler output was used.

## F-202-1: the 32 functions of #202 reconstructed

All 32 functions pair with the original at 100% in `unitdiff.py` once the vtables, `sInstance` and the callee names carry the compiled names. The `CSaveManager` constructor, the `ISaveManager` constructor, the `shared_ptr_deleter` constructor and the `function_imp` constructor of F-201-1 were 99.3% to 99.5% only because their relocations named unnamed `.data` and `.sbss`. `configure.py`, `ninja` and `verify.py` exit 0 and the linked `main.dol` is unchanged. The unit stays `NonMatching` because it spans #199 to #206.

| Address | Function |
| --- | --- |
| `0x80109760`, `0x80109780` | `shared_ptr<CLayout>` and `shared_ptr<IDlgMsgWin>` `operator bool`, reconstructed by #201 and counted here |
| `0x801097A0` | `CSaveManager::Draw`: draws the layout and the message window when each exists, the mirror of `Calc` |
| `0x80109820` | `ISaveManager::Create()`: `shared_ptr<ISaveManager>(new CSaveManager)` |
| `0x80109880` | `CSaveManager::CSaveManager()`: stores `this` in the file-static `sInstance` (`.sbss` `0x804B9070`) after the members |
| `0x801098E0`, `0x80109920` | `ISaveManager` constructor and deleting destructor (vtable `0x803AD780`) |
| `0x80109980` | `shared_ptr<CFiber>()` |
| `0x801099A0`-`0x80109B50` | `queue`, `deque`, `__deque_container`, `__deque_storage` and `__deque_imp` default constructors, then the `compressed_pair`, `CompressedPairStorage`, `allocator`, `__cdeque` and `__cdeque_pointer` constructors; the `__cdeque_buf<void*>` constructor at `0x800F8F70` is external |
| `0x80109B80`-`0x80109E20` | the destructor chain: `__cdeque_pointer`, `__cdeque`, `CompressedPairStorage`, `compressed_pair`, then `__deque_storage`, `__deque_container`, `deque` and `queue`; the `__cdeque_buf<void*>` destructor at `0x800F9070` is external |
| `0x80109E80`-`0x80109FD0` | `shared_ptr<ISaveManager>(CSaveManager*)` with its `move_ptr`, `compressed_pair`, `CompressedPairStorage` and `default_delete` constructors and the `shared_ptr_deleter` constructor |

## F-202-2: construction contracts found by the match

- **Two classes with no members sit between `deque` and `__deque_imp`.** Constructor and destructor each run five levels from `queue` to `__deque_imp`: `queue`, `deque`, two classes that add no function, then `__deque_imp`. `push_back` and `pop_front` cross no extra level, so the two classes are public bases that contribute through inheritance only. `include/deque` names them `__deque_container` and `__deque_storage`; the names are descriptive (low confidence), the shape is evidenced by the call chains at `0x801099D0`-`0x80109A30` and `0x80109D00`-`0x80109E20`.
- **`__deque_imp()` zeroes `start_` and `size_` before its members are built** (`0x80109A60`), so the constructor initialises them in its list.
- **`__cdeque_buf<void*>` has a non-trivial destructor defined elsewhere.** The destructor of `__cdeque_pointer` (`0x80109B80`) calls the external `0x800F9070`; with a trivial destructor the whole `compressed_pair` chain is not emitted. `include/deque` declares `~__cdeque_buf()` without a body, matching the external reference. Its constructor `0x800F8F70` is a one-call wrapper around `0x800F8FA0`, so `__cdeque_buf` itself probably has a base class that the header does not model yet; this block does not need it.
- **Base destructors take 0 and member destructors take -1.** `CompressedPairStorage` (`0x80109C40`) destroys its `__cdeque` member with -1 and `__cdeque` destroys its base with 0.
- **`ISaveManager::Create` mirrors `IDlgMsgWin::Create`.** The two callers (`0x800BA9B0`, `0x800BAA90`) pass a return slot and store the `shared_ptr` at offset `0xC68`. Calling it a static member of the interface is descriptive (low confidence).
- **The destructor body of `__deque_imp` is a stub.** `~__deque_imp() {}` only destroys `buf_`, which emits the `compressed_pair` chain of this block. The real body (`0x8010A150`, `0xFC` bytes, block iteration and a `deallocate(p, 0x100)` per block) belongs to #203 and is 33.76% until then. `~CSaveManager` (`0x8010A0B0`), required for the vtable, is written and matches at 99.86% (relocation names only).

## F-202-3: names given outside the block

The `ISaveManager` vtable `0x803AD780`, the `CSaveManager` vtable `0x803AD69C`, the `shared_ptr_deleter<CSaveManager>` vtable `0x803AD2C8`, the `function_imp` vtable `0x803AD380` (closing the relocation difference of F-201-1) and `sInstance` `0x804B9070`. Also `shared_ptr<IDlgMsgWin>()` `0x80095160`, the `__cdeque_buf<void*>` constructor `0x800F8F70` and destructor `0x800F9070`, `~CSaveManager` `0x8010A0B0`, `~__deque_imp` `0x8010A150`, and for `move_ptr<CSaveManager>` the `release` `0x8010A030`, `first` `0x8010A080`, `enable_from_this` `0x8010A090`, `second` `0x8010AC20`, `default_delete::operator()` `0x8010AB70` and destructor `0x8010AAE0`. The `force_active` pragmas are removed: the vtable now references every virtual slot.

`push_back(const T&, int2type<false>)` at `0x8010A260` and `pop_front(n, int2type<false>)` at `0x8010A5D0` are outside this block and stay with #203.

## Traps

- **A trivial destructor hides a whole chain.** The member and base destructors of `compressed_pair`, `CompressedPairStorage`, `__cdeque` and `__cdeque_pointer` are not emitted unless the innermost class has a non-trivial destructor. If a destructor chain is missing from the compiled object, check that its leaf has one.
- **Levels without members are still separate functions.** Under `dont_inline` two classes that carry no members still show as separate constructor and destructor functions; look for intermediate base classes instead of merging levels.
- **Shared-header changes were checked against `report.json`.** Adding the two classes, `__deque_imp()` and `~__cdeque_buf()` to `include/deque` changed no function score in `ScriptCamera` or `CMsgBalloon`, the other units that include `<deque>`.
- **`ninja` rewrites `splits.txt` ordering.** Restore it with `git checkout config/wpse-v1/splits.txt` before committing.
