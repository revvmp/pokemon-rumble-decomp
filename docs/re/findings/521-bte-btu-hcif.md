# Issue 521: BTE security manager tail and HCI event interface

Covers issue #521 (`0x803224D0`–`0x803255F4`) and the two `btu_hcif.c` functions left in #522 (`0x803255F4`–`0x8032582C`). Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/`.

## F-521-1: Unit boundaries

| Range | Unit | Owner | Evidence |
| --- | --- | --- | --- |
| `0x803224D0`–`0x803245D0` | `btm_sec.c` (tail) | #520 | 22 functions in Petari's `btm_sec.c` order, from `btm_sec_mx_access_request` (`Security Manager: MX service not found`) to `btm_read_trusted_mask`. The file starts in #520: `BTM_SetSecurityLevel` at `0x803217D8` already logs `BTM_Sec: application registered` (F-524-4). #520 owns the split, so these functions are not split or reconstructed here. String `btm_sec_collision_timeout()` at `.data` `0x803E2904` belongs to this file. |
| `0x803245D0`–`0x8032582C` | `btu_hcif.c` | #521 | `0x803245D0` reads the event code and length from the message, rejects every event except 14 and 15 while `btm_cb.devcb.state` is 0, and dispatches on the HCI event code, logging `Ctlr H/w error event` for event 16. `0x8032459C` before it returns `btm_find_dev` result plus 0x10, Petari's `btm_read_trusted_mask`, the last surviving `btm_sec.c` function. The unit ends where `btu_init.c` starts (F-522-1). Strings `Ctlr H/w error event`, `Event mismatch opcode=%X cmd opcode=%X`, `Cmd timeout; no cmd in queue` and `BTU HCI command timeout - cmd opcode = 0x%02x` fill `.data` `0x803E2920`–`0x803E29AE`, padded to `0x803E29B0` where `gap_conn.c` data starts. |

`btu_hcif.c` starts in #521 and runs 0x234 bytes into #522, so #521 owns its whole split, including `btu_hcif_cmd_timeout` (`0x803255F4`) and `btu_hcif_link_key_notification_evt` (`0x8032574C`) that F-522-1 left unsplit.

The 12 surviving functions follow Petari's source order: `btu_hcif_process_event`, `btu_hcif_send_cmd`, `btu_hcif_connection_comp_evt`, `btu_hcif_connection_request_evt`, `btu_hcif_qos_setup_comp_evt`, `btu_hcif_esco_connection_comp_evt`, `btu_hcif_hdl_command_complete`, `btu_hcif_command_complete_evt`, `btu_hcif_hdl_command_status`, `btu_hcif_command_status_evt`, `btu_hcif_cmd_timeout`, `btu_hcif_link_key_notification_evt`.

## F-521-2: Reconstruction and compiler

`btu_hcif.c` matches with GC/3.0a3 and `cflags_sdk` (`BTE_COMPILER`, F-524-2, F-522-2): all 12 functions are 100% in objdiff, all 144 bytes of `.data` match, and the full DOL verifies with the unit linked from source.

- `btu_hcif_store_cmd` is inlined into `btu_hcif_send_cmd`; 28 of the 35 event handlers are inlined into `btu_hcif_process_event`. The other seven stay out of line, matching the target. Empty handlers are kept so the dispatch matches Petari's `switch`.
- `btu_hcif_send_host_rdy_for_data` has no code in the target and is not reconstructed.
- The HCI traces are unconditional, with no trace-level check: `LogMsg_0` with `0x70000` (`TRACE_LAYER_HCI`, error) and `0x70001` (warning).
- The target keeps the `HCI_QOS_SETUP_COMP_EVT` (13) case labels that Petari uses in `btu_hcif_hdl_command_status` and `btu_hcif_cmd_timeout` (`cmpwi 0xD`), where an opcode would be expected.
- Event 0xEF is dispatched to `btm_vendor_specific_evt` together with 0xFF; it is named `HCI_BRCM_VENDOR_SPECIFIC_EVT` (descriptive, medium confidence). Opcode 0xFC4C, the only completion `btu_hcif_command_complete_evt` re-queues on mismatch, is named `HCI_BRCM_WRITE_RAM` after the Broadcom patch-RAM vendor command (medium confidence).

## F-521-3: External names

To link `btu_hcif.c` from source, these outside symbols were given Petari names in `symbols.txt`. Each is the callee at the matching relocation of a 100% function, and each target call passes the arguments the Petari call does. Splits, sizes and types stay with the owning issues.

