# F-355-1: FA `pf_cluster.c`

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #355. Compiler: Wii/1.0 `cflags_sdk` (F-350-1), default 16-byte function alignment.

`pf_cluster.c` follows `pf_cache.c` and occupies `.text` `0x80200010` to `0x80201200`. Its initial seven-function result and remaining register mismatch are historical below. [F-355-5](355-complete-inventory.md) resolves the final mismatch: all eight functions and the complete section are now 100%, the unit is `Matching`, and the full source-linked executable verifies exactly.

## Boundaries

- The start is the end of `pf_cache.c` (F-354-1).
- The end is the end of `PFCLUSTER_DeleteChain`, which has no trailing padding. The next routine, at `0x80201200`, walks directory entries. The routines after it manage the directory descriptor tables at volume offsets `0xE3C` (three entries, stride `0x280`) and `0x15BC` (three entries, stride `0x20`). That is the start of the directory unit. The file order matches the alphabetical order of the PrFILE2 sources (`pf_cache`, `pf_cluster`, `pf_dir`).
- The four routines from `0x802006B0` work on two entry iterators and directory entries, not on an open file. They are kept in this unit because they only edit cluster chains and they sit before the directory unit. They have no VF counterpart. Each one is called by exactly one API routine (`0x8020FA80`, `0x8020FD00`, `0x80210040` and `0x802101F0`).

## Names

These are VF names without `VFi`, medium confidence:

- `PFCLUSTER_UpdateLastAccessCluster`
- `PFCLUSTER_AppendCluster`
- `PFCLUSTER_GetAppendSize`

These names are descriptive, low confidence:

- `PFCLUSTER_FreeUnusedCluster` frees the clusters past the end of the file and terminates the chain.
- `PFCLUSTER_CombineChain` appends the source chain to the destination chain and frees any clusters that would exceed 4 GiB.
- `PFCLUSTER_DivideChain` cuts a chain at a byte offset. It copies the cluster that holds the cut into a new cluster.
- `PFCLUSTER_InsertChain` inserts newly allocated clusters at a chain index.
- `PFCLUSTER_DeleteChain` removes clusters at a chain index.

Callees named for the link:

| Name | Address | Basis |
| --- | --- | --- |
| `PFFAT_GetSectorAllocated` | `0x8020B2E0` | Calls the sector lookup with mode 2, as VF does. FA's version takes four arguments. |
| `PFFAT_GetContinuousSector` | `0x8020B320` | The next routine in VF order; it takes five arguments. |
| `PFFAT_FreeChain` | `0x8020B740` | VF name |
| `PFFAT_TraceClustersChain` | `0x8020C260` | VF name |
| `PFFAT_ReadValueToSpecifiedCluster` | `0x8020C4C0` | VF name |
| `PFFAT_GetValueOfEOC2` | `0x8020C710` | VF name; reads the table at `0x80368188` with stride 20 |
| `PFFAT_WriteValueToSpecifiedCluster` | `0x8020C410` | Descriptive; writes a FAT12, FAT16 or FAT32 entry |
| `PFFAT_AllocateContinuousClusters` | `0x8020C540` | Descriptive |
| `PFENT_UpdateSFNEntry` | `0x802072C0` | VF name |
| `PFENT_getcurrentDateTimeForEnt` | `0x80207500` | VF name |
| `PFFILE_Cursor_MoveToEnd` | `0x8020E9A0` | VF name |
| `PFFILE_Cursor_MoveToClusterEnd` | `0x8020E9F0` | VF name |
| `PFSEC_ReadData` | `0x80214F90` | VF name; seven arguments |
| `PFSEC_WriteData` | `0x80215380` | VF name; seven arguments |

## Layouts

All of these are in `include/revolution/fa/pf_volume.h`.

`PF_FFD` is 0x38 bytes:

