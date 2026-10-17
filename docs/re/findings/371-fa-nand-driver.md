# F-371-2: Complete native NAND-backed disk driver

The native driver starts at `0x8021E420` after the kernel adapter and ends at `0x8021FFB4`, before the SD interrupt handler at `0x8021FFC0`. The eight-entry callback table at `.rodata` `0x8036C440` names its initialization, finalization, mount, unmount, format, physical read/write wrappers, and geometry query. Together with common static records and helper calls, this establishes a complete native unit spanning ten functions in #371 and eight in #372.

`RVL_SDK/fa/nand_drv.c` reconstructs all eighteen functions in C: 6,976 instruction bytes. Objdiff reports every function and all five sections at 100%. Split-only and source-linked configure/build/frozen-manifest verification exit 0; direct comparison reproduces the original executable without changing its expected hash.

| Section | Exact range | Size |
| --- | --- | --- |
| `.text` | `0x8021E420` to `0x8021FFB4` | `0x1B94`, including 84 alignment bytes |
| `.rodata` | `0x8036C440` to `0x8036C460` | `0x20`, callback table |
| `.bss` | `0x804215F8` to `0x80422BE0` | `0x15E8` |
| `.sdata` | `0x804B88D8` to `0x804B88E0` | `0x08`, path literals |
| `.sbss` | `0x804B9218` to `0x804B9224` | `0x0C` |

| Address | Size | Reconstructed name |
| --- | --- | --- |
| `0x8021E420` | `0xBC` | `NandOpenDevice` |
| `0x8021E4E0` | `0x1CC` | `NandFormatDevice` |
| `0x8021E6B0` | `0x204` | `NandCreateDeviceFile` |
| `0x8021E8C0` | `0x140` | `NandGetFatType` |
| `0x8021EA00` | `0x130` | `NandBuildFsInfoSector` |
| `0x8021EB30` | `0x45C` | `NandBuildBootSector` |
| `0x8021EF90` | `0x8C` | `pfd_nanddrv_init` |
| `0x8021F020` | `0x1BC` | `pfd_nanddrv_mount` |
| `0x8021F1E0` | `0xF8` | `pfd_nanddrv_format` |
| `0x8021F2E0` | `0xC4` | `pfd_nanddrv_pread` |
| `0x8021F3B0` | `0xC4` | `pfd_nanddrv_pwrite` |
| `0x8021F480` | `0x208` | `pfd_nanddrv_unmount` |
| `0x8021F690` | `0xEC` | `pfd_nanddrv_finalize` |
| `0x8021F780` | `0x140` | `pfd_nanddrv_get_disk_info` |
| `0x8021F8C0` | `0x18` | `pfd_nanddrv_init_drv_tbl` |
| `0x8021F8E0` | `0x120` | `pfd_nanddrv_register_file` |
| `0x8021FA00` | `0x2F0` | `NandPhysicalRead` |
| `0x8021FCF0` | `0x2C4` | `NandPhysicalWrite` |

## Layout and behavior

`PFD_NAND_DEVICE` is `0xF0` bytes: disk pointer `0x00`, path `[80]` `0x04`, file size `0x54`, existing SDK `NANDFileInfo` `0x58` (`0x8C` bytes), flags `0xE4`, unobserved reserved four-byte storage `0xE8`, and signed last error `0xEC`. There are 23 records. Flag 2 is mounted; flag 4 identifies the explicit formatting operation. No meaning is claimed for the reserved slot.

Static layout: working directory `[64]` at `0x804215F8`, records at `0x80421638` (`0x1590` bytes), semaphore parameters at `0x80422BC8` (`0x0C`), semaphore at `0x80422BD4` (`0x0C`). Small-data state is driver initialization at `0x804B9218`, NAND initialization at `0x804B921C`, and the four-byte semaphore handle at `0x804B9220`. The handle's inferred `0x20` extent included the next driver's alignment; only four bytes belong to this unit.

The driver stores a FAT image in a NAND file, creates path components, generates boot and FSInfo sectors, and uses 32-byte-aligned 512-byte bounce buffers. Read/write requests beyond the registered size copy or write the available prefix and return -22; the final clipped write ignores its result, as the native code does. Successful directory-file creation returns the working-directory restore result, preserving the original error behavior.

The geometry query calculates cylinders as `total_sectors / 255 / 63 / 512`; the additional division is native and retained. FAT classification preserves the alignment-dependent root-directory calculation. Endian writers retain both native addressing paths, including the aligned serial-number destinations at 76 for byte offset 67 and 48 for byte offset 39. These cases are established by matching the entire helper instruction streams, not by assuming standards-correct FAT behavior.

## Names, compiler, and provenance

File, helper, record, and member names are descriptive. Callback roles and parameter types follow the target's table and existing `PDM_FUNCTBL`, `PDM_DISK`, `PDM_DISK_INFO`, and `NANDFileInfo`. Confidence is high for behavior and layout, medium for chosen spelling. Original names were not recovered from this stripped unit. The older CC0 Petari VF NAND driver (`SMGCommunity/Petari`, `e0ab141914921726a595573dc48d3f13d0922efe`) was consulted for interface roles; its record layout, image format, and I/O implementation differ. No implementation bodies were copied.

Wii/1.0 and unchanged `cflags_sdk` reproduce the whole unit. Controlled comparisons of Wii/1.0, 1.0a, 1.0RC1, 1.1, and 1.3 with the same flags produced identical results during reconstruction; no compiler override is introduced.

The path parser uses the named `next_component` transition after a successful directory change to preserve native ordering of its two error exits. It is ordinary C control flow, without address labels or pasted instructions.

## Remaining issue scope

This completes ten additional #371 functions (3,932 bytes), so its kernel plus NAND coverage is 20 of 28 functions and 4,980 of 5,460 instruction bytes. The eight public filesystem wrappers at `0x8021DDD0` through `0x8021DFDC` remain for their source unit starting in #370. The eight NAND functions in #372 cover 3,044 of that issue's 5,224 bytes; its nine SD functions remain. Neither issue is claimed complete by this unit pull request.

## Traps

- A signed integer FAT result produced arithmetic selection; the proper `PDM_FAT_TYPE` local restores the two native threshold branches. Evidence: complete `NandGetFatType` comparison.
- The tested prefix `__attribute__((noinline))` did not preserve FSInfo outlining under this compiler. A trailing declaration with `__attribute__((never_inline))`, already used by the project, restores both the helper and caller at 100%.
- Physical I/O pointer declaration order matters: declaring the device pointer after the byte-count locals reproduces the native register group. Earlier placement leaves the full instruction sequence with rotated pointer/argument registers at 99.4%. A bounded eight-position comparison identified the matching ordinary declaration order; compiler revision, register hints, and const parameter variants did not resolve it.
- All-zero small-data sections can hide object-order differences. Check the individual symbols: the first declaration order put the semaphore handle at section offset zero; the verified declaration order places initialization words at offsets zero and four and the handle at eight. Full executable verification confirms the final addresses.
