# WUD device lists and Bluetooth events

## F-504-13: Remaining list operations, vendor events and stored keys

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. Five functions add 2,700 reconstructed function bytes with Wii/1.0 and unchanged `cflags_sdk`.

| Function | Address | Extent | Non-relocation result |
| --- | --- | --- | --- |
| `WUDiMoveTopOfDisconnectedSmpDevice` | `0x8030E9E0` | `0x168` | Exact |
| `WUDiMoveTopOfDisconnectedStdDevice` | `0x8030ED90` | `0x190` | Exact |
| `WUDiMoveTopOfUnusedStdDevice` | `0x8030EF20` | `0x198` | Exact |
| `WUDVendorSpecificCallback` | `0x8030F8F0` | `0x2A8` | Four identical-message address differences |
| `__wudLinkKeyEventStackCallback` | `0x8030FBF0` | `0x354` | Fourteen identical-message address differences |

All 94 relocation entries agree with the original by relocation type and instruction opcode/registers. Calls and callbacks agree by symbol; BSS operands select the same actual offset; small-data operands select the same named storage or byte-identical message. Direct initialized-data references select either the exact HID descriptor/base or an identical message. Every remaining non-relocation difference is a folded `addi` address to an identical message.

## Behavior

The linked-list operations reorder existing nodes while interrupts are disabled. Simple devices move before the first other disconnected entry. Standard devices also preserve the linked Balance Board condition when deciding where to insert. Unused standard-device insertion handles the tail separately, preserving its next pointer and tail update. These are real typed list operations, with complete original instruction streams.

Vendor event 8 invokes the standard synchronization callback with `(0, 0)` when available, otherwise starts ordinary synchronization. Event 9 invokes deletion notification with busy status or starts the timed deletion driver. Event 10 stores the serial-port status byte. Event 16 calls `OSPanic` with target file `WUD.c`, line 5342 and the original module-fatal message. The target retains only the private sync/delete helper messages, not the older reference's extra `VSE` event messages.

The stored-key callback iterates returned address/key pairs, associates them with known devices or allocates free records, and preserves the target's controller/NAND storage-state updates. It copies the last paired address when a known entry is found. Read/write/delete completion events reset the key-command state and retain all original diagnostics. An unexpected event calls `OSPanic` with `WUD.c` and line 5498. These literal line values come from original operands, not a reference's source line numbering.

`WUDStartClearDevice` is reconstructed as a functional inline helper: the native inventory has no separate retained entry. Its alarm setup and library/busy checks are inlined into vendor-event handling. The standard synchronization branch calls the existing retained `__wudStartSyncDevice` with the original four arguments. No extra native functions are counted.

## Validation and remaining work

Actual `all_source` compilation now retains 71 functions / 18,240 bytes with original metadata extents. Fifty-nine bodies / 8,584 bytes have exact non-relocation streams. Twelve partial bodies retain 102 folded addresses to identical data. All 340 baseline source objects remain byte-identical. Whole-unit diagnostics: text 72.735016%, data 5.473637%, BSS 100%, sdata 77.77778%, sbss 47.16981%, sdata2 100%.

Full Ninja exits zero before explicit frozen verification of `build/504-source-header-refresh-1/wpse-v1`. WUD remains `NonMatching`; its original object is selected, contributing zero linked WUD source credit. Fifteen native initialization and firmware-patch functions remain, along with complete initialized/small-data layout and final relocated-byte acceptance.

## Provenance

List operations and linked-board conditions follow target-corroborated [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. Functional event handlers and diagnostics follow [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found. Target operands correct panic lines, private helper names and reference-only messages. Comments and annotations are removed. No decompiler text is included.

## Traps

- **Panic metadata is revision-specific.** Reference line numbers differ from this target. Read the original file operand and immediate line value before reconstructing the call; this target uses `WUD.c` at 5342 and 5498 for these two branches.
- **Reference event logs can change register lifetimes.** Older vendor-event messages are absent here. Keeping them preserved the input pointer across calls and changed registers throughout the callback. Removing only the proven extra messages, and reconstructing the actual inline clear-device helper, restores the original extent and registers.