| Address | Name | Call site evidence |
| --- | --- | --- |
| `0x803116D0` | `GKI_enqueue_head` | `btu_hcif_command_complete_evt`, re-queues the command for opcode 0xFC4C |
| `0x803119B0` | `GKI_queue_is_empty` | Before each `btu_start_timer`/`btu_stop_timer` pair on `btu_cb.cmd_cmpl_q` |
| `0x8031A878` | `btm_acl_encrypt_change` | Event 8, before `btm_sec_encrypt_change` with the same arguments |
| `0x8031ABE8` | `btm_read_link_policy_complete` | Opcode 0x080C completion |
| `0x8031ACEC` | `btm_read_remote_version_complete` | Event 12 |
| `0x8031AD78` | `btm_read_remote_features_complete` | Event 11 |
| `0x8031B194` | `btm_process_clk_off_comp_evt` | Event 28, before `btm_sec_update_clock_offset` |
| `0x8031B230` | `btm_acl_role_changed` | Event 18; `HCI_SWITCH_ROLE` failure with role 0xFF |
| `0x8031B664` | `btm_qos_setup_complete` | `btu_hcif_qos_setup_comp_evt`; status failure with a null flow |
| `0x8031B9B4` | `btm_read_rssi_complete` | Opcode 0x1405 completion |
| `0x8031BAD0` | `btm_read_link_quality_complete` | Opcode 0x1403 completion |
| `0x8031C6E0` | `BTM_DeviceReset` | Event 16, called with null after `BTM_IsDeviceUp` |
| `0x8031C824` | `BTM_IsDeviceUp` | Event 16 |
| `0x8031CBC8` | `btm_reset_complete` | Opcode 0x0C03 completion |
| `0x8031CD8C` | `btm_read_hci_buf_size_complete` | Opcode 0x1005 completion |
| `0x8031CE54` | `btm_read_local_version_complete` | Opcode 0x1001 completion |
| `0x8031CF10` | `btm_read_local_features_complete` | Opcode 0x1003 completion |
| `0x8031D4C4` | `btm_read_local_name_complete` | Opcode 0x0C14 completion |
| `0x8031D590` | `btm_read_local_addr_complete` | Opcode 0x1009 completion |
| `0x8031D7D8` | `btm_vsc_complete` | Vendor-specific (0xFC00 group) completion and status |
| `0x8031D87C` | `btm_vendor_specific_evt` | Events 0xEF and 0xFF |
| `0x8031DC2C` | `btm_read_stored_link_key_complete` | Opcode 0x0C0D completion |
| `0x8031DCA8` | `btm_write_stored_link_key_complete` | Opcode 0x0C11 completion |
| `0x8031DD04` | `btm_delete_stored_link_key_complete` | Opcode 0x0C12 completion |
| `0x8031DD6C` | `btm_return_link_keys_evt` | Event 21, after writing event code 1 before the key count |
| `0x8031DEF8` | `btm_report_device_status` | `btu_hcif_cmd_timeout`, status 2 |
| `0x8031F260` | `btm_event_filter_complete` | Opcode 0x0C05 completion |
| `0x8031F480` | `btm_process_inq_results` | Events 2 (mode 0) and 34 (mode 1) |
| `0x8031F6F8` | `btm_process_inq_complete` | Event 1; opcode 0x0402 completion with 0; `HCI_INQUIRY` status failure |
| `0x8031F94C` | `btm_process_remote_name` | Event 7 with length minus 7; `HCI_RMT_NAME_REQUEST` failure |
| `0x803204FC` | `btm_pm_proc_cmd_status` | Status of opcodes 0x0801–0x0806 |
| `0x803205AC` | `btm_pm_proc_mode_change` | Event 20, before `hidd_pm_proc_mode_change` |
| `0x803209F4` | `btm_sco_chk_pend_unpark` | Event 20 |
| `0x80320B80` | `btm_sco_conn_req` | `btu_hcif_connection_request_evt`, non-ACL link |
| `0x80320D60` | `btm_sco_connected` | `btu_hcif_connection_comp_evt`, `btu_hcif_esco_connection_comp_evt`, eSCO setup failure |
| `0x8032103C` | `btm_sco_removed` | Event 5 when `l2c_link_hci_disc_comp` returns false |
| `0x803213B0` | `btm_esco_proc_conn_chg` | Event 45; eSCO setup failure on an active SCO link |
| `0x803214B4` | `btm_is_sco_active` | `HCI_SETUP_ESCO_CONNECTION` status failure |
| `0x8032276C` | `btm_sec_conn_req` | `btu_hcif_connection_request_evt`, ACL link; logs `Security Manager: connect request from n...` |
| `0x803229A8` | `btm_sec_rmt_name_request_complete` | Event 7; logs `Security Manager: rmt_name_complete stat...` |
| `0x80322C90` | `btm_sec_auth_complete` | Event 6; logs `Security Manager: auth_complete status:` |
| `0x80322FD4` | `btm_sec_mkey_comp_event` | Event 10; logs `Security Manager: mkey comp status:%d` |
| `0x803230D0` | `btm_sec_encrypt_change` | Event 8; logs `Security Manager: encrypt_change status:` |
| `0x8032324C` | `btm_sec_connected` | `btu_hcif_connection_comp_evt`; create-connection failure with handle 0xFFFF; logs `Security Manager: btm_sec_connected hand...` |
| `0x80323788` | `btm_sec_disconnected` | Event 5 |
| `0x803238C4` | `btm_sec_link_key_notification` | `btu_hcif_link_key_notification_evt`; logs `btm_sec_link_key_notification()  BDA:` |
| `0x80323A70` | `btm_sec_link_key_request` | Event 23; logs `btm_sec_link_key_request()  BDA:` |
| `0x80323C14` | `btm_sec_pin_code_request` | Event 22; logs `btm_sec_pin_code_request bonding sending` |
| `0x80323F70` | `btm_sec_update_clock_offset` | Event 28 |
| `0x8032E780` | `l2c_link_hci_conn_comp` | ACL connection complete and create-connection failure |
| `0x8032EA6C` | `l2c_link_hci_disc_comp` | Event 5 |
| `0x8032EB28` | `l2c_link_hci_qos_violation` | Event 30 |
| `0x8032F278` | `l2c_link_process_num_completed_pkts` | Event 19 |
| `0x8032F5AC` | `l2c_link_role_changed` | Event 18 with status 0 |
| `0x8032F62C` | `l2c_link_role_change_failed` | `HCI_SWITCH_ROLE` status failure |
| `0x8032F774` | `l2c_pin_code_request` | Event 22, before `btm_sec_pin_code_request` |
| `0x804AAD78` | `btm_cb` (`.bss`, 0x27C8 bytes) | `btm_cb.devcb.state` read at offset 0x64E in `btu_hcif_process_event` and `btu_hcif_send_cmd`, matching the existing `tBTM_CB` declaration |

