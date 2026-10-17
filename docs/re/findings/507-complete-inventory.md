# Complete WUD tail and GKI issue inventory

## F-507-4: Complete inventory accepted

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #507 owns 26 functions totaling 4,908 native bytes. Thirteen functions belong to the complete WUD source supplied by #719. The five WUDHidHost functions, WUD debug function and seven GKI buffer functions already link from source on main, as recorded in F-507-1 through F-507-3 and F-508. This audit adds no duplicate reconstructed source or byte credit.

Current acceptance: source prerequisite #719 and this complete inventory audit are integrated in the same change. All 26 original issue functions / 4,908 native bytes are accepted in actual source-selected Matching objects, with exact original extents, raw instruction streams, paired relocation offsets/types, final ELF addresses and direct frozen DOL equality. This completes the original issue inventory without duplicate source-byte credit.

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

## Final source-selected acceptance

After dropping the prerequisite branch history and rebasing onto main, `python configure.py`, full `.venv/Scripts/ninja.exe`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0. The independent complete original-inventory audit verifies all 26 functions / 4,908 bytes, every non-relocated instruction word, paired relocation offsets and types, actual source object selection without the corresponding original fallback objects, and every final ELF address and size. Direct full-DOL equality covers the complete resolved instruction and data operands. The frozen original and rebuilt DOL are 4,094,880 bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

No source, split, header or metadata selection is changed by this inventory acceptance. Its unit source and initialized-data proofs are recorded in the owning findings, including F-504-16 and F-504-17. The issue audit adds no duplicate unit-byte credit. WUD whole-unit fuzzy scores remain diagnostic as disclosed in F-504-16; exact native and resolved artifact comparisons are the acceptance evidence. The pull request records the final exact-head gate after its last rebase. Verification scope remains the main game executable, excluding the boot loader, other package contents and WAD packaging.

## Earlier source-prerequisite preview

The branch is stacked on source prerequisite #719 at commit c468b91d96c00fde5388f1bf17ea912b816294d5. A fresh default-path configure and full Ninja both exit 0. Explicit frozen verification of build/wpse-v1 exits 0. The complete issue inventory audit finds all 26 original functions / 4908 native bytes in actual source-selected objects, with original extents and final ELF addresses; every DOL byte agrees with the frozen reference.

This earlier preview is historical. The final source-selected acceptance above supersedes its pending main integration checkpoint; source-unit credit remains counted once.

## Provenance

The prerequisite and existing findings record pinned Petari and Swiftshine/key CC0 source for WUD. GKI retains the Broadcom Apache-2.0 notice documented in the #508 license record. This audit introduces no reused source or declarations.

## Traps

No new reusable trap was established. The final audit explicitly checks that the WUD tail and the three already reconstructed companion units all select source objects, with no corresponding original fallback.
