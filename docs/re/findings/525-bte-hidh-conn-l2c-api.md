# Issue 525: BTE HID host connection and L2CAP API

## F-525-1: unit boundaries

| Range | Unit | Contents |
| --- | --- | --- |
| `.text 0x8032A544`–`0x8032C584`; `.rodata 0x80398170`–`0x80398198`; `.data 0x803E2ED8`–`0x803E32C0` | `hidh_conn.c` | 16 functions from `hidh_conn_reg` to `hidh_conn_dereg`; `hst_reg_info` L2CAP registration table; string pool |
| `.text 0x8032C584`–`0x8032D150`; `.data 0x803E32C0`–`0x803E3A50` | `l2c_api.c` | 13 linked functions from `L2CA_Register` to `L2CA_RegisterCompression`; string pool |

`l2c_api.c` ends after the issue's estimated boundary (`0x8032CF68`). The four functions at `0x8032CF68`–`0x8032D150` belong to the same unit and are reconstructed here. The issue for that range (#526) starts its own unit at `l2c_csm_execute` (`0x8032D150`). `L2CA_Ping`, `L2CA_SetDesireRole`, `L2CA_SetAclPriority`, `L2CA_SetCompression`, `L2CA_Flush` and `L2CA_GetNumQueuedBufs` are dead-stripped, but their strings remain in the target `.data` pool, in source order. They stay in source so that the pool matches. `L2CA_RegisterCompression` is an empty function that `bta_dm_act.c` calls at `0x80314B34`.

`hidh_conn_snd_data` and `find_conn_by_cid` are inlined. The three non-static functions `hidh_sec_check_complete_term`, `hidh_proc_repage_timeout` and `hidh_sec_check_complete_orig` are global in the object.

## F-525-2: compiler and code-generation details

Both units build with GC/3.0a3 (`BTE_COMPILER`) under `cflags_sdk`, as F-524 found for the other HID units. objdiff reports 16/16 functions for `hidh_conn.c` and 13/13 for `l2c_api.c`, all at 100%. The source-linked `ninja` build exits 0 and frozen-manifest verification of `main.dol` passes.

- **Security channel argument.** Petari's `attr_mask & HID_SEC_REQUIRED ? HID_SEC_CHN : HID_NOSEC_CHN` produced a branch, but the target computes `2 - bit` without one (`rlwinm`, `neg`, `addi`). Unsigned channel constants (`#define HID_SEC_CHN 1U`, `HID_NOSEC_CHN 2U`) reproduce the target in both call sites. The constants only reach `UINT32` parameters elsewhere, and `hidh_api.c` still matches.
- **Undeclared helper return type.** Without a declaration, `btm_get_acl_disc_reason_code` returns an implicit `int`, and `hidh_l2cif_disconnect_ind` truncated the value when it was stored. Declaring it `UINT16` in `btm_int.h` reproduces the target, which truncates the value only where it is compared. The target function at `0x8031B0E8` loads a byte from `btm_cb` and returns it; it is named in `symbols.txt` for the link.
- **Missing macros.** Petari's `HID_BUILD_HDR`, `HID_GET_TRANS_FROM_HDR` and `HID_GET_PARAM_FROM_HDR` macros were missing from the headers. They compiled as implicit function calls instead of failing. Check the undefined symbols in each object (`nm -u`) for names that should be macros.

## Provenance

The source adapts Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, files `src/RVL_SDK/bte/hidh_conn.c` and `l2c_api.c`. Both files carry Broadcom Apache-2.0 notices; see [the issue 525 notice](../licenses/525-bte-NOTICE.md). The adaptation:

- removes comments and Petari's configurable trace macros, replacing them with fixed-level `HIDH_TRACE_*` and `L2CAP_TRACE_*` macros;
- drops a disabled `#if 0` copy of `hidh_conn_snd_data`;
- replaces empty `#if 0` placeholders in the compression functions;
- names the unnamed placeholder parameters `reserved`.

## Traps

- C implicit declarations hide missing macros and prototypes in this compiler. A macro absent from a header still compiles, as a call to an external function. An undeclared function returns `int`, which changes register truncation. Before scoring, list each object's undefined symbols and check that every one is declared or intended.
- Issue range estimates can split a source unit. Here `l2c_api.c` crosses into #526; inspect the next functions' strings and the next source unit's first function before trusting the issue boundary.
