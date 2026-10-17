# Issue 501: RVL_SDK WPAD.c (second block)

## F-501-1: Function identification and names

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The block `0x802F8C00`–`0x802FE620` holds 54 WPAD.c functions, and the three accessors `0x802FE620`–`0x802FE648` that F-500-1 placed in WPAD.c complete the file. All 57 are named in `config/wpse-v1/symbols.txt`. Static functions are `scope:local`.

The identification uses the dtk 1.8.4 assembly in `build/wpse-v1/asm/RVL_SDK/wpad/WPAD.s`: callees, control block offsets (F-500-2), report IDs and lengths pushed onto the command queue, and the `.data` and `.sdata` strings in address order. Names and order follow robojumper's sdk_2009-12-11 reconstruction (unlicensed; read only as a naming and ordering aid) and Petari's CC0 `src/RVL_SDK/wpad/WPAD.c` at `e0ab141`, whose older bodies supplied the structure.

| Address | Name | Evidence |
| --- | --- | --- |
| `0x802F8C00` | `WPADInit` | Sets `_wpadStartup`, registers `ShutdownFunctionInfo`, calls `WUDInit` and `__wpadInitSub` |
| `0x802F8C70` | `WPADStartFastSimpleSync` | Tail branch to `WUDStartFastSyncSimple` |
| `0x802F8C80` | `WPADStopSimpleSync` | Tail branch to `WUDStopSyncSimple` |
| `0x802F8C90` | `WPADSetSimpleSyncCallback` | Tail branch to `WUDSetSyncSimpleCallback` |
| `0x802F8CA0` | `WPADRegisterAllocator` | Tail branch to `WUDRegisterAllocator` |
| `0x802F8CB0` | `WPADGetStatus` | Tail branch to `WUDGetStatus` |
| `0x802F8D10` | `WPADGetSensorBarPosition` | Reads `_wpadSensorBarPos` under disabled interrupts |
| `0x802F8D50` | `__wpadSetupConnectionCallback` | Sets `setup` and calls the connect callback on success, otherwise disconnects; last read callback of the init sequence |
| `0x802F8E20` | `__wpadAbortConnectionCallback` | Clears the command queue and disconnects on failure |
| `0x802F8F10` | `__wpadInitConnectionCallback` | Prints ` ==>this error means that the firmware is for NDEV %s` with `"2.0"` or `"2.1 or later"`, then queues report type, DPD CSB, port, two game info reads (`0x2A`, `0x62`), the configuration read (20 bytes at `0x176C` or 42 at 0) and a status request |
| `0x802F9810` | `__wpadRetrieveChannel` | Compares the device name with `"Nintendo RVL-WBC"`, reserves channel 3 for the balance board, otherwise picks the latest-device or first free channel |
| `0x802F9930` | `__wpadConnectionCallback` | Prints `connection is %s` with `"opened"` or `"closed"`, `"clean up command queue"` and the unassigned-handle warning; registered by `__wpadManageHandler` |
| `0x802F9DF0` | `__wpadReceiveCallback` | Calls the HID parser and prints `HID Parser reports: %d`; registered by `__wpadManageHandler` |
| `0x802F9E40` | `WPADGetAccGravityUnit` | Petari body: core and Nunchuk `acc_1g - acc_0g` |
| `0x802F9F20` | `__wpadDisconnectCallback` | `BTA_HhClose` unless the result is `WPAD_ERR_NO_CONTROLLER`; passed to `WPADControlLed` |
| `0x802F9F50` | `WPADDisconnect` | Clears the device history, then the polite disconnect through `WPADControlLed(chan, 0, ...)` |
| `0x802FA0A0` | `WPADSetSamplingCallback` | KPAD warnings naming `WPADSetSamplingCallback`; stores at `0x8A8` |
| `0x802FA220` | `WPADGetDataFormat` | Reads `dataFormat` (`0x8B8`) |
| `0x802FA270` | `WPADSetDataFormat` | Calls `WPADiSendSetReportType` with the new format and stores it |
| `0x802FA330` | `WPADGetInfo` | `WPADGetInfoAsync` with `__wpadSyncCallback`, then sleeps on the thread queue |
| `0x802FA3A0` | `__wpadInfoCallback` | Calls and clears `infoCallback` (`0x988`) and `infoLocked` (`0x98C`) |
| `0x802FA6F0` | `WPADEnableMotor` | Writes `_wpadRumbleFlag` |
| `0x802FA730` | `WPADIsMotorEnabled` | Reads `_wpadRumbleFlag` |
| `0x802FA770` | `WPADControlLed` | Queues report `0x11` with `pattern << 4` |
| `0x802FA960` | `WPADSaveConfig` | `SCSetWpadSpeakerVolume`, `SCSetWpadMotorMode`, `SCFlushAsync` |
| `0x802FAA10` | `WPADRead` | Copies the current receive buffer with the format size |
| `0x802FAB00` | `WPADSetAutoSamplingBuf` | Clears the buffer and marks each entry's `err` |
| `0x802FACB0` | `WPADiExcludeButton` | Clears opposing D-pad directions for core, Classic and train buttons |
| `0x802FADD0` | `WPADiCopyOut` | Ring-buffer copy and sampling callback, increments `packetCnt` |
| `0x802FC560` | `WPADGetSpeakerVolume` | Reads `_wpadSpeakerVol` |
| `0x802FC5A0` | `WPADSetSpeakerVolume` | Clamps to 0..127 and stores `_wpadSpeakerVol` |
| `0x802FC5F0` | `__wpadIsBusyStream` | `WUDGetBufferStatus`, `_WUDGetQueuedSize`, `_WUDGetNotAckedSize`, `_WUDGetLinkNumber`; called by both stream functions |
| `0x802FC9D0` | `WPADGetDpdSensitivity` | Reads `_wpadDpdSense` |
| `0x802FCA10` | `WPADIsDpdEnabled` | Reads `infos.dpd` (`0x838`) |
| `0x802FCA60` | `WPADGetDpdFormat` | `dpdFormat` when DPD is enabled, else 0 |
| `0x802FCAC0` | `__wpadDpdCallback` | Copies `dpdCommand` to `dpdFormat`, clears `dpdBusy`; passed by `WPADControlDpd` |
| `0x802FCB00` | `WPADControlDpd` | Queues reports `0x13` and `0x1A`, writes `0x04B00030`, the two sensitivity tables (`.rodata` `0x80397DD8` and `0x80397E08`, indexed by `_wpadDpdSense - 1`) and the mode at `0x04B00033` |
| `0x802FDB80` | `WPADControlBLC` | Requires `WUDIsLinkedWBC`; writes `AA AA AA 55 c c c` to extension register `0xF1` |
| `0x802FDD90` | `WPADiSendSetReportType` | Report `0x12`, 16-case format table |
| `0x802FE000` | `WPADiSendWriteDataCmd` | One-byte report `0x16`, the write inlined |
| `0x802FE1C0` | `WPADiSendWriteData` | Report `0x16`, length 21, `len & 0x1F` |
| `0x802FE380` | `WPADiSendReadData` | Report `0x17`, length 6, sets the read buffer, length and address |
| `0x802FE530` | `WPADiIsAvailableCmdQueue` | `queueSize + num <= cmdlen - 1` |
| `0x802FE5C0` | `WPADiClearQueue` | Already referenced by `__wpadClearControlBlock` (F-500-3) |
| `0x802FE630` | `WPADiGetGameType` | Returns `_wpadGameType` |
| `0x802FE640` | `WPADiGetGameCode` | Returns `_wpadGameCode` |

