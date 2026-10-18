# Issue 277: SaveData.cpp loading

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, read in the dtk split assembly of `game/SaveData.cpp` ([F-276-1](276-savedata.md#f-276-1-unit-boundaries)). Issue #277 covers `.text` `0x801613F0`–`0x80165AD4`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). No decompiler output was used.

## F-277-1: functions

| Address | Function | Match |
| --- | --- | --- |
| `0x801613F0` | `SerializeStruct<SD_BattleRoyalData>` (#276 PR) | 100% |
| `0x801615F0`–`0x80161630` | `GetDataSize()` of `SD_PiiCollectionData`, `SlotData`, `SD_StatisticValue`, `UsedPassword`, `SaveData` (`return sizeof(Data)` as `u16`) | 100% |
| `0x80161640` | `Load(bool backup)` | 70% |
| `0x80161E90`, `0x80162670`, `0x80162E50`, `0x80163270`, `0x80163A90` | `DeserializeStruct<T>` for `SaveData`, `UsedPassword`, `SD_StatisticValue`, `SlotData`, `SD_PiiCollectionData` | 99.86% (jump-table relocation only) |
| `0x80162050`, `0x80162830`, `0x80163010`, `0x80163430`, `0x80163C50`, `0x80164F60` | `DeserializeVector<T>` for `UsedPassword`, `SD_StatisticValue`, `SlotData`, `SD_PiiCollectionData`, `SD_PiiPersonalData`, `SD_BattleRoyalData` | 89% (`SlotData` 2%) |
| `0x80164360`, `0x80165580` | `DeserializeStruct<SD_PiiPersonalData>`, `DeserializeStruct<SD_BattleRoyalData>` | 74%, 71% |

`Load` replaces `sSaveData` with a new `SaveData`, resizes its slot vector to three, loads `nand:savedata.bin` (or `nand:backup/savedata.bin`) through `LoadResourceFile` (`0x80066930`, a wrapper that ORs the flags with 0x2000), and returns 0x65 when the file is missing or failed, 0x66 when empty, 0x68 when shorter than the 0x18-byte header, 0x67 when the SHA-1 of everything after the digest differs, 0x69 when the version is not 4, and 0 after deserializing. It undoes the `Save` scramble block by block from the last 16-byte block down, using signed arithmetic on the file size.

`DeserializeStruct<T>(T&, u32 size, const u8*)` clears `mData` when `size` is below `T::GetDataSize()`, copies `size` bytes, and then reads type bytes until 0, passing each named vector to `DeserializeVector`; unknown types are skipped. Only the `for (;;)` loop with the `return` as the last `case` reproduces the jump table and epilogue; a `do`/`while` or flag loop rotates the loop or keeps the type in a register. `DeserializeVector<T>` reads the element size and count halfwords, `resize`s the vector and deserializes each element.

The helpers that the template does not inline appear only for the five types whose `DeserializeStruct` is out of line: `GetDataSize` before `Load`, `ClearData` (`memset` of `mData`, `0x80165B00`–`0x80165B70`, #278) after the template block. The compiler emits them in these positions from the natural template code.

## F-277-2: class and library evidence

- **Constructors.** Each save structure's constructor clears `mData` (the temporary in `vector::resize` stores the vtable and `memset`s the data). `CMapRecord` has a constructor that zeroes its value: the `SlotData` temporary at `0x80165C40` stores the first record and zeroes the other 1,199 in a loop.
- **`VectorStructs` destructor.** With implicitly trivial destructors the compiler folds `size_ -= size_` to a zero store; the original keeps `subf r0, rX, rX` in every inlined vector `clear`. A user-declared non-virtual `~VectorStructs() {}` reproduces it and raises the vector functions from 81% to 89% and `Load` from 57% to 67%. The vtable has no destructor slot, so it is not virtual.
- **`vector::append`.** For `SlotData` the original calls three out-of-line members: `0x80169A30` takes the already computed shortfall `size + n - capacity`, throws `length_error` if it exceeds `max_size() - capacity` and returns false; `0x80169B40` is `construct_at_end(n)` and `0x8016BC60` is `append_realloc(n)`. `append` therefore constructs in place when the capacity suffices or the growth helper succeeds, and reallocates otherwise. `include/vector` now has this shape (`recommend`, `grow_by(n, pod_type)` returning `bool`). In the inlined instances the original also keeps the dead capacity comparisons of the growth helper, which the current header does not reproduce.
- **`length_error` helper.** `__throw_vector_length_error` is now `static inline`, so its literal is a local string of each unit, as the original `vector length error` at `.data` `0x803B75B4` is.
- **Vector copy constructor.** `SlotData`'s implicit copy needs `vector(const vector&)`; the original copies are out of line at `0x8016B2E0`–`0x8016BA20` (#279), allocating `last - first` elements and copy-constructing them.

## Remaining work

- Every inlined `length_error` throw stores the `logic_error` members at frame offsets (`stw r3, 0xa4(r31)`), where the current `stdexcept` and `memory` headers address them through a `this` register (`stw r3, 0x4(r28)`). No unit built from source reproduces the frame-offset form yet (`BWorld`, `Message` show the same difference). This and the missing dead comparisons keep the vector functions near 89%.
- `SlotData` vector growth: the original keeps `grow_by`, `construct_at_end` and `append_realloc` out of line because their `SlotData` bodies (inlined constructor, `SlotData::Data` copy `0x8016B130`, vector copies) are large; here they are inlined. This needs the #278/#279 `SlotData` constructor, destructor and copy, and blocks `DeserializeVector<SlotData>`, `Load`'s `resize(3)` and the two large `DeserializeStruct` instances.
- The `BWorld` `vector<shared_ptr<CEffect>>::resize` and `SetCamera`, both non-matching, dropped from 93.95% to 92.28% and 91.22% to 90.33% with the new `append`; no function that was at 100% changed.

## Traps

- **`python -` hangs in Git Bash** even with `2>/dev/null` before a heredoc command; write scripts to the scratch directory and run them as files.
- **A `||` condition inside an inlined helper becomes branch-free arithmetic.** `if (a > b || c > d)` in `append` turned into `orc`/`srwi` sequences once the helper returned `bool`; the original keeps compare-and-branch. Prefer separate `if` statements when the original branches.
- **symbols.txt names are unquoted even with `<>` and commas.** Quoting a renamed template symbol is not the file's convention.