| Offset | Field | Evidence |
| --- | --- | --- |
| 0x00 | `chain_stamp` | `0x8020C6B0` (InitFFD) and `0x8020C670` (ResetFFD) store 1 here. `0x8020AE80` copies it into the hint's third word. |
| 0x04 | `start_cluster` | Not accessed in this unit; F-355-4 shows `PFDIR_p_rename` using it as the start-cluster slot, VF's offset 0 |
| 0x08 | `p_start_cluster` | |
| 0x0C | `last_cluster` | 8 bytes |
| 0x14 | `last_access` | 8 bytes |
| 0x1C | `cluster_link` | VF layout |
| 0x30 | `p_hint` | |
| 0x34 | `p_vol` | |

`PF_FAT_HINT` is 12 bytes. Its third word records `chain_stamp`.

`PF_SFD`:

- `stat` at 0. `PFCLUSTER_*` require bits 1 and 2.
- `ffd` at 4.
- `dir_entry` at 0x3C.

`PF_DIR_ENT` keeps the 0x240-byte VF layout. `file_size` is at 0x228, `p_vol` at 0x22C and `start_cluster` at 0x234.

`PF_FILE`:

- `p_sfd` at 8.
- `hint` at 0xC.
- A word at 0x18, named `last_error` after VF (low confidence).
- `cursor` at 0x1C.

`PF_ENT_ITER` has the VF member order, with FA's 0x38-byte `PF_FFD`, which makes it 0x70 bytes. The caller at `0x8020FA80` copies it with a 0x70-byte `pf_memcpy`.

`PF_VOLUME` now names `num_free_clusters` at 0x38 and `fsi_flag` at 0x1F90. `PFCLUSTER_AppendCluster` returns 6 when bit 4 of `fsi_flag` is set and the free count is 0. Unlike VF, it does not test for an unknown count.

## Differences from VF

- There is no volume-set range check or `p_file->stat` test. Only the SFD status bits are checked.
- `PFCLUSTER_AppendCluster` takes three arguments and keeps the sector in a local. It calls `PFFAT_GetSectorAllocated` and then `PFFAT_GetContinuousSector`.

# F-355-2: Historical `PFCLUSTER_DeleteChain` difference

Resolved by [F-355-5](355-complete-inventory.md); the failed approaches below remain useful history.

The only difference is register assignment. The target keeps the byte offset (`chain_index * cluster_size`) in `r25` and the clamped size in `r24`. The reconstruction swaps them. These variants did not swap them:

- every order of the `cluster_size`, `p_vol`, `offset`, `size` and `err` declarations, with the stack locals before or after them
- reusing the `num_cluster` parameter for the size
- `s32`, `int` and `u16` parameter types
- rewriting the clamp as `offset + size > file_size`
- declaring `p_vol` and `cluster_size` with initializers

## Traps

- **The first-declared local can be allocated below the parameters.** Trigger: `PFCLUSTER_InsertChain` matched only when `size` was declared first. It then took `r22`, below every parameter, while the next locals took `r31` downward. This is unlike `PFCLUSTER_CombineChain`, where the declaration order maps directly onto `r31` downward. Evidence: F-355-1.
- **The order of a three-term sum controls its association.** Trigger: in `PFCLUSTER_CombineChain`, the target adds `num_src_cluster + num_src_unused` first. Only the source order `num_src_unused + num_dst_cluster + num_src_cluster`, with the limit computed in a statement before it, reproduces both adds and the `li r3,-1` register. Evidence: F-355-1.
- **The cluster-copy loop increments the sectors before the index.** Trigger: `PFCLUSTER_DivideChain` matched only with `src_sector++, dst_sector++, i++` in the increment list. Evidence: F-355-1.
- **An `if` whose then-branch only copies a value can leave a bare branch.** Trigger: in `PFCLUSTER_CombineChain`, the source-chain test emits `beq; b` around a load. Writing it in the same form as the destination test, `if (size != 0 && next != 0) unused = next; else unused = *p_start;`, reproduces it, because the copy is coalesced away. Evidence: F-355-1.
