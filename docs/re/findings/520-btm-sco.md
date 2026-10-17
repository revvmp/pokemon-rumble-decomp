# Issue 520: BTE SCO manager and the start of the security manager

Covers issue #520 (`0x803209F4`-`0x803224D0`) and the first two `btm_sco.c` functions in #519 (`0x803207D4`-`0x803209F4`). Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/btm_sco.c`.

## F-520-1: Unit boundaries

| Range | Unit | Owner | Evidence |
| --- | --- | --- | --- |
| `0x803207D4`-`0x8032160C` | `btm_sco.c` | #519 | `0x803207D4` clears `btm_cb.sco_cb` and is called from `btm_init`; `0x80320818` logs `btm_esco_conn_rsp -> No Resources`. `btm_pm_proc_mode_change` ends at `0x803207D4`. The file ends after `btm_is_sco_active_by_bdaddr`, before `BTM_SecRegister`. `.rodata` `0x80398150`-`0x80398160` is `btm_esco_defaults`; `.data` `0x803E1C10`-`0x803E1F10` holds the 13 SCO strings, padded to where `btm_sec.c` data starts. |
| `0x8032160C`-`0x803224D0` | `btm_sec.c` (start) | #520 | Split and `Matching` since [F-521-5](521-bte-btu-hcif.md#f-521-5-btm_secc-matching) (PR #687). |

`btm_sco.c` starts in #519, which owns its split; it is taken here with #519 also claimed, so this change covers the 15 surviving functions: the two in #519 and the 13 in #520.

## F-520-2: Reconstruction

`btm_sco.c` matches with GC/3.0a3 and `cflags_sdk` (`BTE_COMPILER`): all 15 functions, `.rodata` and `.data` are 100% in objdiff, and the full DOL verifies with the unit linked from source.

- Petari's source compiles to the target unchanged apart from the trace macros, which become `BTM_TRACE_<level><n>` macros on `btm_cb.trace_level`, as in `btm_sec.c`.
- `btm_send_connect_request` is inlined into `btm_sco_chk_pend_unpark`, and `BTM_CreateSco`, `BTM_SetEScoMode`, the `BTM_ReadSco*` accessors, `btm_accept_sco_link`, `btm_reject_sco_link`, `BTM_EScoConnRsp`, `btm_read_def_esco_mode` and `btm_is_any_sco_active` are dead-stripped. They stay in the source because the `.data` string pool keeps their strings in source order.
- The grouped dtk string symbols at `0x803E1C10` and `0x803E1D3C` were split at the compiled string starts, as in F-521-5; before that `BTM_ChangeEScoLinkParms` scored 99.9% with identical instruction words.

| Address | Size | Name | Evidence |
| --- | --- | --- | --- |
| `0x803207D4` | `0x44` | `btm_sco_init` | Clears `sco_cb`, called from `btm_init`; 100% match, high |
| `0x80320818` | `0x1DC` | `btm_esco_conn_rsp` (static) | String, high |
| `0x80320F00` | `0xB8` | `BTM_RemoveSco` | Source order and 100% match, high |
| `0x80321208` | `0x1A8` | `BTM_ChangeEScoLinkParms` | String `BTM_ChangeEScoLinkParms`, high |
| `0x80321524` | `0x60` | `btm_num_sco_links_active` | Source order and 100% match, high |
| `0x80398150` | `0x10` | `btm_esco_defaults` (static) | 64 kbit/s rates, latency 10, packet types `0x3F`, high |

Outside names from the call sites: `BTM_GetHCIConnHandle` `0x8031B0F8` (`btm_sco_chk_pend_unpark`, compared with the HCI handle) and `btm_chg_all_acl_pkt_types` `0x8031BCA4` (`TRUE` from `btm_esco_conn_rsp` and `btm_sco_chk_pend_unpark`, `FALSE` from `btm_sco_removed`). Their splits are left to the owning issues.

## Provenance

Sources and headers are rewritten after Petari's BTE files; see [BTE provenance](../licenses/520-bte-NOTICE.md).

## Traps

None new. The grouped-string-symbol trap of F-521-5 applies again; split the symbols before looking for code differences.
