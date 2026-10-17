# Issue 522: BTE core init, GAP and HCI command units

Covers issue #522 (`0x803255F4`–`0x80327880`), all of #523 (`0x80327880`–`0x80328878`), and the `hcicmds.c` tail of #524 (`0x80328878`–`0x80329258`). Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/`.

## F-522-1: Unit boundaries and names

| Range | Unit | Functions | Evidence |
| --- | --- | --- | --- |
| `0x803255F4`–`0x8032582C` | `btu_hcif.c` (tail) | `btu_hcif_cmd_timeout`, and the out-of-line `btu_hcif_link_key_notification_evt` at `0x8032574C` | `0x803255F4` uses the strings `Cmd timeout; no cmd in queue` and `BTU HCI command timeout - cmd opcode = 0x%02x`. The file starts in #521, which owns its split; left unsplit here. |
| `0x8032582C`–`0x803258A4` | `btu_init.c` | `btu_init_core`, `BTE_Init` | Calls `btm_init`, `l2c_init`, `sdp_init`, `btm_discovery_db_init`; `BTE_Init` clears the 0x84-byte `btu_cb` and stores `0x2A4` and `1`. `.sdata2` `0x804BB630` holds `BT_BD_ANY` (six `0xFF` bytes), referenced from the `bta_dm_act.c` and `hidh_conn.c` regions. |
| `0x803258A4`–`0x80325984` | `wbt_ext.c` | `WBT_ExtCreateRecord` | Device ID record setup through `SDP_GetLocalDiRecord`, `SDP_SetLocalDiRecord`, `SDP_AddUuidSequence` (`0x1002`) and `SDP_AddAttribute` (`0x8001`). |
| `0x80325984`–`0x803259E4` | `gap_api.c` | `GAP_Init` | Clears `gap_cb` (0x3AC bytes at `.bss` `0x804AD540`), stores `gap_btm_cback0/1`, trace level 5, calls `gap_conn_init`. |
| `0x803259E4`–`0x80326520` | `gap_conn.c` | `gap_conn_init` and the seven L2CAP callbacks it registers | `gap_conn_init` stores the seven callback addresses; strings `GAP_CONN - Rcvd L2CAP ...` at `.data` `0x803E29B0`–`0x803E2A80`, `*******` at `.sdata` `0x804B8F18`. |
| `0x80326520`–`0x80326B3C` | `gap_utils.c` | `btm_cback`, `gap_btm_cback0`, `gap_btm_cback1`, `gap_find_addr_name_cb`, `gap_find_addr_inq_cb`, `gap_convert_btm_status` | `GAP Discovery`, `GAP: FindAddrByName` strings and five jump tables at `.data` `0x803E2A80`–`0x803E2D70`; defines `gap_cb`. |
| `0x80326B3C`–`0x80329258` | `hcicmds.c` | 59 `btsnd_hcic_*` builders, `btsnd_hcic_inquiry` to `btsnd_hcic_vendor_spec_cmd` | Every function writes an HCI opcode into a GKI buffer and calls `btu_hcif_send_cmd` (`0x80324B88`); the opcodes appear in Petari source order. `.rodata` `0x80398160` is the 10-byte AFH channel map initializer of `btsnd_hcic_set_afh_channels`. |

`hcicmds.c` therefore spans the whole of #523 and the first 15 functions of the #524 block. The survey's estimated block boundaries at `0x80327880` and `0x80328878` fall inside it.

Names come from Petari. Opcode-to-name mapping was confirmed by compiling Petari's builders and comparing every instruction; `btsnd_hcic_switch_role` (`0x80327E7C`, opcode `0x080B`) follows `btsnd_hcic_exit_park_mode` because `btsnd_hcic_qos_setup` is dead-stripped.

## F-522-2: Compiler

All six units match with GC/3.0a3 and `cflags_sdk` (`BTE_COMPILER`, as in F-524-2): all 77 functions match instruction for instruction in objdiff and in a direct object comparison, and the full DOL verifies with all six units linked from source.

## F-522-3: Dead-stripped code and what the data proves

