# WUD initialization and synchronization issue inventory

## F-505-1: Complete inventory and source dependency

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #505 owns 24 functions totaling 10,308 native bytes. Every entry falls inside the WUD translation unit whose first function belongs to #504. PR #719 reconstructs that complete unit; this issue records the independent scope inventory and final acceptance audit without adding duplicate source-byte credit.

Current checkpoint: the source prerequisite remains unmerged. The normal main build still links the original WUD object. This issue is not complete, and no matching or source-link acceptance is claimed here. Final acceptance requires #719, rebase onto latest main, configure/Ninja/frozen verification and an audit of every function below in the actual source-selected object and final ELF.

| Address | Native extent | Reconstructed name from #719 |
| --- | --- | --- |
| `0x8030A970` | `0x7C` | `__wudUpdateWiiFitCallback` |
| `0x8030A9F0` | `0x54` | `__wudCloseWiiFitCallback` |
| `0x8030AA50` | `0x168` | `__wudSyncDone` |
| `0x8030ABC0` | `0xAE4` | `__wudSyncHandler` |
| `0x8030B6B0` | `0x20` | `__wudSyncHandler0` |
| `0x8030B6D0` | `0x58` | `__wudDeleteFlushCallback` |
| `0x8030B730` | `0x98` | `WUDiTerminateDevice` |
| `0x8030B7D0` | `0x204` | `__wudDeleteCleanupDatabase` |
| `0x8030B9E0` | `0x350` | `__wudDeleteHandler` |
| `0x8030BD30` | `0x20` | `__wudDeleteHandler0` |
| `0x8030BD50` | `0x10C` | `__wudStackCheckDeviceInfo` |
| `0x8030BE60` | `0x144` | `__wudStackHandler` |
| `0x8030BFB0` | `0x20` | `__wudEnableStackHandler0` |
| `0x8030BFD0` | `0x4C` | `__wudInitFlushCallback` |
| `0x8030C020` | `0x638` | `__wudInitDevInfo` |
| `0x8030C660` | `0x80` | `__wudNandResultCallback` |
| `0x8030C6E0` | `0x18` | `__wudNandFlushCallback` |
| `0x8030C700` | `0x530` | `__wudGetDevInfoFromWiiFit` |
| `0x8030CC30` | `0x114` | `__wudInitHandler` |
| `0x8030CD50` | `0x20` | `__wudInitHandler0` |
| `0x8030CD70` | `0x4C` | `__wudShutdownFlushCallback` |
| `0x8030CDC0` | `0xB4` | `__wudShutdownHandler` |
| `0x8030CE80` | `0x20` | `__wudShutdownHandler0` |
| `0x8030CEA0` | `0x394` | `__wudClearControlBlock` |

The target's source boundary continues through `0x80310394`; the survey end `0x8030D234` is only this issue's last function end. The unit's split, configuration and header remain with #504. The issue's 24 functions include synchronization and deletion dispatchers, settings flush callbacks, stack initialization, native device import, NAND recovery and control-block clearing.

## Provenance

The source prerequisite records pinned Petari and Swiftshine/key CC0 provenance, target operands and exact matching evidence in its F-504 findings. This audit adds no reference source or declarations.

## Traps

No new reusable trap is established at this inventory checkpoint. A fallback-selected frozen build is insufficient for issue acceptance; final source selection remains a required check.
