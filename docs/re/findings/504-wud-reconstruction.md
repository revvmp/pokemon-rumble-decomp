# WUD reconstruction

## F-504-4: Complete WUD split and source inventory

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. WUD.c starts with `WUDIsLinkedWBC` at `0x80309FA0` and ends at `0x80310394`, before WUDHidHost.c. Its 86 retained functions total 25,028 function bytes and span issues #504 through #507. #504 owns the first function and therefore the complete file split. The later WUDHidHost.c and debug_msg.c units remain as already established by #507.

| Section | Start | Exclusive end |
| --- | --- | --- |
| `.text` | `0x80309FA0` | `0x80310394` |
| `.data` | `0x803DF0B8` | `0x803E0260` |
| `.bss` | `0x8047C320` | `0x8047E4C8` |
| `.sdata` | `0x804B8E88` | `0x804B8EA0` |
| `.sbss` | `0x804B9B28` | `0x804B9B60` |
| `.sdata2` | `0x804BB5D8` | `0x804BB5E0` |

The small-BSS range begins at its first word, `0x804B9B28`. The preceding four bytes after the USB unit are alignment padding; including them in WUD's range provoked dtk's eight-byte alignment warning. The corrected range removes the warning, and the full split-only build and frozen DOL verification pass before source registration.

All 86 retained native functions, totaling 25,028 bytes, reproduce every instruction and relocation at their original extents with Wii/1.0 and unchanged `cflags_sdk`. [F-504-16](504-wud-importer-match.md) records complete normal source selection, exact linked DOL identity, initialized data and storage acceptance.

