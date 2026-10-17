# Issue 512: BTA device-manager actions

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). Disassembly is from dtk 1.8.4's split of the verified build, `build/wpse-v1/asm/auto_03_802B1A10_text.s`, with binutils 2.42-2. The naming and structure reference is Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` (`src/RVL_SDK/bte`); target instructions are the evidence for correspondence.

## F-512-1: Unit boundaries in 0x8031486C-0x8031590C

The issue block crosses three source files.

| Unit | Range, exclusive end | Functions in this issue | Owner of the split |
| --- | --- | --- | --- |
| `ptim.c` | `.text` starts at `0x80314820` (`ptim_init`, issue #511) and ends at `0x80314A00` | `ptim_timer_update` `0x8031486C`, `ptim_start_timer` `0x80314920`, `ptim_stop_timer` `0x803149C0` | #511 |
| `utl.c` | `.text` `0x80314A00-0x80314A44` | `utl_freebuf` | #512 |
| `bta_dm_act.c` | `.text` `0x80314A44-0x80316D80`; `.rodata` `0x80397EE8-0x80397F90`; `.data` `0x803E06E8-0x803E0828`; `.bss` `0x804AA968-0x804AA998`; `.sdata2` `0x804BB5F8-0x804BB600` | 21 functions from `bta_dm_enable` to `bta_dm_queue_search` | #512; the file continues through #513 and #514 |

Evidence:

- `ptim.c`: `0x80314820` calls `GKI_init_timer_list` and stores the period and timer id, as in the reference `ptim_init`. The three following functions update, start and stop the timer queue with the reference's `GKI_MS_TO_TICKS` division by ten. Only `ptim_init` belongs to #511, but it is the file's first function, so the split belongs to #511, which is assigned. This change renames the three in-region functions and leaves the split and source to that owner.
- `utl.c`: `0x80314A00` frees a buffer through a pointer and clears it, the reference `utl_freebuf`. The reference's other utl functions are absent from the linked target, so the linker dead-stripped them. The following function starts the device manager, so the unit is exactly this function.
- `bta_dm_act.c` starts at `bta_dm_enable` (`0x80314A44`): it clears the search and connected-service control blocks and registers the security callbacks. `BTA_EnableBluetooth` at `0x80316D80` clears `bta_dm_cb` and registers the DM subsystem, which starts `bta_dm_api.c`. The reference's last functions, a 4-byte empty `bta_dm_reset_complete` at `0x80316D2C` and `bta_dm_send_hci_reset` at `0x80316D30`, close the file.
- Data: `bta_service_id_to_uuid_lkup_tbl` (`0x80397EE8`, 23 halfwords, padded to `0x30`), `bta_service_id_to_btm_srv_id_lkup_tbl` (`0x80397F18`, 23 words, padded to `0x60`) and `bta_security` (`0x80397F78`, 6 pointers) are referenced only from this file, and `0x80397F90` is first referenced by `bta_dm_pm.c` at `0x803171FC`. Seven trace strings at `0x803E06E8-0x803E0823` are all referenced from this file's text; the next string, `0x803E0828`, is referenced from `bta_dm_pm.c`. `bta_dm_compress_srvcs` (`0x804AA968`, 5 × 9 bytes) precedes `bta_dm_search_cb` and `bta_dm_cb`, which belong to `bta_dm_main.c`. `0x804BB5F8-0x804BB600` holds the 4-byte `attr_list` initializer of `bta_dm_find_services`.

The reference's static helpers `bta_dm_start_remote_name_response_timer`, `bta_dm_stop_remote_name_response_timer`, `bta_dm_remote_name_request_cmd_failed`, `bta_dm_init_compress` and `bta_dm_get_remname` have no standalone copy in the target. They are inlined into this issue's functions, so their definitions are included in the source at their reference positions. `bta_dm_search_cancel_notify` (`0x80315A38`, issue #513) also exists standalone and is inlined into `bta_dm_search_cancel`; its body is included for that reason.

Confidence: high for all boundaries and names.

## F-512-2: bta_dm_act.c needs a GameCube compiler

With identical source and Petari's SDK flags (`cflags_sdk`: `-O4,p -inline auto,level=3 -ipa file -sdata 8 -sdata2 8`), the 21 functions compare as follows. The comparison counts exact instruction streams with relocation targets masked, against the dtk-split original object.

| Compiler | Exact functions |
| --- | --- |
| Wii/1.0, 1.0a, 1.0RC1, 1.1, 1.3, `0x4201_127`, each with `-func_align 4` | 12/21 |
| GC/3.0a3, GC/3.0a5, GC/3.0a5.2 (4-byte function alignment by default) | 21/21 |

The Wii compilers schedule independent loads and stores differently, for example the `p_sec_cback` load in `bta_dm_enable` and the `sdp_results` store in `bta_dm_search_result`. `-ipa file` is required: without it the later-defined static helpers are not inlined (15/21 under GC/3.0a3). The unit uses GC/3.0a3 with `cflags_sdk`, following Petari's BTE library setting. GC/3.0a3, 3.0a5 and 3.0a5.2 are not distinguished by this unit. `utl_freebuf` matches under every candidate. Confidence: high that a GC compiler is required; the exact GC version is unresolved.

This contradicts the generic RVL_SDK result in F-0007, which did not sample BTE. F-511-1 found Wii/1.0 with `-func_align 4` sufficient for `bta_sys_conn.c`, whose functions have no scheduling choices of this kind. Test BTE units against GC compilers before choosing.

## F-512-3: Reconstruction status

objdiff 3.8.2 reports `utl_freebuf` at 100% code and data, and all 21 `bta_dm_act.c` functions in this issue at 100% (3,784 code bytes). The `bta_dm_act.c` unit as a whole is 41.95% because the other 35 functions belong to #513 and #514. Both units stay `NonMatching` and link from the original objects, because:

- Their source references externals still carrying dtk names outside this region: `GKI_getbuf` (`0x80310ED0`), `GKI_freebuf` (`0x80311160`), `bdcpy` (`0x803142B4`), `bta_sys_*`, `BTM_*`, `SDP_*`, `L2CA_*`, `WBT_ExtCreateRecord`, `LogMsg_0`, `appl_trace_level` (`0x804B9B90`), `btm_cb` (`0x804AAD78`), `BT_BD_ANY` (`0x804BB630`), `bta_dm_cfg` (`0x80397E48`), `bta_dm_cb` (`0x804AAA14`), `bta_dm_search_cb` (`0x804AA998`) and `bta_dm_conn_srvcs` (`0x804AAB18`).
- `bta_dm_act.c` lacks the functions, statics and five trace strings owned by #513 and #514.

`python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exit 0 with the new splits, names and source. Verification uses the original objects for these units, so it shows that the splits and names keep the DOL identical, not that the source links.

## Headers

The new BTE headers in `include/RVL_SDK/bte/` declare only what these units use, with layouts from the reference. Target-checked offsets: `tBTA_DM_CB` (`p_sec_cback` `0x50`, `disabling` `0x72`, `disable_timer` `0x74`, `device_list.count` `0x4D`, peer device stride `0x0B`, `keep_acl_on_shut_down` `0x101`); `tBTA_DM_SEARCH_CB` (size `0x7C`, `peer_bdaddr` `0x1A`, `peer_name` `0x20`, `search_timer` `0x40`, `rnr_timer` `0x58`, `service_index` `0x70`, `p_search_queue` `0x74`, `wait_disc`/`sdp_results` `0x78`/`0x79`); `tBTA_DM_MSG` (size `0x110`; discovery result services at `0x108`, result at `0x10C`); `tBTM_INQ_INFO` (`remote_bd_addr` at `2`, `appl_knows_rem_name` at `0x10`); `TIMER_LIST_ENT` (`p_cback` at `8`); and `btm_cb.devcb.page_timeout` at `0x64C`. The complete `tBTM_CB` layout compiles to `0x27C4` bytes, inside the target's `0x27C8` allocation. Its other members come from the reference and are not exercised here.

`include/MSL_C/string.h` declares only `memcpy`, `memset` and `strncpy`.

## Provenance

The source and header layouts adapt Petari's `bta_dm_act.c`, `bta_dm_int.h`, `bta_api.h`, `bta_sys.h`, `btm_api.h`, `btm_int.h`, `bt_types.h`, `bt_trace.h`, `bt_target.h`, `gki.h`, `sdp_api.h`, `sdpdefs.h`, `l2c_api.h`, `wbt_api.h`, `bd.h`, `btu.h` and `utl.c`. These files carry Broadcom Apache-2.0 notices and changes by muff1n1634; see the [BTE notice](../licenses/510-bte-NOTICE.md#issue-512-additions).
