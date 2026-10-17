# F-518-3: Complete BTM controller/discovery/inquiry issue inventory

Issue #518 owns 22 functions and 4,456 code bytes at `0x8031D914`-`0x8031EA7C`. It spans three source files, so accepting only the discovery or inquiry file does not complete its original inventory. The required deliveries are:

| File | Owned functions / bytes | Delivery |
| --- | --- | --- |
| `btm_devctl.c` | 9 / 1,540 | Combined [PR #702](https://github.com/revvmp/pokemon-rumble-decomp/pull/702), complete controller file owned by #517 |
| `btm_discovery.c` | 3 / 308 | Merged [PR #699](https://github.com/revvmp/pokemon-rumble-decomp/pull/699), commit `baa65d206631b7b869a6632ddfc826d9ead767bf` |
| `btm_inq.c` | 10 / 2,608 | Merged [PR #698](https://github.com/revvmp/pokemon-rumble-decomp/pull/698), commit `ea37468eba51483c2e13d57ade20ffefb27822d1` |

This acceptance audit adds no source bytes, new source units, split ranges, or duplicate ledger rows. Existing delivery evidence is in [F-518-1 and F-518-2](518-btm-discovery.md), [F-519-1 and F-519-2](519-btm-inq-main-pm.md), and the complete controller's F-517-2. All three files use measured GC/3.0a3 (`BTE_COMPILER`) with SDK flags. Source and license provenance remain with the delivering files and their existing notices.

## Original issue function inventory

| Address | Size | Recovered name | Complete source file |
| --- | --- | --- | --- |
| `0x8031D914` | `0xA0` | `BTM_WritePageTimeout` | `btm_devctl.c` |
| `0x8031D9B4` | `0xD8` | `BTM_ReadStoredLinkKey` | `btm_devctl.c` |
| `0x8031DA8C` | `0xD0` | `BTM_WriteStoredLinkKey` | `btm_devctl.c` |
| `0x8031DB5C` | `0xD0` | `BTM_DeleteStoredLinkKey` | `btm_devctl.c` |
| `0x8031DC2C` | `0x7C` | `btm_read_stored_link_key_complete` | `btm_devctl.c` |
| `0x8031DCA8` | `0x5C` | `btm_write_stored_link_key_complete` | `btm_devctl.c` |
| `0x8031DD04` | `0x68` | `btm_delete_stored_link_key_complete` | `btm_devctl.c` |
| `0x8031DD6C` | `0x18C` | `btm_return_link_keys_evt` | `btm_devctl.c` |
| `0x8031DEF8` | `0x20` | `btm_report_device_status` | `btm_devctl.c` |
| `0x8031DF18` | `0x78` | `btm_discovery_db_init` | `btm_discovery.c` |
| `0x8031DF90` | `0xB8` | `btm_discovery_db_reset` | `btm_discovery.c` |
| `0x8031E048` | `0x4` | `btm_discovery_timeout` | `btm_discovery.c` |
| `0x8031E04C` | `0x284` | `BTM_SetDiscoverability` | `btm_inq.c` |
| `0x8031E2D0` | `0xB0` | `BTM_SetInquiryScanType` | `btm_inq.c` |
| `0x8031E380` | `0xB0` | `BTM_SetPageScanType` | `btm_inq.c` |
| `0x8031E430` | `0x98` | `BTM_SetInquiryMode` | `btm_inq.c` |
| `0x8031E4C8` | `0x188` | `BTM_SetConnectability` | `btm_inq.c` |
| `0x8031E650` | `0x10` | `BTM_IsInquiryActive` | `btm_inq.c` |
| `0x8031E660` | `0x108` | `BTM_CancelInquiry` | `btm_inq.c` |
| `0x8031E768` | `0x1B4` | `BTM_StartInquiry` | `btm_inq.c` |
| `0x8031E91C` | `0xE8` | `BTM_ReadRemoteDeviceName` | `btm_inq.c` |
| `0x8031EA04` | `0x78` | `BTM_CancelRemoteDeviceName` | `btm_inq.c` |

These are nine controller functions, three discovery functions, and ten inquiry functions in the original issue body. The controller tail ends at `0x8031DF18`; discovery then ends at `0x8031E04C`. Its ten inquiry functions stop at `0x8031EA7C`, while the full inquiry source file continues into the already-closed #519 inventory. Acceptance must compare complete files and retain the exact original issue counts, rather than credit their neighboring functions twice.

## Acceptance status

PR #702 combines the complete device/controller source and this original-issue acceptance audit with the two earlier merged deliveries. After rebasing onto the refreshed controller delivery, `python configure.py --no-progress --build-dir build/518-audit-refresh-1`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja`, all three complete original/source objdiff comparisons, the 22-function / 4,456-byte original inventory audit, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/518-audit-refresh-1/wpse-v1` each exit 0. Every compared code/data section is 100%. The actual source-selected DOL matches frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The combined PR repeats both original inventories and complete source-linked frozen verification at its final committed head after rebasing onto main. Scope is the main game executable only, excluding boot content 6, data contents, and WAD packaging. Exact head and commands are recorded in the pull request before merge. Local verification is the merge gate under the updated take-issue procedure.

## Traps

No new reverse-engineering trap emerged from this inventory audit. Whole-file deliveries cross the survey block boundaries; confirm every original entry is source-selected before accepting the issue. The full files and earlier merged work receive no duplicate reconstruction byte credit.
