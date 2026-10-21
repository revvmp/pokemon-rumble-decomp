# Issue 147: CMsgBalloon stage models and the std::map<unsigned long, bool> chain

Target `wpse-v1`, checked with `tools/re/unitdiff.py src/game/CMsgBalloon.cpp`, a full `ninja`, `tools/verify/verify.py` and a per-function comparison of `build/wpse-v1/report.json` against a fresh baseline of `origin/main`. No decompiler output was used.

## F-147-1: the 24 functions of 0x800C88D0-0x800C9424

Result: every function of the block compares 100% in objdiff apart from relocation names of callees that other issues own. `SetupStageModels` is 99.37% (only the names of its string and float literals differ), the `red_black_tree` destructor is 99.77% (its call to the `tree_imp` destructor at `0x800CA660`) and the `map` iterator `operator==` is 95.00% (its tail call to the `tree_imp` iterator comparison at `0x800C9430`). The other 21 are 100.00%. Confidence high for structure, medium for names. Full `ninja` links and `verify.py` passes; per-function scores of every other unit are unchanged or higher against the baseline.

| Address | Function |
| --- | --- |
| `0x800C88D0` | `CMsgBalloon::SetupStageModels(StageID)` (descriptive name, medium) |
| `0x800C8FE0`, `0x800C9010`, `0x800C9040`, `0x800C9070` | constructors of `std::map<unsigned long, bool>`, `detail::map_base`, `detail::tree`, `detail::red_black_tree` |
| `0x800C90A0` | `detail::tree_imp` constructor |
| `0x800C9110`, `0x800C9140`, `0x800C9180` | `compressed_pair<unsigned long, allocator<value_type>>(unsigned long)`, its `CompressedPairStorage<..., 2>` and `allocator<value_type>::allocator()` |
| `0x800C9190`, `0x800C91C0`, `0x800C9200`, `0x800C9210` | `compressed_pair<allocator<node>, tree_root_ptr>()`, its storage constructor, `allocator<node>::allocator()` and `tree_root_ptr(tree_node_base*)` |
| `0x800C9220`, `0x800C9250`, `0x800C9260`, `0x800C9270` | `tree_begin_holder<Compare>()`, `map_value_compare()`, `tree_imp::begin_node()` and `tree_begin_holder::second()` |
| `0x800C9280`, `0x800C9290` | `tree_imp::end_node()` and the root pair's `second()` |
| `0x800C92A0`, `0x800C9300`, `0x800C9360`, `0x800C93C0` | destructors of `red_black_tree`, `tree`, `map_base`, `map` |
| `0x800C9420` | `map_base::iterator::operator==` |

### SetupStageModels

`SetupStageWarp` calls it for stage kinds other than 2 with a copy of its `StageID`, which the callee modifies through the pointer: it is the same 32-bit value as a `MapIndex`, and `mMap` (the low byte) runs from 1 to 9. For each map it fetches `IGameSession::GetMapDesc` (vtable offset `0x40`), tests the save record of the map for flag 2 and walks the `MapDesc` table at `+0xC`. Row cell 2 equal to 7 skips the row. Cells 0 and 1 are 16-bit monster numbers, accepted from 1 to 495, and each is merged into a `std::map<u32, bool>` that records whether any map holding that monster is cleared (`unlocked[id] = cleared` for a new key, `unlocked[id] |= cleared` for a known one).

The map size, limited to 18, resizes `mModelTextures`. The collection box (`IGameSession` vtable offset `0x28`) is asked for the collection type of every entry whose flag is set, and the entries are walked again to fill up to 18 model slots: a 256 by 256 `CPiiTex` is built from the `CPiiPersonalData` that `CreatePiiPersonalData(oyaInfo, GetPiiSpeciesData(id, 0), 0)` returns, set on the `PII_TEX` and `PII_TEX_GRAY` textures of `mModelLayouts[i]`, and the panes `PII_TEX`, `PII_TEX_GRAY`, `NOT_YET` and `PII_UNKNOWN` show collection type 3, type 2, the motion `pii_not_yet` or `init`, and types 0 and 1. Remaining slots are hidden and the pane `OBTAIN` of the warp layout shows message `0x600C9` with the number of type 3 entries and the unclamped size of the map.

