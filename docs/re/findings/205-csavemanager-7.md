# Issue 205: CSaveManager.cpp, seventh block

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #205 lists 25 functions in `.text` `0x8010B330`-`0x8010B7A4`, the seventh block of the file started in [F-199-1](199-csavemanager.md) and continued through [F-204-1](204-csavemanager-6.md). No decompiler output was used.

## F-205-1: the 25 functions of #205 reconstructed

All 25 functions pair with the original in `unitdiff.py`. Twenty-one are at 100%. Four differ only in relocation names of neighbours that other issues still own: `__cdeque_pointer<T>::iterator::operator==` and `operator-(const iterator&)` (95%, tail calls to the `0x800FB470` and `0x800FB4D0` base iterator members), `iterator::operator+=` (99.58%, call to `0x800FD3C0`) and the four-argument `call_bound` (99.52%, calls to `0x8010B7B0`, `0x8010B800` and `0x8010B810`, owned by #206). `configure.py`, `ninja` and `verify.py` exit 0 and the linked `main.dol` is unchanged. The unit stays `NonMatching` because it spans #199 to #206.

| Address | Function |
| --- | --- |
| `0x8010B330`-`0x8010B390` | `void_iterator::operator!=` and the `operator==` and `operator-` forwards to the pointer iterator (`0x8010B360`, `0x8010B370`, `0x8010B390`) |
| `0x8010B3A0`-`0x8010B420` | `void_iterator::operator+` and the two `operator+=` forwards |
| `0x8010B450` | `void_iterator::operator=`: a single block copy of the iterator and the one-byte functor |
| `0x8010B480`, `0x8010B4E0` | `std::distance` for `void_iterator` and its random access overload |
| `0x8010B4F0`-`0x8010B5D0` | `void_iterator` copy constructor, `operator*`, the `pointer_cast` functor `operator()` and its `get` |
| `0x8010B600`, `0x8010B630`, `0x8010B640` | `void_iterator::operator++`, `iterator::base()`, `void_iterator(const iterator&)` |
| `0x8010B670` | `bound_func<void, mem_fn_1<void, CSaveManager, WorkType>, tuple<CSaveManager*, WorkType>>::operator()` |
| `0x8010B6A0`, `0x8010B6D0` | the two `call_bound` overloads: forward with an `int2type<0>` tag, then call the member function with the two resolved arguments |
| `0x8010B750`, `0x8010B780`, `0x8010B790` | `get_bound<1>` of the tuple and the `bound_element<1>` constructor and `get` |
| `0x8010B7A0` | `resolve_bound_argument(A&, const Arguments&)`, the identity |

## F-205-2: why the pointer iterator is a member of void_iterator

The original `void_iterator::operator=` is a straight block copy (four `lwz`, one `lbz`, four `stw`, one `stb`). Under `#pragma dont_inline on`, the compiler emits an implicit assignment operator that calls the assignment of every base class but copies a member of class type with a trivial assignment inline. The block copy therefore needs the pointer iterator to be a member, not a base: `void_iterator` holds `iterator mIt` followed by the empty `pointer_cast mCast`, and forwards `operator++`, `operator+=`, `operator-`, `operator==` and `operator!=` to `mIt`. The pointer iterator keeps its base class. The previous inheritance from the pointer iterator produced a call to the base iterator `operator=` and scored 0% for the 44 bytes.

## F-205-3: the bound call chain

`bound_func<R, F, tuple<T0, T1>>::operator()` declares an empty `no_arguments` local without initialisation and passes it by const reference, which the original shows as an address on the stack with no store. `call_bound(F&, Tuple&, const Arguments&)` forwards with `int2type<0>()` (this one has the `li`/`stb` tag store); the tagged overload evaluates the second argument first and calls `rFunction(resolve_bound_argument(get_bound<0>(t), arguments), resolve_bound_argument(get_bound<1>(t), arguments))`. `get_bound<I>` builds a one-pointer `bound_element<I, T0, T1>` over the tuple (`stw r4, 0(r3)`) and returns its `get()` (`lwz r3, 0(r3)` for element 0 and `lwz`/`addi 4` for element 1). `mem_fn_1::operator()` takes the object pointer by `C* const&`, so the original loads it (`lwz r3, 0(r4)`) before the pointer-to-member call. Only `CSaveManager.cpp` instantiates the two-element `bound_func`; the one-element specialisation and `mem_fn_0` are unchanged.

## F-205-4: shared header changes and baseline comparison

`include/deque` replaces the `void_iterator` base class with members (see F-205-2). `include/tuple` gains `bound_element` and `get_bound` and the friend declaration for the two-element tuple. `include/functional` gains `no_arguments`, `resolve_bound_argument(A&, const Arguments&)`, the `call_bound` overloads and the changed `mem_fn_1::operator()` signature. `symbols.txt` renames the fourteen `fn_` placeholders of the block to the names the compiled unit produces. `report.json` per-function scores of every unit are unchanged against a fresh `origin/main` build (15411 functions compared, zero lower).

## Traps

- **Implicit assignment of a derived class calls the base assignment under `dont_inline`.** A class that derives from an iterator with a user copy constructor compiled a call to the base `operator=` plus an inline byte copy, whereas the same type held as a member compiled to the original block copy. When an original implicit `operator=` is a block copy, test member composition before changing the base class.
- **An empty by-value argument copies a byte, an uninitialised local passed by reference does not.** `no_arguments()` as a temporary emitted `li`/`stb`; a named local passed by value emitted `lbz`/`stb`; a user-provided empty constructor emitted a constructor call. Declaring the empty class as an uninitialised local and passing it as `const T&` reproduced the original address-only argument.
- **The compiled names live in the other object.** In `objdump -t`, `build/wpse-v1/obj/game/CSaveManager.o` is the split original and carries `fn_` placeholders; the compiled source object is `build/wpse-v1/src/game/CSaveManager.o`. Take the names to give to `symbols.txt` from the latter, matching by size and call order.
