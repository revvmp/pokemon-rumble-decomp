# Issue 530: BTE RFCOMM L2CAP interface and multiplexer state machine units

## F-530-1: Unit boundaries and source for `rfc_l2cap_if.c` and `rfc_mx_fsm.c`

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/rfc_l2cap_if.c` and `rfc_mx_fsm.c`.

| File | `.text` | `.data` | Present functions |
| --- | --- | --- | --- |
| `rfc_l2cap_if.c` | `0x803340D0`-`0x80334A04` | `0x803E4D58`-`0x803E4E91` (LCID diagnostics) | `rfcomm_l2cap_if_init`, `RFCOMM_ConnectInd`, `RFCOMM_ConnectCnf`, `RFCOMM_ConfigInd`, `RFCOMM_ConfigCnf`, `RFCOMM_QoSViolationInd`, `RFCOMM_DisconnectInd`, `RFCOMM_BufDataInd`, `RFCOMM_CongestionStatusInd`, `rfc_save_lcid_mcb` |
| `rfc_mx_fsm.c` | `0x80334A04`-`0x80335648` | `0x803E4E98`-`0x803E514E` (state diagnostics and the six jump tables) | `rfc_mx_sm_execute`, `rfc_mx_sm_state_idle`, `rfc_mx_sm_state_wait_conn_cnf`, `rfc_mx_sm_state_configure`, `rfc_mx_sm_sabme_wait_ua`, `rfc_mx_sm_state_wait_sabme`, `rfc_mx_sm_state_connected`, `rfc_mx_sm_state_disc_wait_ua`, `rfc_mx_conf_cnf`, `rfc_mx_conf_ind` |

`rfcomm_l2cap_if_init` is identified by its ten callback stores into `rfc_cb.rfc.reg_info` and the tail call to `L2CA_Register` with PSM 3. The other names follow the diagnostics (`RFCOMM_ConnectCnf LCID:0x%x`, `rfc_mx_sm_state_idle - evt:%d`, `rfc_mx_conf_cnf p_cfg:%08x res:%d `) and the bluedroid order, which the target keeps. `rfc_find_lcid_mcb` is inlined into all seven callers and its out-of-line copy is stripped; its two strings remain at the start of the `.data` split. `rfc_save_lcid_mcb` is inlined into `RFCOMM_ConnectInd` but called from the multiplexer states, so it keeps an out-of-line copy. `rfc_mx_send_config_req` is inlined into both callers. The `.data` split of `rfc_l2cap_if.c` ends at the last string; `lbl_803E4D58` was shortened to `0x139` bytes so the split can end before the padding, as in F-532-1.

The tail functions `0x80335464` and `0x80335558` left by #531 (F-531-1) are `rfc_mx_conf_cnf` and `rfc_mx_conf_ind`, and are part of this split.

External callees named from the paired call sequences of the compiled and target objects:

| Address | Name | Evidence |
| --- | --- | --- |
| `0x803326FC` | `PORT_StartCnf` | Called with `RFCOMM_ERROR` when `L2CA_ConnectReq` returns 0, and with the L2CAP result |
| `0x8033285C` | `PORT_StartInd` | SABME case of `rfc_mx_sm_state_wait_sabme` |
| `0x80333404` | `PORT_CloseInd` | Every `RFC_MX_EVENT_DISC_IND` case |
| `0x80333DA4` | `port_find_dlci_port` | Called with the received DLCI in `RFCOMM_BufDataInd` after a SABME on an unknown port |

The units are compiled with GC/3.0a3 (`BTE_COMPILER`, F-534-2) and the library `cflags_sdk`. All 20 functions and both `.data` sections score 100% in objdiff 3.8.2 on the first build. With both objects `Matching`, `ninja` exits 0 and `tools/verify/verify.py verify` passes.

The first six functions of the block, `0x80333C74`-`0x803340D0`, are `port_release_port`, `port_find_mcb_dlci_port`, `port_find_dlci_port`, `port_find_port`, `port_flow_control_user` and `port_flow_control_peer`, by the `rfc_port_closed DONE` and `PORT_DataInd Data reached HW. Sending FC set.` diagnostics. They are the tail of `port_utils.c`, whose first function, `port_allocate_port` at `0x80333AF8`, lies in #529's block, so #529 owns that split. They are not handled here.

## Provenance

Sources and the declarations added to `include/revolution/bte/` are rewritten for this project after Petari's BTE files. Those files carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE SDP provenance](../licenses/534-bte-sdp-NOTICE.md), which also covers these files.

## Traps

- **A helper can be inlined in one caller and called in another.** `rfc_save_lcid_mcb` is inlined into `RFCOMM_ConnectInd` but called out of line from `rfc_mx_fsm.c`, so its copy survives at the end of `rfc_l2cap_if.c`. A function present in the binary is not proof that no caller inlines it. Evidence: F-530-1.
