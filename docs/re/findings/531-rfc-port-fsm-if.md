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

The first two functions of the block, `0x80335464` and `0x80335558`, are `rfc_mx_conf_cnf` and `rfc_mx_conf_ind`. They are the tail of `rfc_mx_fsm.c`, which starts in #530's block, so F-531-1 left them for #530. They were subsequently supplied by PR #651; see the combined audit in F-531-2.

## Provenance

Sources and the declarations added to `include/revolution/bte/` are rewritten for this project after Petari's BTE files. Those files carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE SDP provenance](../licenses/534-bte-sdp-NOTICE.md), which also covers these files.

## Traps

- **A dead-stripped neighbour can shift a name by one.** Petari's first `rfc_port_if.c` function is `RFCOMM_StartReq`, but the target keeps only `RFCOMM_StartRsp`, which has the same size. Naming by position gave a 21% objdiff score. Check the event constant and argument use before naming by source order. Evidence: F-531-1.

## F-531-2: Combined source coverage audit

PR #645 (`359b05e`) supplied `rfc_port_fsm.c` and `rfc_port_if.c`. PR #651 (`d18d33b`, #530) later supplied the complete `rfc_mx_fsm.c`, including the two outstanding functions listed in #531. All 25 issue functions and 5,708 code bytes now have C implementations in units registered `Matching`. The source-object symbol sizes equal the individual issue extents. This audit adds no reconstructed code bytes and retains attribution of neighboring functions to #530 and #532.

The existing compiler selection is GC/3.0a3 (`BTE_COMPILER`) with `cflags_sdk`, superseding the survey issue's generic Wii/1.0 statement. The three complete units contain 39 retained functions. Their source, headers, splits, compiler settings, and existing matching status are unchanged by this audit.

| Address | Size | Recovered name | Source unit |
| --- | --- | --- | --- |
| `0x80335464` | `0xF4` | `rfc_mx_conf_cnf` | `rfc_mx_fsm.c` |
| `0x80335558` | `0xF0` | `rfc_mx_conf_ind` | `rfc_mx_fsm.c` |
| `0x80335648` | `0x90` | `rfc_port_sm_execute` | `rfc_port_fsm.c` |
| `0x803356D8` | `0x174` | `rfc_port_sm_state_closed` | `rfc_port_fsm.c` |
| `0x8033584C` | `0x19C` | `rfc_port_sm_sabme_wait_ua` | `rfc_port_fsm.c` |
| `0x803359E8` | `0x1BC` | `rfc_port_sm_term_wait_sec_check` | `rfc_port_fsm.c` |
| `0x80335BA4` | `0x150` | `rfc_port_sm_orig_wait_sec_check` | `rfc_port_fsm.c` |
| `0x80335CF4` | `0x1EC` | `rfc_port_sm_opened` | `rfc_port_fsm.c` |
| `0x80335EE0` | `0x11C` | `rfc_port_sm_disc_wait_ua` | `rfc_port_fsm.c` |
| `0x80335FFC` | `0xE8` | `rfc_process_pn` | `rfc_port_fsm.c` |
| `0x803360E4` | `0x30C` | `rfc_process_rpn` | `rfc_port_fsm.c` |
| `0x803363F0` | `0x16C` | `rfc_process_msc` | `rfc_port_fsm.c` |
| `0x8033655C` | `0x8C` | `rfc_process_rls` | `rfc_port_fsm.c` |
| `0x803365E8` | `0x4` | `rfc_process_nsc` | `rfc_port_fsm.c` |
| `0x803365EC` | `0x8` | `rfc_process_test_rsp` | `rfc_port_fsm.c` |
| `0x803365F4` | `0x64` | `rfc_process_fcon` | `rfc_port_fsm.c` |
| `0x80336658` | `0x64` | `rfc_process_fcoff` | `rfc_port_fsm.c` |
| `0x803366BC` | `0x88` | `rfc_process_l2cap_congestion` | `rfc_port_fsm.c` |
| `0x80336744` | `0xA4` | `rfc_set_port_state` | `rfc_port_fsm.c` |
| `0x803367E8` | `0x2C` | `RFCOMM_StartRsp` | `rfc_port_if.c` |
| `0x80336814` | `0x68` | `RFCOMM_DlcEstablishReq` | `rfc_port_if.c` |
| `0x8033687C` | `0x70` | `RFCOMM_DlcEstablishRsp` | `rfc_port_if.c` |
| `0x803368EC` | `0xE0` | `RFCOMM_ParNegReq` | `rfc_port_if.c` |
| `0x803369CC` | `0x28` | `RFCOMM_ParNegRsp` | `rfc_port_if.c` |
| `0x803369F4` | `0xBC` | `RFCOMM_PortNegReq` | `rfc_port_if.c` |

### Diagnostic symbol evidence

The target data symbols combined security-state diagnostics in `rfc_port_fsm.c`. The termination-state ignored-event string starts at `0x803E5278` and occupies 48 bytes, following the 44-byte aligned error string at `0x803E524C`. The originating-state ignored-event string starts at `0x803E5318` and occupies 48 bytes, following the 48-byte aligned error string at `0x803E52E8`. These boundaries follow the source literals, target bytes, and identical instruction references. The new symbol names describe the messages; they do not assert original variable names.

Before separating these diagnostics, two identical `addi` instructions received differing synthetic `R_PPC_NONE` targets/addends and the unit text scored 99.991135%. The complete source-linked executable already passed the frozen comparison. The symbol correction addresses analysis metadata without changing code, data bytes, or comparison settings. An equivalent discrepancy in #530's earlier multiplexer-state diagnostics is recorded separately in a prepared scope-extension patch; that patch remains unapplied pending approval.

### Validation status

`python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, and `.venv/Scripts/ninja` exit 0. Initial source-linked `main.dol` matches the project's SHA-1 link check. The audit checks all 25 issue address/size pairs against compiled source objects and records full-unit objdiff results. Final frozen-manifest verification and rebased acceptance follow before merging. Scope is the main game executable only, excluding boot content 6, data contents, and WAD packaging.

### Traps from the combined audit

- **Completed neighboring units do not close a previously partial issue automatically.** The two remaining #531 functions arrived in #530's whole-unit merge. Reconcile the original address inventory with compiled source sizes and full executable verification before accepting the block. No new reconstruction bytes should be counted for an acceptance audit.
- **Grouped diagnostic symbols affect synthetic relocations.** The two security-state diagnostic pairs produced objdiff discrepancies in otherwise identical instructions. The literal boundaries and complete executable comparison identify the metadata issue; keep default comparison settings and record the individual boundaries.
