# Issue 526: BTE L2CAP channel state machine and link units

## F-526-1: Unit boundaries and source for `l2c_csm.c` and `l2c_link.c`

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/l2c_csm.c` and `l2c_link.c`.

| File | `.text` | `.data` | Present functions |
| --- | --- | --- | --- |
| `l2c_csm.c` | `0x8032D150`-`0x8032E62C` | `0x803E3A50`-`0x803E4287` (state jump tables and diagnostics) | `l2c_csm_execute`, static `l2c_csm_closed`, `l2c_csm_orig_w4_sec_comp`, `l2c_csm_term_w4_sec_comp`, `l2c_csm_w4_l2cap_connect_rsp`, `l2c_csm_w4_l2ca_connect_rsp`, `l2c_csm_config`, `l2c_csm_open`, `l2c_csm_w4_l2cap_disconnect_rsp`, `l2c_csm_w4_l2ca_disconnect_rsp`, and `forward_peer_data` |
| `l2c_link.c` | `0x8032E62C`-`0x8032F7B8` | `0x803E4288`-`0x803E44F2` | `l2c_link_hci_conn_req`, `l2c_link_hci_conn_comp`, `l2c_link_sec_comp`, `l2c_link_hci_disc_comp`, `l2c_link_hci_qos_violation`, `l2c_link_timeout`, `l2c_link_send_to_lower`, `l2c_link_check_send_pkts`, `l2c_link_adjust_allocation`, `l2c_link_process_num_completed_pkts`, `l2c_link_processs_num_bufs`, `l2cap_link_chk_pkt_start`, `l2cap_link_chk_pkt_end`, `l2c_link_role_changed`, `l2c_link_role_change_failed`, `l2c_link_segments_xmitted`, `l2c_pin_code_request` |

`l2c_csm.c` starts at `l2c_csm_execute` (`0x8032D150`, already named). Its `.data` starts with that function's nine-entry jump table at `0x803E3A50`, whose entries point into `l2c_csm_execute`, followed by `L2CAP - st: CLOSED evt: %d`. It ends with `L2CAP - cannot send buffer, offset: %d`, used by `forward_peer_data`. `l2c_link.c` starts at `l2c_link_hci_conn_req` (`0x8032E62C`, `L2CAP failed to allocate LCB`). It ends at `l2c_pin_code_request`, right before `l2c_init` (F-527-1). Its strings end at `L2CAP - rcvd segment complete, unknown handle: %d`. The four bytes of padding before `l2c_main.c` data at `0x803E44F8` are left outside the split.

Everything before `0x8032D150` and `0x803E3A50` in this issue's block belongs to `l2c_api.c`: `L2CA_SetIdleTimeout` (`0x8032CF68`) and the three functions after it, plus their strings. The first `l2c_api.c` function is in #525's block, so that split belongs to #525 and is not handled here.

Petari's `l2c_link_get_decompressed_pkt` and `l2c_link_pkts_rcvd` have no strings and would be dead-stripped. Nothing in the binary shows whether this build contained them, so the source omits them. Petari passes `0` to `pL2CA_ConnectPnd_Cb`, and the target code loads the same constant.

`l2c_link_sec_comp` selects the event with an `if`/`else`. With the GC/3.0a3 optimizer, the ternary form in Petari compiles to a different branchless sequence.

The callees outside these units were named by pairing the relocations of the compiled and target objects per function:

| Address | Name |
| --- | --- |
| `0x8031A220` | `btm_acl_created` |
| `0x8031C370` | `btm_find_dev` |
| `0x8031FDCC` | `BTM_ReadPowerMode` |
| `0x80321150` | `btm_sco_acl_removed` |
| `0x8032206C` | `btm_sec_l2cap_access_req` |

The units are compiled with GC/3.0a3 (`BTE_COMPILER`, F-534-2) and the library `cflags_sdk`. All 28 functions and both `.data` sections score 100% in objdiff 3.8.2. With both objects `Matching`, `ninja` exits 0 and `tools/verify/verify.py verify` passes.

`l2c_link.c` includes the nine functions `0x8032F0E0`-`0x8032F7B8` that #527 left for this split (F-527-1). Every function in #527's block is now in a matching unit.

## Provenance

Sources and the declarations added to `include/revolution/bte/` are rewritten for this project after Petari's BTE files. Those files carry Broadcom Apache-2.0 notices and changes by muff1n1634. See [BTE SDP provenance](../licenses/534-bte-sdp-NOTICE.md), which also covers these files.

## Traps

- **Petari's ternaries do not always match on GC/3.0a3.** `event = status == BTM_SUCCESS ? L2CEVT_SEC_COMP : L2CEVT_SEC_COMP_NEG;` compiled to branches. The target computes `-(status == 0)` outside the loop and adds 8 inside it. An `if`/`else` assignment produces that exact sequence; the other expression forms tried do not. When a small selection mismatches, try `if`/`else` before arithmetic rewrites. Evidence: F-526-1.
