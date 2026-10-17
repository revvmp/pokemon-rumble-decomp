# F-357-1: `pf_entry.c` ends at `0x802076C4`; `pf_entry_iterator.c` starts at `0x802076D0`

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #357. Compiler: Wii/1.0 `cflags_sdk` (F-350-1), default 16-byte function alignment.

F-356-2 split one unit, `pf_entry.c`, over `.text` `0x80205CD0`–`0x802095D4`. That range holds two translation units, matching VF's `pf_entry.c` and `pf_entry_iterator.c`:

| Unit | Ranges |
| --- | --- |
| `pf_entry.c` | `.text` `0x80205CD0`–`0x802076C4`, `.data` `0x803C4258`–`0x803C4264`, `.sdata2` `0x804BA5C0`–`0x804BA5C8` |
| `pf_entry_iterator.c` | `.text` `0x802076D0`–`0x802095D4`, `.sdata` `0x804B8878`–`0x804B8890` |

Evidence:

- **Inlining stops at the boundary in both directions.** Functions before `0x802076D0` call `PFENT_ITER_IteratorInitialize` and `PFENT_ITER_Advance` as real calls. These are `PFENT_searchEmptyTailSFN`, `PFENT_findEntry` and `PFENT_allocateEntry`. After the boundary, `PFENT_ITER_DoFindEntry` and `PFENT_LookupPath` inline both. In the other direction, `PFENT_IsNotMatchedEntry`, `PFENT_LoadMatchedEntry` and `PFENT_UpdateSFNEntry` inline `PFENT_CalcCheckSum`, `PFENT_LoadShortNameFromBuf`, `PFENT_loadEntryNumericFieldsFromBuf` and `PFENT_StoreEntryNumericFieldsToBuf`. From `0x802076D0` on, `PFENT_ITER_DoFindEntry`, `PFENT_ITER_FindCluster` and `PFENT_ITER_GetEntry` call them. Under `-ipa file`, one source file gives the first pattern everywhere. With both halves in one file, the three early callers grew by 80 to 170 instructions. After the split, all 20 functions before the boundary are identical.
- **The first function after the boundary is VF's first iterator function.** `0x802076D0` is `VFiPFENT_ITER_RecalcEntryIterator`, the first function of Petari's `src/RVL_SDK/vf/pf_entry_iterator.c`.
- **The data divides cleanly.** The `.sdata` strings `".."`, `"."`, `":"`, `"\\"`, `"/"` and `"\0"` are read only from `0x802076D0` on. The 12-byte `.data` string `"NO NAME    "` belongs to `PFENT_MakeRootDir`, and the `.sdata2` byte `0xE5` to `PFENT_RemoveEntry`. Both are before the boundary.
- `PFENT_RemoveEntry` ends at `0x802076C4`. The next function starts at the 16-byte boundary `0x802076D0`.

`lbl_804BA5C0` is now one byte long, not eight. The original object counted the alignment padding as part of the symbol. The linker still pads to `0x804BA5C8`, and objdiff then reports 100% for the data.

`pf_entry.c` is `Matching` and linked: objdiff reports 100% for its code and data, 20 of 20 functions. `pf_entry_iterator.c` is `NonMatching` and owned by this issue, because its first function is in #357's range. The functions from `0x80208620` belong to #358.

# F-357-2: The #357 functions

All 22 functions in #357's range are written. 21 are identical in the committed build. `PFENT_ITER_DoFindEntry` is identical only when `PFENT_ITER_Advance` and `PFENT_ITER_MoveTo` are also in `pf_entry_iterator.c`, because the target inlines `Advance`. I tested this with VF's two functions appended in a scratch copy. Those functions belong to #358 (`0x80208730` and `0x80208830`), so they are not committed. Without them, the build calls `Advance`, and `DoFindEntry` has 426 instructions instead of 462.

`pf_entry.c`:

| Address | Name | Confidence | Notes |
| --- | --- | --- | --- |
| `0x80206120` | `PFENT_searchEmptyTailSFN` | medium, VF | |
| `0x80206490` | `PFENT_findEmptyTailSFN` | medium, VF | |
| `0x80206620` | `PFENT_LoadShortNameFromBuf` | medium, VF | |
| `0x80206670` | `PFENT_loadEntryNumericFieldsFromBuf` | medium, VF | |
| `0x80206720` | `PFENT_StoreEntryNumericFieldsToBuf` | medium, VF | |
| `0x802067C0` | `PFENT_LoadLFNEntryFieldsFromBuf` | medium, VF | No `is_reverse` argument. It checks the masked ordinal against 20 before it reads the last-entry bit. |
| `0x802069A0` | `PFENT_storeLFNEntryFieldsToBuf` | medium, VF | |
| `0x80206B10` | `PFENT_findEntry` | medium, VF | Its sixth argument is the `u8` excluded-attribute mask that it passes to `PFENT_LoadMatchedEntry`, not `is_short_search`. With `PFPATH_ParseEntryIndex`, it searches one entry index. |
| `0x80206DD0` | `PFENT_SearchEntry` | low | |
| `0x80206E00` | `PFENT_allocateEntry` | medium, VF | VF's `ITER_DoAllocateEntry` body, with no argument checks. It always stores `*p_pos`. |
| `0x802070E0` | `PFENT_AllocateEntry` | low | Wraps `PFENT_allocateEntry` and discards the position, as `PFENT_SearchEntry` wraps `PFENT_findEntry`. Called at `0x8020EFD4` and `0x80215C10`. |
| `0x80207110` | `PFENT_GetRootDir` | medium, VF | No checks |
| `0x80207190` | `PFENT_MakeRootDir` | medium, VF | On FAT12 and FAT16, it zeroes the root directory sectors one at a time. On FAT32, it calls `PFFAT_MakeRootDir` and ignores the result. Other FAT types return 7. |
| `0x802072C0` | `PFENT_UpdateSFNEntry` | medium, VF | Only the `start_cluster == 1` check remains. It sets the archive bit only when `flag == 1`. |
| `0x80207420` | `PFENT_AdjustSFN` | medium, VF | |
| `0x80207500` | `PFENT_getcurrentDateTimeForEnt` | medium, VF | The date and time fields are 32-bit (`pf_system.h`) |
| `0x80207590` | `PFENT_RemoveEntry` | medium, VF | Always flushes the data cache |

