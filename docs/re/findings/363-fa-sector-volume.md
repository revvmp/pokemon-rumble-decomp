# F-363-1: FA sector and volume boundaries

## Scope and ownership

The original #363 inventory has 23 functions / 8,748 bytes at 0x80214960–0x80216BF4. Pinned original FA names and file order from CC0 IPL `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c` establish three different units. The first four functions are the end of pf_path.c, whose first function belongs to unclaimed #362. Their source/header/split remain untouched here. The next four functions form the complete first-owned pf_sector.c. The remaining fifteen functions begin pf_volume.c at 0x80215610, continuing into #364; that owner confirms no volume bodies or record edits and coordinates its later tail contribution.

| Function | Target address | Native size |
| --- | --- | --- |
| PFSEC_ReadFAT | 0x80214EC0 | 0xC4 |
| PFSEC_ReadData | 0x80214F90 | 0x2F4 |
| PFSEC_WriteFAT | 0x80215290 | 0xE8 |
| PFSEC_WriteData | 0x80215380 | 0x288 |

The sector text extent ends at 0x80215608, before the next unit's alignment. No sector data allocation is observed. All four ordinary C scratch implementations reproduce all 1,832 native function bytes and 34 native call relocation sites under the established Wii/1.0 cflags_sdk profile. Existing canonical volume/cache types and offsets are reused unchanged. Reference structure was compared with CC0 Petari `e0ab141914921726a595573dc48d3f13d0922efe` pf_sector.c; root and file-level license text was inspected. The ordered split-only, NonMatching compilation and Matching source-selected link gates now pass. The real Ninja object reproduces all 1,832 function bytes, 34 original-name paired calls and the complete 1,864-byte text allocation including alignment. All allocated objdiff sections are 100%; final ELF addresses/extents and every link input are audited, with the original sector object excluded. Explicit frozen verification of the actual custom output passes with main.dol SHA-256 ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da. The original issue remains incomplete: four path tails and fifteen volume functions are not selected from source yet.

## Canonical declarations

The owned sector header gains only ReadFAT and WriteFAT signatures with native u16 offset/size arguments. The proven u32 PFDRV_IsInserted(PF_VOLUME*) call at 0x801FBF20 is declared beside existing PFDRV_lread/lwrite in canonical pf_volume.h under explicit maintainer coordination. This adds no pf_driver.h, neighboring driver split/body or storage layout.

Baseline main `76b5be1b85d6d2e6a00db58484d5800b2a83ee71` passes fresh523-target configure/plain Ninja and explicit frozen verification. After the three declaration additions, Ninja and actual-output frozen verification pass again. All516 existing source objects preserve every allocated section and canonical relocation;515 are whole-file identical, with the remaining object differing only in compiler-local naming metadata. No executable/reference hash changes.

## Volume research, not yet a published layout

Owner-coordinated #369 native file-lock evidence refines the old PF_SFD opaque lock tail without changing any size or following offset. PF_LOCK has u16 mode/count, u32 wcount, PF_FILE* owner, OSSemaphore* resource and embedded OSSemaphore storage, size0x1C; PF_SFD.lock remains at0x27C, with num_handlers at0x298 and total size0x29C. The three file-lock functions independently match all352 native bytes. Native file consumers at0x8020FA38/FA48 export mode/owner;0x802103E4/10458 compare/clear owner;0x8020EBC4 and0x80210814–820 initialize/increment num_handlers. The subsequent fresh sector split/source build includes this combined typed-lock header. All516 pre-existing source objects preserve every allocated section and canonical relocation;515 are whole-file identical, with the same independently controlled pf_dir local-name metadata difference. The full source-selected actual-output frozen verifier exits0. The separate shared-header prerequisite still requires its own latest-main gate before canonical integration.

Independent driver and volume consumers use volume stride0x1FAC, flags at0x1F8C, p_part at0x1FA0 and array base pf_vol_set+0x88. InitModule at0x80215EB0 zeros26 records of0x1FAC bytes. Existing canonical0x3F58 size includes an inflated opaque tail. No record-size change is published until full native field/interface proof and unchanged existing-consumer gates support it.