The functions already named before this issue, `WPADGetRadioSensitivity`, `WPADProbe`, `WPADSetConnectCallback`, `WPADSetExtensionCallback`, `WPADGetInfoAsync`, `WPADControlMotor`, `WPADIsSpeakerEnabled`, `WPADControlSpeaker`, `WPADCanSendStreamData`, `WPADSendStreamData` and `WPADSetCallbackByKPAD`, keep their names and gain `scope:global`. Public names are high confidence. Static names are medium confidence, as in F-500-2.

`WPADCancelSyncDevice`, `WPADGetAddress`, `WPADRecalibrate`, `WPADIsUsedCallbackByKPAD`, `WPADiSendSetPort`, `WPADiSendDPDCSB`, `WPADiSendEnableDPD`, `WPADiSendEnableSpeaker`, `WPADiSendMuteSpeaker`, `WPADiSendGetContStat` and `WPADiSendStreamData` are inlined at every use here and have no copy in the DOL; the linker dead-strips them. Confidence high.

## F-501-2: Outside symbols named for the link

Linking WPAD.c from source needs these labels outside the block named in `symbols.txt`. Sizes, splits and types stay with the owning issues.

| Address | Name | Evidence |
| --- | --- | --- |
| `0x802FFDD0` | `WPADiHIDParser` | Called with port and report from `__wpadReceiveCallback` (#502) |
| `0x803039E0` | `WPADiClearMemBlock` | Called by `__wpadClearControlBlock` (F-500-3) |
| `0x80304530` | `WPADWriteExtReg` | `(chan, buf, len, addr, callback)`: checks status and setup, then `WPADiSendWriteData(&cmdq, buf, len, addr | 0x04A40000, callback)`. This version has no separate register-space argument, unlike the 2009-12 reconstruction; name medium confidence |
| `0x80304600` | `DEBUGPrint` | F-500-2 |
| `0x80309FA0` | `WUDIsLinkedWBC` | Returns a WUD `.sbss` flag; gates the balance board paths |
| `0x8030D240` | `WUDInit` | Called by `WPADInit` |
| `0x8030D360` | `WUDRegisterAllocator` | Target of `WPADRegisterAllocator` |
| `0x8030D3B0` | `WUDShutdown` | Called by `__wpadShutdown` |
| `0x8030D5D0` | `WUDGetStatus` | Target of `WPADGetStatus` |
| `0x8030D610` | `WUDGetBufferStatus` | First call in `__wpadIsBusyStream`; follows `WUDGetStatus` as in Petari `WUD.c` |
| `0x8030D6D0` | `WUDSetSyncSimpleCallback` | Swaps the callback at WUD control block offset 4 |
| `0x8030D880` | `WUDStartFastSyncSimple` | `StartSyncDevice(1, -1, 0, TRUE)` as in Petari `WUD.c` |
| `0x8030D8A0` | `WUDCancelSyncDevice` | Sets the abort flag, then the shared stop-sync body |
| `0x8030D930` | `WUDStopSyncSimple` | Stop-sync body without the abort flag |
| `0x8030D9C0` | `WUDSetDisableChannel` | Called by `__wpadCalcAfh` |
| `0x8030DAB0`, `0x8030DB10` | `WUDSetHidRecvCallback`, `WUDSetHidConnCallback` | Registered by `__wpadManageHandler` |
| `0x8030DB70` | `WUDSetVisibility` | Called by `__wpadCleanup` |
| `0x8030F0C0` | `WUDIsBusy` | Called by `OnShutdown` |
| `0x80310080` | `_WUDGetDevAddr` | Returns the device address for a handle |
| `0x803100E0`, `0x80310140` | `_WUDGetQueuedSize`, `_WUDGetNotAckedSize` | Per-handle `u16` tables read by `__wpadIsBusyStream` |
| `0x803101A0` | `_WUDGetLinkNumber` | Reads the WUD link count |
| `0x803101F0` | `WUDSetDeviceHistory` | Called by `__wpadCleanup`, `__wpadRetrieveChannel` and `WPADDisconnect` |
| `0x80310270` | `WUDIsLatestDevice` | `memcmp` of the address with SC device entry `chan + 10` |
| `0x803102D0` | `WUDUpdateSCSetting` | Called by `__wpadCalcSystemSettings` |
| `0x80310ED0` | `GKI_getbuf` | Allocates the HID report buffer in `__wpadSendDataSub` |
| `0x80319570` | `BTA_HhClose` | Builds BTA event `0x1701` with the handle |
| `0x80319688` | `BTA_HhSendData` | Sends the report buffer in `__wpadSendDataSub` |
| `0x8031982C` | `BTA_HhGetAclQueueInfo` | Last call of `__wpadManageHandler` |
| `0x8031BBE8` | `btm_remove_acl` | Called with the device address on impolite disconnects |

Confidence high for the WUD and BTE names that Petari's CC0 `WUD.c` and the BTE sources pair with the same bodies, medium for `WUDIsLinkedWBC`, `WUDIsLatestDevice` and `WPADWriteExtReg`, which only the unlicensed reconstruction names.

## F-501-3: Compiler behavior in the second block

All 80 WPAD.c functions match at 100% in objdiff with Wii/1.0 and `cflags_sdk`. Observed behavior:

- `-inline auto` decides from the callee's source size. `WPADDisconnect` keeps its call to `WPADControlLed` only when `WPADControlLed` stores `WPAD_ERR_NONE` into `status` before reading the control block and keeps the queue result in a separate `success` variable. Without those two statements the whole function, including the command push, is inlined. `WPADGetInfo` likewise keeps its call to `WPADGetInfoAsync`.
- `switch` statements emit their case blocks in source order. `__wpadFmt2Size` matches only with the order Nunchuk, Classic (with guitar and drum), train, balance board, VSM, then `WPAD_FMT_CORE_ACC_DPD_FULL`. `WPADiSendSetReportType` lists drum after guitar.
- Register allocation follows declaration order in `__wpadRetrieveChannel` (address, index, channel), `WPADControlLed` (`setup` before `status`) and `__wpadIsBusyStream` (`enabled` before the control block pointer).
- `WPADSetAutoSamplingBuf` needs an `int` index and a `u32` address cast; a `u32` index strength-reduces the unrolled loop.
- `WPADiSendWriteData` and `WPADiSendReadData` are inlined in this file and also emitted, because the HID parser and memory code after WPAD.c call them. `WPADiSendSetReportType` is never inlined.

## F-501-4: Header and split changes

`include/revolution/wpad.h` gains `WPAD_CHAN_INVALID`, `WPAD_DEV_BALANCE_CHECKER` (3), `WPAD_SPEAKER_REINIT` (5, treated like `WPAD_SPEAKER_ON`; name descriptive, low confidence), `WPAD_DPD_DISABLE`, the balance board commands `WPAD_BLC_DISABLE` (0), `WPAD_BLC_UPDATE` (`0x55`, descriptive, low confidence) and `WPAD_BLC_ENABLE` (`0xAA`), and prototypes for the new public and `WPADi` functions. `WPADVSStatus` grows to 0x4A bytes: `__wpadFmt2Size` returns 0x4A for formats 13 and 14, and the VSM parser at `0x803012A0` writes single bytes at `0x47`, `0x48` and `0x49`. They are `u8 auxData[3]`, a descriptive name with low confidence. `include/revolution/wud/wud.h` gains `WUDIsLatestDevice` and `WUDIsLinkedWBC`.

`__wpadGetDpdSensitivity` now casts `SCGetBtDpdSensibility()` to `u8`. #636 changed the shared declaration to `u32` for the SC unit, and without the cast `__wpadInitSub` and `__wpadManageHandler` lose the zero-extension that F-500-4 matched.

The WPAD.c `.text` split now ends at `0x802FE648`, the end of `WPADiGetGameCode`, instead of `0x802FE650`. The next assembly unit is 4-byte aligned, so a split ending at the next unit moved all later code back 8 bytes when linked from source (F-476-3). The other sections already end where the following units' 8-byte alignment puts them. Confidence high.

## Traps

- **Shared header fixes can silently break a NonMatching unit.** #636 corrected `SCGetBtDpdSensibility` to `u32` for SC, which broke two of #500's matched WPAD functions. Nothing failed, because a `NonMatching` unit links the original object. Rerun objdiff on every unit that includes a header you change, or link it from source.
- **Inlining depends on source size, not only on the final code.** A function that matches on its own can still be inlined into a caller that originally called it. Compare callers too, and try the reference's dead stores and temporaries (F-501-3).