New outside names, all descriptive and medium confidence: `GetPiiSpeciesData(u16, int)` `0x80124EE0` (the 28-byte record lookup of F-115-6), `CreatePiiPersonalData(const COyaInfo*, const PiiSpeciesData*, u32)` `0x800EE800` (a `shared_ptr` by value that reads two words of the `COyaInfo` for the id and original trainer), `CPiiCollectionBox::GetCollectionType(u16)` `0x800E1C10` ([F-91-1](91-cdlgpiiselector.md) slot 8 object), `MapPiiTable::GetCell(int, int)` `0x80110100` (8-byte cells, first row skipped, stride at `+0x28`) and the instance `FormatMessage<int, unsigned long>` `0x800CB4F0`. `MapPiiTable` is the object at `MapDesc + 0xC` (cells pointer at 0, row count at `0x24`, column count at `0x28`); its row-count accessor `0x800C95C0` belongs to #148.

## F-147-2: the layers of std::map

The local map is 12 bytes and its constructor and destructor are four wrapper layers deep because the block is compiled with `#pragma dont_inline on`. The destructor flags (0 for a base, -1 for a member) give the shape: `map` derives from `detail::map_base`, which holds the tree as a member; `tree` derives from `red_black_tree`, which derives from `tree_imp` (the classes `set` already used). `tree_imp` is `compressed_pair<size_type, Allocator>` at 0, `compressed_pair<node_allocator, tree_root_ptr>` at 4 and a `tree_begin_holder<Compare>` (a `Compare` base and one pointer) at 8; `end_node()` is the address of the root slot and `root()` is the same address as a reference, which is why `0x800CBBF0` is a one-instruction tail call. The tree constructor stores `end_node()` through `begin_node()`.

What the target shows and the code now reproduces:

- A `tree_root_ptr` with a defaulted argument produces `li r4, 0` plus the call to `0x800C9210`; a plain pointer in `compressed_pair` stores inline.
- The begin holder's base must have a user-declared constructor (`map_value_compare()`), or the holder's implicit constructor and the `blr` at `0x800C9250` are not emitted.
- `map<int, std::list<int> >` in `CPpuEngine` is 16 bytes (begin pointer at `+0xC`, [F-195](195-cppuengine.md)), so a non-POD value selects `map_member_compare`, whose `Compare` member makes it non-empty. POD values with an empty compare give 12 bytes, and pointer-sized POD values keep the `map_compact_tree` of #46.
- `std::map::find(key)` goes through `tree_imp::find<Key>` with the compare overloaded for a key and a value; `operator[]` inserts and returns `second`.

## F-147-3: statement shapes that matter

- `anyCleared |= cleared; allCleared &= cleared;` keeps both loop-carried booleans in the target (`or`, `and`, then the `neg`/`or`/`srwi` normalisation); `x = x | y` lets the compiler delete them.
- `1 <= first && first < 496` gives the two separate unsigned compares of the target; `first >= 1 && first < 496` and `first > 0 && first < 496` fuse into a range check or a record-form compare.
- A `u16 first` converted to a temporary `unsigned long` at each `map` call stores four separate temporaries; a `u32 first` binds the variable itself.
- `MapPiiTable* pTable = &pDesc->mPiiTable;` keeps the table address in `r27`; repeating `pDesc->mPiiTable` recomputes `addi r3, r30, 0xC`.
- The first `unlocked.size()` is clamped in a variable that is later passed by reference, so it lives in memory; the message recomputes `total = unlocked.size()` (the clamp is for the textures only).
- The `switch` lists `case 3`, `case 2`, then `case 0` and `case 1`; the dispatch (`cmplwi 1`, `3`, `2`) and the block order follow the source order.

## Remaining differences

- Relocation names of literals in `.data`, `.sdata` and `.sdata2`, and of the neighbouring functions `0x800C9430`, `0x800CA660`, `end`, `find`, `operator[]`, `begin`, `operator++`, `operator!=`, `size` and `resize`, which belong to #148 onwards.
- `BWorld` still compiles its `map<unsigned long, CEffect*>` with `map_compact_tree`; `RemoveOldestEffect` rose from 96.74% to 99.03% and its map destructor from 98.31% to 98.88% with the new `map_base` layer, which is further evidence that the base class exists.

## Traps

- **A silent no-op edit on a CRLF file.** A Python replacement written with `\n` matched nothing in a CRLF source and the build looked unchanged. Normalise to LF around the replacement, restore CRLF afterwards, and confirm the diff before judging a result.
- **dont_inline layers decide template shape.** If the target shows four or five tiny functions in a row for one constructor, read the destructor flags (0 base, -1 member) in the wrapper layers before inventing a class; they give the inheritance and membership exactly.
- **An empty-looking comparator can add 4 bytes.** A map whose compare class has a member is 16 bytes; one whose compare class is empty is 12. `CPpuEngine` caught this: making every `std::map` 12 bytes moved all later members by 4 and dropped four Matching units from 100%.
- **Overlapping compares fuse unless the constant is on the left.** `1 <= x && x < 496` stays as two compares; `x >= 1 && x < 496` becomes a range check.
