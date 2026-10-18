# F-765-1: Complete FA disk-manager candidate

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. The initial scratch candidate below is historical. The first-function owner accepted the complete unit in PR #970; [F-765-2](765-complete-inventory.md) independently accepts both original #765 functions / 104 instruction bytes, the complete five-function source-selected unit and exact BSS extent. Shared-unit authorization, source selection and full frozen-DOL acceptance are resolved.

| Function | Address | Native size | Issue |
| --- | --- | --- | --- |
| `pdm_init_diskmanager` | `0x801FC550` | `0x4` | #351 |
| `pdm_open_disk` | `0x801FC560` | `0x38` | #351 |
| `pdm_close_disk` | `0x801FC5A0` | `0x4` | #351 |
| `pdm_open_partition` | `0x801FC5B0` | `0x38` | #765 |
| `pdm_close_partition` | `0x801FC5F0` | `0x30` | #765 |

The scratch candidate adapts CC0 Petari `src/RVL_SDK/vf/pdm_dskmng.c` at `e0ab141914921726a595573dc48d3f13d0922efe`, using FA's established names without the `VFi` prefix. FA delegates disk-set initialization to `pdm_disk_init_disk_set`, and omits the reference's pointer checks. The separate FA map in CC0 Wii IPL `config/43U/symbols.txt` at `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c` independently names all five functions with these exact extents; its split inventory places them together in `libs/RVL_SDK/src/fa/pdm_dskmng.c`. These are original FA names rather than descriptive replacements. The public wrappers retain the reference's two unused initialization parameters (`u32 config`, `void* param`) and signed partition ID; neither requires a target instruction change.

Wii/1.0 with the repository's exact `cflags_sdk` compiles all five bodies to the original extents and instruction streams. The candidate's whole `0xD0`-byte `.text` extent matches `0x801FC550–0x801FC620`, including alignment, after independently resolving its five REL24 relocations to the actual final-ELF callee addresses. This is a scratch native comparison, not final artifact acceptance. Full source selection, the actual map and frozen full-DOL verification remain required after authorization.

`PDM_DISK_SET pdm_disk_set` emits `0x8F4` bytes of NOBITS storage. Its canonical layout is the four-byte header, 26 disks of `0x30` bytes and 26 partitions of `0x28` bytes. The proposed BSS range is `0x803EAFA8–0x803EB89C`; the four alignment bytes through `0x803EB8A0` precede the next unit's disk table. The existing symbol's `0x8F8` size includes those alignment bytes and would need the bounded `0x8F4` correction. Other allocated data is absent.

The same pinned IPL FA map independently records `pdm_disk_set` at `0x81177818` with size `0x8F4`. This corroborates the size derived from the target-facing source layout; it does not authorize changing the neighboring split or replace the required target build.

## Traps

- Reversing an error-return condition changes the branch-free mask instructions. `if (result != PDM_OK) return result; return PDM_OK;` produces the target's `neg/or/srawi/and`; the reversed condition produces `cntlzw/rlwinm/neg/andc`. Both implement the same result selection, but only the former matches these three wrappers.
- The neighboring driver unit's loop is unrolled. At `0x801FBE40` and `0x801FBEC0`, thirteen iterations process two partition/flag pairs each. The second pair is `0x1FAC` after the first (`0x3F4C - 0x1FA0`, and `0x3F38 - 0x1F8C`), while the iteration step is `0x3F58`. This supports 26 records spaced `0x1FAC` apart; it does not prove that a single volume is `0x3F58` bytes. The old F-351-1/F-354-1 size conclusion and current padded `PF_VOLUME` tail need re-evaluation before a driver reconstruction uses `sizeof(PF_VOLUME)` or volume-array indexing. Existing matching cache and directory functions access only proven prefixes, so their instruction identity did not test the padded size. No neighboring layout has been changed here.

Independent scratch consumers strengthen the stride conclusion. A full header view with `PF_VOLUME` size `0x1FAC`, `p_part` at `0x1FA0`, `flags` at `0x1F8C`, and 26 volume records at `pf_vol_set + 0x88` compiles both callbacks to their native `0x80`- and `0x58`-byte bodies. All 216 bytes compare exactly after separately resolving four HA/LO relocations to the actual global address `0x803EB970`. A `PF_VOLUME*` declared before the index, with pointer advancement before index increment, preserves the observed scheduling. This is bounded consumer evidence for a future shared-layout correction; it neither changes the canonical header nor completes the rest of #351.

The separate native volume lookup at `0x80216BA0` directly multiplies its drive index by `0x1FAC`, then adds `pf_vol_set + 0x88`. The routine at `0x80216C00` repeats the same calculation before loading the record's driver error. These independent consumers corroborate the callback-derived stride without relying on loop unrolling or the reference layout.
