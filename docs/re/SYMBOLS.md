# Symbols and interfaces

## CNT (#286)

For `wpse-v1` / `main.dol`, [F-286-1](findings/286-cnt.md) establishes `CNTInit` (`0x801778F0`, `0x44` bytes), `CNTShutdown` (`0x80177940`, 4 bytes), `CNTReleaseHandle` (`0x80178000`, `0x68` bytes), `CNTOpen` (`0x80178070`, `0x33C` bytes), `CNTGetLength` (`0x801784A0`, `0x30` bytes), and `CNTClose` (`0x801784D0`, `0x5C` bytes). All are `.text`; subtract `0x8004FD40` for DOL file offsets. Names are high confidence from behavior, embedded diagnostics, and public CNT interface corroboration. Reconstructed source destination is `src/RVL_SDK/cnt/cnt.c`.

The descriptive local names `sCntVersion` (`.sdata`, `0x804B80F0`, 4 bytes) and `sCntInitialized` (`.sbss`, `0x804B9100`, first 4 bytes of an 8-byte padded range) describe the banner pointer and one-time initialization flag. These original spellings are unknown. File-info layout and calling conventions are recorded in F-286-1 and represented in `include/RVL_SDK/cnt.h`; the header is private to this reconstruction so far.

For each recovered symbol record target, artifact, section, runtime address, file offset, extent, original label if present, chosen name, kind, confidence, evidence ID, and source destination. Distinguish known bounds from inferred ones.

For each shared type or interface record size, alignment, field offsets, parameter and return conventions, signedness, ownership evidence, and unresolved questions. Keep target-specific address mappings separate from shared declarations. Rename speculative labels only when evidence supports the change and preserve traceability.