- `gap_conn.c`: compiling Petari's whole file gives 0x197 bytes of `.data`; the target has 0xCB bytes plus padding. The `GAP_Conn*` API and its strings are therefore absent from this SDK's file, so only `gap_conn_init`, the callbacks, `gap_find_ccb_by_cid` and `gap_release_ccb` (both inlined) are reconstructed.
- `gap_utils.c`: the target `.data` keeps the `GAP Inquiry Results Callback` strings although no code references them, so `gap_inq_results_cb` is reconstructed and dead-stripped by the linker. `gap_free_cb` and `gap_find_local_addr_by_name` must stay defined: removing them stops their inlining into `btm_cback` and `gap_find_addr_inq_cb`. `gap_allocate_cb` and `gap_is_service_busy` leave no trace and are omitted.
- `btu_init.c` `BTU_AclPktSize`, `wbt_ext.c` `WBT_ExtAddPinCode` and `wbt_sdp_show_ext`, the rest of `gap_api.c`, and 66 of Petari's 125 HCI builders have no code or data in the target and are not reconstructed.

## F-522-4: External names

To link the units from source, these outside symbols were given Petari names in `symbols.txt`, matched by the call sites in the verified functions: `GKI_getpoolbuf` `0x80311070`, `GKI_freebuf` `0x80311160`, `GKI_enqueue` `0x80311590`, `GKI_dequeue` `0x80311810`, `btm_discovery_db_init` `0x8031DF18`, `BTM_ReadRemoteDeviceName` `0x8031E91C`, `BTM_InqDbFirst` `0x8031EB3C`, `BTM_InqDbNext` `0x8031EBF4`, `btm_init` `0x8031FAE4`, `btu_hcif_send_cmd` `0x80324B88`, `L2CA_Deregister` `0x8032C720`, `L2CA_ConnectRsp` `0x8032C9E4`, `L2CA_ConfigReq` `0x8032CB74`, `L2CA_ConfigRsp` `0x8032CC2C`, `L2CA_DisconnectReq` `0x8032CCFC`, `L2CA_DisconnectRsp` `0x8032CDA0`, `l2c_init` `0x8032F7B8`, `SDP_SetLocalDiRecord` `0x80339170`, `SDP_GetLocalDiRecord` `0x803394D0`, `SDP_AddAttribute` `0x80339E48`, `SDP_AddUuidSequence` `0x8033A1E0`, `sdp_init` `0x8033B8E8`, `memcmp` `0x803456E8`, and `btu_cb` `.bss` `0x804AA850`. Their splits and sizes are left to the owning issues. `GKI_freebuf` and `memcmp` are two of the names F-524-3 was waiting for.

## F-522-5: btu_hcif.c tail now Matching

This supersedes the unsplit `btu_hcif.c` row of [F-522-1](#f-522-1-unit-boundaries-and-names). [F-521-1](521-bte-btu-hcif.md#f-521-1-unit-boundaries) splits `btu_hcif.c` as `0x803245D0`–`0x8032582C` and registers it `Matching`; `btu_hcif_cmd_timeout` (`0x803255F4`) and `btu_hcif_link_key_notification_evt` (`0x8032574C`) are 100% in objdiff and the full DOL verifies with the unit linked from source. The whole #522 range `0x803255F4`–`0x80327880` is now covered by `Matching` units: the `btu_hcif.c` tail, `btu_init.c`, `wbt_ext.c`, the three GAP units and the start of `hcicmds.c`.

## Provenance

Sources and headers are rewritten after Petari's BTE files; see [BTE provenance](../licenses/522-bte-NOTICE.md).

## Traps

- **An enum constant and a macro constant can compile differently.** `gap_congestion_ind` selects `GAP_EVT_CONN_CONGESTED` or `GAP_EVT_CONN_UNCONGESTED` with `?:`. With the event values as enumerators the function was 4 bytes short and scheduled differently; as plain `#define` integers, as Petari effectively has them, it matches. See [F-522-3](#f-522-3-dead-stripped-code-and-what-the-data-proves) for the unit.
- **Inline-only helpers must stay in the source.** Dropping the dead-stripped `gap_free_cb` and `gap_find_local_addr_by_name` from `gap_utils.c` changed `btm_cback` and `gap_find_addr_inq_cb`, because the compiler could no longer inline them. Keep a stripped function when a surviving function inlined it.
- **Two BTE header trees exist.** `include/RVL_SDK/bte/` (from #524) and `include/revolution/bte/` (from #510 to #514) declare overlapping types. These units include only `include/revolution/bte/`, which this change extends; do not mix the two trees in one unit.
- **Splitting moves the asm.** After a unit is split, dtk writes its assembly to `build/wpse-v1/asm/RVL_SDK/bte/<unit>.s` instead of the `auto_*` file; scripts that read the old file miss those addresses.