Native PFSYS_GetCurrentContextID at0x8021C890 reads pf_vol_set+0x64, and async worker0x8021C550 writes the incoming command context ID there. Context records used by PFVOL_regctx/SetCurrentVolume begin at+0x68; nondefault records1–3 are at+0x70/+0x78/+0x80. These observations explain the volume array start+0x88 without guessed padding. Proposed current_context_id naming is descriptive. Complete full-record source and allocated-data acceptance remain future work.

The final expanded #882 prerequisite passes a fresh default configure/map,
plain Ninja, explicit report/all_source, and frozen verification on
ea4ffb4742. All 532 pre-existing objects preserve allocated sections and
relocations; 531 are whole-file identical. The sole numeric-local .strtab
difference in pf_dir is independently controlled by reproducing its exact
original hash with original headers. This accepts only the bounded header
changes above; complete volume record publication remains future work.

## F-363-2: First fifteen volume functions match in diagnostic probes

All fifteen original #363 pf_volume.c functions at 0x80215610–0x80216BF4 now reproduce 5,568 native instruction bytes with 168 independently resolved branch, HA/LO and small-data relocations under the unchanged Wii/1.0 SDK compiler profile. This includes mount/unmount, format, volume-label creation/query/removal, module initialization, read/write checks and context/current-directory accessors. Combined with the four sector functions, nineteen first-owned functions / 7,400 bytes have diagnostic native proof. This is not source-selected build acceptance or a completed #363 inventory: the four #362-owned path tails and fifteen volume bodies still require their complete canonical layout, whole-volume allocation boundaries, existing-object controls and source-selected frozen build.

The diagnostic volume record is 0x1FAC and volume-set record is 0x33800; the typed source references the native current-context field and four context slots rather than a guessed byte offset. The volume record preserves the established prefix and uses a native cluster-link subrecord at 0x1F94, partition pointer at 0x1FA0, callback at 0x1FA4 and format parameter at 0x1FA8. Seven native FAT call sites at 0x8020969C, 0x8020C810, 0x8020CDB4, 0x8020D03C, 0x8020D37C, 0x8020D49C and 0x8020D86C independently establish the callback as s32 (*)(s32 error): each passes last_driver_error from 0x1F84 in r3 and checks the signed result against zero and one. PFDRV_mount at 0x801FC17C stores the disk-information format pointer at 0x1FA8; DoMountVolume later forwards it to format. Other tail and complete-record consumers are still being audited before proposing canonical records. The label query writes its date/time as 32-bit output fields after promoting the native 16-bit directory-entry fields.

A canonical entry-allocator identity collision was caught before publication. The existing six-argument declaration named PFENT_allocateEntry at 0x80206E00 is named PFENT_allocateEntryPos in the pinned original FA map. The five-argument wrapper at 0x802070E0 is original PFENT_allocateEntry and is the callee required by the native set-volume-label function. A maintainer-controlled atomic correction is pending; no conflicting prototype or alias has been added to canonical headers.

Upstream #913 subsequently supplies the five-argument wrapper as canonical PFENT_AllocateEntry while retaining the existing six-argument PFENT_allocateEntry. Future actual source integration preserves those accepted spellings and records their original-map correspondence; no conflicting declaration is required. Its typed PF_DIR_TAIL independently agrees with the attach evidence below.

## Owner-coordinated #364 volume tail probes

The #364 owner delegated PFVOL_attach at 0x80217B40 / 0x2C0 and PFVOL_detach at 0x80217E00 / 0xFC for independent reconstruction. Both ordinary C candidates reproduce all 956 native bytes and 36 explicitly resolved relocation sites. No source is counted here a second time; the #364 owner will integrate the full pf_volume.c tail and repeat the whole-unit source-selected gate. The pinned original FA split map ends pf_volume.c at native 0x80219D00; the following PFVOL_iswriteprotected belongs to the separate first-owned #364 pf_volume_apiadd.c.

