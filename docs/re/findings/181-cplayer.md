# Issues 181, 182 and 183: CPlayer.cpp

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md) plus `#pragma dont_inline on`. No decompiler output was used. Issues #181 (`0x800F75F0`-`0x800F8044`), #182 (`0x800F8050`-`0x800F8484`) and #183 (`0x800F8490`-`0x800F8D5C`) each cover part of one file, so one pull request takes all three.

## F-181-1: unit boundaries

| Section | Range | Evidence |
| --- | --- | --- |
| `.text` | `0x800F77E0`-`0x800F8D5C` | `0x800F8D60` is the `CPlayerPii` constructor (vtable `0x803A9448`) |
| `extab` | `0x800261C0`-`0x80026418` | Records of the first and last function |
| `extabindex` | `0x8004A88C`-`0x8004AA84` | Entries `0x800F77E0` and `0x800F8D60` |
| `.data` | `0x803A9320`-`0x803A93C8` | `CPlayer` vtable, `runtime_error` vtable, type names, the `list::splice` message; `0x803A93C8` is `"minimap_player"` of `CPlayerPii` |
| `.sdata` | `0x804B7108`-`0x804B7120` | `"CPlayer"`, `__RTTI__7CPlayer`, `__RTTI__Q23std13runtime_error` |

The two functions at the start of #181's list, `0x800F75F0` and `0x800F7710`, do not belong to this unit. They are the destructors of a `__vec_split_buffer` and its base `__vec_deleter` over a `shared_ptr`, referenced only from the cleanup records of `0x800F6780` (`extab` `0x80025FBC`, `0x80025FF4`), which sits in the file that ends with the `shared_ptr_deleter<COffScreenTexture>` destructor `0x800F75B0`. They remain in the fallback object and belong to #180's file. The estimated block start is therefore corrected: this file starts at `0x800F77E0`.

## F-181-2: classes

`CPlayer` derives from `ICalc` (vtable `0x803A9320`: destructor, `Calc` `0x800F85B0`, the empty default `CalcMtx`) and is 0x74 bytes: flags (4), player number (8), `IController*` (0xC), two `State` records (0x10 and 0x38), a `weak_ptr<CPlayerPii>` (0x60), then three words (0x68 reserved, 0x6C accumulated hold, 0x70 current hold). `State` is 0x28 bytes: a joined flag, a `CPiiBox` (0x14: type, capacity 1200, a `list<shared_ptr<CPiiPersonalData>>`) and two `shared_ptr<CPiiPersonalData>`. Names of the state members, the flags (`FLAG_COLLECT_HOLD` 1, `FLAG_PII_CHANGE_REQUESTED` 2) and the `Join`, `Leave`, `SaveState`, `RestoreState`, `SetPii` functions are descriptive (low); `SetController`, `GetPii` and `IsJoined` are the older names.

`Calc` reads `IUIInput::GetHold` (slot 3) of the controller: with the collect flag set it ORs the hold into word 0x6C, otherwise it stores it in 0x70. `SetPii` stores `static_pointer_cast<CPlayerPii>(shared_from_this())` of the Pii into the weak pointer (the inline `CPlayerPii::GetSharedPtr`, `0x80095180`) and takes the Pii's personal data. `GetPii` locks the weak pointer and returns the object only when `BWorldObj::mWorld` is non-null.

## F-181-3: the unit is built without inlining

Every `bl` to a one-line template or constructor in this file proves that no inlining took place: `#pragma dont_inline on` before the includes reproduces the 0x84-byte `CPlayer` constructor, the separate `State` constructor (0x54), `ICalc()` and `weak_ptr()`. `BWorld.cpp`, `CDlgPiiSelector.cpp`, `CMsgBalloon.cpp` and `ScriptCamera.cpp` use the same pragma.

## F-181-4: list structure recovered from the unit

The 81 functions are `State` and `CPlayer` members plus the instances of `std::list<shared_ptr<CPiiPersonalData>>` that no earlier file emits. They fix these properties of the MSL list:

- The container is `list` : `__list_imp` : `__list_deleter`; `size_` is `compressed_pair<Allocator, size_t>` and `tail_` is `compressed_pair<node_allocator, node_base>`. The accessors `alloc()` (`0x800F80F0`), `size_ref()` (`0x800F89F0`), `node_alloc()` (`0x800F8A00`) and `tail()` (`0x800F80D0`) are separate functions. Names are descriptive.
- `iterator` is `generic_iterator<false>` nested in `__list_deleter`, as in the CC0 reference `Swiftshine/key` (`MSL_C++/list`, documented from another title). It has an explicit copy constructor (hidden-pointer returns); `const_iterator` has none (register returns).
- `erase(list*, first, last)` (`0x800F8790`), `splice_all` (`0x800F8890`), `insert`, `link_nodes` and `unlink_nodes` are members or statics of `__list_deleter`. The temporary list of the assignment is a `__list_imp` built from a `const_iterator` range (`0x800F7E00`), so the dropped destructors are `~__list_deleter` `0x8008E3E0`, `~__list_imp` `0x8008E4E0` and `~list` `0x8008E5C0`. `~__list_deleter` calls the static `erase`.
- `list::operator=` (`0x800F7B50`) calls `copy_from` (`0x800F7B80`), which calls the free template `__assign_range` (`0x800F7C30`) with `begin()`, `nth(min(size(), x.size()))` and `end()`; the range is copied with `std::copy` (`0x800F7D60`) and either spliced from a temporary or erased.
- `advance` passes the iterator category twice: `__advance(it, n, tag)` calls `__advance(it, n, tag, int2type<0>())`.
- `list::splice` throws `runtime_error("list::splice called with unequal allocators")` when the allocators compare unequal; `operator!=` of two allocators is a function returning false. `runtime_error` holds an `__error_string` (a `shared_ptr<char>` filled by `strlen`, `new char[]` and `strcpy`) at offset 4.

## F-181-5: emission order

The retained order is reproduced by these source choices, found with scratch units and checked against all 81 functions:

- A member declared in the class but defined outside it is emitted in the template pass after the in-class chains. Templates are visited in reverse order of first declaration, so `list::locate` comes first, then the nested iterator's default constructor, then the `__list_deleter` members `erase` and `splice_all`, then `size_ref`, `node_alloc`, `unlink_nodes` and `link_nodes`. `locate` belongs to `list`; `erase` and `splice_all` belong to `__list_deleter`, whose nested iterator template is declared between them.
- `shared_ptr::reset` and the `weak_ptr` destructor and `lock` are defined outside their classes (`reset` was moved out of the class in `<memory>`). `<MSL_CPP/compressed_pair.h>` is included before `<MSL_CPP/allocator.h>`, which puts `allocator::deallocate` before the pair accessors.
- A default-constructed `iterator` in a pass function is placed after that function only when the iterator is a nested template of a template whose members follow; as a plain nested class of `list` itself it came last.
- `tail_(alloc())` rather than `tail_(size_.first())` reproduces the constructor's callee order.

## Source changes

`src/game/CPlayer.cpp`, `include/game/CPlayer.h`, `include/game/CPiiBox.h`, `CPlayerPii::GetSharedPtr` and `BWorldObj::IsInWorld` are new. `include/list` gains the range constructor, `operator=`, `splice`, iterator classes, `erase(first, last)` and the accessors; `<algorithm>` gains `copy`, `<iterator>` gains `advance`, `<memory>` makes `weak_ptr(const shared_ptr&)` carry the defaulted `restrict_to` parameter and `shared_ptr::reset` out of line, `allocator` gains converting and default constructors, and `compressed_pair` a one-argument constructor for an empty first member. `<MSL_CPP/runtime_error.h>` is new. `config/wpse-v1/symbols.txt` gives 111 functions and four data symbols the names the source produces, including the dropped weak instances in earlier files (`0x8008E3E0`, `0x80063DC0` and others) that must carry the same names to be discarded at link.

## Acceptance

`configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with `game/CPlayer.cpp` as `Matching`. Every function of #182 and #183, and all of #181 except `0x800F75F0` and `0x800F7710`, is in the unit.

## Traps

- **Unreferenced destructor pairs at the top of a block can belong to the previous file.** Their only references are cleanup records of that file's functions (F-181-1).
- **Dropped weak instances still count.** Functions that earlier files supply must be renamed in `symbols.txt` to the exact mangled name, otherwise the weak copy in the new object survives and shifts everything after it. `nm` of `main.elf` against the object's symbols shows the survivors.
- **Do not add `<string>` to `<stdexcept>`.** The extra include changes template emission in other units; `runtime_error` lives in its own header.
- **Piping a script into `head` kills it.** A mapping script that wrote its output file after printing lost the file when `head` closed the pipe.
