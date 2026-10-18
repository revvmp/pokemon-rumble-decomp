# F-355-5: Cluster deletion and complete issue inventory

Target: `wpse-v1`, issue #355. The final cluster-deletion register mismatch is resolved. All 25 functions in the original issue inventory, totaling 17,696 instruction bytes, are reconstructed and selected from matching source units.

## Register allocation

`PFCLUSTER_DeleteChain` at `0x80201050` now keeps the offset in `r25` and the requested/clamped byte count in `r24`, as the original does. Computing `available_size = p_ent->file_size - offset` once gives the clamp a named input. Declaring locals in the order `cluster_size`, `offset`, `p_vol`, `size`, `available_size` reproduces all 432 instruction bytes. The arithmetic, unsigned comparison, and error paths retain their original behavior. No parameter, layout, compiler setting, or original hash changed.

A targeted search over declaration orders with this additional meaningful local found the exact form. The earlier exhaustive search described in F-355-2 and F-356-3 did not include a separate available-byte-count local. Multiplication operand swaps, equivalent ternary clamps, register hints, and inner scopes did not independently fix the mismatch. Computing the available byte count first changes which value survives the clamp and is also insufficient.

## Complete unit acceptance

| Unit | Native text extent | Functions | Instruction bytes | Source selection | objdiff |
| --- | --- | --- | --- | --- | --- |
| `pf_cache.c` | `0x801FE250` to `0x80200004` | 36 | 7,404 | Matching | Every function and section 100% |
| `pf_cluster.c` | `0x80200010` to `0x80201200` | 8 | 4,540 | Matching | Every function and section 100% |
| `pf_dir.c` | `0x80201200` to `0x80205CC8` | 38 | 18,964 | Matching | Every function and all three sections 100% |

The cache and directory units were already reconstructed and accepted. This change enables the complete cluster unit after correcting its last function; it does not claim new reconstruction credit for the neighboring units. Their established boundaries and interfaces are unchanged. Directory data remains `.sdata` `0x804B8858` to `0x804B8874` and `.sdata2` `0x804BA5B8` to `0x804BA5C0`.

## Original issue inventory

Each original address and size was checked against the fresh objdiff report, including source selection and matching status. All 82 functions across the three complete units and every section are also 100%.

| Address | Size | Function | Unit | Match |
| --- | --- | --- | --- | --- |
| `0x801FFD90` | `0xD0` | `PFCACHE_FlushDataCacheSpecific` | `pf_cache.c` | 100% |
| `0x801FFE60` | `0x58` | `PFCACHE_FlushAllCaches` | `pf_cache.c` | 100% |
| `0x801FFEC0` | `0x144` | `PFCACHE_FreeAllCaches` | `pf_cache.c` | 100% |
| `0x80200010` | `0xD4` | `PFCLUSTER_UpdateLastAccessCluster` | `pf_cluster.c` | 100% |
| `0x802000F0` | `0x224` | `PFCLUSTER_AppendCluster` | `pf_cluster.c` | 100% |
| `0x80200320` | `0x240` | `PFCLUSTER_FreeUnusedCluster` | `pf_cluster.c` | 100% |
| `0x80200560` | `0x144` | `PFCLUSTER_GetAppendSize` | `pf_cluster.c` | 100% |
| `0x802006B0` | `0x33C` | `PFCLUSTER_CombineChain` | `pf_cluster.c` | 100% |
| `0x802009F0` | `0x45C` | `PFCLUSTER_DivideChain` | `pf_cluster.c` | 100% |
| `0x80200E50` | `0x1F8` | `PFCLUSTER_InsertChain` | `pf_cluster.c` | 100% |
| `0x80201050` | `0x1B0` | `PFCLUSTER_DeleteChain` | `pf_cluster.c` | 100% |
| `0x80201200` | `0x11C` | `PFDIR_IsEmptyDirectory` | `pf_dir.c` | 100% |
| `0x80201320` | `0x110` | `PFDIR_GetSDD` | `pf_dir.c` | 100% |
| `0x80201430` | `0x100` | `PFDIR_OpenDirEntry` | `pf_dir.c` | 100% |
| `0x80201530` | `0x2E4` | `PFDIR_p_opendir` | `pf_dir.c` | 100% |
| `0x80201820` | `0x124` | `PFDIR_p_readdir` | `pf_dir.c` | 100% |
| `0x80201950` | `0x8C8` | `PFDIR_p_mkdir` | `pf_dir.c` | 100% |
| `0x80202220` | `0x27C` | `PFDIR_p_chdir` | `pf_dir.c` | 100% |
| `0x802024A0` | `0x234` | `PFDIR_p_chmod` | `pf_dir.c` | 100% |
| `0x802026E0` | `0x330` | `PFDIR_p_rmdir` | `pf_dir.c` | 100% |
| `0x80202A10` | `0x18C` | `PFDIR_p_fstat` | `pf_dir.c` | 100% |
| `0x80202BA0` | `0x9C8` | `PFDIR_p_rename` | `pf_dir.c` | 100% |
| `0x80203570` | `0x9C0` | `PFDIR_p_move` | `pf_dir.c` | 100% |
| `0x80203F30` | `0x304` | `PFDIR_p_fsnext` | `pf_dir.c` | 100% |
| `0x80204240` | `0x104` | `PFDIR_p_fsfirst` | `pf_dir.c` | 100% |

## Validation

`python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exit 0. The source-selected full executable matches the frozen reference exactly; no original cluster object is selected for linking. Repeated after rebasing onto the main revision used for the final merge.

## Traps

- **A named clamp input can change register allocation without changing instructions.** When declaration-order changes alone leave offset and size in swapped registers, preserve the available-byte-count expression as its own local and check its declaration order together with the persistent locals. For this function, the order above reproduces both the prologue and every later use; this is evidence for this function, not a general compiler rule.
