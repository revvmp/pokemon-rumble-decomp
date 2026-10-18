# F-351-3: Driver and disk-manager unit reconstruction

Target: `wpse-v1`, issue #351. The original FA names and consecutive sizes in the pinned primary IPL FA map establish the complete driver unit at `0x801FBBE0` to `0x801FC544` (17 functions, 2,308 instruction bytes) followed by `pdm_dskmng.c` at `0x801FC550` to `0x801FC620` (five functions, 168 instruction bytes). The latter includes the two tail functions in #765. The preceding two #351 string functions are already accepted in `pf_str.c`.

## Split-only acceptance

Neither new unit has source registration at this checkpoint. `pf_driver.c` owns only its text. `pdm_dskmng.c` owns its text and `.bss` `0x803EAFA8` to `0x803EB89C`: the canonical `PDM_DISK_SET` is `0x8F4` bytes, and the symbol's prior `0x8F8` extent included the four following alignment bytes. Those bytes remain outside the unit. The next disk unit still starts its BSS at `0x803EB8A0`.

The primary map facts and native directions are detailed in [F-351-2](351-driver-label-interface.md); [F-765-1](765-fa-disk-manager.md) independently establishes the five-wrapper grouping and disk-set extent. Its callback stride evidence agrees with the now-canonical `PF_VOLUME` record size `0x1FAC` and 26 records. The old thirteen-record / `0x3F58` interpretation in F-351-1 is superseded: each of thirteen unrolled iterations handles two records. No volume layout is changed by this split.

`python configure.py --map`, plain `.venv/Scripts/ninja`, and the explicit frozen verifier each exit 0. The previous DOL was removed before building. Both new original split objects remain selected, and the resulting DOL matches the immutable manifest. No hash or compiler setting changes.

## Reconstruction state

Both units are now registered `Matching` and selected from compiled C. The original split-only gate was committed as `86023100`; the later source-selected configure/map, plain Ninja and explicit frozen verification each exit 0. The full DOL is identical. No original fallback object for either new unit remains in the actual link graph.

| Unit | Retained functions | Instruction bytes | Allocated sections | Final acceptance |
| --- | --- | --- | --- | --- |
| `pf_driver.c` | 17 | 2,308 | Text 2,404 bytes including padding | Every function/section 100%; actual ELF/map exact |
| `pdm_dskmng.c` | 5 | 168 | Text 208 bytes including padding, BSS 2,292 bytes | Every function/section 100%; actual ELF/map exact |
| Existing `pf_str.c` | 11 | 1,384 | Text 1,468 bytes including padding | Independently audited; no new source credit |

New code credit is limited to 2,476 instruction bytes across the two new units, plus the disk-set storage. The original #351 inventory is 22 functions / 2,888 bytes: two inherited string functions / 516 bytes, all seventeen driver functions / 2,308 bytes, and the first three disk-manager wrappers / 64 bytes. The remaining two disk-manager functions / 104 bytes belong to #765, whose independent issue acceptance is separate.

## Source and behavior evidence

All seventeen `PFDRV_*` spellings and the five disk-manager spellings follow the consecutive FA functions and exact sizes in the pinned primary map linked by F-351-2. Their target addresses and native accesses establish the interfaces. The existing complete `PF_VOLUME`, cache-page, PDM callback and disk records remain unchanged. The small `pf_driver.h` exposes the two media callbacks and includes the existing volume header's fifteen driver declarations; it does not duplicate or replace them.

The driver implementation is independently written from the native instruction streams. Related CC0 Petari VF driver code at `e0ab141914921726a595573dc48d3f13d0922efe` was consulted for library roles; its init, callbacks and validation differ from this FA revision. The disk-manager wrappers adapt that pinned reference's straightforward wrapper structure and unused initialization parameters, preserving FA's omitted checks and four-byte tail branches. No restrictive source is imported, and neither implementation embeds original code bytes.

Media callbacks visit all 26 volume records and preserve the observed flag transitions. Initialization installs insert/eject callbacks before initializing the partition and clears them on failure. Driver error 21 records the underlying error in both the volume set and the volume, returning 4096; other underlying errors return -1. Mount preserves its unchecked cache-page allocation, fixed 512-byte boot/FSINFO reads, base `buffer` access, FAT32-only FSINFO path, and original cleanup order. Label updates use `p_buf` and the volume's current sector size, require exactly one successful sector, update the FAT-specific eleven-byte label field, release the page, then update the cached label only after success. These native side effects and unusual paths are preserved rather than normalized to a reference revision.

