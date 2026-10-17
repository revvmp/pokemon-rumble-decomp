# WUD lifecycle and device-management issue inventory

## F-506-1: Complete inventory accepted

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #506 owns 39 functions totaling 11,280 native bytes. Every entry falls inside the WUD translation unit whose first function belongs to #504. PR #719 reconstructs that complete unit; this issue records the independent scope inventory and final acceptance audit without adding duplicate source-byte credit.

Current acceptance: source prerequisite #719 and this complete inventory audit are integrated in the same change. All 39 original issue functions / 11,280 native bytes are accepted in actual source-selected Matching objects, with exact original extents, raw instruction streams, paired relocation offsets/types, final ELF addresses and direct frozen DOL equality. This completes the original issue inventory without duplicate source-byte credit.

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

## Final source-selected acceptance

After dropping the prerequisite branch history and rebasing onto main, `python configure.py`, full `.venv/Scripts/ninja.exe`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0. The independent complete original-inventory audit verifies all 39 functions / 11,280 bytes, every non-relocated instruction word, paired relocation offsets and types, actual source object selection without the corresponding original fallback objects, and every final ELF address and size. Direct full-DOL equality covers the complete resolved instruction and data operands. The frozen original and rebuilt DOL are 4,094,880 bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

No source, split, header or metadata selection is changed by this inventory acceptance. Its unit source and initialized-data proofs are recorded in the owning findings, including F-504-16 and F-504-17. The issue audit adds no duplicate unit-byte credit. WUD whole-unit fuzzy scores remain diagnostic as disclosed in F-504-16; exact native and resolved artifact comparisons are the acceptance evidence. The pull request records the final exact-head gate after its last rebase. Verification scope remains the main game executable, excluding the boot loader, other package contents and WAD packaging.

## Earlier source-prerequisite preview

The branch is stacked on source prerequisite #719 at commit c468b91d96c00fde5388f1bf17ea912b816294d5. A fresh default-path configure and full Ninja both exit 0. Explicit frozen verification of build/wpse-v1 exits 0. The complete issue inventory audit finds all 39 original functions / 11280 native bytes in actual source-selected objects, with original extents and final ELF addresses; every DOL byte agrees with the frozen reference.

This earlier preview is historical. The final source-selected acceptance above supersedes its pending main integration checkpoint; source-unit credit remains counted once.

## Provenance

The source prerequisite records pinned Petari and Swiftshine/key CC0 provenance, target operands and exact matching evidence in its F-504 findings. This audit adds no reference source or declarations.

## Traps

No new reusable trap was established. A fallback-selected frozen build is insufficient for issue acceptance; the final audit explicitly confirms source selection for every owned function.
