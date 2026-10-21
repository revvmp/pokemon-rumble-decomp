# Issue 278: SaveData.cpp third block (0x80165AE0-0x8016AAD4)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, read in the dtk split assembly of `game/SaveData.cpp` ([F-276-1](276-savedata.md#f-276-1-unit-boundaries)). Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). The block is the tail of the `SaveData` weak-template output and the small free functions around it. No decompiler output was used.

## F-278-1: functions

| Address | Function | Match |
| --- | --- | --- |
| `0x80165AE0`, `0x80165AF0`, `0x80165B10`, `0x80165B40`, `0x80165B60` | `__vec_deleter<T>::erase_tail(n, int2type<0>)` (the `resize` shrink path) for `SD_BattleRoyalData`, `SD_PiiPersonalData`, `SD_PiiCollectionData`, `SD_StatisticValue`, `UsedPassword` | 100% |
| `0x80165B80` | `DeleteSaveFiles()` (`banner.bin`, `savedata.bin`, `backup/`) | 100% |
| `0x80165BC0`, `0x80165C00` | `IsGameFlagSet(int)` (name from the `CPauseMenu` use) and `SetFlag(int, int)`: bit `flag & 7` of byte `flag >> 3` of the current slot's `SlotData::Data` | 100% |
| `0x80165C40` | `ResetSlot(int)`: `sSaveData->mSlotData[slot] = SlotData()` | 100% |
| `0x80165FB0` | `SlotData::~SlotData()` | 100% |
| `0x80166140`-`0x80167940` | `vector<T>::operator=` for `SD_PiiPersonalData`, `SD_PiiCollectionData`, `SD_BattleRoyalData`, `SD_StatisticValue` (three slot vectors share it), `UsedPassword` | 94-97% (register numbering only) |
| `0x80167E10` | `GetSaveData(int)` | 100% |
| `0x80167E50`-`0x80167E80` | `GetCurrentSlot()`, `SetCurrentSlot(u8)`, `GetOption()`, `SetOption(u8)` | 100% |
| `0x80167E90` | `CheckNandSpace()` | 99.95% (relocations) |
| `0x80168110` | `FormatWide<unsigned long>(const wchar_t*, const unsigned long&)` (file-local template over `NIntl::basic_formatter<wchar_t>`) | 92.8% |
| `0x801685C0`-`0x8016A540` | `__vec_constructor<T>::append(n, int2type<0>)` for five element types | 96% |
| `0x80169880` | `__vec_deleter<SlotData>::erase_tail` | 100% |

`SaveData::Data` is `{u8 mCurrentSlot; u8 mOption; u32 mSlotDigests[3][5]}`: bytes `+4` and `+5` of the object are read by `0x80167E50`/`0x80167E70`. The flag bitmap is the first 16 bytes of `SlotData::Data`.

## F-278-2: the vector shapes that reproduce the original

- **`erase_at_end` is a depth-limited out-of-line function.** The five tiny functions are `size_ -= n` for element types with trivial destructors. They are called, not inlined, from the large `DeserializeStruct` instances and inlined into `DeserializeVector`. Routing `resize`'s shrink through a one-line wrapper (`erase_at_end(n)` calling `erase_tail(n, pod_type())`) adds the inline level that reproduces this: `DeserializeStruct<SD_BattleRoyalData>` rose from 72% to 99.9%. The out-of-line function has no `count` copy of `n`; the destructor path of `__vec_deleter` still needs the copy (the `BWorld` and `CGloballyReusableTexBuf` destructors regress to 99% without it), so the two paths are separate functions. The user-declared `~VectorStructs() {}` of F-277-2 is wrong: with it the element destructors are non-trivial and a destroy loop appears.
- **`vector::assign` for non-POD elements.** The block order is: compute `n`; if `n > capacity` take the growth capacity from `recommend(n - capacity, pod_type())`; otherwise copy-assign `min(n, size)` elements, then `erase_at_end(size - n)` or `construct_at_end(mid, last)`, and return; the reallocation block (`clear`, free, `allocate`, `construct_at_end`) follows the `else`. `std::min(n, size_)` selects the first argument on equality (compare operands swapped relative to `std::min(size_, n)`). The element copy loop is unrolled only when written with a separate iterator variable (`for (Iterator it = first; it < mid; ++it, ++result)`); using `first` itself disables the unrolling.
- **`append` branching.** `(n <= cap && size <= cap - n) || grow_by(...)` compiles to branch-free arithmetic. Nested `if` statements with a `goto` into the construct arm keep compare-and-branch and the dead growth comparisons: `if (n <= cap) { if (size <= cap - n) goto construct; } if (grow_by(size + n - cap, pod_type())) { construct: construct_at_end(n, pod_type()); } else { append_realloc(n); }`.
- **Allocation of empty-class tag temporaries.** In functions with a frame pointer the by-value `int2type` and exception-flag temporaries get slots in this order: the function's own temporaries from the top downward in source order, then the temporaries of each inline callee that CW expands as a separate unit, appended in call order with each callee's list reversed. Splitting `assign` into callees for the growth capacity (`recommend(n - cap, pod_type())`), the in-place copy and the reallocation reproduces the exact slot sequence of the `0x80167470` instance; a single inline body or a helper taking a tag parameter does not.

