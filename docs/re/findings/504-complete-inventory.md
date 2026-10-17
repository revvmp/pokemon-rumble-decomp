# F-504-17: Complete original issue inventory accepted

Target: `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. All 40 functions in issue #504's original inventory, totaling 16,488 native bytes, are now reconstructed in normal source-selected Matching units. This completes the issue inventory. KPAD's prerequisite is the merged source PR #784 (`d82ab3ece969732bae3f1e5c954d77540a387a49`), following canonical header prerequisite #720.

## Inventory and normal source selection

| Source object | Original issue functions | Native bytes |
| --- | --- | --- |
| `RVL_SDK/kpad/KPAD.o` | 13 | 7,800 |
| `RVL_SDK/euart/euart.o` | 3 | 884 |
| `RVL_SDK/usb/usb.o` | 14 | 5,352 |
| `RVL_SDK/wud/WUD.o` | 10 | 2,452 |

The audit reads the original issue inventory, rather than counting functions introduced by this pull request. For every row it checks the recovered metadata name, original function size, compiled source extent, exact non-relocation instruction words, paired relocation offsets and types, and final ELF address and size. The actual normal link inputs select all four source objects and exclude their four original fallback objects. The complete resolved DOL is compared directly against the frozen original, so relocated operands and destinations are also covered without normalization.

| Original address | Size | Accepted symbol | Source object |
| --- | --- | --- | --- |
| `0x80306800` | `0x8D0` | `read_kpad_stick` | `KPAD.o` |
| `0x803070D0` | `0xC` | `KPADRead` | `KPAD.o` |
| `0x803070E0` | `0x72C` | `KPADiRead` | `KPAD.o` |
| `0x80307810` | `0xC` | `KPADInit` | `KPAD.o` |
| `0x80307820` | `0x444` | `KPADInitEx` | `KPAD.o` |
| `0x80307C70` | `0x164` | `KPADiConnectCallback` | `KPAD.o` |
| `0x80307DE0` | `0x58` | `KPADSetConnectCallback` | `KPAD.o` |
| `0x80307E40` | `0x18` | `KPADiBalanceControlCallback` | `KPAD.o` |
| `0x80307E60` | `0x18` | `KPADiBalanceCalibrationCallback` | `KPAD.o` |
| `0x80307E80` | `0x1C` | `KPADDisableDPD` | `KPAD.o` |
| `0x80307EA0` | `0x1C` | `KPADEnableDPD` | `KPAD.o` |
| `0x80307EC0` | `0x94` | `KPADiControlDpdCallback` | `KPAD.o` |
| `0x80307F60` | `0x768` | `KPADiSamplingCallback` | `KPAD.o` |
| `0x803086D0` | `0xFC` | `EUARTInit` | `euart.o` |
| `0x803087D0` | `0x48` | `InitializeUART` | `euart.o` |
| `0x80308820` | `0x230` | `WriteUARTN` | `euart.o` |
| `0x80308A50` | `0xA8` | `USB_LOG` | `usb.o` |
| `0x80308B00` | `0xAC` | `USB_ERR` | `usb.o` |
| `0x80308BB0` | `0xF8` | `IUSB_OpenLib` | `usb.o` |
| `0x80308CB0` | `0x8` | `IUSB_CloseLib` | `usb.o` |
| `0x80308CC0` | `0x19C` | `_intrBlkCtrlIsoCb` | `usb.o` |
| `0x80308E60` | `0x144` | `IUSB_OpenDeviceIds` | `usb.o` |
| `0x80308FB0` | `0x124` | `IUSB_CloseDeviceAsync` | `usb.o` |
| `0x803090E0` | `0x354` | `__LongBlkMsgInt` | `usb.o` |
| `0x80309440` | `0x358` | `__IntrBlkMsgInt` | `usb.o` |
| `0x803097A0` | `0x74` | `IUSB_ReadIntrMsgAsync` | `usb.o` |
| `0x80309820` | `0xA0` | `IUSB_ReadBlkMsgAsync` | `usb.o` |
| `0x803098C0` | `0xA0` | `IUSB_WriteBlkMsgAsync` | `usb.o` |
| `0x80309960` | `0x5A4` | `__CtrlMsgInt` | `usb.o` |
| `0x80309F10` | `0x8C` | `IUSB_WriteCtrlMsgAsync` | `usb.o` |
| `0x80309FA0` | `0x8` | `WUDIsLinkedWBC` | `WUD.o` |
| `0x80309FB0` | `0x50` | `App_MEMalloc` | `WUD.o` |
| `0x8030A000` | `0x50` | `App_MEMfree` | `WUD.o` |
| `0x8030A050` | `0x78` | `__wudSyncFlushCallback` | `WUD.o` |
| `0x8030A0D0` | `0x158` | `__wudSyncPrepareSearch` | `WUD.o` |
| `0x8030A230` | `0x170` | `__wudSyncTryConnect` | `WUD.o` |
| `0x8030A3A0` | `0x378` | `__wudSyncVirginStandard` | `WUD.o` |
| `0x8030A720` | `0x134` | `__wudSyncStoredDevInfoToNand` | `WUD.o` |
| `0x8030A860` | `0x7C` | `__wudOpenWiiFitCallback` | `WUD.o` |
| `0x8030A8E0` | `0x84` | `__wudSeekWiiFitCallback` | `WUD.o` |

## Fresh final integration proof

The integration checkpoint is source commit `ee041973`, rebased onto main `3ddfbf2cca21b84dbe4c8d4d5696251aabeae0ad`. The 23 earlier issue commits were consolidated with an interactive rebase before integrating current main; the consolidated tree was identical to verified snapshot `c468b91d96c00fde5388f1bf17ea912b816294d5`. The EUART, USB and WUD source bodies and their two owned headers remain unchanged from that snapshot.

Shared-file conflicts preserve main's full KPAD split and object line, every other library and source selection, and all unrelated symbol rows. Only the EUART, USB and WUD unit ranges, object lines and required symbol records are reapplied. Four WUD-owned callback names or scopes imported with the KPAD prerequisite are reconciled to the already verified source definitions. No other source uses the superseded aliases.

The fresh isolated build uses `build/504-final-after-kpad-784/wpse-v1`. `python configure.py --build-dir build/504-final-after-kpad-784`, full `.venv/Scripts/ninja.exe`, and the explicit frozen verifier of that actual build directory all exit 0. Direct comparison accepts all 4,094,880 original DOL bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The build's default-path SHA-1 check is supplemental; the explicit verifier checks the fresh isolated candidate itself.

The independent complete WUD audit reproduces all 86 native functions / 25,028 bytes, all 1,185 paired relocations, all 4,519 emitted initialized-data bytes, nine jump-table relocations, the final alignment byte, and complete BSS and small-data layouts. All 86 final WUD function addresses agree. The source-only message catalogs and standalone `WUDStartClearDevice` body are stripped. The historical 367-object equivalence proof remains scoped to the #720 baseline; unrelated later main additions are retained and verified in the fresh complete DOL.

EUART, USB and WUD together supply 103 retained native functions / 31,264 bytes. Their source is accepted once by this issue's source change, even where unit ownership spans #505 through #507. Those later issue audits verify their respective original inventories and add no duplicate unit-byte credit. KPAD's source credit belongs to prerequisite #784.

Whole-unit WUD objdiff remains a diagnostic: text 99.89692%, data 11.763171%, BSS/sdata/sbss 100%, sdata2 63.636364%. Coarse correspondence and source-only stripped objects explain the diagnostic limits documented in [F-504-16](504-wud-importer-match.md); acceptance uses exact raw instructions, all paired relocations and direct frozen full-artifact equality. No fuzzy score substitutes for those checks.

## Scope and traps

Verification covers the main game executable only, excluding the boot loader, other package contents and WAD packaging. No original hash, frozen manifest or expected byte changed. There is no new matching trap beyond those recorded in F-504-3 and F-504-16. Latest-main exact-head configure, Ninja and explicit frozen verification are repeated before merge; the pull request records that final commit and its results.
