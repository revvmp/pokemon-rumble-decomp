# F-372-1: Ownership of the #372 block

Issue #372, target `wpse-v1`. The block `0x8021F3B0`–`0x80220884` holds parts of two native units.

The first eight functions, `0x8021F3B0`–`0x8021FFB4`, end the NAND-backed disk driver. That unit begins at `0x8021E420` in #371. Pull request #918 for #371 splits and reconstructs it as `RVL_SDK/fa/nand_drv.c` and names these eight functions. This pull request does not touch that range.

The remaining nine functions, `0x8021FFC0`–`0x80220884`, begin the SD card driver. Its diagnostics start the new `.data` string pool at `0x803C47F0`, directly after `pf_kernel.c`, and name `pfd_st_inter_callback()`, `pfd_st_removal_callback()`, `pfd_sddrv_init()` and `pfd_sddrv_mount()`. Because the file's first function is in #372, this issue owns its split, object line and header.

# F-372-2: SD driver unit boundaries

The descriptive file `RVL_SDK/fa/sd_drv.c`, named after `nand_drv.c`, owns these sections:

| Section | Range | Evidence |
| --- | --- | --- |
| `.text` | `0x8021FFC0`–`0x80222FA8` | Last function `0x80222E90`, size `0x118`, is called by the first two callbacks. `0x80222FB0` uses the next unit's `.sbss` at `0x804B9268` |
| `.rodata` | `0x8036C460`–`0x8036C5D0` | `0x20`-byte driver function table, then the `0x150`-byte format parameter table used by `0x80221030`–`0x80222930`. `0x8036C5D0` begins an existing split |
| `.data` | `0x803C47F0`–`0x803C5260` | The string pool runs from `pfd_st_inter_callback()` to `ERR Failed to build up and write MBR and BPB fields.` at `0x803C5228`. `0x803C5260` is used by `0x80223630` |
| `.bss` | `0x80422BE0`–`0x80422E40` | Driver state `0x20`, ISD handle `0x28` at `0x80422C00`, 32-byte aligned `0x200` sector buffer at `0x80422C40`. `0x80422E40` is used by `0x802236C0` |
| `.sbss` | `0x804B9240`–`0x804B9268` | Insert and eject callbacks, and the interrupt event word at `0x804B9260`. `0x804B9248`–`0x804B925F` has no code reference |

The `.text` range spans 28 functions: nine in #372 and nineteen in #373 (`0x80220890`–`0x80222E90`). The split-only configure, build and frozen-manifest verification all exit 0.

The rodata function table at `0x8036C460` lists, in `PDM_FUNCTBL` order, `0x802201F0` init, `0x802207A0` finalize, `0x80220440` mount, `0x80220760` unmount, `0x80220640` format, `0x802206E0` physical read, `0x80220720` physical write and `0x80220890` get disk info.

# F-372-3: Names, layouts and reconstruction

| Address | Name | Confidence |
| --- | --- | --- |
| `0x8021FFC0` | `pfd_st_inter_callback` | high, native diagnostic |
| `0x802200E0` | `pfd_st_removal_callback` | high, native diagnostic |
| `0x802201F0` | `pfd_sddrv_init` | high, native diagnostic |
| `0x80220440` | `pfd_sddrv_mount` | high, native diagnostic |
| `0x80220640` | `pfd_sddrv_format` | low, table slot; its message names its callee |
| `0x802206E0` | `pfd_sddrv_pread` | low, table slot, after `pfd_nanddrv_pread` |
| `0x80220720` | `pfd_sddrv_pwrite` | low, table slot |
| `0x80220760` | `pfd_sddrv_unmount` | low, table slot |
| `0x802207A0` | `pfd_sddrv_finalize` | low, table slot |

Callees named for the link, owned by #373: `pfd_sddrv_get_disk_info` (`0x80220890`), `pfd_sddrv_physical_read` (`0x80220AF0`), `pfd_sddrv_physical_write` (`0x80220CF0`) and `pfd_sddrv_full_format` (`0x80222CB0`), all high from their own diagnostics. The `full_format` reset message is in `0x80222CB0`, so the format entry's message names its callee. `pfd_sddrv_init_drv_tbl` (`0x80220A20`) and `pfd_sddrv_set_media_callback` (`0x80220A40`) are descriptive, medium. `SdGetDriveLetters` (`0x80222E90`) is descriptive, low; it collects up to `max` drive characters of volumes on partitions of the disk.

