# Issue 203: CSaveManager.cpp, fifth block

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #203 lists 24 functions in `.text` `0x80109FD0`-`0x8010A7D4`, the fifth block of the file started in [F-199-1](199-csavemanager.md) and continued in [F-200-1](200-csavemanager-2.md), [F-201-1](201-csavemanager-3.md) and [F-202-1](202-csavemanager-4.md). No decompiler output was used.

## F-203-1: the 24 functions of #203 reconstructed

All 24 functions pair with the original at 100% in `unitdiff.py`, except `allocate_back` at 99.94%: its one differing pair is the relocation of the string literal `deque:: length error` (the original names it `lbl_803AD880`, the compiled object a local `@5932`), the same kind of difference as `Work` and `StartWork`. `configure.py`, `ninja` and `verify.py` exit 0 and the linked `main.dol` is unchanged. The unit stays `NonMatching` because it spans #199 to #206.

| Address | Function |
| --- | --- |
| `0x80109FD0`, `0x8010A030`, `0x8010A080`, `0x8010A090` | `shared_ptr_deleter<CSaveManager>` constructor and the `move_ptr` / `enable_from_this` instances, already 100% after #202 |
| `0x8010A0A0` | `CSaveManager::GetInstance()`: returns `sInstance` |
| `0x8010A0B0` | `~CSaveManager`, 100% once its callees carry the compiled names |
| `0x8010A150` | `~__deque_imp`: destroys the elements, then frees every non-null block pointer of the ring buffer |
| `0x8010A250` | `__deque_imp::alloc()`: the allocator half of the `compressed_pair` at `+8` |
| `0x8010A260` | `push_back(const T&, int2type<false>)`: grows when no back room is left, constructs at `end()`, increments the size |
| `0x8010A2F0` | `back_capacity()`: `capacity() - (start_ + size_)` |
| `0x8010A330` | `allocate_back(size_type n)`: the block allocator behind `push_back` |
| `0x8010A5D0` | `pop_front(size_type, int2type<false>)` |
| `0x8010A6A0`, `0x8010A6D0` | `destroy_all()` and `destroy_all(pod_type)` |
| `0x8010A6E0` | `__deque_imp::iterator::operator*` |
| `0x8010A6F0`-`0x8010A710` | `__cdeque<T*>::push_back`, `pop_front`, `swap` |
| `0x8010A720`, `0x8010A760` | `__cdeque_pointer<T>::begin()` and `end()` |
| `0x8010A7A0`-`0x8010A7D0` | `__cdeque_pointer<T>::front`, `back`, `push_back`, `swap` |

## F-203-2: how the deque is built

`__deque_imp<T>` stores `start_` and `size_` at 0 and 4 and a `compressed_pair<__cdeque<T*>, Allocator>` at 8. A block holds `1024 / sizeof(T)` elements (256 for the 4-byte `WorkType`), so `0x100` is the block size in `allocate`, `deallocate` and the `start_` arithmetic.

- **The ring buffer of block pointers always ends in a null sentinel.** Every place that adds a block writes the new pointer into `back()` (the sentinel) and then `push_back(nullptr)` for the next sentinel. The destructor skips null entries for the same reason. `back_capacity()` therefore counts capacity in elements as `(buffer size - 1) * block_size`.
- **`allocate_back(n)` has three stages.** It first moves whole unused blocks from the front to the back while `start_ >= block_size`; if blocks are still needed it checks `max_size()` and `capacity()` and throws `length_error("deque:: length error")`; then it either allocates into the ring buffer, or builds a spare `__deque_imp`, reserves `max(2 * capacity, size + blocks + 1)` slots, allocates the blocks, copies the old block pointers except the old sentinel in with `insert`, swaps the buffers and clears the spare so its destructor frees nothing.
- **`pop_front` frees the front block once `start_` reaches two blocks and recentres `start_` at `block_size / 2` when the deque empties.** No element is destroyed: the loop that a non-POD element would need compiles to nothing for the enum, and `destroy_all` is likewise just `size_ = 0`. The `pod_type` dispatch is therefore a descriptive guess (low confidence).
- **Layers of the ring buffer.** `__cdeque<T*>` forwards `push_back`, `pop_front`, `swap` and `operator[]` and re-exports `begin`, `end`, `front`, `back`, `size`, `empty`, `capacity`, `reserve`, `clear` and `insert` from `__cdeque_pointer<T>`, which casts between `T*` and the `void*` storage of `__cdeque_buf<void*>`. This reproduces the one-instruction tail-call layers: `push_back` has three (`0x8010A6F0` to `0x8010A7C0` to `0x800FA680`), `swap` has three (`0x8010A710` to `0x8010A7D0` to `0x800FC0E0`), `front` and `back` have one.
- **`__cdeque_buf<void*>::iterator` is a 16-byte ring iterator** (current element, first slot, begin position, end of storage) with a copy constructor, `operator*`, `operator++` and `operator!=`; the pointer iterator derives from it and casts the element reference.

