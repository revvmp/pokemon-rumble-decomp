# Issue 137: CGameStage, HUD and layout manager (1/5), `0x800BCB40`-`0x800BE9D8`

Target `wpse-v1`, analysed with `tools/re/unitdiff.py` under `cflags_game` and the full `ninja` plus `tools/verify/verify.py` build.

## F-137-1: unit boundaries

- **`CGameStage.cpp` starts at `0x800BCAA0`, not `0x800BCB40`.** The NonCopyable destructor `0x800BCAA0` and the SoundHandle destructor `0x800BCAE0` are weak functions kept in this unit. Starting at `0x800BCB40` leaves a retained local copy and shifts the DOL by 0x40.
- `game/CGameStage.cpp`: extab `0x8001B840`-`0x8001BC8C`, extabindex `0x800487D4`-`0x800488A0`, `.text` `0x800BCAA0`-`0x800BD9C0`, `.data` `0x803A4088`-`0x803A4260`, `.sdata` `0x804B6918`-`0x804B6940`, `.sdata2` `0x804B9EF8`-`0x804B9F00`.
- `game/CGloballyReusableTexBuf.cpp`: extab `0x8001BC8C`-`0x8001BFC0`, extabindex `0x800488A0`-`0x80048930`, `.text` `0x800BD9C0`-`0x800BE9D8`, `.ctors` `0x803529D0`-`0x803529D4`, `.data` `0x803A4260`-`0x803A4440`, `.bss` `0x803E6600`-`0x803E6610` (destructor chain object), `.sdata` `0x804B6940`-`0x804B6960`, `.sbss` `0x804B9008`-`0x804B9010`, `.sdata2` `0x804B9F00`-`0x804B9F08`.

## F-137-2: status

`CGameStage.cpp` is Matching: every function is 100% and the full frozen DOL verifies. `CGloballyReusableTexBuf.cpp` is NonMatching. Thirteen of the fourteen compared functions are 100% (the static initializer is `fn_800BE9B0` in the target and is not compared), including `dispose<Work>`, the `move_ptr<Work>` destructor and the `__vec_split_buffer` destructor, whose EH tables now have the original sizes (`InitializeWork` extab 0x1E0, split-buffer destructor 0x64). `InitializeWorkBuffer` is 99.91%. Every instruction opcode, immediate and stack offset matches; nine rows differ in three register choices: the `first`/`last` pointers of the inlined split-buffer destructor (r26/r25 against r25/r26), the `pBuffer` register in `WorkBuffer::Allocate` (r25 against r28), and the `.sdata2` relocation name of the texture-size array. Remaining work: find the source shape that gives those registers. The static initializer is `fn_800BE9B0` in the target; its name needs the symbols entry only when the unit becomes Matching.

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

Changes to `include/vector` need a per-function comparison of `report.json` against a fresh baseline of the same `origin/main`, because the vector instances are shared with BWorld, CControllerComposition, CDividedGround, CPauseMenu and CSaveManager. The current changes were checked that way on top of `58520c41`: no function in any other unit changed its score, and the full frozen DOL verifies.

### F-137-8: EH action counts follow the helper used to compute a range

The extra `DestroyBase(__vec_deleter)` action in the split-buffer destructor and in `InitializeWorkBuffer` came from computing the end of the element range as `first + size_` in the loop header. Calling `end()` (`begin() + size_`) gives the original two-action chain and the 0x64 and 0x1E0 extab sizes. The loop shape (`while (last > first) { --last; destroy(last); }`) does not matter once `end()` is used.

### F-137-9: stack slot order follows the nesting depth of inlined helpers

The `stb 0` temporaries for empty `int2type` tags, the word temporaries behind `std::max` and the EH locals are laid out by a rule that alternates with the nesting depth of the inlined function that owns them: adding one forwarding member function between a caller and a callee reverses the order of every temporary below that callee. Member order inside a class, definition position, operand order of comparisons and compiler version (Wii/1.0 to 1.5) do not change the layout. Consequences measured on the TexBuf unit:

- Tag-less forwarding overloads (`construct_at_end(n)`, `swap(buffer)`, `buffer.allocate(size, n)`, `can_append(n)`, and the two `recommend_*` forwarders) put the `append_realloc` temporaries into the original order. The original evidently called helpers through such overloads.
- `WorkBuffer::Allocate(const TexSize*, u32)` must contain both the size loop and the allocation. With the loop in `InitializeWorkBuffer` and a separate `Allocate()`, the word temporary of `std::max` and the `GameMemory::Free` pointer swap places. Keeping them in one nested function matches the original order and fixes the loop registers.
- Adding a forwarding layer inside `recommend` around the throw fixes the registers of the inlined throw blocks and the order of the two byte temporaries in `InitializeWorkBuffer`, but changes `recommend` instances in CDividedGround, CPauseMenu, CSaveManager, BWorld and CControllerComposition from 100% to 90-99%. Fix the order with forwarders on the callers' paths instead.

### F-137-10: a swapped register pair in an inlined loop can come from a missing local

`T* p = data_ + size_` inside the element-destroy helper gave swapped loop registers in `dispose<Work>` and the two destructors. Writing `size_type n = size_; T* p = data_ + n;` in the caller and passing both to the helper fixed all three, but main's later `erase_at_end` rewrite already reached 100% for them, so that change is not part of this pull request. The lesson stays: declare the pointer and the count as separate locals in the caller before the helper call.
