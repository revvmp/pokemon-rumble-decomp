# F-517-2: Complete BTM security-device and controller units

Issue #517 owns 37 functions / 8,664 code bytes at `0x8031B73C`-`0x8031D914`. The first six ACL functions (1,928 bytes) are supplied in pending PR #696. This delivery reconstructs the remaining 31 functions / 6,736 bytes and nine #518 controller functions / 1,540 bytes. The complete device and controller units contain 40 retained functions / 8,276 code bytes. Earlier ACL reconstruction receives no duplicate byte credit, and remaining #518 inquiry work is excluded.

The prerequisite canonical interfaces and target-evidenced controller-ready value are documented in [F-517-1](517-btm-device-headers.md) and pending PR #700. The refreshed prerequisite head is `9abc1cad13ae3de93aa547f500050094536c0b98`; it preserves the complete reset enum already merged through F-519-2, including `WAIT_BUF_SIZE` and ready value 5. This source branch is stacked on that prerequisite and must rebase onto its eventual merged `main` before final acceptance. The full source build must include the ACL dependency and preserve all existing matching units. Actions billing/spending-limit failures currently block the merge backstops; no merge is claimed.

Reference names, algorithms, layouts, and data come from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/bte/btm_dev.c` and `btm_devctl.c`. See the [file-specific notice](../licenses/517-bte-NOTICE.md). Canonical SDK types and headers replace reference include paths; comments live in documentation. Logging uses existing BTM trace level/layer and typed `LogMsg` functions, with explicit integer conversions for trace arguments. The reverse-stream operation uses the project's equivalent `REVERSED_STREAM_TO_ARRAY` macro. Intermediate reset-state names describe target-proven queries, as recorded in F-517-1.

## Compiler, retained coverage, and boundaries

Measured `GC/3.0a3`, `BTE_COMPILER`, and `cflags_sdk` reproduce every retained non-relocation instruction stream and complete original/source code/data comparisons. This replaces the issue survey's generic Wii/1.0 assumption. Device has seven retained functions; controller has thirty-three. Helpers and unused public reference APIs remain ordinary C source when needed for inlining or diagnostic-pool fidelity, and their out-of-line code is stripped normally. No unlinked body receives progress credit.

| Unit | Functions / code bytes | Exact split extents |
| --- | --- | --- |
| `btm_dev.c` | 7 / 1,740 | `.text` `0x8031BEC4`-`0x8031C590` |
| `btm_devctl.c` | 33 / 6,536 | `.text` `0x8031C590`-`0x8031DF18`; `.data` `0x803E1760`-`0x803E19C0`; `.sdata` `0x804B8F00`-`0x804B8F18` |

The controller `.data` payload has 602 bytes through `0x803E19BA`, followed by six original alignment bytes. Its `.sdata` payload has 18 bytes through `0x804B8F12`, followed by six alignment bytes. Both compiled sections require eight-byte alignment. The four-byte initial class literal at `0x804B8F00` is `00 1F 00 00`; the following diagnostics are `TRUE` at `0x804B8F04` (five bytes) and `FALSE` at `0x804B8F0C` (six bytes). These are reference data, not executable byte blobs. Complete source payload comparisons find zero differing data bytes. Symbols retain exact payload sizes and splits include the natural trailing gaps, with no synthetic source padding.

Existing `tBTM_SEC_DEV_REC`, `tBTM_DEVCB`, and `tBTM_CB` definitions remain unchanged. Exact accesses, callback relocations, and the frozen link validate those layouts for these units. Controller-ready value 5 is proved by the original device-up check and all reconstructed reset paths. Target-specific caller behavior for the vendor command remains as documented in F-517-1; the function definition uses its measured `UINT16` opcode signature without changing the existing L2CAP caller.

## Original issue inventory

| Address | Size | Recovered name | Source |
| --- | --- | --- | --- |
| `0x8031B73C` | `0x13C` | `BTM_ReadRSSI` | `btm_acl.c` |
| `0x8031B878` | `0x13C` | `BTM_ReadLinkQuality` | `btm_acl.c` |
| `0x8031B9B4` | `0x11C` | `btm_read_rssi_complete` | `btm_acl.c` |
| `0x8031BAD0` | `0x118` | `btm_read_link_quality_complete` | `btm_acl.c` |
| `0x8031BBE8` | `0xBC` | `btm_remove_acl` | `btm_acl.c` |
| `0x8031BCA4` | `0x220` | `btm_chg_all_acl_pkt_types` | `btm_acl.c` |
| `0x8031BEC4` | `0x188` | `BTM_SecAddDevice` | `btm_dev.c` |
| `0x8031C04C` | `0xAC` | `BTM_SecDeleteDevice` | `btm_dev.c` |
| `0x8031C0F8` | `0xA0` | `BTM_SecReadDevName` | `btm_dev.c` |
| `0x8031C198` | `0x120` | `btm_sec_alloc_dev` | `btm_dev.c` |
| `0x8031C2B8` | `0xB8` | `btm_find_dev_by_handle` | `btm_dev.c` |
| `0x8031C370` | `0x88` | `btm_find_dev` | `btm_dev.c` |
| `0x8031C3F8` | `0x198` | `btm_find_oldest_dev` | `btm_dev.c` |
| `0x8031C590` | `0xA0` | `btm_dev_init` | `btm_devctl.c` |
| `0x8031C630` | `0xB0` | `btm_db_reset` | `btm_devctl.c` |
| `0x8031C6E0` | `0x9C` | `BTM_DeviceReset` | `btm_devctl.c` |
| `0x8031C77C` | `0xA8` | `BTM_SendHciReset` | `btm_devctl.c` |
| `0x8031C824` | `0x1C` | `BTM_IsDeviceUp` | `btm_devctl.c` |
| `0x8031C840` | `0xF0` | `BTM_SetAfhChannels` | `btm_devctl.c` |
| `0x8031C930` | `0x298` | `btm_dev_timeout` | `btm_devctl.c` |
| `0x8031CBC8` | `0x1C4` | `btm_reset_complete` | `btm_devctl.c` |
| `0x8031CD8C` | `0xC8` | `btm_read_hci_buf_size_complete` | `btm_devctl.c` |
| `0x8031CE54` | `0xBC` | `btm_read_local_version_complete` | `btm_devctl.c` |
| `0x8031CF10` | `0x4FC` | `btm_read_local_features_complete` | `btm_devctl.c` |
| `0x8031D40C` | `0xB8` | `BTM_SetLocalDeviceName` | `btm_devctl.c` |
| `0x8031D4C4` | `0x8C` | `btm_read_local_name_complete` | `btm_devctl.c` |
| `0x8031D550` | `0x40` | `BTM_ReadLocalDeviceAddr` | `btm_devctl.c` |
| `0x8031D590` | `0x48` | `btm_read_local_addr_complete` | `btm_devctl.c` |
| `0x8031D5D8` | `0x4C` | `BTM_ReadLocalVersion` | `btm_devctl.c` |
| `0x8031D624` | `0x80` | `BTM_SetDeviceClass` | `btm_devctl.c` |
| `0x8031D6A4` | `0x10` | `BTM_ReadDeviceClass` | `btm_devctl.c` |
| `0x8031D6B4` | `0x10` | `BTM_ReadLocalFeatures` | `btm_devctl.c` |
| `0x8031D6C4` | `0x18` | `BTM_RegisterForDeviceStatusNotif` | `btm_devctl.c` |
| `0x8031D6DC` | `0xFC` | `BTM_VendorSpecificCommand` | `btm_devctl.c` |
| `0x8031D7D8` | `0x6C` | `btm_vsc_complete` | `btm_devctl.c` |
| `0x8031D844` | `0x38` | `BTM_RegisterForVSEvents` | `btm_devctl.c` |
| `0x8031D87C` | `0x98` | `btm_vendor_specific_evt` | `btm_devctl.c` |

The controller file also supplies #518's `BTM_WritePageTimeout` at `0x8031D914` (0xA0), `BTM_ReadStoredLinkKey` at `0x8031D9B4` (0xD8), `BTM_WriteStoredLinkKey` at `0x8031DA8C` (0xD0), `BTM_DeleteStoredLinkKey` at `0x8031DB5C` (0xD0), `btm_read_stored_link_key_complete` at `0x8031DC2C` (0x7C), `btm_write_stored_link_key_complete` at `0x8031DCA8` (0x5C), `btm_delete_stored_link_key_complete` at `0x8031DD04` (0x68), `btm_return_link_keys_evt` at `0x8031DD6C` (0x18C), and `btm_report_device_status` at `0x8031DEF8` (0x20).

## Validation

Original/source whole-object comparisons report all retained sections at 100% for device, controller, and the preceding ACL unit. The independent original-issue inventory confirms all 37 addresses and sizes, 8,664 bytes, including six functions from merged PR #696. The split-only baseline and then both units selected as `Matching` each pass the full default-directory pipeline: `python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja`, all three whole-object comparisons, the inventory audit, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exit 0. Frozen main DOL SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The final rebased exact head must repeat these checks after all dependencies merge. Main game DOL is the only verification scope, excluding boot content 6, data contents, and WAD packaging.

## Traps

Do not infer an empty helper's identity from `blr` bytes alone. The unlinked `BTM_ContinueReset` candidate accidentally matched already-named `btm_discovery_timeout` at `0x8031E048` outside this file. Excluding that weak candidate preserves the correct contiguous controller boundary `0x8031DF18` and leaves the neighboring symbol unchanged. F-517-1 records the full evidence.

Missing feature macros can compile as implicit external function calls in this C compiler. The initial controller probe was 0x80 bytes too long in local-features completion and had undefined `HCI_*SUPPORTED` symbols. Adding the exact reference bit-test macros restores the 0x4FC-byte target routine and all other retained instructions. Treat successful compilation alone as insufficient; inspect unresolved symbols and complete comparison results.