`PFENT_storeShortNameToBuf` is VF's static helper, inlined into `PFENT_UpdateSFNEntry`. `PFENT_IsNotMatchedEntry` and `PFENT_LoadMatchedEntry` (F-356-2) now call `PFENT_LoadShortNameFromBuf` and `PFENT_loadEntryNumericFieldsFromBuf`, which are inlined there. They are still identical.

`pf_entry_iterator.c`:

| Address | Name | Confidence | Notes |
| --- | --- | --- | --- |
| `0x802076D0` | `PFENT_ITER_RecalcEntryIterator` | high, VF | When it allocates a new cluster, it zeroes each of the cluster's sectors directly. It does not call VF's `FillVoidEntryToSectors`. |
| `0x80207880` | `PFENT_ITER_DoFindEntry` | medium, VF | Arguments `(p_iter, p_ent, p_pattern, attr_required, attr_excluded, p_is_found)` |
| `0x80207FC0` | `PFENT_LookupPath` | medium | VF's `ITER_DoGetEntryOfPath`. The fifth argument is `wildcard` and the sixth `is_parent`; `pf_entry.h` had them reversed. |
| `0x80208520` | `PFENT_ITER_IteratorInitialize` | high, VF | No null check |

`PFENT_ITER_LoadEntry`, `PFENT_ITER_DoMoveTo` and `PFENT_ITER_LoadFoundEntry` are static inline helpers. The first two are VF's.

Callees named for the link:

| Name | Address | Basis | Confidence |
| --- | --- | --- | --- |
| `PFVOL_LoadVolumeLabelFromBuf` | `0x801FBBE0` | VF name. It copies 11 bytes to `PF_VOLUME.label` and terminates the string. FA takes `(p_vol, buf)`. | medium |
| `PFFAT_MakeRootDir` | `0x8020BEF0` | VF name; called in the FAT32 case | medium |
| `VFiPFPATH_cmpTailSFN` | `0x80212F20` | VF name; returns `pf_strcmp(...) != 0` | high |
| `VFiPFPATH_InitTokenOfPath` | `0x80212F50` | VF name and arguments | medium |
| `VFiPFPATH_GetNextTokenOfPath` | `0x80212F60` | VF name and arguments | medium |
| `PFPATH_MatchFileNameWithPattern` | `0x80213390` | VF name. It returns nonzero on a match. The third argument is 1 for the long name and 0 for the short name. | low |
| `PFPATH_CheckExtShortNameSignature` | `0x80214960` | VF name; compares the head of the name with a two-byte `.sdata2` string | medium |
| `PFPATH_ParseEntryIndex` | `0x80214CD0` | Descriptive. It parses a numeric entry index from a name in VF's extended-short-name form and stores it. | low |
| `PFSYS_TimeStamp` | `0x8021C8B0` | VF name | medium |

Layouts:

- `PF_VOLUME.tail_entry` at `0x1F74` is VF's `PF_DIR_TAIL`, 12 bytes: `tracker_size`, `tracker_buff[1]` and `tracker_bits`.
- `PF_SYS_DATE` and `PF_SYS_TIME` hold 32-bit fields: year, month and day, then hour, minute, second and millisecond.
- `PF_CACHE_PAGE.p_buf` at 8 is the buffer that `PFENT_MakeRootDir` and `PFENT_ITER_RecalcEntryIterator` zero and write.

## Traps

- **A unit boundary can show up only in inlining.** Trigger: `PFENT_ITER_DoFindEntry` was 606 instructions instead of 462, and three earlier functions grew once `PFENT_ITER_IteratorInitialize` was defined in the same file. Check which small same-file functions each caller inlines. A consistent change at one address marks a file boundary. Evidence: F-357-1.
- **An `if` that falls through to a block placed after an `else return` comes from `continue`.** Trigger: in the extended-name loop of `PFENT_ITER_DoFindEntry`, the target had `beq` to the retreat, then a bare `b` over a `li r3,0` return to the found block. Writing the retreat as the `for` increment with `if (!(buf[0] & 0x40)) continue;` reproduces it. Evidence: F-357-2.
- **Two variables can share a saved register when one path always returns.** Trigger: in `PFENT_ITER_DoFindEntry`, `err` is `r30` in the directory loop. In the extended-name path, the iterator status shares `r23` with `is_extsfn`. Using `err` on both paths, with `is_extsfn` as `u32` and `err` as `s32`, reproduces both; the compiler splits the two live ranges. Evidence: F-357-2.
- **The form of a `2`-or-`3` return changes the code.** Trigger: `return token.p_head == NULL ? 3 : 2;` adds a `neg` in `PFENT_LookupPath`. `return !token.p_head + 2;` matches. Evidence: F-357-2.
- **A packed date takes its fields lowest first.** Trigger: VF's high-to-low order swaps the `rlwimi` chain in `PFENT_getcurrentDateTimeForEnt`. Writing day, month and then year, and second, minute and then hour, matches. Evidence: F-357-2.
- **Bash heredocs in this tool environment drop one level of backslashes.** Trigger: `'\\'` in a heredoc reached the file as `'\'`. Write C sources that contain backslashes with a file-writing tool. Evidence: this session.
