# F-353-1: FA `pdm_partition.c`

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #353. Compiler: Wii/1.0 `cflags_sdk` (F-350-1), default 16-byte function alignment.

`pdm_partition.c` is the first part of #353's range. It occupies `.text` `0x801FD5D0`–`0x801FE250` and contains 18 functions. All 18 are identical, and the unit is `Matching` and linked. `python configure.py`, `.venv/Scripts/ninja` (exit 0) and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` pass. The unit has no data of its own. Partitions live in `pdm_disk_set.partition[26]` at `pdm_disk_set + 0x4E4`, after the 26 disks (F-352-1).

## Layout

FA's `PDM_PARTITION` is 0x28 bytes and differs from VF's 0x2C-byte record:

| Offset | Field | Evidence |
| --- | --- | --- |
| 0x00 | `status` | Bits: 1 open, 2 media changed, 4 initialized, 8 mounted. Set and cleared by open/close, `pdm_part_set_change_media_state`, initialize/finalize and mount/unmount. |
| 0x04 | `p_disk` | Every wrapper passes it to `pdm_disk_*`. |
| 0x08 | `part_id` | 32-bit; compared with the running partition count. |
| 0x0C | `start_sector` | Written by `pdm_part_get_start_sector`. |
| 0x10 | `total_sector` | Written by the same function; bounds `lsector` in logical read and write. |
| 0x14 | `driver_last_error` | Set and get accessors. |
| 0x18 | `reserved[2]` | Not accessed in this unit. |
| 0x20 | `callback` | Two media callbacks, insert then eject. |

## Names

Where VF's `pdm_partition.c` has the same routine, the name is the VF name without `VFi`, medium confidence:

- `pdm_part_get_start_sector`
- `pdm_part_open_partition`
- `pdm_part_close_partition`
- `pdm_part_format`
- `pdm_part_logical_read`
- `pdm_part_logical_write`
- `pdm_part_get_media_information`
- `pdm_part_set_change_media_state`
- `pdm_part_set_driver_error_code`
- `pdm_part_get_driver_error_code`

`pdm_part_chg_ltop` is a separate function in FA; VF has it as a `static inline`.

FA has functions with no VF counterpart. Their names are descriptive, low confidence:

- `pdm_part_initialize`, `pdm_part_mount`, `pdm_part_unmount` and `pdm_part_finalize` wrap the disk calls and track status bits.
- `pdm_part_set_callback` and `pdm_part_clear_callback` handle the callbacks at 0x20.
- `pdm_part_is_initialized` returns status bit 4. It is called from `0x80222E90`.

The PDM error codes added to `include/revolution/fa/pdm.h` (`7`, `0xA`, `0xB`, `0xC`, `0xF`, `0x12`) are named for the conditions that return them.

## Behaviour of `pdm_part_get_start_sector`

The function reads sector 0 and composes all four primary entries before it walks them. Then:

- A primary entry whose own first sector is not an MBR is counted as a partition.
- An entry whose first sector is an MBR is followed as an extended chain. The chain loop resets its index to 0 after following a link, and the `for` increment then makes the index 1. The original therefore never revisits entry 0 of the next EPBR. The reconstruction keeps this behaviour.
- Without an MBR, partition IDs 0 and 1 map to the whole disk.

# F-353-2: The rest of #353 is the start of `pf_cache.c`

`0x801FE250`–`0x801FF004` contains the first eight functions of PrFILE2 `pf_cache.c`. They take `PF_CACHE_PAGE` records of 0x28 bytes with the VF layout. They read and write through the driver wrappers `0x801FC450` and `0x801FC4D0` (#351). The order matches VF:

| Address | VF counterpart |
| --- | --- |
| `0x801FE250` | `PFCACHE_InitPageList` |
| `0x801FE4B0` | `PFCACHE_SearchForPage` (an out-of-line function in FA) |
| `0x801FE590` | `PFCACHE_SearchForFreePage` |
| `0x801FE620` | `PFCACHE_FlushPageIfNeeded` |
| `0x801FE710` | `PFCACHE_DoAllocatePage` |
| `0x801FE940` | `PFCACHE_DoReadPage` |
| `0x801FEB50` | `PFCACHE_DoReadPageAndFlushIfNeeded` |
| `0x801FEDB0` | `PFCACHE_DoReadNumSector` |

The unit continues into #354's range. Its functions read `PF_VOLUME` fields; for example, the bytes-per-sector shift is at volume offset 0x20, the `PDM_BPB` at the start of the volume. F-351-1 notes that FA's `PF_VOLUME` (stride 0x3F58) has no verified layout yet. These functions are left unsplit and keep their `fn_` names until that layout and the unit's end are established.

## Traps

- **Repeated initialisation can be visible as a second `li`.** Trigger: `pdm_part_get_start_sector` emitted `li r14, 0` twice, for the early store of 0 and again before the primary-entry loop. Assigning `part_cnt = 0` both at the top and directly before the loop reproduces both. Evidence: F-353-1.
- **Local array declaration order sets stack offsets.** Trigger: the start and number arrays at `0x38` and `0x28` came out swapped. Declaring `start_sectors` before `num_sectors` places them as in the target. Changing the statement order of the byte composition made no difference to scheduling. Evidence: F-353-1.
- **A `const` source pointer changes a two-field copy.** Trigger: `pdm_part_set_callback` loaded both fields before storing when the parameter was `const PDM_PART_CALLBACK*`. The struct assignment did the same. A non-`const` pointer with field-by-field assignment interleaves the loads and stores as in the target. Evidence: F-353-1.
- **An array element reached by pointer and by index generates different code.** Trigger: `pdm_part_open_partition` and `pdm_part_set_change_media_state` matched only when the tests and stores index `pdm_disk_set.partition[part_no]` directly. In `pdm_part_set_change_media_state`, the pointer is kept only for the status update and the callback argument. Evidence: F-353-1.
