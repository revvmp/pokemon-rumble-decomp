# Complete WUD tail and GKI issue inventory

## F-507-4: Complete inventory and source prerequisite

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #507 owns 26 functions totaling 4,908 native bytes. Thirteen functions belong to the complete WUD source supplied by #719. The five WUDHidHost functions, WUD debug function and seven GKI buffer functions already link from source on main, as recorded in F-507-1 through F-507-3 and F-508. This audit adds no duplicate reconstructed source or byte credit.

Current checkpoint: #719 remains unmerged, and main still selects the original WUD object. Final issue acceptance is pending. After that prerequisite, rebase onto latest main and run configure/Ninja/frozen verification plus an independent native inventory and source-selection audit for all four units.

| Address | Native extent | Reconstructed name | Source unit |
| --- | --- | --- | --- |
| `0x8030FF50` | `0x12C` | `__wudPowerMangeEventStackCallback` | `RVL_SDK/wud/WUD.c` |
| `0x80310080` | `0x54` | `_WUDGetDevAddr` | `RVL_SDK/wud/WUD.c` |
| `0x803100E0` | `0x58` | `_WUDGetQueuedSize` | `RVL_SDK/wud/WUD.c` |
| `0x80310140` | `0x58` | `_WUDGetNotAckedSize` | `RVL_SDK/wud/WUD.c` |
| `0x803101A0` | `0x3C` | `_WUDGetLinkNumber` | `RVL_SDK/wud/WUD.c` |
| `0x803101E0` | `0xC` | `WUDiGetDiscoverDevice` | `RVL_SDK/wud/WUD.c` |
| `0x803101F0` | `0x74` | `WUDSetDeviceHistory` | `RVL_SDK/wud/WUD.c` |
| `0x80310270` | `0x54` | `WUDIsLatestDevice` | `RVL_SDK/wud/WUD.c` |
| `0x803102D0` | `0x4C` | `WUDUpdateSCSetting` | `RVL_SDK/wud/WUD.c` |
| `0x80310320` | `0x14` | `WUDiSetDevAddrForHandle` | `RVL_SDK/wud/WUD.c` |
| `0x80310340` | `0x14` | `WUDiGetDevAddrForHandle` | `RVL_SDK/wud/WUD.c` |
| `0x80310360` | `0x14` | `WUDiSetQueueSizeForHandle` | `RVL_SDK/wud/WUD.c` |
| `0x80310380` | `0x14` | `WUDiSetNotAckNumForHandle` | `RVL_SDK/wud/WUD.c` |
| `0x803103A0` | `0x580` | `WUDHidHostCallback` | `RVL_SDK/wud/WUDHidHost.c` |
| `0x80310920` | `0x3C` | `bta_hh_co_data` | `RVL_SDK/wud/WUDHidHost.c` |
| `0x80310960` | `0x10` | `bta_hh_co_open` | `RVL_SDK/wud/WUDHidHost.c` |
| `0x80310970` | `0x10` | `bta_hh_co_close` | `RVL_SDK/wud/WUDHidHost.c` |
| `0x80310980` | `0x8` | `bta_dm_co_get_compress_memory` | `RVL_SDK/wud/WUDHidHost.c` |
| `0x80310990` | `0x50` | `WUD_DEBUGPrint` | `RVL_SDK/wud/debug_msg.c` |
| `0x803109E0` | `0x220` | `gki_init_free_queue` | `RVL_SDK/bte/gki_buffer.c` |
| `0x80310C00` | `0x2AC` | `gki_buffer_init` | `RVL_SDK/bte/gki_buffer.c` |
| `0x80310EB0` | `0x14` | `GKI_init_q` | `RVL_SDK/bte/gki_buffer.c` |
| `0x80310ED0` | `0x1A0` | `GKI_getbuf` | `RVL_SDK/bte/gki_buffer.c` |
| `0x80311070` | `0xE8` | `GKI_getpoolbuf` | `RVL_SDK/bte/gki_buffer.c` |
| `0x80311160` | `0x170` | `GKI_freebuf` | `RVL_SDK/bte/gki_buffer.c` |
| `0x803112D0` | `0x44` | `GKI_get_buf_size` | `RVL_SDK/bte/gki_buffer.c` |

The WUD unit starts in #504 and ends at `0x80310394`. Its split, configuration and header remain with that first-function issue. WUDHidHost and WUD debug ownership remains with #507; GKI buffer belongs to #508, whose whole-unit source covers the final seven entries here. No splits, headers, layouts or symbol extents change in this audit.

## Provenance

The prerequisite and existing findings record pinned Petari and Swiftshine/key CC0 source for WUD. GKI retains the Broadcom Apache-2.0 notice documented in the #508 license record. This audit introduces no reused source or declarations.

## Traps

No new reusable trap is established at this inventory checkpoint. The existing source units do not make the issue complete while its WUD tail still selects an original fallback.
