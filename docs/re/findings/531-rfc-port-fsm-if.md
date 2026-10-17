# Issue 531: BTE RFCOMM port state machine and interface units

## F-531-1: Unit boundaries and source for `rfc_port_fsm.c` and `rfc_port_if.c`

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/rfc_port_fsm.c` and `rfc_port_if.c`.

| File | `.text` | Data | Present functions |
| --- | --- | --- | --- |
| `rfc_port_fsm.c` | `0x80335648`-`0x803367E8` | `.data` `0x803E5150`-`0x803E5476` (diagnostics and the six state jump tables) | `rfc_port_sm_execute`, `rfc_port_sm_state_closed`, `rfc_port_sm_sabme_wait_ua`, `rfc_port_sm_term_wait_sec_check`, `rfc_port_sm_orig_wait_sec_check`, `rfc_port_sm_opened`, `rfc_port_sm_disc_wait_ua`, `rfc_process_pn`, `rfc_process_rpn`, `rfc_process_msc`, `rfc_process_rls`, `rfc_process_nsc`, `rfc_process_test_rsp`, `rfc_process_fcon`, `rfc_process_fcoff`, `rfc_process_l2cap_congestion`, `rfc_set_port_state` |
| `rfc_port_if.c` | `0x803367E8`-`0x80336D14` | `.bss` `0x804AE628`-`0x804AEA40` (`rfc_cb`) | `RFCOMM_StartRsp`, `RFCOMM_DlcEstablishReq`, `RFCOMM_DlcEstablishRsp`, `RFCOMM_ParNegReq`, `RFCOMM_ParNegRsp`, `RFCOMM_PortNegReq`, `RFCOMM_PortNegRsp`, `RFCOMM_ControlReq`, `RFCOMM_FlowReq`, `RFCOMM_LineStatusReq`, `RFCOMM_DlcReleaseReq`, `RFCOMM_DataReq` |

Each state's diagnostics are followed by its jump table, as CodeWarrior emits them. That fixes the `.data` start at the `NULL port event %d` string, `0x803E5150`. The bytes just before it (`rfc_mx_conf_ind p_cfg:%0x`) belong to `rfc_mx_fsm.c`. `rfc_port_uplink_data` is inlined. `RFCOMM_StartReq` was dead-stripped. The first function of `rfc_port_if.c` passes `&result` with event 7 (`RFC_MX_EVENT_START_RSP`), so it is `RFCOMM_StartRsp`.

The external callees were named from the paired call sequences of the compiled and target objects:

| Address | Name |
| --- | --- |
| `0x803224D0` | `btm_sec_mx_access_request` |
| `0x803228F4` | `btm_sec_abort_access_req` |
| `0x803328F8` | `PORT_ParNegInd` |
| `0x80332AA8` | `PORT_ParNegCnf` |
| `0x80332BAC` | `PORT_DlcEstablishInd` |
| `0x80332CEC` | `PORT_DlcEstablishCnf` |
| `0x80332E30` | `PORT_PortNegInd` |
| `0x80332F80` | `PORT_PortNegCnf` |
| `0x80333098` | `PORT_ControlInd` |
| `0x80333210` | `PORT_ControlCnf` |
| `0x803332D4` | `PORT_LineStatusInd` |
| `0x8033339C` | `PORT_DlcReleaseInd` |
| `0x803334A4` | `Port_TimeOutCloseMux` |
| `0x8033353C` | `PORT_DataInd` |
| `0x80333D50` | `port_find_mcb_dlci_port` |

The units are compiled with GC/3.0a3 (`BTE_COMPILER`, F-534-2) and the library `cflags_sdk`. All 29 functions, `.data` and `.bss` score 100% in objdiff 3.8.2 on the first build. With both objects `Matching`, `ninja` exits 0 and `tools/verify/verify.py verify` passes.

`rfc_port_if.c` also covers `0x80336AB0`-`0x80336D14`, the six functions #532 left for this issue (F-532-1). With this split, every function of #532 is in a matching unit.

The first two functions of the block, `0x80335464` and `0x80335558`, are `rfc_mx_conf_cnf` and `rfc_mx_conf_ind`. They are the tail of `rfc_mx_fsm.c`, which starts in #530's block, so they are left for #530.

## Provenance

Sources and the declarations added to `include/revolution/bte/` are rewritten for this project after Petari's BTE files. Those files carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE SDP provenance](../licenses/534-bte-sdp-NOTICE.md), which also covers these files.

## Traps

- **A dead-stripped neighbour can shift a name by one.** Petari's first `rfc_port_if.c` function is `RFCOMM_StartReq`, but the target keeps only `RFCOMM_StartRsp`, which has the same size. Naming by position gave a 21% objdiff score. Check the event constant and argument use before naming by source order. Evidence: F-531-1.
