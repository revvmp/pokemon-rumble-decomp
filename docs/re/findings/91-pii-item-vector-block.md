# Block 0x80092630-0x80092F6C (issue #91, CDlgMsgWin/CDlgPiiList 19/19)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. The issue lists 34 functions from `0x80092630` to `0x80092EB0` plus the `CDlgPiiSelector` factory at `0x80092EB0`, which `src/game/CDlgPiiSelector.cpp` already reconstructs ([F-91-2](91-cdlgpiiselector.md#f-91-2-factory-and-session-interface)). No source was added for the other 34 functions; this file records why and what a later session needs.

## F-91-3: The block is the tail of an unsplit template-instance range, not the tail of CDlgPiiList.cpp

[F-82-2](82-cdlgpiilist.md#f-82-2-unit-end-and-first-matched-functions) already showed that `0x80091360`-`0x80092EB0` is outside `CDlgPiiList.cpp`: its `extab` and `extabindex` records (`0x80012CD8`-`0x80012F68`, `0x800467E8`-`0x80046AD0`) are unowned and it has no `.data`, `.sdata` or `.ctors` entries. The issue title's "CDlgPiiList" is a survey estimate. There is no existing source to extend, so the first requirement of the take-issue procedure for files that span blocks (an existing owner split) is unmet. The neighbouring blocks #88, #89 and #90 (`0x80091610`-`0x80092624`) are unclaimed and hold the rest of the range.

A scan of every `bl` in `.text` for targets in `0x80091360`-`0x80092EB0` gives the external callers. They are `0x80094xxx` to `0x8009Exxx` (inside `CDlgPiiSelector`), `0x800AD3xx`-`0x800AD7xx`, `0x800E64xx` (`CPiiListFilterSort` code) and `0x800F7Bxx`-`0x800F7Fxx`. Four units call the same small functions, for example `0x80092980` (`CPlayer::IsJoined`, already named) from six selector sites. That pattern is a set of weak template instances folded to one copy by the linker, so the range is the first translation unit in link order that emitted them. `CDlgPiiSelector.cpp` is the first candidate, because its factory follows immediately and F-82-2 found its callers there, but no string, data or RTTI record proves it. Moving the `CDlgPiiSelector.cpp` split start from `0x80092EB0` back to `0x80091360` is therefore a hypothesis, not a finding, and it changes a shared split that issues #92 to #112 are working in.

### Layout recovered from the instruction streams

The block instantiates a `vector` of 8-byte elements, plausibly `vector<shared_ptr<CUIPiiListItem> >`, using the same member order as `include/vector` (`data_` at 0, `size_` at 4, compressed capacity-and-allocator pair at 8).

| Address | Size | Shape | Confidence |
| --- | --- | --- | --- |
| `0x80092630` | 0x3C | Reads the word at the object, then calls the helper `0x80092380` with its address | Low |
| `0x80092670`, `0x80092690` | 8 | Dereference the first word of the argument | Low |
| `0x80092680` | 0xC | Copies one word | Low |
| `0x800926A0` | 4 | Tail-calls `0x800926B0` | Low |
| `0x800926E0` | 0x8C | Insert at end: compares `size_` (offset 4) with capacity read through `0x80092610`; below capacity constructs in place at `data_ + size_ * 8` and increments `size_`, otherwise calls the reallocating path `0x80092860` | Medium |
| `0x80092770`, `0x80092780`, `0x80092790` | 8, 4, 0x14 | `alloc()` (adds 8), an empty `construct`/`destroy` body, and `end()` (`data_ + size_ * 8`) | Medium |
| `0x800927B0`, `0x80092810` | 0x58, 0x4C | Placement `new` of an 8-byte `shared_ptr`, then its copy constructor that retains the control block (`shared_ptr_deleter_common::retain`) | High |
| `0x80092860`, `0x80092BB0`, `0x80092BF0` | 0x2C, 0x2C, 0x2C | Out-of-line wrappers that pass a zero `bool` (`int2type`/POD tag) on the stack to `0x8009AF60`, `0x80092190` and `0x8009B310` | Low |
| `0x80092890` | 0x10 | Iterator `operator++` by 8 bytes | High |
| `0x800928A0` | 0xD4 | Gets the `IGameSession` singleton, calls slot 25 with the player index (vtable offset 0x64), returns 0 unless `CPlayer::IsJoined`, then builds a temporary vector from the player's sub-object at offset 0x14 and passes it to `0x80091360`; no caller except `0x80096C24` | Medium |
| `0x80092980` | 8 | `CPlayer::IsJoined`, `lbz` at offset 0x10; already named in `symbols.txt` | High |
| `0x80092990` | 8 | Returns `this + 0x14` | High |
| `0x800929A0`, `0x800929F0` | 0x44 | Argument shuffles that forward a range and a `bool` to the next stage; `0x800929F0` ends in `0x80092A40` | Low |
| `0x80092A40` | 0x164 | Range insert: computes the new element count, compares with capacity (`0x80092140`), reallocates through `0x80092BB0` or inserts in place via `0x80091EA0` and `0x8009B190` | Medium |
| `0x80092C20`, `0x80092C50`, `0x80092C80`, `0x80092CD0`, `0x80092D00`, `0x80092D40` | 0x30-0x44 | Chain of constructors for the vector: outermost calls `0x80092C50`, then `0x80092C80` which zeroes `data_` and `size_`, then `0x80092CD0` for the capacity pair, which stores zero capacity | High |
| `0x80092D50`, `0x80092DB0` | 0x58 | Destructor of the storage base, and the deleting destructor of the vector that calls it and `operator delete` when the flag is positive | High |
| `0x80092E10` | 0x60 | Constructs a temporary vector (`0x80092C20`), calls `0x80091360`, destroys it; only caller `0x8009A768` | High |
| `0x80092E70` | 0x40 | `GetCurrentGameMap()`, virtual slot 8 (offset 0x20), copies the returned word to the stack and passes its address to `0x80125180`; callers `0x80096FE8`, `0x8009705C`, `0x800971C0`, `0x80099794` | Medium |

The frame and exception shapes of `0x800927B0`, `0x800926E0` and `0x80092810` are consistent with the existing `include/vector` and `include/memory` templates used by `src/game/BWorld.cpp`, so a reconstruction can start from those headers.

## Why the block was not reconstructed

- The functions instantiate templates shared with `0x80091360`-`0x80092630` (`0x80092080`, `0x80092140`, `0x80091E20`, `0x80091EA0` and others), which belong to unclaimed #88 to #90. Matching them in objdiff needs those symbols named identically in `symbols.txt`.
- The range has no registered split and no evidence of its source file name. The take-issue procedure says to ask the user before taking on a split that its owner issue has not claimed.
- The element type, the container that `0x80091360` fills (a `set` or `list` is iterated at `0x800928A0`) and the iterator types are not proven. IDA was unavailable in this session (connection refused).

## Traps

- **Issue titles for this range name the wrong file.** Blocks #83 to #91 are labelled `CDlgPiiList`, but F-82-2 and the caller scan above show the range after `0x80091354` is shared template code used by four other units. Resolve the unit before planning source.
- **Zero-caller functions are not dead code.** `0x800928A0` and `0x80092E10` have one caller each in `CDlgPiiSelector`; a text search of the top-level generated `auto_*` assembly finds no caller because the callers sit in split units under `build/wpse-v1/asm/game/`. Scan `bl` instructions in the DOL instead.

## Next step

Assign the range to a single owner (#88 to #91 together), decide whether it starts `CDlgPiiSelector.cpp`, and reconstruct it as one unit starting from the `include/vector` instance the block uses.
