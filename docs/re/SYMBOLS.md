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

## Issue 510: BTE transport, initialization and logging

[F-510-1](findings/510-bte-transport-logging.md) records the complete recovered function ranges, data addresses, ABI confidence and outstanding shared-symbol dependencies. Names are supported by Petari's BTE reference and target instruction/call structure. bte_hcisu_send/task/close and bta_ci_hci_msg_handler occupy 0x80313A60-0x80313B30; BTE_InitStack is at 0x80313B30; LogMsg and its seven fixed-argument wrappers occupy 0x80313B60-0x80313D00. BTUInterruptHandler (0x80313D00), BTA_Init (0x80313D20), and BTA_CleanUp (0x80313E00) begin bte_main.c, which continues into issue #511.

The two transport pointers are at 0x804B9B78 and 0x804B9B7C. The header and callback offsets, and the distinction between reference-derived declarations and target-observed accesses, are recorded in the finding. The logging allocation at 0x804A9040 includes unresolved alignment padding and must not yet be treated as source-link matched.

[F-510-2](findings/510-bte-transport-logging.md#f-510-2-complete-main-unit-split-and-remaining-three-functions) establishes bte_main.c ending at 0x80313E70, including the #511 callback. All three #510 main functions now compare at 100% with the target. Recovered original names for its data are __BTUInterruptHandlerStack (.bss 0x804A9820, 0x1000 bytes), _bte_alarm (.bss 0x804AA820, 0x30 bytes), bte_hcisu_h2_cfg (.sdata 0x804B8EE8, four observed bytes plus four split padding bytes), bte_target_mode (.sbss 0x804B9B80, one byte), and _bte_app_info (.sbss 0x804B9B84, four-byte callback pointer). Confidence is high from the pinned reference and matching access patterns. OSAlarm/OSContext remain opaque; the transport configuration storage is viewed as two unsigned shorts without assigning unknown field meanings. These data are not yet defined by source.

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

## RVL_SDK nand

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x802E4630` | `0x108` | `fn_802E4630` | `NANDCreate` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4740` | `0xF0` | `fn_802E4740` | `NANDPrivateCreate` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4830` | `0x118` | `fn_802E4830` | `NANDCreateAsync` | function | medium | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4950` | `0x100` | `fn_802E4950` | `NANDPrivateCreateAsync` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4A50` | `0xB0` | `fn_802E4A50` | `NANDDelete` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4B00` | `0x98` | `fn_802E4B00` | `NANDPrivateDelete` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4BA0` | `0xD8` | `fn_802E4BA0` | `NANDDeleteAsync` | function | medium | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4C80` | `0xC0` | `fn_802E4C80` | `NANDPrivateDeleteAsync` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4D40` | `0x68` | `fn_802E4D40` | `NANDRead` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4DB0` | `0x78` | `fn_802E4DB0` | `NANDReadAsync` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4E30` | `0x68` | `fn_802E4E30` | `NANDWrite` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4EA0` | `0x78` | `fn_802E4EA0` | `NANDWriteAsync` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4F20` | `0x98` | `fn_802E4F20` | `NANDSeek` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E4FC0` | `0xA8` | `fn_802E4FC0` | `NANDSeekAsync` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E5070` | `0x110` | `fn_802E5070` | `NANDCreateDir` | function | medium | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E5180` | `0x118` | `fn_802E5180` | `NANDCreateDirAsync` | function | medium | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E52A0` | `0x100` | `fn_802E52A0` | `NANDPrivateCreateDirAsync` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E53A0` | `0x1A8` | `fn_802E53A0` | `nandMove` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E5550` | `0x80` | `fn_802E5550` | `NANDMoveAsync` | function | medium | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E55D0` | `0x88` | `fn_802E55D0` | `NANDGetLength` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E5660` | `0x88` | `fn_802E5660` | `NANDTell` | function | medium | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E56F0` | `0x54` | `fn_802E56F0` | `nandComposePerm` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E5750` | `0x8C` | `fn_802E5750` | `nandSplitPerm` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E57E0` | `0x154` | `fn_802E57E0` | `nandGetStatus` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E5940` | `0x74` | `fn_802E5940` | `nandGetStatusCallback` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E59C0` | `0x64` | `fn_802E59C0` | `NANDGetStatus` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E5A30` | `0x64` | `fn_802E5A30` | `NANDPrivateGetStatus` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E5AA0` | `0x84` | `fn_802E5AA0` | `NANDPrivateGetStatusAsync` | function | high | F-490-2 | `src/RVL_SDK/nand/nand.c` |
| `0x802E5B30` | `0x84` | `fn_802E5B30` | `NANDGetHomeDirUsageAsync` | function | medium | F-490-2 | `src/RVL_SDK/nand/nand.c` |

`NANDCommandBlock` follows Petari's layout. Offsets confirmed by this unit: `callback` 0x4, `status` 0x14, `attr` 0x20, `ownerAcc` 0x24, `groupAcc` 0x28, `othersAcc` 0x2C. `NANDStatus`: `ownerId` 0x0, `groupId` 0x4, `attribute` 0x6, `permission` 0x7. `NANDFileInfo.fileDescriptor` is at 0x0. `ISFSFileStats`: `size` 0x0, `offset` 0x4. Remaining fields are unverified for this game; confidence medium.

## ENC utility dependencies (#493)

[F-493-1](findings/493-enc-utility-names.md) establishes `ENCiCheckParameters` (`0x802EA030`, `0xF8` bytes), `ENCiCheckBreakType` (`0x802EA130`, `0x34` bytes), and `ENCiWriteBreakType` (`0x802EA170`, `0x100` bytes), with call signatures, source provenance, confidence and DOL offset mapping. This names-only change enables source linking for the #494 Latin unit; utility functions remain original fallback.

## Issue 477: GX framebuffer and lighting

[F-477-1](findings/477-gx-framebuf-light.md) records all 30 function names, code ranges, DOL offset mapping, light-object layout and external dependencies. GXFrameBuf owns 0x802CD440-0x802CDEC0; GXLight owns 0x802CDEC0-0x802CE630. Names are high confidence. Both units remain NonMatching pending external names and complete data/source-link validation.

## AX command-list output mode (#486)

`__AXClMode` is a global unsigned 32-bit output mode at `.sbss:0x804B98B0`, extent four bytes, formerly `lbl_804B98B0`. It has no file-backed DOL offset. Original spelling and type have high confidence from the AXCL reference, target initialization, setter, command dispatch, and AXAux accesses; see [F-486-1](findings/486-axcl.md). No source implementation is accepted by this symbol-only change.
