# Issue 524: BTE HID device and host units

## F-524-1: Unit boundaries in 0x80328878–0x8032A544

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly `build/wpse-v1/asm/auto_03_802B1A10_text.s`, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/`.

| Range | Unit | Owner | Evidence |
| --- | --- | --- | --- |
| `0x80328878`–`0x80329258` | `hcicmds.c` (tail) | #522 | 15 HCI command builders that fill a GKI buffer and call the HCI send routine at `0x80324B88`. The run of such builders starts at `0x80326B3C`, inside #522, so #522 owns the split; these bodies are not split here. `0x80329164` (`ori r8, r4, 0xFC00`) is the vendor-specific command builder and ends the unit. |
| `0x80329258`–`0x803292C0` | `hidd_api.c` | #524 | `HID_DevInit`: clears the 0x144-byte `hd_cb`, sets both L2CAP MTUs to 64 and stores `hidd_proc_repage_timeout` as the repage timer parameter. |
| `0x803292C0`–`0x80329380` | `hidd_conn.c` | #524 | `hidd_conn_initiate`; strings `HID - Originate started` and `HID - Originate failed` at `.data` `0x803E2D70`–`0x803E2DA0`. |
| `0x80329380`–`0x80329448` | `hidd_mgmt.c` | #524 | `hidd_proc_repage_timeout`; its name string at `.data` `0x803E2DA0`–`0x803E2DC0`. `hd_cb` (`.bss` `0x804AD8F0`, 0x144 bytes plus 4 bytes padding) is placed here following Petari; its owning file cannot be distinguished by layout, confidence medium. |
| `0x80329448`–`0x803297C4` | `hidd_pm.c` | #524 | `hidd_pm_set_now`, `hidd_pm_proc_mode_change`, `hidd_pm_inact_timeout`. `hidd_pm_set_power_mode` and `hidd_pm_start` are inlined and their out-of-line copies are dead-stripped; `hidd_pm_init` and the `pwr_modes` table are absent. |
| `0x803297C4`–`0x8032A544` | `hidh_api.c` | #524 | 11 functions from `HID_HostGetSDPRecord` to `HID_HostSetSecurityLevel`; string pool at `.data` `0x803E2DC0`–`0x803E2ED8`; `hh_cb` at `.bss` `0x804ADA38`–`0x804ADE40` (0x404 bytes plus padding). `hidh_get_str_attr` is inlined three times into `hidh_search_callback` and its own copy is dead-stripped. |
| `0x8032A544` onward | `hidh_conn.c` | #525 | `hidh_conn_reg` references the next string block at `0x803E2ED8`. |

The remaining `hidd_api.c`, `hidd_conn.c`, and `hidd_mgmt.c` functions in Petari have no code in the target and were dead-stripped; they are not reconstructed, because nothing can verify them.

## F-524-2: BTE HID units use a GameCube 3.0 compiler

Function starts such as `0x803297C4` and `0x8032988C` are only 4-byte aligned, which rules out the Wii/1.0 default 16-byte alignment the issue assumed. Compiling `hidd_pm.c` and `hidh_api.c` with the RVL_SDK flags (`cflags_sdk`: `-O4,p -inline auto,level=3 -ipa file -sdata 8 -sdata2 8`) gives:

| Compiler | Result |
| --- | --- |
| GC/3.0a3, 3.0a3.2, 3.0a3.3, 3.0a3.4, 3.0a5, 3.0a5.2 | Identical instructions to the target in every function; only the dead-stripped out-of-line inline copies differ |
| Wii/1.0RC1, 1.0a, 1.0, 1.1, 1.3 | `hidd_pm_set_now` lowers its `switch` to a linear compare chain instead of the target's binary decision tree |
| GC/3.0a3 without `-ipa file` | Forward-declared callees are not inlined |

The HID units are therefore built with GC/3.0a3 (`BTE_COMPILER` in `configure.py`), matching Petari's BTE configuration. 3.0a3 versus 3.0a5.2 is unresolved. Confidence high for the GC 3.0 family. Other BTE units may need the same compiler; the #510 units were measured only with objdiff, which ignores alignment.

## F-524-3: Source-link evidence and external names

With GC/3.0a3 all 17 functions score 100% in objdiff and all `.data` and `.bss` bytes match (the 99.1% `.data` score for `hidh_api.c` is the 5 bytes of trailing alignment padding inside the split). In a local experiment, the 16 external symbols below were temporarily named in `symbols.txt` and the five units set to `Matching`; `ninja` exited 0 with the units linked from source and `tools/verify/verify.py verify` exited 0. The experiment was reverted, since those symbols belong to other issues; the units are committed as `NonMatching`.

| Address | Name needed | Owning block |
| --- | --- | --- |
| `0x80311160` | `GKI_freebuf` | RVL_SDK block containing `0x80311160` |
| `0x80314230` | `btu_start_timer` | block containing `0x80314230` |
| `0x803142A0` | `btu_stop_timer` | block containing `0x803142A0` |
| `0x8031FBFC` | `BTM_SetPowerMode` | block containing `0x8031FBFC` |
| `0x803217D8` | `BTM_SetSecurityLevel` | block containing `0x803217D8` |
| `0x8032A544` | `hidh_conn_reg` | #525 |
| `0x8032A69C` | `hidh_conn_disconnect` | #525 |
| `0x8032C1C0` | `hidh_conn_snd_data` | #525 |
| `0x8032C45C` | `hidh_conn_initiate` | #525 |
| `0x8032C558` | `hidh_conn_dereg` | #525 |
| `0x8032C7B0` | `L2CA_ConnectReq` | block containing `0x8032C7B0` |
| `0x80338B3C` | `SDP_InitDiscoveryDb` | block containing `0x80338B3C` |
| `0x80338EB4` | `SDP_ServiceSearchRequest` | block containing `0x80338EB4` |
| `0x80338F74` | `SDP_FindAttributeInRec` | block containing `0x80338F74` |
| `0x80339068` | `SDP_FindServiceUUIDInDb` | block containing `0x80339068` |
| `0x803456E8` | `memcmp` | MSL block containing `0x803456E8` |

Once these names land, switch the five objects to `Matching` and rerun full verification. Superseded by [F-524-4](#f-524-4-units-linked-from-source-and-matching).

## F-524-4: Units linked from source and matching

This supersedes the `NonMatching` status in F-524-3. #600 (F-522-4) named `GKI_freebuf` and `memcmp`, and #613 (F-534-3) independently named `btu_start_timer`, `btu_stop_timer`, `BTM_SetSecurityLevel`, and `L2CA_ConnectReq` while this work was in progress. The other 10 names are now recorded in `config/wpse-v1/symbols.txt` and `docs/re/SYMBOLS.md`; only the names changed, and splits, sizes, and types stay with their owning issues. Each name is the callee at the matching relocation of a 100% objdiff function, cross-checked against the target function's own references in dtk 1.8.4 assembly and the original DOL:

| Address | Size | Name | Target evidence |
| --- | --- | --- | --- |
| `0x80314230` | `0x70` | `btu_start_timer` | Reads `btu_cb` and calls two GKI timer-list routines; called by `hidd_proc_repage_timeout` and the inlined `hidd_pm_start` with the HID timer entries |
| `0x803142A0` | `0x14` | `btu_stop_timer` | Passes its timer entry and `btu_cb` + 0x40 to the same GKI timer-list removal routine by tail call; called by `HID_HostCloseDev` |
| `0x8031FBFC` | `0x1D0` | `BTM_SetPowerMode` | Compares a BD address with `memcmp` against `btm_cb` (`0x804AAD78`) device records; called four times by `hidd_pm_set_now` |
| `0x803217D8` | `0x228` | `BTM_SetSecurityLevel` | Logs `BTM_Sec: application registered` (`0x803E1F10`) and `BTM_SEC_REG[%d]: id %d, is_orig %d, psm 0x%04x, ...` (`+0xE0`) and matches service names with `strncmp`, so Petari's `btm_sec_set_security_level` is inlined; called by `HID_HostSetSecurityLevel` |
| `0x8032A544` | `0x158` | `hidh_conn_reg` | Logs `HID Control Registration failed` and `HID Interrupt Registration failed` (`0x803E2ED8`, `0x803E2EF8`), uses `hh_cb`, calls `L2CA_Deregister` |
| `0x8032A69C` | `0xA4` | `hidh_conn_disconnect` | Logs `HID - disconnect` (`0x803E2F1C`), calls `L2CA_DisconnectReq` twice |
| `0x8032C1C0` | `0x29C` | `hidh_conn_snd_data` | Allocates with `GKI_getpoolbuf`, copies with `memcpy`, frees with `GKI_freebuf` |
| `0x8032C45C` | `0xFC` | `hidh_conn_initiate` | Logs `HID - Originate failed` (`0x803E303C`), calls `L2CA_ConnectReq` |
| `0x8032C558` | `0x2C` | `hidh_conn_dereg` | Only two `L2CA_Deregister` calls, for the control and interrupt PSMs |
| `0x8032C7B0` | `0x234` | `L2CA_ConnectReq` | Logs `L2CA_ConnectReq()  PSM: 0x%04x` (`0x803E33D0`) |
| `0x80338B3C` | `0x378` | `SDP_InitDiscoveryDb` | Logs `SDP_InitDiscoveryDb Illegal param: p_db 0x%x, len %d, num_uuid %d, num_attr %d` (`0x803E55E0`) and clears the database with `memset`; called by `HID_HostGetSDPRecord` |
| `0x80338EB4` | `0x5C` | `SDP_ServiceSearchRequest` | Forwards to a single SDP connection routine; called by `HID_HostGetSDPRecord` right after `SDP_InitDiscoveryDb` with `hidh_search_callback` |
| `0x80338F74` | `0x28` | `SDP_FindAttributeInRec` | Leaf walk of a record's attribute list comparing the 16-bit ID at +4; called by `hidh_search_callback`, including its three inlined `hidh_get_str_attr` copies |
| `0x80339068` | `0x108` | `SDP_FindServiceUUIDInDb` | Walks discovery records calling an SDP UUID comparison helper; called by `hidh_search_callback` |

The five objects are registered `Matching`. From a deleted `build/`, `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0 with the units linked from source. `build/wpse-v1/report.json` gives 100% fuzzy, 100% matched code, and 100% complete code and data for all five. `hidh_api.c` reports 78.7% matched data only because objdiff scores its 280-byte `.data` string pool as one symbol, which is 99.1% fuzzy from the trailing alignment padding inside the split, as F-524-3 recorded; the linked DOL is byte-identical.

