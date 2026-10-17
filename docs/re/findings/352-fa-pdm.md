# F-352-1: FA PDM BPB, disk and MBR units

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #352. Compiler: Wii/1.0 `cflags_sdk` (F-350-1), default 16-byte function alignment.

The issue range `0x801FC5B0`–`0x801FD5C4` covers the end of the PDM disk manager and three complete PDM units. These units sit in the RVL_SDK.FA library and are split and linked here.

| Unit | `.text` | Other sections | Status | Functions |
| --- | --- | --- | --- | --- |
| `pdm_bpb.c` | `801FC620–801FCB18` | None | Matching | 3/3 identical |
| `pdm_disk.c` | `801FCB20–801FD354` | `.bss` `803EB8A0–803EB970` (`pdm_disk_tbl`) | Matching | 17/17 identical |
| `pdm_mbr.c` | `801FD360–801FD5C4` | None | NonMatching, linked | 1/2 identical; 94.1% fuzzy |

`python configure.py`, `.venv/Scripts/ninja` (exit 0) and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` pass with all three units linked.

## Order and naming evidence

The function order and bodies follow VF's `pdm_bpb.c`, `pdm_disk.c` and `pdm_mbr.c` in the CC0 Petari sources (`e0ab141`). FA drops VF's `VFi` prefix, so the names are `pdm_bpb_*`, `pdm_disk_*` and `pdm_mbr_*`. The prefix change is inferred from FA's `PFSTR_*` and `pf_*` names (F-350-2). Confidence is medium.

FA differs from VF in these ways:

- `pdm_bpb_check_boot_sector` lives in `pdm_mbr.c`, after `pdm_mbr_check_master_boot_record`. The `.text` order shows this.
- FA has no `pdm_disk_check_disk_handle` or lock calls. Each `pdm_disk_*` driver wrapper calls `pdm_part_set_driver_error_code` (`0x801FDC40`) and returns PDM error `0x15`.
- The disk set at `.bss` `0x803EAFA8` (`pdm_disk_set`, 0x8F4 bytes) holds 26 `PDM_DISK` entries of 0x30 bytes, starting at offset 4.
- The driver tables live in a separate static `pdm_disk_tbl[26]` at `0x803EB8A0`.

`include/revolution/fa/pdm.h` records the layouts:

- `PDM_DISK`
  - `status` at 0
  - `disk_tbl` at 4
  - `open_disk_cnt` (s16) at 0xC
  - `mount_cnt` (s16) at 0xE
  - `disk_info` at 0x18
  - `p_cur_part` at 0x2C
- `PDM_BPB` (0x38)
- `PDM_DISK_INFO` (0x14)

## The `pdm_dskmng.c` boundary

The disk manager starts at `0x801FC550`, inside #351's range. Its last two functions, `0x801FC5B0` and `0x801FC5F0`, fall at the start of #352's range. They match VF `pdm_open_partition` and `pdm_close_partition`. They are left unsplit for whoever reconstructs `pdm_dskmng.c` with the rest of the unit; splitting them alone would need a half-unit.

# F-352-2: Remaining `pdm_mbr_check_master_boot_record` difference

The remaining 20 instruction differences are all scheduling and register choices inside the first loop. That loop composes the little-endian `start_sector` and `num_sectors` words from bytes. The control flow, both loops, the stores and the returns already line up.

The original stores 0 to `start_sector[i]` before the composed value. Writing `start_sector[i] = 0;` first reproduces that dead store. These variants did not close the gap:

- `|` instead of `+`
- shift-then-add order
- a pointer temporary
- an index into `buf + i * 16` instead of advancing `buf`

`PDM_LOAD_LE32` and `PDM_LOAD_LE16` emit different code, the byte-swap and aligned paths, so the composition is open-coded.

## Traps

- **A chain of `!=` comparisons becomes an unsigned binary search.** Trigger: `pdm_bpb_check_boot_sector` compares bytes-per-sector, sectors-per-cluster and media against constant sets using `cmplwi` trees. A `switch` emitted signed `cmpwi`. Writing the VF-style chain of `!=` tests on unsigned locals (`u16`, `u8`) reproduces the tree exactly. Evidence: F-352-1.
- **FA's aligned little-endian 16-bit load reads the rounded-up even offset.** Trigger: field loads at odd BPB offsets (`0xB`, `0x11`, `0x13`) take a runtime alignment branch. The aligned path reads the halfword at `(off + 1) & ~1` and swaps it. The `PDM_LOAD_LE16` macro in `pdm.h` reproduces both paths. Evidence: F-352-1.
- **A direct assignment of the driver result keeps `unmount` in one register.** Trigger: `pdm_disk_unmount` mismatched when the result went through a separate `err` temporary. Assigning `result = p_func->unmount(...)` matches. Evidence: F-352-1.
