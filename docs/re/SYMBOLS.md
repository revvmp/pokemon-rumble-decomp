# Symbols and interfaces

## CNT (#286)

For `wpse-v1` / `main.dol`, [F-286-1](findings/286-cnt.md) establishes `CNTInit` (`0x801778F0`, `0x44` bytes), `CNTShutdown` (`0x80177940`, 4 bytes), `CNTReleaseHandle` (`0x80178000`, `0x68` bytes), `CNTOpen` (`0x80178070`, `0x33C` bytes), `CNTGetLength` (`0x801784A0`, `0x30` bytes), and `CNTClose` (`0x801784D0`, `0x5C` bytes). All are `.text`; subtract `0x8004FD40` for DOL file offsets. Names are high confidence from behavior, embedded diagnostics, and public CNT interface corroboration. Reconstructed source destination is `src/RVL_SDK/cnt/cnt.c`.

The descriptive local names `sCntVersion` (`.sdata`, `0x804B80F0`, 4 bytes) and `sCntInitialized` (`.sbss`, `0x804B9100`, first 4 bytes of an 8-byte padded range) describe the banner pointer and one-time initialization flag. These original spellings are unknown. File-info layout and calling conventions are recorded in F-286-1 and represented in `include/RVL_SDK/cnt.h`; the header is private to this reconstruction so far.

For each recovered symbol record target, artifact, section, runtime address, file offset, extent, original label if present, chosen name, kind, confidence, evidence ID, and source destination. Distinguish known bounds from inferred ones.

For each shared type or interface record size, alignment, field offsets, parameter and return conventions, signedness, ownership evidence, and unresolved questions. Keep target-specific address mappings separate from shared declarations. Rename speculative labels only when evidence supports the change and preserve traceability.

## RVL_SDK PPCArch

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x802B1830` | `0x8` | `fn_802B1830` | `PPCMthid0` | function | high | F-463-1 | `src/RVL_SDK/base/PPCArch.c` |
| `0x802B1870` | `0x8` | `fn_802B1870` | `PPCSync` | function | high | F-463-1 | `src/RVL_SDK/base/PPCArch.c` |

## Issue 510: BTE transport, initialization and logging

[F-510-1](findings/510-bte-transport-logging.md) records the complete recovered function ranges, data addresses, ABI confidence and outstanding shared-symbol dependencies. Names are supported by Petari's BTE reference and target instruction/call structure. bte_hcisu_send/task/close and bta_ci_hci_msg_handler occupy 0x80313A60-0x80313B30; BTE_InitStack is at 0x80313B30; LogMsg and its seven fixed-argument wrappers occupy 0x80313B60-0x80313D00. BTUInterruptHandler (0x80313D00), BTA_Init (0x80313D20), and BTA_CleanUp (0x80313E00) begin bte_main.c, which continues into issue #511.

The two transport pointers are at 0x804B9B78 and 0x804B9B7C. The header and callback offsets, and the distinction between reference-derived declarations and target-observed accesses, are recorded in the finding. The logging allocation at 0x804A9040 includes unresolved alignment padding and must not yet be treated as source-link matched.

## Issue 524: BTE HID device and host

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x80329258` | `0x68` | `fn_80329258` | `HID_DevInit` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_api.c` |
| `0x803292C0` | `0xC0` | `fn_803292C0` | `hidd_conn_initiate` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_conn.c` |
| `0x80329380` | `0xC8` | `fn_80329380` | `hidd_proc_repage_timeout` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_mgmt.c` |
| `0x80329448` | `0x1D8` | `fn_80329448` | `hidd_pm_set_now` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_pm.c` |
| `0x80329620` | `0x158` | `fn_80329620` | `hidd_pm_proc_mode_change` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_pm.c` |
| `0x80329778` | `0x4C` | `fn_80329778` | `hidd_pm_inact_timeout` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_pm.c` |
| `0x803297C4` | `0xC8` | `fn_803297C4` | `HID_HostGetSDPRecord` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032988C` | `0x450` | `fn_8032988C` | `hidh_search_callback` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x80329CDC` | `0x44` | `fn_80329CDC` | `HID_HostInit` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x80329D20` | `0x7C` | `fn_80329D20` | `HID_HostRegister` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x80329D9C` | `0xE4` | `fn_80329D9C` | `HID_HostDeregister` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x80329E80` | `0x190` | `fn_80329E80` | `HID_HostAddDev` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032A010` | `0xC8` | `fn_8032A010` | `HID_HostRemoveDev` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032A0D8` | `0x64` | `fn_8032A0D8` | `HID_HostOpenDev` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032A13C` | `0x140` | `fn_8032A13C` | `HID_HostWriteDev` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032A27C` | `0xA0` | `fn_8032A27C` | `HID_HostCloseDev` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032A31C` | `0x228` | `fn_8032A31C` | `HID_HostSetSecurityLevel` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x804AD8F0` | `0x144` | `lbl_804AD8F0` | `hd_cb` (`tHID_DEV_CTB`) | object | high name, medium file | F-524-1 | `src/RVL_SDK/bte/hidd_mgmt.c` |
| `0x804ADA38` | `0x404` | `lbl_804ADA38` | `hh_cb` (`tHID_HOST_CTB`) | object | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |

Layouts follow Petari and are confirmed by target accesses: `hd_cb` uses `conn` at 0x10 (timer parameter 0x2C), `idle_tle` 0x34, `pm_params` 0x4C, `curr_pm` 0x6A, `pm_ctrl_busy` 0x6E, `conn_substate` 0x6F, `final_pm` 0x70, `callback` 0xC4, L2CAP configs 0xC8 and 0x104 (0x3C each), `reg_flag` 0x140, `trace_level` 0x141. `hh_cb` has 16 0x34-byte devices, `sdp_busy` 0x380, `sdp_cback` 0x384, `p_sdp_db` 0x388, `sdp_rec` 0x38C (0x74), `reg_flag` 0x400, `trace_level` 0x401.
