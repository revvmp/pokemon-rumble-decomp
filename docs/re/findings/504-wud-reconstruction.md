# WUD reconstruction

## F-504-4: Complete WUD split and first source functions

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. WUD.c starts with `WUDIsLinkedWBC` at `0x80309FA0` and ends at `0x80310394`, before WUDHidHost.c. Its 86 retained functions total 25,028 function bytes and span issues #504–#507. #504 owns the first function and therefore the complete file split. The later WUDHidHost.c and debug_msg.c units remain as already established by #507.

| Section | Start | Exclusive end |
| --- | --- | --- |
| `.text` | `0x80309FA0` | `0x80310394` |
| `.data` | `0x803DF0B8` | `0x803E0260` |
| `.bss` | `0x8047C320` | `0x8047E4C8` |
| `.sdata` | `0x804B8E88` | `0x804B8EA0` |
| `.sbss` | `0x804B9B28` | `0x804B9B60` |
| `.sdata2` | `0x804BB5D8` | `0x804BB5E0` |

The small-BSS range begins at its first word, `0x804B9B28`. The preceding four bytes after the USB unit are alignment padding; including them in WUD's range provoked dtk's eight-byte alignment warning. The corrected range removes the warning, and the full split-only build and frozen DOL verification pass before source registration.

The current source contains 45 retained functions, totaling 6,936 function bytes. The following 42 functions, totaling 4,848 bytes, reproduce every non-relocation instruction word at their exact original sizes with Wii/1.0 and `cflags_sdk`. The security callback and registration/removal paths are reconstructed separately with remaining differences.

| Function | Address | Bytes |
| --- | --- | --- |
| `WUDIsLinkedWBC` | `0x80309FA0` | `0x8` |
| `App_MEMalloc` | `0x80309FB0` | `0x50` |
| `App_MEMfree` | `0x8030A000` | `0x50` |
| `__wudSyncFlushCallback` | `0x8030A050` | `0x78` |
| `__wudDeleteFlushCallback` | `0x8030B6D0` | `0x58` |
| `WUDiTerminateDevice` | `0x8030B730` | `0x98` |
| `__wudInitFlushCallback` | `0x8030BFD0` | `0x4C` |
| `__wudShutdownFlushCallback` | `0x8030CD70` | `0x4C` |
| `WUDRegisterAllocator` | `0x8030D360` | `0x4C` |
| `WUDGetStatus` | `0x8030D5D0` | `0x3C` |
| `WUDGetBufferStatus` | `0x8030D610` | `0x48` |
| `WUDSetSniffMode` | `0x8030D660` | `0x6C` |
| `WUDSetSyncSimpleCallback` | `0x8030D6D0` | `0x5C` |
| `__wudStartSyncDevice` | `0x8030D730` | `0x14C` |
| `WUDStartFastSyncSimple` | `0x8030D880` | `0x14` |
| `WUDCancelSyncDevice` | `0x8030D8A0` | `0x90` |
| `WUDStopSyncSimple` | `0x8030D930` | `0x88` |
| `WUDSetDisableChannel` | `0x8030D9C0` | `0xE8` |
| `WUDSetHidRecvCallback` | `0x8030DAB0` | `0x5C` |
| `WUDSetHidConnCallback` | `0x8030DB10` | `0x5C` |
| `WUDSetVisibility` | `0x8030DB70` | `0x58` |
| `__wudInitSub` | `0x8030E270` | `0x170` |
| `WUDiGetDevInfo` | `0x8030E6C0` | `0xD8` |
| `WUDiMoveTopSmpDevInfoPtr` | `0x8030E7A0` | `0x114` |
| `WUDiMoveBottomSmpDevInfoPtr` | `0x8030E8C0` | `0x114` |
| `WUDiMoveTopStdDevInfoPtr` | `0x8030EB50` | `0x114` |
| `WUDiMoveBottomStdDevInfoPtr` | `0x8030EC70` | `0x114` |
| `WUDIsBusy` | `0x8030F0C0` | `0x74` |
| `__wudCleanupStackCallback` | `0x8030F140` | `0x34` |
| `__wudDeviceStatusEventStackCallback` | `0x8030FBA0` | `0x50` |
| `_WUDGetDevAddr` | `0x80310080` | `0x54` |
| `_WUDGetQueuedSize` | `0x803100E0` | `0x58` |
| `_WUDGetNotAckedSize` | `0x80310140` | `0x58` |
| `_WUDGetLinkNumber` | `0x803101A0` | `0x3C` |
| `WUDiGetDiscoverDevice` | `0x803101E0` | `0xC` |
| `WUDSetDeviceHistory` | `0x803101F0` | `0x74` |
| `WUDIsLatestDevice` | `0x80310270` | `0x54` |
| `WUDUpdateSCSetting` | `0x803102D0` | `0x4C` |
| `WUDiSetDevAddrForHandle` | `0x80310320` | `0x14` |
| `WUDiGetDevAddrForHandle` | `0x80310340` | `0x14` |
| `WUDiSetQueueSizeForHandle` | `0x80310360` | `0x14` |
| `WUDiSetNotAckNumForHandle` | `0x80310380` | `0x14` |

