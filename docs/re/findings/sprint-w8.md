# Sprint w8: CPlayerPii and CSaveManager deque

`include/deque` now models the MSL `__cdeque_buf` with a separate `const_iterator` (const pointer members, own `copy(const T*, const T*, T*)` overload), pointer-based `construct_at_end`/`construct_at_front` for the `Allocator&` buffer, a buffer `swap` through a free `swap(__cdeque_buf&, __cdeque_buf&)`, a tag-dispatched `copy_construct` and `__cdeque_common<b>::throw_length_error`.

| Unit | Before | After |
| --- | --- | --- |
| `game/CPlayerPii` | 218/254 | 250/254 |
| `game/CSaveManager` | 240/243 | 243/243 |

Open in CPlayerPii: `GetShadowName` (only the schedule slot of the `mpPlayer` load differs; local copies, `const CPlayer&`, element constness and statement order did not move it) and three functions whose only differences are relocations into `.data` (`deque:: length error` and the `length_error` RTTI strings sit at other offsets because the unit's `.data` layout differs). Neither unit can flip yet: the weak template `.text` order differs from row 126 (CSaveManager) and row 60 (CPlayerPii).

## Traps

- **An implicit trivial copy constructor is still called out of line for an explicit `T(x)`.** Under `#pragma dont_inline on`, `const_iterator(*this) += n` calls a generated copy constructor (`fn_800FCAE0`), while passing the same iterator by value copies it inline with four word moves. A user-declared copy constructor turns the by-value copies into calls as well. The target therefore needs a user copy constructor on the mutable `iterator` (called in `insert<void_iterator>`) and none on `const_iterator` (inline in `distance` and `copy_construct`).
- **An uninitialised one-byte copy (`lbz r0,8(r1); stb r0,0xc(r1)`) is a default-constructed tag passed by value.** `__cdeque_imp`'s copy constructor passes `iterator_traits<const_iterator>::iterator_category()`; an `Allocator()` temporary instead calls the allocator's out-of-line default constructor (63% to 100%).
- **Split a `lbl_` string blob when objdiff flags `addi rX, rBase, off`.** CSaveManager's `Work` matched only after `lbl_803AD140` was split in `symbols.txt` at the source's literal boundaries (0x80, 0x98, 0x54 bytes); the bytes were already identical.
- **Relocations into `.data` compare by offset.** Identical string literals still miss when the unit's `.data` layout differs (CPlayerPii's extra weak vtables and RTTI strings come first). These functions only reach 100% once the data order matches.
- **The cdeque `copy(T*, T*, T*)` has two copies in CPlayerPii.** The `scope:local` one at `0x800FC970` is the `const T*` overload used by `copy(const_iterator, const_iterator, T*)`; the global one is the plain `T*` overload used by the iterator `copy` and by the `Allocator&` buffer's pointer `construct_at_end`.

## Emission order measurements (sandbox, `dont_inline`, Wii/1.1)

- Groups follow reverse definition order of the class templates, independent of instantiation and request order, for in-class and out-of-class members alike.
- A deferred member requests a member of a later group: it joins that group in the same round. A member of its own class declared earlier goes to the next round.
- A class instantiated after parsing lists its in-class members in request order, ahead of its out-of-class members (declaration order). A class instantiated during parsing lists all members in declaration order.
- In the target the `__cdeque_pointer` `insert` overloads come a round later than every other `__cdeque_pointer` member although `allocate_back` calls them in round 1, and the call site passes `li r7, 0`. They are probably member templates with a `restrict_to` default pointer argument. Making them member templates and moving the iterator classes ahead of `__cdeque_buf` fixed rows 127-139 of CSaveManager, but the next differences (`__ne__` of the pointer iterator, `push_back_realloc`, the `bind`/`vector` groups) were not solved, so neither change was kept.
