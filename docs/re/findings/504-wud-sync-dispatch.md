# WUD synchronization dispatcher

## F-504-11: Sync driver, NAND initiators and fiber wrapper

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. The native `__wudSyncHandler` at `0x8030ABC0` has extent `0xAE4`; `__wudSyncHandler0` at `0x8030B6B0` has extent `0x20`. The source retains both extents with Wii/1.0 and unchanged `cflags_sdk`.

The dispatcher implements discovery delay/search/result handling, incoming connections, existing/unknown device preparation, registration, simple/standard pairing, stored-key persistence, settings flush, completion/error and the four Wii Fit NAND stages. Cases 6, 13 and 26 remain actionless. It preserves the target's warning default and unsigned byte state selection. The result path does not read the write-only sync-target byte; that extra condition belongs to another reference revision.

The dispatcher differs in exactly 16 non-relocation words. Every difference is an `addi` with identical opcode and registers, addressing a byte-identical message in the incomplete source pool. The wrapper's non-relocation words are all exact. A paired audit checks every relocation of both bodies: 111 in the dispatcher and 5 in the wrapper. Relocation types, instruction opcodes and registers agree; call/callback names agree; BSS operands have the same offsets; small-data references select the same named storage or literal bytes. These observations do not establish final linked acceptance.

## NAND operations and storage

The open path is `/title/00010004/52464e4a/data/RPHealth.dat`, with write access. Seek uses `0x4AF18`; write uses the typed pairing record and 132 bytes. The operation initiators set the lock byte before their asynchronous call and retain each original diagnostic, including address/key/name/checksum output. Their completions are recorded in [F-504-10](504-wud-settings-callbacks.md).

| Object | Target address | Native BSS offset | Canonical size |
| --- | --- | --- | --- |
| `sWudWiiFitFileInfo` | `0x8047D120` | `0xE00` | `0x8C` |
| `sWudWiiFitCommandBlock` | `0x8047D1AC` | `0xE8C` | `0xBC` |
| `sWudHandlerStack` | `0x8047D280` | `0xF60` | `0x1000` |

The source uses the existing `NANDFileInfo` and `NANDCommandBlock` declarations. The command block ends at `0x8047D268`; 24 alignment bytes precede the 32-byte-aligned stack. There is no alignment gap between the file info and command block. The following `0x208` bytes at `0x8047E280` remain coarse metadata pending their own references. The fiber wrapper supplies the top of this stack to `OSSwitchFiberEx`.

The stored-key callback at `0x8030FBF0` is named `__wudLinkKeyEventStackCallback` from paired dispatch references and the reference implementation; its body remains unfinished. The outside callee at `0x8031DA8C` receives the evidenced name `BTM_WriteStoredLinkKey`, without changing its owning split, extent or type.

## Validation and remaining work

The actual `all_source` target compiles 58 retained functions totaling 13,308 bytes, all with original metadata extents. Fifty bodies / 6,356 bytes have exact non-relocation streams. Eight partial bodies have 71 folded addresses to identical data, including this dispatcher's 16. The existing HID descriptor and reset command still supply 245 exact initialized bytes. The other 340 main-branch source objects are byte-identical to the baseline.

Whole-unit objdiff: text 53.06361%, data 5.473637%, BSS 93.942924%, sdata 37.037037%, sbss 47.16981%, sdata2 100%. Full Ninja exits zero, followed by explicit frozen verification of `build/504-source-header-refresh-1/wpse-v1`; `main.dol` matches SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

WUD remains `NonMatching`, with the original WUD object selected and zero linked WUD source credit. Finish the remaining 28 native functions, data layout, folded addresses and all relocated bytes before acceptance. EUART and USB remain source-selected and exact.

## Provenance

Functional discovery/pairing/dispatch helpers and retained messages follow [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found. Wii Fit initiators, updated search behavior and separate standard-device allocation follow target-corroborated [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. Comments and annotations are removed. No decompiler output is included.

## Traps

- **Inlining boundaries can explain register differences.** Combining standard-device search into the interrupt-protected allocator produced the same dispatcher size but different registers through the simple-pairing case. Restoring the reference's separate standard-device helper reproduces those registers naturally and preserves all earlier function extents and raw streams.
- **Canonical structure size needs a compiled check.** The compiled NAND command block is `0xBC`, not an estimated `0xB8`. Its observed start plus canonical size leaves 24 bytes before the aligned stack. Preserve the existing header layout and record actual offsets before subdividing coarse BSS metadata.