## F-203-3: names given outside the block

For the link: `__cdeque_buf<void*>` `size` `0x800FA400`, `empty` `0x800FA410`, `capacity` `0x800FA420`, `reserve` `0x800FA470`, `begin` `0x800FA550`, `end` `0x800FA570`, `front` `0x800FA600`, `back` `0x800FA620`, `push_back` `0x800FA680`, `pop_front` `0x800FA810`, `clear` `0x800FA8A0`, `swap` `0x800FC0E0` and the iterator copy constructor, `operator*`, `operator++` and `operator!=` at `0x800FAAE0`, `0x800FAB80`, `0x800FAB90` and `0x800FB870`; `std::max<unsigned long>` `0x800887B0`; the shared `__throw_deque_length_error` `0x800FA240`; and in the neighbouring blocks `allocator::allocate`, `deallocate`, `construct` (`0x8010ABA0`-`0x8010ABC0`), the pointer-iterator constructor, `operator*`, `operator++`, `operator-` (`0x8010A7E0`-`0x8010A850`), `insert` `0x8010AC50`, the `compressed_pair` `second` `0x8010AC40`, and in the `#207` region `front_capacity` `0x8010B960`, `max_size` `0x8010B970`, the spare-deque constructor `0x8010B9C0`, `capacity` `0x8010BBD0`, `__deque_imp::end` `0x8010BC50` and the pointer-iterator `operator!=` `0x8010BC60`. The bodies of `capacity`, `max_size`, `insert`, `__deque_imp::iterator` and the `operator-` stay declarations until their owning issues reconstruct them.

`GetInstance` is the one function of the block that nothing calls: no instruction and no data word in the executable refers to `0x8010A0A0`. The original unit kept it although it is unreferenced, so the source wraps it in `#pragma force_active on` (the compiler removes an unused member of an anonymous-namespace class otherwise). The name and the accessor shape are descriptive (low confidence).

## Traps

- **Declaration order changes CodeWarrior register allocation, and nothing else tried did.** `allocate_back` matched the original instruction for instruction except that three long-lived values swapped registers (blocks `r27`/`r29`, pointer `r29`/`r28`, zero constant `r28`/`r27`). Moving loop variables, using `while` loops, inlining the counts, hoisting or inlining the pointer, adding unused or dead uses, `register`, and named null variables all left 97.06%. Declaring `T* pBlock; size_type count; size_type limit;` before `size_type blocks = ...` at the top of the function, after the two references, gave the original allocation. Try declaration order at the top of the function before restructuring code.
- **Toy functions on the real compiler find allocator rules in seconds.** `mwcceppc` compiles a 20-line file in well under a second, so a stand-alone toy with the same call pattern shows which values outrank which. Observed: a copy of a call result (`mr rN, r3`) outranks an arithmetic result (`add`), which outranks a plain load (`lwz`); extra copies of a value into call arguments do not raise its rank, but extra arithmetic uses in the loop body can.
- **Lazy dereference of a reference result changes the instruction order.** `rSpare.reserve(max(a, b))` keeps the `max` result pointer in `r3`, moves it to `r4`, loads the object into `r3` and then dereferences; storing the result in a `size_type` local first loads before the move. Pass the call expression directly.
- **Statement order in `pop_front` follows the scheduler, not the source.** `start_ += n; size_ -= n;` compiled the size update first; writing `size_ -= n; start_ += n;` produced the original load order.
- **An argument of integer 0 to a by-value iterator-range parameter is a real `int`.** `rSpare.insert(pos, first, last, 0)` produced `li r7, 0`; a `Metrowerks::int2type` temporary would have been a stored byte and an address.
- **A function that nothing references can still exist in the original.** Look for a `bl`, a `.data` pointer and a `lis`/`addi` pair before deciding a symbol belongs to a caller; with none, `#pragma force_active on` reproduces it, as for `GetInstance`.
- **Shared-header changes were checked against `report.json`.** The `include/deque` rewrite changed the score of no function outside `CSaveManager`; `ScriptCamera`, `CMsgBalloon` and every other unit that includes `<deque>` or `<queue>` kept their per-function scores against a fresh `origin/main` build.
- **`ninja` rewrites `splits.txt` ordering.** Restore it with `git checkout config/wpse-v1/splits.txt` before committing if it shows as changed.