The unsigned mount/unmount mode conversion remains `mode == 1`. Disk open and partition open clear their output pointers first. Disk-manager initialization delegates to `pdm_disk_init_disk_set`; it is a real tail branch, not an immediate zero return. The complete disk-set global is defined once in this unit and is still referenced by the existing matching PDM units.

## Complete original issue inventory

| Native address | Original size | Function | Source unit | Acceptance |
| --- | --- | --- | --- | --- |
| `0x801FB9D0` | `0x118` | `PFSTR_StrNCmp` | `pf_str.c` | 100%, exact ELF/map |
| `0x801FBAF0` | `0xEC` | `PFSTR_ToUpperNStr` | `pf_str.c` | 100%, exact ELF/map |
| `0x801FBBE0` | `0x3C` | `PFDRV_LoadVolumeLabelFromBuf` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FBC20` | `0x3C` | `PFDRV_StoreVolumeLabelToBuf` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FBC60` | `0x1D8` | `PFDRV_StoreVolumeLabelToBPB` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FBE40` | `0x80` | `PFDRV_MediaEject` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FBEC0` | `0x58` | `PFDRV_MediaInsert` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FBF20` | `0xC` | `PFDRV_IsInserted` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FBF30` | `0xC` | `PFDRV_IsMountRequested` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FBF40` | `0x10` | `PFDRV_ClearMountRequested` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FBF50` | `0xC` | `PFDRV_IsUnmountRequested` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FBF60` | `0x10` | `PFDRV_ClearUnmountRequested` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FBF70` | `0xB4` | `PFDRV_init` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FC030` | `0x84` | `PFDRV_finalize` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FC0C0` | `0x294` | `PFDRV_mount` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FC360` | `0x78` | `PFDRV_unmount` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FC3E0` | `0x6C` | `PFDRV_format` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FC450` | `0x74` | `PFDRV_lread` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FC4D0` | `0x74` | `PFDRV_lwrite` | `pf_driver.c` | 100%, exact ELF/map |
| `0x801FC550` | `0x4` | `pdm_init_diskmanager` | `pdm_dskmng.c` | 100%, exact ELF/map |
| `0x801FC560` | `0x38` | `pdm_open_disk` | `pdm_dskmng.c` | 100%, exact ELF/map |
| `0x801FC5A0` | `0x4` | `pdm_close_disk` | `pdm_dskmng.c` | 100%, exact ELF/map |

## Independent source-selected verification

The original issue table is joined by address and size to the fresh aggregate objdiff report. All 33 retained functions across the three complete units, every section, and all 22 original issue rows pass. Actual `readelf -sW` symbols and final map contributions independently confirm every native address, size and source-object owner, including the two #765 wrappers. The real `ninja -t query build/wpse-v1/main.elf` dependency graph includes the compiled source inputs and excludes each original fallback. The final global `pdm_disk_set` has native address `0x803EAFA8` and exact size 2,292 in the linked ELF.

Configure/map, the completed default Ninja build, explicit frozen verification and each unitdiff command exit 0. The prior DOL was removed before every acceptance link; verification followed a known successful Ninja completion. The outside `351-source-audit.py` additionally checks raw original/rebuilt DOL equality, all native ELF/map rows, source-only inputs and the global extent, exiting 0. Latest-main integration is repeated through `tools/collab/land.py` before merging.

## Traps

- An unrolled loop step is not a record size. The native `0x3F58` step covers two `0x1FAC` volume records; verify the second access and the independent indexed consumers before choosing a public layout.
- The disk-set global ends four bytes before the next unit. Retain only its proven `0x8F4` bytes and leave the next unit's alignment padding outside the owned split.

- **Inlining a generic result converter can retain an extra success test or return temporary.** The initial driver candidate matched ten functions, including label I/O and both callbacks. Direct-return wrappers needed their actual error arms written in place to avoid an extra return-register copy. In mount, translate errors only inside the failing read's arm; mapping before testing the translated result retained a redundant zero assignment/comparison and produced 32 extra text bytes. Ordinary native control flow resolves all seventeen functions without a compiler change.
- **The two cache outputs use different stack orders in different callers.** Mount requires its hit local before its page-pointer declaration to place the page at stack `0x0C` and hit at `0x10`; the already matching label writer uses the opposite order. Do not apply a file-wide declaration reorder.
- **A wrapper's condition direction affects branch-free result selection.** The disk-manager wrappers need `if (result != PDM_OK) return result; return PDM_OK;` for the native `neg/or/srawi/and` sequence; the reversed condition selects another equivalent instruction sequence.