## F-278-3: exception object frame layout

The inline `throw length_error(...)` stores `logic_error`'s message members at frame offsets (`stw r3, 0x8c(r31)`), not through a `this` register. It is reproduced by the MSL message holder that the standalone constructors at `0x80087010`-`0x80087120` show: `logic_error` holds a class whose constructor takes the `const char*` (`strlen`, `new char[]`, `shared_ptr<char>` with `default_delete<char[]>`, `strcpy`), and whose destructor chain gives the nested null checks at `0x800585B0`. Class name `std::__msl_error` is descriptive (low). With it the inlined throws match the original addressing in `append`, `operator=` and `ResetSlot` (it was the reason the vector functions stayed at 89% in F-277-2).

## F-278-4: NIntl formatter

`0x80168110` is a function template instance that builds a `NIntl::basic_formatter<wchar_t>` from the format string, applies `% arg` through an out-of-line weak `operator%` taking `const unsigned long&` (`0x801F3BF0`; the existing `include/NIntl/basic_formatter.h` gained the overload) and inlines `str()` and the result copy. The existing header declares the constructor and destructor out of line for the units that call them; `SaveData.cpp` defines both before use, which lets the compiler inline them here only. The reconstruction is the file-local `FormatWide<T>(const wchar_t*, const T&)` (`(formatter % arg).str()` on a named formatter, returning a copy of the result string); its size is 0x4A8 against 0x4B0.

## Remaining work

- `FormatWide` 92.8%: the original keeps a second callee-saved register for the formatter address and hoists the short-string data address out of the character loop.
- `append` instances (96%) and the `operator=` instances (94-97%) differ only in callee-saved register numbering around the inlined `length_error` throw: the original gives the exception-object pointer the highest of the three throw registers in `operator=` and the lowest in `append`, the reconstruction does not, and frame-offset stores (F-278-3) and register priority could not be obtained together.
- `SlotData` growth: the original calls `grow_by`, `construct_at_end` and `append_realloc` out of line (`0x80169A30`, `0x80169B40`, `0x8016BC60`) with the default constructor and the member copies inlined into the loop. Here the constructor stays a call inside loops and `append<SlotData>` inlines everything. These and the vector copy constructors (`0x8016B2E0`-`0x8016BA20`, #279) are not reconstructed.
- The destructor at `0x8016A4D0` (cleanup action of a `__vec_deleter`-like base in one extab) is unattributed.

## Traps

- **A constructor inlined into a loop can stay a call.** `SlotData()` inlined into `ResetSlot` but not into the `construct_at_end` loop or a placement-new expression, even with identical flags; judge inline decisions on the target's call graph, not on one use.
- **Splitting a template helper changes load sharing.** Moving the `recommend` capacity formula into its own function made CW reload `capacity_` after the throw check and cost 3%; keep the check and formula in one function.
- **A `charT` copy for `push_back` changes loop structure.** In the formatter loop, replacing the address-taken `c` by a copy dropped the function from 90% to 7%.
- **Header edits that look neutral regress other units.** Moving `erase_at_end` out of the class body, or deleting its `count` copy, regressed the `BWorld`, `CGloballyReusableTexBuf` and `CUIList` destructors from 100% to 99%; compare every unit's per-function scores in `report.json` against a seed build after touching `include/vector`.
- **Another agent may add the same header.** `include/NIntl/basic_formatter.h` and `include/game/NIntl.h` were created independently while this work was in progress; rebase early and reuse the merged header.
