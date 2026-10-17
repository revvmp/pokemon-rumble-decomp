# WUD initialization and synchronization issue inventory

## F-505-1: Complete inventory accepted

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #505 owns 24 functions totaling 10,308 native bytes. Every entry falls inside the WUD translation unit whose first function belongs to #504. PR #719 reconstructs that complete unit; this issue records the independent scope inventory and final acceptance audit without adding duplicate source-byte credit.

Current acceptance: source prerequisite #719 and this complete inventory audit are integrated in the same change. All 24 original issue functions / 10,308 native bytes are accepted in actual source-selected Matching objects, with exact original extents, raw instruction streams, paired relocation offsets/types, final ELF addresses and direct frozen DOL equality. This completes the original issue inventory without duplicate source-byte credit.

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

## Final source-selected acceptance

After dropping the prerequisite branch history and rebasing onto main, `python configure.py`, full `.venv/Scripts/ninja.exe`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0. The independent complete original-inventory audit verifies all 24 functions / 10,308 bytes, every non-relocated instruction word, paired relocation offsets and types, actual source object selection without the corresponding original fallback objects, and every final ELF address and size. Direct full-DOL equality covers the complete resolved instruction and data operands. The frozen original and rebuilt DOL are 4,094,880 bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

No source, split, header or metadata selection is changed by this inventory acceptance. Its unit source and initialized-data proofs are recorded in the owning findings, including F-504-16 and F-504-17. The issue audit adds no duplicate unit-byte credit. WUD whole-unit fuzzy scores remain diagnostic as disclosed in F-504-16; exact native and resolved artifact comparisons are the acceptance evidence. The pull request records the final exact-head gate after its last rebase. Verification scope remains the main game executable, excluding the boot loader, other package contents and WAD packaging.

## Earlier source-prerequisite preview

The branch is stacked on source prerequisite #719 at commit c468b91d96c00fde5388f1bf17ea912b816294d5. A fresh default-path configure and full Ninja both exit 0. Explicit frozen verification of build/wpse-v1 exits 0. The complete issue inventory audit finds all 24 original functions / 10308 native bytes in actual source-selected objects, with original extents and final ELF addresses; every DOL byte agrees with the frozen reference.

This earlier preview is historical. The final source-selected acceptance above supersedes its pending main integration checkpoint; source-unit credit remains counted once.

## Provenance

The source prerequisite records pinned Petari and Swiftshine/key CC0 provenance, target operands and exact matching evidence in its F-504 findings. This audit adds no reference source or declarations.

## Traps

No new reusable trap was established. A fallback-selected frozen build is insufficient for issue acceptance; the final audit explicitly confirms source selection for every owned function.
