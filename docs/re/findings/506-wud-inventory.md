# WUD lifecycle and device-management issue inventory

## F-506-1: Complete inventory and source dependency

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #506 owns 39 functions totaling 11,280 native bytes. Every entry falls inside the WUD translation unit whose first function belongs to #504. PR #719 reconstructs that complete unit; this issue records the independent scope inventory and final acceptance audit without adding duplicate source-byte credit.

Current checkpoint: the source prerequisite remains unmerged. The normal main build still links the original WUD object. This issue is not complete, and no matching or source-link acceptance is claimed here. Final acceptance requires #719, rebase onto latest main, configure/Ninja/frozen verification and an audit of every function below in the actual source-selected object and final ELF.

| Address | Native extent | Reconstructed name from #719 |
| --- | --- | --- |
| `0x8030D240` | `0x11C` | `WUDInit` |
| `0x8030D360` | `0x4C` | `WUDRegisterAllocator` |
| `0x8030D3B0` | `0x220` | `WUDShutdown` |
| `0x8030D5D0` | `0x3C` | `WUDGetStatus` |
| `0x8030D610` | `0x48` | `WUDGetBufferStatus` |
| `0x8030D660` | `0x6C` | `WUDSetSniffMode` |
| `0x8030D6D0` | `0x5C` | `WUDSetSyncSimpleCallback` |
| `0x8030D730` | `0x14C` | `__wudStartSyncDevice` |
| `0x8030D880` | `0x14` | `WUDStartFastSyncSimple` |
| `0x8030D8A0` | `0x90` | `WUDCancelSyncDevice` |
| `0x8030D930` | `0x88` | `WUDStopSyncSimple` |
| `0x8030D9C0` | `0xE8` | `WUDSetDisableChannel` |
| `0x8030DAB0` | `0x5C` | `WUDSetHidRecvCallback` |
| `0x8030DB10` | `0x5C` | `WUDSetHidConnCallback` |
| `0x8030DB70` | `0x58` | `WUDSetVisibility` |
| `0x8030DBD0` | `0x30` | `__wudModuleRebootCallback` |
| `0x8030DC00` | `0xF0` | `__wudInstallPatchCallback` |
| `0x8030DCF0` | `0x254` | `__wudWritePatchCallback` |
| `0x8030DF50` | `0x1B4` | `__wudRemovePatchCallback` |
| `0x8030E110` | `0x5C` | `__wudSuperPeekPokeCallback` |
| `0x8030E170` | `0xFC` | `__wudAppendRuntimePatch` |
| `0x8030E270` | `0x170` | `__wudInitSub` |
| `0x8030E3E0` | `0x154` | `WUDiRegisterDevice` |
| `0x8030E540` | `0x180` | `WUDiRemoveDevice` |
| `0x8030E6C0` | `0xD8` | `WUDiGetDevInfo` |
| `0x8030E7A0` | `0x114` | `WUDiMoveTopSmpDevInfoPtr` |
| `0x8030E8C0` | `0x114` | `WUDiMoveBottomSmpDevInfoPtr` |
| `0x8030E9E0` | `0x168` | `WUDiMoveTopOfDisconnectedSmpDevice` |
| `0x8030EB50` | `0x114` | `WUDiMoveTopStdDevInfoPtr` |
| `0x8030EC70` | `0x114` | `WUDiMoveBottomStdDevInfoPtr` |
| `0x8030ED90` | `0x190` | `WUDiMoveTopOfDisconnectedStdDevice` |
| `0x8030EF20` | `0x198` | `WUDiMoveTopOfUnusedStdDevice` |
| `0x8030F0C0` | `0x74` | `WUDIsBusy` |
| `0x8030F140` | `0x34` | `__wudCleanupStackCallback` |
| `0x8030F180` | `0x554` | `__wudSecurityEventStackCallback` |
| `0x8030F6E0` | `0x208` | `__wudSearchEventStackCallback` |
| `0x8030F8F0` | `0x2A8` | `WUDVendorSpecificCallback` |
| `0x8030FBA0` | `0x50` | `__wudDeviceStatusEventStackCallback` |
| `0x8030FBF0` | `0x354` | `__wudLinkKeyEventStackCallback` |

The target's source boundary continues through `0x80310394`; the survey end `0x8030FF44` is only this issue's last function end. The unit's split, configuration and header remain with #504. The issue's 39 functions include startup and shutdown, synchronization controls, callbacks, device registration and list maintenance, firmware installation, security, discovery and stored-link-key reconciliation.

## Provenance

The source prerequisite records pinned Petari and Swiftshine/key CC0 provenance, target operands and exact matching evidence in its F-504 findings. This audit adds no reference source or declarations.

## Traps

No new reusable trap is established at this inventory checkpoint. A fallback-selected frozen build is insufficient for issue acceptance; final source selection remains a required check.
