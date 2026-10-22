# Sprint a16 findings

## Results

- `ScriptPii`: `showPiiInfoWorldObjStat`, `getNodeY`, `showHPBar` match at 100%. `SearchByGroupNo` needs a `groupNo != 0` guard around the scan, and `FindEnemyByGroupNo`/`CountEnemyByGroupNo` need one `CPpuEngine* pEngine` variable used for every engine call. 119 of 171 functions are at 100%, the rest are weak template copies.
- `CPiiInfoWorldObjStat` (0x104 bytes: vtable, `mFlags` 4, `mPosition` 8, `CLayout` 0x14, `shared_ptr<CPiiPersonalData>` 0xFC) has a header; its constructor is `0x800E5090` and `Load` is `0x800E5190`.
- `PhotoSave`: binding the second `shared_ptr<u8>` through a named `pStack` reproduces the target order of `Allocate` before the deleter load (98.31% to 98.63%).

## Traps

- **A unit with weak template copies links short when the original kept every copy.** `ScriptPii` as `Matching` links but the DOL is 0xC0 bytes short: the original keeps its own `IsKindOf<CPlayerPii>` and `DynamicCast<CPii>` copies (dtk marks them `scope:local` or leaves placeholders at `0x80154450`, `0x80154490`, `0x801540F0`), but our weak copies are deduplicated against the earlier global definitions with the same name. Evidence: `elf` symbol list versus `symbols.txt` sequence. Until duplicate copies are renamed to placeholders for every earlier copy, such units stay `NonMatching`.
- **Reference-map names can sit on the wrong type.** `0x80065D60` was named as the `tree_imp` node `alloc_ptr` destructor but is the `list<BWorldObj*>` node one (ScriptPii `insert` calls it). With the wrong name our own copy was not deduplicated and the DOL grew by 0x80 bytes. It is renamed now.
- **Extending a split pulls referenced data into the unit.** `ScriptPii` `.data` was cut at `0x803B71B8` and `.sdata` at `0x804B7A90`, leaving the `default_delete` RTTI at `0x804B7A68` and the exception strings to auto regions that referenced the unit's local labels. The splits now cover `0x803B6FA8-0x803B72F8` and `0x804B7A68-0x804B7AB8`.
- **A shape matcher renames placeholders safely.** Comparing normalised mnemonic and register sequences of a placeholder against the rebuilt object's functions finds the template instantiation (unique matches only; same-shape groups need a stable order). Names that already exist elsewhere in `symbols.txt` must not be reused.
- **SaveData `SlotData` vector helpers.** The target has `can_append` (268 bytes, `0x80169A30`), `construct_at_end` (1004 bytes), `append_realloc` (2832 bytes) and one out-of-line copy constructor per vector member plus `SlotData::Data` (428 bytes, `0x8016B130`). Forcing them out with `noinline` reproduces the sizes of can_append, the five vector copy constructors and the destructors exactly, but is not an acceptable fix and breaks other vector users. The remaining difference is that our `Data` copy is inlined and `append` is not pruned. Adding statements to `can_append` did not change the structure, so the budget model in sprint-i1 does not fully explain this case.
