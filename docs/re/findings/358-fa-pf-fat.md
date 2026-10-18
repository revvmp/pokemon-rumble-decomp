# F-358-1: `pf_entry_iterator.c` completed

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issues: #358, and the last #357 function. Compiler: Wii/1.0 `cflags_sdk` (F-350-1), default 16-byte function alignment.

All 16 functions of `pf_entry_iterator.c` (`.text` `0x802076D0`–`0x802095D4`, `.sdata` `0x804B8878`–`0x804B8890`) are identical to the target, and the unit is `Matching` and linked. objdiff reports 100% for code and data. `PFENT_ITER_DoFindEntry` (#357) became identical once `PFENT_ITER_MoveTo` and `PFENT_ITER_Advance` were in the file, as F-357-2 predicted.

| Address | Name | Confidence | Notes |
| --- | --- | --- | --- |
| `0x80208620` | `PFENT_ITER_IsAtLogicalEnd` | high, VF | |
| `0x80208730` | `PFENT_ITER_MoveTo` | high, VF | A global function here, not VF's static inline |
| `0x80208830` | `PFENT_ITER_Advance` | high, VF | |
| `0x80208930` | `PFENT_ITER_IsInDirectory` | low, descriptive | Not in VF. `(p_ent, cluster, p_is_found)`: builds a local iterator at the entry's sector and walks `..` entries up the tree, setting `*p_is_found` when it meets `cluster`. Its first cluster is computed without the `+ 2` offset |
| `0x80208CC0` | `PFENT_ITER_Retreat` | high, VF | No null check. When the sector changes, it sets the offset and returns 0 instead of calling `MoveTo` |
| `0x80208F20` | `PFENT_ITER_FindCluster` | high, VF | Returns 2 for a root directory entry. It loads the short name before it compares the cluster, and it clears `*p_is_found` when the final cluster test fails |
| `0x80209260` | `PFENT_ITER_FindEntryOfPath` | low | Resolves the root or current directory like `PFENT_LookupPath`, then calls `PFENT_ITER_DoFindEntry` |
| `0x802093D0` | `PFENT_ITER_FindEntry` | medium, VF | Returns 10 when `attr_required & attr_excluded` |
| `0x80209420` | `PFENT_GetEntryOfPath` | medium | `PFENT_LookupPath(..., 0, is_parent)` |
| `0x80209430` | `PFENT_GetParentEntryOfPath` | low | `PFENT_LookupPath(..., 1, 1)` |
| `0x80209440` | `PFENT_ITER_GetEntry` | low | |
| `0x802094B0` | `PFENT_ITER_LoadLongName` | low | Retreats over the LFN slots and copies their name fields |

The fifth argument of `PFENT_ITER_FindEntry` and `PFENT_ITER_FindEntryOfPath` is the `u8` excluded-attribute mask, and the fifth argument of `PFENT_GetEntryOfPath` is `is_parent`. `pf_entry_iterator.h` had `wildcard` for both. Every caller passes a constant, so the callers' code does not change.

# F-358-2: `pf_fat.c` boundaries and first functions

`pf_fat.c` is split as `.text` `0x802095E0`–`0x8020C72C` and `.rodata` `0x80368188`–`0x803681C4`. It is registered `NonMatching`.

- The `.text` start is the first routine after `pf_entry_iterator.c` (F-356-2). The end is the end of `PFFAT_GetValueOfEOC2`, VF's last `pf_fat.c` function. `0x8020C730` starts the FAT12 entry routines (`pf_fat12.c`).
- The only data object is VF's `fat_special_values` table, 60 bytes (three entries of `bad`, `eoc1`, `eoc2`, `fat0_mask` and `fat1`). Only this range reads it. The four bytes after it are alignment padding. The symbol was resized from `0x40` to `0x3C`, and `lbl_803681C8` gets `align:8` so the next object keeps its address.

17 functions, all of #358's range, are written. 16 are identical:

| Address | Name | Confidence | Notes |
| --- | --- | --- | --- |
| `0x802095E0` | `PFFAT_ReadFATSector` | medium, VF | Reads into a page the caller gives: `(p_vol, p_page, cluster)` |
| `0x80209730` | `PFFAT_SearchForNumFreeClusters` | medium, VF | Allocates its own data page. The wraparound test compares with `end_cluster` |
| `0x80209970` | `PFFAT_ClearCluster` | low, descriptive | Not in VF. Zeroes every sector of a cluster and always returns 0 |
| `0x80209AA0` | `PFFAT_FindClusterLink` | medium, VF | No FAT sector read |
| `0x80209C30` | `PFFAT_FindClusterLinkPage` | medium, VF | |
| `0x80209DD0` | `PFFAT_ReadCluster` | low | The non-page form of VF's `ReadClusterPage` |
| `0x80209F90` | `PFFAT_ReadClusterPage` | medium, VF | |
| `0x8020A160` | `PFFAT_WriteCluster` | medium, VF | |
| `0x8020A380` | `PFFAT_WriteClusterPage` | medium, VF | |
| `0x8020A5A0` | `PFFAT_DoAllocateChain` | medium, VF | Sixth argument: clear the first cluster with `PFFAT_ClearCluster`. It writes every active FAT copy with `PFSEC_WriteFAT` |
| `0x8020A900` | `PFFAT_GetClusterInChain` | medium, VF | Uses `last_access` but not the hint |
| `0x8020AC80` | `PFFAT_GetClusterContinuousSectorInChain` | medium, VF | |
| `0x8020AD90` | `PFFAT_GetClusterAllocatedInChain` | medium, VF | |
| `0x8020AE80` | `PFFAT_GetClusterSpecified` | medium, VF | Also copies `chain_stamp` into the hint |
| `0x8020B010` | `PFFAT_GetClusterAllocated` | medium, VF | Four arguments, no cluster count |
| `0x8020B130` | `PFFAT_GetSector` | medium, VF | 99.5%: see below |
| `0x8020B290` | `PFFAT_GetSectorSpecified` | medium, VF | No argument checks |

`ReadFATEntry`, `ReadFATEntryPage`, `WriteFATEntry`, `WriteFATEntryPage`, `UpdateClusterLink`, `ClearClusterLink`, `UpdateFATEntry`, `AllocateChain`, `GetNumberOfCluster`, `GetNumberOfAllocatedCluster` and `GetSectorInRootDirRegion` are static inline helpers. The FA page routines take `PF_CACHE_PAGE*`, not VF's `PF_CACHE_PAGE**`.

`PFFAT_GetSector` differs only in two registers: the target keeps `sectors_per_cluster` in `r7` and the first quotient in `r8`, and the build swaps them. Declaration order, statement order, operand order, temporaries, branch order, structure of the root-directory helper and Wii/1.1 and 1.3 all give the same swap or worse.

Callees named for the link: `PFFAT_GetBeforeChain` (`0x8020BB30`) and `PFFAT_RefreshFSINFO` (`0x8020C0C0`), VF names, medium; `PFSEC_ReadFAT` (`0x80214EC0`) and `PFSEC_WriteFAT` (`0x80215290`), VF names by their place next to `PFSEC_ReadData` and `PFSEC_WriteData`, medium; and the twelve FAT entry routines from `0x8020C730` to `0x8020D7A0`, which `main` had already named `PFFAT12_ReadFATEntry`, `PFFAT12_WriteFATEntry`, `PFFAT12_WriteFATEntryWithBuf`, `PFFAT12_ReadFATEntryWithBuf` and the same for FAT16 and FAT32. The dispatch by FAT type and the page argument agree with those names. FAT12 takes `u16` cluster and value.

## Layouts

`PFFAT_ReadFATSector` calls `PF_VOLUME.p_callback` (`0x1FA4`) with `last_driver_error` after a driver error 4096, which agrees with the layout already in `pf_volume.h`.

## Traps

- **Expanded helper code decides auto-inlining.** Trigger: `PFFAT_WriteCluster` and `PFFAT_WriteClusterPage` were inlined into their callers, which grew from 216 to 452 and from 223 to 341 instructions. CodeWarrior measures a function before its own inline calls are expanded. VF's `err = PFFAT_UpdateClusterLink(...); if (err != 0) return err;`, though the helper always returns 0, makes them large enough to stay calls. Evidence: F-358-2.
- **Stack slots do not follow declaration order alone.** Trigger: `PFFAT_ClearCluster` put its page pointer at 12 instead of 16 for most orders of its three address-taken locals. Test every order of the address-taken locals, not only reversals. Evidence: F-358-2.
- **A byte offset can be strength-reduced away.** Trigger: `PFENT_ITER_LoadLongName` copied through a pointer that the compiler advanced by 26 each pass. `pos = i * 26;` with `(u8*)p_ent->long_name + pos` matches; `long_name[num * 26 / sizeof(u16)]` gives the `mulli`, `clrrwi` terminator. Evidence: F-358-1.
- **Unsigned shifts of `u16` fields need a cast.** Trigger: `iter.offset >> 5` gave `srawi`; `(u32)iter.offset >> 5` gives the target's `srwi`. Evidence: F-358-1.