Attach independently proves the complete driver-table pointer fields at offsets zero/four and drive/status bytes at eight/nine, the cache-setting page/buffer pointers, two page-count halfwords and two buffer-size words. It also initializes the twelve-byte PF_DIR_TAIL at volume offset 0x1F74: tracker_size, tracker_buff[1] and tracker_bits. Detach consumes only the drive argument. Independent #369 dispatcher and public-stub callers supply only that argument; an inherited two-argument declaration with an unused mode was rejected before canonical acceptance, even though adding an unused parameter leaves this callee's bytes unchanged.

Four existing FA consumers were recompiled against the external complete-record proposal: pf_str, pf_dir, pf_cache and pf_cluster preserve every allocated section and canonical relocation. Three objects are whole-file identical; pf_dir retains only the previously controlled compiler-local naming difference. This limited diagnostic control supplements, and does not replace, the pending complete existing-object baseline and frozen executable gate.

Six further public functions delegated by #364 now have independent ordinary C instruction and paired-target proof: format/mount contribute 1,580 bytes / 79 targets; setvol/getvol contribute 1,508 bytes / 78 targets; getdev/buffering contribute 1,484 bytes / 68 targets. Together with attach/detach, these eight supplied bodies total 5,528 bytes / 261 targets. They remain diagnostic, with no duplicate source credit. The device-information record's four native-used words are cls/ecl/bps/spc at zero/four/eight/twelve, establishing a natural minimum size of sixteen bytes. The canonical volume record and complete source unit still require owner integration and full build acceptance.

The setvol implementation uses ordinary nested cache-flush error handling: a failed FAT flush records both errors; otherwise the data-cache flush runs and records its error. The following common success check writes the label to the BPB. This source matches without a synthetic inline helper. A diagnostic extraction called FlushVolumeLabel was rejected during fidelity review in favor of the equally exact nested implementation; that helper is not presented as a recovered original identity.

## Traps

- FA combines the VF reference's partial-page and full-sector data functions into one ReadData and one WriteData. Retain all three explicit offset/alignment cases and the final invalid-offset return; collapsing the logically redundant final offset check removed twenty bytes and failed the native stream. This target-specific source distinction is proven by four complete native scratch matches.
- An issue's first function is not necessarily its first source unit. Do not take pf_path.c's split/header merely because its final four functions occur in #363.
- Do not treat the old inflated PF_VOLUME opaque tail as a proven native record size, and do not promote a related VF layout wholesale. Keep record fixes outside canonical headers until their actual field users and baseline objects are verified.

- Volume-label routines allocate the root-directory and found-entry records in reverse declaration order under this compiler. Native control flow also retains a separate error return and places the existing-label branch before label creation. Ordinary source structure reproduces all four label/format routines; changing semantics, compiler flags or using assembly was unnecessary.
- Small-data relocation type109 patches the instruction register base and low16 displacement, rather than the first16 instruction bits. An initial diagnostic relocation patch was corrected before matching claims; the final complete fifteen-function comparison includes every relocated instruction.

- Source comparison spelling can affect this compiler: the original-looking signed range checks `index < 0 || 26 <= index` preserve separate signed comparisons in attach. Reversing the second comparison produced a folded unsigned range check and a sixteen-byte deficit. This is limited to the measured compiler/source examples, not a general C equivalence rule.

## Verified sector checkpoint commands

At the committed checkpoint based on main76b5be1b, `python configure.py --build-dir build/363-sector-split --map`, `.venv/Scripts/ninja -j4` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/363-sector-split/wpse-v1` exit0. The fresh split-only build has523 default targets. The source-selected four-function audit additionally checks all34 paired original/source relocations, final native addresses/sizes, the complete original #363 issue subset, objdiff sections100, actual source input selection, original-sector fallback exclusion, exact rebuilt DOL bytes and the516-object independent baseline. Latest-main integration remains pending the shared-header prerequisite.
