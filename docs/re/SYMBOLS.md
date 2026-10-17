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

## Issue 511: BTA connection notifications

[F-511-1](findings/511-bta-sys-conn.md) records the eight recovered function names, addresses, sizes and DOL mapping for `0x80314388–0x803145DC`. `bta_sys_cb` at `.bss` `0x804AA8D8` (structure `0x8C`, existing padded allocation `0x90`) is owned by `bta_sys_main.c`, whose first function is within #511. The complete control-block layout and reference-derived versus observed fields are described in the finding; the connection unit verifies callback slots at `0x80/0x84/0x88`.

For #514 dependency coordination, `bdcpy` (`0x803142B4`, `0x34` bytes) and `bta_sys_sendmsg` (`0x80314760`, `0x1C` bytes) are also renamed within #511; their original fallback implementations remain. F-511-1 records behavior and reference evidence.


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
## Issue #497: IPC allocator and profiling

[F-497-1](findings/497-ipc-memory-profile.md) records target-disassembly and reference-source evidence. High-confidence recovered names: `IOS_Close` at `0x802F1DF0`, `IOS_IoctlvReboot` at `0x802F2A10`, `__iosAlloc` at `0x802F2E40`, `iosAlloc` at `0x802F3040`, `IPCiProfAck` at `0x802F33A0`, and `IPCiProfReply` at `0x802F33B0`. The eight-entry heap table at `0x804772A0` is `__heaps`; each 16-byte record has base pointer at 0, owner at 4, size at 8, and free-list pointer at 12. Allocator and profiling function names and layouts are corroborated by 100% object code/data comparisons. The two client names are behavioral identifications; their functions still link from fallback.
## RVL_SDK EXI, SI, and DB (#472)

`wpse-v1` `main.dol`, names from matching RVL_SDK reference source and confirmed by byte-identical reconstruction. Confidence high. Evidence and unit boundaries: [F-472-1](findings/472-exi-si-db-units.md#f-472-1-unit-boundaries).

| Address | Size | Name | Kind | Source |
| --- | --- | --- | --- | --- |
| `0x802C23C0` | `0x178` | `ProbeBarnacle` | static function | `src/RVL_SDK/exi/EXIUart.c` |
| `0x802C2700` | `0x188` | `EXIWriteReg` | function | `src/RVL_SDK/exi/EXICommon.c` |
| `0x802C2890` | `0x1C` | `SIBusy` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C28B0` | `0x38` | `SIIsChanBusy` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C28F0` | `0x2F4` | `CompleteTransfer` | static function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C2FE0` | `0x78` | `SIEnablePollingInterrupt` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C3060` | `0xEC` | `SIUnregisterPollingHandler` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C3210` | `0x1B0` | `__SITransfer` | static function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C33C0` | `0x78` | `SIGetStatus` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C3440` | `0x14` | `SISetCommand` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C3460` | `0x10` | `SITransferCommands` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C34D0` | `0x88` | `SIEnablePolling` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C3560` | `0x6C` | `SIDisablePolling` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C35D0` | `0x124` | `SIGetResponse` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C3700` | `0x8C` | `AlarmHandler` | static function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C3D80` | `0x110` | `SIGetTypeAsync` | function | `src/RVL_SDK/si/SIBios.c` |
| `0x802C3F70` | `0x8` | `SIRefreshSamplingRate` | function | `src/RVL_SDK/si/SISamplingRate.c` |
| `0x802C3FB0` | `0x48` | `__DBExceptionDestinationAux` | function | `src/RVL_SDK/db/db.c` |
| `0x802C4080` | `0x198` | `OnShutdown` | static function of `vi.c`, not yet reconstructed | [F-472-2](findings/472-exi-si-db-units.md#f-472-2-fn_802c4080-is-onshutdown-the-first-function-of-vic) |
| `0x804B9598`–`0x804B95A4` | 4 each | `BarnacleEnabled`, `Enabled`, `Dev`, `Chan` | `.sbss` statics | `src/RVL_SDK/exi/EXIUart.c` |
| `0x80453850` | `0x20` | `XferTime` | `.bss` static | `src/RVL_SDK/si/SIBios.c` |
| `0x80453890` | `0xC0` | `Alarm` | `.bss` static | `src/RVL_SDK/si/SIBios.c` |
| `0x80453950` | `0x20` | `InputBuffer` | `.bss` static | `src/RVL_SDK/si/SIBios.c` |
| `0x80453970` | `0x10` | `InputBufferValid` | `.bss` static | `src/RVL_SDK/si/SIBios.c` |
| `0x80453980` | `0x10` | `InputBufferVcount` | `.bss` static | `src/RVL_SDK/si/SIBios.c` |
| `0x80453990` | `0x10` | `RDSTHandler` | `.bss` static | `src/RVL_SDK/si/SIBios.c` |
| `0x804539A0` | `0x10` | `@LOCAL@GetTypeCallback__FlUlP9OSContext@cmdFixDevice` | function static | `src/RVL_SDK/si/SIBios.c` |
| `0x804539B0` | `0x40` | `TypeCallback` | `.bss` static | `src/RVL_SDK/si/SIBios.c` |
| `0x804B95A8` | `0x4` | `@LOCAL@SIInterruptHandler__FsP9OSContext@cmdTypeAndStatus` | function static | `src/RVL_SDK/si/SIBios.c` |
| `0x804B95B4` | `0x4` | `__PADFixBits` | global | `src/RVL_SDK/si/SIBios.c` |
| `0x803D52B8` | `0x30` | `XYPAL` | `.data` static | `src/RVL_SDK/si/SISamplingRate.c` |

Callees renamed outside the block so the units link, identified from the matched call sites: `EXIAttach` `0x802C1470`, `EXIDetach` `0x802C1590`, `OSCancelAlarm` `0x802B3310`, `OSGetWirelessID` `0x802BB590`, `OSSetWirelessID` `0x802BB610`, `VIGetCurrentLine` `0x802C6520`.

`SIControl Si` at `0x803D5260`, size `0x14`: `chan` +0x0 (`s32`), `poll` +0x4, `inputBytes` +0x8, `input` +0xC, `callback` +0x10. `SIPacket`, size `0x20`: `chan`, `output`, `outputBytes`, `input`, `inputBytes`, `callback`, then `fire` (`OSTime`) at +0x18. `DBInterface` at physical `0x40`: `exceptionMask` +0x4, `ExceptionDestination` +0x8. Declarations are in `include/revolution/si.h`, `include/revolution/db.h`, and `src/RVL_SDK/si/SIBios.c`.
