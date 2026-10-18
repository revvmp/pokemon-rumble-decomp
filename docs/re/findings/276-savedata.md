# Issue 276: SaveData.cpp

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf` and the dtk split assembly. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #276 lists 23 functions in `.text` `0x8015EEB0`–`0x801613E4`, the first quarter of the save-data block (#276 to #279). No decompiler output was used.

## F-276-1: unit boundaries

`0x8015EEB0`–`0x8016CB18` is one translation unit, registered as `game/SaveData.cpp` (descriptive, medium: no source-file string is embedded; the RTTI class names `SaveData` and `SlotData` are in its `.data`).

- **Start.** The preceding `0x8015ECE0`–`0x8015EE70` are the weak `shared_ptr_deleter<CWaza, default_delete<CWaza>>` members of the script-binding file, whose vtable, type names and type info end at `.data` `0x803B73DE` and `.sdata` `0x804B7AC4`. `0x8015EEB0` writes the NAND banner and uses the first object of the next `.data` block, the `SlotData` child-type table at `0x803B73E0`.
- **End.** The only `.ctors` entry between `0x80131780` and `0x80174D70` is `0x803529F4`, the static initializer `0x8016CAF0`, which registers the destructor of the global `shared_ptr<SaveData>` (`.sbss` `0x804B90D0`, destructor-chain node `.bss` `0x803E9900`). `0x8016CB20` (#280) writes `.sbss` `0x804B90D8`, and `0x8016CB90` reads `.data` `0x803B7D90` and `.sdata` `0x804B7B10`.
- **One file.** The `SlotData` vtable (`.data` `0x803B7928`) references the inline getters at `0x80160CB0`–`0x80160D10` and lies after the jump tables of `0x80161E90`–`0x80165AB8` and after the `SaveData` deleter cluster of `0x8016AAE0`; vtables are emitted after a file's function data, so `0x8015EEB0` and the later functions share one data block. No static initializer, deleter cluster or string cluster separates them.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80032AA8`–`0x80033D8C` | Entries of the records below |
| `extabindex` | `0x8004FA7C`–`0x8004FD94` | First record names `0x8015EEB0`, last `0x8016CAF0` |
| `.text` | `0x8015EEB0`–`0x8016CB18` | Ends after the static initializer |
| `.ctors` | `0x803529F4`–`0x803529F8` | `__sinit_\SaveData_cpp` |
| `.data` | `0x803B73E0`–`0x803B7D90` | Child-type table, paths, jump tables, deleter vtables, RTTI names and vtables |
| `.bss` | `0x803E9900`–`0x803E9910` | Destructor-chain node; `0x803E9910` belongs to `NPpuVM.cpp` |
| `.sdata` | `0x804B7AC8`–`0x804B7B10` | `SaveData` child-type table, `"./"`, `"backup/"`, `"nand:"`, type info |
| `.sbss` | `0x804B90B8`–`0x804B90D8` | Five element child-type tables and `sSaveData` |

The split alone links and verifies. The unit spans #276 to #279; the later issues add their functions to the same source file.

## F-276-2: save structures

Every saved structure derives from `VectorStructs` (RTTI `0x804B68D0`), whose vtable has eight getters that return `std::vector<...>*` and default to null (`0x800BAD70`–`0x800BADE0`, emitted in the `CGameSession` file). A structure is a vptr, a POD `Data` member and the vectors it owns. Each class has a static child-type table, a zero-terminated list of `VectorStructType` values naming the vectors serialized after its data.

| Class | Size | `Data` | Child types | Evidence |
| --- | --- | --- | --- | --- |
| `SD_PiiPersonalData` | `0x30` | `0x2C` at 4 | none (`.sbss` `0x804B90B8`) | `memset` of 0x2C and vtable `0x803A3B48` in `0x80163C50` |
| `SD_BattleRoyalData` | `0x8` | 4 at 4 | none (`0x804B90BC`) | |
| `SD_StatisticValue` | `0x8` | 4 at 4 | none (`0x804B90C0`) | Three vectors of it |
| `SD_PiiCollectionData` | `0x14` | `0x10` at 4 | none (`0x804B90C4`) | |
| `UsedPassword` | `0x8` | 4 at 4 | none (`0x804B90C8`) | `ScriptBinding_IsPass...` and `ScriptBinding_SetPas...` use the vector at slot `0x1350` |
| `SlotData` | `0x1360` | `0x1300` at 8, 8-aligned | 1, 2, 4, 5, 6, 7, 8 (`.data` `0x803B73E0`) | `GetSaveData`, the member copy `0x8016B130` |
| `SaveData` | `0x50` | `0x40` at 4 | 3 (`.sdata` `0x804B7AC8`) | Global `shared_ptr<SaveData>` |

`VectorStructType` values and the matching getter slots: 1 `PiiPersonalData` (+0x8), 2 `BattleRoyalData` (+0x10), 3 `SlotData` (+0x14), 4 to 6 `StatisticValue1` to `3` (+0x18 to +0x20), 7 `PiiCollectionData` (+0xC), 8 `UsedPassword` (+0x24). `SlotData` keeps the vectors at `0x1308`, `0x1314`, `0x1320`, `0x132C`, `0x1338`, `0x1344` and `0x1350` in getter-slot order; `SaveData` keeps `vector<SlotData>` at `0x44`.