| Function | Address | Bytes |
| --- | --- | --- |
| `WUDIsLinkedWBC` | `0x80309FA0` | `0x8` |
| `App_MEMalloc` | `0x80309FB0` | `0x50` |
| `App_MEMfree` | `0x8030A000` | `0x50` |
| `__wudSyncFlushCallback` | `0x8030A050` | `0x78` |
| `__wudSyncPrepareSearch` | `0x8030A0D0` | `0x158` |
| `__wudSyncTryConnect` | `0x8030A230` | `0x170` |
| `__wudSyncVirginStandard` | `0x8030A3A0` | `0x378` |
| `__wudSyncStoredDevInfoToNand` | `0x8030A720` | `0x134` |
| `__wudOpenWiiFitCallback` | `0x8030A860` | `0x7C` |
| `__wudSeekWiiFitCallback` | `0x8030A8E0` | `0x84` |
| `__wudUpdateWiiFitCallback` | `0x8030A970` | `0x7C` |
| `__wudCloseWiiFitCallback` | `0x8030A9F0` | `0x54` |
| `__wudSyncDone` | `0x8030AA50` | `0x168` |
| `__wudSyncHandler` | `0x8030ABC0` | `0xAE4` |
| `__wudSyncHandler0` | `0x8030B6B0` | `0x20` |
| `__wudDeleteFlushCallback` | `0x8030B6D0` | `0x58` |
| `WUDiTerminateDevice` | `0x8030B730` | `0x98` |
| `__wudDeleteCleanupDatabase` | `0x8030B7D0` | `0x204` |
| `__wudDeleteHandler` | `0x8030B9E0` | `0x350` |
| `__wudDeleteHandler0` | `0x8030BD30` | `0x20` |
| `__wudStackCheckDeviceInfo` | `0x8030BD50` | `0x10C` |
| `__wudStackHandler` | `0x8030BE60` | `0x144` |
| `__wudEnableStackHandler0` | `0x8030BFB0` | `0x20` |
| `__wudInitFlushCallback` | `0x8030BFD0` | `0x4C` |
| `__wudInitDevInfo` | `0x8030C020` | `0x638` |
| `__wudNandResultCallback` | `0x8030C660` | `0x80` |
| `__wudNandFlushCallback` | `0x8030C6E0` | `0x18` |
| `__wudGetDevInfoFromWiiFit` | `0x8030C700` | `0x530` |
| `__wudInitHandler` | `0x8030CC30` | `0x114` |
| `__wudInitHandler0` | `0x8030CD50` | `0x20` |
| `__wudShutdownFlushCallback` | `0x8030CD70` | `0x4C` |
| `__wudShutdownHandler` | `0x8030CDC0` | `0xB4` |
| `__wudShutdownHandler0` | `0x8030CE80` | `0x20` |
| `__wudClearControlBlock` | `0x8030CEA0` | `0x394` |
| `WUDInit` | `0x8030D240` | `0x11C` |
| `WUDRegisterAllocator` | `0x8030D360` | `0x4C` |
| `WUDShutdown` | `0x8030D3B0` | `0x220` |
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
| `__wudModuleRebootCallback` | `0x8030DBD0` | `0x30` |
| `__wudInstallPatchCallback` | `0x8030DC00` | `0xF0` |
| `__wudWritePatchCallback` | `0x8030DCF0` | `0x254` |
| `__wudRemovePatchCallback` | `0x8030DF50` | `0x1B4` |
| `__wudSuperPeekPokeCallback` | `0x8030E110` | `0x5C` |
| `__wudAppendRuntimePatch` | `0x8030E170` | `0xFC` |
| `__wudInitSub` | `0x8030E270` | `0x170` |
| `WUDiRegisterDevice` | `0x8030E3E0` | `0x154` |
| `WUDiRemoveDevice` | `0x8030E540` | `0x180` |
| `WUDiGetDevInfo` | `0x8030E6C0` | `0xD8` |
| `WUDiMoveTopSmpDevInfoPtr` | `0x8030E7A0` | `0x114` |
| `WUDiMoveBottomSmpDevInfoPtr` | `0x8030E8C0` | `0x114` |
| `WUDiMoveTopOfDisconnectedSmpDevice` | `0x8030E9E0` | `0x168` |
| `WUDiMoveTopStdDevInfoPtr` | `0x8030EB50` | `0x114` |
| `WUDiMoveBottomStdDevInfoPtr` | `0x8030EC70` | `0x114` |
| `WUDiMoveTopOfDisconnectedStdDevice` | `0x8030ED90` | `0x190` |
| `WUDiMoveTopOfUnusedStdDevice` | `0x8030EF20` | `0x198` |
| `WUDIsBusy` | `0x8030F0C0` | `0x74` |
| `__wudCleanupStackCallback` | `0x8030F140` | `0x34` |
| `__wudSecurityEventStackCallback` | `0x8030F180` | `0x554` |
| `__wudSearchEventStackCallback` | `0x8030F6E0` | `0x208` |
| `WUDVendorSpecificCallback` | `0x8030F8F0` | `0x2A8` |
| `__wudDeviceStatusEventStackCallback` | `0x8030FBA0` | `0x50` |
| `__wudLinkKeyEventStackCallback` | `0x8030FBF0` | `0x354` |
| `__wudPowerMangeEventStackCallback` | `0x8030FF50` | `0x12C` |
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

The actual normal build selects the WUD source object, and full frozen verification passes. All 1,185 paired relocations agree by position, type, operands and destination. The prelink objdiff diagnostics remain text 99.89692%, data 11.763171%, BSS/sdata/sbss 100%, and sdata2 63.636364%; exact final bytes, including initialized data, alignment and stripping, are proved independently in F-504-16. EUART and USB also remain source-linked.

## Historical reconstruction checkpoints

The partial-source counts and earlier retention work below precede F-504-16; they do not describe the final acceptance state.

The implementations reuse the existing typed WUD headers. `_wcb` is the established 0x750-byte controller block at `0x8047C320`. Paired source/target SDA relocations identify the four-byte `sWudInitialized` at `0x804B9B28`, `sWudReadNand` at `0x804B9B2C` and `_linkedWBC` at `0x804B9B40`; no field address is guessed from declaration order. The last is returned by `WUDIsLinkedWBC`. Its weak qualifier follows the licensed newer SDK reference; the target directly establishes its word storage and access, not an original ELF binding.

