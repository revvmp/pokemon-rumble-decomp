# F-516-1: HID-host utilities and BTM ACL reconstruction

Issue #516 owns 30 functions / 7,016 code bytes at `0x80319BD4`-`0x8031B73C`. Its first two functions (512 bytes) belong to the complete HID state-machine file in pending PR #694. This delivery adds the remaining 28 functions / 6,504 bytes and six #517 ACL functions / 1,928 bytes. The complete utility and ACL units contain 34 retained functions / 8,432 code bytes; no prior state-machine byte credit is duplicated.

The work is prepared on the exact pending PR #694 source branch, head `f2fd4e685a0693eb5c6c240f5ecf0795f8de7821`, so canonical HID declarations have one definition. The final merge requires that dependency on `main`, a fresh rebase, and the full exact-head acceptance checks. Local verification is the acceptance gate; GitHub CI does not run automatically.

Reference names, algorithms, layouts, and constants come from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, corresponding `src/RVL_SDK/bte/bta_hh_utils.c`, `btm_acl.c`, and SDK headers. The [file-specific notice](../licenses/516-bte-NOTICE.md) preserves Broadcom and muff1n1634 provenance and links the full Apache-2.0 license. Source uses canonical includes and existing typed logging calls, with explicit `UINT32` conversions for trace arguments. The BTM trace guard is `btm_cb.trace_level` and layer `TRACE_LAYER_BTM`; HID utilities use the application trace level and origin.

## Measured compiler and unit extents

`GC/3.0a3`, `BTE_COMPILER`, and `cflags_sdk` reproduce every retained instruction stream with a unique same-sized target match. This supersedes the issue survey's generic Wii/1.0 description. The target retains five utility functions and twenty-nine ACL functions. Original/source objdiff compares all retained code/data sections at 100%. A direct comparison of the complete source `.data` payloads also finds zero differing bytes: 698 utility bytes and 1,053 ACL bytes.

| Unit | Functions / code bytes | Exact split extents |
| --- | --- | --- |
| `bta_hh_utils.c` | 5 / 940 | `.text` `0x80319DD4`-`0x8031A180`; `.data` `0x803E1080`-`0x803E1340` |
| `btm_acl.c` | 29 / 7,492 | `.text` `0x8031A180`-`0x8031BEC4`; `.data` `0x803E1340`-`0x803E1760` |

Both compiled `.data` sections require eight-byte alignment. Utility payload ends at `0x803E133A`, leaving six original alignment bytes before ACL; ACL payload ends at `0x803E175D`, leaving three bytes before the next original unit. Symbols retain exact string lengths, and each split contains its own trailing gap. No padding arrays or modified reference hashes are used.

Ordinary reference helper/API bodies that are unlinked in this target remain available for compiler inlining or reference string-pool fidelity. In particular, the unlinked keyboard and mouse parser bodies contribute diagnostics to the exact original utility `.data` pool. Their two static key maps occupy a 47-byte source `.rodata` section that is linker-stripped and has no retained target extent. No unlinked body or key-map bytes receive reconstruction credit. `btm_bda_to_acl` and other helpers inline into retained callers; unused out-of-line copies are stripped normally. Keeping those bodies preserves idiomatic source and measured code generation.

## Interfaces and layouts

The existing `tACL_CONN` and `tBTM_CB` definitions are unchanged. Complete target instruction accesses validate the current control-block fields and callback ABI. Added names are the six ACL role-switch states, link-policy timer type 4, packet capability masks/sizes, link-policy/SCN integer aliases, two callback result structures, missing public/internal prototypes, `BT_READ_TRACE_LEVEL = 0xFF`, and the const null Bluetooth address declaration. Existing declarations are not retyped or resized.

`tBTM_LINK_POLICY_RESULTS` has status at offset 0, HCI status at 1, remote address at 2, and 16-bit settings at 8, total 0xA bytes. `tBTM_QOS_SETUP_CMPL` has the 0x14-byte flow specification at 0, handle at 0x14, and status at 0x16, total 0x18 bytes. These new callback layouts are evidenced by reference declarations and exact generated accesses. The null address declaration is the reference `extern BD_ADDR const bd_addr_null` and resolves through the original external data symbol; this issue does not split or resize that object.

## Original issue inventory