This diagnostic excludes relocation-bearing words and does not establish matching function acceptance. Whole-unit objdiff is text 27.619307%, data 4.8480787%, BSS 37.36787%, sdata 0%, sbss 33.962265% and sdata2 100%. WUD remains `NonMatching`; the final executable continues to link the original WUD object and gains zero linked WUD source-byte credit. The partial-source full build and explicit frozen verification pass. EUART and USB remain accepted source-linked units.

The implementations reuse the existing typed WUD headers. `_wcb` is the established 0x750-byte controller block at `0x8047C320`. Paired source/target SDA relocations identify the four-byte `sWudInitialized` at `0x804B9B28`, `sWudReadNand` at `0x804B9B2C` and `_linkedWBC` at `0x804B9B40`; no field address is guessed from declaration order. The last is returned by `WUDIsLinkedWBC`. Its weak qualifier follows the licensed newer SDK reference; the target directly establishes its word storage and access, not an original ELF binding.

The two small read-only objects are the four-byte `Wii` name initializer at `0x804BB5D8` and the three-byte Bluetooth device class `{0x00, 0x04, 0x48}` at `0x804BB5DC`. Their full seven-byte sequence compares at 100%. They are not a six-byte zero Bluetooth address.

The eight tail routines add 368 function bytes. Paired HA/LO and SDA relocations establish `_work` at `0x8047CB78` (0x60 bytes), `_scArray` at `0x8047CBD8` (0x461 bytes), `_dev_handle_to_bda` at `0x8047D0E0` (0x40 bytes), `_dev_handle_queue_size` at `0x8047E488` (0x20 bytes), `_dev_handle_notack_num` at `0x8047E4A8` (0x20 bytes), and byte flag `sWudSettingsDirty` at `0x804B9B5C`. The 0x461-byte settings array uses the existing `SCBtDeviceInfoArray` layout: one count byte followed by sixteen 70-byte records. Its former coarse 0x468 extent included seven padding bytes. The pointer array consumes only the first 0x40 bytes of its former 0x1A0 record; the remaining 0x160 bytes retain their coarse unnamed record pending reconstruction.

Device history addresses records `10 + chan`, clears the complete record for a null address, and otherwise copies the six-byte address. Updating settings writes the array and initiates an asynchronous flush only when the write succeeds, then clears the dirty flag. These bodies follow the pinned key reference using the project's existing typed SC fields. The original relocation accesses establish the flag's byte address rather than a guessed position at the end of `.sbss`.

The target's embedded messages supply the exact private names `__wudSyncFlushCallback`, `__wudDeleteFlushCallback`, `__wudShutdownFlushCallback`, `__wudCleanupStackCallback` and `__wudInitSub`. The device-status and power-manager callback references are also named from the target's messages: `__wudDeviceStatusEventStackCallback` at `0x8030FBA0` and `__wudPowerMangeEventStackCallback` at `0x8030FF50`. Preserve `Mange` exactly. The device-status callback is implemented; the power-manager callback body is not implemented yet. `WUDVendorSpecificCallback` at `0x8030F8F0` and `WUDiRegisterDevice` at `0x8030E3E0` receive reference-based names through relocated callback/call operands; confidence in their original spelling is medium. Existing names established by #507 are retained.

The initialization-flush callback stores state 5, and `WUDIsBusy` requires state 6 in addition to zero sync/delete states and initialized stack state 4. The original store at `0x8030C004` and comparison at `0x8030F104` establish these values directly. They differ from the older reference's values 4 and 5. Source-local descriptive constants `WUD_INIT_FLUSH_COMPLETE` and `WUD_INIT_READY` preserve these proven values without changing the shared header prematurely. The device-status callback logs its original `__wudDeviceStatusEventStackCallback` name and reports a USB fatal error for status 2; this target does not call the newer reference's fatal-error screen helper. These three functions add 272 bytes, all non-relocation words exact.

Seven synchronization/channel/count routines add 1,040 function bytes. Starting synchronization checks the proven ready state through `WUDIsBusy`, stores its target byte at `0x804B9B5D`, initializes the search counters and schedules the original callback `__wudSyncHandler0` at `0x8030B6B0`. Cancellation additionally sets the four-byte abort flag at `0x804B9B30`; stopping alone does not set it. These globals receive descriptive names `sWudSyncTarget` and `sWudAbortSync`. Paired source/target relocations prove all three bindings. The private start/callback spellings follow the pinned key reference with medium confidence. The common stop helper is naturally inlined and absent from the compiled source object; it is outside the force-active region.

The channel and queue-count APIs retain their existing signed-byte public parameter types. The release bounds checks explicitly cast to `UINT8`, matching the target: channel values above 13 return false, and handle values above 15 return zero. Count-array indexing also uses the unsigned-byte value. This reproduces every non-relocation word without changing shared declarations. The channel routine's relocated call identifies `BTM_SetAfhChannels` at `0x8031C840`; only the outside symbol name changes, not its extent, type or split.