The two small read-only objects are the four-byte `Wii` name initializer at `0x804BB5D8` and the three-byte Bluetooth device class `{0x00, 0x04, 0x48}` at `0x804BB5DC`. Their full seven-byte sequence compares at 100%. They are not a six-byte zero Bluetooth address.

The eight tail routines add 368 function bytes. Paired HA/LO and SDA relocations establish `_work` at `0x8047CB78` (0x60 bytes), `_scArray` at `0x8047CBD8` (0x461 bytes), `_dev_handle_to_bda` at `0x8047D0E0` (0x40 bytes), `_dev_handle_queue_size` at `0x8047E488` (0x20 bytes), `_dev_handle_notack_num` at `0x8047E4A8` (0x20 bytes), and byte flag `sWudSettingsDirty` at `0x804B9B5C`. The 0x461-byte settings array uses the existing `SCBtDeviceInfoArray` layout: one count byte followed by sixteen 70-byte records. Its former coarse 0x468 extent included seven padding bytes. The pointer array consumes only the first 0x40 bytes of its former 0x1A0 record; the remaining 0x160 bytes retain their coarse unnamed record pending reconstruction.

Device history addresses records `10 + chan`, clears the complete record for a null address, and otherwise copies the six-byte address. Updating settings writes the array and initiates an asynchronous flush only when the write succeeds, then clears the dirty flag. These bodies follow the pinned key reference using the project's existing typed SC fields. The original relocation accesses establish the flag's byte address rather than a guessed position at the end of `.sbss`.

The target's embedded messages supply the exact private names `__wudSyncFlushCallback`, `__wudDeleteFlushCallback`, `__wudShutdownFlushCallback`, `__wudCleanupStackCallback` and `__wudInitSub`. The device-status and power-manager callback references are also named from the target's messages: `__wudDeviceStatusEventStackCallback` at `0x8030FBA0` and `__wudPowerMangeEventStackCallback` at `0x8030FF50`. Preserve `Mange` exactly. Both device-status and power-manager callback bodies are implemented; their acceptance follows the whole-unit gates. `WUDVendorSpecificCallback` at `0x8030F8F0` and `WUDiRegisterDevice` at `0x8030E3E0` receive reference-based names through relocated callback/call operands; confidence in their original spelling is medium. Existing names established by #507 are retained.

The initialization-flush callback stores state 5, and `WUDIsBusy` requires state 6 in addition to zero sync/delete states and initialized stack state 4. The original store at `0x8030C004` and comparison at `0x8030F104` establish these values directly. They differ from the older reference's values 4 and 5. Source-local descriptive constants `WUD_INIT_FLUSH_COMPLETE` and `WUD_INIT_READY` preserve these proven values without changing the shared header prematurely. The device-status callback logs its original `__wudDeviceStatusEventStackCallback` name and reports a USB fatal error for status 2; this target does not call the newer reference's fatal-error screen helper. These three functions add 272 bytes, all non-relocation words exact.

Seven synchronization/channel/count routines add 1,040 function bytes. Starting synchronization checks the proven ready state through `WUDIsBusy`, stores its target byte at `0x804B9B5D`, initializes the search counters and schedules the original callback `__wudSyncHandler0` at `0x8030B6B0`. Cancellation additionally sets the four-byte abort flag at `0x804B9B30`; stopping alone does not set it. These globals receive descriptive names `sWudSyncTarget` and `sWudAbortSync`. Paired source/target relocations prove all three bindings. The private start/callback spellings follow the pinned key reference with medium confidence. The common stop helper is naturally inlined and absent from the compiled source object; it is outside the force-active region.

