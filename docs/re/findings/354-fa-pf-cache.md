# F-354-1: FA `pf_cache.c`

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issues: #354, together with the tails of #353 and #355 that belong to the same unit. Compiler: Wii/1.0 `cflags_sdk` (F-350-1), default 16-byte function alignment.

`pf_cache.c` occupies `.text` `0x801FE250`–`0x80200004` and is the PrFILE2 sector cache. Its 36 functions are all identical, and the unit is `Matching` and linked. `python configure.py`, `.venv/Scripts/ninja` (exit 0) and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` pass.

The unit crosses issue boundaries:

| Issue | Range |
| --- | --- |
| #353 | `0x801FE250`–`0x801FF004` |
| #354 | `0x801FF010`–`0x801FFD84` |
| #355 | `0x801FFD90`–`0x80200004` |

The split ends at the last function's end, `0x80200004`. The next unit starts at `0x80200010`.

## Layout

`PF_CACHE_PAGE` (0x28 bytes) and `PF_SECTOR_CACHE` (0x24 bytes) have the VF layouts. They are in `include/revolution/fa/pf_cache.h`.

FA's `PF_VOLUME` is `0x3F58` bytes (F-351-1). In this unit, only three parts of it are accessed:

| Offset | Field | Evidence |
| --- | --- | --- |
| 0x0000 | `bpb` (`PDM_BPB`) | `bytes_per_sector` at 0, `total_sectors` at 0x8, `fs_info_sector` at 0x14, `fat_type` at 0x1C, `log2_bytes_per_sector` at 0x20 and `first_data_sector` at 0x30 are all read. |
| 0x1624 | `cache` | Every cache field is accessed. |
| 0x1648 | `label[12]` | Written by the volume-label routines (F-351-1). |

`include/revolution/fa/pf_volume.h` gives the remaining ranges as `files` (0x38–0x1624) and `state` (0x1654–0x3F58) until their layout is recovered. The driver calls at `0x801FC450` and `0x801FC4D0` take `(p_vol, buf, sector, num_sector, p_num_success)`. They are named `PFDRV_lread` and `PFDRV_lwrite` after VF.

## Differences from VF

FA's cache is an older PrFILE2 revision than VF's:

- There are no `dCommon_FlushFromVol` calls, no FAT mirror update after a write, and no `is_fat` test in the page-list size check.
- `PFCACHE_SearchForPage` and `PFCACHE_SearchForFreePage` are out-of-line functions. Each checks the list head separately from its loop.
- `PFCACHE_FlushPageIfNeeded` writes the whole page when no modified range is recorded.
- `PFCACHE_DoWritePage` and `PFCACHE_DoWriteSector` widen the modified range from `p_buf` instead of taking explicit start and end pointers.
- In `PFCACHE_DoAllocatePage`, a FAT32 sector outside the data area does not fail. Only the FSInfo sector shrinks the page to one sector.
- `PFCACHE_DoWriteNumSectorAndFreeIfNeeded` keeps the original's partial-overlap start pointer, `buffer + ((size - (sector - page_sector) - 1) << shift)`. It writes through only when sectors remain uncached.
- `PFCACHE_FlushDataCacheSpecific` runs when any of the three mode bits is set.
- `PFCACHE_DoFlushCache` returns immediately in write-through mode.

## Names

VF names without `VFi`, medium confidence:

- `PFCACHE_InitPageList`
- `PFCACHE_SearchForPage`
- `PFCACHE_SearchForFreePage`
- `PFCACHE_FlushPageIfNeeded`
- `PFCACHE_DoAllocatePage`
- `PFCACHE_DoReadPage`
- `PFCACHE_DoReadPageAndFlushIfNeeded`
- `PFCACHE_DoReadNumSector`
- `PFCACHE_DoWritePage`
- `PFCACHE_DoWriteNumSectorAndFreeIfNeeded`
- `PFCACHE_DoFlushCache`
- `PFCACHE_SetCache`
- `PFCACHE_SetFATBufferSize`
- `PFCACHE_SetDataBufferSize`
- `PFCACHE_InitCaches`
- `PFCACHE_AllocateDataPage`
- `PFCACHE_FreeDataPage`
- `PFCACHE_ReadFATPage`
- `PFCACHE_ReadDataPage`
- `PFCACHE_ReadDataPageAndFlushIfNeeded`
- `PFCACHE_ReadDataNumSector`
- `PFCACHE_WriteFATPage`
- `PFCACHE_WriteDataPage`
- `PFCACHE_WriteDataNumSectorAndFreeIfNeeded`
- `PFCACHE_SearchDataCache`
- `PFCACHE_FlushFATCache`
- `PFCACHE_FlushDataCacheSpecific`
- `PFCACHE_FlushAllCaches`
- `PFCACHE_FreeAllCaches`

Descriptive, low confidence. FA routines without a VF counterpart are named for what they do:

- `PFCACHE_DoWriteSector` (`0x801FF130`) and `PFCACHE_WriteFATSector` (`0x801FFC50`) write one sector through the FAT cache.
- `PFCACHE_SetWriteThrough` (`0x801FF7D0`) and `PFCACHE_ClearWriteThrough` (`0x801FF850`) set and clear mode bit 1.
- `PFCACHE_AllocateFATPage` (`0x801FF870`) and `PFCACHE_FreeFATPage` (`0x801FF970`) are the FAT-list counterparts of VF's data-page routines.
- `PFCACHE_FlushDataCache` (`0x801FFD50`) is the data-list counterpart of `PFCACHE_FlushFATCache`.

Mode bits: 1 is write-through, 2 is signature-based flushing, 4 is FAT write-through. Page status bits: 1 is used, 2 is modified, 4 is FAT page.

## Traps

- **A function that looks inlineable can still be a call.** Trigger: VF's `static inline` `SearchForPage` and `SearchForFreePage` were inlined into every caller, so the code grew. FA calls them out of line, and each one checks the head outside the loop. That larger body is above the auto-inline limit. Writing that body reproduces both the functions and their callers. Evidence: F-354-1.
- **One page-clearing idiom compiles three ways.** Trigger: the `stat`, `p_mod_sbuf` and `p_mod_ebuf` stores were scheduled differently in different functions.
  - `FlushPageIfNeeded` needs the pointer stores before `stat`.
  - The read paths need `stat` first, written through `(*pp_page)->`, which reloads the pointer after each store.
  - The free paths need `stat &= ~MODIFIED; stat &= ~USED` before the pointer stores.
  - The global `PFCACHE_FreeFATPage` and `PFCACHE_FreeDataPage` need `p_mod_sbuf`, then a combined `stat` mask, then `p_mod_ebuf`.

  A single inline helper cannot produce all of these. Evidence: F-354-1.
- **VF's unsimplified overlap arithmetic matters.** Trigger: case three of the multi-sector copies. `size - (page_sector + size - (sector + n))` is folded later than LICM, so it produces a fresh `n + sector` add. The simplified `sector + n - page_sector` reuses the hoisted loop-invariant sum, which changes register use. In the write path, the counts must also be in VF's order (`num_rest_sector` before `*p_num_success`). That lets one value serve both statements. Evidence: F-354-1.
