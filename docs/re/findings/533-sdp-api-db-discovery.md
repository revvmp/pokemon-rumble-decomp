# Issue 533: BTE SDP API, database and discovery units

## F-533-1: Unit boundaries and source for `sdp_api.c`, `sdp_db.c` and `sdp_discovery.c`

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), dtk 1.8.4 split assembly, compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/sdp_api.c`, `sdp_db.c` and `sdp_discovery.c`.

| File | `.text` | `.data` | Present functions |
| --- | --- | --- | --- |
| `sdp_api.c` | `0x80338B3C`-`0x803399A4` | `0x803E55E0`-`0x803E5630` (the `SDP_InitDiscoveryDb` diagnostic) | `SDP_InitDiscoveryDb`, `SDP_ServiceSearchRequest`, `SDP_ServiceSearchAttributeRequest`, `SDP_FindAttributeInRec`, `SDP_FindServiceInDb`, `SDP_FindServiceUUIDInDb`, `SDP_SetLocalDiRecord`, `SDP_GetLocalDiRecord`, `SDP_SetTraceLevel` |
| `sdp_db.c` | `0x803399A4`-`0x8033A670` | `0x803E5630`-`0x803E56C8` (three diagnostics) | `sdp_db_service_search`, `find_uuid_in_seq`, `sdp_db_find_record`, `sdp_db_find_attr_in_rec`, `SDP_CreateRecord`, `SDP_DeleteRecord`, `SDP_AddAttribute`, `SDP_AddUuidSequence`, `SDP_AddServiceClassIdList`, `SDP_DeleteAttribute` |
| `sdp_discovery.c` | `0x8033A670`-`0x8033B8E8` | `0x803E56C8`-`0x803E5824` (diagnostics and the `add_attr` jump table) | `sdpu_build_uuid_seq`, `sdp_snd_service_search_req`, `sdp_disc_connected`, `sdp_disc_server_rsp`, `process_service_search_rsp`, `process_service_attr_rsp`, `process_service_search_attr_rsp`, `save_attr_seq`, `add_record`, `add_attr` |

Names come from the callers: `SDP_SetLocalDiRecord` calls `0x80339C54` (`SDP_CreateRecord`) and then `0x8033A2B8` (`SDP_AddServiceClassIdList`) before its `SDP_AddAttribute` calls, and deletes through `0x80339D34` (`SDP_DeleteRecord`); `SDP_AddAttribute` calls `0x8033A434` (`SDP_DeleteAttribute`, which carries the `Deleting attr_id` diagnostic). `0x80339980` reads and conditionally writes `sdp_cb.trace_level`, which is `SDP_SetTraceLevel`. The data boundaries follow the string references: the RFCOMM strings of `rfc_utils.c` end before `0x803E55E0`, and `sdp_main.c` data starts at `0x803E5828`.

`SDP_FindAttributeInDb`, the protocol-list and profile-version finders, the device-identification discovery helpers, `SDP_AddSequence`, `SDP_AddProtocolList`, `SDP_AddAdditionProtoLists`, `SDP_AddProfileDescriptorList` and `SDP_AddLanguageBaseAttrIDList` have no code in the target and are not reconstructed.

Compiled with GC/3.0a3 (`BTE_COMPILER`, F-524-2 and F-534-2) and the library `cflags_sdk`, all 29 functions and all three `.data` sections score 100% in objdiff 3.8.2 at the first attempt. With the three objects `Matching`, `ninja` exits 0 and `tools/verify/verify.py verify` passes.

The first nine functions of the block, `0x8033868C`-`0x80338B3C`, are the tail of the RFCOMM `rfc_utils.c` (timer, close and credit routines with `rfc_*` diagnostics). That file starts in #532's block, so #532 owns its split; they are not handled here.

## Provenance

Sources and the declarations added to `include/revolution/bte/` are rewritten for this project after Petari's BTE files, which carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE SDP provenance](../licenses/534-bte-sdp-NOTICE.md), which also covers these files.

## Traps

- **Petari's SDP sources match without changes in control flow.** All 29 functions matched with Petari's statement order and loop forms; restructuring loops or conditions is unnecessary and only risks a mismatch. Keep the device-identification `memset` of `sizeof(tSDP_DI_RECORD)` in `SDP_GetLocalDiRecord`, which is shorter than the destination. Evidence: F-533-1.
