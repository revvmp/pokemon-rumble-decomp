# F-363-1: FA sector and volume boundaries

## Scope and ownership

The original #363 inventory has 23 functions / 8,748 bytes at 0x80214960–0x80216BF4. Pinned original FA names and file order from CC0 IPL `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c` establish three different units. The first four functions are the end of pf_path.c, whose first function belongs to unclaimed #362. Their source/header/split remain untouched here. The next four functions form the complete first-owned pf_sector.c. The remaining fifteen functions begin pf_volume.c at 0x80215610, continuing into #364; that owner confirms no volume bodies or record edits and coordinates its later tail contribution.

| Function | Target address | Native size |
| --- | --- | --- |
| PFSEC_ReadFAT | 0x80214EC0 | 0xC4 |
| PFSEC_ReadData | 0x80214F90 | 0x2F4 |
| PFSEC_WriteFAT | 0x80215290 | 0xE8 |
| PFSEC_WriteData | 0x80215380 | 0x288 |

The sector text extent ends at 0x80215608, before the next unit's alignment. No sector data allocation is observed. All four ordinary C scratch implementations reproduce all 1,832 native function bytes and 34 native call relocation sites under the established Wii/1.0 cflags_sdk profile. Existing canonical volume/cache types and offsets are reused unchanged. Reference structure was compared with CC0 Petari `e0ab141914921726a595573dc48d3f13d0922efe` pf_sector.c; root and file-level license text was inspected. This is local scratch proof, pending actual unit split/source-selected link acceptance.

## Canonical declarations

The owned sector header gains only ReadFAT and WriteFAT signatures with native u16 offset/size arguments. The proven u32 PFDRV_IsInserted(PF_VOLUME*) call at 0x801FBF20 is declared beside existing PFDRV_lread/lwrite in canonical pf_volume.h under explicit maintainer coordination. This adds no pf_driver.h, neighboring driver split/body or storage layout.

Baseline main `76b5be1b85d6d2e6a00db58484d5800b2a83ee71` passes fresh523-target configure/plain Ninja and explicit frozen verification. After the three declaration additions, Ninja and actual-output frozen verification pass again. All516 existing source objects preserve every allocated section and canonical relocation;515 are whole-file identical, with the remaining object differing only in compiler-local naming metadata. No executable/reference hash changes.

## Volume research, not yet a published layout

Owner-coordinated #369 native file-lock evidence refines the old PF_SFD opaque lock tail without changing any size or following offset. PF_LOCK has u16 mode/count, u32 wcount, PF_FILE* owner, OSSemaphore* resource and embedded OSSemaphore storage, size0x1C; PF_SFD.lock remains at0x27C, with num_handlers at0x298 and total size0x29C. The three file-lock functions independently match all352 native bytes. Native file consumers at0x8020FA38/FA48 export mode/owner;0x802103E4/10458 compare/clear owner;0x8020EBC4 and0x80210814–820 initialize/increment num_handlers. Existing-consumer baseline and full actual frozen verification must repeat on the combined canonical header before acceptance; the earlier516-object checkpoint above covers the three prototypes only.

Independent driver and volume consumers use volume stride0x1FAC, flags at0x1F8C, p_part at0x1FA0 and array base pf_vol_set+0x88. InitModule at0x80215EB0 zeros26 records of0x1FAC bytes. Existing canonical0x3F58 size includes an inflated opaque tail. No record-size change is published until full native field/interface proof and unchanged existing-consumer gates support it.

Native PFSYS_GetCurrentContextID at0x8021C890 reads pf_vol_set+0x64, and async worker0x8021C550 writes the incoming command context ID there. Context records used by PFVOL_regctx/SetCurrentVolume begin at+0x68; nondefault records1–3 are at+0x70/+0x78/+0x80. These observations explain the volume array start+0x88 without guessed padding. Proposed current_context_id naming is descriptive. Complete full-record source and allocated-data acceptance remain future work.

The final expanded #882 prerequisite passes a fresh default configure/map,
plain Ninja, explicit report/all_source, and frozen verification on
ea4ffb4742. All 532 pre-existing objects preserve allocated sections and
relocations; 531 are whole-file identical. The sole numeric-local .strtab
difference in pf_dir is independently controlled by reproducing its exact
original hash with original headers. This accepts only the bounded header
changes above; complete volume record publication remains future work.

## Traps

- FA combines the VF reference's partial-page and full-sector data functions into one ReadData and one WriteData. Retain all three explicit offset/alignment cases and the final invalid-offset return; collapsing the logically redundant final offset check removed twenty bytes and failed the native stream. This target-specific source distinction is proven by four complete native scratch matches.
- An issue's first function is not necessarily its first source unit. Do not take pf_path.c's split/header merely because its final four functions occur in #363.
- Do not treat the old inflated PF_VOLUME opaque tail as a proven native record size, and do not promote a related VF layout wholesale. Keep record fixes outside canonical headers until their actual field users and baseline objects are verified.
