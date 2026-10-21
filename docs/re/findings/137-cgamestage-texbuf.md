# Issue 137: CGameStage, HUD and layout manager (1/5), `0x800BCB40`-`0x800BE9D8`

Target `wpse-v1`, analysed with `tools/re/unitdiff.py` under `cflags_game` and the full `ninja` plus `tools/verify/verify.py` build.

## F-137-1: unit boundaries

- **`CGameStage.cpp` starts at `0x800BCAA0`, not `0x800BCB40`.** The NonCopyable destructor `0x800BCAA0` and the SoundHandle destructor `0x800BCAE0` are weak functions kept in this unit. Starting at `0x800BCB40` leaves a retained local copy and shifts the DOL by 0x40.
- `game/CGameStage.cpp`: extab `0x8001B840`-`0x8001BC8C`, extabindex `0x800487D4`-`0x800488A0`, `.text` `0x800BCAA0`-`0x800BD9C0`, `.data` `0x803A4088`-`0x803A4260`, `.sdata` `0x804B6918`-`0x804B6940`, `.sdata2` `0x804B9EF8`-`0x804B9F00`.
- `game/CGloballyReusableTexBuf.cpp`: extab `0x8001BC8C`-`0x8001BFC0`, extabindex `0x800488A0`-`0x80048930`, `.text` `0x800BD9C0`-`0x800BE9D8`, `.ctors` `0x803529D0`-`0x803529D4`, `.data` `0x803A4260`-`0x803A4440`, `.bss` `0x803E6600`-`0x803E6610` (destructor chain object), `.sdata` `0x804B6940`-`0x804B6960`, `.sbss` `0x804B9008`-`0x804B9010`, `.sdata2` `0x804B9F00`-`0x804B9F08`.

## F-137-2: status

`CGameStage.cpp` is Matching: every function is 100% and the full frozen DOL verifies. `CGloballyReusableTexBuf.cpp` is NonMatching. Twelve of fifteen functions are 100%. `InitializeWorkBuffer` is 99.03% (stack numbering of EH flag bytes, r5/r6 swap). `dispose<Work>`, the `move_ptr<Work>` destructor and the `__vec_split_buffer` destructor differ only by swapped loop registers in the inlined element-destroy loop. The extab of `InitializeWorkBuffer` and of the split-buffer destructor each carry one extra `DestroyBase(__vec_deleter)` action (0x1EC vs 0x1E0 and 0x70 vs 0x64 bytes), so the DOL extab grows by 0x20. Weak-function emission order already equals the original. Remaining work: find the `__vec_split_buffer`/`__vec_deleter` constructor form in `include/vector` whose EH chain has two actions.

## Traps

### F-137-3: destructor vptr store needs an inlined call

CW drops the vptr store in a destructor unless the body contains an inlined call (even an empty one). `~CGameStage` keeps it with an empty private `Release()`; the original statement is unknown.

### F-137-4: by-value MapIndex and RequestState switch

`MapIndex` (4-byte bitfield) by value is passed through a pointer to a caller copy, which prevents load hoisting in `ChangeMap`. The `RequestState` switch matches only with `default` written first.

### F-137-5: out-of-line constructor and holder destructor placement

An out-of-line constructor in an anonymous namespace that is fully inlined and dropped still emits its EH-required weak destructors early. `shared_ptr_holder::~shared_ptr_holder` must be defined out of class in `include/memory` to match the weak emission order.

### F-137-6: include order and late inline definitions

Include order of `IGameMap.h`, `IGameResult.h`, `IGameEnding.h`, `IMultiRanking.h`, `IGameSession.h` decides the order of weak `shared_ptr` destructors. `WorkBuffer::Allocate` must be defined after the other functions in `CGloballyReusableTexBuf.cpp`.

### F-137-7: shared vector header

Experiments with `include/vector` (`append` goto form with `grow_in_place`, `clear()` calling `erase_at_end(n, pod_type())`) reached 100% on `dispose<Work>` in some variants but regressed CDividedGround and conflicted with concurrent edits, so they are not included. With the mainline `include/vector`, InitializeWorkBuffer is 95.07% and the unit stays NonMatching. `CGameStage.cpp` does not depend on the vector change.
