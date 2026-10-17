# Issue 518: BTE discovery unit

Covers the `btm_discovery.c` part of issue #518 (`0x8031D914`-`0x8031EA7C`). The `btm_inq.c` functions of the block were matched in [F-519-1](519-btm-inq-main-pm.md#f-519-1-unit-boundaries). Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`), compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/bte/btm_discovery.c`.

## F-518-1: Unit boundaries

| Range | Unit | Owner | Evidence |
| --- | --- | --- | --- |
| `0x8031D914`-`0x8031DF18` | `btm_devctl.c` (tail) | #517 | `BTM_WritePageTimeout` to `btm_report_device_status`, with the `BTM: BTM_WritePageTimeout` to `BTM: BTM_DeleteStoredLinkKey` strings at `.data` `0x803E18C0`-`0x803E19C0`. The file starts in #517, which owns the split; not split here. |
| `0x8031DF18`-`0x8031E04C` | `btm_discovery.c` | #518 | `btm_discovery_db_init` points `btm_cb.p_btm_ddb` at the 4000-byte discovery area, sets the public browse group UUID filter and calls `SDP_InitDiscoveryDb`; `btm_discovery_db_reset` and the empty `btm_discovery_timeout` follow. `BTM_SetDiscoverability` at `0x8031E04C` starts `btm_inq.c`. The unit has no data. |
| `0x8031E04C`-`0x8031EA7C` | `btm_inq.c` (start) | #518 | Split and `Matching` since F-519-1 (PR #698). |

## F-518-2: Reconstruction

`btm_discovery.c` matches with GC/3.0a3 and `cflags_sdk` (`BTE_COMPILER`): all three functions are 100% in objdiff and the full DOL verifies with the unit linked from source. Petari's file compiles unchanged. `btm_disc_compl_cb`, `BTM_SetDiscoveryParams`, `BTM_StartDiscovery`, `BTM_FindAttribute` and `BTM_FindService` are unused and dead-stripped; the unit has no strings, so nothing shows they are absent from the SDK, and they are kept.

| Address | Size | Name | Evidence |
| --- | --- | --- | --- |
| `0x8031DF90` | `0xB8` | `btm_discovery_db_reset` | Stops the discovery timer, completes a pending discovery and calls `btm_discovery_db_init`; 100% match, high |

## Remaining work in #518

The nine `btm_devctl.c` functions in `0x8031D914`-`0x8031DF18` belong to the `btm_devctl.c` split, which starts in #517. Their complete delivery and the original issue's acceptance inventory are recorded in [F-518-3](518-btm-completion-audit.md).

## Provenance

Sources and headers are rewritten after Petari's BTE files; see [BTE provenance](../licenses/518-bte-NOTICE.md).

## Traps

- **Unused code can reference symbols that no longer exist.** `BTM_FindAttribute` calls `SDP_FindAttributeInDb`, which has no code in the target. The link still succeeds because the linker strips the caller first, so declaring the callee is enough.