The source uses `force_active` only to keep these proven retained private function bodies available for comparison while their callers are unreconstructed. It introduces no dummy callers or placeholder implementations. Recheck natural retention after completing the unit, and retain no unneeded helpers merely to fill the target's shape.

## Security callback reconstruction

`__wudSecurityEventStackCallback` at `0x8030F180` now has its complete native 0x554-byte body (1,364 bytes), handling Bluetooth enable/disable, PIN requests, authentication, authorization, link changes and other security notifications. Its name follows the pinned key reference with medium spelling confidence. The target's first message at `0x803DFB34` actually reads `__wudCleanupStackCallback: `; this contradictory original string is preserved rather than treated as the function's identity.

The callback retains 32 non-relocation instruction differences. The current partial unit assigns the data and BSS base registers in reverse order from the original, and its folded message/working-device/settings offsets reflect incomplete preceding data. Before the descriptor/registration pass, the source working device was at BSS offset 0x7D0 versus target 0x858, and the first message was at data offset 0x1EC versus target 0xA7C. Later additions change partial pool offsets; the security callback still has 32 instruction differences. These are unresolved diagnostics, not matching acceptance. The unchanged 42-function raw audit still passes. Complete the naturally emitted unit data and address operands before accepting this callback.

Header prerequisite #720 supplies the names `pinCodeSubmitted` and `linkKeyStorage`. The security callback uses the PIN field. The address-reversal, PIN and stack-enable helpers are actual functional SDK implementations. They and the common stop helper naturally inline; none has a standalone source-object body. The referenced stack callback receives reference name `__wudEnableStackHandler0` at `0x8030BFB0`. Its body remains unreconstructed. The outside referenced function `BTA_HhEnable` at `0x803194B0` receives its SDK name without changing its split, type or extent.

After rebasing onto #720, a fresh `build/504-source-header-refresh-1/wpse-v1` all-source build retains byte identity for all 340 pre-existing main-branch source objects. Full Ninja and explicit verification against this fresh directory pass. WUD still links the original object and contributes zero linked source credit. The source draft depends on #720, which must merge first.

[F-504-6](504-wud-hid-registration.md) records the added 217-byte HID descriptor, exact-size registration/removal functions, their proven Balance Board condition and twelve remaining folded literal-address operands. The descriptor bytes match exactly; WUD remains NonMatching.

## Remaining work

- Complete the remaining 41 native functions, including synchronization, the Wii Fit NAND paths, patch handling and Bluetooth event processing.
- Reconstruct and verify the complete unit data and layouts; the current source defines the controller block, working device, settings and handle arrays, three proven flag words and a dirty byte.
- Complete the security callback's pooled data/BSS addressing and register allocation and the registration/removal literal offsets, and resolve any further layout or naming needs through shared-header prerequisites. [F-504-5](504-wud-device-state-headers.md) records the two device-member renames in #720 and its unchanged 340-object baseline.
- Resolve every call, callback, data relocation and folded reference. The current raw-word comparison is deliberately narrower than acceptance.
- Mark WUD `Matching` only after all retained code and the complete source-selected frozen DOL verify. The source prerequisite for KPAD remains separate in #710.

## Provenance

The implemented bodies derive from [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0, with no file-specific license exception found. C++ convenience includes and comments are removed; the implementations use the project's existing SDK C headers. Target message strings correct the private names. The weak linked-board flag and the wrapper's behavior are corroborated by [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. No decompiler text is included.

## Traps

- **Declaration order is not target small-data order.** A guessed layout from another SDK revision would have put the initialization flags near the end of this unit. Paired target relocations place them at `0x804B9B28` and `0x804B9B2C`; record those observed accesses before naming or defining the remaining globals.
- **Private bodies may disappear from an incomplete source.** The first partial compile dropped unreferenced static callbacks before comparison. Scoped `force_active` preserves only the actual retained functions while their source callers are missing. Do not create fake callers or count unrelated reference helpers as target functions.
- **Original message spelling takes precedence over reference helper names.** The target prints `__wudPowerMangeEventStackCallback`; use that spelling instead of silently correcting it to the reference's `PowerManager` name. Other callback names likewise retain the embedded `__wud` prefixes.

- **WUD initialization states vary by SDK revision.** The old reference stores 4 after an initialization flush and checks 5 for readiness; this target stores 5 and checks 6. The newer reference also adds fatal-error behavior absent here. Compare target instructions before importing state values or callback side effects.

- **Signed public parameters can have unsigned release checks.** The WUD queue-count getters and channel exclusion API retain signed-byte declarations, but this target bounds-checks their `UINT8` interpretations. The older reference's signed comparisons added instructions; the newer reference's release cast and target disassembly explain the exact source change.

- **A callback can print another callback's name.** The security-event routine at `0x8030F180` prints `__wudCleanupStackCallback: `. Its event dispatch and relocated calls identify its security role. Preserve the original log typo while recording the reference-derived function name and confidence separately.
