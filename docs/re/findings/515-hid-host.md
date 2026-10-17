# F-515-1: HID-host action, API, and state-machine reconstruction

Issue #515 spans 29 functions / 8,088 code bytes at `0x80317C3C`-`0x80319BD4`. Its first two power callbacks (300 bytes) are supplied by the complete power unit in PR #690 for #514. This delivery reconstructs its remaining 27 HID-host functions / 7,788 bytes and two #516 state-machine helpers / 512 bytes. The complete three HID units contain 29 retained functions / 8,300 code bytes. This source delivery does not complete the remaining #516 utility functions.

Reference names, algorithms, declarations, and data tables come from Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, corresponding `src/RVL_SDK/bte/bta_hh_act.c`, `bta_hh_api.c`, `bta_hh_main.c`, and headers. See the [file-specific BTE notice](../licenses/515-bte-NOTICE.md) and [full Apache-2.0 license](../licenses/510-apache-2.0.txt). Comments are documented here, includes use the canonical SDK tree, and trace calls use the existing typed `LogMsg` interface. Pointer trace arguments are explicitly converted to `UINT32`, preserving the target ABI.

## Compiler and boundaries

Measured `GC/3.0a3`, `BTE_COMPILER`, and `cflags_sdk` reproduce every retained function's complete non-relocation instruction stream with a unique same-sized target candidate in the surrounding range. This replaces the survey issue's generic Wii/1.0 compiler assumption. Whole-object default objdiff compares all code and data sections at 100%, including resolved callback and string relocations.

| Unit | Functions / code bytes | Section extents |
| --- | --- | --- |
| `bta_hh_act.c` | 18 / 5,960 | `.text` `0x80317D68`-`0x803194B0`; `.data` `0x803E0878`-`0x803E0D70`; `.sdata` `0x804B8EF8`-`0x804B8EFC` |
| `bta_hh_api.c` | 8 / 976 | `.text` `0x803194B0`-`0x80319880`; `.data` `0x803E0D70`-`0x803E0DA0`; `.sdata2` `0x804BB610`-`0x804BB618` |
| `bta_hh_main.c` | 3 / 1,364 | `.text` `0x80319880`-`0x80319DD4`; `.rodata` `0x803980B8`-`0x8039813C`; `.data` `0x803E0DA0`-`0x803E1080`; `.bss` `0x804AAB48`-`0x804AAD78` |

The API's retained registration record is two pointers / eight bytes. Its only retained diagnostic has 46 bytes at `0x803E0D70`, followed by two alignment bytes. The unlinked reference `BTA_HhParseBootRpt` body has an additional diagnostic absent from this target and is omitted; its public declaration remains available. Other unlinked API wrappers stay ordinary C source and earn no retained-function credit. The action unit has a one-byte empty device-name string in `.sdata` followed by three alignment bytes. Exact literal sizes and natural switch tables are recorded in `symbols.txt`; no synthetic source padding is inserted.

The state-machine file owns `bta_hh_cb` at `0x804AAB48`, size 0x230. Its keyboard state occupies 0xD bytes; sixteen 0x20-byte device blocks begin at offset 0x10. The current-device pointer is at 0x210, index map at 0x214, application callback at 0x224, SDP database pointer at 0x228, trace level at 0x22C, connection count at 0x22D, and pending-disable flag at 0x22E. Device fields and callback/message layouts come from the pinned reference and are validated by exact source accesses and the full link. The action table has twelve pointers at `0x803980B8`; three 0x18-byte state tables follow at `0x803980E8`, `0x80398100`, and `0x80398118`, then the 0xC-byte table of state-table pointers at `0x80398130`.

The new HID-host headers extend the canonical `include/revolution/bte/` tree. Existing shared declarations retain their types and layouts. Additions are `tBTA_SEC` as `UINT8`, the 35-byte service-name limit, subsystem identifier 23, and named handshake status constants. These constants and declarations introduce no changes to existing shared ABI types.

## Owned HID-host function inventory