The channel and queue-count APIs retain their existing signed-byte public parameter types. The release bounds checks explicitly cast to `UINT8`, matching the target: channel values above 13 return false, and handle values above 15 return zero. Count-array indexing also uses the unsigned-byte value. This reproduces every non-relocation word without changing shared declarations. The channel routine's relocated call identifies `BTM_SetAfhChannels` at `0x8031C840`; only the outside symbol name changes, not its extent, type or split.

The source uses `force_active` only to keep these proven retained private function bodies available for comparison while their callers are unreconstructed. It introduces no dummy callers or placeholder implementations. Recheck natural retention after completing the unit, and retain no unneeded helpers merely to fill the target's shape.

## Security callback reconstruction

`__wudSecurityEventStackCallback` at `0x8030F180` now has its complete native 0x554-byte body (1,364 bytes), handling Bluetooth enable/disable, PIN requests, authentication, authorization, link changes and other security notifications. Its name follows the pinned key reference with medium spelling confidence. The target's first message at `0x803DFB34` actually reads `__wudCleanupStackCallback: `; this contradictory original string is preserved rather than treated as the function's identity.

The callback now retains 22 non-relocation instruction differences, all folded addresses of byte-identical messages with identical opcode/register fields. Adding the real connection routine restored the original BSS prefix and resolved the earlier register/BSS differences; see [F-504-8](504-wud-sync-entry.md). Complete the naturally emitted unit data and address operands before accepting this callback. The 49-function raw audit remains exact.

Header prerequisite #720 supplies the names `pinCodeSubmitted` and `linkKeyStorage`. The security callback uses the PIN field. The address-reversal, PIN and stack-enable helpers are actual functional SDK implementations. They and the common stop helper naturally inline; none has a standalone source-object body. The referenced stack callback receives reference name `__wudEnableStackHandler0` at `0x8030BFB0`. Its body remains unreconstructed. The outside referenced function `BTA_HhEnable` at `0x803194B0` receives its SDK name without changing its split, type or extent.

After rebasing onto #720, a fresh `build/504-source-header-refresh-1/wpse-v1` all-source build retains byte identity for all 340 pre-existing main-branch source objects. Full Ninja and explicit verification against this fresh directory pass. WUD still links the original object and contributes zero linked source credit. The source draft depends on #720, which must merge first.

[F-504-6](504-wud-hid-registration.md) records the added 217-byte HID descriptor, exact-size registration/removal functions, their proven Balance Board condition and twelve remaining folded literal-address operands. The descriptor bytes match exactly; WUD remains NonMatching.

[F-504-7](504-wud-search-power.md) adds the exact-size search and power callbacks, seven/five folded data-operand differences, three proven discovery-state bindings and the exact 28-byte authentication-reset payload. The reset helpers naturally inline.

[F-504-8](504-wud-sync-entry.md) adds exact raw search preparation, exact-size connection attempts, the naturally restored BSS prefix and the reduced security diagnostic. All 51 differing operands across the six partial bodies resolve to identical data.

[F-504-9](504-wud-standard-pairing.md) adds exact-size standard pairing, four folded addresses to identical data, the typed Wii Fit workspace/settings views and its exact checksum stream. The pairing workspace and following handle array reproduce their original BSS positions. All 55 differing operands across seven partial bodies resolve to identical data.

[F-504-10](504-wud-settings-callbacks.md) adds six exact non-relocation streams for settings save, four NAND completion callbacks and synchronization completion, with 80 paired call/data operands checked. It records the Japan-only continuation, separate sync/delete fallback states and the lock byte.

[F-504-11](504-wud-sync-dispatch.md) adds the exact-size sync dispatcher and raw-exact fiber wrapper. All 16 dispatcher differences are folded addresses of identical messages; all 116 call/data relocation entries agree by opcode and symbol or target bytes. The canonical NAND structures and stack occupy their original BSS offsets. Current total: 58 bodies / 13,308 bytes; 50 raw-exact bodies / 6,356 bytes; eight partial bodies with 71 identical-data address differences.

