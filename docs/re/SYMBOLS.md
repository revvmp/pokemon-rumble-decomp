# Symbols and interfaces

## CNT (#286)

For `wpse-v1` / `main.dol`, [F-286-1](findings/286-cnt.md) establishes `CNTInit` (`0x801778F0`, `0x44` bytes), `CNTShutdown` (`0x80177940`, 4 bytes), `CNTReleaseHandle` (`0x80178000`, `0x68` bytes), `CNTOpen` (`0x80178070`, `0x33C` bytes), `CNTGetLength` (`0x801784A0`, `0x30` bytes), and `CNTClose` (`0x801784D0`, `0x5C` bytes). All are `.text`; subtract `0x8004FD40` for DOL file offsets. Names are high confidence from behavior, embedded diagnostics, and public CNT interface corroboration. Reconstructed source destination is `src/RVL_SDK/cnt/cnt.c`.

The descriptive function names `CNTReadAt` (`0x801783B0`, `0xF0` bytes) and `contentConvertErrorCode` (`0x80177D20`, `0x150` bytes) describe offset reading and ES error conversion; original spellings remain unknown. [F-286-2](findings/286-cnt.md#f-286-2-handle-release-offset-reads-and-file-close) records the handle layout, external call addresses and partial comparison results.

The descriptive local names `sCntVersion` (`.sdata`, `0x804B80F0`, 4 bytes) and `sCntInitialized` (`.sbss`, `0x804B9100`, first 4 bytes of an 8-byte padded range) describe the banner pointer and one-time initialization flag. These original spellings are unknown. File-info layout and calling conventions are recorded in F-286-1 and represented in `include/RVL_SDK/cnt.h`; the header is private to this reconstruction so far.


For each recovered symbol record target, artifact, section, runtime address, file offset, extent, original label if present, chosen name, kind, confidence, evidence ID, and source destination. Distinguish known bounds from inferred ones.

For each shared type or interface record size, alignment, field offsets, parameter and return conventions, signedness, ownership evidence, and unresolved questions. Keep target-specific address mappings separate from shared declarations. Rename speculative labels only when evidence supports the change and preserve traceability.

## RVL_SDK PPCArch

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x802B1830` | `0x8` | `fn_802B1830` | `PPCMthid0` | function | high | F-463-1 | `src/RVL_SDK/base/PPCArch.c` |
| `0x802B1870` | `0x8` | `fn_802B1870` | `PPCSync` | function | high | F-463-1 | `src/RVL_SDK/base/PPCArch.c` |

## Issue 466: OSContext

[F-466-1](findings/466-oscontext.md) records the `wpse-v1` OSContext unit, all five recovered function names and extents, file-offset mapping, and the 0x2C8-byte context ABI. The added `include/revolution/os/OSContext.h` follows the reference layout, with every saved register offset corroborated by target loads and stores. Absolute current-context/FPU-context pointers are at `0x800000D4`/`0x800000D8`. Source and data compare at 100% in objdiff; the external compiler conversion-helper name remains unmodified pending its owner.

## Issue 475: GX FIFO

[F-475-1](findings/475-gxfifo.md) lists all nine functions in `0x802CACE0–0x802CB6C0`, their exact extents and high-confidence original SDK names. The DOL file offset is runtime address minus `0x8004FD40`; source destination is `src/RVL_SDK/gx/GXFifo.c`. `GXSetBreakPtCallback` at `0x802CB450` returns/replaces the callback under interrupt protection. Internal state is 36 bytes; field offsets and the public 128-byte FIFO ABI are recorded in the finding and `include/RVL_SDK/gx/GXFifo.h`.

GPFifo is at `.bss:0x80454200`, CPUFifo at `0x80454224`; each uses 36 bytes. `.sbss` contains CPUFifoReady (`0x804B96C0`, byte), GPFifoReady (`0x804B96C1`, byte), __GXOverflowCount (`0x804B96C4`, word), __GXCurrentBP (`0x804B96C8`, pointer), BreakPointCB (`0x804B96CC`, function pointer), GXOverflowSuspendInProgress (`0x804B96D0`, BOOL), __GXCurrentThread (`0x804B96D4`, pointer), and CPGPLinked (`0x804B96D8`, byte plus dtk padding). Names follow reference source and are corroborated by target use. Existing dtk symbol extents include padding and remain unchanged; they do not enlarge the C objects.

## Issue 510: BTE transport, initialization and logging

[F-510-1](findings/510-bte-transport-logging.md) records the complete recovered function ranges, data addresses, ABI confidence and outstanding shared-symbol dependencies. Names are supported by Petari's BTE reference and target instruction/call structure. bte_hcisu_send/task/close and bta_ci_hci_msg_handler occupy 0x80313A60-0x80313B30; BTE_InitStack is at 0x80313B30; LogMsg and its seven fixed-argument wrappers occupy 0x80313B60-0x80313D00. BTUInterruptHandler (0x80313D00), BTA_Init (0x80313D20), and BTA_CleanUp (0x80313E00) begin bte_main.c, which continues into issue #511.

The two transport pointers are at 0x804B9B78 and 0x804B9B7C. The header and callback offsets, and the distinction between reference-derived declarations and target-observed accesses, are recorded in the finding. The logging allocation at 0x804A9040 includes unresolved alignment padding and must not yet be treated as source-link matched.

[F-510-2](findings/510-bte-transport-logging.md#f-510-2-complete-main-unit-split-and-remaining-three-functions) establishes bte_main.c ending at 0x80313E70, including the #511 callback. All three #510 main functions now compare at 100% with the target. Recovered original names for its data are __BTUInterruptHandlerStack (.bss 0x804A9820, 0x1000 bytes), _bte_alarm (.bss 0x804AA820, 0x30 bytes), bte_hcisu_h2_cfg (.sdata 0x804B8EE8, four observed bytes plus four split padding bytes), bte_target_mode (.sbss 0x804B9B80, one byte), and _bte_app_info (.sbss 0x804B9B84, four-byte callback pointer). Confidence is high from the pinned reference and matching access patterns. OSAlarm/OSContext remain opaque; the transport configuration storage is viewed as two unsigned shorts without assigning unknown field meanings. These data are not yet defined by source.

## Issue 514: Bluetooth device manager public API

[F-514-1](findings/514-bta-dm-api.md) records all eleven recovered `.text` names, exact extents, prior dtk labels, DOL mapping, reference confidence, message layouts and external dependencies for `wpse-v1` / `main.dol`. Destination is `src/RVL_SDK/bte/bta_dm_api.c`; the private header is `include/RVL_SDK/bte/dm_api.h`. The 1,132-byte unit and sixteen registration bytes are split completely, but source remains NonMatching. Ten wrappers are implemented; the initializer awaits the control-block owner. Reference-derived inquiry internals are distinguished from target-observed copy sizes in the finding.

## ESP and IPC main (#496)

[F-496-1](findings/496-esp-ipc-main.md) records the instruction and Petari evidence. Recovered function names in address order are ESP_LaunchTitle (0x802F0990), ESP_OpenContentFile (0x802F0A30), ESP_ReadContentFile (0x802F0AA0), ESP_SeekContentFile (0x802F0B40), ESP_CloseContentFile (0x802F0BD0), ESP_GetTicketViews (0x802F0C50), ESP_GetTmdView (0x802F0F10), ESP_GetConsumption (0x802F1160), and IPCReInit (0x802F1280). All are high-confidence original SDK names supported by the reference implementation, or the loader's content-operation strings plus ioctl behavior. Existing function extents are preserved.

IPC statics: Initialized at 0x804B99D8 is one byte; IPCCurrentBufferLo at 0x804B99DC, IPCCurrentBufferHi at 0x804B99E0, IPCBufferLo at 0x804B99E4, and IPCBufferHi at 0x804B99E8 are four-byte pointers. They are local to ipcMain.c. IPC register index and register data are unsigned 32-bit values; accesses use the absolute volatile register array at 0xCD000000.

ESP IOS vectors contain a four-byte pointer at offset 0 and unsigned length at offset 4. Ticket views have extent 0xD8; ticket inputs have extent 0x2A4. ESP titles/tickets use 64-bit identifiers, and consumption entries contain two unsigned 32-bit fields. Types in the unit's header follow the CC0 reference and its four-byte packing. External names awaiting neighbor-owned changes are explicitly listed in F-496-1.

## VI I2C (#473)

Target `wpse-v1`, `.text` and associated small-data sections in `main.dol`; source destination `src/RVL_SDK/vi/i2c.c`. [F-473-1](findings/473-vi-i2c.md) establishes the original Petari names with high confidence from source structure, matching non-relocation instruction words, and matching data references.

| Original label | Recovered name | Runtime address | Extent | Kind |
| --- | --- | --- | --- | --- |
| `fn_802C6A30` | `WaitMicroTime` | `0x802C6A30` | `0x88` | Function |
| `fn_802C6AC0` | `sendSlaveAddr` | `0x802C6AC0` | `0x348` | Static function |
| `fn_802C6E10` | `__VISendI2CData` | `0x802C6E10` | `0x534` | Function |
| `lbl_804B8B00` | `__i2c_ident_flag` | `0x804B8B00` | 4 | Static volatile u32 |
| `lbl_804B9678` | `__i2c_ident_first` | `0x804B9678` | 4 | Static volatile u32, BSS |

`WaitMicroTime` takes signed 32-bit microseconds; `__VISendI2CData` takes an unsigned 8-bit slave address, byte-buffer pointer, and signed 32-bit byte count and returns signed 32-bit success. `OSTime` is signed 64-bit. The register accesses are volatile unsigned 32-bit words. The header is unit-local and does not establish shared OS structure layouts. Trailing four-byte data alignment is excluded from each variable's extent.

DOL file offsets: `WaitMicroTime` `0x276CF0`, `sendSlaveAddr` `0x276D80`, `__VISendI2CData` `0x2770D0`, `__i2c_ident_flag` `0x3E5740`. `__i2c_ident_first` is BSS and has no file-backed bytes.

## OSAlarm, wpse-v1

[F-464-1](findings/464-os-alarm.md) records exact section ranges, file offsets, recovered function names, provenance, and the descriptive name for the newer final routine. Confidence is high for layouts and behavior; that final routine's original name remains unknown.

`OSAlarm` has 8-byte alignment and size 48: handler pointer 0x00, unsigned-long tag 0x04, signed 64-bit fire time 0x08, previous and next pointers 0x10/0x14, signed 64-bit period 0x18 and start 0x20, userData pointer 0x28, and trailing alignment padding. The handler takes OSAlarm* and OSContext*. Pointer and unsigned-long widths are 32 bits. The queue contains head/tail pointers, totaling eight bytes.

The private OSContext layout follows the reference and the exception entry's stores: 32 GPRs at 0x00, CR/LR/CTR/XER at 0x80–0x8C, 32 doubles at 0x90, FPSCR padding/value 0x190/0x194, SRR0/SRR1 at 0x198/0x19C, 16-bit mode/state at 0x1A0/0x1A2, eight GQRs at 0x1A4, PSF padding at 0x1C4, and 32 doubles at 0x1C8. Size is 0x2C8, alignment eight. The shutdown registration record is 16 bytes: callback, priority, next, previous, each four bytes. These dependent declarations remain private to OSAlarm.c until their owning units establish shared headers.

## AX initialization, issue #484

`wpse-v1`, `main.dol`: [F-484-1](findings/484-ai.md) records AXInit at `.text:0x802DA490` (0x54 bytes), AXIsInit at `.text:0x802DA4F0` (0x8 bytes), __AXVersion at `.sdata:0x804B8BD0`, and __AXInitialized at `.sbss:0x804B9840`. Placeholder names were the corresponding `fn_` or `lbl_` plus address. AXInit and initializer dependency names come from the CC0 reference; AXIsInit and __AXInitialized are descriptive names. Version and state each contain one four-byte value followed by four-byte split alignment. The header exposes only `void AXInit(void)` and the 32-bit signed flag query `int AXIsInit(void)`; no shared SDK structure is introduced.

The same finding corrects the survey boundary at the end of #483: AIRegisterDMACallback at `0x802D9EE0` owns the beginning of ai.c and references its static callback at `0x804B9838`. No AI symbols or split ranges were changed in this contribution.

## ENC utility dependencies (#493)

[F-493-1](findings/493-enc-utility-names.md) establishes `ENCiCheckParameters` (`0x802EA030`, `0xF8` bytes), `ENCiCheckBreakType` (`0x802EA130`, `0x34` bytes), and `ENCiWriteBreakType` (`0x802EA170`, `0x100` bytes), with call signatures, source provenance, confidence and DOL offset mapping. This names-only change enables source linking for the #494 Latin unit; utility functions remain original fallback.

## Issue 477: GX framebuffer and lighting

[F-477-1](findings/477-gx-framebuf-light.md) records all 30 function names, code ranges, DOL offset mapping, light-object layout and external dependencies. GXFrameBuf owns 0x802CD440-0x802CDEC0; GXLight owns 0x802CDEC0-0x802CE630. Names are high confidence. Both units remain NonMatching pending external names and complete data/source-link validation.

## AX command-list output mode (#486)

`__AXClMode` is a global unsigned 32-bit output mode at `.sbss:0x804B98B0`, extent four bytes, formerly `lbl_804B98B0`. It has no file-backed DOL offset. Original spelling and type have high confidence from the AXCL reference, target initialization, setter, command dispatch, and AXAux accesses; see [F-486-1](findings/486-axcl.md). No source implementation is accepted by this symbol-only change.

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

## RVL_SDK GDTexture and NdevExi2A

Names follow the Dolphin/Revolution SDK API as reconstructed by Petari and the TP decompilation (both CC0). `.sbss` statics in DebuggerDriver.c are file-local.

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x8033E520` | `0x18` | `fn_8033E520` | `GDOverflowed` | function | high | F-535-1 | `src/RVL_SDK/gd/GDBase.c` (#534) |
| `0x804B9B98` | `0x4` | `lbl_804B9B98` | `__GDCurrentDL` | object | high | F-535-1 | `src/RVL_SDK/gd/GDBase.c` (#534) |
| `0x8033E540` | `0x260` | `fn_8033E540` | `GDSetTexLookupMode` | function | high | F-535-1, F-0007 | `src/RVL_SDK/gd/GDTexture.c` |
| `0x8033E7A0` | `0xF8` | `fn_8033E7A0` | `GDSetTexImgAttr` | function | high | F-535-1, F-0007 | `src/RVL_SDK/gd/GDTexture.c` |
| `0x8033E8A0` | `0xE8` | `fn_8033E8A0` | `GDSetTexImgPtr` | function | high | F-535-1, F-0007 | `src/RVL_SDK/gd/GDTexture.c` |
| `0x8033E990` | `0x204` | `fn_8033E990` | `GDSetTexCached` | function | high | F-535-1 | `src/RVL_SDK/gd/GDTexture.c` |
| `0x8033EBA0` | `0x314` | `fn_8033EBA0` | `GDSetTexCoordScale` | function | high | F-535-1 | `src/RVL_SDK/gd/GDTexture.c` |
| `0x804B8F48`–`0x804B8F70` | `0x8` each | `lbl_804B8F48`–`lbl_804B8F70` | `GDTexMode0Ids`, `GDTexMode1Ids`, `GDTexImage0Ids`, `GDTexImage1Ids`, `GDTexImage2Ids`, `GDTexImage3Ids` | objects | high | F-535-1 | `src/RVL_SDK/gd/GDTexture.c` |
| `0x804B8F78` | `0x6` | `lbl_804B8F78` | `GD2HWFiltConv` | object | high | F-535-1 | `src/RVL_SDK/gd/GDTexture.c` |
| `0x8033EEB4`–`0x8033F1E4` | | `fn_8033EEB4`–`fn_8033F1E4` | `__DBMtrHandler`, `__DBIntrHandler`, `DBInitComm`, `DBInitInterrupts`, `DBQueryData`, `DBRead`, `DBWrite`, `DBOpen`, `DBClose` | functions | high | F-535-1 | `src/RVL_SDK/NdevExi2A/DebuggerDriver.c` |
| `0x804B8F80` | `0x1` | `lbl_804B8F80` | `l_byOffsetCounter` | local object | high | F-535-1 | `src/RVL_SDK/NdevExi2A/DebuggerDriver.c` |
| `0x804B9BA0`–`0x804B9BB0` | | `lbl_804B9BA0`–`lbl_804B9BB0` | `__DBMtrCallback`, `__DBDbgCallback`, `__DBEXIInputFlag`, `__DBRecvMail`, `__DBRecvDataSize` | local objects | high | F-535-1 | `src/RVL_SDK/NdevExi2A/DebuggerDriver.c` |
| `0x8033F1E8`–`0x8033F894` | | `fn_8033F1E8`–`fn_8033F894` | `__EXI2Imm`, `__DBEXIInit`, `__DBEXIReadReg`, `__DBEXIWriteReg`, `__DBEXIReadRam`, `__DBEXIWriteRam` | functions | high | F-535-1 | `src/RVL_SDK/NdevExi2A/exi2.c` |

## ENC Latin (#494)

[F-494-1](findings/494-enc-latin.md) records 22 high-confidence names at `.text` `0x802ED120–0x802ED700`, all twenty map symbols and their addresses at `.rodata` `0x8037BB90–0x8037D690`, DOL offset mapping, exact function extents, ABI and four-byte reverse-hash layout. Source destination is `src/RVL_SDK/enc/enclatin.c`; declarations are under `include/revolution/enc/`. The previous labels were `fn_<address>` and `lbl_<address>`; symbol records preserve every original address and size.

## RVL_SDK ARC (`wpse-v1`)

Thirteen functions at 0x802EFCD0–0x802F08D8 in `.text`, named from Petari's `arc.c` and the unit's assert and warning strings; high confidence; evidence [F-495-1](findings/495-arc-unit.md); source `src/RVL_SDK/arc/arc.c`. `_current_locale` (`.data` 0x803E6228, 0x48 bytes, MSL) is named because `ARCConvertPathToEntrynum` references it through the inlined `tolower`.

| Type | Size | Fields (offset) |
| --- | --- | --- |
| `ARCHeader` | 0x20 | `magic` 0x0 (`0x55AA382D`), `fstStart` 0x4, `fstSize` 0x8, `fileStart` 0xC, `reserved[4]` 0x10 |
| `ARCHandle` | 0x1C | `archiveStartAddr` 0x0, `FSTStart` 0x4, `fileStart` 0x8, `entryNum` 0xC, `FSTStringStart` 0x10, `FSTLength` 0x14, `currDir` 0x18 |
| `ARCFileInfo` | 0xC | `handle` 0x0, `startOffset` 0x4, `length` 0x8 |
| `ARCDir` | 0x10 | `handle` 0x0, `entryNum` 0x4, `location` 0x8, `next` 0xC |
| `ARCDirEntry` | 0x10 | `handle` 0x0, `entryNum` 0x4, `isDir` 0x8, `name` 0xC |
| `__locale` (MSL) | 0x48 | `ctype_cmpt_ptr` 0x38; `_loc_ctype_cmpt::lower_map_ptr` 0x10 |

## Issue records

- AXFX ReverbStdExp, ChorusExp, LfoTable, SrcCoef, Hooks, and MEM heapCommon and expHeap (`0x802DF190`–`0x802E18F0`), with the external OS, MEM list, and MSL names they link against: [issue 488 symbols](findings/488-axfx-mem-units.md#symbols).
