# Issue 529: BTE RFCOMM port utility unit

## F-529-1: Unit boundaries and source for `port_utils.c`

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/port_utils.c`.

| File | `.text` | `.data` | Present functions |
| --- | --- | --- | --- |
| `port_utils.c` | `0x80333AF8`-`0x803340D0` | `0x803E4C10`-`0x803E4D56` (MTU, close and flow-control diagnostics) | `port_select_mtu`, `port_release_port`, `port_find_mcb_dlci_port`, `port_find_dlci_port`, `port_flow_control_user`, `port_get_signal_changes`, `port_flow_control_peer` |

The names follow the diagnostics (`port_select_mtu bad packet size`, `rfc_port_closed DONE`, `PORT_DataInd Data reached HW. Sending FC set.`) and the bluedroid order, which the target keeps. `port_flow_control_user` returns `PORT_EV_FC` or `PORT_EV_FC | PORT_EV_FCS` after comparing the transmit queue against 8000 bytes and 16 buffers; `port_get_signal_changes` masks the modem-signal events with `ev_mask`. `port_allocate_port`, `port_set_defaults`, `port_find_mcb` and `port_find_port` have no callers in the target and are dead-stripped, with the `.rodata` `default_port_pars`. The source keeps them.

The first function, `port_select_mtu`, starts the file: the preceding `port_rfc_closed` strings at `0x803E4BB4`-`0x803E4C0B` belong to `port_rfc.c`, and the `.data` split begins at the first `port_select_mtu` string. The constants match bluedroid's watermarks for this build: `PORT_RX_HIGH_WM` 8000, `PORT_RX_LOW_WM` 5000, `PORT_RX_CRITICAL_WM` 12000, `PORT_TX_HIGH_WM` 8000, `PORT_RX_BUF_LOW_WM` 8, `PORT_RX_BUF_HIGH_WM` 16, `PORT_TX_BUF_HIGH_WM` 16, and `L2CAP_PKT_OVERHEAD` 4.

`btm_get_max_packet_size` (`0x8031B480`) is named from its only call in `port_select_mtu` on `p_port->bd_addr`.

The unit is compiled with GC/3.0a3 (`BTE_COMPILER`, F-534-2) and the library `cflags_sdk`. All seven functions and `.data` score 100% in objdiff 3.8.2 on the first build. With the object `Matching`, `ninja` exits 0 and `tools/verify/verify.py verify` passes.

This unit covers the six functions `0x80333C74`-`0x803340D0` that #530 left for the `port_utils.c` split (F-530-1), so every function of #530 is now in a matching unit.

The other sixteen functions of this block, `0x803328F8`-`0x80333AF8`, are `PORT_ParNegInd` through `port_rfc_closed` and the tail of `port_rfc.c`. That file starts with `PORT_StartCnf` at `0x803326FC`, after `RFCOMM_Init` (`0x803326AC`, the end of `port_api.c`), in #528's block, so #528 owns its split. They are not handled here.

## Provenance

The source and the declarations added to `include/revolution/bte/` are rewritten for this project after Petari's BTE files. Those files carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE SDP provenance](../licenses/534-bte-sdp-NOTICE.md), which also covers this file.

## Traps

- **Dead-stripped functions can sit at the start of a file.** `port_utils.c` begins in the binary with `port_select_mtu`, its third bluedroid function, because `port_allocate_port` and `port_set_defaults` are stripped. Locate a file's start from the first surviving function and its first string, not from the reference's first function. Evidence: F-529-1.
