# Issue 204: CSaveManager.cpp, sixth block

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #204 lists 26 functions in `.text` `0x8010A7E0`-`0x8010B324`, the sixth block of the file started in [F-199-1](199-csavemanager.md) and continued in [F-200-1](200-csavemanager-2.md), [F-201-1](201-csavemanager-3.md), [F-202-1](202-csavemanager-4.md) and [F-203-1](203-csavemanager-5.md). No decompiler output was used.

## F-204-1: the 26 functions of #204 reconstructed

All 26 functions pair with the original at 100% in `unitdiff.py`, except `get_deleter` of the `CSaveManager` `shared_ptr_deleter` at 99.74%: its one differing pair is the relocation of the `type_info` operand, which the original names `lbl_804B7350` and the compiled object `__RTTI__Q33std6detail60default_delete<...>` (a `.sdata` object owned by the unit's data, not by this block). `configure.py`, `ninja` and `verify.py` exit 0 and the linked `main.dol` is unchanged. The unit stays `NonMatching` because it spans #199 to #206.

| Address | Function |
| --- | --- |
| `0x8010A7E0`-`0x8010A820` | pointer-iterator constructor from the base iterator, `operator*` and `operator++`, already 100% after #203 |
| `0x8010A850` | `__cdeque_pointer<T>::iterator::operator-(int) const`: copies the iterator, subtracts, returns the copy |
| `0x8010A8A0` | `__vec_constructor<void*>::push_back_realloc`, already 100% after #202 |
| `0x8010A920`, `0x8010A970` | `function_imp::clone()` and `clone(void*)`, the heap and placement copies of the bound `CSaveManager::Work` callable |
| `0x8010A9E0` | `function_imp::operator()`: `addi r3, r3, 4` and a tail call into the bound call |
| `0x8010A9F0`, `0x8010AA40` | `shared_ptr_deleter<CSaveManager>::get_deleter(const type_info&)` and `dispose()` |
| `0x8010AA90`-`0x8010ABC0` | `shared_ptr<CLayout>::reset`, `move_ptr` destructor, `default_delete::operator()`, `allocator<WorkType>` `allocate`, `deallocate`, `construct`, already 100% |
| `0x8010AC20`-`0x8010AC40` | `CompressedPairStorage::second()` instances; `0x8010AC30` is the `const` overload for the empty-base layout |
| `0x8010AC50` | `__cdeque_pointer<T>::insert(pos, first, last, int)` |
| `0x8010ACD0` | `insert(pos, first, last, bidirectional_iterator_tag)`: two `int2type<0>` temporaries |
| `0x8010AD60` | `insert(pos, first, last, int2type<0>, int2type<0>)`: converts the position to the base iterator and both ends to the `void*` iterator |
| `0x8010ADF0` | `__cdeque_buf<void*>::insert<It>(pos, first, last, bidirectional_iterator_tag)` |
| `0x8010AE20` | `__cdeque_buf<void*>::insert<It>(pos, first, last, int2type<0>)`: the work function |
| `0x8010B180` | `__cdeque_buf<void*>::copy<It>(first, last, iterator)`: segment-wise copy into the ring buffer |
| `0x8010B2B0` | `__cdeque_buf<void*>::copy<It>(first, last, void**)`: element loop |

`0x8010A850` and the four `shared_ptr_deleter`/`function_imp` virtuals were named from the symbols the compiled unit already produced, so only `symbols.txt` changed for them.

## F-204-2: how the ring-buffer insert is built

`__cdeque_pointer<T>::insert` is a chain of five layers over the `void*` ring buffer `__cdeque_buf<void*, allocator<void*>>`:

- **Typed layers.** `insert(pos, first, last, int)` forwards with the iterator category, which slices a `random_access_iterator_tag` into the `bidirectional_iterator_tag` parameter of the next layer; that forwards with two zero-initialised `int2type<0>` temporaries; the last typed layer builds the base position with `iterator::base()`, wraps both ends in `void_iterator` and passes the category of `void_iterator`.
- **`void_iterator`.** A pointer iterator with a one-byte empty functor member after the 16 bytes of the iterator (20 bytes in all). Its `operator*` copies the iterator, hands the copy to the functor, which copies it once more and dereferences the `T*` slot, returning the element as `void*`. Its comparison, `operator-`, `operator+=` and `operator+` forward to the pointer iterator, which forwards to the base iterator. The one-byte member is the reason the original copy constructor copies four words and a byte.
- **`__cdeque_buf<void*>::insert`.** Computes the element count with `std::distance`, the index of the position as a distance from `data_ + start_` (adding the capacity when the position is before it), and tests `n <= capacity` and `size <= capacity - n`.
  - If the data fits, it opens a gap of `n` elements on the nearer side: when `index < size - index` it moves the iterator `n` places back, sets `start_` from the address of the new first slot and copies the first `index` elements forward; otherwise it grows `size_`, copies the tail backward to the end and moves back `n`. It then copies `[first, last)` into the gap.
  - Otherwise it builds a `__cdeque_buf<void*, allocator<void*>&>` (a split buffer in the sense of the vector `__vec_split_buffer`), allocates `recommend(n)` slots, writes the new elements at `index`, appends the tail and prepends the head, clears the moved-from buffer and swaps `data_`, `start_`, `size_` and the capacity.
- **`copy<It>(first, last, iterator)`.** Copies `min(remaining, room to the end of the segment)` elements at a time into the destination, advances the destination and its `cur` pointer, and finishes by returning the destination. `copy<It>(first, last, void**)` is the plain element loop it calls for each segment.

The `__cdeque_buf` members that these layers call, and that the 800F-range unit (#184) also instantiates, stay declarations: `recommend`, `clear_moved`, `allocate(size, n)`, `construct_at_end`, `construct_at_front`, `copy(iterator, iterator, iterator)`, `copy_backward`, the iterator `operator+=`, `operator-(const iterator&)`, `operator==`. Their symbol names were attached to the original addresses `0x800FA8B0`, `0x800FA990`, `0x800FBEC0`, `0x800FC030`, `0x800FBF40`, `0x800FB8A0`, `0x800FB5F0`, `0x800FBB50`-`0x800FBBE0`, `0x800FBF20`, `0x800FBF70`, `0x800FC060`, `0x800FC0A0`, `0x800FCEA0`, `0x800FABD0` and `0x800FB490`-`0x800FB5B0`, and the `void_iterator` and `function_imp` helpers to `0x8010B330`-`0x8010B950`.

## F-204-3: shared header changes

`include/deque` gains the `__cdeque_buf` constructors (default and with an allocator reference), `alloc`, the iterator arithmetic, comparison and accessors, the `insert` and `copy` templates, the pointer-iterator typedefs and operators, `base()` and `void_iterator`. The `__cdeque_buf` default constructor now initialises the three members to zero, as the original `0x800F8F70` does. `report.json` per-function scores of every other unit are unchanged against a fresh `origin/main` build; only `game/CSaveManager` moved (71.05% to 88.96% fuzzy).

Draft pull request #1131 (#184, #185) also restructures `include/deque` (`__deque_deleter`, `__cdeque_imp`, a `__cdeque_buf` default constructor and destructor body). Whichever merges second must reconcile: the `__cdeque_buf` default constructor text is intentionally identical in both.

## Traps

- **An uninitialised-copy tag comes from a tag class with base classes.** `typename iterator_traits<It>::iterator_category()` for `random_access_iterator_tag` (which derives from `bidirectional_iterator_tag`) compiles to `lbz r0, 8(r1)` / `stb r0, 0xc(r1)`, a copy of an uninitialised byte, while `int2type<0>()` or a base-less tag compiles to `li r0, 0` / `stb`. `std::distance` reproduces the first form with no extra effort. When two layers show different forms, the first passes a category and the second passes `int2type<0>()`.
- **Slicing a derived tag into a base-class parameter makes the compiler hold the temporary's address in a callee-saved register.** `insert(..., random_access_iterator_tag)` with a `random_access_iterator_tag` argument reproduced the original `addi r30, r1, 0xc` and the fifth saved register; passing the same tag to a parameter of its own type rematerialised the address at the call and saved one register fewer.
- **By-value forwarding copies, reference forwarding does not.** The original forwarded the iterator ends from the thin `insert` into the work function with no copy; this needs `ForwardIterator&` parameters on the work function and by-value parameters on the thin one. A non-const reference cannot bind to a temporary, so the position parameter is `const iterator&`.
- **A function returning an object by value hides the constructor result.** Converting the pointer iterator with a constructor call used `r3` for the next argument; the original reloaded the temporary's address with `addi`, which a by-value member function `base()` reproduces.
- **`return iterator(*this) -= n;` and `iterator it(*this); return it -= n;` differ.** The first reuses the constructor's returned `this` in `r3`; the second reloads the address. The originals use the first form for every `operator+` and `operator-`.
- **If/else order follows the condition's polarity.** The original places the no-growth path first; writing the growth test as `n > room || size > room - n` emitted the growth path first and scored 49%. Write the positive test (`n <= room && size <= room - n`).
- **A decrement before a call needs to sit before it in the source.** A `for (; n != 0; --n)` loop executed the decrement after the `++first` call; a `while` loop with `--n` before `++first` matched.
- **A vtable function can score 0% in the objdiff listing only because it still has its placeholder name.** `get_deleter`, `dispose`, `clone`, `operator()` and the `second() const` of the `CSaveManager` deleter already matched; renaming the original symbols to the compiled names was the whole fix.