## RVL_SDK PPCArch

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x802B1830` | `0x8` | `fn_802B1830` | `PPCMthid0` | function | high | F-463-1 | `src/RVL_SDK/base/PPCArch.c` |
| `0x802B1870` | `0x8` | `fn_802B1870` | `PPCSync` | function | high | F-463-1 | `src/RVL_SDK/base/PPCArch.c` |

## Issue 510: BTE transport, initialization and logging

[F-510-1](findings/510-bte-transport-logging.md) records the complete recovered function ranges, data addresses, ABI confidence and outstanding shared-symbol dependencies. Names are supported by Petari's BTE reference and target instruction/call structure. bte_hcisu_send/task/close and bta_ci_hci_msg_handler occupy 0x80313A60-0x80313B30; BTE_InitStack is at 0x80313B30; LogMsg and its seven fixed-argument wrappers occupy 0x80313B60-0x80313D00. BTUInterruptHandler (0x80313D00), BTA_Init (0x80313D20), and BTA_CleanUp (0x80313E00) begin bte_main.c, which continues into issue #511.

The two transport pointers are at 0x804B9B78 and 0x804B9B7C. The header and callback offsets, and the distinction between reference-derived declarations and target-observed accesses, are recorded in the finding. The logging allocation at 0x804A9040 includes unresolved alignment padding and must not yet be treated as source-link matched.

## Issue 467: OSFont, OSInterrupt, OSMessage, OSMemory

Evidence and confidence are in [F-467-2](findings/467-os-font-interrupt-memory.md#f-467-2-names). Every symbol below was renamed in `config/wpse-v1/symbols.txt`.

| Address | Size | dtk label | Chosen name | Kind | Confidence | Source |
| --- | --- | --- | --- | --- | --- | --- |
| `0x802B7900` | `0x138` | `fn_802B7900` | `GetFontCode` | static function | high | `src/RVL_SDK/os/OSFont.c` |
| `0x802B7A40` | `0x1A8` | `fn_802B7A40` | `Decode` | static function | high | `src/RVL_SDK/os/OSFont.c` |
| `0x802B7BF0` | `0x4C` | `fn_802B7BF0` | `OSGetFontEncode` | function | high | `src/RVL_SDK/os/OSFont.c` |
| `0x802B7CC0` | `0x310` | `fn_802B7CC0` | `ReadFont` | static function | high | `src/RVL_SDK/os/OSFont.c` |
| `0x802B7FD0` | `0x118` | `fn_802B7FD0` | `OSLoadFont` | function | high | `src/RVL_SDK/os/OSFont.c` |
| `0x802B80F0` | `0xF8` | `fn_802B80F0` | `ParseStringS` | static function | high | `src/RVL_SDK/os/OSFont.c` |
| `0x802B81F0` | `0x1B4` | `fn_802B81F0` | `ParseStringW` | static function | high | `src/RVL_SDK/os/OSFont.c` |
| `0x802B83B0` | `0x288` | `fn_802B83B0` | `OSGetFontTexel` | function | high | `src/RVL_SDK/os/OSFont.c` |
| `0x802B8640` | `0x26C` | `fn_802B8640` | `ExpandFontSheet` | static function | medium | `src/RVL_SDK/os/OSFont.c` |
| `0x802B88B0` | `0x1E0` | `fn_802B88B0` | `OSInitFont` | function | medium | `src/RVL_SDK/os/OSFont.c` |
| `0x802B8A90` | `0x148` | `fn_802B8A90` | `OSGetFontTexture` | function | medium | `src/RVL_SDK/os/OSFont.c` |
| `0x802B8BE0` | `0xB0` | `fn_802B8BE0` | `OSGetFontWidth` | function | medium | `src/RVL_SDK/os/OSFont.c` |
| `0x802B8D20` | `0x10` | `fn_802B8D20` | `__OSGetInterruptHandler` | function | high | `src/RVL_SDK/os/OSInterrupt.c` |
| `0x802B9160` | `0x2A8` | `fn_802B9160` | `__OSDispatchInterrupt` | function | high | `src/RVL_SDK/os/OSInterrupt.c` |
| `0x802B9480` | `0x60` | `fn_802B9480` | `OSInitMessageQueue` | function | high | `src/RVL_SDK/os/OSMessage.c` |
| `0x802B94E0` | `0xC8` | `fn_802B94E0` | `OSSendMessage` | function | high | `src/RVL_SDK/os/OSMessage.c` |
| `0x802B95B0` | `0xDC` | `fn_802B95B0` | `OSReceiveMessage` | function | high | `src/RVL_SDK/os/OSMessage.c` |
| `0x802B9690` | `0xD0` | `fn_802B9690` | `OSJamMessage` | function | high | `src/RVL_SDK/os/OSMessage.c` |
| `0x802B9790` | `0x3C` | `fn_802B9790` | `OnShutdown` | static function | high | `src/RVL_SDK/os/OSMemory.c` |
| `0x802B9820` | `0x80` | `fn_802B9820` | `ConfigMEM1_24MB` | static asm function | high | `src/RVL_SDK/os/OSMemory.c` |
| `0x802B98A0` | `0xD4` | `fn_802B98A0` | `ConfigMEM2_52MB` | static asm function | high | `src/RVL_SDK/os/OSMemory.c` |
| `0x802B9980` | `0xD4` | `fn_802B9980` | `ConfigMEM2_56MB` | static asm function | high | `src/RVL_SDK/os/OSMemory.c` |
| `0x802B9A60` | `0xA8` | `fn_802B9A60` | `ConfigMEM2_64MB` | static asm function | high | `src/RVL_SDK/os/OSMemory.c` |
| `0x802B9B10` | `0xD4` | `fn_802B9B10` | `ConfigMEM2_112MB` | static asm function | high | `src/RVL_SDK/os/OSMemory.c` |
| `0x802B9BF0` | `0xA8` | `fn_802B9BF0` | `ConfigMEM2_128MB` | static asm function | high | `src/RVL_SDK/os/OSMemory.c` |
| `0x802B9CA0` | `0x18` | `fn_802B9CA0` | `RealMode` | static asm function | high | `src/RVL_SDK/os/OSMemory.c` |
| `0x802B9DE0` | `0x44` | `fn_802B9DE0` | `__OSRestoreMEM1Config` | function | low, descriptive | `src/RVL_SDK/os/OSMemory.c` |
| `0x803C7F58` | `0x180` | `lbl_803C7F58` | `HankakuToCode` | static u16 table | high | `src/RVL_SDK/os/OSFont.c` |
| `0x803C80D8` | `0x98A` | `lbl_803C80D8` | `Zenkaku2Code` | static u16 table | high | `src/RVL_SDK/os/OSFont.c` |
| `0x803C8A68` | `0x30` | `lbl_803C8A68` | `InterruptPrioTable` | static mask table | high | `src/RVL_SDK/os/OSInterrupt.c` |
| `0x804B8A88` | `0x2` | `lbl_804B8A88` | `FontEncode` | static u16 | high | `src/RVL_SDK/os/OSFont.c` |
| `0x804B94B8` | `0x4` | `lbl_804B94B8` | `ParseString` | static parser pointer | high | `src/RVL_SDK/os/OSFont.c` |
| `0x804B94BC` | `0x4` | `lbl_804B94BC` | `FixedPitch` | static BOOL | high | `src/RVL_SDK/os/OSFont.c` |
| `0x804B94C0` | `0x4` | `lbl_804B94C0` | `FontDataSjis` | static pointer | high | `src/RVL_SDK/os/OSFont.c` |
| `0x804B94C4` | `0x4` | `lbl_804B94C4` | `FontDataAnsi` | static pointer | high | `src/RVL_SDK/os/OSFont.c` |

The types follow the reference and were confirmed by access offsets. `OSFontHeader` is 0x30 bytes: `cellWidth` 0x10, `cellHeight` 0x12, `sheetSize` 0x14, `sheetFormat` 0x18, `sheetColumn` 0x1A, `sheetRow` 0x1C, `sheetWidth` 0x1E, `widthTable` 0x22, `sheetImage` 0x24, `sheetFullSize` 0x28, and color indices `c0` to `c3` at 0x2C. `OSMessageQueue` is 0x20 bytes: send queue 0x00, receive queue 0x08, `msgArray` 0x10, `msgCount` 0x14, `firstIndex` 0x18, `usedCount` 0x1C. `OSContext` keeps `srr0` at 0x198 and GQR1 to GQR7 at 0x1A8 to 0x1C0. `OSShutdownFunctionInfo` is 16 bytes: callback, priority, next, previous. The low-memory words are the user interrupt mask at `0x800000C4`, the OS interrupt mask at `0x800000C8`, the interrupt table at `0x80003040`, the module list head and tail at `0x800030C8` and `0x800030CC`, and the module string table at `0x800030D0`.
