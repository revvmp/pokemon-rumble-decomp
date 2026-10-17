# Issue 528: BTE RFCOMM port API and port RFCOMM units

## F-528-1: Unit boundaries and source for `port_api.c` and `port_rfc.c`

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/port_api.c` and `port_rfc.c`.

| File | `.text` | `.data` | Present functions |
| --- | --- | --- | --- |
| `port_api.c` | `0x803326AC`-`0x803326FC` | None | `RFCOMM_Init` |
| `port_rfc.c` | `0x803326FC`-`0x80333AF8` | `0x803E48E0`-`0x803E4C0B` (string pool) | `PORT_StartCnf`, `PORT_StartInd`, `PORT_ParNegInd`, `PORT_ParNegCnf`, `PORT_DlcEstablishInd`, `PORT_DlcEstablishCnf`, `PORT_PortNegInd`, `PORT_PortNegCnf`, `PORT_ControlInd`, `PORT_ControlCnf`, `PORT_LineStatusInd`, `PORT_DlcReleaseInd`, `PORT_CloseInd`, `Port_TimeOutCloseMux`, `PORT_DataInd`, `PORT_FlowInd`, `port_rfc_send_tx_data`, `port_rfc_closed` |

`RFCOMM_Init` clears `rfc_cb` (0x418 bytes), sets `last_mux` to 1 and the trace level to 5, and calls `rfcomm_l2cap_if_init`. It is the only surviving function of `port_api.c`; the WPAD stack uses no public `PORT_` or `RFCOMM_CreateConnection` entry points, so the rest of the file is dead-stripped with its strings. The source keeps only `RFCOMM_Init`.

In `port_rfc.c`, `port_open_continue`, `port_start_control`, `port_start_par_neg` and `port_start_close` are dead-stripped, and `port_get_credits` is inlined into `PORT_ParNegInd` and `PORT_ParNegCnf`. The source keeps all five. The `.data` split begins with the `port_open_continue` strings although that function is absent: GC/3.0a3 emits a file's string literals as one pooled object addressed from a single base (`lbl_803E48E0`, used by `PORT_PortNegCnf`), so the pool keeps the strings of stripped functions. Removing `port_open_continue` from the source would shift every later string.

`port_rfc_send_tx_data` (`0x8033386C`) is named from its diagnostic `Sending RFCOMM_DataReq` and its calls from `PORT_ControlInd`, `PORT_ControlCnf` and `PORT_FlowInd`.

The units are compiled with GC/3.0a3 (`BTE_COMPILER`, F-534-2) and the library `cflags_sdk`. All 19 functions and `.data` score 100% in objdiff 3.8.2 on the first build. With both objects `Matching`, `ninja` exits 0 and `tools/verify/verify.py verify` passes.

`port_rfc.c` covers the sixteen functions `0x803328F8`-`0x80333AF8` that #529 left for the `port_rfc.c` split (F-529-1), so every function of #529 is now in a matching unit.

The first 22 functions of this block, `0x803311F8`-`0x803326AC`, are the tail of `l2c_utils.c`, by the `L2CAP - no buffer for ...` diagnostics and the bluedroid order (for example `0x80332534` walks the link control blocks by state, as `l2cu_find_lcb_by_state` does, and `0x803325C4` is `l2cu_lcb_disconnecting`). That file's `.data` starts at `0x803E47F8` (`L2CAP - no buffer cmd_rej`), and its first function lies in #527's block, so #527 owns the split. They are not handled here.

## Provenance

Sources and the declarations added to `include/revolution/bte/` are rewritten for this project after Petari's BTE files. Those files carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE SDP provenance](../licenses/534-bte-sdp-NOTICE.md), which also covers these files.

## Traps

- **A pooled string object keeps a stripped function's strings.** `port_rfc.c` addresses all its literals from one `.data` object, so `port_open_continue` must stay in the source even though its code is dead-stripped; otherwise the pool shrinks and every later string offset moves. Check for leading strings with no surviving user before dropping a stripped function. Evidence: F-528-1.
