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

The first nine functions of the block, `0x8033868C`-`0x80338B3C`, are the tail of the RFCOMM `rfc_utils.c` (timer, close and credit routines with `rfc_*` diagnostics). That file starts in #532's block, so #532 owns its split; they were not handled by F-533-1. F-533-2 below verifies their subsequent completion by #532.

## Provenance

Sources and the declarations added to `include/revolution/bte/` are rewritten for this project after Petari's BTE files, which carry Broadcom Apache-2.0 notices and changes by muff1n1634; see [BTE SDP provenance](../licenses/534-bte-sdp-NOTICE.md), which also covers these files.

## Traps

- **Petari's SDP sources match without changes in control flow.** All 29 functions matched with Petari's statement order and loop forms; restructuring loops or conditions is unnecessary and only risks a mismatch. Keep the device-identification `memset` of `sizeof(tSDP_DI_RECORD)` in `SDP_GetLocalDiRecord`, which is shorter than the destination. Evidence: F-533-1.

## F-533-2: Complete block acceptance after the RFCOMM merge

PR #620 (`232b7e1`) supplied the three SDP units. PR #630 (`cb0229a`, #532) subsequently supplied all of `rfc_utils.c`, including the nine outstanding functions in #533. This audit verifies the combined source coverage instead of counting those existing implementations as new reconstructed bytes. All 32 functions and 9,076 code bytes listed in #533 are implemented in C, registered `Matching`, and linked from source. Their source object symbol sizes equal the issue's individual extents. The four complete units contain 44 retained functions; the twelve outside #533 remain attributed to their owning issues.

The issue's original Wii/1.0 compiler statement is superseded by the measured GC/3.0a3 `BTE_COMPILER` selection and existing library `cflags_sdk`. No compiler, source, header, split, or frozen fingerprint changes are needed.

| Address | Size | Recovered name | Source unit |
| --- | --- | --- | --- |
| `0x8033868C` | `0x80` | `rfc_port_timer_start` | `rfc_utils.c` |
| `0x8033870C` | `0x58` | `rfc_port_timer_stop` | `rfc_utils.c` |
| `0x80338764` | `0xC4` | `rfc_check_mcb_active` | `rfc_utils.c` |
| `0x80338828` | `0x3C` | `rfcomm_process_timeout` | `rfc_utils.c` |
| `0x80338864` | `0x54` | `rfc_sec_check_complete` | `rfc_utils.c` |
| `0x803388B8` | `0x158` | `rfc_port_closed` | `rfc_utils.c` |
| `0x80338A10` | `0x8C` | `rfc_inc_credit` | `rfc_utils.c` |
| `0x80338A9C` | `0x3C` | `rfc_dec_credit` | `rfc_utils.c` |
| `0x80338AD8` | `0x64` | `rfc_check_send_cmd` | `rfc_utils.c` |
| `0x80338B3C` | `0x378` | `SDP_InitDiscoveryDb` | `sdp_api.c` |
| `0x80338EB4` | `0x5C` | `SDP_ServiceSearchRequest` | `sdp_api.c` |
| `0x80338F10` | `0x64` | `SDP_ServiceSearchAttributeRequest` | `sdp_api.c` |
| `0x80338F74` | `0x28` | `SDP_FindAttributeInRec` | `sdp_api.c` |
| `0x80338F9C` | `0xCC` | `SDP_FindServiceInDb` | `sdp_api.c` |
| `0x80339068` | `0x108` | `SDP_FindServiceUUIDInDb` | `sdp_api.c` |
| `0x80339170` | `0x360` | `SDP_SetLocalDiRecord` | `sdp_api.c` |
| `0x803394D0` | `0x4B0` | `SDP_GetLocalDiRecord` | `sdp_api.c` |
| `0x80339980` | `0x24` | `SDP_SetTraceLevel` | `sdp_api.c` |
| `0x803399A4` | `0x13C` | `sdp_db_service_search` | `sdp_db.c` |
| `0x80339AE0` | `0xD8` | `find_uuid_in_seq` | `sdp_db.c` |
| `0x80339BB8` | `0x5C` | `sdp_db_find_record` | `sdp_db.c` |
| `0x80339C14` | `0x40` | `sdp_db_find_attr_in_rec` | `sdp_db.c` |
| `0x80339C54` | `0xE0` | `SDP_CreateRecord` | `sdp_db.c` |
| `0x80339D34` | `0x114` | `SDP_DeleteRecord` | `sdp_db.c` |
| `0x80339E48` | `0x398` | `SDP_AddAttribute` | `sdp_db.c` |
| `0x8033A1E0` | `0xD8` | `SDP_AddUuidSequence` | `sdp_db.c` |
| `0x8033A2B8` | `0x17C` | `SDP_AddServiceClassIdList` | `sdp_db.c` |
| `0x8033A434` | `0x23C` | `SDP_DeleteAttribute` | `sdp_db.c` |
| `0x8033A670` | `0xEC` | `sdpu_build_uuid_seq` | `sdp_discovery.c` |
| `0x8033A75C` | `0x130` | `sdp_snd_service_search_req` | `sdp_discovery.c` |
| `0x8033A88C` | `0x34` | `sdp_disc_connected` | `sdp_discovery.c` |
| `0x8033A8C0` | `0x140` | `sdp_disc_server_rsp` | `sdp_discovery.c` |

### String boundaries and comparison evidence

The original object grouped `rfc_port_timer_stop` and `rfc_port_closed` diagnostics in the data symbol at `0x803E55A4`. Split its final sixteen bytes at `0x803E55B8`, named `sRfcPortClosedMessage`. The first string occupies twenty bytes including alignment. In `sdp_discovery.c`, the old data symbol at `0x803E5718` grouped seven distinct diagnostics. Their word-aligned extents are `0x803E5718`-`0x803E5740`, `0x803E5740`-`0x803E5760`, `0x803E5760`-`0x803E5770`, `0x803E5770`-`0x803E57A0`, `0x803E57A0`-`0x803E57BC`, `0x803E57BC`-`0x803E57DC`, and `0x803E57DC`-`0x803E5800`. The source literals, target bytes, and references establish these boundaries. Names describe the diagnostics rather than claiming recovered original variable names.

Before the correction, objdiff 3.8.2 scored `rfc_utils.c` text at 99.99008% and `sdp_discovery.c` text at 99.966156%. The affected instructions had identical words but different synthesized `R_PPC_NONE` string targets/addends. These are analysis records, not linker relocations. Separating the actual strings restores 100% for every compared section in all four units without modifying any executable byte or weakening comparison settings.

Validation runs `python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja`, four whole-object `objdiff-cli diff -1 <original split object> -2 <compiled source object>` comparisons, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`. All exit 0; `.text`, `.data`, and `rfc_utils.c`'s `.rodata` compare at 100%. The source audit checks each of the 32 function sizes against the issue's original inventory. The full source-linked `main.dol` matches frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This accepts the main game executable only, excluding boot content 6, data contents, and WAD packaging. Final rebased verification is recorded in the pull request.

### Traps from the acceptance audit

- **A grouped diagnostic symbol can cause a false relocation discrepancy.** Identical instructions referencing a later string inside a grouped target symbol received synthetic `R_PPC_NONE` addends, whereas compiled source had separate literal symbols. Recover actual string boundaries from bytes and references; preserve default objdiff settings and verify the complete source-linked executable. This was confirmed in both units above, and does not justify dismissing other objdiff discrepancies.
