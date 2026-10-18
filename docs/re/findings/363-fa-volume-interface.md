# F-363-3: Complete native FA volume interface

## Scope and ownership

This bounded prerequisite completes the first-owned pf_volume.h and moves PF_VOLUME_SET from the partial pf_str.h storage view to its canonical volume header. Every existing main declaration remains available. Main's typed PF_DIR_TAIL and the previously accepted PF_LOCK/PF_SFD definitions remain unchanged. The complete source is prepared jointly with the #364 owner; its actual source-selection gate is separate from the interface diagnostics. The #363 issue still needs four first-owned #362 path tails and is not complete.

The root approved native-required forwarding declarations in the existing volume/fat/file headers, without taking the unclaimed driver source, split or a new driver header. The #369 owner approved its getter and file-lock declaration prerequisite. The canonical NAND driver interface from #918 is reused as pfd_nanddrv_register_file from nand_drv.h; no competing NAND declaration is added. The existing five-argument PFENT_AllocateEntry name from #913 is preserved alongside its six-argument PFENT_allocateEntry.

## Complete records

| Record | Native size | Evidence |
| --- | --- | --- |
| PF_VOLUME | 0x1FAC | GetVolumeFromDrvChar multiplies this stride; complete 37-function typed consumer match |
| PF_VOLUME_SET | 0x33800 | Array of 26 complete volumes at0x88; exact BSS allocation and all field consumers |
| PF_CLUSTER_LINK_VOL | 12 | flag/interval halfwords, buffer/count words at volume0x1F94 |
| PF_CACHE_SETTING | 20 | attach reads pointer0/pointer4, halfwords8/A and buffer-size wordsC/10 |
| PF_DRV_TBL | 12 | attach reads partition0/cache4 and signed-drive8/status9, native descriptor stride |
| PF_DEV_INF | 16 | getdev writes clusters0/free-clusters4/bytes-per-sector8/sectors-per-clusterC |
| PF_VOL_INF | 24 | label12/attributeC/date10/time14, native query promotes date/time to words |
| PF_VOLUME_CONFIG | 16 | set/getvolcfg read/write four words: flags0/file-flags4/FAT-buffer-size8/data-buffer-sizeC |
| PF_CLUSTER_LINK_CONFIG | 12 | volume and file mode-one consumers read pointer0/count4/interval-halfword8 |

PF_VOLUME preserves its previously established prefix: cluster_link at0x1F94, partition pointer1FA0, callback1FA4 and format parameter1FA8 complete its tail. Seven independent FAT callback calls establish s32(*)(s32 error), not a fabricated callback shape. Native mount stores the format pointer and the reconstructed mount helper forwards it. The complete set preserves current_vol[4] atzero, attached/mounted counts30/34, config38, parameter3C, last errors40/44, code callbacks48, setting60, current_context_id64, four context records68 and volume array88.

The output/configuration record spellings retain established reference names where available; PF_VOLUME_CONFIG and PF_CLUSTER_LINK_CONFIG are descriptive canonical names already accepted for their opaque forwarding tags. Their extents are the natural minima covering every observed field. No padding-only fields, unknown-byte tails or alternate storage aliases were introduced.

## Source consumer proof

All 37 ordinary C definitions in native order compile using the proposed tracked canonical records, the owner-approved pending CP932 header and the exact main #918 NAND header. The source contains no duplicate output/configuration definitions and no outside local extern declarations; six same-unit forward declarations remain. The strict audit resolves every relocation independently and compares 18,008 native function bytes / 735 sites, the whole18,160-byte text allocation including padding, all 37 starts, data12/sdata5/sdata2one and BSS0x33800. All pass with unchanged Wii/1.0 SDK flags. The complete native symbol-to-reference identity table remains part of the combined source delivery.