[F-504-12](504-wud-lifecycle.md) adds eight native functions for deletion, stack enable and shutdown: 2,232 function bytes, six raw-exact streams and 125 paired call/data relocations. The deletion and stack drivers have 7/6 folded addresses to identical messages. Canonical companion-device settings restore the full original BSS layout. Current totals are 66 bodies / 15,540 bytes; 56 raw-exact bodies / 7,416 bytes; ten partial bodies with 84 identical-data address differences.

[F-504-13](504-wud-list-events.md) adds the three remaining device-list operations and vendor/stored-key callbacks, totaling 2,700 function bytes. The list streams are exact; the callbacks have 4/14 folded addresses to identical messages. All 94 new call/data relocations agree. Current totals: 71 bodies / 18,240 bytes, 59 raw-exact bodies / 8,584 bytes, twelve partial bodies with 102 identical-data address differences.

[F-504-14](504-wud-firmware-patch.md) adds all six retained firmware-patch functions and three target-verified protocol arrays. The original data prefix now reproduces 534 initialized bytes plus six padding bytes. Five earlier bodies now also have exact raw streams. Current totals: 77 native bodies / 19,904 bytes, 66 raw-exact bodies / 13,940 bytes, eleven partial bodies with 69 identical-data address differences.

[F-504-15](504-wud-full-initialization.md) adds the final nine native bodies and restores complete initialized-message and small-data layouts. All 86 native extents total 25,028 bytes; 85 bodies / 23,436 bytes have exact non-relocation streams. Their 1,128 relocation entries agree by type, position, instruction opcode/registers and semantic destination. The importer retains 82 differing words; its 57 destination entries agree only as an ordered sequence, not as an instruction-stream proof. Resolved data and a source-selected candidate match every executable byte outside the importer, including final alignment and stripping. The original WUD object remains selected.

## Issue acceptance and merge prerequisites

- WUD source matching is complete. Full #504 acceptance also requires the thirteen KPAD functions supplied by #710; #720 has already merged.
- Audit all forty original #504 functions in their actual source-selected units after the KPAD prerequisite, then run the latest-main configure/Ninja/frozen-verification merge loop.
- Neighboring #505, #506 and #507 retain their own complete inventories and acceptance audits; source-byte credit is counted once per unit.

## Provenance

The implemented bodies derive from [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0, with no file-specific license exception found. C++ convenience includes and comments are removed; the implementations use the project's existing SDK C headers. Target message strings correct the private names. The weak linked-board flag and the wrapper's behavior are corroborated by [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. No decompiler text is included.

## Traps

- **Declaration order is not target small-data order.** A guessed layout from another SDK revision would have put the initialization flags near the end of this unit. Paired target relocations place them at `0x804B9B28` and `0x804B9B2C`; record those observed accesses before naming or defining the remaining globals.
- **Private bodies may disappear from an incomplete source.** The first partial compile dropped unreferenced static callbacks before comparison. Scoped `force_active` preserves only the actual retained functions while their source callers are missing. Do not create fake callers or count unrelated reference helpers as target functions.
- **Original message spelling takes precedence over reference helper names.** The target prints `__wudPowerMangeEventStackCallback`; use that spelling instead of silently correcting it to the reference's `PowerManager` name. Other callback names likewise retain the embedded `__wud` prefixes.

- **WUD initialization states vary by SDK revision.** The old reference stores 4 after an initialization flush and checks 5 for readiness; this target stores 5 and checks 6. The newer reference also adds fatal-error behavior absent here. Compare target instructions before importing state values or callback side effects.

- **Signed public parameters can have unsigned release checks.** The WUD queue-count getters and channel exclusion API retain signed-byte declarations, but this target bounds-checks their `UINT8` interpretations. The older reference's signed comparisons added instructions; the newer reference's release cast and target disassembly explain the exact source change.

- **A callback can print another callback's name.** The security-event routine at `0x8030F180` prints `__wudCleanupStackCallback: `. Its event dispatch and relocated calls identify its security role. Preserve the original log typo while recording the reference-derived function name and confidence separately.
