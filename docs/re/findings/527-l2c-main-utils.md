# Issue 527: BTE L2CAP main and utility units

## F-527-1: Unit boundaries and source for `l2c_main.c` and `l2c_utils.c`

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/l2c_main.c`, `l2c_utils.c` and `l2c_int.h`.

| File | `.text` | Data | Present functions |
| --- | --- | --- | --- |
| `l2c_main.c` | `0x8032F7B8`-`0x803307E8` | `.data` `0x803E44F8`-`0x803E47F3` (diagnostics and the `process_l2cap_cmd` jump table), `.bss` `0x804ADE40`-`0x804AE628` (`l2cb`) | `l2c_init`, `l2c_rcv_acl_data`, `process_l2cap_cmd`, `l2c_process_timeout`, `l2c_process_held_packets` |
| `l2c_utils.c` | `0x803307E8`-`0x803326AC` | `.data` `0x803E47F8`-`0x803E48DF`, `.sdata` `0x804B8F20`-`0x804B8F2E` (`TRUE`, `FALSE`) | `l2cu_allocate_lcb`, `l2cu_release_lcb`, `l2cu_find_lcb_by_bd_addr`, `l2cu_find_lcb_by_handle`, `l2cu_get_conn_role`, `l2cu_build_header`, `l2cu_send_peer_cmd_reject`, `l2cu_send_peer_connect_req`, `l2cu_send_peer_connect_rsp`, `l2cu_reject_connection`, `l2cu_send_peer_config_req`, `l2cu_send_peer_config_rsp`, `l2cu_send_peer_config_rej`, `l2cu_send_peer_disc_req`, `l2cu_send_peer_disc_rsp`, `l2cu_send_peer_echo_req`, `l2cu_send_peer_echo_rsp`, `l2cu_send_peer_info_rsp`, `l2cu_allocate_ccb`, `l2cu_release_ccb`, `l2cu_find_ccb_by_cid`, `l2cu_allocate_rcb`, `l2cu_release_rcb`, `l2cu_find_rcb_by_psm`, `l2cu_process_peer_cfg_req`, `l2cu_process_peer_cfg_rsp`, `l2cu_process_our_cfg_req`, `l2cu_process_our_cfg_rsp`, `l2cu_device_reset`, `l2cu_create_conn`, `l2cu_create_conn_after_switch`, `l2cu_find_lcb_by_state`, `l2cu_lcb_disconnecting` |

`l2c_main.c` starts with `l2c_init` (`0x8032F7B8`, already named), directly after `l2c_pin_code_request`, the last `l2c_link.c` function; its `.data` starts at `L2CAP - expected pkt start, got: %d`. `l2c_utils.c` starts with `l2cu_allocate_lcb` (the `memset`, `memcpy` and `l2c_link_adjust_allocation` call) right after `l2c_process_held_packets` (`L2CAP HOLD CONTINUE`), and its `.data` starts at `L2CAP - no buffer cmd_rej`. It ends before `RFCOMM_Init` (F-528-1). `l2cb` (0x7E8 bytes) is the whole L2CAP `.bss` and is defined in `l2c_main.c`; the `tL2C_CB`, `tL2C_LCB`, `tL2C_CCB` and `tL2C_RCB` layouts from Petari reproduce every offset.

`l2c_process_compress_setup_timeout` is empty and inlined into `l2c_process_timeout`. `l2cu_send_peer_info_req` and `l2cu_find_ccb_by_remote_cid` are dead-stripped; the source keeps them. Petari's `l2cu_send_feature_req`, `l2cu_check_feature_req`, `l2cu_check_feature_rsp` and the compression stubs are not in this build: their `badly formatted feature` strings are absent from the `l2c_utils.c` string data, which would keep them even if the functions were stripped (F-528-1). The source omits them.

The callees outside these units were named from the paired relocations of the compiled and target objects, matched per function, because the stripped functions shift absolute offsets:

| Address | Name |
| --- | --- |
| `0x803118B0` | `GKI_remove_from_queue` |
| `0x80311980` | `GKI_getfirst` |
| `0x80311990` | `GKI_getnext` |
| `0x8031A534` | `btm_acl_removed` |
| `0x8031A65C` | `BTM_SwitchRole` |
| `0x8031D6B4` | `BTM_ReadLocalFeatures` |
| `0x8031EA7C` | `BTM_InqDbRead` |
| `0x80320FB8` | `btm_remove_sco_links` |
| `0x80321584` | `btm_is_sco_active_by_bdaddr` |
| `0x8032320C` | `btm_sec_is_bonding` |
| `0x803236CC` | `btm_sec_disconnect` |
| `0x8032D150` | `l2c_csm_execute` |
| `0x8032EB98` | `l2c_link_timeout` |
| `0x8032EE14` | `l2c_link_check_send_pkts` |
| `0x8032F0E0` | `l2c_link_adjust_allocation` |

The units are compiled with GC/3.0a3 (`BTE_COMPILER`, F-534-2) and the library `cflags_sdk`. All 38 functions and every data section score 100% in objdiff 3.8.2 on the first build. With both objects `Matching`, `ninja` exits 0 and `tools/verify/verify.py verify` passes.

`l2c_utils.c` covers the 22 functions `0x803311F8`-`0x803326AC` that #528 left for the `l2c_utils.c` split (F-528-1), so every function of #528 is now in a matching unit.

The first nine functions of this block, `0x8032F0E0`-`0x8032F7B8` (`l2c_link_adjust_allocation`, `l2c_link_process_num_completed_pkts`, `l2c_link_processs_num_bufs`, `l2cap_link_chk_pkt_start`, `l2cap_link_chk_pkt_end`, `l2c_link_role_changed`, `l2c_link_role_change_failed`, `l2c_link_segments_xmitted` and `l2c_pin_code_request`), are the tail of `l2c_link.c`, whose first function lies in #526's block, so #526 owns that split. They are not handled here.

## Provenance

Sources and the declarations added to `include/revolution/bte/` are rewritten for this project after Petari's BTE files. Those files carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE SDP provenance](../licenses/534-bte-sdp-NOTICE.md), which also covers these files.

## Traps

- **Pair relocations per function, not by object offset.** Kept but dead-stripped functions shift every later offset in the compiled object. Pairing by absolute offset named `0x8031EA7C` `btm_is_sco_active_by_bdaddr`; it is `BTM_InqDbRead`. objdiff still scored 100%, so only the full link and verification catch such a name. Key relocations by function and offset within the function. Evidence: F-527-1.