The source payload extents are text80215610–80219D00, data803C4268–803C4274, BSS803EB970–8041F170, sdata804B88C0–804B88C5 and sdata2 804BA5D8–804BA5D9. The source object's alignments are16/8/8/8/8 respectively. The linked split ends for the two small-data sections are804B88C8 and804BA5E0: their final three and seven zero bytes are linker alignment, not additional C storage. The pinned original FA split independently gives both corresponding small-data sections eight-byte linked extents. Separate source-payload and final-link padding checks preserve this distinction. Entry's identical label string at803C4258, API data at803C4278 and the following system objects at804B88C8 remain outside the volume unit.

## Existing-source control

The actual source-selected link additionally requires the following
outside names, independently resolved at the native paired relocation
sites. Only their fn_ placeholder names change; every outside extent,
type, attribute and split remains intact.

| Address | Original FA name |
| --- | --- |
| 801FBC20 | PFDRV_LoadVolumeLabelFromBuf |
| 801FBC60 | PFDRV_StoreVolumeLabelToBPB |
| 801FBF30 | PFDRV_IsMountRequested |
| 801FBF40 | PFDRV_ClearMountRequested |
| 801FBF50 | PFDRV_IsUnmountRequested |
| 801FBF60 | PFDRV_ClearUnmountRequested |
| 801FBF70 | PFDRV_init |
| 801FC030 | PFDRV_finalize |
| 801FC0C0 | PFDRV_mount |
| 801FC360 | PFDRV_unmount |
| 801FC3E0 | PFDRV_format |
| 8020B560 | PFFAT_CountFreeClusters |
| 8020BCB0 | PFFAT_InitFATRegion |
| 8020C0C0 | PFFAT_RefreshFSINFO |
| 802108B0 | PFFILE_FinalizeAllFiles |
| 8021C330 | PF_InitLockFile |
| 8021C890 | PFSYS_GetCurrentContextID |

The first-owned file-unit investigation refines PF_FILE.open_mode at offset4 from unsigned32 to signed32, without changing its0x30-byte size or any offset. The pinned TFP2 record independently declares pf_s32. Native fclose at80210B90 compares this loaded word using cmpwi; the unsigned declaration produced the sole differing cmplwi instruction after the complete close algorithm matched. The signed declaration reproduces all0x2AC native bytes and every paired target. Original-header controls recompile the complete37-function volume implementation and both actual existing entry consumers; all three whole object files are byte-identical, and the full37/18,008/735 native/data/BSS audit still passes. No current canonical source reads open_mode directly. The final combined existing-object and frozen gates still apply.

A fresh 537-target all_source build on canonical mainc39785e8 exits0. A second pass recompiles every consumer of the final declaration additions and also exits0. All535 pre-existing source objects preserve every allocated section and canonical relocation; 513 whole files are identical. The remaining22 objects differ only in compiler-local numeric .strtab labels, with every other section identical. The upstream #917 EF source is preserved and independently recompiled under original canonical headers as its current-main control. An actual compiled20-word layout record also confirms every listed size and the critical set/volume/output offsets. The final combined source-selected/frozen gate remains mandatory before source acceptance. No new matching credit is claimed by this header prerequisite.

## Traps

- Ending the small-data split at its five-byte source payload made dtk1.8.4 create an automatic fallback at the odd address804B88C5; its default alignment relaxation stops at four and rejects that address. Preserve the independently evidenced eight-byte linked extent, while keeping the compiled payload5/1 and separately checking the3/7 linker zeros. Do not add source padding or absorb the next system object.
- An old 0x3F58 PF_VOLUME extent is twice the native stride. Validate native multiplication and full object consumers before changing an opaque tail; an exact instruction stream for one pointer-based function alone does not establish a record size.
- The file and volume public cluster-link configuration share pointer/count/interval widths, but differ from internal traversal records. Reuse the canonical public tag rather than conflating their storage.
- Native error paths and driver argument counts must be measured independently. Detach's callers supply one drive argument; LockFile's callers supply an additional owner pointer even though that callee does not consume it.
