# Issue 519: BTE inquiry, BTM init and power manager units

Covers issue #519 (`0x8031EA7C`-`0x803209F4`) and the first ten `btm_inq.c` functions in #518 (`0x8031E04C`-`0x8031EA7C`). The last two #519 functions, `btm_sco_init` and `btm_esco_conn_rsp`, were matched with `btm_sco.c` in [F-520-1](520-btm-sco.md#f-520-1-unit-boundaries). Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/`.

## F-519-1: Unit boundaries

| Range | Unit | Owner | Evidence |
| --- | --- | --- | --- |
| `.text` `0x8031E04C`-`0x8031FAE4`; `.data` `0x803E19C0`-`0x803E1BD0`; `.sdata2` `0x804BB618`-`0x804BB620` | `btm_inq.c` | #518 | Starts at `BTM_SetDiscoverability` (string `BTM_SetDiscoverability: mode %d`), after `btm_discovery_timeout` ends `btm_discovery.c`; ends with `btm_inq_rmt_name_failed`. The nine inquiry strings start at `0x803E19C0`, after the `BTM_DeleteStoredLinkKey` string of `btm_devctl.c`. `.sdata2` holds `general_inq_lap` and `limited_inq_lap`. |
| `.text` `0x8031FAE4`-`0x8031FB40`; `.bss` `0x804AAD78`-`0x804AD53C` | `btm_main.c` | #519 | `btm_init` clears the 0x27C4-byte `btm_cb` and calls the five init functions in Petari's order. The `.bss` split ends at the end of `btm_cb`; the four bytes up to `gap_cb` are alignment padding, so the `btm_cb` symbol is 0x27C4 bytes, not 0x27C8. |
| `.text` `0x8031FB40`-`0x803207D4`; `.rodata` `0x80398140`-`0x80398150`; `.data` `0x803E1BD0`-`0x803E1C10`; `.sdata2` `0x804BB620`-`0x804BB628` | `btm_pm.c` | #519 | `BTM_PmRegister` to `btm_pm_proc_mode_change` (string `btm mode change AFTER unsniffing`); `.rodata` is the 9-byte `btm_pm_md_comp_matrix`, `.sdata2` the 3-byte `btm_pm_mode_off` and `btm_pm_mode_msk`. |

`btm_inq.c` starts in #518, which owns its split; it is taken here with #518 claimed.

## F-519-2: Reconstruction

All three units match with GC/3.0a3 and `cflags_sdk` (`BTE_COMPILER`): 25 `btm_inq.c`, 1 `btm_main.c` and 10 `btm_pm.c` functions are 100% in objdiff, as is every data section, and the full DOL verifies with all three linked from source.

- **Periodic inquiry is absent from this SDK.** Petari's `btm_inq.c` compiled to 0x282 bytes of `.data`; the target has 0x20A. The difference is exactly the strings of `BTM_SetPeriodicInquiryMode` and `BTM_CancelPeriodicInquiry`, so both functions are left out of the source and their prototypes are not declared. The other unused functions (`BTM_ReadDiscoverability`, `BTM_ReadConnectability`, `BTM_InqFirstResult`, `BTM_InqNextResult`, `BTM_ReadNumInqDbEntries`, `BTM_InquiryRegisterForChanges`, `BTM_SetInquiryFilterCallback`, `btm_clr_inq_db`, `btm_inq_check_ageing`) leave no data and are kept; the linker strips them. `BTM_IsPowerManagerOn` is likewise stripped from `btm_pm.c`.
- `btm_clr_inq_result_flt`, `btm_inq_db_find` and `btm_initiate_inquiry` are inlined into their callers.
- **Device states.** `btm_inq.c` compares `btm_cb.devcb.state` with 3, which needs the full state list of this SDK: 0 and 1 as before, then 2, 3 and 4, and `BTM_DEV_STATE_READY` = 5 (Petari, from `btm_devctl.c`). The former header value `READY` = 2 was unused. States 2 to 4 are named `BTM_DEV_STATE_WAIT_BUF_SIZE`, `BTM_DEV_STATE_WAIT_LOCAL_VERSION` and `BTM_DEV_STATE_WAIT_LOCAL_FEATURES` after the reset step Petari's `btm_devctl.c` performs in each (descriptive, medium confidence).
- `BTM_GENERAL_INQUIRY_ACTIVE` and the other inquiry-active flags are plain macros, as Petari makes them with `(enumerator + 0)`.

| Address | Size | Name | Evidence |
| --- | --- | --- | --- |
| `0x8031E2D0` | `0xB0` | `BTM_SetInquiryScanType` | Source order and 100% match, high |
| `0x8031E380` | `0xB0` | `BTM_SetPageScanType` | Same, high |
| `0x8031E430` | `0x98` | `BTM_SetInquiryMode` | Same, high |
| `0x8031EE00` | `0x198` | `btm_inq_db_reset` | Same, high |
| `0x8031EF98` | `0x4` | `btm_inq_db_init` | Called first by `btm_init`, high |
| `0x8031EF9C` | `0xDC` | `btm_inq_find_bdaddr` (static) | Source order and 100% match, high |
| `0x8031F078` | `0x104` | `btm_inq_db_new` (static) | Same, high |
| `0x8031F17C` | `0xE4` | `btm_set_inq_event_filter` (static) | Same, high |
| `0x8031FB40` | `0xBC` | `BTM_PmRegister` | Same, high |
| `0x8031FE84` | `0x78` | `btm_pm_reset` | Same, high |
| `0x8031FEFC` | `0x50` | `btm_pm_sm_alloc` | Same, high |
| `0x8031FF4C` | `0x21C` | `btm_pm_compare_modes` (static) | Same, high |
| `0x80320168` | `0x160` | `btm_pm_get_set_mode` (static) | Same, high |
| `0x803202C8` | `0x234` | `btm_pm_snd_md_req` (static) | Same, high |

Outside names from the call sites: `GKI_get_tick_count` `0x80311F40` (`btm_process_inq_results`), `btm_acl_init` `0x8031A180` and `btm_dev_init` `0x8031C590` (`btm_init`), `btm_handle_to_acl_index` `0x8031A19C` (`btm_pm_proc_mode_change`) and `BTM_ReadDeviceClass` `0x8031D6A4` (`BTM_SetDiscoverability`). Their splits are left to the owning issues.

## Provenance

Sources and headers are rewritten after Petari's BTE files; see [BTE provenance](../licenses/519-bte-NOTICE.md).

## Traps

- **A data-size excess can mean whole functions are missing.** `btm_inq.c` matched every surviving function but had 0x78 bytes too much `.data`. Comparing the compiled string pool with the target strings showed two functions absent from this SDK. Compare the strings before shifting a split to fit.
- **A `.bss` object can end before the next unit's alignment.** `btm_cb` ends at `0x804AD53C`; ending the split there and shrinking the symbol to 0x27C4 leaves the padding to the next unit, as in F-488-1.