## F-521-4: Remaining btm_sec.c functions in the #521 block

The 22 `btm_sec.c` functions in `0x803224D0`–`0x803245D0` are left to the owner of the `btm_sec.c` split (#520). Eleven of them are named in F-521-3. The order matches Petari's `btm_sec.c` with `btm_sec_change_link_key` absent, which gives these names for the rest, with medium confidence except where a string confirms it:

| Address | Size | Proposed name | Evidence |
| --- | --- | --- | --- |
| `0x803224D0` | `0x29C` | `btm_sec_mx_access_request` | String `Security Manager: MX service not found`, high |
| `0x80322898` | `0x1C` | `btm_sec_init` | Order |
| `0x803228B4` | `0x40` | `btm_sec_dev_reset` | Order |
| `0x803228F4` | `0xB4` | `btm_sec_abort_access_req` | Order |
| `0x8032320C` | `0x40` | `btm_sec_is_bonding` | Order |
| `0x803236CC` | `0xBC` | `btm_sec_disconnect` | Order |
| `0x80323BA0` | `0x74` | `btm_sec_pin_code_request_timeout` | String `btm_sec_pin_code_request_timeout()`, high |
| `0x80323FC0` | `0x3E0` | `btm_sec_execute_procedure` | Order; called by `btm_sec_collision_timeout` with `btm_cb.p_collided_dev_rec` |
| `0x803243A0` | `0x11C` | `btm_sec_start_authorization` | Order |
| `0x803244BC` | `0xE0` | `btm_sec_collision_timeout` | String `btm_sec_collision_timeout()`, high |
| `0x8032459C` | `0x34` | `btm_read_trusted_mask` | Returns `btm_find_dev` result plus 0x10, high |

#521 therefore cannot close until #520 splits and matches `btm_sec.c`. Superseded by [F-521-5](#f-521-5-btm_secc-matching).

## F-521-5: btm_sec.c Matching

`btm_sec.c` is split as `.text` `0x8032160C`-`0x803245D0`, `.data` `0x803E1F10`-`0x803E2920` and `.sdata2` `0x804BB628`-`0x804BB62E`, registered `Matching` with GC/3.0a3 (`BTE_COMPILER`, `cflags_sdk`), and linked from source. All 31 surviving functions are 100% in objdiff, as are `.data` and `.sdata2`, and the full DOL verifies. This supersedes the hand-off in [F-521-4](#f-521-4-remaining-btm_secc-functions-in-the-521-block): the 22 functions in `0x803224D0`-`0x803245D0` are reconstructed, so the #521 block is fully covered by `Matching` units.

- **Start.** The file begins at `BTM_SecRegister` (`0x8032160C`), the first function that logs `BTM_Sec: application registered` and stores the 0x18-byte `btm_cb.api`. The two functions before it, `0x80321524` and `btm_is_sco_active_by_bdaddr`, end `btm_sco.c`. Nine `btm_sec.c` functions (`BTM_SecRegister` to `btm_sec_l2cap_access_req`, `0x8032160C`-`0x803224D0`) therefore lie in the #520 block, which owns the split; this change takes the split for both blocks.
- **Source.** Petari's `btm_sec.c` compiles to the target with its statement order unchanged. Its trace macros become explicit `BTM_TRACE_<level><n>` macros that test `btm_cb.trace_level` (offset `0x27C0`) and log with layer `0x000D0000`, as the target's `cmplwi`/`LogMsg_n` pairs show.
- **Dead-stripped functions stay in the source.** `.data` is one string pool in source order. It still holds the strings of `BTM_SetSecurityMode`, `BTM_SetEncryption` and `btm_restore_mode`, which have no code in the target, so the whole Petari file is kept and the linker strips the unused functions. `btm_sec_change_link_key`, the `btm_sec_start_*` helpers other than `btm_sec_start_authorization`, and the `btm_sec_find_*` lookups have no out-of-line copy.
- **String symbols.** The original `.data` symbols grouped several strings (`lbl_803E1F10` 0x98 bytes, `lbl_803E1FA8`, `lbl_803E2110`, `lbl_803E220C` and `lbl_803E26A8`). objdiff then gave the string loads in six functions synthetic relocations to the wrong string and scored them 99.6-99.9% with identical instruction words. Splitting `symbols.txt` at the 50 string starts of the compiled pool, and merging the six one-byte `.sdata2` symbols into the 6-byte `0xFF` address initializer of `btm_sec_mkey_comp_event`, restores 100% without changing any byte.

| Address | Size | Name | Evidence |
| --- | --- | --- | --- |
| `0x80321728` | `0xB0` | `BTM_SetPinType` | String `BTM_SetPinType: pin type %d`, high |
| `0x80322898` | `0x1C` | `btm_sec_init` | 100% match of Petari's function in source order, high |
| `0x803228B4` | `0x40` | `btm_sec_dev_reset` | Same, high |
| `0x80323BA0` | `0x74` | `btm_sec_pin_code_request_timeout` | String, high |
| `0x80323FC0` | `0x3E0` | `btm_sec_execute_procedure` (static) | String `btm_sec_execute_procedure: Required`, high |
| `0x803243A0` | `0x11C` | `btm_sec_start_authorization` (static) | String `Start authorization`, high |
| `0x803244BC` | `0xE0` | `btm_sec_collision_timeout` (static) | String, high |

Outside names, from the call sites in the 100% functions: `btm_sec_alloc_dev` `0x8031C198` (`BTM_SecBond`, `btm_sec_l2cap_access_req`, `btm_sec_connected` after `btm_find_dev` fails), `btm_find_dev_by_handle` `0x8031C2B8` (handle lookups in `btm_sec_auth_complete` and seven other handlers), `BTM_DeleteStoredLinkKey` `0x8031DB5C` (`BTM_SecBond`) and `btm_initiate_rem_name` `0x8031F7FC` (`btm_sec_execute_procedure`, `btm_sec_start_authorization`). Their splits are left to the owning issues.

## Provenance

Sources and headers, including `btm_sec.c` (F-521-5), are rewritten after Petari's BTE files; see [BTE provenance](../licenses/521-bte-NOTICE.md).

## Traps

- **Git Bash `sed -i` drops CRLF.** Editing a CRLF header with `sed -i` in Git Bash rewrote it with LF endings. Use the agent's edit tool, or a script that reads and writes bytes, and check with `file` afterwards.
- **Compare-tree holes are not cases.** `btu_hcif_process_event` compares event 0x21 and `btu_hcif_hdl_command_complete` compares opcode 0x1404 and branches to the end; these are gaps inside a case range, not missing cases. Petari's `switch`, with no case for them, reproduces the tree.
- **Grouped string symbols lower objdiff scores of identical code.** `btm_sec.c` scored 99.6-99.9% in six functions whose instruction words all matched, because dtk had grouped several strings into one `.data` symbol. Split the symbols at the compiled object's string starts before looking for code differences; see [F-521-5](#f-521-5-btm_secc-matching).
- **Git Bash `sed -i` also drops CRLF on sources and `configure.py`.** Check with `file` and restore the endings, or edit with a byte-preserving script.
