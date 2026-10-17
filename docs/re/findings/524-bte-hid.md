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

Once these names land, switch the five objects to `Matching` and rerun full verification.

## Provenance

Sources and headers are rewritten for this project after Petari's BTE files, which carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE provenance](../licenses/524-bte-hid-NOTICE.md).