The SD layer below the driver opens `/dev/sdio/slot0` through IOS. A diagnostic, `ERR Failed to write BPB fields. ISD_WriteBlock()`, gives its `ISD_` prefix. These #374 functions get descriptive `ISD_` names, low confidence for the suffix: `ISD_Init` (`0x80224BB0`), `ISD_Open` (`0x80224A90`), `ISD_Close` (`0x80224B70`), `ISD_GetStatus` (`0x80224030`), `ISD_Reset` (`0x80224490`), `ISD_SendCommand` (`0x80224D30`, commands 3, 9, 10 and 41), `ISD_GetCardSize` (`0x802251A0`), `ISD_RegisterDeviceIntrHandler` (`0x802252F0`, ioctl `0x40`) and `ISD_UnregisterDeviceIntrHandler` (`0x802253A0`, ioctl `0x41`). The second pair follows the native warning `Faild to UnregisterDeviceIntrHandler`. Their splits and final names remain #374's.

The driver state, `sSdDriver`, is accessed as a volatile object, because the interrupt callbacks change it. Every reload and reread in the unmount, finalize and callback code needs `volatile`. Fields: `status` `0x00` (bit 0 initialized, bit 1 mounted, bit 2 ISD ready), `sector_size` `0x04` (512), `p_handle` `0x08`, `p_disk` `0x0C`, `inserted` `0x10`, `media_changed` `0x14` (set on insert, cleared after a successful reset), and `drive` `0x18` (signed `char`, read with `extsb`). `ISD_HANDLE` is `0x28` bytes: IOS descriptor `0x00`, RCA `0x10` (set by reset, read by commands), card size words `0x18`–`0x20` (written by `ISD_GetCardSize`), and an open flag `0x24`. The remaining words stay `reserved`.

Interrupt handlers take a signed event (`cmpwi`). The registration function reads an unsigned event word through its third argument (`cmplwi`). CodeWarrior placed this unit's `.sbss` objects in reverse declaration order.

Status: `RVL_SDK/fa/sd_drv.c` is registered `NonMatching` with the nine #372 functions. objdiff reports 100% for eight of them: both callbacks, `pfd_sddrv_mount`, `pfd_sddrv_format`, `pfd_sddrv_pread`, `pfd_sddrv_pwrite`, `pfd_sddrv_unmount` and `pfd_sddrv_finalize`. `pfd_sddrv_init` is at 95.0%. Two differences remain:

- The target tests `p_disk` against zero with `li r0,0` and `cmplw r3,r0`. The source gives `cmpwi r3,0`. These forms all give `cmpwi`: `p_disk == NULL`, `!p_disk`, `0 == p_disk`, `(u32)` casts, `const` and `volatile` casts, and an inline equality helper.
- The target keeps the branch for `p_disk != sSdDriver.p_disk` that returns -44 or 0. The source becomes a branchless select. Equality, reversed operands and a `result` variable give the same select.

Wii/1.1, 1.3, 1.5, 0x4201_127 and GC/3.0a5.2 all produce the same `cmpwi`, so the difference is in the source. The unit stays `NonMatching` until #373 reconstructs its 19 functions and `pfd_sddrv_init` matches. The fallback object still supplies every byte, and full verification passes.

Source was written for this target from native instructions and diagnostics. No external implementation was copied.

## Traps

- **An unreferenced static function is dropped from a partial unit.** Trigger: the first build of the partial file emitted no `.text`, because nothing referenced the static table entries. The function table and the entry that references it belong to #373. Until then the entries are external, declared in `sd_drv.h`.
- **The SD driver state is volatile.** Trigger: `pfd_sddrv_unmount` and the callbacks lacked reloads of `status` and `drive`. Declaring `sSdDriver` volatile reproduced them.