| Address | Size | Recovered name | Source |
| --- | --- | --- | --- |
| `0x80317D68` | `0x174` | `bta_hh_api_enable` | `bta_hh_act.c` |
| `0x80317EDC` | `0x104` | `bta_hh_api_disable` | `bta_hh_act.c` |
| `0x80317FE0` | `0xA4` | `bta_hh_disc_cmpl` | `bta_hh_act.c` |
| `0x80318084` | `0x134` | `bta_hh_sdp_cback` | `bta_hh_act.c` |
| `0x803181B8` | `0x194` | `bta_hh_start_sdp` | `bta_hh_act.c` |
| `0x8031834C` | `0x168` | `bta_hh_sdp_cmpl` | `bta_hh_act.c` |
| `0x803184B4` | `0x68` | `bta_hh_api_disc_act` | `bta_hh_act.c` |
| `0x8031851C` | `0x128` | `bta_hh_open_cmpl_act` | `bta_hh_act.c` |
| `0x80318644` | `0xB8` | `bta_hh_open_act` | `bta_hh_act.c` |
| `0x803186FC` | `0x58` | `bta_hh_data_act` | `bta_hh_act.c` |
| `0x80318754` | `0x268` | `bta_hh_handsk_act` | `bta_hh_act.c` |
| `0x803189BC` | `0x1F8` | `bta_hh_ctrl_dat_act` | `bta_hh_act.c` |
| `0x80318BB4` | `0x200` | `bta_hh_close_act` | `bta_hh_act.c` |
| `0x80318DB4` | `0x1C` | `bta_hh_get_dscp_act` | `bta_hh_act.c` |
| `0x80318DD0` | `0x164` | `bta_hh_maint_dev_act` | `bta_hh_act.c` |
| `0x80318F34` | `0x11C` | `bta_hh_get_acl_q_info` | `bta_hh_act.c` |
| `0x80319050` | `0x1C8` | `bta_hh_write_dev_act` | `bta_hh_act.c` |
| `0x80319218` | `0x298` | `bta_hh_cback` | `bta_hh_act.c` |
| `0x803194B0` | `0x88` | `BTA_HhEnable` | `bta_hh_api.c` |
| `0x80319538` | `0x38` | `BTA_HhDisable` | `bta_hh_api.c` |
| `0x80319570` | `0x64` | `BTA_HhClose` | `bta_hh_api.c` |
| `0x803195D4` | `0xB4` | `BTA_HhOpen` | `bta_hh_api.c` |
| `0x80319688` | `0x90` | `BTA_HhSendData` | `bta_hh_api.c` |
| `0x80319718` | `0xA8` | `BTA_HhAddDev` | `bta_hh_api.c` |
| `0x803197C0` | `0x6C` | `BTA_HhRemoveDev` | `bta_hh_api.c` |
| `0x8031982C` | `0x54` | `BTA_HhGetAclQueueInfo` | `bta_hh_api.c` |
| `0x80319880` | `0x354` | `bta_hh_sm_execute` | `bta_hh_main.c` |

The other two original issue entries are `bta_dm_pm_btm_status` at `0x80317C3C` (0x120) and `bta_dm_pm_timer` at `0x80317D5C` (0xC), delivered separately in PR #690. The complete main file also supplies #516's `bta_hh_hdl_event` at `0x80319BD4` (0x14C) and local `bta_hh_evt_code` at `0x80319D20` (0xB4).

## Validation

The initial `NonMatching` split-only artifact passes the explicit frozen verifier. `.venv/Scripts/ninja all_source` exits 0; `.venv/Scripts/ninja build/515-baseline/wpse-v1/main.dol` exits 0; the explicit custom-directory frozen verifier exits 0. All three original/source whole-object comparisons report every code/data section at 100%. The original issue inventory independently confirms the 27 new functions / 7,788 bytes. The subsequent default-directory source-selected pipeline also passes: `python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja`, all three whole-object comparisons and the issue inventory, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exit 0. Frozen main game DOL SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scope excludes boot content 6, data contents, and WAD packaging. PR #690 is now merged. Final acceptance revalidates its two power callbacks together with all 27 HID-host functions: 29 issue functions / 8,088 bytes at their native final addresses, with the complete power and HID objects selected from source. Local exact-head verification after the dependency merges is the acceptance gate; GitHub CI does not run automatically.

## Traps

The custom-directory default Ninja CHECK reads the literal `build/wpse-v1/main.dol` recorded in `build.sha1`, so it fails when that default file is absent despite a successful custom-directory link. Verify the actual artifact explicitly, and use the default build directory for the subsequent complete source-selected pipeline. This confirms the custom-directory CHECK limitation already documented in F-511-4; do not change the frozen check file to accommodate scratch builds.

Keeping the absent boot-report parser retained an extra 24-byte diagnostic after the API's real data payload and overlapped the following state-machine strings. The target calls and unique same-size instruction matching establish the parser is absent. Omitting that function produces the exact 46-byte API data payload and restores whole-object data matching; do not truncate the diagnostic or invent padding.