The whole #524 range `0x80328878`–`0x8032A544` is now covered by `Matching` units: `hcicmds.c` (`0x80326B3C`–`0x80329258`, #600) and the five HID units.

## Provenance

Sources and headers are rewritten for this project after Petari's BTE files, which carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE provenance](../licenses/524-bte-hid-NOTICE.md).

## Traps

- **BTE is not built with Wii/1.0.** The HID units match only with GC/3.0a3–3.0a5.2 plus `cflags_sdk`; Wii compilers emit a different `switch` tree, and 4-byte-aligned function starts rule out the Wii default alignment. Check alignment before trusting an issue's compiler label; see [F-524-2](#f-524-2-bte-hid-units-use-a-gamecube-30-compiler).
- **BTE headers live only in `include/revolution/bte/`.** The first version of these units shipped a parallel header set under `include/RVL_SDK/bte/` whose include guards and types (`BD_ADDR`, `TIMER_LIST_ENT`, `bt_trace.h`) collided with the shared headers, so the two sets could not be combined in one unit. The HID headers moved to `include/revolution/bte/`, and the other declarations were merged into the shared headers as additions. Extend those headers instead of adding new ones; see [D-0013](../../DECISIONS.md#d-0013-shared-rvl_sdk-headers-come-from-petari).