| Address | Size | Recovered name | Source |
| --- | --- | --- | --- |
| `0x80319BD4` | `0x14C` | `bta_hh_hdl_event` | `bta_hh_main.c` |
| `0x80319D20` | `0xB4` | `bta_hh_evt_code` | `bta_hh_main.c` |
| `0x80319DD4` | `0x13C` | `bta_hh_find_cb` | `bta_hh_utils.c` |
| `0x80319F10` | `0x80` | `bta_hh_clean_up_kdev` | `bta_hh_utils.c` |
| `0x80319F90` | `0xBC` | `bta_hh_add_device_to_list` | `bta_hh_utils.c` |
| `0x8031A04C` | `0x54` | `bta_hh_tod_spt` | `bta_hh_utils.c` |
| `0x8031A0A0` | `0xE0` | `bta_hh_trace_dev_db` | `bta_hh_utils.c` |
| `0x8031A180` | `0x1C` | `btm_acl_init` | `btm_acl.c` |
| `0x8031A19C` | `0x84` | `btm_handle_to_acl_index` | `btm_acl.c` |
| `0x8031A220` | `0x314` | `btm_acl_created` | `btm_acl.c` |
| `0x8031A534` | `0xC4` | `btm_acl_removed` | `btm_acl.c` |
| `0x8031A5F8` | `0x64` | `btm_acl_device_down` | `btm_acl.c` |
| `0x8031A65C` | `0x21C` | `BTM_SwitchRole` | `btm_acl.c` |
| `0x8031A878` | `0x170` | `btm_acl_encrypt_change` | `btm_acl.c` |
| `0x8031A9E8` | `0x1F0` | `BTM_SetLinkPolicy` | `btm_acl.c` |
| `0x8031ABD8` | `0x10` | `BTM_SetDefaultLinkPolicy` | `btm_acl.c` |
| `0x8031ABE8` | `0x104` | `btm_read_link_policy_complete` | `btm_acl.c` |
| `0x8031ACEC` | `0x8C` | `btm_read_remote_version_complete` | `btm_acl.c` |
| `0x8031AD78` | `0x22C` | `btm_read_remote_features_complete` | `btm_acl.c` |
| `0x8031AFA4` | `0x10` | `BTM_SetDefaultLinkSuperTout` | `btm_acl.c` |
| `0x8031AFB4` | `0xD8` | `BTM_IsAclConnectionUp` | `btm_acl.c` |
| `0x8031B08C` | `0x5C` | `BTM_GetNumAclLinks` | `btm_acl.c` |
| `0x8031B0E8` | `0x10` | `btm_get_acl_disc_reason_code` | `btm_acl.c` |
| `0x8031B0F8` | `0x9C` | `BTM_GetHCIConnHandle` | `btm_acl.c` |
| `0x8031B194` | `0x9C` | `btm_process_clk_off_comp_evt` | `btm_acl.c` |
| `0x8031B230` | `0x1F4` | `btm_acl_role_changed` | `btm_acl.c` |
| `0x8031B424` | `0x5C` | `btm_acl_timeout` | `btm_acl.c` |
| `0x8031B480` | `0x1A0` | `btm_get_max_packet_size` | `btm_acl.c` |
| `0x8031B620` | `0x44` | `BTM_AclRegisterForChanges` | `btm_acl.c` |
| `0x8031B664` | `0xD8` | `btm_qos_setup_complete` | `btm_acl.c` |

The complete ACL file additionally supplies #517's `BTM_ReadRSSI` at `0x8031B73C` (0x13C), `BTM_ReadLinkQuality` at `0x8031B878` (0x13C), `btm_read_rssi_complete` at `0x8031B9B4` (0x11C), `btm_read_link_quality_complete` at `0x8031BAD0` (0x118), `btm_remove_acl` at `0x8031BBE8` (0xBC), and `btm_chg_all_acl_pkt_types` at `0x8031BCA4` (0x220). The rest of #517 remains outside this delivery.

## Validation

The default-directory split-only pipeline exits 0 for `python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja`, and the explicit frozen verifier. Every retained code/data section compares at 100% for utilities, ACL, and the preceding complete HID main file. An independent original-issue inventory verifies all 30 functions / 7,016 bytes, including the two functions from merged prerequisite PR #694. After selecting both new units as `Matching`, the same default-directory configure, all-source compilation, full Ninja link/CHECK, whole-object comparisons, inventory audit, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exit 0. The source-selected main DOL matches frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scope excludes boot content 6, data contents, and WAD packaging. Final rebased exact-head verification is required before merge after the dependency merges.

## Traps

Unlinked code can still account for retained strings. Removing the unused HID boot parsers from this utility file would discard diagnostics that are present byte-for-byte in the target pool. Preserve these ordinary source bodies and let the linker strip their code and key maps. This differs from the target-absent public boot-report wrapper in F-515-1, whose unique diagnostic is absent. Compare the full payload and retained call relocations before deciding which reference functions to omit; do not generalize from one nearby file.
