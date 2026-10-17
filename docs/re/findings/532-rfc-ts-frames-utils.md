# Issue 532: BTE RFCOMM frame and utility units

## F-532-1: Unit boundaries and source for `rfc_ts_frames.c` and `rfc_utils.c`

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/rfc_ts_frames.c` and `rfc_utils.c`.

| File | `.text` | Data | Present functions |
| --- | --- | --- | --- |
| `rfc_ts_frames.c` | `0x80336D14`-`0x8033835C` | `.data` `0x803E5478`-`0x803E554E`, `.sdata` `0x804B8F30`-`0x804B8F3F` (frame diagnostics; `Bad UA` and `Bad DM` are short enough for `.sdata`) | `rfc_send_sabme`, `rfc_send_ua`, `rfc_send_dm`, `rfc_send_disc`, `rfc_send_buf_uih`, `rfc_send_pn`, `rfc_send_fcon`, `rfc_send_fcoff`, `rfc_send_msc`, `rfc_send_rls`, `rfc_send_rpn`, `rfc_send_test`, `rfc_send_credit`, `rfc_parse_data`, `rfc_process_mx_message` |
| `rfc_utils.c` | `0x8033835C`-`0x80338B3C` | `.rodata` `0x80398198`-`0x80398298` (`rfc_crctable`), `.data` `0x803E5550`-`0x803E55DA` (timer, close and credit diagnostics) | `rfc_calc_fcs`, `rfc_check_fcs`, `rfc_alloc_multiplexer_channel`, `rfc_release_multiplexer_channel`, `rfc_timer_start`, `rfc_timer_stop`, `rfc_port_timer_start`, `rfc_port_timer_stop`, `rfc_check_mcb_active`, `rfcomm_process_timeout`, `rfc_sec_check_complete`, `rfc_port_closed`, `rfc_inc_credit`, `rfc_dec_credit`, `rfc_check_send_cmd` |

The function names follow the diagnostics (`rfc_timer_start - timeout:%d`, `rfc_port_closed`, `rfc_inc_credit:%d`, `Bad SABME`, `Illegal MX Frame ea:%d len:%d`) and the bluedroid order, which the target keeps. `rfc_send_nsc` has no code of its own: its only caller, `rfc_process_mx_message`, inlines it, and the linker strips the out-of-line copy. The source keeps the function.

The callees outside these units were named from the paired call sequences of the compiled and target objects. The tables below list them. `rfc_cb` (`0x804AE628`, 0x418 bytes) is the RFCOMM control block, which `rfc_port_if.c` defines.

| Address | Name | Evidence |
| --- | --- | --- |
| `0x80310EB0` | `GKI_init_q` | Only call in `rfc_alloc_multiplexer_channel` on `&p_mcb->cmd_q` |
| `0x8033373C` | `PORT_FlowInd` | Called by `rfc_inc_credit` when `tx.peer_fc` is set |
| `0x80333964` | `port_rfc_closed` | Last call of `rfc_port_closed`, with `PORT_CLOSED` |
| `0x80334A04` | `rfc_mx_sm_execute` | Called with `RFC_MX_EVENT_CLOSE_REQ` and `RFC_EVENT_TIMEOUT` |
| `0x80335648` | `rfc_port_sm_execute` | Called with `RFC_EVENT_TIMEOUT` and `RFC_EVENT_SEC_COMPLETE` |
| `0x80335FFC` | `rfc_process_pn` | PN case of `rfc_process_mx_message` |
| `0x803360E4` | `rfc_process_rpn` | RPN case |
| `0x803363F0` | `rfc_process_msc` | MSC case |
| `0x8033655C` | `rfc_process_rls` | RLS case |
| `0x803365E8` | `rfc_process_nsc` | NSC case |
| `0x803365EC` | `rfc_process_test_rsp` | TEST response case |
| `0x803365F4` | `rfc_process_fcon` | FCON case |
| `0x80336658` | `rfc_process_fcoff` | FCOFF case |

The units are compiled with GC/3.0a3 (`BTE_COMPILER`, F-524-2 and F-534-2) and the library `cflags_sdk`. All 30 functions and every data section score 100% in objdiff 3.8.2. With both objects `Matching`, `ninja` exits 0 and `tools/verify/verify.py verify` passes.

The first six functions of the block, `0x80336AB0`-`0x80336D14`, belong to `rfc_port_if.c`. That file starts in #531's block and also defines `rfc_cb`, so #531 owns its split. These functions are not handled here.

## Provenance

Sources and the declarations added to `include/revolution/bte/` (`rfc_int.h`, `port_int.h`, `port_api.h`, `rfcdefs.h` and the RFCOMM constants, timer types and trace macros) are rewritten for this project after Petari's BTE files. Those files carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE SDP provenance](../licenses/534-bte-sdp-NOTICE.md), which also covers these files.

## Traps

- **A function missing from the binary can be inlined, not absent.** `rfc_send_nsc` has no address, because `rfc_process_mx_message` inlines its only call and the linker strips the copy. Keep such functions in the source; removing them changes the caller's code. Evidence: F-532-1.
- **Ending a data split before the padding can need a smaller symbol.** dtk rejected `.data` ending at `0x803E554E`: the split ended within `lbl_803E5478`, whose inferred size included the two padding bytes. Reduce that symbol's size in `symbols.txt` to end at the last string. Do not move the split end to the padded boundary, which leaves the `.data` comparison at 99.5%. Evidence: F-532-1.
