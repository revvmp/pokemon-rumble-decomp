# WUD deletion, stack enable and shutdown

## F-504-12: Lifecycle drivers and full BSS reconstruction

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. Eight native functions add 2,232 reconstructed function bytes with Wii/1.0 and unchanged `cflags_sdk`.

| Function | Address | Extent | Non-relocation result |
| --- | --- | --- | --- |
| `__wudDeleteCleanupDatabase` | `0x8030B7D0` | `0x204` | Exact |
| `__wudDeleteHandler` | `0x8030B9E0` | `0x350` | Seven identical-message address differences |
| `__wudDeleteHandler0` | `0x8030BD30` | `0x20` | Exact |
| `__wudStackCheckDeviceInfo` | `0x8030BD50` | `0x10C` | Exact |
| `__wudStackHandler` | `0x8030BE60` | `0x144` | Six identical-message address differences |
| `__wudEnableStackHandler0` | `0x8030BFB0` | `0x20` | Exact |
| `__wudShutdownHandler` | `0x8030CDC0` | `0xB4` | Exact |
| `__wudShutdownHandler0` | `0x8030CE80` | `0x20` | Exact |

All 125 relocation entries across these eight bodies have the same types and instruction opcodes/registers as their original counterparts. Calls and callbacks agree by symbol, BSS references agree by actual offset, and small-data operands select the same named storage or literal bytes. The two partial drivers' 13 differing non-relocation words are folded `addi` operands addressing byte-identical messages.

## Behavior and storage evidence

Deletion waits for links to disappear, removes retained standard/simple devices, and clears both settings arrays. A linked Balance Board in Japan region 0 first zeroes the Wii Fit pairing key, recomputes its checksum and enters NAND state 100. The shared NAND initiators from [F-504-11](504-wud-sync-dispatch.md) handle states 100 through 103 for either synchronization or deletion. Settings cleanup uses the reference's boolean OR of the two settings writes and retains the observed flush/failure transitions. The completion path restores visibility, cancels the alarm and invokes the completion callback with result 1.

Stack enable reads stored keys once the HID host is enabled. Device-state byte `linkKeyStorage` distinguishes controller-only entries (1), fully synchronized entries (3) and entries requiring removal. Controller-only keys are deleted one at a time before advancing. Completion logs the local firmware version fields; revision 167 selects the runtime patch path, while other revisions call `__wudInitSub`. All six original firmware messages are retained. Firmware querying is an inlined helper here; it is not an additional retained native function.

Shutdown waits for settings to become available, combines both writes with the read-NAND status, and either flushes asynchronously or advances directly. Its final stage cancels the alarm and disables Bluetooth.

The compiled canonical `SCBtCmpDevInfoArray _spArray` has size `0x205` and occupies target address `0x8047E280`, native BSS offset `0x1F60`. Three trailing alignment bytes precede the queue-count array at `0x8047E488`/offset `0x2168`; the not-acknowledged counts follow at `0x8047E4A8`/offset `0x2188`. Both arrays retain size `0x20`. This resolves the last coarse WUD BSS object and gives the entire `.bss` a 100% diagnostic match.

Paired call references name outside SDK functions `BTM_ReadLocalVersion` at `0x8031D5D8` and `BTM_ReadStoredLinkKey` at `0x8031D9B4`. Only their names change. The WUD runtime-patch routine at `0x8030E170` is named `__wudAppendRuntimePatch` from the reference and firmware branch; its body remains unfinished.

## Validation and remaining work

The actual `all_source` target compiles 66 native functions totaling 15,540 bytes with original metadata extents. Fifty-six bodies / 7,416 bytes have exact non-relocation streams. Ten partial bodies retain 84 folded addresses to identical data. All 340 baseline source objects remain byte-identical. Whole-unit diagnostics are text 61.967556%, data 5.473637%, BSS 100%, sdata 37.037037%, sbss 47.16981% and sdata2 100%.

The full build exits zero before explicit frozen verification of `build/504-source-header-refresh-1/wpse-v1`. WUD remains `NonMatching`, with its original object linked and zero linked WUD source credit. Complete the remaining 20 functions, initialized and small-data layout, folded addresses and final relocated bytes before acceptance.

## Provenance

Lifecycle bodies and firmware diagnostics follow [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found. Balance Board deletion and current private names follow target-corroborated [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. Comments and annotations are removed. No decompiler output is included.

## Traps

- **A reference accessor may have no retained target function.** The deletion completion inlines `WUDGetConnectable`. A separate source definition inside `force_active` emitted an extra body absent from the native inventory. Use its functional inline helper and verify the target call sites; the nearby `0x8030DBD0` routine is a reboot path, not this getter.
- **Stack completion includes inlined firmware queries.** Leaving firmware querying as an external call made the stack driver `0xB0` instead of `0x144`. Reconstructing the real version query and messages restores the native extent and instruction stream apart from six pooled message addresses.
