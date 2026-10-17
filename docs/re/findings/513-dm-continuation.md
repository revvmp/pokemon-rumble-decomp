# F-513-1: Device-manager continuation acceptance inventory

Issue #513 owns 29 functions and 4,160 code bytes at `0x8031590C`-`0x8031694C`. Every function lies inside `bta_dm_act.c`, whose first function belongs to #512. The source delivery is [PR #679](https://github.com/revvmp/pokemon-rumble-decomp/pull/679); the complete source and unit boundaries are documented in its F-512-5. This issue audit adds no reconstructed source bytes and does not create another unit or duplicate its ledger row.

The preliminary inventory audit uses the complete source objects from PR #679's verified clean build. Each listed source function has the exact original issue address and size. Default objdiff 3.8.2 reports 100% for every section of the complete action unit. These checks establish coverage in the source delivery; acceptance against current `main` remains pending until that delivery merges and the audit branch completes its own full build and frozen verification.

| Address | Size | Recovered name |
| --- | --- | --- |
| `0x8031590C` | `0x48` | `bta_dm_queue_disc` |
| `0x80315954` | `0x44` | `bta_dm_search_clear_queue` |
| `0x80315998` | `0x44` | `bta_dm_search_cancel_cmpl` |
| `0x803159DC` | `0x5C` | `bta_dm_search_cancel_transac_cmpl` |
| `0x80315A38` | `0x18` | `bta_dm_search_cancel_notify` |
| `0x80315A50` | `0x1A0` | `bta_dm_find_services` |
| `0x80315BF0` | `0xE0` | `bta_dm_discover_next_device` |
| `0x80315CD0` | `0x48` | `bta_dm_sdp_callback` |
| `0x80315D18` | `0x90` | `bta_dm_inq_results_cb` |
| `0x80315DA8` | `0x4C` | `bta_dm_inq_cmpl_cb` |
| `0x80315DF4` | `0x40` | `bta_dm_service_search_remname_cback` |
| `0x80315E34` | `0x8C` | `bta_dm_remname_cback` |
| `0x80315EC0` | `0x88` | `bta_dm_disc_remname_cback` |
| `0x80315F48` | `0x4` | `bta_dm_cancel_rmt_name` |
| `0x80315F4C` | `0xC8` | `bta_dm_authorize_cback` |
| `0x80316014` | `0xC0` | `bta_dm_pinname_cback` |
| `0x803160D4` | `0x130` | `bta_dm_pin_cback` |
| `0x80316204` | `0x8` | `bta_dm_link_key_request_cback` |
| `0x8031620C` | `0x9C` | `bta_dm_new_link_key_cback` |
| `0x803162A8` | `0x84` | `bta_dm_authentication_complete_cback` |
| `0x8031632C` | `0x28` | `bta_dm_local_addr_cback` |
| `0x80316354` | `0x3C` | `bta_dm_signal_strength` |
| `0x80316390` | `0xF4` | `bta_dm_signal_strength_timer_cback` |
| `0x80316484` | `0x70` | `bta_dm_acl_change_cback` |
| `0x803164F4` | `0x280` | `bta_dm_acl_change` |
| `0x80316774` | `0x1C` | `bta_dm_disable_conn_down_timer_cback` |
| `0x80316790` | `0x6C` | `bta_dm_rssi_cback` |
| `0x803167FC` | `0x6C` | `bta_dm_link_quality_cback` |
| `0x80316868` | `0xE4` | `bta_dm_l2cap_server_compress_cback` |

Names come from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, exact instruction correspondence, source-file order, and final call/callback relocations. The compiler is the measured GC/3.0a3 `BTE_COMPILER` with `cflags_sdk`, superseding the survey issue's generic Wii/1.0 description. Source and declaration provenance remain with the delivering unit, the existing [BTE notice](../licenses/510-bte-NOTICE.md#issue-512-additions), and the [full Apache-2.0 license](../licenses/510-apache-2.0.txt).

## Acceptance status

Pending: merge PR #679, rebase this audit onto current `main`, compile the real registered-source and full build targets, repeat the original/source whole-object comparison and 29-function inventory, and run the frozen verifier on the actual rebuilt output. Scope is the main game executable only, excluding boot content 6, data contents, and WAD packaging. Final commands and verified head belong in the audit pull request.

## Traps

No new reverse-engineering trap emerged from this inventory audit. Whole-file delivery can complete a neighboring block's source without updating that block's issue status; verify its original address inventory and full source-selected executable before accepting it, and count no duplicate reconstruction bytes.
