# Issue 507: WUDHidHost.c and WUD debug_msg.c

## F-507-1: Unit boundaries and sections

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The block `0x8030FF50`–`0x80311314` holds three parts:

- the end of WUD.c;
- two complete WUD units;
- the start of gki_buffer.c, already split and matched under #508 ([F-508-1](508-bte-gki.md#f-508-1-unit-boundaries-and-sections)).

| Unit | Section ranges | Evidence |
| --- | --- | --- |
| `RVL_SDK/wud/WUD.c` (not split here) | `.text` up to `0x80310394` | `fn_8030FF50` (the `Unknown device is connected` report), `_WUDGetDevAddr` through `_WUDGetLinkNumber`, `WUDiGetDiscoverDevice`, `WUDSetDeviceHistory`, `WUDIsLatestDevice`, `WUDUpdateSCSetting` and the four per-handle accessors. They follow the order of Swiftshine/key's CC0 `WUD.c`. That unit starts in #505/#506, so its tail stays with that work |
| `RVL_SDK/wud/WUDHidHost.c` | `.text` `0x803103A0`–`0x80310988`; `.data` `0x803E0260`–`0x803E04F7` | `WUDHidHostCallback`, `bta_hh_co_data`, `bta_hh_co_open`, `bta_hh_co_close` and `bta_dm_co_get_compress_memory`. The `.data` range holds the BTA_HH event strings, `Nintendo RVL-CNT`, the switch jump table and the three callout strings, ending at the last string. gki_buffer.c data starts at `0x803E04F8` |
| `RVL_SDK/wud/debug_msg.c` | `.text` `0x80310990`–`0x803109E0` | `WUD_DEBUGPrint`, an empty variadic function. WUD.c and WUDHidHost.c call it throughout |

Both units compile with Wii/1.0 and `cflags_sdk` at default 16-byte alignment, like the WPAD units. Both link from source, and full DOL verification passes.

## F-507-2: Names

| Address | Name | Confidence |
| --- | --- | --- |
| `0x803103A0` | `WUDHidHostCallback` | Medium. Petari uses this name; Swiftshine/key's later revision calls it `WUDiHidHostEventCallback` |
| `0x80310920` | `bta_hh_co_data` | High (BTA callout) |
| `0x80310960` | `bta_hh_co_open` | High |
| `0x80310970` | `bta_hh_co_close` | High |
| `0x80310990` | `WUD_DEBUGPrint` | High (Petari, key) |

Callees in WUD.c are named for the link, with no change to sizes or splits. The names follow the order and bodies of Petari's and Swiftshine/key's CC0 `WUD.c`, and are high confidence:

| Address | Name |
| --- | --- |
| `0x8030D660` | `WUDSetSniffMode` |
| `0x8030E540` | `WUDiRemoveDevice` |
| `0x8030E6C0` | `WUDiGetDevInfo` |
| `0x8030E7A0` | `WUDiMoveTopSmpDevInfoPtr` |
| `0x8030E8C0` | `WUDiMoveBottomSmpDevInfoPtr` |
| `0x8030E9E0` | `WUDiMoveTopOfDisconnectedSmpDevice` |
| `0x8030EB50` | `WUDiMoveTopStdDevInfoPtr` |
| `0x8030EC70` | `WUDiMoveBottomStdDevInfoPtr` |
| `0x8030ED90` | `WUDiMoveTopOfDisconnectedStdDevice` |
| `0x803101E0` | `WUDiGetDiscoverDevice` |
| `0x80310320` | `WUDiSetDevAddrForHandle` |
| `0x80310340` | `WUDiGetDevAddrForHandle` |
| `0x80310360` | `WUDiSetQueueSizeForHandle` |
| `0x80310380` | `WUDiSetNotAckNumForHandle` |

`lbl_8047C320` becomes `_wcb` (0x750 bytes).

## F-507-3: Revision differences

This revision sits between Petari's (SMG) and Swiftshine/key's WUD:

- Like Petari, it keeps every `DEBUGPrint` call and string.
- Like key, it reaches `_work` and the per-handle tables only through the WUD.c accessors.
- Like key, it skips moving a standard device to the top when a Wii Balance Board is linked and the device name is not `Nintendo RVL-CNT`.
- Like key, the failed-open path looks up the discovering device before comparing addresses.

The switch cases follow the string table order:

1. enable, disable, open, close;
2. add device, remove device;
3. the custom buffer-status event 15;
4. the report, protocol, idle and descriptor events;
5. virtual unplug.

With this order the jump table and all string offsets match.

## Provenance

`src/RVL_SDK/wud/WUDHidHost.c` adapts Petari `e0ab141914921726a595573dc48d3f13d0922efe` `src/RVL_SDK/wud/WUDHidHost.c`. The file has no file-level notice, so Petari's root CC0 applies. The accessor calls and the Balance Board condition were reconstructed from the target disassembly. Swiftshine/key `7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2` (CC0) `src/revolution/WPAD/WUDHidHost.c` and `WUD.c` supplied the accessor names and confirmed the structure. `src/RVL_SDK/wud/debug_msg.c` follows Petari's `wud/debug_msg.c`.

## Traps

- **Petari's WUD is an older revision.** Copying its `WUDHidHostCallback` wrote the per-handle tables directly, but this target calls the WUD.c accessors at `0x80310320`–`0x80310380`. Check Swiftshine/key's newer WUD for the accessor layer, and keep Petari's debug strings.