The type names come from RTTI strings. Their assignment to the five element shapes is medium confidence: `SD_PiiPersonalData` and `UsedPassword` are confirmed by the uses above, and the other three follow from the RTTI data order, which is the reverse of the child-type table order of the five classes, as declaration order predicts under `-ipa file`. Getter, enum and `Data` member names are descriptive (low).

`SlotData::Data` is 8-aligned: the member copy `0x8016B130` copies a 64-bit value at `0x12D8`, and `CGameSession` stores the two halves of a 64-bit value at slot `0x12FC` and `0x1304`. The map records of F-141 are its `mMapRecords` at offset `0x10` (slot `0x18`). `SaveData::Data` holds the current slot byte at 0 (`GetSaveData(-1)`) and the three slot digests at 4.

## F-276-3: functions

| Address | Function | Match |
| --- | --- | --- |
| `0x8015EEB0` | `WriteBanner(int mode)`: builds the `NANDBanner` from `savebanner.arc` TPLs into `/tmp/banner.bin` (mode 1 writes only, 2 moves only) | 94.00% |
| `0x8015F9A0` | `list<shared_ptr<FileList>>` destructor | 100% |
| `0x8015FA80` | `Save(int mode)` | 100% |
| `0x801601A0` | `SerializeStruct<SaveData>` | 100% |
| `0x801603B0` | `SaveData::GetSlotData()` | 100% |
| `0x801603C0`–`0x80161300` | `SerializeVector<T>` and `SerializeStruct<T>` for `UsedPassword`, `SD_StatisticValue`, `SlotData`, `SD_PiiCollectionData`, `SD_PiiPersonalData` and `SerializeVector<SD_BattleRoyalData>` | 100% |
| `0x80160CB0`–`0x80160D10` | The seven `SlotData` getters | 100% |
| `0x801613F0` | `SerializeStruct<SD_BattleRoyalData>` (#277; emitted by the same template) | 100% |

`SerializeStruct` copies `Data` into the buffer, then for each child type writes the type byte and serializes that vector. `SerializeVector` writes the element data size and count as two halfwords and, when given a digest array, stores the SHA-1 of each element's bytes; only the `SlotData` vector receives the digest array, which is `SaveData::Data`'s slot digests. `Save` serializes into a 0x34000-byte MEM2 buffer at offset 0x18 behind a header of SHA-1 digest, version 4 and data size 0x40, scrambles each 16-byte block by adding bytes of the previous block, writes `/tmp/savedata.bin`, moves the old `savedata.bin` into `backup/` and the new file into place, and commits the banner. NAND errors go through one inline switch that shows message `0x1001D`, `0x1001F` (fatal) or `0x10020`.

Outside the unit, renamed for the link (descriptive, low unless stated): `ShowSystemMessage(const wchar_t*, bool)` `0x801E09B0`, the shared empty default text `gEmptyText` `.sdata` `0x804B5C48` (passed as the `GetMessageText` default in 49 files), `FormatString(const char*, ...)` `0x800721A0` (returns `std::string`, F-60), `LoadFileList` `0x80127CF0` and `LoadResourceFileList` `0x80072DD0` (return `shared_ptr<list<shared_ptr<CFileData>>>`; the list is created at `0x80128084`, and the wrapper ORs the flags with 0x2000), and the `shared_ptr_deleter<NANDBanner, void (*)(void*) throw()>` vtable `0x803B7878` (high: compiler name).

## Remaining work

- `WriteBanner`: the inlined `list::push_back` keeps two node pointers and the end node in stack temporaries (`0x18`/`0x1C`, `0x30`/`0x34`) and stores an empty allocator byte before allocating, and the loop keeps the short-string data address of the temporary path in r27. Both come from MSL `list` and `basic_string` internals that the repository headers do not reproduce (#327, #328); the rest of the function matches except the registers that follow from them.
- The object also emits the base destructors of that list type and the `std::string` destructor, which the original file does not contain; the unit stays `NonMatching` until #277 to #279 are done and those are resolved.

## Traps

- **String literal and jump table relocations match only at the original data offsets.** `Save` and the serializers scored below 100% until `WriteBanner`, which owns the first literals of `.data`, existed; then every literal and jump table landed at its original offset and objdiff paired them. A missing earlier function can make later relocation-only rows look wrong.
- **A small out-of-line callee is inlined into its only caller.** A placeholder `WriteBanner` was inlined into `Save` and switched `Save` to base-register addressing of `.data`.
- **Locals declared before a loop take higher registers.** `SerializeVector` and the `Save` scramble loop matched only with the digest pointer and the block pointers declared ahead of the loop, as the author's style suggests.
- **`sed -i` in Git Bash converts CRLF files to LF.** Re-normalize line endings after scripted edits.
