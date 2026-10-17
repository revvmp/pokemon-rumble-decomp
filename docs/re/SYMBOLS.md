# Symbols and interfaces

## OS IPC buffer and time conversion (#470)

For `wpse-v1` / `main.dol`, [F-470-1 and F-470-2](findings/470-os-ipc.md) recover `__OSGetIPCBufferHi` at `.text:0x802BD980` (8 bytes), `__OSGetIPCBufferLo` at `.text:0x802BD990` (8 bytes), and `__OSTimeToSystemTime` at `.text:0x802BD100` (0x58 bytes). These replace the corresponding `fn_` labels with high-confidence original names corroborated by Petari and target data flow. Text file offsets equal address minus `0x8004FD40`. The getters return 32-bit pointers in r3. Time conversion takes and returns signed 64-bit time in r3/r4; its source remains fallback. OSIpc source and its two 4-byte local data objects are described in the finding.

## CNT (#286)

For `wpse-v1` / `main.dol`, [F-286-1](findings/286-cnt.md) establishes `CNTInit` (`0x801778F0`, `0x44` bytes), `CNTShutdown` (`0x80177940`, 4 bytes), `CNTReleaseHandle` (`0x80178000`, `0x68` bytes), `CNTOpen` (`0x80178070`, `0x33C` bytes), `CNTGetLength` (`0x801784A0`, `0x30` bytes), and `CNTClose` (`0x801784D0`, `0x5C` bytes). All are `.text`; subtract `0x8004FD40` for DOL file offsets. Names are high confidence from behavior, embedded diagnostics, and public CNT interface corroboration. Reconstructed source destination is `src/RVL_SDK/cnt/cnt.c`.

The descriptive function names `CNTReadAt` (`0x801783B0`, `0xF0` bytes) and `contentConvertErrorCode` (`0x80177D20`, `0x150` bytes) describe offset reading and ES error conversion; original spellings remain unknown. [F-286-2](findings/286-cnt.md#f-286-2-handle-release-offset-reads-and-file-close) records the handle layout, external call addresses and partial comparison results.

The descriptive local names `sCntVersion` (`.sdata`, `0x804B80F0`, 4 bytes) and `sCntInitialized` (`.sbss`, `0x804B9100`, first 4 bytes of an 8-byte padded range) describe the banner pointer and one-time initialization flag. These original spellings are unknown. File-info layout and calling conventions are recorded in F-286-1 and represented in `include/RVL_SDK/cnt.h`; the header is private to this reconstruction so far.

[F-286-3](findings/286-cnt.md#f-286-3-complete-cnt-reconstruction) adds `contentInitHandleNAND` (`0x80177950`, `0x124`, local), `contentConvertPathToEntrynumDVD` (`0x80177A80`, `0x294`), `entryToPathDVD` (`0x80177E70`, `0x180`, local), and `CNTInitHandle` (`0x80177FF0`, `0xC`). `CNTInitHandle` and `contentConvertPathToEntrynumDVD` are corroborated by the public RB3 CNT header. The other two names are descriptive. The handle and file-info layouts in `include/revolution/cnt.h` are `CNTHandleNAND` (an `ARCHandle` at 0, the ESP descriptor at `0x1C`, the allocator at `0x20`), `CNTHandleDVD` (index, root directory and current directory at 0, 4 and 8), and the type byte at `0x24`. `CNTFileInfo` holds either the NAND handle, start, length and read offset, or a `DVDFileInfo` followed by the read offset at `0x3C`. Its type byte is at `0x40`. External callees named for the link, with high confidence from their bodies and call roles: `DVDFastOpen` (`0x802D1880`), `DVDClose` (`0x802D18F0`), `DVDReadPrio` (`0x802D1920`), `DVDGetFSTLocation` (`0x802D1A60`, returns `BootInfo->FSTLocation`), `MEMAllocFromAllocator` (`0x802E1B90`), `MEMFreeToAllocator` (`0x802E1BA0`).


For each recovered symbol record target, artifact, section, runtime address, file offset, extent, original label if present, chosen name, kind, confidence, evidence ID, and source destination. Distinguish known bounds from inferred ones.

For each shared type or interface record size, alignment, field offsets, parameter and return conventions, signedness, ownership evidence, and unresolved questions. Keep target-specific address mappings separate from shared declarations. Rename speculative labels only when evidence supports the change and preserve traceability.

## RVL_SDK PPCArch

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x802B1830` | `0x8` | `fn_802B1830` | `PPCMthid0` | function | high | F-463-1 | `src/RVL_SDK/base/PPCArch.c` |
| `0x802B1870` | `0x8` | `fn_802B1870` | `PPCSync` | function | high | F-463-1 | `src/RVL_SDK/base/PPCArch.c` |

## Issue 466: OSContext

[F-466-1](findings/466-oscontext.md) records the `wpse-v1` OSContext unit, all five recovered function names and extents, file-offset mapping, and the 0x2C8-byte context ABI. The added `include/revolution/os/OSContext.h` follows the reference layout, with every saved register offset corroborated by target loads and stores. Absolute current-context/FPU-context pointers are at `0x800000D4`/`0x800000D8`. [F-466-2](findings/466-oscontext.md) accepts all four units as Matching after exact linked verification; main now provides the external compiler conversion-helper name.

## Issue 474: matrix units

[F-474-1](findings/474-matrix.md#f-474-1-unit-boundaries-and-source-provenance) establishes these original SDK names through matching implementations. Confidence is high.

| Address | Name | Size |
| --- | --- | --- |
| `0x802C9590` | `PSMTXMultVec` | `0x54` |
| `0x802C95F0` | `PSMTXMultVecArray` | `0x8C` |
| `0x802C9680` | `PSMTXMultVecSR` | `0x54` |
| `0x802C96E0` | `C_MTXFrustum` | `0x9C` |
| `0x802C9780` | `C_MTXPerspective` | `0xEC` |
| `0x802C9870` | `C_MTXOrtho` | `0x98` |

`Vec` contains single-precision `x`, `y`, `z` at offsets 0, 4, 8, size 12 and alignment 4. `Mtx` is a row-major 3 by 4 single-precision matrix, size 48; `Mtx44` is 4 by 4, size 64. Both have alignment 4. These layouts follow the observed load/store offsets and the CC0 SDK reference. `C_MTXPerspective` calls the matching reference's `tan` at `0x8034D4B8`; the referenced callee is now named `tan` under the updated outside-symbol rule, with its size `0x78`, type and owning split unchanged (F-474-4).

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

## RVL_SDK nand outside symbols (#490)

Names only, so `nand.c` links from source; splits, sizes and types stay with the owning issues. Evidence in [F-490-5](findings/490-nand.md#f-490-5-outside-symbols-named-and-nandc-links-from-source).

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Owner |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x802E6290` | `0x98` | `fn_802E6290` | `nandGetRelativeName` | function | high | F-490-5 | #491 |
| `0x802E64B0` | `0x58` | `fn_802E64B0` | `nandIsUnderPrivatePath` | function | high | F-490-5 | #491 |
| `0x802E6DD0` | `0x3C` | `fn_802E6DD0` | `nandCallback` | function | high | F-490-5 | #491 |
| `0x802E71C0` | `0xC` | `fn_802E71C0` | `nandGetHomeDir` | function | high | F-490-5 | #491 |
| `0x802F36E0` | `0xF4` | `fn_802F36E0` | `ISFS_CreateDir` | function | high | F-490-5 | #498 |
| `0x802F37E0` | `0xF4` | `fn_802F37E0` | `ISFS_CreateDirAsync` | function | high | F-490-5 | #498 |
| `0x802F3B90` | `0x158` | `fn_802F3B90` | `ISFS_GetAttr` | function | high | F-490-5 | #498 |
| `0x802F3CF0` | `0x134` | `fn_802F3CF0` | `ISFS_GetAttrAsync` | function | high | F-490-5 | #498 |
| `0x802F3E30` | `0xD8` | `fn_802F3E30` | `ISFS_Delete` | function | high | F-490-5 | #498 |
| `0x802F3F10` | `0xD0` | `fn_802F3F10` | `ISFS_DeleteAsync` | function | high | F-490-5 | #498 |
| `0x802F3FE0` | `0x104` | `fn_802F3FE0` | `ISFS_Rename` | function | high | F-490-5 | #498 |
| `0x802F40F0` | `0x104` | `fn_802F40F0` | `ISFS_RenameAsync` | function | high | F-490-5 | #498 |
| `0x802F4340` | `0x124` | `fn_802F4340` | `ISFS_GetUsageAsync` | function | medium | F-490-5 | #498 |
| `0x802F4470` | `0xF4` | `fn_802F4470` | `ISFS_CreateFile` | function | high | F-490-5 | #498 |
| `0x802F4570` | `0xF4` | `fn_802F4570` | `ISFS_CreateFileAsync` | function | high | F-490-5 | #498 |
| `0x802F4800` | `0xD0` | `fn_802F4800` | `ISFS_GetFileStats` | function | high | F-490-5 | #498 |
| `0x802F48D0` | `0x4` | `fn_802F48D0` | `ISFS_Seek` | function | high | F-490-5 | #498 |
| `0x802F48E0` | `0x8C` | `fn_802F48E0` | `ISFS_SeekAsync` | function | high | F-490-5 | #498 |
| `0x802F4970` | `0x20` | `fn_802F4970` | `ISFS_Read` | function | high | F-490-5 | #498 |
| `0x802F4990` | `0xA4` | `fn_802F4990` | `ISFS_ReadAsync` | function | high | F-490-5 | #498 |
| `0x802F4A40` | `0x20` | `fn_802F4A40` | `ISFS_Write` | function | high | F-490-5 | #498 |
| `0x802F4A60` | `0xA4` | `fn_802F4A60` | `ISFS_WriteAsync` | function | high | F-490-5 | #498 |
| `0x80345D64` | `0x2C` | `fn_80345D64` | `strcat` | function | high | F-490-5 | #538 |

## ENC utility dependencies (#493)

[F-493-1](findings/493-enc-utility-names.md) establishes `ENCiCheckParameters` (`0x802EA030`, `0xF8` bytes), `ENCiCheckBreakType` (`0x802EA130`, `0x34` bytes), and `ENCiWriteBreakType` (`0x802EA170`, `0x100` bytes), with call signatures, source provenance, confidence and DOL offset mapping. This names-only change enables source linking for the #494 Latin unit; utility functions remain original fallback.

## Issue 477: GX framebuffer and lighting

[F-477-1](findings/477-gx-framebuf-light.md) records all 30 function names, code ranges, DOL offset mapping, light-object layout and external dependencies. GXFrameBuf owns 0x802CD440-0x802CDEC0; GXLight owns 0x802CDEC0-0x802CE630. Names are high confidence. Both units are now Matching and linked from source after [F-477-2](findings/477-gx-framebuf-light.md#f-477-2-complete-source-link). Their referenced external names are __GetImageTileCount at 0x802CE760, __cvt_fp2unsigned at 0x8034033C, cos at 0x8034CEE4 and sqrt at 0x8034D798. GXLight constants end at 0x804BB344; 0x804BB344-0x804BB348 is padding, not part of the last float.

## Issue 524: BTE HID device and host

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x80329258` | `0x68` | `fn_80329258` | `HID_DevInit` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_api.c` |
| `0x803292C0` | `0xC0` | `fn_803292C0` | `hidd_conn_initiate` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_conn.c` |
| `0x80329380` | `0xC8` | `fn_80329380` | `hidd_proc_repage_timeout` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_mgmt.c` |
| `0x80329448` | `0x1D8` | `fn_80329448` | `hidd_pm_set_now` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_pm.c` |
| `0x80329620` | `0x158` | `fn_80329620` | `hidd_pm_proc_mode_change` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_pm.c` |
| `0x80329778` | `0x4C` | `fn_80329778` | `hidd_pm_inact_timeout` | function | high | F-524-1 | `src/RVL_SDK/bte/hidd_pm.c` |
| `0x803297C4` | `0xC8` | `fn_803297C4` | `HID_HostGetSDPRecord` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032988C` | `0x450` | `fn_8032988C` | `hidh_search_callback` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x80329CDC` | `0x44` | `fn_80329CDC` | `HID_HostInit` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x80329D20` | `0x7C` | `fn_80329D20` | `HID_HostRegister` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x80329D9C` | `0xE4` | `fn_80329D9C` | `HID_HostDeregister` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x80329E80` | `0x190` | `fn_80329E80` | `HID_HostAddDev` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032A010` | `0xC8` | `fn_8032A010` | `HID_HostRemoveDev` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032A0D8` | `0x64` | `fn_8032A0D8` | `HID_HostOpenDev` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032A13C` | `0x140` | `fn_8032A13C` | `HID_HostWriteDev` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032A27C` | `0xA0` | `fn_8032A27C` | `HID_HostCloseDev` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8032A31C` | `0x228` | `fn_8032A31C` | `HID_HostSetSecurityLevel` | function | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x804AD8F0` | `0x144` | `lbl_804AD8F0` | `hd_cb` (`tHID_DEV_CTB`) | object | high name, medium file | F-524-1 | `src/RVL_SDK/bte/hidd_mgmt.c` |
| `0x804ADA38` | `0x404` | `lbl_804ADA38` | `hh_cb` (`tHID_HOST_CTB`) | object | high | F-524-1 | `src/RVL_SDK/bte/hidh_api.c` |
| `0x8031FBFC` | `0x1D0` | `fn_8031FBFC` | `BTM_SetPowerMode` | function, outside block | high name | F-524-4 | owning block issue |
| `0x8032A544` | `0x158` | `fn_8032A544` | `hidh_conn_reg` | function, outside block | high name | F-524-4 | owning block issue |
| `0x8032A69C` | `0xA4` | `fn_8032A69C` | `hidh_conn_disconnect` | function, outside block | high name | F-524-4 | owning block issue |
| `0x8032C1C0` | `0x29C` | `fn_8032C1C0` | `hidh_conn_snd_data` | function, outside block | high name | F-524-4 | owning block issue |
| `0x8032C45C` | `0xFC` | `fn_8032C45C` | `hidh_conn_initiate` | function, outside block | high name | F-524-4 | owning block issue |
| `0x8032C558` | `0x2C` | `fn_8032C558` | `hidh_conn_dereg` | function, outside block | high name | F-524-4 | owning block issue |
| `0x80338B3C` | `0x378` | `fn_80338B3C` | `SDP_InitDiscoveryDb` | function, outside block | high name | F-524-4 | owning block issue |
| `0x80338EB4` | `0x5C` | `fn_80338EB4` | `SDP_ServiceSearchRequest` | function, outside block | high name | F-524-4 | owning block issue |
| `0x80338F74` | `0x28` | `fn_80338F74` | `SDP_FindAttributeInRec` | function, outside block | high name | F-524-4 | owning block issue |
| `0x80339068` | `0x108` | `fn_80339068` | `SDP_FindServiceUUIDInDb` | function, outside block | high name | F-524-4 | owning block issue |

Layouts follow Petari and are confirmed by target accesses: `hd_cb` uses `conn` at 0x10 (timer parameter 0x2C), `idle_tle` 0x34, `pm_params` 0x4C, `curr_pm` 0x6A, `pm_ctrl_busy` 0x6E, `conn_substate` 0x6F, `final_pm` 0x70, `callback` 0xC4, L2CAP configs 0xC8 and 0x104 (0x3C each), `reg_flag` 0x140, `trace_level` 0x141. `hh_cb` has 16 0x34-byte devices, `sdp_busy` 0x380, `sdp_cback` 0x384, `p_sdp_db` 0x388, `sdp_rec` 0x38C (0x74), `reg_flag` 0x400, `trace_level` 0x401.

## AX command-list output mode (#486)

`__AXClMode` is a global unsigned 32-bit output mode at `.sbss:0x804B98B0`, extent four bytes, formerly `lbl_804B98B0`. It has no file-backed DOL offset. Original spelling and type have high confidence from the AXCL reference, target initialization, setter, command dispatch, and AXAux accesses; see [F-486-1](findings/486-axcl.md). No source implementation is accepted by this symbol-only change.

## Issue 467: OSFont, OSInterrupt, OSMessage, OSMemory

Evidence and confidence are in [F-467-2](findings/467-os-font-interrupt-memory.md#f-467-2-names). Every symbol below was renamed in `config/wpse-v1/symbols.txt`. The `OSMemory.c` names now link from `Matching` source ([F-467-6](findings/467-os-font-interrupt-memory.md#f-467-6-osmemoryc-links-from-source)); no new names were needed.

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

Outside symbols named so the OS units link. The owning issues keep their splits, sizes, and types. Evidence is in [F-467-5](findings/467-os-font-interrupt-memory.md#f-467-5-match-and-link-status).

| Address | Size | dtk label | Chosen name | Owner | Confidence |
| --- | --- | --- | --- | --- | --- |
| `0x802B40A0` | `0x30` | `fn_802B40A0` | `DCStoreRange` | #465 | high |
| `0x802BB460` | `0x124` | `fn_802BB460` | `__OSReadROM` | #469 | high |
| `0x802BCF60` | `0xF8` | `fn_802BCF60` | `OSWakeupThread` | #469 | high |
| `0x802BD730` | `0x114` | `fn_802BD730` | `OSUTF8to32` | #470 | high |
| `0x802BD850` | `0x70` | `fn_802BD850` | `OSUTF16to32` | #470 | high |
| `0x802BD8C0` | `0x7C` | `fn_802BD8C0` | `OSUTF32toANSI` | #470 | high |
| `0x802BD940` | `0x40` | `fn_802BD940` | `OSUTF32toSJIS` | #470 | high |

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

## RVL_SDK WPAD (#500)

For `wpse-v1` / `main.dol`, [F-500-1](findings/500-wpad.md) names the 23 functions from `0x802F6BC0` to `0x802F8BFC` in `config/wpse-v1/symbols.txt`. The public `WPADIsEnabled*` getters and the weak `WBC*` defaults are high confidence from strings, call sites, and SDK convention. Static names (`__wpadNoAlloc`, `OnShutdown`, `__wpadSendDataSub`, `__wpadIsControllerDataChanged`, `__wpadManageHandler`, `__wpadClearControlBlock`, `__wpadInitSub` and the rest) are medium confidence, from the 2009 SDK reconstruction cited in F-500-2. The same holds for the WPAD data names: `__WPADVersion` (`.sdata` `0x804B8DC8`), `_wpadOnReconnect`, both noise thresholds, the `.sbss` globals `0x804B9A50`–`0x804B9AA8`, `_wpadManageAlarm` (`.bss` `0x804774E0`), `_wpdcb` (`0x80477510`, the pointer table declared in the shared Petari `wpad.h`), `__wpadManageHandlerStack` (`0x80477520`, 0x1000 bytes), `_wpadHandle2PortTable` (`0x80478520`, 16 bytes), `_wpd` (`0x80478540`, 4 x 0x9C0, Petari's name), and `ShutdownFunctionInfo` (`.data` `0x803DCC18`). The control block layout is in `include/revolution/wpad.h`. Offsets and confidence are in F-500-2. The format constant `WPAD_FMT_VSM_EXTENDED` (14) and the VSM status fields after `err` are descriptive placeholders with low confidence.
## RVL_SDK WPAD second block (#501)

For `wpse-v1` / `main.dol`, [F-501-1](findings/501-wpad.md#f-501-1-function-identification-and-names) names the remaining 57 WPAD.c functions from `0x802F8C00` to `0x802FE648` in `config/wpse-v1/symbols.txt`, with the address, name and evidence of each. Public `WPAD*` and `WPADi*` names are high confidence. The ten static callbacks and helpers (`__wpadSetupConnectionCallback`, `__wpadAbortConnectionCallback`, `__wpadInitConnectionCallback`, `__wpadRetrieveChannel`, `__wpadConnectionCallback`, `__wpadReceiveCallback`, `__wpadDisconnectCallback`, `__wpadInfoCallback`, `__wpadIsBusyStream`, `__wpadDpdCallback`) are `scope:local` and medium confidence, from the reconstruction cited in F-500-2. [F-501-2](findings/501-wpad.md#f-501-2-outside-symbols-named-for-the-link) names 31 outside labels the unit links against: `WPADiHIDParser`, `WPADiClearMemBlock`, `WPADWriteExtReg`, `DEBUGPrint`, 22 WUD functions, `GKI_getbuf`, three `BTA_Hh*` functions and `btm_remove_acl`. `WUDIsLinkedWBC`, `WUDIsLatestDevice` and the five-argument `WPADWriteExtReg` are medium confidence. Descriptive, low-confidence header names from [F-501-4](findings/501-wpad.md#f-501-4-header-and-split-changes): `WPAD_SPEAKER_REINIT` (5), `WPAD_BLC_UPDATE` (`0x55`) and `WPADVSStatus.auxData` (offsets `0x47`–`0x49`, structure size 0x4A). The function-static DPD tables are `cfg1` (`.rodata` `0x80397DD8`, 5 x 9 bytes) and `cfg2` (`0x80397E08`, 5 x 2 bytes).

## Issue 512: BTA device-manager actions

[F-512-1](findings/512-bta-dm-act.md) records the names, ranges and evidence. Renamed in `config/wpse-v1/symbols.txt`, all high confidence:

| Address | Size | Name | Unit |
| --- | --- | --- | --- |
| `0x8031486C` | `0xB4` | `ptim_timer_update` | `ptim.c` (split owned by #511) |
| `0x80314920` | `0xA0` | `ptim_start_timer` | `ptim.c` |
| `0x803149C0` | `0x40` | `ptim_stop_timer` | `ptim.c` |
| `0x80314A00` | `0x44` | `utl_freebuf` | `utl.c` |
| `0x80314A44` | `0x110` | `bta_dm_enable` | `bta_dm_act.c` |
| `0x80314B54` | `0xB0` | `bta_dm_disable` | `bta_dm_act.c` |
| `0x80314C04` | `0xBC` | `bta_dm_disable_timer_cback` (local) | `bta_dm_act.c` |
| `0x80314CC0` | `0x8` | `bta_dm_set_dev_name` | `bta_dm_act.c` |
| `0x80314CC8` | `0x48` | `bta_dm_set_visibility` | `bta_dm_act.c` |
| `0x80314D10` | `0x70` | `bta_dm_bond` | `bta_dm_act.c` |
| `0x80314D80` | `0x9C` | `bta_dm_pin_reply` | `bta_dm_act.c` |
| `0x80314E1C` | `0xD4` | `bta_dm_auth_reply` | `bta_dm_act.c` |
| `0x80314EF0` | `0x60` | `bta_dm_search_start` | `bta_dm_act.c` |
| `0x80314F50` | `0x84` | `bta_dm_search_cancel` | `bta_dm_act.c` |
| `0x80314FD4` | `0x114` | `bta_dm_discover` | `bta_dm_act.c` |
| `0x803150E8` | `0x224` | `bta_dm_inq_cmpl` | `bta_dm_act.c` |
| `0x8031530C` | `0x16C` | `bta_dm_rmt_name` | `bta_dm_act.c` |
| `0x80315478` | `0x64` | `bta_dm_disc_rmt_name` | `bta_dm_act.c` |
| `0x803154DC` | `0x268` | `bta_dm_sdp_result` | `bta_dm_act.c` |
| `0x80315744` | `0x18` | `bta_dm_search_cmpl` | `bta_dm_act.c` |
| `0x8031575C` | `0x50` | `bta_dm_disc_result` | `bta_dm_act.c` |
| `0x803157AC` | `0x88` | `bta_dm_search_result` | `bta_dm_act.c` |
| `0x80315834` | `0x4C` | `bta_dm_search_timer_cback` (local) | `bta_dm_act.c` |
| `0x80315880` | `0x44` | `bta_dm_free_sdp_db` | `bta_dm_act.c` |
| `0x803158C4` | `0x48` | `bta_dm_queue_search` | `bta_dm_act.c` |
| `0x80397EE8` | `0x2E` | `bta_service_id_to_uuid_lkup_tbl` | `bta_dm_act.c` `.rodata` |
| `0x80397F18` | `0x5C` | `bta_service_id_to_btm_srv_id_lkup_tbl` | `bta_dm_act.c` `.rodata` |
| `0x80397F78` | `0x18` | `bta_security` | `bta_dm_act.c` `.rodata` |
| `0x804AA968` | `0x2D` | `bta_dm_compress_srvcs` (local) | `bta_dm_act.c` `.bss` |

The control-block and message offsets that this code checks against the target are listed under the finding's headers section.
## RVL_SDK GXMisc and GXGeometry

Names, ranges and evidence for 25 renamed functions and four statics are in [F-476-1](findings/476-gxmisc-gxgeometry.md). Statics, all local to `GXMisc.c`: `FinishQueue` `.sbss` `0x804B96E0` (OSThreadQueue, 8 bytes), `DrawDone` `0x804B96E8` (volatile GXBool), `DrawDoneCB` `0x804B96EC`, `TokenCB` `0x804B96F0`.

External GX names given for the source link, evidence in [F-476-4](findings/476-gxmisc-gxgeometry.md): `__GXSetVCD` `0x802CB930` (0xB0), `__GXCalculateVLim` `0x802CB9E0` (0x12C), `__GXSetVAT` `0x802CC080` (0x80), `__GXSetSUTexRegs` `0x802CF270` (0x164), `__GXUpdateBPMask` `0x802CFB20` (0x4), `__GXSetProjection` `0x802D0A60` (0x44), `__GXSetViewport` `0x802D0DB0` (0x90), `__GXSetMatrixIndex` `0x802D0F90` (0x88). Confidence high; sizes are dtk's and remain with the owning units.

`GXData` (`include/revolution/gx/GXTypes.h`, reached through the `.sdata2` pointer `__GXData`) is 0x600 bytes, 4-byte aligned. Offsets confirmed by target accesses: `vNumNot` 0x0, `bpSentNot` 0x2, `vNum` 0x4, `vLim` 0x6, `lpSize` 0x7C, `ambColor` 0xA8, `matColor` 0xB0, `chanCtrl` 0xB8, `texGenCtrl` 0xC8, `dualTexGenCtrl` 0xE8, `suTs0` 0x108, `peCtrl` 0x22C, `genMode` 0x254, `dlSaveContext` 0x5F9, `abtWaitPECopy` 0x5FA, `dirtyState` 0x5FC. Other fields follow Petari's layout; confidence medium until their units are matched. `GX_DIRTY_*`, `XF_REG_*`, and `PE_REG_*` names in `include/revolution/gx/GXState.h` are descriptive choices, not recovered names.
## Issue #497: IPC allocator and profiling

[F-497-1](findings/497-ipc-memory-profile.md) records target-disassembly and reference-source evidence. High-confidence recovered names: `IOS_Close` at `0x802F1DF0`, `IOS_IoctlvReboot` at `0x802F2A10`, `__iosAlloc` at `0x802F2E40`, `iosAlloc` at `0x802F3040`, `IPCiProfAck` at `0x802F33A0`, and `IPCiProfReply` at `0x802F33B0`. The eight-entry heap table at `0x804772A0` is `__heaps`; each 16-byte record has base pointer at 0, owner at 4, size at 8, and free-list pointer at 12. Allocator and profiling function names and layouts are corroborated by 100% object code/data comparisons. [F-497-2](findings/497-ipc-memory-profile.md#f-497-2-ipc-client) adds the reconstructed IPC client: local `IpcReplyHandler` at `0x802F1350` and `__ios_Ioctlv` at `0x802F2700`, BSS `__timeout_alarm` at `0x80477230` (0x30-byte `OSAlarm`) and 32-byte-aligned `__rpcBuf` at `0x80477260`, and `.sbss` `__relnchFl`, `__relnchRpc`, `__relnchRpcSave` at `0x804B99F0`–`0x804B99F8`. All client names are corroborated by 22/22 functions at 100% objdiff and full DOL verification.
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
## RVL_SDK OS arena, audio, and cache

All entries target `wpse-v1` `main.dol`, `.text`. File offsets equal runtime address minus `0x80052500` plus `0x27C0`. Names have high confidence from matching SDK routines and observed behavior; see [F-465-1](findings/465-os-arena-audio-cache.md#f-465-1-unit-boundaries-and-recovered-names).

| Address | Size | Previous label | Recovered name | Source |
| --- | --- | --- | --- | --- |
| `0x802B3B40` | `0x20` | `fn_802B3B40` | `OSAllocFromMEM1ArenaHi` | `src/RVL_SDK/os/OSArena.c` |
| `0x802B40A0` | `0x30` | `fn_802B40A0` | `DCStoreRange` | `src/RVL_SDK/os/OSCache.c` |
| `0x802B4100` | `0x2C` | `fn_802B4100` | `DCStoreRangeNoSync` | `src/RVL_SDK/os/OSCache.c` |
| `0x802B41D0` | `0xCC` | `fn_802B41D0` | `__LCEnable` | `src/RVL_SDK/os/OSCache.c` |
| `0x802B42A0` | `0x38` | `fn_802B42A0` | `LCEnable` | `src/RVL_SDK/os/OSCache.c` |
| `0x802B4310` | `0x24` | `fn_802B4310` | `LCLoadBlocks` | `src/RVL_SDK/os/OSCache.c` |
| `0x802B4340` | `0x24` | `fn_802B4340` | `LCStoreBlocks` | `src/RVL_SDK/os/OSCache.c` |
| `0x802B4370` | `0xA0` | `fn_802B4370` | `LCStoreData` | `src/RVL_SDK/os/OSCache.c` |
| `0x802B4410` | `0xC` | `fn_802B4410` | `LCQueueLength` | `src/RVL_SDK/os/OSCache.c` |
| `0x802B4420` | `0x14` | `fn_802B4420` | `LCQueueWait` | `src/RVL_SDK/os/OSCache.c` |

`OSContext` follows the reference SDK layout with 8-byte alignment and size `0x2C8`: `gpr[32]` at `0x0`, `cr` `0x80`, `lr` `0x84`, `ctr` `0x88`, `xer` `0x8C`, `fpr[32]` `0x90`, `fpscr_pad` `0x190`, `fpscr` `0x194`, `srr0` `0x198`, `srr1` `0x19C`, `mode` `0x1A0`, `state` `0x1A2`, `gqr[8]` `0x1A4`, `psf_pad` `0x1C4`, `psf[32]` `0x1C8`. This issue directly validates the `srr1` access at `0x19C`; the complete layout is now also established by the neighboring OSContext unit in #466. `OSError` is `u16`; `OSErrorHandler` receives error, context pointer, and variadic arguments. The cache handler's variadic register-save prologue matches exactly. Pointer and `u32` arena parameters use the standard PowerPC integer registers and pointer return in r3.
## RVL_SDK AXAux

Target `wpse-v1`, `main.dol`; evidence [F-485-1](findings/485-axaux.md#f-485-1-axaux-unit-and-reconstruction), confidence high. Functions are original reference names; source `src/RVL_SDK/ax/AXAux.c`, callback interface `include/revolution/ax.h`. Function file offsets equal runtime address minus `0x8004FD40`; BSS has no file payload. Callback pointers and context pointers occupy four bytes. No structures are introduced.

| Runtime address | Extent | Section | Previous label | Recovered name |
| --- | --- | --- | --- | --- |
| `0x802DA9F0` | `0x114` | `.text` | `fn_802DA9F0` | `__AXAuxInit` |
| `0x802DAB10` | `0x34` | `.text` | `fn_802DAB10` | `__AXGetAuxAInput` |
| `0x802DAB50` | `0x1C` | `.text` | `fn_802DAB50` | `__AXGetAuxAOutput` |
| `0x802DAB70` | `0x20` | `.text` | `fn_802DAB70` | `__AXGetAuxAInputDpl2` |
| `0x802DAB90` | `0x20` | `.text` | `fn_802DAB90` | `__AXGetAuxAOutputDpl2R` |
| `0x802DABB0` | `0x20` | `.text` | `fn_802DABB0` | `__AXGetAuxAOutputDpl2Ls` |
| `0x802DABD0` | `0x20` | `.text` | `fn_802DABD0` | `__AXGetAuxAOutputDpl2Rs` |
| `0x802DABF0` | `0x34` | `.text` | `fn_802DABF0` | `__AXGetAuxBInput` |
| `0x802DAC30` | `0x1C` | `.text` | `fn_802DAC30` | `__AXGetAuxBOutput` |
| `0x802DAC50` | `0x20` | `.text` | `fn_802DAC50` | `__AXGetAuxBInputDpl2` |
| `0x802DAC70` | `0x20` | `.text` | `fn_802DAC70` | `__AXGetAuxBOutputDpl2R` |
| `0x802DAC90` | `0x20` | `.text` | `fn_802DAC90` | `__AXGetAuxBOutputDpl2Ls` |
| `0x802DACB0` | `0x20` | `.text` | `fn_802DACB0` | `__AXGetAuxBOutputDpl2Rs` |
| `0x802DACD0` | `0x34` | `.text` | `fn_802DACD0` | `__AXGetAuxCInput` |
| `0x802DAD10` | `0x1C` | `.text` | `fn_802DAD10` | `__AXGetAuxCOutput` |
| `0x802DAD30` | `0x3AC` | `.text` | `fn_802DAD30` | `__AXProcessAux` |
| `0x802DB0E0` | `0x64` | `.text` | `fn_802DB0E0` | `AXRegisterAuxACallback` |
| `0x802DB150` | `0x64` | `.text` | `fn_802DB150` | `AXRegisterAuxBCallback` |
| `0x802DB1C0` | `0x64` | `.text` | `fn_802DB1C0` | `AXRegisterAuxCCallback` |
| `0x802DB230` | `0x14` | `.text` | `fn_802DB230` | `AXGetAuxACallback` |
| `0x802DB250` | `0x14` | `.text` | `fn_802DB250` | `AXGetAuxBCallback` |
| `0x802DB270` | `0x14` | `.text` | `fn_802DB270` | `AXGetAuxCCallback` |
| `0x804594A0` | `0x1200` | `.bss` | `lbl_804594A0` | `__AXBufferAuxA` |
| `0x8045A6A0` | `0x1200` | `.bss` | `lbl_8045A6A0` | `__AXBufferAuxB` |
| `0x8045B8A0` | `0xD80` | `.bss` | `lbl_8045B8A0` | `__AXBufferAuxC` |
| `0x804B9850` | `0x4` | `.sbss` | `lbl_804B9850` | `__AXAuxCpuReadWritePosition` |
| `0x804B9854` | `0x4` | `.sbss` | `lbl_804B9854` | `__AXAuxDspReadPosition` |
| `0x804B9858` | `0x4` | `.sbss` | `lbl_804B9858` | `__AXAuxDspWritePosition` |
| `0x804B985C` | `0x4` | `.sbss` | `lbl_804B985C` | `__AXAuxCDspRead` |
| `0x804B9860` | `0x4` | `.sbss` | `lbl_804B9860` | `__AXAuxCDspWrite` |
| `0x804B9864` | `0x4` | `.sbss` | `lbl_804B9864` | `__AXAuxBDspRead` |
| `0x804B9868` | `0x4` | `.sbss` | `lbl_804B9868` | `__AXAuxBDspWrite` |
| `0x804B986C` | `0x4` | `.sbss` | `lbl_804B986C` | `__AXAuxADspRead` |
| `0x804B9870` | `0x4` | `.sbss` | `lbl_804B9870` | `__AXAuxADspWrite` |
| `0x804B9874` | `0x4` | `.sbss` | `lbl_804B9874` | `__AXContextAuxC` |
| `0x804B9878` | `0x4` | `.sbss` | `lbl_804B9878` | `__AXContextAuxB` |
| `0x804B987C` | `0x4` | `.sbss` | `lbl_804B987C` | `__AXContextAuxA` |
| `0x804B9880` | `0x4` | `.sbss` | `lbl_804B9880` | `__AXCallbackAuxC` |
| `0x804B9884` | `0x4` | `.sbss` | `lbl_804B9884` | `__AXCallbackAuxB` |
| `0x804B9888` | `0x4` | `.sbss` | `lbl_804B9888` | `__AXCallbackAuxA` |
| `0x804B988C` | `0x3` | `.sbss` | `lbl_804B988C` | `__clearAuxC` |
| `0x804B9890` | `0x3` | `.sbss` | `lbl_804B9890` | `__clearAuxB` |
| `0x804B9894` | `0x3` | `.sbss` | `lbl_804B9894` | `__clearAuxA` |

## Issue 494: Japanese conversion and encoding contexts

[F-494-3](findings/494-enc-latin.md#f-494-3-japanese-and-context-conversion-units) establishes these names with high confidence from pinned reference source, target relocations, 100% object comparisons and full executable verification. Addresses and extents are for target `wpse-v1`.

| Address | Recovered name |
| --- | --- |
| `0x802EBAD0` | `ENCConvertStringSjisToUnicode` |
| `0x802EBAE0` | `ENCConvertStringUnicodeToSjis` |
| `0x802EBAF0` | `ENCiConvertStringSjisToUnicode` |
| `0x802EBE60` | `ENCiConvertStringUnicodeToSjis` |
| `0x802EC110` | `ENCiConvertStringJisToUnicode` |
| `0x802EC580` | `ENCiConvertStringUnicodeToJis` |
| `0x802ECA40` | `ENCiConvertUnicodeToSjis` |
| `0x802ECB40` | `ENCiFindSjisFromUnicode` |
| `0x802ECC60` | `ENCiGetEscapeSequence` |
| `0x802ECDD0` | `ENCiSetEscapeSequence` |
| `0x802ECED0` | `ENCiGetJisStateFromUnicode` |
| `0x802ECFB0` | `ENCiConvertSjisKanji3` |
| `0x802ED700` | `ENCInitContext` |
| `0x802ED760` | `ENCSetExternalEncoding` |
| `0x802ED880` | `ENCSetAlternativeCharacter` |
| `0x802ED8A0` | `ENCConvertToInternalEncoding` |
| `0x802EDB30` | `ENCConvertFromInternalEncoding` |
| `0x802EDDD0` | `ENCiGetEncodingTypeFromHashTable` |
| `0x802EE010` | `ENCGetNextCharacterWidth` |
| `0x802EE1F0` | `ENCiConvertToInternalEncoding` |
| `0x802EE4A0` | `ENCiConvertFromInternalEncoding` |

`ENCContext` has size 16, alignment 4: encoding enum at 0, break enum at 4, multibyte state (`s32`) at 8, unmappable alternative (`u16`) at 12 and invalid alternative (`u16`) at 14. `struct encoding` is 24 bytes: width (`u32`) at 0, seven-bit flag (`BOOL`) at 4 and 16-byte name at 8. `encoding_hash_table_entry` is 32 bytes: 24-byte normalized name at 0, encoding enum at 24 and collision index (`int`) at 28. Encoding values run 0–25, while several original validity checks reject values at or above 24; that observed behavior is preserved.

Japanese data: `enc_tbl_jp_mbtowc` at `0x803718A8` (18,800 bytes), `enc_tbl_jp_wctomb` at `0x80376218` (22,257 bytes), `enc_offset_jp` at `0x8037B910` (512 bytes), and `kana_array` at `0x8037BB10` (126 bytes). Context data: `encoding_array` at `0x8037D690` (624 bytes), `encoding_table` at `0x8037D900` (4,128 bytes). Loaded BOOLs: Japanese `0x804B8D60`, Korean `0x804B8D68`, UHC `0x804B8D6C`, Chinese `0x804B8D70`, each four bytes. These names are taken from the reference source and confirmed by target accesses.

## Issue 495: Chinese and Korean ENC conversions

[F-495-3](findings/495-arc-unit.md#f-495-3-chinese-and-korean-enc-units-complete-the-block) confirms these names from pinned source, exact code/data comparisons and the full source-linked executable. Confidence is high; no new shared structure layouts are introduced.

| Address | Function |
| --- | --- |
| `0x802EE650` | `ENCiConvertStringGb2312ToUnicode` |
| `0x802EE920` | `ENCiConvertStringUnicodeToGb2312` |
| `0x802EEC20` | `ENCiFindGb2312FromUnicode` |
| `0x802EED40` | `ENCiFindExKanjiUnicode` |
| `0x802EEE00` | `ENCiFindExKanjiGBK` |
| `0x802EEEC0` | `ENCiConvertStringUhcToUnicode` |
| `0x802EF2C0` | `ENCiConvertStringUnicodeToUhc` |
| `0x802EF580` | `ENCiConvertUnicodeToUhc` |
| `0x802EF690` | `ENCiFindEuckrFromUnicode` |
| `0x802EF7B0` | `ENCiFindKanjiFromUnicode` |
| `0x802EF8F0` | `ENCiFindUhcFromUnicode` |

| Address | Table | Byte size | Element type |
| --- | --- | --- | --- |
| `0x8037E920` | `enc_tbl_cn_mbtowc` | 17672 | `u16` |
| `0x80382E28` | `enc_tbl_cn_wctomb` | 22365 | `u8` |
| `0x80388588` | `enc_offset_cn` | 512 | `u16` |
| `0x80388788` | `enc_tbl_cnex_mbtowc` | 992 | `u16` |
| `0x80388B68` | `enc_tbl_kr_mbtowc1` | 7520 | `u16` |
| `0x8038A8C8` | `enc_tbl_kr_mbtowc2` | 9776 | `u16` |
| `0x8038CEF8` | `enc_tbl_kr_wctomb1` | 10968 | `u8` |
| `0x8038F9D0` | `enc_tbl_kr_wctomb2` | 15015 | `u8` |
| `0x80393478` | `enc_offset_kr1` | 512 | `u16` |
| `0x80393678` | `enc_offset_kr2` | 512 | `u16` |
| `0x80393878` | `enc_tbl_uhc_mbtowc` | 17644 | `u16` |
| `0x80397D68` | `enc_offset_uhc` | 88 | `u16` |
## RVL_SDK TPL and GDBase

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x802CE820` | `0x20C` | `fn_802CE820` | `GXInitTexObj` | function | medium-high | F-534-1 | callee of `TPLGetGXTexObjFromPalette` |
| `0x802CEA80` | `0x104` | `fn_802CEA80` | `GXInitTexObjLOD` | function | medium-high | F-534-1 | callee of `TPLGetGXTexObjFromPalette` |
| `0x8033E1B0` | `0x118` | `fn_8033E1B0` | `TPLBind` | function | high | F-534-1, `TPL.c` panic string | `src/RVL_SDK/tpl/TPL.c` |
| `0x8033E2D0` | `0x20` | `fn_8033E2D0` | `TPLGet` | function | high | F-534-1 | `src/RVL_SDK/tpl/TPL.c` |
| `0x8033E2F0` | `0xE0` | `fn_8033E2F0` | `TPLGetGXTexObjFromPalette` | function | high | F-534-1 | `src/RVL_SDK/tpl/TPL.c` |
| `0x8033E3D0` | `0x18` | `fn_8033E3D0` | `GDInitGDLObj` | function | high | F-534-1 | `src/RVL_SDK/gd/GDBase.c` |
| `0x8033E3F0` | `0x10` | `fn_8033E3F0` | `GDFlushCurrToMem` | function | high | F-534-1 | `src/RVL_SDK/gd/GDBase.c` |
| `0x8033E400` | `0x118` | `fn_8033E400` | `GDPadCurr32` | function | high | F-534-1 | `src/RVL_SDK/gd/GDBase.c` |
| `0x804B9B9C` | `0x4` | `lbl_804B9B9C` | `overflowcb` | `.sbss` static | high | F-534-1 | `src/RVL_SDK/gd/GDBase.c` |
## BTE close callback, issue 511

| Address | Name | Evidence |
| --- | --- | --- |
| `0x80313E40` | `bta_usb_close_evt` | High confidence: pinned Petari correspondence and all twelve target instructions; [F-511-2](findings/511-bte-close-callback.md) |

Signature: `void bta_usb_close_evt(signed char status)`. The existing `_bte_app_info` callback receives unsigned-byte status zero for a nonnegative input and one otherwise. A null callback returns immediately.
## DVD queue and disc ID (#482)

[F-482-1](findings/482-dvd-queue-id.md) records function addresses, extents, file-offset mapping and reference evidence for the six waiting-queue operations at 0x802D66A0-0x802D6920 and DVDCompareDiskID at 0x802D7340. The queue's existing local WaitingQueue occupies 0x20 BSS bytes at 0x80458F30. Original names are high confidence; all seven functions compile at 100% objdiff. The finding distinguishes target-observed DVDCommandBlock and DVDDiskID members from the remainder of their reference-derived layouts. The external strncmp at 0x80345EF8 (0x40 bytes) is named from bounded unsigned-byte comparison, NUL stopping and reference call-site evidence; its extent, type and split are unchanged.

The dvd.c tail names __DVDTestAlarm (0x802D6660, 0x20 bytes), __DVDStopMotorAsync (0x802D6680, 8 bytes), and __DVDRestartMotor (0x802D6690, 4 bytes) are also restored within #482. Reference order, alarm-pointer comparison followed by the low-level alarm test, constant-success stub and empty restart body support these original names. No dvd.c split or source coverage is claimed.


## BTE source storage acceptance (#510)

[F-510-3](findings/510-bte-transport-logging.md#f-510-3-complete-source-linking-and-exact-storage-boundaries) establishes `bte_log_buffer` as a 2000-byte allocation at `0x804A9040`, followed by sixteen bytes of alignment padding. `__BTUInterruptHandlerStack` is 4096 bytes aligned to 32 at `0x804A9820`; `_bte_alarm` follows at `0x804AA820`, size 48, using the shared SDK OSAlarm declaration. `bte_hcisu_h2_cfg` contains two unsigned 16-bit values `0x0A5C`, `0x2101` at `0x804B8EE8`, size four; the following four bytes are padding. The byte `bte_target_mode` and callback `_bte_app_info` occupy `0x804B9B80` and `0x804B9B84`; transport pointers `p_hcisu_if`, `p_hcisu_cfg` occupy `0x804B9B78`, `0x804B9B7C`. These names follow the pinned Petari reference and their storage addresses are confirmed by complete source-linked DOL verification.
## Issue 540: MSL math and case-insensitive comparison

[F-540-1](findings/540-msl-math.md) records the compiler, source provenance, exact section boundaries and full verification. Names are supported by reference source, instructions, relocations and link order. `w_log.c` is an inferred filename. The `nan` identification additionally follows the reference body and parser call sites in F-540-2. External callees retain their owning issues and original extents.

| Address | Name |
| --- | --- |
| `0x80345A7C` | `__fpclassifyd` |
| `0x80349664` | `__ieee754_acos` |
| `0x803498FC` | `__ieee754_asin` |
| `0x80349B54` | `__ieee754_atan2` |
| `0x80349D84` | `__ieee754_fmod` |
| `0x8034A104` | `__ieee754_log` |
| `0x8034A3B8` | `__ieee754_log10` |
| `0x8034A4CC` | `__ieee754_pow` |
| `0x8034ACE0` | `__ieee754_rem_pio2` |
| `0x8034B064` | `__kernel_cos` |
| `0x8034C82C` | `__kernel_sin` |
| `0x8034C8EC` | `__kernel_tan` |
| `0x8034CEB8` | `copysign` |
| `0x8034D184` | `ldexp` |
| `0x8034D2F0` | `modf` |
| `0x8034D3EC` | `sin` |
| `0x8034D4B8` | `tan` |
| `0x8034D530` | `acos` |
| `0x8034D534` | `asin` |
| `0x8034D538` | `atan2` |
| `0x8034D53C` | `fmod` |
| `0x8034D540` | `log` |
| `0x8034D544` | `log10` |
| `0x8034D548` | `pow` |
| `0x8034D54C` | `__ieee754_sqrt` |
| `0x8034D794` | `nan` |
| `0x8034D798` | `sqrt` |
| `0x8034D79C` | `stricmp` |
| `0x803E6228` | `_current_locale` |
| `0x804B8FB0` | `__float_nan` |
| `0x804B9BD0` | `errno` |

The source constants occupy `0x804BBBB8–0x804BBBF8` as five doubles for `ldexp` and one double each for `sin`, `tan`, and `__ieee754_sqrt`. `errno` at `0x804B9BD0` is accessed as a four-byte int; its outside symbol extent is unchanged. `__float_nan` at `0x804B8FB0` is accessed through the reference int-array view as a float; its outside size and type attributes remain unchanged. The locale layout used by `stricmp` is the existing imported MSL declaration. No new structure layouts are introduced.
## Issue #479 completion helper

`sqrt` at `0x8034D798` is the outside-block scalar square-root entry called by GXInitFogAdjTable and GXInitSpecularDirHA in the matching SDK references. The name resolves the source relocation; its existing size, type and split remain unchanged. [F-479-3](findings/479-gx.md#f-479-3-full-issue-completion-from-source) records the exact source completion evidence for all thirty owned functions.

## AXAlloc completion (#485)

[F-485-3](findings/485-axaux.md#f-485-3-axalloc-completes-issue-485) records the nine original function names and extents at 0x802DA500-0x802DA9EC, the two 32-entry voice-stack arrays at 0x804593A0 and 0x80459420, and callback-stack pointer at 0x804B9848. Names and existing AXVPB layout have high confidence from pinned ogws source, target accesses and exact source-linked verification. The external __AXSetPBDefault at 0x802DD260 retains its prior extent and split.
## Init section startup, issue 37

| Address | Name | Evidence |
| --- | --- | --- |
| `0x80004000`-`0x80004250` | `__check_pad3`, `__set_debug_bba`, `__get_debug_bba`, `__start`, `__init_registers`, `__init_data` | High confidence: existing names confirmed by matching `__start.c`; [F-37-1](findings/37-init-section.md) |
| `0x804B9538` | `Debug_BBA` | `u8` static, one byte, high confidence; [F-37-1](findings/37-init-section.md) |
| `0x800030E4` | `Pad3Button` | `u16` low-memory pad state, fixed-address declaration in `__start.c`; masked with `0xEEF` |
| `0x80004300`-`0x802C0A70` | `__init_hardware`, `__flush_cache`, `__init_user`, `__init_cpp`, `exit` | High confidence: matching `__ppc_eabi_init.c` |
| `0x80004364`-`0x8033F974` | `memcpy`, `__fill_mem`, `memset`, `strlen` | High confidence: matching `__mem.c` |
## HBM frame and group animation controllers (#287)

[F-287-1](findings/287-hbm-core.md#f-287-1-hbm-compiler-flags-and-first-unit-boundaries) names `homebutton::FrameController::init` (`0x80178530`), `initFrame` (`0x80178570`), `calc` (`0x80178590`), and `homebutton::GroupAnmController`'s constructor (`0x801786E0`), destructor (`0x80178700`) and `do_calc` (`0x80178740`), plus `__RTTI__Q210homebutton15FrameController` (`0x803BA010`), `__vt__Q210homebutton18GroupAnmController` (`0x803BA018`) and `__RTTI__Q210homebutton18GroupAnmController` (`0x803BA058`). Confidence is high: RTTI strings give the classes and every function matches. `FrameController` (0x20 bytes): vtable `0x0`, `mMaxFrame` `0x4`, `mMinFrame` `0x8`, `mCurFrame` `0xC`, `mFrameDelta` `0x10`, `mState` `0x14`, `mAnmType` `0x18`, `mAltFlag` `0x1C`. `GroupAnmController` (0x28 bytes) adds `mpGroup` `0x20` and `mpAnimGroup` `0x24`. The game's global `operator delete` at `0x801B1580` is renamed `__dl__FPv` from its use in every deleting destructor.
| `0x8032582C` | `0x2C` | `fn_8032582C` | `btu_init_core` | function | high | F-522-1 | `src/RVL_SDK/bte/btu_init.c` |
| `0x80325858` | `0x4C` | `fn_80325858` | `BTE_Init` | function | high | F-522-1 | `src/RVL_SDK/bte/btu_init.c` |
| `0x803258A4` | `0xE0` | `fn_803258A4` | `WBT_ExtCreateRecord` | function | high | F-522-1 | `src/RVL_SDK/bte/wbt_ext.c` |
| `0x80325984` | `0x60` | `fn_80325984` | `GAP_Init` | function | high | F-522-1 | `src/RVL_SDK/bte/gap_api.c` |
| `0x803259E4` | `0x70` | `fn_803259E4` | `gap_conn_init` | function | high | F-522-1 | `src/RVL_SDK/bte/gap_conn.c` |
| `0x80325A54` | `0x18C` | `fn_80325A54` | `gap_connect_ind` | static function | high | F-522-1 | `src/RVL_SDK/bte/gap_conn.c` |
| `0x80325BE0` | `0x1DC` | `fn_80325BE0` | `gap_connect_cfm` | static function | high | F-522-1 | `src/RVL_SDK/bte/gap_conn.c` |
| `0x80325DBC` | `0x13C` | `fn_80325DBC` | `gap_config_ind` | static function | high | F-522-1 | `src/RVL_SDK/bte/gap_conn.c` |
| `0x80325EF8` | `0x1EC` | `fn_80325EF8` | `gap_config_cfm` | static function | high | F-522-1 | `src/RVL_SDK/bte/gap_conn.c` |
| `0x803260E4` | `0x1F8` | `fn_803260E4` | `gap_disconnect_ind` | static function | high | F-522-1 | `src/RVL_SDK/bte/gap_conn.c` |
| `0x803262DC` | `0x110` | `fn_803262DC` | `gap_data_ind` | static function | high | F-522-1 | `src/RVL_SDK/bte/gap_conn.c` |
| `0x803263EC` | `0x134` | `fn_803263EC` | `gap_congestion_ind` | static function | high | F-522-1 | `src/RVL_SDK/bte/gap_conn.c` |
| `0x80326520` | `0x1D8` | `fn_80326520` | `btm_cback` | static function | high | F-522-1 | `src/RVL_SDK/bte/gap_utils.c` |
| `0x803266F8` | `0xC` | `fn_803266F8` | `gap_btm_cback0` | function | high | F-522-1 | `src/RVL_SDK/bte/gap_utils.c` |
| `0x80326704` | `0xC` | `fn_80326704` | `gap_btm_cback1` | function | high | F-522-1 | `src/RVL_SDK/bte/gap_utils.c` |
| `0x80326710` | `0x218` | `fn_80326710` | `gap_find_addr_name_cb` | function | high | F-522-1 | `src/RVL_SDK/bte/gap_utils.c` |
| `0x80326928` | `0x1B4` | `fn_80326928` | `gap_find_addr_inq_cb` | function | high | F-522-1 | `src/RVL_SDK/bte/gap_utils.c` |
| `0x80326ADC` | `0x60` | `fn_80326ADC` | `gap_convert_btm_status` | function | high | F-522-1 | `src/RVL_SDK/bte/gap_utils.c` |
| `0x80326B3C` | `0xA8` | `fn_80326B3C` | `btsnd_hcic_inquiry` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80326BE4` | `0x60` | `fn_80326BE4` | `btsnd_hcic_inq_cancel` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80326C44` | `0xC0` | `fn_80326C44` | `btsnd_hcic_per_inq_mode` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80326D04` | `0xE0` | `fn_80326D04` | `btsnd_hcic_create_conn` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80326DE4` | `0x88` | `fn_80326DE4` | `btsnd_hcic_disconnect` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80326E6C` | `0x8C` | `fn_80326E6C` | `btsnd_hcic_add_SCO_conn` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80326EF8` | `0x60` | `fn_80326EF8` | `btsnd_hcic_accept_conn` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80326F58` | `0x5C` | `fn_80326F58` | `btsnd_hcic_reject_conn` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80326FB4` | `0x1A0` | `fn_80326FB4` | `btsnd_hcic_link_key_req_reply` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327154` | `0xA0` | `fn_80327154` | `btsnd_hcic_link_key_neg_reply` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803271F4` | `0x1E4` | `fn_803271F4` | `btsnd_hcic_pin_code_req_reply` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803273D8` | `0xA0` | `fn_803273D8` | `btsnd_hcic_pin_code_neg_reply` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327478` | `0x90` | `fn_80327478` | `btsnd_hcic_change_conn_type` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327508` | `0x7C` | `fn_80327508` | `btsnd_hcic_auth_request` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327584` | `0x8C` | `fn_80327584` | `btsnd_hcic_set_conn_encrypt` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327610` | `0xD8` | `fn_80327610` | `btsnd_hcic_rmt_name_req` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803276E8` | `0xA0` | `fn_803276E8` | `btsnd_hcic_rmt_name_req_cancel` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327788` | `0x7C` | `fn_80327788` | `btsnd_hcic_rmt_features_req` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327804` | `0x7C` | `fn_80327804` | `btsnd_hcic_rmt_ver_req` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327880` | `0x7C` | `fn_80327880` | `btsnd_hcic_read_rmt_clk_offset` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803278FC` | `0xFC` | `fn_803278FC` | `btsnd_hcic_setup_esco_conn` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803279F8` | `0xE8` | `fn_803279F8` | `btsnd_hcic_accept_esco_conn` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327AE0` | `0x60` | `fn_80327AE0` | `btsnd_hcic_reject_esco_conn` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327B40` | `0xB4` | `fn_80327B40` | `btsnd_hcic_hold_mode` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327BF4` | `0xCC` | `fn_80327BF4` | `btsnd_hcic_sniff_mode` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327CC0` | `0x84` | `fn_80327CC0` | `btsnd_hcic_exit_sniff_mode` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327D44` | `0xB4` | `fn_80327D44` | `btsnd_hcic_park_mode` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327DF8` | `0x84` | `fn_80327DF8` | `btsnd_hcic_exit_park_mode` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327E7C` | `0xB0` | `fn_80327E7C` | `btsnd_hcic_switch_role` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327F2C` | `0x94` | `fn_80327F2C` | `btsnd_hcic_write_policy_set` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80327FC0` | `0x5C` | `fn_80327FC0` | `btsnd_hcic_reset` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x8032801C` | `0x1BC` | `fn_8032801C` | `btsnd_hcic_set_event_filter` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803281D8` | `0x74` | `fn_803281D8` | `btsnd_hcic_write_pin_type` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x8032824C` | `0x60` | `fn_8032824C` | `btsnd_hcic_read_stored_key` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803282AC` | `0x1C0` | `fn_803282AC` | `btsnd_hcic_write_stored_key` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x8032846C` | `0xB0` | `fn_8032846C` | `btsnd_hcic_delete_stored_key` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x8032851C` | `0x140` | `fn_8032851C` | `btsnd_hcic_change_name` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x8032865C` | `0x38` | `fn_8032865C` | `btsnd_hcic_write_page_tout` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328694` | `0x30` | `fn_80328694` | `btsnd_hcic_write_scan_enable` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803286C4` | `0x44` | `fn_803286C4` | `btsnd_hcic_write_pagescan_cfg` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328708` | `0x44` | `fn_80328708` | `btsnd_hcic_write_inqscan_cfg` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x8032874C` | `0x74` | `fn_8032874C` | `btsnd_hcic_write_auth_enable` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803287C0` | `0x74` | `fn_803287C0` | `btsnd_hcic_write_encr_mode` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328834` | `0x44` | `fn_80328834` | `btsnd_hcic_write_dev_class` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328878` | `0x44` | `fn_80328878` | `btsnd_hcic_write_auto_flush_tout` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803288BC` | `0xBC` | `fn_803288BC` | `btsnd_hcic_set_host_buf_size` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328978` | `0x94` | `fn_80328978` | `btsnd_hcic_write_link_super_tout` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328A0C` | `0x70` | `fn_80328A0C` | `btsnd_hcic_write_cur_iac_lap` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328A7C` | `0x60` | `fn_80328A7C` | `btsnd_hcic_read_local_ver` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328ADC` | `0x5C` | `fn_80328ADC` | `btsnd_hcic_read_local_features` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328B38` | `0x28` | `fn_80328B38` | `btsnd_hcic_read_buffer_size` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328B60` | `0x60` | `fn_80328B60` | `btsnd_hcic_read_bd_addr` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328BC0` | `0x7C` | `fn_80328BC0` | `btsnd_hcic_get_link_quality` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328C3C` | `0x78` | `fn_80328C3C` | `btsnd_hcic_read_rssi` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80328CB4` | `0x420` | `fn_80328CB4` | `btsnd_hcic_set_afh_channels` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x803290D4` | `0x30` | `fn_803290D4` | `btsnd_hcic_write_inqscan_type` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80329104` | `0x30` | `fn_80329104` | `btsnd_hcic_write_inquiry_mode` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80329134` | `0x30` | `fn_80329134` | `btsnd_hcic_write_pagescan_type` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x80329164` | `0xF4` | `fn_80329164` | `btsnd_hcic_vendor_spec_cmd` | function | high | F-522-1 | `src/RVL_SDK/bte/hcicmds.c` |
| `0x804AD540` | `0x3AC` | `lbl_804AD540` | `gap_cb` (`tGAP_CB`) | `.bss` object | high | F-522-1 | `src/RVL_SDK/bte/gap_utils.c` |
| `0x804BB630` | `0x6` | `lbl_804BB630` | `BT_BD_ANY` | `.sdata2` object | high | F-522-1 | `src/RVL_SDK/bte/btu_init.c` |
| `0x804AA850` | `0x84` | `lbl_804AA850` | `btu_cb` (`tBTU_CB`) | `.bss` object, outside block | high name | F-522-4 | owned by the `btu_task1.c` block |
| `0x80311070` | `0xE8` | `fn_80311070` | `GKI_getpoolbuf` | function, outside block | high name | F-522-4 | owning block issue |
| `0x80311160` | `0x170` | `fn_80311160` | `GKI_freebuf` | function, outside block | high name | F-522-4 | owning block issue |
| `0x80311590` | `0x13C` | `fn_80311590` | `GKI_enqueue` | function, outside block | high name | F-522-4 | owning block issue |
| `0x80311810` | `0xA0` | `fn_80311810` | `GKI_dequeue` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8031DF18` | `0x78` | `fn_8031DF18` | `btm_discovery_db_init` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8031E91C` | `0xE8` | `fn_8031E91C` | `BTM_ReadRemoteDeviceName` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8031EB3C` | `0xB8` | `fn_8031EB3C` | `BTM_InqDbFirst` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8031EBF4` | `0x13C` | `fn_8031EBF4` | `BTM_InqDbNext` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8031FAE4` | `0x5C` | `fn_8031FAE4` | `btm_init` | function, outside block | high name | F-522-4 | owning block issue |
| `0x80324B88` | `0x18C` | `fn_80324B88` | `btu_hcif_send_cmd` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8032C720` | `0x90` | `fn_8032C720` | `L2CA_Deregister` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8032C9E4` | `0x190` | `fn_8032C9E4` | `L2CA_ConnectRsp` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8032CB74` | `0xB8` | `fn_8032CB74` | `L2CA_ConfigReq` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8032CC2C` | `0xD0` | `fn_8032CC2C` | `L2CA_ConfigRsp` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8032CCFC` | `0xA4` | `fn_8032CCFC` | `L2CA_DisconnectReq` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8032CDA0` | `0xA4` | `fn_8032CDA0` | `L2CA_DisconnectRsp` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8032F7B8` | `0x114` | `fn_8032F7B8` | `l2c_init` | function, outside block | high name | F-522-4 | owning block issue |
| `0x80339170` | `0x360` | `fn_80339170` | `SDP_SetLocalDiRecord` | function, outside block | high name | F-522-4 | owning block issue |
| `0x803394D0` | `0x4B0` | `fn_803394D0` | `SDP_GetLocalDiRecord` | function, outside block | high name | F-522-4 | owning block issue |
| `0x80339E48` | `0x398` | `fn_80339E48` | `SDP_AddAttribute` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8033A1E0` | `0xD8` | `fn_8033A1E0` | `SDP_AddUuidSequence` | function, outside block | high name | F-522-4 | owning block issue |
| `0x8033B8E8` | `0x1A0` | `fn_8033B8E8` | `sdp_init` | function, outside block | high name | F-522-4 | owning block issue |
| `0x803456E8` | `0x4C` | `fn_803456E8` | `memcmp` | function, outside block | high name | F-522-4 | owning block issue |

## Issue 482 complete DVD region

[F-482-2/F-482-3](findings/482-dvd-queue-id.md) records the corrected 128-byte DVD error record, callback chain, named NAND buffers and aligned error records, message tables, external linkage names, and explicit dvd.c tail implementation fragment. All 30 owned functions (4404 bytes) are source-linked and match exactly; no preceding dvd.c functions are counted. Target callbacks establish cover at 0x10 and status at 0x14. Detailed symbol addresses, section extents, provenance and neighboring fallbacks are in that finding.
## RVL_SDK VI (#472)

`wpse-v1` `main.dol`, `.text` file offset is address minus `0x8004FD40`. Names follow the matching RVL_SDK reference source and are confirmed by byte-identical reconstruction in `src/RVL_SDK/vi/vi.c`; confidence high. Evidence: [F-472-5](findings/472-exi-si-db-units.md#f-472-5-vic-unit-and-2009-differences).

| Address | Size | Name | Kind |
| --- | --- | --- | --- |
| `0x802C4080` | `0x198` | `OnShutdown` | static function |
| `0x802C4220` | `0x80C` | `__VIRetraceHandler` | static function |
| `0x802C4A30` | `0x44` | `VISetPreRetraceCallback` | function |
| `0x802C4A80` | `0x44` | `VISetPostRetraceCallback` | function |
| `0x802C4AD0` | `0x118` | `getTiming` | static function |
| `0x802C4BF0` | `0x200` | `__VIInit` | function |
| `0x802C4DF0` | `0x548` | `VIInit` | function |
| `0x802C5340` | `0x54` | `VIWaitForRetrace` | function |
| `0x802C53A0` | `0x2A8` | `setFbbRegs` | static function |
| `0x802C5650` | `0xE0` | `setHorizontalRegs` | static function |
| `0x802C5730` | `0x194` | `setVerticalRegs` | static function |
| `0x802C58D0` | `0x6CC` | `VIConfigure` | function |
| `0x802C5FA0` | `0x354` | `VIConfigurePan` | function |
| `0x802C6300` | `0x114` | `VIFlush` | function |
| `0x802C6420` | `0x6C` | `VISetNextFrameBuffer` | function |
| `0x802C6490` | `0x78` | `VISetBlack` | function |
| `0x802C6510` | `0x8` | `VIGetRetraceCount` | function |
| `0x802C6610` | `0x60` | `VIGetScanMode` | function |
| `0x802C6670` | `0x3C` | `VIGetDTVStatus` | function |
| `0x802C66B0` | `0x234` | `__VIDisplayPositionToXY` | function |
| `0x802C68F0` | `0xF8` | `VISetTimeToDimming` | function |
| `0x802C69F0` | `0x14` | `VIResetDimmingCount` | function |
| `0x802C6A10` | `0x10` | `__VIResetRFIdle` | function |
| `0x802C6A20` | `0x10` | `__VIResetSIIdle` | function |
| `0x803D5380` | `0x1A2` | `timing` | `.data` static, 11 `timing_s` of `0x26` bytes |
| `0x803D5524` | `0x32` | `taps` | `.data` static, 25 `u16` |
| `0x803D5558`, `0x803D5594`, `0x803D55D0` | `0x3C` each | `GXPal528Prog`, `GXPal528ProgSoft`, `GXPal524ProgAa` | `.data` `GXRenderModeObj` |
| `0x803D5610` | `0x10` | `ShutdownFunctionInfo` | `.data` static |
| `0x804539F0`, `0x80453A68` | `0x76` each | `shdwRegs`, `regs` | `.bss` statics |
| `0x80453AE0` | `0x58` | `HorVer` | `.bss` static `HorVer_s` |
| `0x80453B38` | `0x28` | `__VIDimmingFlag_DEV_IDLE` | `.bss` static |
| `0x804B8AE0` | 4 | `__VIVersion` | `.sdata` global |
| `0x804B95C8`-`0x804B9674` | 4 each, except `changed`, `shdwChanged` and `retraceQueue` 8, `displayOffsetH` and `displayOffsetV` 2 | file and function statics, order in F-472-5 | `.sbss` |

Outside this unit, call sites name `__VISetYUVSEL` (`0x802C7350`), `__VISetFilter4EURGB60` (`0x802C73F0`), `__VISetCGMS` (`0x802C7430`), `__VISetWSS` (`0x802C7490`), `__VISetClosedCaption` (`0x802C7500`), `__VISetMacrovision` (`0x802C7570`), `__VISetGamma1_0` (`0x802C8450`), `__VISetGamma` (`0x802C8460`), `__VISetTrapFilter` (`0x802C8480`), `__VISetRGBOverDrive` (`0x802C84E0`), `VISetRGBModeImm` (`0x802C8570`), `__VISetRevolutionModeSimple` (`0x802C85D0`), `Vdac_Flag_Changed` (`.sbss` `0x804B9680`), `OSWakeupThread` (`0x802BCF60`), `__OSSetVIForceDimming` (`0x802BDDC0`), `SCGetDisplayOffsetH` (`0x802E9640`), `SCGetScreenSaverMode` (`0x802E97A0`), `__shl2i` (`0x803408DC`) and `__DVDStopMotorCommandBlock` (`.bss` `0x80458ED0`). Only names changed; extents and splits stay with their owners.
## Issue 486 AX command, output, studio, and voice interfaces

Original SDK names cover all 35 functions at 0x802DB290-0x802DD2A0; exact object matches and full source-linked verification corroborate them. AXCL owns __AXCommandList at 0x8045C620 and small state at 0x804B9898-0x804B98BC. The table pointer at 0x804B98A4 and release count at 0x804B98A0 have descriptive, behavior-proven names sCompressorTable and sCompressorReleaseFrames; their original spelling is unknown.

AXOut owns the 56-byte AXPROFILE at 0x8045C720, four 180-sample remote buffers at 0x8045C760, 768-byte surround buffer at 0x8045CD00, three 96-word output buffers at 0x8045D000, 80-byte DSPTaskInfo at 0x8045D480, and 64-byte DSP DRAM image at 0x8045D4E0. AXPROFILE has six signed 64-bit timestamps at offsets 0,8,16,24,32,40 and a signed 32-bit voice count at48; its final four bytes are alignment padding. AXSTUDIO at 0x8045D520 has twenty packed s32/s16 sample/depop pairs, total120 bytes. Source declarations retain the original per-channel member names. AXVPB's implementation fragment references shared pointers/counters at 0x804B9950-0x804B996C and cycle tables at 0x803D8578/0x803D85F8 without moving their storage or claiming the later #487 functions. See [F-486-2](findings/486-axcl.md#f-486-2-command-list-output-studio-and-parameter-buffer-reconstruction) for callee evidence and limitations.
## Issue 493: Product information, WENC and remaining ENC units

[F-493-2 through F-493-4](findings/493-enc-utility-names.md) establish the source units, compiler override and exact verification. Function names have high confidence from the reference implementations, instructions and relocations. Outside functions retain their owning splits and sizes. The previously documented parameter/break helpers keep their names.

| Address | Function |
| --- | --- |
| `0x800046B4` | `memset` |
| `0x802B2E90` | `OSRegisterVersion` |
| `0x802E9A90` | `__SCF1` |
| `0x802E9C00` | `SCGetProductArea` |
| `0x802E9C90` | `SCGetProductGameRegion` |
| `0x802E9D14` | `WENCGetEncodeData` |
| `0x802E9FF0` | `ENCiRegisterVersion` |
| `0x802EA270` | `ENCiConvertStringUnicodeToAscii` |
| `0x802EA4B0` | `ENCiConvertStringAsciiToUnicode` |
| `0x802EA690` | `ENCiConvertStringUtf32ToUtf16` |
| `0x802EA950` | `ENCiConvertStringUtf16ToUtf32` |
| `0x802EABF0` | `ENCiConvertStringUtf16ToUtf8` |
| `0x802EAF00` | `ENCiConvertStringUtf8ToUtf16` |
| `0x802EB220` | `ENCiConvertStringUtf7ToUtf16` |
| `0x802EB540` | `ENCiConvertStringUtf16ToUtf16` |
| `0x802EB7C0` | `ENCiConvertUtf8To32` |
| `0x802EB870` | `ENCiConvertUtf32To8` |
| `0x80345DDC` | `strcmp` |

`ProductAreaAndStringTbl` at `0x803DC840` contains fourteen five-byte records: signed area byte at 0, four-character array at 1. `ProductGameRegionAndStringTbl` at `0x803DC888` contains six four-byte records: signed game-region byte at 0, three-character array at 1. Their final records use a negative sentinel. The reference keyword strings are `AREA` at `0x804B8D48` and `GAME` at `0x804B8D50`, each five bytes including the terminator.

The existing `WENCInfo` declaration is reused unchanged. The target accesses six signed 32-bit state fields `xn`, `dl`, `qn`, `dn`, `dlh`, `dlq` at offsets 0, 4, 8, 12, 16 and 20. Its existing eight-byte reserved tail is not accessed by this function. The coefficient table at `0x80371868` contains eight doubles; the conversion constant at `0x804BB4F0` is another double.

`__ENCVersion` is a four-byte pointer at `0x804B8D58` to the 70-byte observed version string at `0x803DC8A0`. `encRegistered` is a four-byte BOOL at `0x804B99D0`. `base64_array` is a 128-byte lookup table at `0x803DC8E8`. `ENCiConvertUtf8To32` and `ENCiConvertUtf32To8` are the two emitted private Unicode helpers; other retained helpers inline.
## Complete VI encoder and affine matrix units (#474)

[F-474-5 and F-474-6](findings/474-matrix.md#f-474-5-complete-affine-matrix-unit) document all 31 newly source-linked functions, original reference names, owned layouts, and C versus inline-assembly coverage. `__VISetGamma1_0` at `0x802C8450` and `gEncoderState` at `0x804B9684` are descriptive names: the former selects gamma 1.0; the latter is proven only to receive zero at initialization's end. Other encoder and matrix names have high confidence from reference correspondence and exact function comparison. `VIGammaObj` is 34 bytes and `VIMacroVisionObj` is 26 bytes. Owned scalar `gammaSet` has `noreloc` because adjacent u16 outputs resemble a pointer.

Referenced outside-issue names, without size/type/split changes: `VIGetDTVStatus` at `0x802C6670`, `PSVECNormalize` at `0x802C9940`, `PSVECCrossProduct` at `0x802C9A00`, `cos` at `0x8034CEE4`, and `sin` at `0x8034D3EC`. Evidence is the pinned matching SDK call sites and corresponding target behavior. This claims no reconstruction coverage for those callees.

## RVL_SDK MEM, CX, and DSP (#489)

Names given in `config/wpse-v1/symbols.txt`; evidence in [489-mem-cx-dsp](findings/489-mem-cx-dsp.md).

| Address | Size | Old name | Name | Unit |
| --- | --- | --- | --- | --- |
| `0x802E18F0` | `0x84` | `fn_802E18F0` | `MEMCreateFrmHeapEx` | `mem_frameHeap.c` |
| `0x802E1980` | `0x30` | `fn_802E1980` | `MEMDestroyFrmHeap` | `mem_frameHeap.c` |
| `0x802E19B0` | `0x120` | `fn_802E19B0` | `MEMAllocFromFrmHeapEx` | `mem_frameHeap.c` |
| `0x802E1AD0` | `0xA0` | `fn_802E1AD0` | `MEMFreeToFrmHeap` | `mem_frameHeap.c` |
| `0x802E1B70` | `0x10` | `fn_802E1B70` | `AllocatorAllocForExpHeap_` (local) | `mem_allocator.c` |
| `0x802E1B80` | `0x8` | `fn_802E1B80` | `AllocatorFreeForExpHeap_` (local) | `mem_allocator.c` |
| `0x802E1B90` | `0x10` | `fn_802E1B90` | `MEMAllocFromAllocator` | `mem_allocator.c` |
| `0x802E1BA0` | `0x10` | `fn_802E1BA0` | `MEMFreeToAllocator` | `mem_allocator.c` |
| `0x802E1BB0` | `0x1C` | `fn_802E1BB0` | `MEMInitAllocatorForExpHeap` | `mem_allocator.c` |
| `0x804BB4E8` | `0x8` | `lbl_804BB4E8` | `sAllocatorFunc` (local) | `mem_allocator.c` |
| `0x802E1CF0` | `0x40` | `fn_802E1CF0` | `CXInitUncompContextHuffman` | `CXStreamingUncompression.c` |
| `0x802E1D30` | `0x3C8` | `fn_802E1D30` | `CXReadUncompHuffman` | `CXStreamingUncompression.c` |
| `0x802E2100` | `0x40` | `fn_802E2100` | `CXGetUncompressedSize` | `CXUncompression.c` |
| `0x802E2140` | `0x14C` | `fn_802E2140` | `CXUncompressLZ` | `CXUncompression.c` |
| `0x802E2290` | `0x160` | `fn_802E2290` | `CXUncompressHuffman` | `CXUncompression.c` |
| `0x802E23F0` | `0x170` | `fn_802E23F0` | `CXSecureUncompressRL` | `CXSecureUncompression.c` |
| `0x802E2560` | `0x220` | `fn_802E2560` | `CXSecureUncompressLZ` | `CXSecureUncompression.c` |
| `0x802E2780` | `0x1B0` | `fn_802E2780` | `CXiVerifyHuffmanTable_` | `CXSecureUncompression.c` |
| `0x802E2930` | `0x25C` | `fn_802E2930` | `CXSecureUncompressHuffman` | `CXSecureUncompression.c` |
| `0x802E2B90` | `0x178` | `fn_802E2B90` | `CXiHuffImportTree` | `CXSecureUncompression.c` |
| `0x802E2D10` | `0xE8` | `fn_802E2D10` | `CXSecureUnfilterDiff` | `CXSecureUncompression.c` |
| `0x802E2E00` | `0x1F4` | `fn_802E2E00` | `CXiLHVerifyTable` | `CXSecureUncompression.c` |
| `0x802E3000` | `0x390` | `fn_802E3000` | `CXSecureUncompressLH` | `CXSecureUncompression.c` |
| `0x802E3390` | `0x1E8` | `fn_802E3390` | `RCAddCount_` (local) | `CXSecureUncompression.c` |
| `0x802E3580` | `0x94` | `fn_802E3580` | `RCSearch_` (local) | `CXSecureUncompression.c` |
| `0x802E3620` | `0x4BC` | `fn_802E3620` | `CXSecureUncompressLRC` | `CXSecureUncompression.c` |
| `0x802E3AE0` | `0x10` | `fn_802E3AE0` | `DSPCheckMailToDSP` | `dsp.c` |
| `0x802E3AF0` | `0x10` | `fn_802E3AF0` | `DSPCheckMailFromDSP` | `dsp.c` |
| `0x802E3B00` | `0x14` | `fn_802E3B00` | `DSPReadMailFromDSP` | `dsp.c` |
| `0x802E3B20` | `0x14` | `fn_802E3B20` | `DSPSendMailToDSP` | `dsp.c` |
| `0x802E3B40` | `0xC0` | `fn_802E3B40` | `DSPInit` | `dsp.c` |
| `0x802E3C00` | `0x8` | `fn_802E3C00` | `DSPCheckInit` | `dsp.c` |
| `0x802E3C10` | `0x70` | `fn_802E3C10` | `DSPAddTask` | `dsp.c` |
| `0x802E3C80` | `0xBC` | `fn_802E3C80` | `DSPAssertTask` | `dsp.c` |
| `0x804B8BE8` | `0x4` | `lbl_804B8BE8` | `__DSPVersion` | `dsp.c` |
| `0x804B9988` | `0x4` | `lbl_804B9988` | `__DSP_init_flag` (local) | `dsp.c` |
| `0x802E3D40` | `0x50` | `fn_802E3D40` | `__DSP_debug_printf` | `dsp_debug.c` |
| `0x802E3D90` | `0x42C` | `fn_802E3D90` | `__DSPHandler` | `dsp_task.c` |
| `0x802E41C0` | `0x1A4` | `fn_802E41C0` | `__DSP_exec_task` | `dsp_task.c` |
| `0x802E4370` | `0x18C` | `fn_802E4370` | `__DSP_boot_task` | `dsp_task.c` |
| `0x802E4500` | `0xA0` | `fn_802E4500` | `__DSP_insert_task` | `dsp_task.c` |
| `0x802E45A0` | `0x88` | `fn_802E45A0` | `__DSP_remove_task` | `dsp_task.c` |
| `0x804B9990`–`0x804B99A4` | `0x4` each | `lbl_804B9990`… | `__DSP_rude_task_pending`, `__DSP_rude_task`, `__DSP_tmp_task`, `__DSP_last_task`, `__DSP_first_task`, `__DSP_curr_task` | `dsp_task.c` |

`MEMiFrmHeapHead` is 0xC bytes after `MEMiHeapHead`: head allocator `0x0`, tail allocator `0x4`, state list `0x8`. `CXUncompContextHuffman` is 0x220 bytes: `destp` `0x0`, `destCount` `0x4`, `forceDestCount` `0x8`, `treep` `0xC`, `srcTmp` `0x10`, `destTmp` `0x14`, `treeSize` `0x18` (s16), `srcTmpCnt` `0x1A`, `destTmpCnt` `0x1B`, `bitSize` `0x1C`, `headerSize` `0x1D`, `tree[0x200]` `0x20`; offsets proven by `CXInitUncompContextHuffman`, field names from the NITRO/TWL SDK convention (medium confidence). `DSPTaskInfo` `state`, `priority`, and `flags` must be `vu32`; the remaining layout is Petari's.

## RVL_SDK OS (#464)

`wpse-v1` `main.dol`; [F-464-3](findings/464-os.md). Names follow xbret/xenoblade's `OS.c` for the same SDK build. The unit is `NonMatching`; confidence is high for the functions, which compare at 100%, and medium for the statics.

| Address | Size | Name | Kind |
| --- | --- | --- | --- |
| `0x802B2E80` | `0x10` | `__OSGetDIConfig` | function, low byte of DI register 9 |
| `0x802B2EA0` | `0x84` | `OSGetAppGamename` | function |
| `0x802B2F30` | `0x20` | `OSGetAppType` | function, wraps static `GetAppType` |
| `0x80452540` | `0x20` | `DriveInfo` | `.bss` static `DVDDriveInfo` |
| `0x80452560` | `0x30` | `DriveBlock` | `.bss` static `DVDCommandBlock` |
| `0x804B8A50` | 4 | `AppGameNameForSysMenu` | `.sdata` static `const char*` |
| `0x804B9468` | 5 | `GameNameBuffer` | `.sbss` static `char[5]` |

## RVL_SDK ISFS (#498)

`wpse-v1` `main.dol`; [F-498-1](findings/498-fs.md). Names follow Petari's `fs.c` and `ipcclt.c`, confirmed by byte-identical linked reconstruction; confidence high.

| Address | Size | Name | Kind |
| --- | --- | --- | --- |
| `0x802F38E0` | `0x154` | `ISFS_ReadDir` | function |
| `0x802F3A40` | `0x148` | `ISFS_ReadDirAsync` | function |
| `0x802F4200` | `0x138` | `ISFS_GetUsage` | function |
| `0x802F4B10` | `0x4` | `ISFS_Close` | function, tail call to `IOS_Close` |
| `0x802F4B20` | `0x84` | `ISFS_CloseAsync` | function |
| `0x802F4BB0` | `0x90` | `ISFS_ShutdownAsync` | function |
| `0x802F1D30`-`0x802F2840` | | `IOS_CloseAsync`, `IOS_ReadAsync`, `IOS_Read`, `IOS_WriteAsync`, `IOS_Write`, `IOS_SeekAsync`, `IOS_Seek`, `IOS_Ioctl`, `IOS_IoctlvAsync` | IPC client functions in address order (unit not yet split) |
| `0x804B9A10` | 4 | `_asynCnt` | `.sbss` static |
| `0x804B9A14` | 4 | `lo` in `ISFS_OpenLib` | `.sbss` function static |
| `0x804B9A18` | 4 | `hi` in `ISFS_OpenLib` | `.sbss` function static |

`__isfsCtxt` is 0x140 bytes: a 32-byte-aligned 256-byte `ioBuf` at 0x0, `cb` 0x100, `ctxt` 0x104, `func` 0x108, argument union 0x10C.

## RVL_SDK PAD (#499)

`wpse-v1` `main.dol`; [F-499-1](findings/499-pad.md). Names follow zeldaret/tp's `Pad.c` and `Padclamp.c`, confirmed by byte-identical linked reconstruction; confidence high. The two clamp-region names are descriptive (medium confidence).

| Address | Size | Name | Kind |
| --- | --- | --- | --- |
| `0x802F4C40` | `0x130` | `ClampStick` | static function |
| `0x802F4D70` | `0x11C` | `ClampCircle` | static function |
| `0x802F4E90` | `0xF0` | `PADClamp` | function |
| `0x802F4F80` | `0xE8` | `PADClampCircle` | function |
| `0x802F5070` | `0xB4` | `PADClamp2` | function |
| `0x802F5130` | `0xA4` | `PADClampCircle2` | function |
| `0x802F51E0` | `0xA8` | `PADClampTrigger` | function |
| `0x802F5290`-`0x802F6B00` | | `UpdateOrigin`, `PADOriginCallback`, `PADOriginUpdateCallback`, `PADProbeCallback`, `PADTypeAndStatusCallback`, `PADReceiveCheckCallback`, `PADReset`, `PADRecalibrate`, `PADInit`, `PADRead`, `PADControlMotor`, `SPEC0_MakeStatus`, `SPEC1_MakeStatus`, `SPEC2_MakeStatus`, `OnShutdown`, `SamplingHandler` | functions in address order |
| `0x80397DC0` | `0xA` | `MarginClampRegion` | `.rodata` `PADClampRegion`, minimums 30/15/15 |
| `0x80397DCC` | `0xA` | `NoMarginClampRegion` | `.rodata` `PADClampRegion`, minimums 0 |
| `0x803DCB58` | `0x10` | `ShutdownFunctionInfo` | `.data` `OSShutdownFunctionInfo`, priority 127 |
| `0x80477420` | `0x30` | `Origin` | `.bss` `PADStatus[4]` |
| `0x80477450` | `0x10` | `Type` | `.bss` `u32[4]` |
| `0x80477460` | `0x10` | `CmdProbeDevice` | `.bss` `u32[4]` |
| `0x80477470` | `0x30` | `pre_status` in `SPEC2_MakeStatus` | `.bss` function static, unreferenced |
| `0x804774A0` | `0x30` | `pre_status` in `PADRead` | `.bss` function static |
| `0x804B8DA8`-`0x804B8DC4` | 4 each | `__PADVersion`, `ResettingChan`, `XPatchBits`, `AnalogMode`, `Spec`, `MakeStatus`, `CmdReadOrigin`, `CmdCalibrate` | `.sdata` |
| `0x804B9A20`-`0x804B9A48` | 4 each | `recalibrated` in `OnShutdown`, `SamplingCallback`, `__PADSpec`, `BarrelBits`, `PendingBits`, `CheckingBits`, `WaitingBits`, `RecalibrateBits`, `ResettingBits`, `EnabledBits`, `Initialized` | `.sbss` |

`PADStatus` is 0xC bytes in arrays: `button` (u16) 0x0, `stickX` 0x2, `stickY` 0x3, `substickX` 0x4, `substickY` 0x5, `triggerLeft` 0x6, `triggerRight` 0x7, `analogA` 0x8, `analogB` 0x9, `err` (s8) 0xA. `PADClampRegion` is 10 bytes: `minTrigger`, `maxTrigger`, `minStick`, `maxStick`, `xyStick`, `minSubstick`, `maxSubstick`, `xySubstick`, `radStick`, `radSubstick`.

## RVL_SDK OSAlloc (#464)

`wpse-v1` `main.dol`; [F-464-2](findings/464-os-alloc.md). Names follow Petari's `OSAlloc.c` and are confirmed by byte-identical linked reconstruction; confidence high.

| Address | Size | Name | Kind |
| --- | --- | --- | --- |
| `0x802B3810` | `0xB0` | `DLInsert` | static function, `.text` file offset `0x263AD0` |
| `0x804B8A58` | 4 | `__OSCurrHeap` | `.sdata` global `volatile OSHeapHandle`, initial `-1` |
| `0x804B9490` | 4 | `HeapArray` | `.sbss` static `HeapDesc*` |

`HeapDesc` is 12 bytes: `long size` at 0x0, free list `Cell*` at 0x4, allocated list `Cell*` at 0x8. `Cell` is a 12-byte header with `prev`, `next` and `long size`; allocations return the cell address plus 32.
## HBM GUI manager (#287)

[F-287-3](findings/287-hbm-core.md#f-287-3-hbmguimanagercpp-is-one-unit-from-0x801787f0-to-0x8017a008) names the 56 `homebutton::gui` functions at `0x801787F0`–`0x8017A008` (`drawLine_`, `Component::update`, `Manager` and `PaneManager` members, `is_visible`, `PaneComponent::contain`, `draw` and `isVisible`, and the weak inline members they emit), the vtables and RTTI at `0x803BA060`–`0x803BA2D0`, and `PaneManager::suIDCounter` at `0x803E99E0` (4 bytes; was a dtk 8-byte guess). Confidence high: RTTI strings give the classes, vtable slots give the member order, and every function matches. Outside names (nw4hbm list and pane functions, lyt `typeInfo` statics, MEM allocator, GX and PSMTX functions, `operator new`) are recorded in the finding with their addresses. Class layouts are in the finding.

## HBM controller (#289)

[F-289-1](findings/289-hbm-controller.md#f-289-1-hbmcontrollercpp-spans-0x8017a010-to-0x8017ac20) names the 25 `homebutton::Controller` functions at `0x8017A010`–`0x8017AC20` and its statics `sBatteryFlag` (`0x803E99E8`, 4 bytes; was a dtk 8-byte guess), `sAlarm` (`0x803E99F0`), `sAlarmSoundOff` (`0x803E9AB0`) and `sThis` (`0x803E9B70`). Confidence high: the CC0 key reference gives the names and every function matches. It also names nine WPAD functions and five `RemoteSpk` members called from the unit; the addresses and the `Controller` and `RemoteSpk` layouts are in the finding.

## HBM remote speaker (#290)

[F-290-1](findings/290-hbm-remotespk.md#f-290-1-hbmremotespkcpp-spans-0x8017ac20-to-0x8017b53c) names the 15 `homebutton::RemoteSpk` functions at `0x8017AC20`–`0x8017B53C`, `__vt__Q210homebutton9RemoteSpk` (`0x803BA2D0`), `__RTTI__Q210homebutton9RemoteSpk` (`0x803BA2F8`) and `RemoteSpk::spInstance` (`0x803E9B80`, 4 bytes; was a dtk 8-byte guess). It also names `WPADCanSendStreamData` (`0x802FC730`) and `WPADSendStreamData` (`0x802FC7C0`). Confidence high: RTTI gives the class, the CC0 references give the member names, and every function matches. The `ChanInfo` and `RemoteSpk` layouts are in the finding.

## HBM sequence player (#291)

[F-291-1](findings/291-hbm-axsound.md#f-291-1-hbmaxsoundcpp-spans-0x8017b540-to-0x8017c064) names the ten `HBMAxSound.cpp` functions at `0x8017B540`–`0x8017C064`, `WT_FILENAME` (`0x80366A68`), `PCM_FILENAME` (`0x80366A7C`), `SOUND_FILENAME` (`0x803BA5E0`) and `sWork` (`0x803E9B88`, 4 bytes; was a dtk 8-byte guess). Confidence high: CC0 xenoblade symbols give every name with identical sizes and order. The finding also lists 20 callees named for linking: the `HBMSEQ`, `HBMSYN` and `HBMMIX` entry points, plus `OSCreateThread`, `OSJoinThread`, `OSResumeThread`, `AICheckInit` and `AIInit`. Medium confidence: the descriptive names of the `SeqPlayer`, `SeqPlayerList` and `AxSoundWork` members. `reserved` (`0x14344`) and `padding` (`0x146BC`, 0x44 bytes) are not accessed in this unit. The sound-effect enum gains `HBMSE_MANUAL_OPEN` to `HBMSE_MANUAL_RETURN_APP` (22–27) from the MIDI file names.

## NW4R ef draw order and effect (#375)

For `wpse-v1` / `main.dol`, [F-375-1](findings/375-ef-draworder-effect.md) names the 18 functions of `nw4r::ef::DrawOrder` (`0x80225820`, `0x802258B0`, `0x80225930`) and `nw4r::ef::Effect` (`0x80225940`–`0x80226544`) with their mangled CodeWarrior names, and the vtables `__vt__Q34nw4r2ef9DrawOrder` (`.data` `0x803C52D0`, 0x14 bytes) and `__vt__Q34nw4r2ef6Effect` (`.data` `0x803C52E8`, 0x20 bytes). Names are high confidence from the CC0 ogws reference and exact matches, except `Effect::GetFirstEmitter` (`0x802263D0`, 8 bytes), a descriptive name for the active-list head accessor, and `Effect::Calc(u32)`, whose parameter type is inferred from bit tests. Subtract `0x8004FD40` for `.text` file offsets.

Layout: `Effect` is 0xA0 bytes with `mActivityList` at 0x24, the active count at 0x3C, and `mFlags` at 0x50, as in ogws. `Emitter` keeps the ogws layout through `mMtxDirty`, then adds `math::VEC3 mGlobalTranslate` at 0x108 and `math::VEC3 mPrevGlobalTranslate` at 0x114 (descriptive names, low confidence). Placing them before `mMtx` is unverified; no reconstructed function accesses `mMtx` yet. `EmitterDesc::commonFlag` bit 30 gates emitter creation. The outside callees renamed for linking are listed in the finding.

## NW4R ef effect system (#375, #376)

For `wpse-v1` / `main.dol`, [F-375-2](findings/375-ef-draworder-effect.md#f-375-2-nw4r-ef-effect-system-unit) names the 15 functions of `ef_effectsystem.cpp` with mangled CodeWarrior names, high confidence from the CC0 Kirby reference and the Mario Kart Wii symbol map: `EffectSystem::~EffectSystem` (`0x80226550`), `GetInstance` (`0x802265E0`), `Initialize` (`0x802265F0`), `ActivityList::ActivityList` (`0x80226700`), `Closing` (`0x80226750`), `CreateEffect(const char*, u32, u16)` (`0x802267C0`), `CreateEffect(EmitterResource*, u32, u16)` (`0x80226830`), `RetireEffect` (`0x80226960`), `RetireEffectAll` (`0x80226A00`), `RetireEmitterAll` (`0x80226AF0`), `RetireParticleAll` (`0x80226B80`), `Calc(u32, u32)` (`0x80226C10`), `Draw` (`0x80226CB0`), `SetProcessCamera` (`0x80226D40`) and `__sinit_\ef_effectsystem_cpp` (`0x80226DD0`). Data: `mSortEffectOperatorZDrawInfo` (`.bss` `0x80423190`, 0xA0 plus a 0xC destructor-chain record), `instance` (`.bss` `0x8042323C`, 0xC06C), `gBasicDrawOrder`, `gBasicDrawStrategyBuilder`, `gBasicEmitFormBuilder` (`.sbss` `0x804B92B0`–`0x804B92B8`) and `mDisplayVersion` (`.sbss` `0x804B92BC`). Outside symbols named for the link: `Resource::GetInstance` (`0x802347C0`), `Resource::_FindEmitter` (`0x80234C70`), `CreationQueue::CreationQueue` (`0x80236910`), `__vt__Q34nw4r2ef15EmitFormBuilder` (`0x803C55D8`) and `__vt__Q34nw4r2ef19DrawStrategyBuilder` (`0x803C5658`). `EffectSystem` member offsets are in the finding; `CreationQueueData::mExtraTranslate` (0x24) is a low-confidence name.

## NW4R ef emitter (#376, #377)

For `wpse-v1` / `main.dol`, [F-376-1](findings/376-ef-emitter.md) names the 20 functions of `ef_emitter.cpp` (`0x80226F00`–`0x802290A0`) with mangled CodeWarrior names, high confidence from the CC0 Whippy reference and exact matches: `Emitter::Emitter` (`0x80226F00`), `~Emitter` (`0x80226F60`), `SendClosing` (`0x80227030`), `DestroyFunc` (`0x80227040`), `Closing` (`0x80227130`), `RetireParticleManagerAll` (`0x802271E0`), `InitializeDatas` (`0x802272B0`), `Initialize` (`0x80227590`), `CreateEmitter` (`0x80227700`), `CreateEmitterTmp` (`0x802279E0`), the file-local `GetLODratio` (`0x80228160`), `Emission` (`0x80228260`), `CalcEmitter` (`0x802286B0`), `CalcParticle` (`0x80228870`), `CalcEmission` (`0x80228900`), `CalcBillboard` (`0x80228A20`) and `RestructMatrix` (`0x80228C00`), plus `__vt__Q34nw4r2ef7Emitter` (`.data` `0x803C5350`, 0x28). Outside names for the link are listed in the finding, including `ParticleManager::smMtxInvId` (`.sbss` `0x804B92C0`, 4 bytes). `Emitter` places `mGlobalTranslate` at 0x108, `mPrevGlobalTranslate` at 0x114, `mMtxDirty` at 0x120 and `mMtx` at 0x124. The pivot flag names (`EmitterDesc` common-flag bits 10–13, `INHERIT_FLAG_PIVOT`, `FLAG_MTX_INHERIT_PIVOT`) are low confidence.

## NW4R ef particle and particle manager (#378, #379)

For `wpse-v1` / `main.dol`, [F-378-1](findings/378-ef-particle.md) names, with mangled CodeWarrior names and high confidence from the CC0 Whippy reference and exact matches: `Particle::Particle` (`0x80231640`), `~Particle` (`0x80231650`), `SendClosing` (`0x80231690`), `DestroyFunc` (`0x802316A0`), `ParticleParameter::Initialize` (`0x802316B0`), `Particle::Initialize` (`0x802319D0`), `Particle::GetAlphaFlick` (`0x80232270`), `ParticleManager::ParticleManager` (`0x802324B0`), `SendClosing` (`0x80232510`), `DestroyFunc` (`0x80232520`), `Closing` (`0x80232530`), `Initialize` (`0x80232670`), `~ParticleManager` (`0x802327C0`), `CreateParticle` (`0x80232800`), `Calc` (`0x80232930`), `Draw` (`0x80233E90`), `Draw_ModifyColor` (`0x80234390`) and `__sinit_\ef_particlemanager_cpp` (`0x802347B0`). Data: `__vt__Q34nw4r2ef8Particle` (`.data` `0x803C5578`, 0x14), `__vt__Q34nw4r2ef15ParticleManager` (`.data` `0x803C55B8`, 0x20), `ParticleManager::smDrawMtxPMtoEM` (`.bss` `0x8042F2A8`, 0x30) and `Resource::mResource` (`.bss` `0x8042F2D8`, 0x20). `EmitterDrawSetting` gains `mTevTexture[4]` at 0x8 and `mAlphaInput` at 0x68.

## NW4R ef resource (#379)

For `wpse-v1` / `main.dol`, [F-379-1](findings/379-ef-resource.md) names the functions of `ef_resource.cpp` with mangled CodeWarrior names, high confidence from the CC0 Whippy reference and exact matches: the file-local `breffRelocate` (`0x802347D0`), `breffSearchName` (`0x80234940`), `breffIndexOf` (`0x80234A00`), `breftRelocate` (`0x80234AA0`) and `breftSearchName` (`0x80234B30`); `Resource::Add` (`0x80234BF0`), `FindTexture` (`0x80234D20`), `AddTexture` (`0x80234DF0`), `relocateCurveTexture` (`0x80234E70`), `relocateCurveChild` (`0x80234F90`), `RelocateCommand` (`0x80235090`) and `__sinit_\ef_resource_cpp` (`0x802352D0`). `GetInstance` (`0x802347C0`), `_FindEmitter` (`0x80234C70`) and `mResource` (`.bss` `0x8042F2D8`) were named earlier. `TextureData` is 0x40 bytes; its trailing `reserve` member and `AC_TYPE_EMITTER_CHILD` (curve type 2) are low-confidence names.
## MSL runtime support, issue 536

Names, addresses and evidence are listed in [F-536-1](findings/536-runtime.md#recovered-names). All are high confidence from reference correspondence with identical instruction streams. `__throw`, `abort` and `__RTTI__Q23std9exception` lie outside the block and were named only so the source link resolves.
## BTE SDP main, server and utilities (#534)

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x8033B8E8` | `0x1A0` | `fn_8033B8E8` | `sdp_init` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033BA88` | `0xC4` | `fn_8033BA88` | `sdp_connect_ind` | static function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033BB4C` | `0x190` | `fn_8033BB4C` | `sdp_connect_cfm` | static function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033BCDC` | `0x13C` | `fn_8033BCDC` | `sdp_config_ind` | static function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033BE18` | `0x17C` | `fn_8033BE18` | `sdp_config_cfm` | static function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033BF94` | `0xF8` | `fn_8033BF94` | `sdp_disconnect_ind` | static function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033C08C` | `0xD4` | `fn_8033C08C` | `sdp_data_ind` | static function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033C160` | `0x114` | `fn_8033C160` | `sdp_conn_originate` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033C274` | `0x9C` | `fn_8033C274` | `sdp_disconnect` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033C310` | `0xBC` | `fn_8033C310` | `sdp_disconnect_cfm` | static function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033C3CC` | `0x88` | `fn_8033C3CC` | `sdp_conn_timeout` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x8033C454` | `0x154` | `fn_8033C454` | `sdp_server_handle_client_req` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_server.c` |
| `0x8033C5A8` | `0x410` | `fn_8033C5A8` | `process_service_search` | static function | high | F-534-3 | `src/RVL_SDK/bte/sdp_server.c` |
| `0x8033C9B8` | `0x390` | `fn_8033C9B8` | `process_service_attr_req` | static function | high | F-534-3 | `src/RVL_SDK/bte/sdp_server.c` |
| `0x8033CD48` | `0x41C` | `fn_8033CD48` | `process_service_search_attr_req` | static function | high | F-534-3 | `src/RVL_SDK/bte/sdp_server.c` |
| `0x8033D164` | `0x90` | `fn_8033D164` | `sdpu_find_ccb_by_cid` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033D1F4` | `0x74` | `fn_8033D1F4` | `sdpu_allocate_ccb` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033D268` | `0x38` | `fn_8033D268` | `sdpu_release_ccb` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033D2A0` | `0x1D0` | `fn_8033D2A0` | `sdpu_build_attrib_seq` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033D470` | `0x154` | `fn_8033D470` | `sdpu_build_attrib_entry` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033D5C4` | `0x13C` | `fn_8033D5C4` | `sdpu_build_n_send_error` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033D700` | `0x350` | `fn_8033D700` | `sdpu_extract_uid_seq` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033DA50` | `0x250` | `fn_8033DA50` | `sdpu_extract_attr_seq` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033DCA0` | `0xD0` | `fn_8033DCA0` | `sdpu_get_len_from_type` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033DD70` | `0xE0` | `fn_8033DD70` | `sdpu_is_base_uuid` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033DE50` | `0x274` | `fn_8033DE50` | `sdpu_compare_uuid_arrays` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033E0C4` | `0x90` | `fn_8033E0C4` | `sdpu_compare_uuid_with_attr` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x8033E154` | `0x50` | `fn_8033E154` | `sdpu_sort_attr_list` | function | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x80398298` | `0x10` | `lbl_80398298` | `sdp_base_uuid` | `.rodata` static | high | F-534-3 | `src/RVL_SDK/bte/sdp_utils.c` |
| `0x804AEA40` | `0x4634` | `lbl_804AEA40` | `sdp_cb` | `.bss` object (`tSDP_CB`) | high | F-534-3 | `src/RVL_SDK/bte/sdp_main.c` |
| `0x80311070` | unchanged | `fn_80311070` | `GKI_getpoolbuf` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x80311160` | unchanged | `fn_80311160` | `GKI_freebuf` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x80314230` | unchanged | `fn_80314230` | `btu_start_timer` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x803142A0` | unchanged | `fn_803142A0` | `btu_stop_timer` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x803217D8` | unchanged | `fn_803217D8` | `BTM_SetSecurityLevel` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x8032C584` | unchanged | `fn_8032C584` | `L2CA_Register` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x8032C7B0` | unchanged | `fn_8032C7B0` | `L2CA_ConnectReq` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x8032C9E4` | unchanged | `fn_8032C9E4` | `L2CA_ConnectRsp` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x8032CB74` | unchanged | `fn_8032CB74` | `L2CA_ConfigReq` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x8032CC2C` | unchanged | `fn_8032CC2C` | `L2CA_ConfigRsp` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x8032CCFC` | unchanged | `fn_8032CCFC` | `L2CA_DisconnectReq` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x8032CDA0` | unchanged | `fn_8032CDA0` | `L2CA_DisconnectRsp` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x8032CE44` | unchanged | `fn_8032CE44` | `L2CA_DataWrite` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x803399A4` | unchanged | `fn_803399A4` | `sdp_db_service_search` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x80339BB8` | unchanged | `fn_80339BB8` | `sdp_db_find_record` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x80339C14` | unchanged | `fn_80339C14` | `sdp_db_find_attr_in_rec` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x8033A88C` | unchanged | `fn_8033A88C` | `sdp_disc_connected` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x8033A8C0` | unchanged | `fn_8033A8C0` | `sdp_disc_server_rsp` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |
| `0x803456E8` | unchanged | `fn_803456E8` | `memcmp` | external function, name only | high | F-534-3 | callee of the SDP units; split left to its owning issue |

`tSDP_CB` is 0x4634 bytes: `l2cap_my_cfg` 0x0 (0x3C), four 0x478-byte `tCONN_CB` entries at 0x3C, `server_db` 0x121C (0x33E8), `reg_info` 0x4604 (0x28), `max_attr_list_size` 0x462C, `max_recs_per_search` 0x462E, `trace_level` 0x4630. In `tCONN_CB`, `timer_entry` is at 0x8, `rem_mtu_size` 0x20, `connection_id` 0x22, `list_len` 0x24, `rsp_list` 0x26 (1000 bytes), `p_cb` 0x414, `disconnect_reason` 0x472, `cont_offset` 0x476. Layouts follow Petari and are confirmed by the 100% code match.
## Issue 466: OS error, exec, and fatal symbols

[F-466-2](findings/466-oscontext.md) establishes `PackArgs` at `802B56F0`, `Utf16ToArg` `802B5870`, `PackInstallerArgs` `802B59D0`, `Run` `802B5BA0`, `Callback` `802B5BE0`, `__OSGetExecParams` `802B5BF0`, `callback` `802B5C20`, `__OSLaunchNextFirmware` `802B5C30`, `__OSLaunchMenu` `802B62C0`, `__OSBootDolSimple` `802B6390`, and `__OSBootDol` `802B6AF0`. The fatal unit starts with `ScreenReport` `802B6CD0`, followed by `ConfigureVideo` `802B7000`, `OSFatal` `802B7120`, and `Halt` `802B7310`. Names have high confidence from the pinned reference, exact instructions, relocations, and linked binary comparison. External callees receive reference names without changing their splits or types.

`__OSErrorTable` is 17 four-byte handler pointers at `80452590`; `__OSFpscrEnableBits` is a four-byte unsigned mask at `804B8A68`. Exec's 32-byte aligned disk ID is at `804525E0`; `__OSNextPartitionType`, cached apploader position, volatile `DVDLowIntType`, `__OSInReboot`, and volatile `Prepared` occupy four-byte words at `804B94A0`, `804B94A4`, `804B94A8`, `804B94AC`, and `804B94B0`. `FatalContext` is a complete 0x2C8-byte OSContext at `80452600`; `FatalParam` at `804528C8` has GXColor foreground at zero, background at four, and message pointer at eight, size twelve. `OSExecParams` is 0x1C bytes: valid, restart code, DOL offset, saved region start/end, default-argument flag, and argument-buffer pointer in that order, all four-byte slots. Its pointer lives at absolute `800030F0`; current TGC offset at `800030F4`, locked flag at `80003187`, and launch partition type at `80003194` follow the reference and target accesses.

## NW4R ut lists, binary header, character reader, and streams (#445)

For `wpse-v1` / `main.dol`, [F-445-1](findings/445-ut-list-streams.md) names 27 `.text` functions from `0x80292D00` to `0x80293DBC` with their mangled CodeWarrior names (`nw4r::ut::List_*`, `detail::LinkListImpl`, `IsValidBinaryFile`, `CharStrmReader::ReadNextChar*`, `IOStream`, `FileStream`, and `FileStream::FilePosition`), including the static initializers `__sinit_\ut_IOStream_cpp` (`0x80293C30`) and `__sinit_\ut_FileStream_cpp` (`0x80293DB0`). It also names the `.sbss` objects `typeInfo__Q34nw4r2ut8IOStream` (`0x804B93B8`, 4 bytes) and `typeInfo__Q34nw4r2ut10FileStream` (`0x804B93C0`, 4 bytes). All names are high confidence from the CC0 ogws reference and exact matches. Subtract `0x8004FD40` for `.text` file offsets. Layouts: `ut::List` is 0xC bytes with its link offset at 0xA; `FileStream::FilePosition` holds the file size at 0 and the position at 4. `IOStream::Write` returns `s32`.
| `0x802B9F00`-`0x802BAE10` | `OSInitMutex`, `OSLockMutex`, `OSUnlockMutex`, `__OSUnlockAllMutex`, `OSTryLockMutex`, `OSInitCond`, `OSWaitCond`, `OSSignalCond`, `__OSReboot`, `OSGetSaveRegion`, `OSRegisterShutdownFunction`, `__OSCallShutdownFunctions`, `__OSShutdownDevices`, `__OSGetDiscState`, `OSShutdownSystem`, `OSRestart`, `__OSReturnToMenu` (local), `OSReturnToMenu`, `OSReturnToDataManager`, `OSReturnToSetting`, `__OSReturnToMenuForError`, `__OSHotResetForError`, `OSGetResetCode`, `OSResetSystem` | High confidence: matching `OSMutex.c`, `OSReboot.c` and `OSReset.c`. Data: `SaveStart` and `SaveEnd` (`.sbss` `0x804B94E8`, `0x804B94EC`), `bootThisDol` (`0x804B94F0`), `__OSIsReturnToIdle` (`0x804B94F4`), `ShutdownFunctionQueue` (`0x804B94F8`, 8 bytes); [F-468-1](findings/468-os-mutex-reboot-reset.md) |

## BTE SDP API, database and discovery (#533)

Names already recorded elsewhere (`SDP_SetLocalDiRecord`, `SDP_GetLocalDiRecord`, `SDP_AddAttribute`, `SDP_AddUuidSequence`, `sdp_db_*`, `sdp_disc_connected`, `sdp_disc_server_rsp`) are unchanged and now link from these sources.

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x80338B3C` | `0x378` | `fn_80338B3C` | `SDP_InitDiscoveryDb` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_api.c` |
| `0x80338EB4` | `0x5C` | `fn_80338EB4` | `SDP_ServiceSearchRequest` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_api.c` |
| `0x80338F10` | `0x64` | `fn_80338F10` | `SDP_ServiceSearchAttributeRequest` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_api.c` |
| `0x80338F74` | `0x28` | `fn_80338F74` | `SDP_FindAttributeInRec` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_api.c` |
| `0x80338F9C` | `0xCC` | `fn_80338F9C` | `SDP_FindServiceInDb` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_api.c` |
| `0x80339068` | `0x108` | `fn_80339068` | `SDP_FindServiceUUIDInDb` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_api.c` |
| `0x80339980` | `0x24` | `fn_80339980` | `SDP_SetTraceLevel` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_api.c` |
| `0x80339AE0` | `0xD8` | `fn_80339AE0` | `find_uuid_in_seq` | static function | high | F-533-1 | `src/RVL_SDK/bte/sdp_db.c` |
| `0x80339C54` | `0xE0` | `fn_80339C54` | `SDP_CreateRecord` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_db.c` |
| `0x80339D34` | `0x114` | `fn_80339D34` | `SDP_DeleteRecord` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_db.c` |
| `0x8033A2B8` | `0x17C` | `fn_8033A2B8` | `SDP_AddServiceClassIdList` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_db.c` |
| `0x8033A434` | `0x23C` | `fn_8033A434` | `SDP_DeleteAttribute` | function | high | F-533-1 | `src/RVL_SDK/bte/sdp_db.c` |
| `0x8033A670` | `0xEC` | `fn_8033A670` | `sdpu_build_uuid_seq` | static function | high | F-533-1 | `src/RVL_SDK/bte/sdp_discovery.c` |
| `0x8033A75C` | `0x130` | `fn_8033A75C` | `sdp_snd_service_search_req` | static function | high | F-533-1 | `src/RVL_SDK/bte/sdp_discovery.c` |
| `0x8033AA00` | `0x148` | `fn_8033AA00` | `process_service_search_rsp` | static function | high | F-533-1 | `src/RVL_SDK/bte/sdp_discovery.c` |
| `0x8033AB48` | `0x2A8` | `fn_8033AB48` | `process_service_attr_rsp` | static function | high | F-533-1 | `src/RVL_SDK/bte/sdp_discovery.c` |
| `0x8033ADF0` | `0x2D4` | `fn_8033ADF0` | `process_service_search_attr_rsp` | static function | high | F-533-1 | `src/RVL_SDK/bte/sdp_discovery.c` |
| `0x8033B0C4` | `0x1EC` | `fn_8033B0C4` | `save_attr_seq` | static function | high | F-533-1 | `src/RVL_SDK/bte/sdp_discovery.c` |
| `0x8033B2B0` | `0xA0` | `fn_8033B2B0` | `add_record` | static function | high | F-533-1 | `src/RVL_SDK/bte/sdp_discovery.c` |
| `0x8033B350` | `0x598` | `fn_8033B350` | `add_attr` | static function | high | F-533-1 | `src/RVL_SDK/bte/sdp_discovery.c` |
[F-445-2](findings/445-ut-list-streams.md#f-445-2-tagprocessorbase-completes-the-block) adds the 12 `nw4r::ut::TagProcessorBase<char>` and `<wchar_t>` functions (`0x80293290`–`0x80293BE4`) and their vtables `__vt__Q34nw4r2ut19TagProcessorBase<w>` (`.data` `0x803C6960`, 0x14 bytes) and `__vt__Q34nw4r2ut19TagProcessorBase<c>` (`.data` `0x803C6974`, 0x14 bytes), with high confidence. It also names the outside callees `CharWriter::GetFontWidth` (`0x80299C10`), `CharWriter::GetFontHeight` (`0x80299C70`), and `TextWriterBase<char>`/`<wchar_t>::GetLineHeight` (`0x8029A8F0`, `0x8029E420`). `PrintContext<T>` is 0x14 bytes: writer pointer at 0, string at 4, origin x and y at 8 and 0xC, flags at 0x10.
## Issue 469: RTC, semaphore, thread, and time symbols

[F-469-1](findings/469-os-thread-time.md) records exact function extents and target corroboration. `WriteSramCallback` starts RTC at `802BAE30`; `OSInitSemaphore`, `OSWaitSemaphore`, `OSTryWaitSemaphore`, and `OSSignalSemaphore` occupy `802BB8E0`, `802BB940`, `802BB9B0`, and `802BBA00`, with sizes 0x58, 0x70, 0x50, and 0x60. Semaphore behavior and standard SDK names have high confidence; the source-unit file name has medium confidence. Its twelve-byte storage view is count at zero, queue head/tail at four/eight. This implementation uses volatile counter access to retain observed loads, without claiming recovery of the original qualifier.

The existing thread ABI is 0x318 bytes, alignment eight: OSContext at zero (0x2C8 bytes), state/attr at 0x2C8/0x2CA, suspend 0x2CC, priority/base 0x2D0/0x2D4, value 0x2D8, queue 0x2DC, queue link 0x2E0, join queue 0x2E8, mutex pointer 0x2F0, mutex queue 0x2F4, active link 0x2FC, stack base/end 0x304/0x308, error 0x30C, and two specific pointers 0x310. DefaultThread is at `80452938`, RunQueue (32 eight-byte queue records) at `80452C50`, and IdleContext at `80452D50`. Reschedule, RunQueueHint, and RunQueueBits are four-byte words at `804B9500`, `804B9504`, and `804B9508`; SwitchThreadCallback at `804B8A98` points to DefaultSwitchThreadCallback at `802BBAE0`. Reference layouts and all target accesses agree.

Scb at `804528E0` is 0x54 bytes aligned to 32: 64 SRAM bytes, offset 0x40, interrupt-enabled flag 0x44, lock flag 0x48, sync flag 0x4C, and callback pointer 0x50. OSCalendarTime is 0x28 bytes with ten signed-int fields at four-byte intervals: sec, min, hour, mday, mon, year, wday, yday, msec, usec. YearDays and LeapYearDays are twelve signed integers at `803C8D10` and `803C8D40`. The time offset is a signed 64-bit word at absolute `800030D8`. The system-call vector spans `802BBA60–802BBA80`, with its copied-vector end label at `802BBA7C` before the trailing nop. Time-base readers return a stable TBU/TBL pair or just TBL; the calendar conversions begin at `802BD160` and `802BD490`.

## Exception runtime and MSL file and decimal units, issue 537

Names, addresses and evidence are listed in [F-537-1](findings/537-msl-io-exceptions.md#recovered-names). Block names are high confidence from reference correspondence with identical instruction streams. `free`, `fclose`, `fflush`, the console procedures, `__signbitd`, `frexp` and `__float_huge` lie outside the block and were named from their call sites so the source link resolves.
| `0x802D0A60`-`0x802D1A60` | `GXSetProjection`, `GXSetProjectionv`, `GXGetProjectionv`, `GXLoadPosMtxIndx`, `GXLoadNrmMtxImm`, `GXLoadNrmMtxIndx3x3`, `GXSetCurrentMtx`, `GXLoadTexMtxImm`, `GXSetViewportJitter`, `GXSetViewport`, `GXSetScissor`, `GXSetScissorBoxOffset`, `GXSetClipMode`, `GXSetGPMetric`, `GXClearGPMetric`, `cbForReadSync` (local), `DVDCancel` (`0x802D61E0`) | High confidence: matching `GXTransform.c`, `GXPerf.c` and `dvdfs.c`. Data: `__DVDLongFileNameFlag` (`.sdata` `0x804B8B80`), `__DVDThreadQueue`, `MaxEntryNum`, `FstStringStart`, `FstStart`, `BootInfo` (`.sbss` `0x804B96F8`-`0x804B9710`); [F-480-1](findings/480-gx-transform-perf-dvdfs.md) |
## NW4R ut DvdFileStream, NandFileStream, LockedCache, and Font (#446)

For `wpse-v1` / `main.dol`, [F-446-1](findings/446-ut-nand-lockedcache-font.md) names 23 `.text` functions from `0x80293DC0` to `0x80294534` with their mangled CodeWarrior names: `__sinit_\ut_DvdFileStream_cpp`, the `NandFileStream` members and initializer, `nw4r::ut::LC::*` and `__sinit_\ut_LockedCache_cpp`, and `Font::InitReaderFunc`. It also names the ten `NandFileStream` inline virtuals at `0x8027E210–0x8027E2A8` from their vtable slots, `NANDOpen` (`0x802E5CD0`) and `NANDClose` (`0x802E5F70`) from relocation targets, `__vt__Q34nw4r2ut14NandFileStream` (`.data` `0x803C6988`, 0x5C bytes), `typeInfo__Q34nw4r2ut13DvdFileStream` (`.sbss` `0x804B93C8`), `typeInfo__Q34nw4r2ut14NandFileStream` (`.sbss` `0x804B93D0`), and the local `sLCImpl` (`.bss` `0x804524A8`, 0x1C bytes: `bool mIsEnabled` at 0, `OSMutex mMutex` at 4). All names are high confidence from the CC0 ogws reference and exact matches. `NandFileStream` layout: `mFilePosition` at 0x14, `mFileInfo` at 0x1C (`NANDCommandBlock` at 0x1C, `NANDFileInfo` at 0xD8, back pointer at 0x164), then `mCanRead`, `mCanWrite`, `mIsBusy`, `mCloseOnDestroyFlg`, and `mCloseEnableFlg` at 0x168–0x16C.
## BTE RFCOMM frames and utilities (#532)

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x80336D14` | `0xB0` | `fn_80336D14` | `rfc_send_sabme` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80336DC4` | `0xB0` | `fn_80336DC4` | `rfc_send_ua` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80336E74` | `0xC4` | `fn_80336E74` | `rfc_send_dm` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80336F38` | `0xB0` | `fn_80336F38` | `rfc_send_disc` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80336FE8` | `0x198` | `fn_80336FE8` | `rfc_send_buf_uih` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80337180` | `0xE8` | `fn_80337180` | `rfc_send_pn` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80337268` | `0x84` | `fn_80337268` | `rfc_send_fcon` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x803372EC` | `0x84` | `fn_803372EC` | `rfc_send_fcoff` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80337370` | `0x120` | `fn_80337370` | `rfc_send_msc` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80337490` | `0xB0` | `fn_80337490` | `rfc_send_rls` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80337540` | `0x11C` | `fn_80337540` | `rfc_send_rpn` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x8033765C` | `0xA8` | `fn_8033765C` | `rfc_send_test` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80337704` | `0xC0` | `fn_80337704` | `rfc_send_credit` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x803377C4` | `0x574` | `fn_803377C4` | `rfc_parse_data` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x80337D38` | `0x624` | `fn_80337D38` | `rfc_process_mx_message` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_ts_frames.c` |
| `0x8033835C` | `0x38` | `fn_8033835C` | `rfc_calc_fcs` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x80338394` | `0x4C` | `fn_80338394` | `rfc_check_fcs` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x803383E0` | `0x158` | `fn_803383E0` | `rfc_alloc_multiplexer_channel` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x80338538` | `0x88` | `fn_80338538` | `rfc_release_multiplexer_channel` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x803385C0` | `0x74` | `fn_803385C0` | `rfc_timer_start` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x80338634` | `0x58` | `fn_80338634` | `rfc_timer_stop` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x8033868C` | `0x80` | `fn_8033868C` | `rfc_port_timer_start` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x8033870C` | `0x58` | `fn_8033870C` | `rfc_port_timer_stop` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x80338764` | `0xC4` | `fn_80338764` | `rfc_check_mcb_active` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x80338828` | `0x3C` | `fn_80338828` | `rfcomm_process_timeout` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x80338864` | `0x54` | `fn_80338864` | `rfc_sec_check_complete` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x803388B8` | `0x158` | `fn_803388B8` | `rfc_port_closed` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x80338A10` | `0x8C` | `fn_80338A10` | `rfc_inc_credit` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x80338A9C` | `0x3C` | `fn_80338A9C` | `rfc_dec_credit` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x80338AD8` | `0x64` | `fn_80338AD8` | `rfc_check_send_cmd` | function | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x80398198` | `0x100` | `lbl_80398198` | `rfc_crctable` | static object | high | F-532-1 | `src/RVL_SDK/bte/rfc_utils.c` |
| `0x804AE628` | `0x418` | `lbl_804AE628` | `rfc_cb` | object (tRFC_CB) | high | F-532-1 | `rfc_port_if.c` (#531) |
| `0x80310EB0` | `0x14` | `fn_80310EB0` | `GKI_init_q` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x8033373C` | `0x130` | `fn_8033373C` | `PORT_FlowInd` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x80333964` | `0x194` | `fn_80333964` | `port_rfc_closed` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x80334A04` | `0x44` | `fn_80334A04` | `rfc_mx_sm_execute` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x80335648` | `0x90` | `fn_80335648` | `rfc_port_sm_execute` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x80335FFC` | `0xE8` | `fn_80335FFC` | `rfc_process_pn` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x803360E4` | `0x30C` | `fn_803360E4` | `rfc_process_rpn` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x803363F0` | `0x16C` | `fn_803363F0` | `rfc_process_msc` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x8033655C` | `0x8C` | `fn_8033655C` | `rfc_process_rls` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x803365E8` | `0x4` | `fn_803365E8` | `rfc_process_nsc` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x803365EC` | `0x8` | `fn_803365EC` | `rfc_process_test_rsp` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x803365F4` | `0x64` | `fn_803365F4` | `rfc_process_fcon` | function | high | F-532-1 | Not reconstructed (fallback) |
| `0x80336658` | `0x64` | `fn_80336658` | `rfc_process_fcoff` | function | high | F-532-1 | Not reconstructed (fallback) |
## Issue 470: OS state, record, network, UTF, and play-time symbols

[F-470-3](findings/470-os-state-network.md) records source acceptance and target differences. OSSetResetCallback is at 802BD9C0, OSSetPowerCallback at 802BDAB0, __OSDefaultResetCallback at 802BDF50, and __OSDefaultPowerCallback at 802BDF60. Initialization binds the power default to PowerCallback at 804B9530 and reset default to ResetCallback at 804B9534; setters access those same slots. This distinguishes the otherwise identical setter instruction patterns. PlayRecordAlarmCallback is 802BE0D0; its identical tail-call sequence also appears inside start-play-record code, so the unit boundary and callback relocation establish the standalone entry. Callback names have high confidence from pinned source and complete code/relocation comparison.

UcsAnsiTable at 803C8D70 contains 32 halfwords. The Unicode page maps are 0x200-byte halfword arrays beginning at 803C8DB0; UcsSjisTable at 803D47B0 contains 256 pointers ending at 803D4BB0. Its non-null entries establish every page address and name. The maps are character data; only the final pointer table has address relocations.

Network shutdown metadata is a 16-byte OSShutdownFunctionInfo at 80453620. Five 32-byte aligned request/result buffers occupy 80453640, 80453660, 80453680, 804536A0, and 804536C0. Shutdown fd is signed 32-bit at 804B8AB0; retry count, pending flag, shutdown-in-progress flag, and completion result are four-byte words at 804B9560, 804B9564, 804B9568, and 804B956C. Descriptive private buffer/state names reflect proven behavior, not recovered original identifiers. NWC24iPrepareShutdown starts at 802BEAD0, NWC24iSynchronizeRtcCounter 802BEB90, NWC24SuspendScheduler 802BEC30, NWC24iRequestShutdown 802BED30, NWC24Shutdown_ 802BEDA0, NWC24iSetRtcCounter_ 802BEE60, and CallbackAsyncIpc 802BEF90. Retained literal tags corroborate the otherwise stripped helper names.

Play-time expiration's 0x30-byte alarm is at 804536E0. __OSExpireAIFade pointer is at 804B9570, expired-file flag at 804B9574, void(void) callback at 804B9578, and signed 64-bit expiration time at 804B9580. The fade record has two 576-byte sample buffers, buffer index 0x480, source pointer 0x484, frame counter 0x488, signed left/right samples 0x48C/0x48E, and DMA callback 0x490, total 0x494 bytes before allocation alignment. Existing OSPlayRecord and OSStateFlags declarations are corroborated by all member accesses, checksum extents, file sizes, and exact data comparisons.

## NW4R ut RomFont (#446, #447)

For `wpse-v1` / `main.dol`, [F-447-1](findings/447-ut-romfont.md) names the 26 `.text` functions of `ut_RomFont.cpp` from `0x80294540` to `0x80294C20` with their mangled CodeWarrior names, including `Font::~Font` (`0x802945A0`), `RomFont::Unload` (`0x80294700`), and `RomFont::HasGlyph` (`0x80294B30`). It also names `__vt__Q34nw4r2ut7RomFont` (`.data` `0x803C6A28`, 0x60 bytes), `__vt__Q34nw4r2ut4Font` (`.data` `0x803C6A88`, 0x60 bytes), and `mFontEncode__Q34nw4r2ut7RomFont` (`.sdata` `0x804B89F8`, `u16`). These names are high confidence from the CC0 ogws and Petari references and exact matches. `GetBaselinePos__Q34nw4r2ut4FontCFUs` (`0x8012F070`, 0x10 bytes) is a descriptive, low-confidence name for the inline `Font` virtual at vtable offset 0x20, which forwards to `GetBaselinePos()`. `RomFont` layout: `mReadFunc` at 0x4, `mFontHeader` at 0x10, `mDefaultWidths` at 0x14, `mAlternateChar` at 0x18.

## MSL wide-character, console and fdlibm units, issue 539

Names, addresses and evidence are listed in [F-539-1](findings/539-msl-wide-math.md#recovered-names). The math kernel and wrapper names at `0x80349664`, `0x803498FC`, `0x80349B54` and `0x8034D530`-`0x8034D538` replace the issue 540 names; see [F-539-2](findings/539-msl-wide-math.md#f-539-2-correcting-the-math-wrapper-names-from-issue-540). `InitializeUART`, `WriteUARTN`, `__TRK_write_console`, `raise` and `__fpclassifyf` lie outside the block and were named from their call sites so the source link resolves.
## SC system configuration (#492)

[F-492-2](findings/492-sc.md) identifies `scsystem.c` and `scapi.c` with the pinned Petari source, target SDK banner, instruction comparison and relocation analysis. Names are high confidence original reference names. `SCItem` retains the imported integer storage union, two type bytes, name length, data size, name/data pointers and packed size; target field accesses corroborate this layout. `SCControl` is 0x190 bytes and retains its imported thread queue, NAND file/command storage, reload state, two file/buffer/size arrays and flush state. No field layouts are inferred merely from names.

| Function | Address |
| --- | --- |
| `SCInit` | `0x802E7C50` |
| `SCCheckStatus` | `0x802E7CF0` |
| `SCReloadConfFileAsync` | `0x802E7E10` |
| `OpenCallbackFromReload` | `0x802E7F30` |
| `ReadCallbackFromReload` | `0x802E8000` |
| `CloseCallbackFromReload` | `0x802E80F0` |
| `FinishFromReload` | `0x802E8180` |
| `CloseCallbackFromReloadError` | `0x802E82B0` |
| `ParseConfBuf` | `0x802E82C0` |
| `UnpackItem` | `0x802E8510` |
| `DeleteItemByID` | `0x802E8690` |
| `CreateItemByID` | `0x802E8830` |
| `SCFindByteArrayItem` | `0x802E8AB0` |
| `SCReplaceByteArrayItem` | `0x802E8B90` |
| `SCFindU8Item` | `0x802E8CC0` |
| `SCFindS8Item` | `0x802E8DA0` |
| `SCFindU32Item` | `0x802E8E80` |
| `SCReplaceU8Item` | `0x802E8F60` |
| `__SCFlushSyncCallback` | `0x802E9070` |
| `SCFlushAsync` | `0x802E9080` |
| `MyNandCallback` | `0x802E92A0` |
| `SCGetAspectRatio` | `0x802E95E0` |
| `SCGetDisplayOffsetH` | `0x802E9640` |
| `SCGetEuRgb60Mode` | `0x802E96C0` |
| `SCGetIdleMode` | `0x802E9720` |
| `SCGetLanguage` | `0x802E9730` |
| `SCGetScreenSaverMode` | `0x802E97A0` |
| `SCGetSoundMode` | `0x802E9800` |
| `SCGetCounterBias` | `0x802E9860` |
| `SCGetBtDeviceInfoArray` | `0x802E98A0` |
| `SCSetBtDeviceInfoArray` | `0x802E98B0` |
| `SCGetBtCmpDevInfoArray` | `0x802E98C0` |
| `SCSetBtCmpDevInfoArray` | `0x802E98D0` |
| `SCGetBtDpdSensibility` | `0x802E98E0` |
| `SCGetWpadMotorMode` | `0x802E9950` |
| `SCSetWpadMotorMode` | `0x802E99B0` |
| `SCGetWpadSensorBarPosition` | `0x802E99C0` |
| `SCGetWpadSpeakerVolume` | `0x802E9A20` |
| `SCSetWpadSpeakerVolume` | `0x802E9A80` |

System storage: `Control` at 8046F040 (0x190 bytes), `ConfBuf` at 8046F1E0 (0x4000), `ConfBufForFlush` at 804731E0 (0x4000), `NameAndIDTbl` at 803DC6F0 (37 eight-byte records), and the three path arrays at 80371810/80371820/80371838. `__SCVersion` is the pointer at 804B8C38. Small runtime state occupies 804B99B8–804B99CF; all extents exclude following unit padding.

## BTE HCI event interface, issue 521

[F-521-1](findings/521-bte-btu-hcif.md) gives the `btu_hcif.c` boundaries and [F-521-3](findings/521-bte-btu-hcif.md#f-521-3-external-names) the call-site evidence for the outside names. [F-521-4](findings/521-bte-btu-hcif.md#f-521-4-remaining-btm_secc-functions-in-the-521-block) proposes names for the unnamed `btm_sec.c` functions in the block; they are not applied.

| Address | Size | Old name | Name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x803245D0` | `0x5B8` | `fn_803245D0` | `btu_hcif_process_event` | function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x80324D14` | `0xDC` | `fn_80324D14` | `btu_hcif_connection_comp_evt` | static function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x80324DF0` | `0x8C` | `fn_80324DF0` | `btu_hcif_connection_request_evt` | static function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x80324E7C` | `0x100` | `fn_80324E7C` | `btu_hcif_qos_setup_comp_evt` | static function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x80324F7C` | `0xE0` | `fn_80324F7C` | `btu_hcif_esco_connection_comp_evt` | static function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x8032505C` | `0x158` | `fn_8032505C` | `btu_hcif_hdl_command_complete` | static function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x803251B4` | `0x118` | `fn_803251B4` | `btu_hcif_command_complete_evt` | static function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x803252CC` | `0x200` | `fn_803252CC` | `btu_hcif_hdl_command_status` | static function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x803254CC` | `0x128` | `fn_803254CC` | `btu_hcif_command_status_evt` | static function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x803255F4` | `0x158` | `fn_803255F4` | `btu_hcif_cmd_timeout` | function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x8032574C` | `0xE0` | `fn_8032574C` | `btu_hcif_link_key_notification_evt` | static function | high | F-521-1 | `src/RVL_SDK/bte/btu_hcif.c` |
| `0x803116D0` | `0x140` | `fn_803116D0` | `GKI_enqueue_head` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x803119B0` | `0x10` | `fn_803119B0` | `GKI_queue_is_empty` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031A878` | `0x170` | `fn_8031A878` | `btm_acl_encrypt_change` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031ABE8` | `0x104` | `fn_8031ABE8` | `btm_read_link_policy_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031ACEC` | `0x8C` | `fn_8031ACEC` | `btm_read_remote_version_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031AD78` | `0x22C` | `fn_8031AD78` | `btm_read_remote_features_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031B194` | `0x9C` | `fn_8031B194` | `btm_process_clk_off_comp_evt` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031B230` | `0x1F4` | `fn_8031B230` | `btm_acl_role_changed` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031B664` | `0xD8` | `fn_8031B664` | `btm_qos_setup_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031B9B4` | `0x11C` | `fn_8031B9B4` | `btm_read_rssi_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031BAD0` | `0x118` | `fn_8031BAD0` | `btm_read_link_quality_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031C6E0` | `0x9C` | `fn_8031C6E0` | `BTM_DeviceReset` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031C824` | `0x1C` | `fn_8031C824` | `BTM_IsDeviceUp` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031CBC8` | `0x1C4` | `fn_8031CBC8` | `btm_reset_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031CD8C` | `0xC8` | `fn_8031CD8C` | `btm_read_hci_buf_size_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031CE54` | `0xBC` | `fn_8031CE54` | `btm_read_local_version_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031CF10` | `0x4FC` | `fn_8031CF10` | `btm_read_local_features_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031D4C4` | `0x8C` | `fn_8031D4C4` | `btm_read_local_name_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031D590` | `0x48` | `fn_8031D590` | `btm_read_local_addr_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031D7D8` | `0x6C` | `fn_8031D7D8` | `btm_vsc_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031D87C` | `0x98` | `fn_8031D87C` | `btm_vendor_specific_evt` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031DC2C` | `0x7C` | `fn_8031DC2C` | `btm_read_stored_link_key_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031DCA8` | `0x5C` | `fn_8031DCA8` | `btm_write_stored_link_key_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031DD04` | `0x68` | `fn_8031DD04` | `btm_delete_stored_link_key_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031DD6C` | `0x18C` | `fn_8031DD6C` | `btm_return_link_keys_evt` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031DEF8` | `0x20` | `fn_8031DEF8` | `btm_report_device_status` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031F260` | `0x220` | `fn_8031F260` | `btm_event_filter_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031F480` | `0x278` | `fn_8031F480` | `btm_process_inq_results` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031F6F8` | `0x104` | `fn_8031F6F8` | `btm_process_inq_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8031F94C` | `0x158` | `fn_8031F94C` | `btm_process_remote_name` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x803204FC` | `0xB0` | `fn_803204FC` | `btm_pm_proc_cmd_status` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x803205AC` | `0x228` | `fn_803205AC` | `btm_pm_proc_mode_change` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x803209F4` | `0x18C` | `fn_803209F4` | `btm_sco_chk_pend_unpark` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x80320B80` | `0x1E0` | `fn_80320B80` | `btm_sco_conn_req` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x80320D60` | `0x1A0` | `fn_80320D60` | `btm_sco_connected` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8032103C` | `0x114` | `fn_8032103C` | `btm_sco_removed` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x803213B0` | `0x104` | `fn_803213B0` | `btm_esco_proc_conn_chg` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x803214B4` | `0x70` | `fn_803214B4` | `btm_is_sco_active` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8032276C` | `0x12C` | `fn_8032276C` | `btm_sec_conn_req` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x803229A8` | `0x2E8` | `fn_803229A8` | `btm_sec_rmt_name_request_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x80322C90` | `0x344` | `fn_80322C90` | `btm_sec_auth_complete` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x80322FD4` | `0xFC` | `fn_80322FD4` | `btm_sec_mkey_comp_event` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x803230D0` | `0x13C` | `fn_803230D0` | `btm_sec_encrypt_change` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8032324C` | `0x480` | `fn_8032324C` | `btm_sec_connected` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x80323788` | `0x13C` | `fn_80323788` | `btm_sec_disconnected` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x803238C4` | `0x1AC` | `fn_803238C4` | `btm_sec_link_key_notification` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x80323A70` | `0x130` | `fn_80323A70` | `btm_sec_link_key_request` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x80323C14` | `0x35C` | `fn_80323C14` | `btm_sec_pin_code_request` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x80323F70` | `0x50` | `fn_80323F70` | `btm_sec_update_clock_offset` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8032E780` | `0x238` | `fn_8032E780` | `l2c_link_hci_conn_comp` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8032EA6C` | `0xBC` | `fn_8032EA6C` | `l2c_link_hci_disc_comp` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8032EB28` | `0x70` | `fn_8032EB28` | `l2c_link_hci_qos_violation` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8032F278` | `0xE4` | `fn_8032F278` | `l2c_link_process_num_completed_pkts` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8032F5AC` | `0x80` | `fn_8032F5AC` | `l2c_link_role_changed` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8032F62C` | `0x6C` | `fn_8032F62C` | `l2c_link_role_change_failed` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x8032F774` | `0x44` | `fn_8032F774` | `l2c_pin_code_request` | function, outside unit | high name | F-521-3 | split left to its owning issue |
| `0x804AAD78` | `0x27C8` | `lbl_804AAD78` | `btm_cb` | `.bss` object, outside unit | high name | F-521-3 | split left to its owning issue |
## NW4R ut ResFontBase (#447, #448, #449)

For `wpse-v1` / `main.dol`, [F-448-1](findings/448-ut-resfontbase.md) names the 29 `.text` functions of `ut_ResFontBase.cpp` from `0x80294C20` to `0x80295578` with their mangled CodeWarrior names in `nw4r::ut::detail::ResFontBase`, including `RemoveResourceBuffer` (`0x80294CD0`), `HasGlyph` (`0x80295110`), `FindGlyphIndex(u16)` (`0x802952A0`), and `GetGlyphFromIndex` (`0x80295470`), plus `__vt__Q44nw4r2ut6detail11ResFontBase` (`.data` `0x803C6AF8`, 0x60 bytes). These names are high confidence from the CC0 ogws and Petari references and exact matches. `ResFontBase` layout (0x1C bytes): `mReadFunc` at 0x4, `mResource` at 0x10, `mFontInfo` at 0x14, `mLastCharCode` (`u16`) at 0x18, `mLastGlyphIndex` (`u16`) at 0x1A; the last two names are descriptive, medium confidence.
## HBM common API (#291)

For `wpse-v1` / `main.dol`, [F-291-3](findings/291-hbm-axsound.md#f-291-3-hbmcommoncpp-and-the-homebutton-layout) names the 15 `HBMCommon.cpp` functions at `0x8017C070`–`0x8017C464` (`HBMAllocMem__FUl`, `HBMFreeMem__FPv`, then `HBMCreate` to `HBMCreateInfoEx` with C linkage), `__HBMVersion` (`.data` `0x803BA698`), `spAllocator` (`0x803BA69C`) and the local `sAllocator` (`.bss` `0x803E9B90`, 0x10 bytes). Confidence is high for every name except `HBMCreateInfoEx` (`0x8017C430`), which is medium because key calls its own name a guess. Callees named for linking, with high confidence from key, xenoblade and their bodies: the `homebutton::HomeButton` members `createInstance` (`0x8017CA90`), `deleteInstance` (`0x8017CAE0`), `getInstance` (`0x8017CB30`), `create` (`0x8017CB40`), `init` (`0x8017D5F0`), `calc` (`0x8017E0D0`), `draw` (`0x80180680`), `getSelectBtnNum` (`0x80185990`), `setAdjustFlag` (`0x801859B0`), `startBlackOut` (`0x80185BC0`), `update_sound` (`0x80185DB0`) and `createInfoEx` (`0x80185E00`), plus `nw4hbm::lyt::Layout::mspAllocator` (`.bss` `0x803E9BF0`). `HomeButton` is 0x828 bytes. Its member offsets are in the finding. Most member names come from key and xenoblade. These descriptive names have low confidence: `mPaneCounterEx` (`0x58`), `mpLayoutEx` (`0x244`), `mpTextEx` (`0x2DC`), `mpResFontEx` (`0x304`), `mpAnmControllerEx` (`0x4C4`) and `BlackFader::fadeColorEnable_` (`+0xC`).
## Issue 478: GX texture and indirect texturing

[F-478-1](findings/478-gx-texture-bump.md) names all 40 functions in `0x802CE630`-`0x802CFB84`. They come from Petari's CC0 `GXTexture.c` and `GXBump.c`, in the target's order and with matching sizes and bodies; confidence is high. `GXInitTexObjFilter` (`0x802CEBC0`) is not in Petari. It is named after the public RVL SDK function whose two filter fields it writes in `mode0`, using `GX2HWFiltConv`; confidence is medium-high. GXTexture's nine 8-byte static tables (`GXTexMode0Ids` through `HW2GXFiltConv`) occupy `.sdata` `0x804B8B38`-`0x804B8B80`, in declaration order. The `.sdata2` constant `1024.0f` at `0x804BB380` used by `GXSetIndTexMtx` is 4 bytes; `0x804BB384`-`0x804BB388` is padding, so its symbol size is now `0x4`.
## NAND library units (#491)

[F-491-1](findings/491-nand.md) recovers the following original reference names from pinned Petari/cmwp sources, target instructions, call patterns and data relocations. Confidence is high. The two local quota state names are descriptive rather than recovered original spellings. Existing NANDFileInfo and NANDCommandBlock field accesses agree with the imported declarations. NANDBanner is 0xF0A0 bytes: signature/flag/iconSpeed at 0/4/8, reserved bytes at 0xA, comments at 0x20, banner texture at 0xA0 and icon textures at 0x60A0.

| Function | Address |
| --- | --- |
| `nandOpen` | `0x802E5BC0` |
| `NANDOpen` | `0x802E5CD0` |
| `NANDPrivateOpen` | `0x802E5D60` |
| `NANDOpenAsync` | `0x802E5DF0` |
| `NANDPrivateOpenAsync` | `0x802E5E70` |
| `nandOpenCallback` | `0x802E5EF0` |
| `NANDClose` | `0x802E5F70` |
| `NANDCloseAsync` | `0x802E5FE0` |
| `nandCloseCallback` | `0x802E6070` |
| `nandRemoveTailToken` | `0x802E60D0` |
| `nandGetHeadToken` | `0x802E61B0` |
| `nandGetRelativeName` | `0x802E6290` |
| `nandConvertPath` | `0x802E6330` |
| `nandIsPrivatePath` | `0x802E6470` |
| `nandIsUnderPrivatePath` | `0x802E64B0` |
| `nandIsInitialized` | `0x802E6510` |
| `nandConvertErrorCode` | `0x802E6530` |
| `nandGenerateAbsPath` | `0x802E66A0` |
| `NANDInit` | `0x802E6770` |
| `nandOnShutdown` | `0x802E68A0` |
| `nandShutdownCallback` | `0x802E6970` |
| `nandChangeDir` | `0x802E6980` |
| `NANDChangeDir` | `0x802E6C20` |
| `nandChangeDirCallback` | `0x802E6C70` |
| `NANDGetCurrentDir` | `0x802E6CF0` |
| `NANDGetHomeDir` | `0x802E6D70` |
| `nandCallback` | `0x802E6DD0` |
| `nandGetType` | `0x802E6E10` |
| `NANDPrivateGetTypeAsync` | `0x802E70F0` |
| `nandGetTypeCallback` | `0x802E7140` |
| `nandGetHomeDir` | `0x802E71C0` |
| `NANDInitBanner` | `0x802E71D0` |
| `NANDSecretGetUsage` | `0x802E72B0` |
| `NANDCheckAsync` | `0x802E7370` |
| `nandUserAreaCallback` | `0x802E7420` |
| `reserveFileDescriptor` | `0x802E7660` |
| `NANDLoggingAddMessageAsync` | `0x802E76C0` |
| `asyncRoutine` | `0x802E77C0` |

NANDCore storage: s_currentDir at 803DC440 (64 bytes), s_homeDir at 8046ECE0 (64 bytes), s_shutdownFuncInfo at 803DC480 (16 bytes), s_libState at 804B99A8 (four bytes), __NANDVersion pointer at 804B8BF8. Quota directory table USER_DIR_LIST is at 803DC5B0 (44 bytes). Logging buffers start at 8046ED40/8046EE40/8046EF40; descriptor is at 804B8C30, callback/stage at 804B99B0/804B99B4.

## BTE RFCOMM port state machine and interface (#531)

`rfc_port_sm_execute` and the eight `rfc_process_*` handlers were named under #532 (F-532-1) and now link from `src/RVL_SDK/bte/rfc_port_fsm.c`.

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x803356D8` | `0x174` | `fn_803356D8` | `rfc_port_sm_state_closed` | static function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_fsm.c` |
| `0x8033584C` | `0x19C` | `fn_8033584C` | `rfc_port_sm_sabme_wait_ua` | static function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_fsm.c` |
| `0x803359E8` | `0x1BC` | `fn_803359E8` | `rfc_port_sm_term_wait_sec_check` | static function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_fsm.c` |
| `0x80335BA4` | `0x150` | `fn_80335BA4` | `rfc_port_sm_orig_wait_sec_check` | static function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_fsm.c` |
| `0x80335CF4` | `0x1EC` | `fn_80335CF4` | `rfc_port_sm_opened` | static function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_fsm.c` |
| `0x80335EE0` | `0x11C` | `fn_80335EE0` | `rfc_port_sm_disc_wait_ua` | static function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_fsm.c` |
| `0x803366BC` | `0x88` | `fn_803366BC` | `rfc_process_l2cap_congestion` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_fsm.c` |
| `0x80336744` | `0xA4` | `fn_80336744` | `rfc_set_port_state` | static function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_fsm.c` |
| `0x803367E8` | `0x2C` | `fn_803367E8` | `RFCOMM_StartRsp` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x80336814` | `0x68` | `fn_80336814` | `RFCOMM_DlcEstablishReq` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x8033687C` | `0x70` | `fn_8033687C` | `RFCOMM_DlcEstablishRsp` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x803368EC` | `0xE0` | `fn_803368EC` | `RFCOMM_ParNegReq` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x803369CC` | `0x28` | `fn_803369CC` | `RFCOMM_ParNegRsp` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x803369F4` | `0xBC` | `fn_803369F4` | `RFCOMM_PortNegReq` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x80336AB0` | `0x20` | `fn_80336AB0` | `RFCOMM_PortNegRsp` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x80336AD0` | `0xA4` | `fn_80336AD0` | `RFCOMM_ControlReq` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x80336B74` | `0xA4` | `fn_80336B74` | `RFCOMM_FlowReq` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x80336C18` | `0x98` | `fn_80336C18` | `RFCOMM_LineStatusReq` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x80336CB0` | `0x2C` | `fn_80336CB0` | `RFCOMM_DlcReleaseReq` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x80336CDC` | `0x38` | `fn_80336CDC` | `RFCOMM_DataReq` | function | high | F-531-1 | `src/RVL_SDK/bte/rfc_port_if.c` |
| `0x803224D0` | see `symbols.txt` | `fn_803224D0` | `btm_sec_mx_access_request` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x803228F4` | see `symbols.txt` | `fn_803228F4` | `btm_sec_abort_access_req` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x803328F8` | see `symbols.txt` | `fn_803328F8` | `PORT_ParNegInd` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x80332AA8` | see `symbols.txt` | `fn_80332AA8` | `PORT_ParNegCnf` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x80332BAC` | see `symbols.txt` | `fn_80332BAC` | `PORT_DlcEstablishInd` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x80332CEC` | see `symbols.txt` | `fn_80332CEC` | `PORT_DlcEstablishCnf` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x80332E30` | see `symbols.txt` | `fn_80332E30` | `PORT_PortNegInd` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x80332F80` | see `symbols.txt` | `fn_80332F80` | `PORT_PortNegCnf` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x80333098` | see `symbols.txt` | `fn_80333098` | `PORT_ControlInd` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x80333210` | see `symbols.txt` | `fn_80333210` | `PORT_ControlCnf` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x803332D4` | see `symbols.txt` | `fn_803332D4` | `PORT_LineStatusInd` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x8033339C` | see `symbols.txt` | `fn_8033339C` | `PORT_DlcReleaseInd` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x803334A4` | see `symbols.txt` | `fn_803334A4` | `Port_TimeOutCloseMux` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x8033353C` | see `symbols.txt` | `fn_8033353C` | `PORT_DataInd` | function | high | F-531-1 | Not reconstructed (fallback) |
| `0x80333D50` | see `symbols.txt` | `fn_80333D50` | `port_find_mcb_dlci_port` | function | high | F-531-1 | Not reconstructed (fallback) |

## NW4R ut ResFont (#449)

For `wpse-v1` / `main.dol`, [F-449-1](findings/449-ut-resfont-archivefont.md) names the five `.text` functions of `ut_ResFont.cpp` from `0x80295580` to `0x8029588C`: `ResFont::ResFont`, `~ResFont`, `SetResource`, `RemoveResource` (`0x80295770`), and the static `Rebuild` (`0x80295780`), plus `__vt__Q34nw4r2ut7ResFont` (`.data` `0x803C6B58`, 0x60 bytes). High confidence from the CC0 ogws and Petari references and exact matches. `ResFont` adds no members to `ResFontBase`.
## AI unit (#483, #484)

[F-483-1](findings/483-dvd-ai.md) corroborates the original Petari function and storage names through exact source instructions, register operands and initialized data. The unit starts in #483 at AIRegisterDMACallback; its remaining ten functions are in #484. The timer-calibration locals use descriptive names based on measured elapsed ticks and sample counts.

| Function | Address |
| --- | --- |
| `AIRegisterDMACallback` | `0x802D9EE0` |
| `AIInitDMA` | `0x802D9F30` |
| `AIStartDMA` | `0x802D9FB0` |
| `AIGetDMABytesLeft` | `0x802D9FD0` |
| `AIGetDMAStartAddr` | `0x802D9FE0` |
| `AIGetDMALength` | `0x802DA000` |
| `AICheckInit` | `0x802DA010` |
| `AIInit` | `0x802DA020` |
| `__AIDHandler` | `0x802DA1A0` |
| `__AICallbackStackSwitch` | `0x802DA250` |
| `__AI_SRC_INIT` | `0x802DA2C0` |

Version pointer __AIVersion is at 804B8BC8. Initialization/callback-active flags are four bytes at 804B9800/804B9804; five eight-byte timing values occupy 804B9808–804B9830. Saved stack, callback stack and DMA callback are four-byte pointers at 804B9830/804B9834/804B9838. Source assembly is necessary for __AICallbackStackSwitch because it changes the stack pointer.

## Vector, GXInit and GXAttr (#475)

For `wpse-v1` / `main.dol`, [F-475-2](findings/475-gxinit-gxattr-vec.md) names the vector routines `PSVECAdd` (`0x802C9910`, `0x24`), `PSVECMag` (`0x802C9990`, `0x44`), `PSVECDotProduct` (`0x802C99E0`, `0x20`), `C_VECHalfAngle` (`0x802C9A40`, `0xD8`) and `PSVECSquareDistance` (`0x802C9B20`, `0x28`); the GXInit statics `__GXDefaultTexRegionCallback` (`0x802C9B50`, `0xE4`), `__GXDefaultTlutRegionCallback` (`0x802C9C40`, `0x24`) and `__GXShutdown` (`0x802C9C70`, `0x170`); and the GXAttr functions from `GXSetVtxDesc` (`0x802CB6C0`) to `GXSetNumTexGens` (`0x802CC600`). All are high confidence from Petari and zeldaret/tp call structure and identical compiled code. Data objects `gxData` (`.bss:0x80453C00`, 0x600 bytes), `DefaultTexData` (`.data:0x803D5F00`, 32-byte aligned), `GXDefaultVATList`, `GXDefaultProjData`, `peCount`, `time`, `calledOnce` and `shutdownFuncRegistered` are file-local. `gGXVersionString` is a descriptive name for the anonymous version literal. GXFifo's state layout is now declared in `include/revolution/gx/GXFifo.h`; the F-475-1 header under `include/RVL_SDK/gx/` is removed. `CPUFifo` is 0x24 bytes and `CPGPLinked` one byte; the remaining bytes are padding.

## RevoEX NET and NCD (#285)

For `wpse-v1` / `main.dol`, [F-285-2](findings/285-revoex-ncd.md#f-285-2-unit-boundaries-names-and-data) names the `.text` functions `NETGetRexPPCVersionPrintable` (`0x801770F4`, 8 bytes), `NETGetWirelessMacAddress` (`0x801770FC`, 4 bytes), `NETSHA1Init` (`0x80177100`, `0x50`), `NETSHA1Update` (`0x80177150`, `0xB4`), `NETSHA1GetDigest` (`0x80177204`, `0x12C`) and `NCDiGetWirelessMacAddress` (`0x801776CC`, `0x128`). These names are high confidence, from the CC0 Wii Menu decompilation and matching Mario Kart Wii symbol sizes. The local `NETSHA1ProcessBlock` (`0x80177330`, `0x39C`) and `NCDiLock` (`0x801777F4`, `0xF0`) have descriptive names.

Data: `NETRexPPCVersionPrintableString` (`.sdata:0x804B80E0`, 4) has a high-confidence name. `__NCDVersion` (`.sdata:0x804B80E8`, 4) follows the SDK convention. The remaining names are descriptive: `ncdInitFlags` (`.sbss:0x804B90F8`, 4), `ncdIpcBuffer` (`.sbss:0x804B90FC`, 4), `ncdMutex` (`.bss:0x803E9980`, `0x18`), `ncdReply` (`.bss:0x803E99A0`, `0x20`, 32-byte aligned), `ncdVectors` (`.bss:0x803E99C0`, `0x20`, four `IOSIoVector`s), `sPaddingHead` (`.sdata2:0x804BA320`, 1) and `sPaddingZero` (`.sbss2:0x804BBC00`, 8). `NETSHA1Context` is `0x60` bytes: `state[5]` at 0, `buffer[64]` at `0x14`, `bufferLength` at `0x54`, `blockCountLo` at `0x58` and `blockCountHi` at `0x5C`.
## AX initialization acceptance (#484)

[F-484-2](findings/484-ai.md#f-484-2-ax-initializer-dependency-and-extent-correction) confirms AXInit at 802DA490 (0x54 bytes) and AXIsInit at 802DA4F0 (8 bytes). AXInit is an original reference name; AXIsInit remains the descriptive name for the target-only initialization query established by F-484-1. __AXVersion at 804B8BD0 and __AXInitialized at 804B9840 are four-byte pointer/int objects; their old eight-byte labels incorrectly included alignment. __AXVPBInit at 802DD2A0 (0x30 bytes) is high confidence from the reference initialization sequence and target stores of 96 voices and three buffer pointers before the common-initializer tail call. Only that external name changes; its source unit remains owned by #487.

## HBM layout animation and archive accessors (#293)

For `wpse-v1` / `main.dol`, [F-293-1](findings/293-nw4hbm-lyt.md#f-293-1-lyt_animationcpp-and-lyt_arcresourceaccessorcpp) names 23 functions with their mangled CodeWarrior names. The 11 `nw4hbm::lyt` animation functions are at `0x80186340`–`0x80186EE8`: the unnamed-namespace `GetStepCurveValue` and `GetHermiteCurveValue`, `AnimTransform::GetFrameSize`, the eight `AnimTransformBasic` members, and `detail::FindAnimationLink`. The 12 archive accessor functions are at `0x80186EF0`–`0x80187520`: the unnamed-namespace `FindNameResource` and `GetResourceSub`, the `ArcResourceAccessor` members, `ArcResourceLink::Set`, and the `MultiArcResourceAccessor` members. The finding also names the vtables `__vt__Q36nw4hbm3lyt18AnimTransformBasic` (`0x803BB4E0`), `__vt__Q36nw4hbm3lyt24MultiArcResourceAccessor` (`0x803BB568`) and `__vt__Q36nw4hbm3lyt19ArcResourceAccessor` (`0x803BB5B8`), the RTTI records at `0x803BB530`, `0x803BB558`, `0x803BB5B0`, `0x803BB600` and `0x803BB628`, and the outside callees listed in the finding. Confidence high: xenoblade symbols give the same functions with identical sizes, key gives the NAND additions, and every function matches.
## Issue 471: OSCrc and OSLaunch

[F-471-3](findings/471-exibios.md#f-471-3-oscrc-and-oslaunch) names `OSCalcCRC32` (`0x802BF9A0`), with its 16-entry nibble table at `.rodata` `0x80371630`. It also names the OSLaunch functions `PackArgs` (`0x802BFAE0`, file-local, a second copy of OSExec's), `__OSCheckCompanyCode`, `__OSCheckTmdSysVersion`, `__OSGetValidTicketIndex`, `__OSLaunchTitle`, `LaunchCommon` and `OSLaunchManualViewer` (`0x802C08D0`), and `OSLaunchNoReturnFlag` (`.sbss` `0x804B9588`, 4 bytes). The OSLaunch names follow Petari, and `OSLaunchManualViewer` comes from its own diagnostic string; confidence is high. `0x804B8AC8` is the one-byte empty panic message, followed by padding. External names recovered from call sites that match the reference: `DVDGetCurrentDiskID` (`0x802D62B0`), `__DVDResetWithNoSpinup` (`0x802D63A0`), `NANDGetHomeDir` (`0x802E6D70`) and `NANDSecretGetUsage` (`0x802E72B0`).
## BTE RFCOMM L2CAP interface and multiplexer state machine (#530)

`rfc_mx_sm_execute` was named under #532 (F-532-1) and now links from `src/RVL_SDK/bte/rfc_mx_fsm.c`.

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x803340D0` | `0x7C` | `fn_803340D0` | `rfcomm_l2cap_if_init` | function | high | F-530-1 | `src/RVL_SDK/bte/rfc_l2cap_if.c` |
| `0x8033414C` | `0x8C` | `fn_8033414C` | `RFCOMM_ConnectInd` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_l2cap_if.c` |
| `0x803341D8` | `0x104` | `fn_803341D8` | `RFCOMM_ConnectCnf` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_l2cap_if.c` |
| `0x803342DC` | `0x108` | `fn_803342DC` | `RFCOMM_ConfigInd` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_l2cap_if.c` |
| `0x803343E4` | `0x108` | `fn_803343E4` | `RFCOMM_ConfigCnf` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_l2cap_if.c` |
| `0x803344EC` | `0x4` | `fn_803344EC` | `RFCOMM_QoSViolationInd` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_l2cap_if.c` |
| `0x803344F0` | `0x124` | `fn_803344F0` | `RFCOMM_DisconnectInd` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_l2cap_if.c` |
| `0x80334614` | `0x2A0` | `fn_80334614` | `RFCOMM_BufDataInd` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_l2cap_if.c` |
| `0x803348B4` | `0x134` | `fn_803348B4` | `RFCOMM_CongestionStatusInd` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_l2cap_if.c` |
| `0x803349E8` | `0x1C` | `fn_803349E8` | `rfc_save_lcid_mcb` | function | high | F-530-1 | `src/RVL_SDK/bte/rfc_l2cap_if.c` |
| `0x80334A48` | `0x214` | `fn_80334A48` | `rfc_mx_sm_state_idle` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_mx_fsm.c` |
| `0x80334C5C` | `0x1BC` | `fn_80334C5C` | `rfc_mx_sm_state_wait_conn_cnf` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_mx_fsm.c` |
| `0x80334E18` | `0x128` | `fn_80334E18` | `rfc_mx_sm_state_configure` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_mx_fsm.c` |
| `0x80334F40` | `0x174` | `fn_80334F40` | `rfc_mx_sm_sabme_wait_ua` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_mx_fsm.c` |
| `0x803350B4` | `0x120` | `fn_803350B4` | `rfc_mx_sm_state_wait_sabme` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_mx_fsm.c` |
| `0x803351D4` | `0x118` | `fn_803351D4` | `rfc_mx_sm_state_connected` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_mx_fsm.c` |
| `0x803352EC` | `0x178` | `fn_803352EC` | `rfc_mx_sm_state_disc_wait_ua` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_mx_fsm.c` |
| `0x80335464` | `0xF4` | `fn_80335464` | `rfc_mx_conf_cnf` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_mx_fsm.c` |
| `0x80335558` | `0xF0` | `fn_80335558` | `rfc_mx_conf_ind` | static function | high | F-530-1 | `src/RVL_SDK/bte/rfc_mx_fsm.c` |
| `0x803326FC` | see `symbols.txt` | `fn_803326FC` | `PORT_StartCnf` | function | high | F-530-1 | Not reconstructed (fallback) |
| `0x8033285C` | see `symbols.txt` | `fn_8033285C` | `PORT_StartInd` | function | high | F-530-1 | Not reconstructed (fallback) |
| `0x80333404` | see `symbols.txt` | `fn_80333404` | `PORT_CloseInd` | function | high | F-530-1 | Not reconstructed (fallback) |
| `0x80333DA4` | see `symbols.txt` | `fn_80333DA4` | `port_find_dlci_port` | function | high | F-530-1 | Not reconstructed (fallback) |

## BTE RFCOMM port utilities (#529)

`port_find_mcb_dlci_port` (F-531-1) and `port_find_dlci_port` (F-530-1) now link from `src/RVL_SDK/bte/port_utils.c`.

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x80333AF8` | `0x17C` | `fn_80333AF8` | `port_select_mtu` | function | high | F-529-1 | `src/RVL_SDK/bte/port_utils.c` |
| `0x80333C74` | `0xDC` | `fn_80333C74` | `port_release_port` | function | high | F-529-1 | `src/RVL_SDK/bte/port_utils.c` |
| `0x80333E2C` | `0x70` | `fn_80333E2C` | `port_flow_control_user` | function | high | F-529-1 | `src/RVL_SDK/bte/port_utils.c` |
| `0x80333E9C` | `0x68` | `fn_80333E9C` | `port_get_signal_changes` | function | high | F-529-1 | `src/RVL_SDK/bte/port_utils.c` |
| `0x80333F04` | `0x1CC` | `fn_80333F04` | `port_flow_control_peer` | function | high | F-529-1 | `src/RVL_SDK/bte/port_utils.c` |
| `0x8031B480` | see `symbols.txt` | `fn_8031B480` | `btm_get_max_packet_size` | function | high | F-529-1 | Not reconstructed (fallback) |

## HBM layout common units (#294)

For `wpse-v1` / `main.dol`, [F-294-1](findings/294-nw4hbm-lyt.md#f-294-1-lyt_bounding-lyt_common-lyt_drawinfo-and-lyt_group) names 29 functions at `0x80187520`–`0x8018893C` with their mangled CodeWarrior names. They are `Bounding` (constructor, destructor, `DrawSelf`, `GetRuntimeTypeInfo`, the weak `ut::Color` destructor and `__sinit_\lyt_bounding_cpp`), the 16 `nw4hbm::lyt::detail` helpers of `lyt_common.cpp` from `EqualsPaneName` (`0x801876A0`) to `InitGXTexObjFromTPL` (`0x801883F0`), `DrawInfo`'s constructor and destructor, and `Group` and `GroupContainer` (`0x801885F0`–`0x8018893C`). The data names are `__vt__Q36nw4hbm3lyt8Bounding` (`0x803BB630`), `__vt__Q36nw4hbm3lyt8DrawInfo` (`0x803BB718`) and `__vt__Q36nw4hbm3lyt5Group` (`0x803BB748`), the RTTI records of `Bounding`, `Pane`, `detail::PaneBase`, `DrawInfo` and `Group`, and `Bounding::typeInfo` (`.bss` `0x803E9BC0`, 4 bytes; dtk guessed 8). The callees named for linking are the `Pane` members `__ct__Q36nw4hbm3lyt4PaneFPCQ46nw4hbm3lyt3res4Pane` (`0x8018C850`), `__dt__Q36nw4hbm3lyt4PaneFv` (`0x8018C970`), `GetVtxColor` (`0x8018CBD0`), `SetVtxColor` (`0x8018CBE0`), `GetColorElement` (`0x8018CBF0`), `SetColorElement` (`0x8018CC20`), `GetVtxColorElement` (`0x8018CC50`), `SetVtxColorElement` (`0x8018CC60`), `FindPaneByName` (`0x8018CC70`), `FindMaterialByName` (`0x8018CD20`), `CalculateMtx` (`0x8018CDE0`), `Draw` (`0x8018D0E0`), `Animate` (`0x8018D1F0`), `AnimateSelf` (`0x8018D280`), `BindAnimation` (`0x8018D340`), `UnbindAnimation` (`0x8018D360`), `UnbindAllAnimation` (`0x8018D3F0`), `UnbindAnimationSelf` (`0x8018D410`), `FindAnimationLink` (`0x8018D4F0`), `SetAnimationEnable` (`0x8018D570`), `LoadMtx` (`0x8018D630`), `GetVtxPos` (`0x8018D740`) and `GetMaterial` (`0x8018D820`), from the `Bounding` vtable order. `ut::detail::LinkListImpl::Erase(Iterator)` (`0x80193810`) is also named. Confidence high: the names match xenoblade's symbols, and the vtable slot order matches the imported `Pane` declaration. The `SetAnimationEnable` parameter list is from the reference header.
## FA common PrFILE2 helpers (#350)

For `wpse-v1` / `main.dol`, [F-350-2](findings/350-fa-common.md#f-350-2-unit-boundaries-names-and-remaining-work) names the FA `.text` functions from `0x801FB030` to `0x801FB920`, in address order: `pf_toupper`, `pf_memcpy`, `pf_memset`, `pf_memcmp`, `pf_strlen`, `pf_strcpy`, `pf_strcmp`, `pf_strncmp`, `pf_w_strlen`, `pf_w_strcpy`, `pf_w_strcmp`, `pf_w_strncmp`, `PFCODE_Combine_Width`, `PFCODE_Divide_Width`, `PF_GET_LE_U16`, `PF_LE16_TO_U16_STR`, `PFSTR_SetCodeMode`, `PFSTR_GetCodeMode`, `PFSTR_SetMarkPos`, `PFSTR_GetStrPos`, `PFSTR_MoveStrPos`, `PFSTR_InitStr`, `PFSTR_StrLen`, `PFSTR_StrNumChar` and `PFSTR_StrCmp`. These are the VF names without the `VFi` prefix, and their confidence is medium. `PFSTR_SetMarkPos` is descriptive.

Data: `pf_valid_fn_char` (`.rodata:0x80368128`, `0x60` bytes) is medium confidence. `pf_vol_set` (`.bss:0x803EB970`) is owned by the FA volume unit; this issue only renames it so the link resolves. In FA, `PF_VOLUME_SET` holds `PF_CHARCODE codeset` at `0x48`. Its six callbacks are `oem2unicode` (`0x48`), `unicode2oem`, `oem_char_width`, `is_oem_mb_char` (`0x54`), `unicode_char_width` (`0x58`) and `is_unicode_mb_char`. The first `0x48` bytes remain opaque. `PF_STR` is `0x10` bytes: `p_head` at 0, `p_tail` at 4, `p_mark` at 8 and `code_mode` at `0xC`.
## AXVPB completion and expanded effects (#487)

[F-487-1](findings/487-axvpb-effects.md) names the following target functions from pinned SDK references and exact target comparison. Expanded-delay public names and members follow the pinned wii-ipl header; its private __InitParams name follows the reference helper convention and has medium confidence. Its bodies are reconstructed from the target. The AXVPB buffers contain 96 parameter blocks (320 bytes each), ITD buffers (64 bytes each), and voices (360 bytes each). Expanded-delay layout is 0x58 bytes with active at 0x34, max/current delay at 0x38/0x3C and bus pointers at 0x48/0x4C; the findings give every member offset and validation.

| Function | Address |
| --- | --- |
| `__AXVPBInit` | `0x802DD2A0` |
| `__AXVPBInitCommon` | `0x802DD2D0` |
| `AXSetVoiceState` | `0x802DD560` |
| `AXSetVoiceAddr` | `0x802DD5D0` |
| `AXGetLpfCoefs` | `0x802DD6C0` |
| `AXSetMaxDspCycles` | `0x802DD780` |
| `AXGetMaxVoices` | `0x802DD790` |
| `__AXGetCurrentProfile` | `0x802DD7A0` |
| `AXFXReverbHiInit` | `0x802DD7E0` |
| `AXFXReverbHiShutdown` | `0x802DD850` |
| `AXFXReverbHiCallback` | `0x802DD880` |
| `AXFXReverbHiExpInit` | `0x802DD890` |
| `AXFXReverbHiExpShutdown` | `0x802DDA30` |
| `AXFXReverbHiExpCallback` | `0x802DDA90` |
| `__AllocDelayLine` | `0x802DDFC0` |
| `__BzeroDelayLines` | `0x802DE140` |
| `__FreeDelayLine` | `0x802DE270` |
| `__InitParams (high reverb)` | `0x802DE390` |
| `AXFXDelayExpGetMemSize` | `0x802DE6C0` |
| `AXFXDelayExpInit` | `0x802DE6F0` |
| `AXFXDelayExpSettings` | `0x802DE9C0` |
| `AXFXDelayExpSettingsUpdate` | `0x802DEB50` |
| `AXFXDelayExpShutdown` | `0x802DEC60` |
| `AXFXDelayExpCallback` | `0x802DED10` |
| `__InitParams` | `0x802DEF90` |

Cycle tables are at 803D8578 (128 bytes) and 803D85F8 (16 bytes). Voice storage starts at 8045D5A0/80464DA0/804665A0 and ends at 8046ECA0; seven small state objects occupy 804B9950–804B996C. The profile flag/index/capacity/pointer occupy 804B9970/804B9974/804B9978/804B997C. Early-size, early-coefficient and filter-size tables start at 803DB5E0/803DB640/803DB6A0, sizes 0x60/0x60/0xE0.
## BTE RFCOMM port API and port RFCOMM (#528)

`RFCOMM_Init` and the `PORT_*` callbacks were named under earlier issues (F-530-1, F-531-1, F-532-1) and now link from `src/RVL_SDK/bte/port_api.c` and `port_rfc.c`.

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x8033386C` | `0xF8` | `fn_8033386C` | `port_rfc_send_tx_data` | function | high | F-528-1 | `src/RVL_SDK/bte/port_rfc.c` |

## Issue 533: RFCOMM and SDP diagnostic boundaries

| Address | Name | Evidence and confidence |
| --- | --- | --- |
| `0x803E55B8` | `sRfcPortClosedMessage` | Sixteen-byte `rfc_port_closed` diagnostic, separated from the preceding timer diagnostic. High boundary confidence; descriptive variable name. [F-533-2](findings/533-sdp-api-db-discovery.md#f-533-2-complete-block-acceptance-after-the-rfcomm-merge). |
| `0x803E5718` | `sSdpWrongAttributeTypeMessage` | Distinct SDP diagnostic established by literal bytes and source/target references. High boundary confidence; descriptive variable name. [F-533-2](findings/533-sdp-api-db-discovery.md#f-533-2-complete-block-acceptance-after-the-rfcomm-merge). |
| `0x803E5740` | `sSdpBadAttributeLengthMessage` | Distinct SDP diagnostic established by literal bytes and source/target references. High boundary confidence; descriptive variable name. [F-533-2](findings/533-sdp-api-db-discovery.md#f-533-2-complete-block-acceptance-after-the-rfcomm-merge). |
| `0x803E5760` | `sSdpDatabaseFullMessage` | Distinct SDP diagnostic established by literal bytes and source/target references. High boundary confidence; descriptive variable name. [F-533-2](findings/533-sdp-api-db-discovery.md#f-533-2-complete-block-acceptance-after-the-rfcomm-merge). |
| `0x803E5770` | `sSdpBadAttributeTypeOrLengthMessage` | Distinct SDP diagnostic established by literal bytes and source/target references. High boundary confidence; descriptive variable name. [F-533-2](findings/533-sdp-api-db-discovery.md#f-533-2-complete-block-acceptance-after-the-rfcomm-merge). |
| `0x803E57A0` | `sSdpAttributeNestingMessage` | Distinct SDP diagnostic established by literal bytes and source/target references. High boundary confidence; descriptive variable name. [F-533-2](findings/533-sdp-api-db-discovery.md#f-533-2-complete-block-acceptance-after-the-rfcomm-merge). |
| `0x803E57BC` | `sSdpBadUuidLengthMessage` | Distinct SDP diagnostic established by literal bytes and source/target references. High boundary confidence; descriptive variable name. [F-533-2](findings/533-sdp-api-db-discovery.md#f-533-2-complete-block-acceptance-after-the-rfcomm-merge). |
| `0x803E57DC` | `sSdpBadBooleanLengthMessage` | Distinct SDP diagnostic established by literal bytes and source/target references. High boundary confidence; descriptive variable name. [F-533-2](findings/533-sdp-api-db-discovery.md#f-533-2-complete-block-acceptance-after-the-rfcomm-merge). |
## FA string comparison (#351)

[F-351-1](findings/351-fa-pf-str.md) names `PFSTR_StrNCmp` (`.text:0x801FB9D0`, `0x118` bytes) and `PFSTR_ToUpperNStr` (`.text:0x801FBAF0`, `0xEC` bytes), the last two functions of `pf_str.c`. Like the rest of #350 they are the VF names without the `VFi` prefix, with medium confidence. `PFSTR_StrNCmp` takes `(p_str, s, target, offset, num)`. Target 3 selects `p_mark` and forces the narrow-string comparison.

## HBM layout unit (#294)

For `wpse-v1` / `main.dol`, [F-294-2](findings/294-nw4hbm-lyt.md#f-294-2-lyt_layoutcpp) names the 15 functions of `lyt_layout.cpp` (`0x80188940`–`0x801895DC`): the anonymous-namespace `SetTagProcessorImpl`, and `nw4hbm::lyt::Layout`'s constructor, destructor, `Build`, `CreateAnimTransform`, `BindAnimation`, `UnbindAnimation`, `UnbindAllAnimation`, `SetAnimationEnable`, `CalculateMtx`, `Draw`, `Animate`, `GetLayoutRect`, `SetTagProcessor` and `BuildPaneObj`. The data names are `__vt__Q36nw4hbm3lyt6Layout` (`0x803BB770`), `__RTTI__Q36nw4hbm3lyt6Layout` (`0x803BB7B8`), `mspAllocator__Q36nw4hbm3lyt6Layout` (`.bss` `0x803E9BF0`, 4 bytes; dtk guessed 8) and `typeInfo__Q36nw4hbm3lyt7TextBox` (`.bss` `0x803E9C08`). The callees named for linking are `Pane::AppendChild` (`0x8018CAB0`) and the resource constructors of `Picture` (`0x8018D840`), `TextBox` (`0x8018DE00`) and `Window` (`0x8018F380`). `BuildPaneObj`'s cases call these constructors in source order, after `Pane` and before `Bounding`. Confidence high: the names and sizes match xenoblade's symbols, and the unit matches.
For `wpse-v1` / `main.dol`, [F-416-1](findings/416-snd-axmanager.md) names the 23 `.text` functions of `snd_AxManager.cpp` from `0x80271E10` to `0x80273650`: the `nw4r::snd::detail::AxManager` constructor, the weak `LinkList<FxBase, 4>` constructor and destructor, `GetInstance`, the implicit destructor, `Init`, `Shutdown`, `GetOutputVolume`, `Update`, `GetZeroBufferAddress`, `RegisterCallback`, `UnregisterCallback`, `SetOutputMode`, `GetOutputMode`, `SetMasterVolume`, `AxCallbackFunc`, `AppendEffect`, `ClearEffect`, `ShutdownEffect`, `AuxCallbackFunc`, `PrepareReset`, `AiDmaCallbackFunc`, and the static initializer; the statics `sZeroBuffer` (`0x8044CF80`), `sBiquadFilterCallbackTable` (`0x8044D080`), the local `instance` (`0x8044D290`, 0x100 bytes), its guard (`0x804B9334`), `finishedFlag` (`0x804B9335`), and the five preset filters `sBiquadFilterLpf` to `sBiquadFilterBpf2048` (`0x804B9320`–`0x804B9333`); and outside the unit `VoiceManager::GetInstance` (`0x80290420`), `VoiceManager::UpdateAllVoicesSync` (`0x80290B40`), `AXSetMaxDspCycles` (`0x802DD780`), and the five `BiquadFilter*` vtables (`0x803C61F8`–`0x803C6247`, Bpf2048 first, Lpf last). `AxManager` is 0x100 bytes: `mOutputMode` 0x0, `mZeroBufferAddress` 0x4, `mCallbackList` 0x8, `mNextAxRegisterCallback` 0x14, `mInitialized` 0x18, `mUpdateVoicePrioFlag` 0x19, `mMasterVolume` 0x1C, `mMainOutVolume` 0x2C, `mVolumeForReset` 0x3C, `mOldAidCallback` 0x4C, `mResetReadyCounter` 0x50, `mAuxFadeVolume` 0x54, `mAuxUserVolume` 0x84, `mFxList` 0xB4, `mAuxCallback` 0xD8, `mAuxCallbackContext` 0xE4, `mAuxCallbackWaitCounter` 0xF0, `mEffectProcessTick` 0xF4. High confidence from the CC0 ogws and zeldaret/ss references and exact matches.
For `wpse-v1` / `main.dol`, [F-416-1](findings/416-snd-axmanager.md) names the 23 `.text` functions of `snd_AxManager.cpp` from `0x80271E10` to `0x80273650`: the `nw4r::snd::detail::AxManager` constructor, the weak `LinkList<FxBase, 4>` constructor and destructor, `GetInstance`, the implicit destructor, `Init`, `Shutdown`, `GetOutputVolume`, `Update`, `GetZeroBufferAddress`, `RegisterCallback`, `UnregisterCallback`, `SetOutputMode`, `GetOutputMode`, `SetMasterVolume`, `AxCallbackFunc`, `AppendEffect`, `ClearEffect`, `ShutdownEffect`, `AuxCallbackFunc`, `PrepareReset`, `AiDmaCallbackFunc`, and the static initializer; the statics `sZeroBuffer` (`0x8044CF80`), `sBiquadFilterCallbackTable` (`0x8044D080`), the local `instance` (`0x8044D290`, 0x100 bytes), its guard (`0x804B9334`), `finishedFlag` (`0x804B9335`), and the five preset filters `sBiquadFilterLpf` to `sBiquadFilterBpf2048` (`0x804B9320`–`0x804B9333`); and outside the unit `VoiceManager::GetInstance` (`0x80290420`), `VoiceManager::UpdateAllVoicesSync` (`0x80290B40`), and the five `BiquadFilter*` vtables (`0x803C61F8`–`0x803C6247`, Bpf2048 first, Lpf last). `AxManager` is 0x100 bytes: `mOutputMode` 0x0, `mZeroBufferAddress` 0x4, `mCallbackList` 0x8, `mNextAxRegisterCallback` 0x14, `mInitialized` 0x18, `mUpdateVoicePrioFlag` 0x19, `mMasterVolume` 0x1C, `mMainOutVolume` 0x2C, `mVolumeForReset` 0x3C, `mOldAidCallback` 0x4C, `mResetReadyCounter` 0x50, `mAuxFadeVolume` 0x54, `mAuxUserVolume` 0x84, `mFxList` 0xB4, `mAuxCallback` 0xD8, `mAuxCallbackContext` 0xE4, `mAuxCallbackWaitCounter` 0xF0, `mEffectProcessTick` 0xF4. High confidence from the CC0 ogws and zeldaret/ss references and exact matches.
## Issue 511: complete BTU and BTA system units

| Address | Name | Evidence and confidence |
| --- | --- | --- |
| `0x80313E70` | `btu_task_init` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x80313EB0` | `btu_task_msg_handler` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x80314230` | `btu_start_timer` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x803142A0` | `btu_stop_timer` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x803142B4` | `bdcpy` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x803142E8` | `bdcmp` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x803145DC` | `bta_sys_init` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x80314640` | `bta_sys_event` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x8031472C` | `bta_sys_timer_update` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x8031474C` | `bta_sys_register` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x80314760` | `bta_sys_sendmsg` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x8031477C` | `bta_sys_start_timer` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x8031479C` | `bta_sys_stop_timer` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x803147B0` | `bta_sys_disable` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x80314818` | `bta_sys_set_trace_level` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x80314820` | `ptim_init` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x8031486C` | `ptim_timer_update` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x80314920` | `ptim_start_timer` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x803149C0` | `ptim_stop_timer` | Original reference name, exact compiled instructions and final linked target; high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x804B8EF0` | `btu_count` | Target accesses and complete source object establish `0x1` bytes in `.sdata`; original reference name, high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x804B8EF4` | `execute_btu` | Target accesses and complete source object establish `0x4` bytes in `.sdata`; original reference name, high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x804B9B88` | `_btu_g_count` | Target accesses and complete source object establish `0x4` bytes in `.sbss`; original reference name, high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x804B9B8C` | `_btu_last_timer_tick` | Target accesses and complete source object establish `0x4` bytes in `.sbss`; original reference name, high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x804AA850` | `btu_cb` | Target accesses and complete source object establish `0x84` bytes in `.bss`; original reference name, high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x804AA8D8` | `bta_sys_cb` | Target accesses and complete source object establish `0x8C` bytes in `.bss`; original reference name, high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x804B9B90` | `appl_trace_level` | Target accesses and complete source object establish `0x1` bytes in `.sbss`; original reference name, high confidence. [F-511-4](findings/511-bta-system-completion.md). |
| `0x804B8ED8` | `p_bta_sys_cfg` | Reference configuration pointer, identified from initialization and mailbox accesses. Name only; owner retains its extent and type. High confidence. [F-511-4](findings/511-bta-system-completion.md). |

## NW4R ut ArchiveFontBase and ArchiveFont (#449, #450)

For `wpse-v1` / `main.dol`, [F-449-2](findings/449-ut-resfont-archivefont.md) names the 25 `.text` functions of `ut_ArchiveFontBase.cpp` (`0x80295890–0x80296DC8`) in `nw4r::ut::detail::ArchiveFontBase` and its nested `CachedStreamReader`, the eight functions of `ut_ArchiveFont.cpp` (`0x80296DD0–0x80297710`) in `nw4r::ut::ArchiveFont`, and the vtables `__vt__Q44nw4r2ut6detail15ArchiveFontBase` (`.data` `0x803C6BB8`, 0x60 bytes) and `__vt__Q34nw4r2ut11ArchiveFont` (`.data` `0x803C6C50`, 0x60 bytes). Names are high confidence from the CC0 forecast and Wii News Channel symbol maps and matching code. Outside callees: `CntBit1__Q24nw4r4mathFUl` (`0x802A34A0`), `strchr` (`0x80345F38`), `strstr` (`0x80345F68`). `ArchiveFontBase` layout: `ResFontBase` (0x1C bytes) followed by `mGlyphIndexAdjustArray` (`u16*`) at 0x1C; `ArchiveFont` adds no members. `ConstructContext` is 0x68 bytes with the forecast field layout; `CachedStreamReader` is 0x1C bytes.
## BTE L2CAP main and utilities (#527)

`l2c_init` was named earlier and now links from `src/RVL_SDK/bte/l2c_main.c`.

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x804ADE40` | `0x7E8` | `lbl_804ADE40` | `l2cb` | object (tL2C_CB) | high | F-527-1 | `src/RVL_SDK/bte/l2c_main.c` |
| `0x8032F8CC` | `0x330` | `fn_8032F8CC` | `l2c_rcv_acl_data` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_main.c` |
| `0x8032FBFC` | `0xA70` | `fn_8032FBFC` | `process_l2cap_cmd` | static function | high | F-527-1 | `src/RVL_SDK/bte/l2c_main.c` |
| `0x8033066C` | `0x50` | `fn_8033066C` | `l2c_process_timeout` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_main.c` |
| `0x803306BC` | `0x12C` | `fn_803306BC` | `l2c_process_held_packets` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_main.c` |
| `0x803307E8` | `0xD4` | `fn_803307E8` | `l2cu_allocate_lcb` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x803308BC` | `0x100` | `fn_803308BC` | `l2cu_release_lcb` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x803309BC` | `0x88` | `fn_803309BC` | `l2cu_find_lcb_by_bd_addr` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80330A44` | `0x90` | `fn_80330A44` | `l2cu_find_lcb_by_handle` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80330AD4` | `0x10` | `fn_80330AD4` | `l2cu_get_conn_role` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80330AE4` | `0xC4` | `fn_80330AE4` | `l2cu_build_header` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80330BA8` | `0xFC` | `fn_80330BA8` | `l2cu_send_peer_cmd_reject` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80330CA4` | `0xC4` | `fn_80330CA4` | `l2cu_send_peer_connect_req` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80330D68` | `0xD8` | `fn_80330D68` | `l2cu_send_peer_connect_rsp` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80330E40` | `0xC0` | `fn_80330E40` | `l2cu_reject_connection` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80330F00` | `0x2F8` | `fn_80330F00` | `l2cu_send_peer_config_req` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x803311F8` | `0x2F8` | `fn_803311F8` | `l2cu_send_peer_config_rsp` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x803314F0` | `0x1B4` | `fn_803314F0` | `l2cu_send_peer_config_rej` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x803316A4` | `0xD4` | `fn_803316A4` | `l2cu_send_peer_disc_req` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80331778` | `0xC0` | `fn_80331778` | `l2cu_send_peer_disc_rsp` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80331838` | `0x170` | `fn_80331838` | `l2cu_send_peer_echo_req` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x803319A8` | `0x19C` | `fn_803319A8` | `l2cu_send_peer_echo_rsp` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80331B44` | `0xA4` | `fn_80331B44` | `l2cu_send_peer_info_rsp` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80331BE8` | `0x10C` | `fn_80331BE8` | `l2cu_allocate_ccb` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80331CF4` | `0x1A4` | `fn_80331CF4` | `l2cu_release_ccb` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80331E98` | `0x5C` | `fn_80331E98` | `l2cu_find_ccb_by_cid` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80331EF4` | `0x4C` | `fn_80331EF4` | `l2cu_allocate_rcb` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80331F40` | `0x10` | `fn_80331F40` | `l2cu_release_rcb` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80331F50` | `0xB8` | `fn_80331F50` | `l2cu_find_rcb_by_psm` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80332008` | `0x148` | `fn_80332008` | `l2cu_process_peer_cfg_req` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80332150` | `0x40` | `fn_80332150` | `l2cu_process_peer_cfg_rsp` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80332190` | `0x110` | `fn_80332190` | `l2cu_process_our_cfg_req` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x803322A0` | `0x40` | `fn_803322A0` | `l2cu_process_our_cfg_rsp` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x803322E0` | `0x6C` | `fn_803322E0` | `l2cu_device_reset` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x8033234C` | `0x118` | `fn_8033234C` | `l2cu_create_conn` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80332464` | `0xD0` | `fn_80332464` | `l2cu_create_conn_after_switch` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x80332534` | `0x90` | `fn_80332534` | `l2cu_find_lcb_by_state` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x803325C4` | `0xE8` | `fn_803325C4` | `l2cu_lcb_disconnecting` | function | high | F-527-1 | `src/RVL_SDK/bte/l2c_utils.c` |
| `0x803118B0` | see `symbols.txt` | `fn_803118B0` | `GKI_remove_from_queue` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x80311980` | see `symbols.txt` | `fn_80311980` | `GKI_getfirst` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x80311990` | see `symbols.txt` | `fn_80311990` | `GKI_getnext` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x8031A534` | see `symbols.txt` | `fn_8031A534` | `btm_acl_removed` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x8031A65C` | see `symbols.txt` | `fn_8031A65C` | `BTM_SwitchRole` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x8031D6B4` | see `symbols.txt` | `fn_8031D6B4` | `BTM_ReadLocalFeatures` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x8031EA7C` | see `symbols.txt` | `fn_8031EA7C` | `BTM_InqDbRead` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x80320FB8` | see `symbols.txt` | `fn_80320FB8` | `btm_remove_sco_links` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x80321584` | see `symbols.txt` | `fn_80321584` | `btm_is_sco_active_by_bdaddr` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x8032320C` | see `symbols.txt` | `fn_8032320C` | `btm_sec_is_bonding` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x803236CC` | see `symbols.txt` | `fn_803236CC` | `btm_sec_disconnect` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x8032D150` | see `symbols.txt` | `fn_8032D150` | `l2c_csm_execute` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x8032EB98` | see `symbols.txt` | `fn_8032EB98` | `l2c_link_timeout` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x8032EE14` | see `symbols.txt` | `fn_8032EE14` | `l2c_link_check_send_pkts` | function | high | F-527-1 | Not reconstructed (fallback) |
| `0x8032F0E0` | see `symbols.txt` | `fn_8032F0E0` | `l2c_link_adjust_allocation` | function | high | F-527-1 | Not reconstructed (fallback) |

## NW4R ut PairFont (#450)

For `wpse-v1` / `main.dol`, [F-450-1](findings/450-ut-pairfont-charwriter.md) names the 23 `.text` functions of `ut_PairFont.cpp` (`0x80297710–0x802983C4`) in `nw4r::ut::PairFont`, including `GetBaselinePos__Q34nw4r2ut8PairFontCFUs` (`0x80297A70`), and `__vt__Q34nw4r2ut8PairFont` (`.data` `0x803C6CC0`, 0x60 bytes). Class and function names follow the Whippy reconstruction and the `Font` interface; high confidence except the `GetBaselinePos(u16)` slot (low). Layout: `mFirst` (`Font*`) at 0x10, `mSecond` (`Font*`) at 0x14, `mAlternateWithFirst` (`bool`) at 0x18; member names medium confidence.
For `wpse-v1` / `main.dol`, [F-417-1](findings/417-snd-axvoice.md) names the 35 `.text` functions of `snd_AxVoice.cpp` from `0x80273650` to `0x80276404`: the `nw4r::snd::detail::AxVoice` constructor and destructor, `Setup`, `IsPlayFinished`, `SetLoopStart`, `SetLoopEnd`, `SetLoopFlag`, `StopAtPoint`, `IsDataAddressCoverd` (original spelling), `GetCurrentPlayingSample`, `VoiceCallback`, `GetSampleByDspAddress`, `SetPriority`, `SetVoiceType`, `EnableRemote`, `ResetDelta`, `SetAddr`, `SetSrcType`, `SetAdpcm`, `IsNeedNextUpdate`, `SetAdpcmLoop`, `SetMix`, `SetRmtMix`, `SetSrc`, `SetVe`, `SetLpf`, `SetBiquad`, `SetRemoteFilter`, and `CalcOffsetAdpcmParam`, then `AxVoiceParamBlock::Sync`, `Set`, `SetVoiceMix`, `SetVoiceSrcType`, `SetVoiceRmtMix`, and `SetVoiceRmtIIRCoefs`; and outside the unit `AxVoiceManager::GetInstance` (`0x80276410`), `AxVoiceManager::ReserveForFreeAxVoice` (`0x80276C30`), `Util::GetRemoteFilterCoefs` (`0x80290FE0`), and `DecodeDspAdpcm` (`0x80292C68`). `AxVoiceParamBlock` is 0x10 bytes: `mVpb` 0x0, `mSync` 0x4, `mPrevVeSetting` 0x8, `mFirstVeUpdateFlag` 0xC, `mVolume` 0xE. `AxVoice` is 0x48 bytes: `mVpb` 0x0, `mWaveData` 0x10, `mFormat` 0x14, `mSampleRate` 0x18, `mFirstMixUpdateFlag` 0x1C, `mReserveForFreeFlag` 0x1D, `mMixPrev` 0x1E, `mCallback` 0x38, `mCallbackData` 0x3C, `node` 0x40. High confidence from the CC0 ogws and zeldaret/ss references and exact matches.

## HBM math and ut units (#299)

For `wpse-v1` / `main.dol`, [F-299-1](findings/299-nw4hbm-ut.md#f-299-1-math_triangular-ut_binaryfileformat-ut_charstrmreader-and-ut_charwriter) names 34 functions at `0x80191880`–`0x801936CC` with their mangled CodeWarrior names. They are `nw4hbm::math::SinFIdx`, `CosFIdx` and `Atan2FIdx`, `nw4hbm::ut::IsValidBinaryFile`, the four `CharStrmReader::ReadNextChar*` readers, and 26 `nw4hbm::ut::CharWriter` members from the constructor (`0x80191CD0`) to `SetupGXWithColorMapping` (`0x801933D0`). The data names are `sSinCosTbl__Q36nw4hbm4math29@unnamed@math_triangular_cpp@` (`0x803BBB00`, 0x1010 bytes), `sArcTanTbl__Q36nw4hbm4math29@unnamed@math_triangular_cpp@` (`0x803BCB10`, 0x108 bytes) and `mLoadingTexture__Q36nw4hbm2ut10CharWriter` (`.bss` `0x803E9C20`, 0x10 bytes). `lbl_80366F04` shrinks to 0x20 bytes because its last word is alignment padding. Confidence high: the names and sizes match xenoblade's symbols, and the units match.
For `wpse-v1` / `main.dol`, [F-418-1](findings/418-snd-axvoicemanager-axfximpl.md) names the nine `.text` functions of `snd_AxVoiceManager.cpp` from `0x80276410` to `0x80276E38` (`nw4r::snd::detail::AxVoiceManager::GetInstance`, the implicit destructor, `GetRequiredMemSize`, `Setup`, `Shutdown`, `AcquireAxVoice`, `FreeAxVoice`, `ReserveForFreeAxVoice`, `FreeAllReservedAxVoice`), its local `instance` (`0x8044D39C`, 0x2C bytes) and guard (`0x804B9338`), and the six functions of `snd_AxfxImpl.cpp` from `0x80276E40` to `0x80277038` (`AxfxImpl::CreateHeap`, `DestroyHeap`, `HookAlloc`, `RestoreAlloc`, `Alloc`, `Free`) with the statics `mCurrentFx` (`0x804B9340`) and `mAllocatedSize` (`0x804B9344`). `AxVoiceManager` is 0x2C bytes: `mActiveVoiceList` 0x0, `mFreeVoiceList` 0xC, `mFreeReservedVoiceList` 0x18, `mInitialized` 0x24, `mVoiceCount` 0x28. `AxfxImpl` is 8 bytes: `mHeap` 0x0, `mAllocCount` 0x4. High confidence from the CC0 ogws and zeldaret/ss references and exact matches.

For `wpse-v1` / `main.dol`, [F-419-1](findings/419-snd-bank-basicplayer.md) names the three functions of `snd_Bank.cpp` from `0x80277040` to `0x802772B8` (`nw4r::snd::detail::Bank::Bank`, `~Bank`, `NoteOn`), the five functions of `snd_BankFile.cpp` from `0x802772C0` to `0x80277980` (`BankFileReader::BankFileReader`, `GetInstParam`, `ReadInstInfo`, `GetReferenceToSubRegion`, `ReadWaveInfo`), and the ten functions of `snd_BasicPlayer.cpp` from `0x80277980` to `0x80277BC0` (`PlayerParamSet::Init`, `BasicPlayer::BasicPlayer`, `InitParam`, `SetFxSend`, `GetFxSend`, `SetBiquadFilter`, `SetRemoteFilter`, `SetRemoteOutVolume`, `GetRemoteOutVolume`, the weak destructor) with the vtable `__vt__Q44nw4r3snd6detail11BasicPlayer` (`0x803C6198`). It also names the referenced `Util::GetDataRefAddressImpl` (`0x80291070`), `WaveArchiveReader::WaveArchiveReader` (`0x802910A0`) and `GetWaveFile` (`0x80291120`), the two `WaveFileReader` constructors (`0x802911B0`, `0x802911D0`), `WaveFileReader::ReadWaveInfo` (`0x802911E0`), `Channel::AllocChannel` (`0x8027A6E0`) and `Start` (`0x8027A2B0`), and `EnvGenerator::SetAttack`, `SetHold`, `SetDecay`, `SetSustain`, `SetRelease` (`0x8027AF40` to `0x8027B030`, identified by call order). `Bank` is 0x10 bytes: `mBankReader` 0x0 (0xC), `mWaveDataAddress` 0xC. `InstInfo` is 0x1C bytes and `WaveInfo` 0x80 bytes. `BasicPlayer` is 0xB4 bytes: vtable 0x0, `mPlayerParamSet` 0x4 (0xAC), `mId` 0xB0. `Channel` offsets written by `NoteOn` are listed in the finding; its remaining layout is provisional. `WaveDataLocationCallback::GetWaveInfo` and `ReleaseWaveInfo` are descriptive names with low confidence; all other names have high confidence from the CC0 ogws and zeldaret/ss references and exact matches.

For `wpse-v1` / `main.dol`, [F-419-2](findings/419-snd-bank-basicplayer.md#f-419-2-nw4r-snd-basicsound-unit) names the 36 functions of `snd_BasicSound.cpp` from `0x80277BC0` to `0x80279300`: the `nw4r::snd::detail::BasicSound` constructor, `InitParam`, `StartPrepared`, `Stop`, `Pause`, `FadeIn`, `Update`, `UpdateMoveValue`, `UpdateParam`, `Shutdown`, the eight attach and detach functions, `GetVoiceOutCount`, `SetPlayerPriority`, `OnUpdatePlayerPriority`, `SetInitialVolume`, `SetVolume`, `SetRemoteFilter`, `SetPanMode`, `SetPanCurve`, `SetAmbientInfo`, `GetAmbientPriority` (0x38 bytes including its trailing `blr`), the handle checks and detaches, `SetId`, the weak destructor, and `GetRuntimeTypeInfo`. It also names the static initialiser `__sinit_\snd_BasicSound_cpp` (`0x80279300`), the vtable `__vt__Q44nw4r3snd6detail10BasicSound` (`0x803C61C0`), and `typeInfo` (`0x804B9348`). Outside the unit it names `SoundHandle::DetachSound` (`0x80287D10`), `SoundPlayer::GetRemoteOutVolume` (`0x80288060`), `detail_SortPriorityList(BasicSound*)` (`0x80288070`), `detail_RemoveSound` (`0x80288500`), `detail_FreePlayerHeap` (`0x80288830`), and `ExternalSoundPlayer::RemoveSound` (`0x8027B4C0`). `BasicSound` is 0x110 bytes; the offsets are in the finding. `SoundParam::userData` and `AmbientArgUpdateCallback::detail_UpdateAmbientArg` are descriptive names with low confidence. All other names have high confidence from the CC0 ogws and zeldaret/ss references and exact matches.

For `wpse-v1` / `main.dol`, [F-421-1](findings/421-snd-biquad-channel-envgenerator.md) names the functions of six snd units from `0x80279310` to `0x8027B5D8`: `GetCoef` and the weak destructors of `nw4r::snd::detail::BiquadFilterLpf`, `Hpf`, `Bpf512`, `Bpf1024`, `Bpf2048` with their `coefTable` arrays (`0x8036C9D8` to `0x8036DA6C`); `ChannelManager::GetInstance`, its destructor, `GetRequiredMemSize`, `Setup`, `Shutdown`, `UpdateAllChannel`, and `Channel::InitParam`, `Update`, `Release`, `NoteOff`, `Stop`, `UpdateSweep`, `SetSweepParam`, `SetBiquadFilter`, `VoiceCallbackFunc`, `FreeChannel`, with the local `instance` (`0x8044D3D8`, 0x20 bytes) and guard (`0x804B9350`); `DisposeCallbackManager::GetInstance`, its destructor, `RegisterDisposeCallback`, `UnregisterDisposeCallback`, `Dispose`, `DisposeWave`, with the local `instance` (`0x8044D404`) and guard (`0x804B9358`); `nw4r::snd::Debug_SetWarningFlag` (`0x8027AC80`); `EnvGenerator::Init`, `Reset`, `GetValue`, `Update`, the constructor, `DecibelSquareTable` (`0x8036DE10`), the local `attackTable` (`0x8036DF10`), and `VOLUME_INIT` (`0x804BAE38`); and the `ExternalSoundPlayer` constructor, destructor, `StopAllSound`, `DetachSoundActorAll`, `AppendSound`, `SetPlayableSoundCount`, and `detail_CanPlaySound`. Outside these units it names `PoolImpl::CreateImpl`, `DestroyImpl`, `AllocImpl`, `FreeImpl`, `LfoParam::Init`, `Lfo::Reset`, `Update`, `GetValue`, `Util::CalcPitchRatio`, `CalcVolumeRatio`, `VoiceManager::AllocVoice`, `SoundThread::GetInstance`, and twenty `Voice` functions; addresses are in the finding. `ChannelManager` is 0x20 bytes: `mPool` 0x0, `mChannelList` 0x4, `mInitialized` 0x10, `mChannelCount` 0x14, `mMem` 0x18, `mMemSize` 0x1C. `Channel` is 0xE0 bytes: `mEnvelope` 0x0, `mLfo` 0x1C, `mLfoTarget` 0x34, flags 0x35 to 0x3C, user parameters 0x40 to 0x7C, sweep 0x80 to 0x8C, initial parameters 0x90 to 0x9C, `mSilenceVolume` 0xA0, `mKey` 0xA8, `mOriginalKey` 0xAC, `mLength` 0xB0, `mPanMode` 0xB4, `mPanCurve` 0xB8, `mAlternateAssign` 0xBC, `mCallback` 0xC0, `mCallbackData` 0xC4, `mWaveDataLocationCallback` 0xC8, `mWaveInfo` 0xCC, `mVoice` 0xD0, `mNextLink` 0xD4, `node` 0xD8. `EnvGenerator` is 0x1C bytes. `ExternalSoundPlayer` is 0x10 bytes: `mSoundList` 0x0, `mPlayableCount` 0xC. `SoundThread::mPauseFlag` (0x395) is a descriptive name with low confidence; all other names have high confidence from the CC0 ogws and zeldaret/ss references and exact matches.

For `wpse-v1` / `main.dol`, [F-423-1](findings/423-snd-fx.md) names `nw4r::snd::FxBase::OnChangeOutputMode` (`0x8027B5E0`) and, for each of `FxChorus` (`0x8027B5F0`), `FxDelay` (`0x8027BA10`), and `FxReverbStd` (`0x8027BE70`), the constructor, `GetRequiredMemSize`, `AssignWorkBuffer`, `ReleaseWorkBuffer`, `StartUp`, `Shutdown`, `SetParam`, and `UpdateBuffer`, with the vtables `0x803C6248`, `0x803C6270`, and `0x803C6298` and the game-side destructors `0x801E7680`, `0x801E7700`, and `0x801E7580`. Each effect has `mIsActive` at 0xC, `mImpl` at 0x10, `mParam` at 0x18, and `mAxfxParam` at 0x2C (chorus, delay) or 0x40 (standard reverb). `FxChorusParam` (0x14 bytes: `delayTime`, `depth`, `rate`, `feedback`, `outGain`), `FxDelayParam` (0x14 bytes: `delay`, `feedback`, `outGain`, `maxDelay`, `lpf`), and `FxReverbStdParam` (0x28 bytes: `preDelayTime`, `fusedTime`, `coloration`, `damping`, `outGain`, `earlyMode`, `preDelayTimeMax`, `fusedMode`, `earlyGain`, `fusedGain`) have type names from riptl/wii-symbols `SUPE01` and descriptive field names with medium confidence, taken from the AXFX state members they are clamped into. Function and class names have high confidence.

For `wpse-v1` / `main.dol`, [F-424-1](findings/424-snd-instancepool-lfo.md) names, with high confidence from the CC0 zeldaret/ss source and exact code, `nw4r::snd::detail::PoolImpl::CreateImpl` (`0x8027C3C0`), `DestroyImpl`, `CountImpl` (`0x8027C5A0`), `AllocImpl`, `FreeImpl`, and `LfoParam::Init` (`0x8027C6B0`), `Lfo::Reset`, `Lfo::Update`, `Lfo::GetValue`, `Lfo::GetSinIdx`, with the function-local `sinTable` (`.rodata` `0x8036E110`, 33 bytes).

For `wpse-v1` / `main.dol`, [F-424-2](findings/424-snd-instancepool-lfo.md#f-424-2-nw4r-snd-memorysoundarchive-unit) names, with high confidence from the CC0 zeldaret/ss source and exact code, `nw4r::snd::MemorySoundArchive`'s constructor (`0x8027C8D0`), destructor, `Setup`, `detail_GetFileAddress`, `detail_GetWaveDataFileAddress`, `OpenStream`, `OpenExtStream`, and `detail_GetRequiredStreamBufferSize` (`0x8027CBD0`); `MemorySoundArchive::MemoryFileStream::Close` (`0x8027CBE0`), `Read`, `Seek` (`0x8027CC60`, 0x48 bytes including an unreachable `blr`), and its weak `GetSize`, `Tell`, `CanWrite`, `CanRead`, `CanAsync`, `CanCancel`, `CanSeek`, and destructor (`0x8027CCC0–0x8027CD6C`); the weak `ut::FileStream::GetRuntimeTypeInfo` (`0x8027CCB0`) and `ut::IOStream::GetOffsetAlign` (`0x8027C8C0`); and the vtables of `MemoryFileStream` (`.data` `0x803C62C0`, 0x60) and `MemorySoundArchive` (`0x803C6320`, 0x20). The `MemoryFileStream` vtable identifies `ut::IOStream::GetSizeAlign` and `GetBufferAlign` as the game-side weak copies at `0x801A4DD0` and `0x801A4DE0`. Outside the unit, by call signature and code: `SoundArchive`'s constructor (`0x80283E10`), destructor (`0x80283E40`), `Setup` (`0x80283EA0`), `detail_ReadGroupInfo` (`0x80283F80`), `detail_ReadGroupItemInfo` (`0x80283F90`), and `detail_ReadFilePos` (`0x80283FC0`); `detail::SoundArchiveFileReader`'s constructor (`0x80284200`), `Init` (`0x80284230`), `SetStringChunk` (`0x802842E0`), and `SetInfoChunk` (`0x80284390`). `SoundArchive` is 0x108 bytes (`mFileReader` 0x4, `mExtFileRoot` 0x8, 0x100 bytes); `SoundArchiveFileReader` is 0x44 bytes (header 0x0, `mInfo` 0x28, `mStringBase` 0x2C, string table and trees 0x30–0x40); `MemorySoundArchive` adds `mData` at 0x108 and the reader at 0x10C; `MemoryFileStream` adds `mBuffer` 0x14, `mSize` 0x18, `mPosition` 0x1C.

For `wpse-v1` / `main.dol`, [F-425-1](findings/425-snd-midiseqplayer-mmlparser.md) assigns the weak `nw4r::snd::detail::SeqPlayer` functions at `0x8027CD70–0x8027CE18` (`OnUpdateFrameSoundThread`, `OnShutdownSoundThread`, `InvalidateWaveData`, `SoundThread::PlayerCallback::OnUpdateVoiceSoundThread`, `IsPause`, `IsStarted`, `IsActive`, and the `@180@`/`@192@` thunks) to `snd_MidiSeqPlayer.cpp`, and names the `MmlParser` vtable (`.data` `0x803C6560`, 0x10) and `MmlParser::mPrintVarEnabledFlag` (`.sbss` `0x804B9360`, 1 byte). The function names come from the reference symbol import and are confirmed by exact code. `SeqPlayer` flags: `mActiveFlag` 0xCC, `mStartedFlag` 0xCD, `mPauseFlag` 0xCE; the `DisposeCallback` base is at 0xB4 and the `PlayerCallback` base at 0xC0. High confidence.

For `wpse-v1` / `main.dol`, [F-794-1](findings/794-snd-remotespeakermanager-seqfile-seqplayer.md) names `nw4r::snd::detail::SeqPlayer::SetChannelPriority` (`0x8027FCC0`), `SetReleasePriorityFix` (`0x8027FCD0`), the `@180@`/`@192@` destructor thunks (`0x80280740`, `0x80280750`), the `SeqPlayer` vtable (`.data` `0x803C65A8`, 0x68 with padding), `SeqPlayer::mGlobalVariable` (`.bss` `0x8044D6E8`, 0x20), `nw4r::snd::RemoteSpeaker`'s constructor (`0x8027E4D0`), and `RemoteSpeakerManager::GetInstance`'s guard (`.sbss` `0x804B9368`) and local instance (`.bss` `0x8044D410`, 0x2D8). The other functions of `snd_RemoteSpeakerManager.cpp`, `snd_SeqFile.cpp`, and `snd_SeqPlayer.cpp` already carry names from the reference symbol import, confirmed by exact code. `SeqPlayer` layout: `mParserParam` at 0xE8 (`priority` 0xE9), `mReleasePriorityFixFlag` 0xCF. `SeqFile::Header`: data-block offset 0x10, label-block offset 0x18; `SeqFile::DataBlock::baseOffset` 0x8. High confidence.

## Issue 525: BTE HID host connection and L2CAP API

Names follow the Petari reference and are corroborated by 100% objdiff and full DOL verification ([F-525-1](findings/525-bte-hidh-conn-l2c-api.md)). `hst_reg_info` is the 0x28-byte `tL2CAP_APPL_INFO` at `.rodata 0x80398170`. `btm_get_acl_disc_reason_code` at `0x8031B0E8` (0x10 bytes) returns the `btm_cb` disconnect-reason byte; it is named for the link, and its unit is owned elsewhere.

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x8032A740` | `0x130` | `fn_8032A740` | `hidh_sec_check_complete_term` | function | high | F-525-1 | `src/RVL_SDK/bte/hidh_conn.c` |
| `0x8032A870` | `0x258` | `fn_8032A870` | `hidh_l2cif_connect_ind` | function | high | F-525-1 | `src/RVL_SDK/bte/hidh_conn.c` |
| `0x8032AC14` | `0x214` | `fn_8032AC14` | `hidh_sec_check_complete_orig` | function | high | F-525-1 | `src/RVL_SDK/bte/hidh_conn.c` |
| `0x8032AE28` | `0x348` | `fn_8032AE28` | `hidh_l2cif_connect_cfm` | function | high | F-525-1 | `src/RVL_SDK/bte/hidh_conn.c` |
| `0x8032B170` | `0x2A8` | `fn_8032B170` | `hidh_l2cif_config_ind` | function | high | F-525-1 | `src/RVL_SDK/bte/hidh_conn.c` |
| `0x8032B418` | `0x30C` | `fn_8032B418` | `hidh_l2cif_config_cfm` | function | high | F-525-1 | `src/RVL_SDK/bte/hidh_conn.c` |
| `0x8032B724` | `0x2C4` | `fn_8032B724` | `hidh_l2cif_disconnect_ind` | function | high | F-525-1 | `src/RVL_SDK/bte/hidh_conn.c` |
| `0x8032B9E8` | `0x248` | `fn_8032B9E8` | `hidh_l2cif_disconnect_cfm` | function | high | F-525-1 | `src/RVL_SDK/bte/hidh_conn.c` |
| `0x8032BC30` | `0x1F8` | `fn_8032BC30` | `hidh_l2cif_cong_ind` | function | high | F-525-1 | `src/RVL_SDK/bte/hidh_conn.c` |
| `0x8032BE28` | `0x398` | `fn_8032BE28` | `hidh_l2cif_data_ind` | function | high | F-525-1 | `src/RVL_SDK/bte/hidh_conn.c` |
| `0x8032CF68` | `0xC8` | `fn_8032CF68` | `L2CA_SetIdleTimeout` | function | high | F-525-1 | `src/RVL_SDK/bte/l2c_api.c` |
| `0x8032D030` | `0x100` | `fn_8032D030` | `L2CA_SetIdleTimeoutByBdAddr` | function | high | F-525-1 | `src/RVL_SDK/bte/l2c_api.c` |
| `0x8032D130` | `0x1C` | `fn_8032D130` | `L2CA_SetTraceLevel` | function | high | F-525-1 | `src/RVL_SDK/bte/l2c_api.c` |
| `0x8032D14C` | `0x4` | `fn_8032D14C` | `L2CA_RegisterCompression` | function | high | F-525-1 | `src/RVL_SDK/bte/l2c_api.c` |
## Issue 512: complete device-manager action names and extents

The earlier table now records actual table and array symbol sizes rather than their alignment tails. [F-512-5](findings/512-dm-completion.md) records the complete unit, payload boundaries and full source-linked verification.

| Address | Name | Evidence and confidence |
| --- | --- | --- |
| `0x8031590C` | `bta_dm_queue_disc` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315954` | `bta_dm_search_clear_queue` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315998` | `bta_dm_search_cancel_cmpl` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x803159DC` | `bta_dm_search_cancel_transac_cmpl` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315A38` | `bta_dm_search_cancel_notify` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315A50` | `bta_dm_find_services` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315BF0` | `bta_dm_discover_next_device` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315CD0` | `bta_dm_sdp_callback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315D18` | `bta_dm_inq_results_cb` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315DA8` | `bta_dm_inq_cmpl_cb` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315DF4` | `bta_dm_service_search_remname_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315E34` | `bta_dm_remname_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315EC0` | `bta_dm_disc_remname_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315F48` | `bta_dm_cancel_rmt_name` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80315F4C` | `bta_dm_authorize_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316014` | `bta_dm_pinname_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x803160D4` | `bta_dm_pin_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316204` | `bta_dm_link_key_request_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x8031620C` | `bta_dm_new_link_key_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x803162A8` | `bta_dm_authentication_complete_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x8031632C` | `bta_dm_local_addr_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316354` | `bta_dm_signal_strength` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316390` | `bta_dm_signal_strength_timer_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316484` | `bta_dm_acl_change_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x803164F4` | `bta_dm_acl_change` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316774` | `bta_dm_disable_conn_down_timer_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316790` | `bta_dm_rssi_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x803167FC` | `bta_dm_link_quality_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316868` | `bta_dm_l2cap_server_compress_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x8031694C` | `bta_dm_compress_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316B84` | `bta_dm_rm_cback` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316C8C` | `bta_dm_keep_acl` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316D10` | `bta_dm_immediate_disable` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316D2C` | `bta_dm_reset_complete` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x80316D30` | `bta_dm_send_hci_reset` | Pinned reference, exact instructions and final call/callback relocations; high confidence. [F-512-5](findings/512-dm-completion.md). |
| `0x804BB5F8` | `sServiceAttributeListInitializer` | Four-byte initializer copied into the local attribute list; descriptive name, high boundary confidence. [F-512-5](findings/512-dm-completion.md). |

## HBM ut font and list units (#300)

For `wpse-v1` / `main.dol`, [F-300-1](findings/300-nw4hbm-ut.md#f-300-1-ut_font-ut_linklist-ut_list-and-ut_resfont) names `Font::InitReaderFunc` (`0x801936D0`), `List_Init` (`0x801938E0`), and `ResFont`'s constructor (`0x80193A50`), destructor (`0x80193A90`), `SetResource` (`0x80193AF0`) and `Rebuild` (`0x80193C40`). The data names are `__vt__Q36nw4hbm2ut7ResFont` (`0x803BCC48`), `__RTTI__Q36nw4hbm2ut7ResFont` (`0x803BCCC8`), `__RTTI__Q46nw4hbm2ut6detail11ResFontBase` (`0x803BCD00`) and `__RTTI__Q36nw4hbm2ut4Font` (`0x803BCD20`). The callees named for linking are the `detail::ResFontBase` constructor (`0x80193D50`), destructor (`0x80193DA0`), `SetResourceBuffer` (`0x80193DE0`), and the 19 virtual functions `GetWidth` (`0x80193DF0`) through `GetEncoding` (`0x801941B0`), in vtable order. Confidence high: the names and sizes match xenoblade's symbols, and the units match.
## BTE L2CAP channel state machine and link (#526)

`l2c_csm_execute`, `l2c_link_hci_conn_comp`, `l2c_link_hci_disc_comp`, `l2c_link_hci_qos_violation`, `l2c_link_timeout`, `l2c_link_check_send_pkts`, `l2c_link_adjust_allocation`, `l2c_link_process_num_completed_pkts`, `l2c_link_role_changed`, `l2c_link_role_change_failed`, `l2c_link_segments_xmitted` and `l2c_pin_code_request` were named earlier and now link from `src/RVL_SDK/bte/l2c_csm.c` and `l2c_link.c`.

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x8032D19C` | `0x294` | `fn_8032D19C` | `l2c_csm_closed` | static function | high | F-526-1 | `src/RVL_SDK/bte/l2c_csm.c` |
| `0x8032D430` | `0x170` | `fn_8032D430` | `l2c_csm_orig_w4_sec_comp` | static function | high | F-526-1 | `src/RVL_SDK/bte/l2c_csm.c` |
| `0x8032D5A0` | `0x168` | `fn_8032D5A0` | `l2c_csm_term_w4_sec_comp` | static function | high | F-526-1 | `src/RVL_SDK/bte/l2c_csm.c` |
| `0x8032D708` | `0x244` | `fn_8032D708` | `l2c_csm_w4_l2cap_connect_rsp` | static function | high | F-526-1 | `src/RVL_SDK/bte/l2c_csm.c` |
| `0x8032D94C` | `0x1D8` | `fn_8032D94C` | `l2c_csm_w4_l2ca_connect_rsp` | static function | high | F-526-1 | `src/RVL_SDK/bte/l2c_csm.c` |
| `0x8032DB24` | `0x3C8` | `fn_8032DB24` | `l2c_csm_config` | static function | high | F-526-1 | `src/RVL_SDK/bte/l2c_csm.c` |
| `0x8032DEEC` | `0x25C` | `fn_8032DEEC` | `l2c_csm_open` | static function | high | F-526-1 | `src/RVL_SDK/bte/l2c_csm.c` |
| `0x8032E148` | `0x1E4` | `fn_8032E148` | `l2c_csm_w4_l2cap_disconnect_rsp` | static function | high | F-526-1 | `src/RVL_SDK/bte/l2c_csm.c` |
| `0x8032E32C` | `0x15C` | `fn_8032E32C` | `l2c_csm_w4_l2ca_disconnect_rsp` | static function | high | F-526-1 | `src/RVL_SDK/bte/l2c_csm.c` |
| `0x8032E488` | `0x1A4` | `fn_8032E488` | `forward_peer_data` | function | high | F-526-1 | `src/RVL_SDK/bte/l2c_csm.c` |
| `0x8032E62C` | `0x154` | `fn_8032E62C` | `l2c_link_hci_conn_req` | function | high | F-526-1 | `src/RVL_SDK/bte/l2c_link.c` |
| `0x8032E9B8` | `0xB4` | `fn_8032E9B8` | `l2c_link_sec_comp` | function | high | F-526-1 | `src/RVL_SDK/bte/l2c_link.c` |
| `0x8032ED0C` | `0x108` | `fn_8032ED0C` | `l2c_link_send_to_lower` | function | high | F-526-1 | `src/RVL_SDK/bte/l2c_link.c` |
| `0x8032F35C` | `0x14` | `fn_8032F35C` | `l2c_link_processs_num_bufs` | function | high | F-526-1 | `src/RVL_SDK/bte/l2c_link.c` |
| `0x8032F370` | `0x1D0` | `fn_8032F370` | `l2cap_link_chk_pkt_start` | function | high | F-526-1 | `src/RVL_SDK/bte/l2c_link.c` |
| `0x8032F540` | `0x6C` | `fn_8032F540` | `l2cap_link_chk_pkt_end` | function | high | F-526-1 | `src/RVL_SDK/bte/l2c_link.c` |
| `0x8031A220` | see `symbols.txt` | `fn_8031A220` | `btm_acl_created` | function | high | F-526-1 | Not reconstructed (fallback) |
| `0x8031C370` | see `symbols.txt` | `fn_8031C370` | `btm_find_dev` | function | high | F-526-1 | Not reconstructed (fallback) |
| `0x8031FDCC` | see `symbols.txt` | `fn_8031FDCC` | `BTM_ReadPowerMode` | function | high | F-526-1 | Not reconstructed (fallback) |
| `0x80321150` | see `symbols.txt` | `fn_80321150` | `btm_sco_acl_removed` | function | high | F-526-1 | Not reconstructed (fallback) |
| `0x8032206C` | see `symbols.txt` | `fn_8032206C` | `btm_sec_l2cap_access_req` | function | high | F-526-1 | Not reconstructed (fallback) |

## HBM ut ResFontBase unit (#301)

For `wpse-v1` / `main.dol`, [F-301-1](findings/301-nw4hbm-ut.md#f-301-1-ut_resfontbase) names `detail::ResFontBase::FindGlyphIndex(const FontCodeMap*, u16)` (`0x801941C0`), `GetGlyphFromIndex` (`0x80194290`), `__vt__Q46nw4hbm2ut6detail11ResFontBase` (`0x803BCD38`, 0x58 bytes), `__vt__Q36nw4hbm2ut4Font` (`0x803BCD90`, 0x58 bytes) and the weak `Font` destructor `__dt__Q36nw4hbm2ut4FontFv` (`0x8018F1A0`). `ResFontBase` is 0x18 bytes: `Font` (vtable `0x0`, reader `0x4`), `mResource` `0x10`, `mFontInfo` `0x14`. Confidence high: names and sizes match xenoblade's symbols, and the unit matches.
## BTE security manager, issues 520 and 521

[F-521-5](findings/521-bte-btu-hcif.md#f-521-5-btm_secc-matching) gives the `btm_sec.c` boundaries and the evidence for these names. The 50 strings in `.data` `0x803E1F10`-`0x803E2920` keep `lbl_` names, split at the compiled string starts.

| Address | Size | Old name | Name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x80321728` | `0xB0` | `fn_80321728` | `BTM_SetPinType` | function | high | F-521-5 | `src/RVL_SDK/bte/btm_sec.c` |
| `0x80322898` | `0x1C` | `fn_80322898` | `btm_sec_init` | function | high | F-521-5 | `src/RVL_SDK/bte/btm_sec.c` |
| `0x803228B4` | `0x40` | `fn_803228B4` | `btm_sec_dev_reset` | function | high | F-521-5 | `src/RVL_SDK/bte/btm_sec.c` |
| `0x80323BA0` | `0x74` | `fn_80323BA0` | `btm_sec_pin_code_request_timeout` | function | high | F-521-5 | `src/RVL_SDK/bte/btm_sec.c` |
| `0x80323FC0` | `0x3E0` | `fn_80323FC0` | `btm_sec_execute_procedure` | static function | high | F-521-5 | `src/RVL_SDK/bte/btm_sec.c` |
| `0x803243A0` | `0x11C` | `fn_803243A0` | `btm_sec_start_authorization` | static function | high | F-521-5 | `src/RVL_SDK/bte/btm_sec.c` |
| `0x803244BC` | `0xE0` | `fn_803244BC` | `btm_sec_collision_timeout` | static function | high | F-521-5 | `src/RVL_SDK/bte/btm_sec.c` |
| `0x804BB628` | `0x6` | `lbl_804BB628`-`lbl_804BB62D` | `lbl_804BB628` | `.sdata2` address initializer | high extent | F-521-5 | `btm_sec_mkey_comp_event` local |
| `0x8031C198` | `0x120` | `fn_8031C198` | `btm_sec_alloc_dev` | function, outside unit | high name | F-521-5 | split left to its owning issue |
| `0x8031C2B8` | `0xB8` | `fn_8031C2B8` | `btm_find_dev_by_handle` | function, outside unit | high name | F-521-5 | split left to its owning issue |
| `0x8031DB5C` | `0xD0` | `fn_8031DB5C` | `BTM_DeleteStoredLinkKey` | function, outside unit | high name | F-521-5 | split left to its owning issue |
| `0x8031F7FC` | `0x150` | `fn_8031F7FC` | `btm_initiate_rem_name` | function, outside unit | high name | F-521-5 | split left to its owning issue |
## NW4R math units and lyt_init (#455)

Evidence: [F-455-1](findings/455-nw4r-math-lyt-init.md).

| Address | Symbol | Size | Confidence | Evidence |
| --- | --- | --- | --- | --- |
| `0x802A3470` | `nw4r::math::FrSqrt(f32)` | `0x24` | High | Xenoblade source match |
| `0x802A34A0` | `nw4r::math::CntBit1(u32)` | `0x58` | High | Name already in symbols; source match |
| `0x802A3500` | `nw4r::math::SinFIdx(f32)` | `0x70` | High | Xenoblade source and symbol |
| `0x802A3570` | `nw4r::math::CosFIdx(f32)` | `0x60` | High | Xenoblade source and symbol |
| `0x802A35D0` | `nw4r::math::SinCosFIdx(f32*, f32*, f32)` | `0x70` | High | Xenoblade source and symbol |
| `0x802A3640` | `nw4r::math::Atan2FIdx(f32, f32)` | `0x1B8` | High | Xenoblade source and symbol |
| `0x802A3800` | `nw4r::math::MTX34ToMTX33` | `0x34` | High | Xenoblade symbol |
| `0x802A3840` | `nw4r::math::MTX34Zero` | `0x20` | High | Xenoblade symbol |
| `0x802A3860` | `nw4r::math::MTX34Scale` | `0x54` | High | Xenoblade symbol |
| `0x802A38C0` | `nw4r::math::MTX34Trans` | `0x6C` | High | Xenoblade symbol |
| `0x802A3930` | `nw4r::math::MTX34RotAxisFIdx` | `0x38` | High | Xenoblade symbol |
| `0x802A3970` | `nw4r::math::MTX34RotXYZFIdx` | `0x168` | High | Size matches `SUPE01` map (360 bytes); paired-single assembly |
| `0x802A3AE0` | `nw4r::math::VEC3TransformNormal` | `0x90` | High | Xenoblade symbol |
| `0x802A3B70` | `nw4r::math::MTX44Identity` | `0x34` | High | Xenoblade symbol |
| `0x802A3BB0` | `nw4r::math::MTX44Copy` | `0x44` | High | Xenoblade symbol |
| `0x802A3C00` | `nw4r::math::VEC3TransformCoord(VEC3*, const MTX44*, const VEC3*)` | `0xF4` | Medium | MTX44 transform with divide by w; name follows the MTX34 overload |
| `0x802A3D00` | `nw4r::math::AABB::Set(const VEC3*, unsigned int)` | `0xBC` | High | Xenoblade source |
| `0x802A3DC0` | `nw4r::math::AABB::Set(const AABB*, const MTX34*)` | `0x1D8` | High | Xenoblade source; `SUPE01` size |
| `0x802A3FA0` | `nw4r::math::FRUSTUM::Set(f32, f32, f32, f32, const MTX34&)` | `0xB4` | High | Xenoblade source; `SUPE01` size |
| `0x802A4060` | `nw4r::math::FRUSTUM::Set(f32, f32, f32, f32, f32, f32, const MTX34&)` | `0x6D8` | High | Xenoblade source |
| `0x802A4740` | `nw4r::math::IntersectionLine3Plane` | `0x100` | High | `SUPE01` name and size |
| `0x802A4840` | `nw4r::math::IntersectionRay3Plane` | `0x164` | High | `SUPE01` name and size |
| `0x802A49B0` | `nw4r::math::FRUSTUM::IntersectAABB_Ex` | `0x1AC` | High | Xenoblade source; `SUPE01` size |
| `0x802A4B60` | `nw4r::lyt::LytInit()` | `0x54` | High | Petari source; version string |
| `0x803705F8` | `nw4r::math::detail::gSinCosTbl` | `0x1010` | High | Referenced by `SinFIdx` and the `MTX34RotXYZFIdx` assembly |
| `0x803C6D20` | anonymous `sArcTanTbl` | `0x108` | High | 33 `{value, delta}` pairs |
| `0x803C6E28` | LYT version string | `0x46` | High | `NW4R_LYT_Version_` target |
| `0x804B8A08` | anonymous `sEpsilon` (`0.0001f`) | `0x4` | Medium | Writable `.sdata` tolerance in the plane intersections; name descriptive |
| `0x804B8A10` | anonymous `NW4R_LYT_Version_` | `0x4` | High | Petari name |
| `0x804BB148` | `sFIdxRange` `{65536.0f, 65536.0f}` | `0x8` | Medium | Paired load in `MTX34RotXYZFIdx`; name descriptive |

## HBM ut TagProcessorBase and TextWriterBase units (#302, #303, #304)

For `wpse-v1` / `main.dol`, [F-302-1](findings/302-nw4hbm-ut.md#f-302-1-ut_tagprocessorbase-and-ut_textwriterbase) names all 95 functions from `0x801943A0` to `0x8019A220` after xenoblade's symbols, which have identical sizes and offsets: `TagProcessorBase<char>` and `<wchar_t>` (constructor, destructor, `Process`, `CalcRect`, `ProcessLinefeed`, `ProcessTab`), `TextWriterBase<char>` (`0x80194E60`–`0x80197800`) and `<wchar_t>` (`0x80197820`–`0x8019A200`), and `__sinit_\ut_TextWriterBase_cpp` (`0x8019A220`). Data: the `TagProcessorBase<wchar_t>` vtable `0x803BCDE8` and RTTI `0x803BCE28`, the `<char>` vtable `0x803BCE30` and RTTI `0x803BCE68`, `mFormatBufferSize` `0x803BCE70`/`0x803BCE74`, `mFormatBuffer` `0x803E9C48`/`0x803E9C50` and `mDefaultTagProcessor` `0x803E9C4C`/`0x803E9C54` (`<char>`/`<wchar_t>`). `TextWriterBase` is 0x60 bytes: `mCharSpace` `0x4C`, `mLineSpace` `0x50`, `mTabWidth` `0x54`, `mDrawFlag` `0x58`, `mTagProcessor` `0x5C`. MSL `vswprintf` is `0x801EE3C0`. Confidence high: names and sizes match xenoblade, and both units match.

## BTE SCO manager, issues 519 and 520

[F-520-1](findings/520-btm-sco.md) gives the `btm_sco.c` boundaries and [F-520-2](findings/520-btm-sco.md#f-520-2-reconstruction) the evidence for these names. The 13 strings in `.data` `0x803E1C10`-`0x803E1F10` keep `lbl_` names, split at the compiled string starts.

| Address | Size | Old name | Name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x803207D4` | `0x44` | `fn_803207D4` | `btm_sco_init` | function | high | F-520-2 | `src/RVL_SDK/bte/btm_sco.c` |
| `0x80320818` | `0x1DC` | `fn_80320818` | `btm_esco_conn_rsp` | static function | high | F-520-2 | `src/RVL_SDK/bte/btm_sco.c` |
| `0x80320F00` | `0xB8` | `fn_80320F00` | `BTM_RemoveSco` | function | high | F-520-2 | `src/RVL_SDK/bte/btm_sco.c` |
| `0x80321208` | `0x1A8` | `fn_80321208` | `BTM_ChangeEScoLinkParms` | function | high | F-520-2 | `src/RVL_SDK/bte/btm_sco.c` |
| `0x80321524` | `0x60` | `fn_80321524` | `btm_num_sco_links_active` | function | high | F-520-2 | `src/RVL_SDK/bte/btm_sco.c` |
| `0x80398150` | `0x10` | `lbl_80398150` | `btm_esco_defaults` | static `.rodata` object | high | F-520-2 | `src/RVL_SDK/bte/btm_sco.c` |
| `0x8031B0F8` | `0x9C` | `fn_8031B0F8` | `BTM_GetHCIConnHandle` | function, outside unit | high name | F-520-2 | split left to its owning issue |
| `0x8031BCA4` | `0x220` | `fn_8031BCA4` | `btm_chg_all_acl_pkt_types` | function, outside unit | high name | F-520-2 | split left to its owning issue |

## BTE inquiry, BTM init and power manager, issues 518 and 519

[F-519-1](findings/519-btm-inq-main-pm.md) gives the unit boundaries and [F-519-2](findings/519-btm-inq-main-pm.md#f-519-2-reconstruction) the evidence for these names.

| Address | Size | Old name | Name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x8031E2D0` | `0xB0` | `fn_8031E2D0` | `BTM_SetInquiryScanType` | function | high | F-519-2 | `src/RVL_SDK/bte/btm_inq.c` |
| `0x8031E380` | `0xB0` | `fn_8031E380` | `BTM_SetPageScanType` | function | high | F-519-2 | `src/RVL_SDK/bte/btm_inq.c` |
| `0x8031E430` | `0x98` | `fn_8031E430` | `BTM_SetInquiryMode` | function | high | F-519-2 | `src/RVL_SDK/bte/btm_inq.c` |
| `0x8031EE00` | `0x198` | `fn_8031EE00` | `btm_inq_db_reset` | function | high | F-519-2 | `src/RVL_SDK/bte/btm_inq.c` |
| `0x8031EF98` | `0x4` | `fn_8031EF98` | `btm_inq_db_init` | function | high | F-519-2 | `src/RVL_SDK/bte/btm_inq.c` |
| `0x8031EF9C` | `0xDC` | `fn_8031EF9C` | `btm_inq_find_bdaddr` | static function | high | F-519-2 | `src/RVL_SDK/bte/btm_inq.c` |
| `0x8031F078` | `0x104` | `fn_8031F078` | `btm_inq_db_new` | static function | high | F-519-2 | `src/RVL_SDK/bte/btm_inq.c` |
| `0x8031F17C` | `0xE4` | `fn_8031F17C` | `btm_set_inq_event_filter` | static function | high | F-519-2 | `src/RVL_SDK/bte/btm_inq.c` |
| `0x8031FB40` | `0xBC` | `fn_8031FB40` | `BTM_PmRegister` | function | high | F-519-2 | `src/RVL_SDK/bte/btm_pm.c` |
| `0x8031FE84` | `0x78` | `fn_8031FE84` | `btm_pm_reset` | function | high | F-519-2 | `src/RVL_SDK/bte/btm_pm.c` |
| `0x8031FEFC` | `0x50` | `fn_8031FEFC` | `btm_pm_sm_alloc` | function | high | F-519-2 | `src/RVL_SDK/bte/btm_pm.c` |
| `0x8031FF4C` | `0x21C` | `fn_8031FF4C` | `btm_pm_compare_modes` | static function | high | F-519-2 | `src/RVL_SDK/bte/btm_pm.c` |
| `0x80320168` | `0x160` | `fn_80320168` | `btm_pm_get_set_mode` | static function | high | F-519-2 | `src/RVL_SDK/bte/btm_pm.c` |
| `0x803202C8` | `0x234` | `fn_803202C8` | `btm_pm_snd_md_req` | static function | high | F-519-2 | `src/RVL_SDK/bte/btm_pm.c` |
| `0x804BB618` | `0x3` | `lbl_804BB618` | `general_inq_lap` | static `.sdata2` object | high | F-519-2 | `src/RVL_SDK/bte/btm_inq.c` |
| `0x804BB61C` | `0x3` | `lbl_804BB61C` | `limited_inq_lap` | static `.sdata2` object | high | F-519-2 | `src/RVL_SDK/bte/btm_inq.c` |
| `0x80398140` | `0x9` | `lbl_80398140` | `btm_pm_md_comp_matrix` | `.rodata` object | high | F-519-2 | `src/RVL_SDK/bte/btm_pm.c` |
| `0x804BB620` | `0x3` | `lbl_804BB620` | `btm_pm_mode_off` | `.sdata2` object | high | F-519-2 | `src/RVL_SDK/bte/btm_pm.c` |
| `0x804BB624` | `0x3` | `lbl_804BB624` | `btm_pm_mode_msk` | `.sdata2` object | high | F-519-2 | `src/RVL_SDK/bte/btm_pm.c` |
| `0x804AAD78` | `0x27C4` | `btm_cb` (0x27C8) | `btm_cb` | `.bss` object, size corrected | high | F-519-1 | `src/RVL_SDK/bte/btm_main.c` |
| `0x80311F40` | see `symbols.txt` | `fn_80311F40` | `GKI_get_tick_count` | function, outside unit | high name | F-519-2 | split left to its owning issue |
| `0x8031A180` | see `symbols.txt` | `fn_8031A180` | `btm_acl_init` | function, outside unit | high name | F-519-2 | split left to its owning issue |
| `0x8031A19C` | see `symbols.txt` | `fn_8031A19C` | `btm_handle_to_acl_index` | function, outside unit | high name | F-519-2 | split left to its owning issue |
| `0x8031C590` | see `symbols.txt` | `fn_8031C590` | `btm_dev_init` | function, outside unit | high name | F-519-2 | split left to its owning issue |
| `0x8031D6A4` | see `symbols.txt` | `fn_8031D6A4` | `BTM_ReadDeviceClass` | function, outside unit | high name | F-519-2 | split left to its owning issue |
## NW4R g3d resource common, dictionary and file (#386)

For `wpse-v1` / `main.dol`, [F-386-1](findings/386-g3d-res-common-dict-file.md) names the 44 functions of `nw4r::g3d` resource access in `.text` `0x8024AE80`–`0x8024CFFC` with their mangled CodeWarrior names: `ResName::operator==` and the five `detail::ResWrite*` command writers (`0x8024AE80`–`0x8024B024`), the six `ResDic` lookups (`0x8024B030`–`0x8024B3B0`), and 32 `ResFile` functions (`0x8024B3B0`–`0x8024CFFC`). Subtract `0x8004FD40` for `.text` file offsets. Names are high confidence from the CC0 ogws and zeldaret/ss references and exact matches, except three descriptive names with low confidence: `ResDic::Find(const char*, u32)` (`0x8024B270`, 0x54 bytes), the `ResFile::GetResPltt` and `GetResTex` overloads taking a name and length (`0x8024B4C0`, `0x8024B630`), and the eight `ResFile::HasRes*` predicates (`0x8024BE00`–`0x8024C07C`, 0x4C bytes each), which return whether the file's top-level dictionary has a group.

The eleven group names are anonymous-namespace `ResNameData27` objects of 0x20 bytes, `ResNameData_Models` through `ResNameData_Ext`, at `.data` `0x803C5760`–`0x803C58C0`; the original `lbl_803C5880` (0x40 bytes) covers `ResNameData_AnmScn` and `ResNameData_Ext`. Revisions checked by `ResFile::CheckRevision`: `ResMdl` 11, `ResAnmChr` 5, `ResAnmVis` 4, `ResAnmClr` 4, `ResAnmTexPat` 4, `ResAnmTexSrt` 5, `ResAnmShp` 4, `ResAnmScn` 5; `ResTex` and `ResPltt` accept 1 or 3 through out-of-line `CheckRevision`.

Outside callees named for the link, from their call roles and bodies: `ResMdl::Bind` (`0x8024D960`), `ResMdl::Init` (`0x8024DA70`), `ResMdl::Terminate` (`0x8024E010`), `ResPltt::DCStore(bool)` (`0x80250F20`), `ResPltt::CheckRevision` (`0x80250F40`), `ResTex::CheckRevision` (`0x802510F0`), `ResTex::Init` (`0x80251120`), and `ResAnmTexPat::Bind` (`0x80251B90`). Their splits and sizes are unchanged.
## HBM sound units (#305, #306)

For `wpse-v1` / `main.dol`, [F-305-1](findings/305-hbm-sound.md#f-305-1-mix-syn-synctrl-synenv-synmix-synpitch-synsample-synvoice-and-seq) names all 56 functions from `0x8019A2D0` to `0x8019DC40` after xenoblade's symbols, which have identical sizes and offsets: `HBMMIX*` and the index allocator (`mix.c`), `HBMSYN*` (`syn.c`), the `__HBMSYN` controller, envelope, mix, pitch, sample and voice helpers, and `HBMSEQ*` (`seq.c`). Data: `__HBMMIXVolumeTable` `0x803BCE78`, `__HBMMIXPanTable` `0x803BD608`, `__HBMMIX_DPL2_front` `0x803BD808`, `__HBMMIX_DPL2_rear` `0x803BD908`, `__HBMSYNn128` `0x803BDA08`, `__HBMSYNVolumeAttenuation` `0x803BDC08`, `__HBMSYNAttackAttnTable` `0x803BDE08`, `__HBMSYNCentsTable` `0x803BDF98`, `__HBMSYNOctavesTableUp` `0x803BE128`, `__HBMSYNSemitonesTableUp` `0x803BE158`, `__HBMSYNSemitonesTableDown` `0x803BE188`, `__HBMSEQMidiEventLength` `0x803BE388`. BSS: `sIndex2IdTable` `0x803E9C60`, `sId2IndexTable` `0x803E9C70`, `__HBMMIXSoundMode` `0x803E9CD0`, `__HBMMIXChannel` `0x803E9CD4`, `__s_HBMMIXChannel` `0x803E9CD8` (16 x 0x64), `__init` `0x803EA318`, `__HBMSYNSynthList` `0x803EA320`, `__HBMSYNVoice` `0x803EA324`, `__s_HBMSYNVoice` `0x803EA328` (16 x 0x4C), `__init` `0x803EA7E8`, `__HBMSEQSequenceList` `0x803EA7F0`, `__init` `0x803EA7F4`. Confidence high: names and sizes match xenoblade, and all nine units match.

## BTE discovery database, issue 518

[F-518-1](findings/518-btm-discovery.md) gives the `btm_discovery.c` boundaries.

| Address | Size | Old name | Name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x8031DF90` | `0xB8` | `fn_8031DF90` | `btm_discovery_db_reset` | function | high | F-518-2 | `src/RVL_SDK/bte/btm_discovery.c` |

## NW4R g3d ResMdl (#388)

For `wpse-v1` / `main.dol`, [F-388-1](findings/388-g3d-resmdl.md) names the 32 `nw4r::g3d::ResMdl` functions in `.text` `0x8024D000`–`0x8024E0F8` with their mangled CodeWarrior names: `GetParent` (`0x8024D000`, 0x20 bytes), `GetResByteCode(const char*)`, the `ResNode`, `ResVtxPos`, `ResVtxNrm` and `ResVtxClr` accessors and counts, `GetResVtxTexCoord(int)`, `GetResVtxFurPos(int)`, the `ResMat` and `ResShp` accessors and counts, `Bind`, `Init` and `Terminate`. Subtract `0x8004FD40` for `.text` file offsets. Names are high confidence from the CC0 zeldaret/ss reference and exact matches, except `GetParent`, which is medium confidence from the identical ogws `ResAnmTexPat::GetParent`. `ResMdlData` follows ss: `toResFileData` at 0xC, the dictionaries from `toResByteCodeDic` at 0x10 to `toResShpDic` at 0x38, `toResVtxFurVecDic` at 0x28 and `toResVtxFurPosDic` at 0x2C.

Outside callees named for the link: `ResShp::Init` (`0x8024E280`), `ResShp::Terminate` (`0x8024E610`), `ResMat::Bind` (`0x802506E0`), `ResMat::Init` (`0x802507B0`), and `DCStore(bool)` of `ResVtxPos` (`0x80250E60`), `ResVtxNrm` (`0x80250E80`), `ResVtxClr` (`0x80250EA0`), `ResVtxTexCoord` (`0x80250EC0`), `ResVtxFurVec` (`0x80250EE0`) and `ResVtxFurPos` (`0x80250F00`).
## RVL_SDK WPADHIDParser and WPADEncrypt (#502)

For `wpse-v1` / `main.dol`, [F-502-2](findings/502-wpad-hidparser-encrypt.md#f-502-2-function-names) names the 25 WPADHIDParser.c functions from `0x802FE650` to `0x80302914` and the three WPADEncrypt.c functions from `0x80302920` to `0x803039E0` in `config/wpse-v1/symbols.txt`. `WPADiHIDParser`, `WPADiCreateKey`, `WPADiCreateKeyFor3rd` and `WPADiDecode` are global and high confidence. The parser statics (`__wpadAbortInitExtension`, `__wpadGetDevConfig`, `__wpadGetExtConfig`, `__wpadGetExtType`, `__wpadGetGameInfo`, the `__a1_*` report handlers and `__parse_*` parsers, `__a1_unused_report`) are `scope:local` and medium confidence. Source-only static data names, medium confidence: `_cExtInvalidData` (`.rodata` `0x80397E18`, 21 bytes), `__a1_input_reports_array` (`.data` `0x803DCF90`, 32 pointers), `_wpadCalibrationX`/`Y`, `_wpadCenterX`/`Y`, `_wpadRolag` (`.bss` from `0x8047AC40`, 4 floats each), `_wpadExtData` (21 bytes), the `.sbss` arrays `_wpadDevType`, `_wpadDevMode`, `_wpadCLCompt`, `_wpadExtInitRetryCnt`, `_recv_3f`, `_recv_3e` (4 bytes each, laid out in reverse declaration order), WPADEncrypt's `base`/`tb`/`base3`/`tb3` (`.data` `0x803DD500`) and the one-byte states `SEED`, `NUM_A`, `NUM_B`, `NUM_C` (`.sbss` `0x804B9AC8`–`0x804B9ACC`). `lbl_804B9ACB` is resized from five bytes to one.

## RVL_SDK WPADMem and debug_msg (#503)

For `wpse-v1` / `main.dol`, [F-503-2](findings/503-wpadmem-debug-msg.md#f-503-2-function-names) names the 13 WPADMem.c functions from `0x803039E0` to `0x803045FC` in `config/wpse-v1/symbols.txt`. `WPADWriteGameData`, `WPADReadGameData`, `WPADReadMemoryAsync`, `WPADWriteMemoryAsync`, `WPADGetGameTitleUtf16` and `WPADSetGameTitleUtf16` are global and high confidence; their declarations are added to `include/revolution/wpad.h`. `__wpadResultCallback`, `__wpadWriteCallback`, `__wpadWriteGameDataSub`, `__wpadWriteCheck1` and `__wpadWriteCheck2` are `scope:local` and medium confidence. The source-only static `_wpadGameTitle` (`.bss` `0x8047ACA8`, `u16[17]`) is medium confidence. `DEBUGPrint` (`0x80304600`) is in debug_msg.c. KPAD.c starts at `0x80304650` and is not named here.
## KPAD June 2009 layouts (#503)

`KPADStatus` is `0xB0`, `KPADUnifiedWpadStatus` is `0x38`, and `KPADInsideStatus` is `0x578`. Sixteen inline samples at `0x13C` precede the optional buffer/count at `0x4BC`/`0x4C0`. Sampling/connect callbacks are `0x54C`/`0x550`; read lock, DPD request, and DPD hardware state are distinct bytes at `0x4FF`/`0x556`/`0x557`. Types, offsets, naming confidence, and validation are recorded in [F-503-4](findings/503-kpad-layout.md).

## KPAD motion symbols (#503)

The first twenty retained functions from `0x80304650` through `0x803067F4` receive Petari-derived names: four parameter setters, `reset_kpad`, `KPADEnableAimingMode`, `calc_button_repeat`, `read_kpad_button`, `calc_acc`, `calc_acc_horizon`, `calc_acc_vertical`, `read_kpad_acc`, four `select_*obj_*` routines, `calc_dpd_variable`, `read_kpad_dpd`, and two stick clamps. Their behavior and locations are supported by target comparison; spelling follows the CC0 reference rather than recovered symbols. The channel array is `inside_kpads` at `0x8047ACD0`. Referenced motion globals retain the reference names, with addresses derived from corresponding SDA relocations. The two words at `0x804B9AE8` form the single eight-byte `Vec2_0` object. [F-503-5](findings/503-kpad-reconstruction.md) records the complete source and source-linked frozen verification.

## NW4R g3d ResShp and ResTev (#389)

For `wpse-v1` / `main.dol`, [F-389-1](findings/389-g3d-resshp-restev.md) names eight `nw4r::g3d::ResShp` functions in `.text` `0x8024E100`–`0x8024E9C4` (`GetResVtxPos`, `GetResVtxNrm`, `GetResVtxClr(u32)`, `GetResVtxFurPos`, `Init`, `Terminate`, `CallPrePrimitiveDisplayList(bool, bool)`, `CallPrimitiveDisplayList(bool)`) and eleven `nw4r::g3d::ResTev` functions in `0x8024E9D0`–`0x8024F0A8` (`GXSetTevAlphaIn`, `GXSetTevAlphaOp`, `GXSetTevKColorSel`, `GXGetTevOrder`, `GXSetTevOrder`, `GXSetTevColorIn`, `GXSetTevColorOp`, `SetNumTevStages`, `CallDisplayList`, `CopyTo`, `DCStore`), with their mangled CodeWarrior names. Subtract `0x8004FD40` for `.text` file offsets. Names are high confidence from the CC0 zeldaret/ss and Kirby's Return to Dreamland references and exact matches. The `ResTev` statics are `r2c` (`.rodata` `0x8036C690`, 0x20), `dlsize` (`.rodata` `0x8036C6B0`, 0x40) and `c2r` (`.data` `0x803C58C0`, 0x10).

Layouts follow ss: `ResTevData` has `nStages` at 0xC, `texCoordToTexMapID` at 0x10 and the display lists at 0x20. Each 0x30-byte `ResTevVariableDL` starts at 0x80 and holds `tevKonstantSel` at +0, `tevOrder` at +0xA, `tevColorCalc` at +0xF and `alphaCalcAndSwap` at +0x19. Outside callees named for the link: `GetArray` of `ResVtxPos` (`0x80250B40`), `ResVtxNrm` (`0x80250C20`), `ResVtxClr` (`0x80250D00`) and `ResVtxTexCoord` (`0x80250DA0`); `detail::Copy32ByteBlocks` (`0x8025B460`); and `detail::ZeroMemory16ByteBlocks` (`0x8025B610`).

## NW4R g3d ResMat (#390, #391)

For `wpse-v1` / `main.dol`, [F-390-1](findings/390-g3d-resmat.md) names 53 functions in `.text` `0x8024F0B0`–`0x80250AFC`: the `nw4r::g3d` members of `ResMatPix`, `ResMatTevColor`, `ResMatIndMtxAndScale`, `ResMatChan`, `ResMatTexCoordGen`, `ResTexObj`, `ResTlutObj`, `ResTexSrt`, `ResGenMode`, `ResMatMisc`, `ResMatFur`, `ResTexPlttInfo` and `ResMat`, with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`. Names are high confidence from CC0 zeldaret/ss and the exact match. Three equal-sized functions were identified by field offset rather than by position: `ResGenMode::GXSetNumTevStages` (`0x8024F770`), `ResMat::GetResMatFur` (`0x80250800`) and `ResMat::GetResUserData` (`0x80250820`). The unit owns a switch jump table at `.data` `0x803C58D0` (0x54) and nine constants at `.sdata2` `0x804BAA98`–`0x804BAACC`; `0x804BAAC8` is 4 bytes. Outside callees named for the link: `ResTex::GetTexObjParam` (`0x80250F70`) and `ResTex::GetTexObjCIParam` (`0x80251030`).
## BTE hcisu_h2 and uusb_ppc (#509)

For `wpse-v1` / `main.dol`, [F-509-2](findings/509-bte-hcisu-uusb.md#f-509-2-function-names) names the eight hcisu_h2.c functions from `0x80312720` and the eleven uusb_ppc.c functions from `0x80312DE0` to `0x80313A54`. The public names are high confidence and the static `uusb_*` callbacks medium confidence, all following Petari `e0ab141`. Data: `hcisu_h2` (`.rodata` `0x80397E30`, 0x14 bytes), `hcisu_h2_cb` (`.bss` `0x804A6FA8`, 0x24), `hcisu_preamble_table` (`.sdata2` `0x804BB5E0`, 4) and `hcisu_msg_evt_table` (`0x804BB5E8`, 8); `usb` (`.bss` `0x804A6FE0`, 0x4C), `__uusb_ppc_stack1` (`0x804A7040`) and `__uusb_ppc_stack2` (`0x804A8040`, 0x1000 each); `wait4hci` and `__ntd_ios_file_descriptor` (`.sdata` `0x804B8EA0`/`0x804B8EA4`); and the `.sbss` flags `uusb_g_usb_devid_found` (`0x804B9B60`), `uusb_g_trace_state_initialized` (`0x804B9B61`), `__ntd_ohci` (`0x804B9B64`), `__ntd_ohci_init_flag` (`0x804B9B68`), `__ntd_pid_vid_specified` (`0x804B9B6C`), `__ntd_vid` (`0x804B9B70`) and `__ntd_pid` (`0x804B9B74`). The field names of `tHCISU_H2_CB`, `tUUSB_CB` and `tUUSB` are descriptive and medium confidence. Outside link names (three GKI pool functions and eight IUSB functions) are listed in F-509-2. `tHCI_CFG` is `{vid, pid}`, high confidence from its values `{0x0A5C, 0x2101}`.

## BTE GKI (#508)

For `wpse-v1` / `main.dol`, [F-508-2](findings/508-bte-gki.md#f-508-2-function-names) names the remaining GKI functions: `gki_init_free_queue` (`0x803109E0`, static), `gki_buffer_init` (`0x80310C00`), `gki_timers_init` (`0x80311E80`), `GKI_sched_lock` (`0x80312560`), `GKI_sched_unlock` (`0x80312570`), `GKI_send_event` (`0x80312590`), `GKI_exception` (`0x803126D0`), `GKI_os_malloc` (`0x803126E0`) and `GKI_os_free` (`0x80312710`). Their callees `App_MEMalloc` (`0x80309FB0`) and `App_MEMfree` (`0x8030A000`) are named for the link. The GKI control block is `gki_cb` (`.bss` `0x8047E4C8`, 0x28AE0 bytes, type `tGKI_CB`). All names follow Petari `e0ab141` and bluedroid and are high confidence. The layout of `tGKI_CB` follows the reference with this revision's pool counts (F-508-3); its total size matches the target object.

## TMC JPEG encoder, first half (#348)

For `wpse-v1` / `main.dol`, [F-348-2](findings/348-tmc-jpeg.md#f-348-2-unit-boundaries-names-and-remaining-work) names the 25 functions in `.text` `0x801F6060`–`0x801F8DC4` in `config/wpse-v1/symbols.txt`. All names are descriptive and medium confidence; only the `TMCJPEG` prefix comes from the banner. Global: `TMCJPEGEncode` (`0x801F6060`), `TMCJPEGEncodeToBuffer`, `TMCJPEGEncodeBlock`, `TMCJPEGSetEncodeEdgeMCU`, `TMCJPEGEncodeMCUsWithEdges`, `TMCJPEGWriteRestartMarker` (`0x801F7C20`), `TMCJPEGInitEncoder` (`0x801F7D5C`), `TMCJPEGEncodeMCUs`, `TMCJPEGForwardDCT` (`0x801F855C`), `TMCJPEGForwardDCTSigned`, `TMCJPEGInitHuffTables` (`0x801F8968`) and `TMCJPEGBuildHuffTables`. `scope:local`: `EncodeEdgeMCU411`/`420`/`422` (`0x801F6874`–`0x801F6B44`), `WriteFrameHeader`, `WriteHeaders`, `SetParam`, `SetupPlanes`, `GetEncoderInfo`, `SetEncodeMCU`, `EncodeMCU411`/`420`/`422` and `WriteScanHeader` (`0x801F8498`). Data: `__TMCJPEGVersion` (`.sdata` `0x804B8850`, 4 bytes, resized from 8) and the local Annex K tables `sDCLumBits`, `sDCLumVals`, `sDCChrBits`, `sDCChrVals`, `sACLumBits`, `sACLumVals`, `sACChrBits` and `sACChrVals` (`.rodata` `0x80367868`–`0x80367A16`, replacing `lbl_80367868`). `lbl_80367840` is resized to 0x22 bytes. Layouts of `TMCJPEGContext` (0x164C), `TMCJPEGState`, `TMCJPEGPlane` (0x34), `TMCJPEGComponent` (0x24), `TMCJPEGStream`, `TMCJPEGEncoder` (0x6FC) and `TMCJPEGParam` (0xA0) are in `include/TMC_JPEG/tmcjpeg.h`; F-348-2 lists the key offsets.

Outside callees named for the link, owned by #349: `TMCJPEGBuildQuantTables` (`0x801F8DC4`), `TMCJPEGStreamInit`, `TMCJPEGStreamGetSize`, `TMCJPEGStreamPutByte`, `TMCJPEGStreamPutWord`, `TMCJPEGStreamPutBytes`, `TMCJPEGStreamPutMarker`, `TMCJPEGStreamPutEOI`, `TMCJPEGSelectPutBits` (`0x801F9488`), `TMCJPEGStreamPutBits` (`0x801F95B4`), `TMCJPEGSetupRGBA8` (`0x801F9764`) and `TMCJPEGSetupRGB565` (`0x801FA424`).
## WUDHidHost and WUD debug_msg (#507)

For `wpse-v1` / `main.dol`, [F-507-2](findings/507-wud-hidhost.md#f-507-2-names) names `WUDHidHostCallback` (`0x803103A0`, medium confidence; Swiftshine/key calls it `WUDiHidHostEventCallback`), `bta_hh_co_data` (`0x80310920`), `bta_hh_co_open` (`0x80310960`), `bta_hh_co_close` (`0x80310970`) and `WUD_DEBUGPrint` (`0x80310990`). WUD.c callees named for the link, high confidence from the CC0 Petari and Swiftshine/key `WUD.c` order and bodies: `WUDSetSniffMode` (`0x8030D660`), `WUDiRemoveDevice` (`0x8030E540`), `WUDiGetDevInfo` (`0x8030E6C0`), `WUDiMoveTopSmpDevInfoPtr` (`0x8030E7A0`), `WUDiMoveBottomSmpDevInfoPtr` (`0x8030E8C0`), `WUDiMoveTopOfDisconnectedSmpDevice` (`0x8030E9E0`), `WUDiMoveTopStdDevInfoPtr` (`0x8030EB50`), `WUDiMoveBottomStdDevInfoPtr` (`0x8030EC70`), `WUDiMoveTopOfDisconnectedStdDevice` (`0x8030ED90`), `WUDiGetDiscoverDevice` (`0x803101E0`), `WUDiSetDevAddrForHandle` (`0x80310320`), `WUDiGetDevAddrForHandle` (`0x80310340`), `WUDiSetQueueSizeForHandle` (`0x80310360`) and `WUDiSetNotAckNumForHandle` (`0x80310380`). The WUD control block `_wcb` is at `.bss` `0x8047C320` (0x750 bytes, type `WUDCB`).

## TMC JPEG quantization and bit stream (#349)

For `wpse-v1` / `main.dol`, [F-349-1](findings/349-tmc-jpeg.md) adds `PutBitsFast` (`0x801F94C8`, `scope:local`) to the #349 callee names that #348 gave for the link. It splits `lbl_80367A18` into seven local `s32[64]` tables, each 0x100 bytes: `sZigzag`, `sAANScale`, `sAANDivisor`, `sDefaultLumScale`, `sDefaultLumQuant`, `sDefaultChrScale` and `sDefaultChrQuant`. It also renames `lbl_80368118` to `sBitMask`, resized from 0x10 to 9 bytes. All names are descriptive, medium confidence. F-349-2 records the layout of the colour converters from `0x801F9850` to `0x801FAE28`, which keep their `fn_` names.

## FA PDM BPB, disk and MBR (#352)

For `wpse-v1` / `main.dol`, [F-352-1](findings/352-fa-pdm.md) names the PDM functions in `.text` `0x801FC620`–`0x801FD5C4` after VF without the `VFi` prefix, medium confidence. `pdm_bpb.c`: `pdm_bpb_calculate_common_bpb_fields` (`0x801FC620`), `pdm_bpb_get_bpb_information`, `pdm_bpb_get_fsinfo_information`. `pdm_disk.c`: `pdm_disk_convert_lsector_to_block` (`0x801FCB20`, `scope:local`), `pdm_disk_convert_block_to_lsector` (`0x801FCBD0`, `scope:local`), `pdm_disk_init_disk_set`, `pdm_disk_open_disk`, `pdm_disk_close_disk`, `pdm_disk_set_disk`, `pdm_disk_get_media_information`, `pdm_disk_get_lba_size`, `pdm_disk_initialize`, `pdm_disk_mount`, `pdm_disk_format`, `pdm_disk_physical_read`, `pdm_disk_physical_write`, `pdm_disk_unmount`, `pdm_disk_finalize`, `pdm_disk_notify_media_eject`, `pdm_disk_notify_media_insert`. `pdm_mbr.c`: `pdm_mbr_check_master_boot_record` (`0x801FD360`) and `pdm_bpb_check_boot_sector`. Callees named for the link: `pdm_part_set_driver_error_code` (`0x801FDC40`) and `pdm_part_set_change_media_state` (`0x801FE190`). Data: `pdm_disk_set` (`.bss` `0x803EAFA8`, 0x8F4 bytes, `PDM_DISK_SET`) and `pdm_disk_tbl` (`.bss` `0x803EB8A0`, 0xD0 bytes, `scope:local`). Layouts are in `include/revolution/fa/pdm.h`.

## FA PDM partition (#353)

For `wpse-v1` / `main.dol`, [F-353-1](findings/353-fa-pdm-partition.md) names the 16 previously unnamed functions of `pdm_partition.c` (`.text` `0x801FD5D0`–`0x801FE250`). VF names without `VFi`, medium confidence: `pdm_part_get_start_sector` (`0x801FD5D0`), `pdm_part_chg_ltop` (`0x801FDA20`), `pdm_part_open_partition` (`0x801FDAF0`), `pdm_part_close_partition` (`0x801FDC00`), `pdm_part_get_driver_error_code` (`0x801FDC50`), `pdm_part_get_media_information` (`0x801FDC60`), `pdm_part_format` (`0x801FDDD0`), `pdm_part_logical_read` (`0x801FDE40`) and `pdm_part_logical_write` (`0x801FDF30`). Descriptive, low confidence: `pdm_part_is_initialized` (`0x801FDC30`), `pdm_part_initialize` (`0x801FDCA0`), `pdm_part_mount` (`0x801FDD20`), `pdm_part_unmount` (`0x801FE020`), `pdm_part_finalize` (`0x801FE0D0`), `pdm_part_set_callback` (`0x801FE150`) and `pdm_part_clear_callback` (`0x801FE170`). `PDM_PARTITION` (0x28 bytes) and `PDM_PART_CALLBACK` are in `include/revolution/fa/pdm.h`. F-353-2 maps `0x801FE250`–`0x801FEDB0` to VF `pf_cache.c` routines; they keep their `fn_` names.

## FA PrFILE2 sector cache (#353, #354, #355)

For `wpse-v1` / `main.dol`, [F-354-1](findings/354-fa-pf-cache.md) names the 36 functions of `pf_cache.c` (`.text` `0x801FE250`–`0x80200004`). VF names without `VFi`, medium confidence, in address order: `PFCACHE_InitPageList` (`0x801FE250`), `PFCACHE_SearchForPage` (`0x801FE4B0`), `PFCACHE_SearchForFreePage` (`0x801FE590`), `PFCACHE_FlushPageIfNeeded` (`0x801FE620`), `PFCACHE_DoAllocatePage` (`0x801FE710`), `PFCACHE_DoReadPage` (`0x801FE940`), `PFCACHE_DoReadPageAndFlushIfNeeded` (`0x801FEB50`), `PFCACHE_DoReadNumSector` (`0x801FEDB0`), `PFCACHE_DoWritePage` (`0x801FF010`), `PFCACHE_DoWriteNumSectorAndFreeIfNeeded` (`0x801FF250`), `PFCACHE_DoFlushCache` (`0x801FF600`), `PFCACHE_SetCache` (`0x801FF6C0`), `PFCACHE_SetFATBufferSize` (`0x801FF6E0`), `PFCACHE_SetDataBufferSize` (`0x801FF6F0`), `PFCACHE_InitCaches` (`0x801FF700`), `PFCACHE_AllocateDataPage` (`0x801FF8F0`), `PFCACHE_FreeDataPage` (`0x801FFA00`), `PFCACHE_ReadFATPage` (`0x801FFA90`), `PFCACHE_ReadDataPage` (`0x801FFAD0`), `PFCACHE_ReadDataPageAndFlushIfNeeded` (`0x801FFB20`), `PFCACHE_ReadDataNumSector` (`0x801FFB70`), `PFCACHE_WriteFATPage` (`0x801FFBD0`), `PFCACHE_WriteDataPage` (`0x801FFC10`), `PFCACHE_WriteDataNumSectorAndFreeIfNeeded` (`0x801FFCA0`), `PFCACHE_SearchDataCache` (`0x801FFD00`), `PFCACHE_FlushFATCache` (`0x801FFD10`), `PFCACHE_FlushDataCacheSpecific` (`0x801FFD90`), `PFCACHE_FlushAllCaches` (`0x801FFE60`) and `PFCACHE_FreeAllCaches` (`0x801FFEC0`). Descriptive, low confidence: `PFCACHE_DoWriteSector` (`0x801FF130`), `PFCACHE_SetWriteThrough` (`0x801FF7D0`), `PFCACHE_ClearWriteThrough` (`0x801FF850`), `PFCACHE_AllocateFATPage` (`0x801FF870`), `PFCACHE_FreeFATPage` (`0x801FF970`), `PFCACHE_WriteFATSector` (`0x801FFC50`) and `PFCACHE_FlushDataCache` (`0x801FFD50`). Driver callees named for the link, VF names, medium confidence: `PFDRV_lread` (`0x801FC450`) and `PFDRV_lwrite` (`0x801FC4D0`). `PF_CACHE_PAGE`, `PF_SECTOR_CACHE` and `PF_VOLUME` (0x3F58 bytes; `bpb` at 0, `cache` at 0x1624, `label` at 0x1648) are in `include/revolution/fa/pf_cache.h` and `include/revolution/fa/pf_volume.h`.
## DVD core unit (#481)

[F-481-1](findings/481-dvd.md) names the dvd.c statics and data from the tp and Petari references, matched against target code. Data names: `ImmCommand`, `BB2`, `FatalAlarm`, `CoverAlarm`, `ResetAlarm`, `DummyCommandBlock`, `CurrDiskID`, `__DVDGameTocBuffer`, `__DVDPartInfoBuffer`, `__DVDRestartMotorCommandBlock` (`.bss` `0x80458F00`, 0x30), the `.sbss` state flags, `LastResetEnd` (`0x804B9788`, 8 bytes) and `sDVDSourceFile` (`.sdata` `0x804B8B9C`). `__DVDStopMotorCommandBlock` is resized from 0x60 to its real 0x30. The static `Callback` at `0x802D6510` and `cbForPrepareCoverRegister` at `0x802D4660` are local. They are distinct from the dvderror.c objects of the same names.

[F-481-2](findings/481-dvd.md#f-481-2-broadway-register-accessor-names) corrects the broadway register accessors. `0x802D98B0`/`0x802D98E0` are the cover pair, `0x802D98C0`/`0x802D9A50` the status pair, and `0x802D98D0`/`0x802D9BC0` the control pair. This supersedes the #482 note above, which placed cover and status at `DVDErrorInfo` 0x10 and 0x14: those members are status and control. The dvderror.c callbacks are `cbForPrepareControlRegister` (`0x802D71C0`) and `cbForPrepareStatusRegister` (`0x802D7250`). Confidence is high for all names; only names changed outside dvd.c.

## NW4R g3d ResVtx and ResTex (#392, part of #393)

For `wpse-v1` / `main.dol`, [F-392-1](findings/392-g3d-resvtx-restex.md) names 17 functions in `.text` `0x80250B00`–`0x80250F18` (`SetArray`, `CopyTo` and `DCStore` of `nw4r::g3d::ResVtxPos`, `ResVtxNrm` and `ResVtxClr`, with `ResVtxClr::SetArray(GXAttr)`; `ResVtxFurPos::SetArray(u16)`; and the already named `GetArray` and `DCStore` members) and 6 in `0x80250F20`–`0x8025112C` (`ResPltt::DCStore`, `ResPltt::CheckRevision`, `ResTex::GetTexObjParam`, `ResTex::GetTexObjCIParam`, `ResTex::CheckRevision`, `ResTex::Init`), with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`. Names are high confidence from CC0 zeldaret/ss and the exact match. `ResPltt` and `ResTex` both declare `COMPATIBLE_REVISION` (1), the second revision their `CheckRevision` accepts; the name is descriptive, medium confidence. `ResPlttData` and `ResTexData` keep `revision` at 0x8 and the block size at 0x4.
## Issue #514 completion

[F-514-4](findings/514-dm-completion.md) supersedes the earlier NonMatching API status: all eleven retained API wrappers, two state-machine dispatchers, and eight power functions are source-linked with GC/3.0a3 and compare at 100%. Its address inventory accepts all 25 #514 functions and identifies the two trailing #515 callbacks. Main owns `bta_dm_search_cb` (0x7C) and `bta_dm_cb` (0x104); power owns `bta_dm_conn_srvcs` (0x2E). Static registration pairs, action/state tables, exact diagnostic lengths, and alignment are recorded there. Canonical declarations now live under `include/revolution/bte/`; the legacy API header forwards to them. The preceding action data split extends to 0x803E0828 to contain its original five-byte alignment gap; actual symbols and string payload sizes do not change.

## F-462-1: NW4R Layout interface

`nw4r::lyt::Layout` is 32 bytes: vptr at 0, animation list at 4, root pane at 0x10, group container at 0x14, and width/height at 0x18/0x1C. Its vtable at 0x803C6EF8 has fourteen methods, including BindAnimationAuto before SetAnimationEnable. The reference origin byte is absent. See [F-462-1](findings/462-layout-interface.md) for native constructor and vtable evidence.

## NW4R g3d ResNode and resource animations (#393, part of #394)

For `wpse-v1` / `main.dol`, [F-393-1](findings/393-g3d-resnode-resanm.md) names 30 functions in `.text` `0x80251130`–`0x80253C7C` with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`: `nw4r::g3d::ResNode::PatchChrAnmResult`, `CalcChrAnmResult` and `GetResUserData`; `detail::GetResKeyFrameAnmResult` and `detail::GetResColorAnmResult`; `GetAnmResult` of `ResAnmVis`, `ResAnmClr`, `ResAnmTexPat`, `ResAnmTexSrt` and `ResAnmChr`; the local `SearchFrame` of `g3d_resanmtexpat.cpp`; the local `g3d_resanmchr.cpp` helpers `GetAnmScale`, `GetAnmRotation`, `GetAnmTranslation`, the eight `GetAnmResult_*` variants and the weak `CalcAnimationFVS` instances for `ResAnmChrFVS96Data`, `ResAnmChrFVS48Data` and `ResAnmChrFVS32Data`; and `ChrAnmResult::GetRotTrans`, `GetMtx` and `SetRotTrans`. The local function table `gGetAnmResultTable` is `.rodata` `0x8036C6F0` (0x20). Names are high confidence from CC0 zeldaret/ss and the exact match; `ResNode::GetResUserData` reads `ResNodeData::toResUserData` at 0x6C.

## NW4R g3d scene animations and transform (#394, part of #395)

For `wpse-v1` / `main.dol`, [F-394-1](findings/394-g3d-resanmscn-transform.md) names 31 functions in `.text` `0x80253C80`–`0x80255724` with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`: `nw4r::g3d::ResLightSet::Bind`; `GetAnmResult` of `ResAnmAmbLight`, `ResAnmLight`, `ResAnmFog`, `ResAnmCamera` and `ResAnmShp`; 21 `ResAnmScn` accessors and `Bind`; and `detail::CalcViewNrmMtx`, `CalcViewTexMtx` and `CalcInvWorldMtx`. Names are high confidence from CC0 zeldaret/ss and the exact match. The `ResAnmScn` accessors absent from ss (`HasResLightSet`, `HasResAnmCamera`, the fog accessors and the `NumEntries` accessors) are medium confidence, named after the ss pattern and placed by their dictionary-name references. Where an `int` and a `u32` index accessor would be byte-identical, the `u32` name is low confidence. The local dictionary names are `ResNameData_LightSet`, `ResNameData_AmbLights`, `ResNameData_Lights`, `ResNameData_Fogs` and `ResNameData_Cameras` at `.data` `0x803C5940`–`0x803C59E0` (0x20 each). `sEpsilon` (`.sdata2` `0x804BAB78`, 4 bytes, `1e-36f`) is a descriptive name.
## WUD device-state member names (#504)

[F-504-5](findings/504-wud-device-state-headers.md) names existing WUDDevInfo byte members at offsets 0x5A and 0x5C as signed pinCodeSubmitted and unsigned linkKeyStorage. The first is set immediately before submitting the discovery PIN; the second reconciles controller-only (1), NAND-only (2), both (3), and cleared (0) key storage. These are descriptive names, not recovered original spellings. Member types, offsets, order and 0x60-byte extent stay unchanged. Before-and-after source-object hashes are identical; F-504-5 and the header pull request record the compared baseline.

For `wpse-v1` / `main.dol`, [F-355-1](findings/355-fa-pf-cluster.md) names the eight functions of `pf_cluster.c` (`.text` `0x80200010`–`0x80201200`). VF names without `VFi`, medium confidence: `PFCLUSTER_UpdateLastAccessCluster` (`0x80200010`), `PFCLUSTER_AppendCluster` (`0x802000F0`) and `PFCLUSTER_GetAppendSize` (`0x80200560`). Descriptive, low confidence: `PFCLUSTER_FreeUnusedCluster` (`0x80200320`), `PFCLUSTER_CombineChain` (`0x802006B0`), `PFCLUSTER_DivideChain` (`0x802009F0`), `PFCLUSTER_InsertChain` (`0x80200E50`) and `PFCLUSTER_DeleteChain` (`0x80201050`). Callees named for the link, VF names, medium confidence: `PFENT_UpdateSFNEntry` (`0x802072C0`), `PFENT_getcurrentDateTimeForEnt` (`0x80207500`), `PFFAT_GetSectorAllocated` (`0x8020B2E0`), `PFFAT_GetContinuousSector` (`0x8020B320`), `PFFAT_FreeChain` (`0x8020B740`), `PFFAT_TraceClustersChain` (`0x8020C260`), `PFFAT_ReadValueToSpecifiedCluster` (`0x8020C4C0`), `PFFAT_GetValueOfEOC2` (`0x8020C710`), `PFFILE_Cursor_MoveToEnd` (`0x8020E9A0`), `PFFILE_Cursor_MoveToClusterEnd` (`0x8020E9F0`), `PFSEC_ReadData` (`0x80214F90`) and `PFSEC_WriteData` (`0x80215380`). Descriptive, low confidence: `PFFAT_WriteValueToSpecifiedCluster` (`0x8020C410`) and `PFFAT_AllocateContinuousClusters` (`0x8020C540`). `PF_FFD` (0x38 bytes), `PF_FAT_HINT` (12 bytes), `PF_LAST_ACCESS`, `PF_LAST_CLUSTER`, `PF_CLUSTER_LINK`, `PF_DIR_ENT` (0x240), `PF_SFD` (`ffd` at 4, `dir_entry` at 0x3C), `PF_CURSOR`, `PF_FILE` (`p_sfd` at 8, `hint` at 0xC, `cursor` at 0x1C) and `PF_ENT_ITER` (0x70) are in `include/revolution/fa/pf_volume.h`, with `PF_VOLUME.num_free_clusters` (0x38) and `fsi_flag` (0x1F90).

## NW4R g3d AnmObjVis (#395, part of #396)

For `wpse-v1` / `main.dol`, [F-395-1](findings/395-g3d-anmvis.md) names 22 functions in `.text` `0x80255730`–`0x80255EBC` with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`: `nw4r::g3d::AnmObjVis` (`Release`, `Attach`, `Detach`, `G3dProc`, the destructor, `IsDerivedFrom`, `GetTypeName`, `GetTypeObj`), `AnmObjVisRes` (`Construct`, `SetFrame`, `GetFrame`, `SetUpdateRate`, `GetUpdateRate`, `UpdateFrame`, `Bind`, `GetResult`, `IsDerivedFrom`, `GetTypeName`, `GetTypeObj`, the destructor) and both `ApplyVisAnmResult` overloads. Data: `TYPE_NAME` of `AnmObjVis` (`.rodata` `0x8036C710`) and `AnmObjVisRes` (`0x8036C720`); vtables of `AnmObjVisRes` (`.data` `0x803C59E0`, 0x44) and `AnmObjVis` (`0x803C5A24`, 0x44); the weak `GetAnmPlayPolicy` `policyTable` (`.sdata` `0x804B8958`). Outside names for the link: `G3dObj::~G3dObj` (`0x8025AD00`), `PlayPolicy_Onetime` (`0x8025AE70`), `PlayPolicy_Loop` (`0x8025AE80`), `AnmObj::SetAnmFlag` (`0x8025AEF0`), `TYPE_NAME` of `G3dObj` (`0x8036C838`) and `AnmObj` (`0x8036C848`), and `FrameCtrl::smBaseUpdateRate` (`.sdata` `0x804B8960`). Names are high confidence from CC0 zeldaret/ss, the type-name strings and the exact match.

## NW4R g3d AnmObjMatClr (#396, part of #397)

For `wpse-v1` / `main.dol`, [F-396-1](findings/396-g3d-anmclr.md) names 23 functions in `.text` `0x80255EC0`–`0x80256B3C` with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`: `nw4r::g3d::AnmObjMatClr` (`TestExistence`, `Release`, `Attach`, `Detach`, `DetachAll`, the destructor, `IsDerivedFrom`, `GetTypeName`, `GetTypeObj`), `AnmObjMatClrRes` (`Construct`, `SetFrame`, `GetFrame`, `SetUpdateRate`, `GetUpdateRate`, `UpdateFrame`, `Bind`, `GetResult`, `G3dProc`, `IsDerivedFrom`, `GetTypeName`, `GetTypeObj`, the destructor) and `ApplyClrAnmResult`. Data: `TYPE_NAME` of `AnmObjMatClr` (`.rodata` `0x8036C738`) and `AnmObjMatClrRes` (`0x8036C750`); vtables of `AnmObjMatClrRes` (`.data` `0x803C5A68`, 0x48) and `AnmObjMatClr` (`0x803C5AB0`, 0x48). Names are high confidence from CC0 zeldaret/ss and the exact match.

## Reference symbol import (D-0014)

For `wpse-v1` / `main.dol`, [F-0013](FINDINGS.md#f-0013-reference-symbol-maps-name-library-functions-by-size-alignment) renamed 1,257 library placeholders (1,240 functions, 17 data objects) in `config/wpse-v1/symbols.txt` from public symbol maps. Each name, with address, section, old label, confidence tier, alignment method, run length and sources, is in [reference-symbols.tsv](reference-symbols.tsv); derived source-file spans are in [reference-files.tsv](reference-files.tsv). These names are medium-strength evidence: C++ names carry the reference revision's mangled signature, so rename on contrary evidence from matched source.
For `wpse-v1` / `main.dol`, [F-0013](FINDINGS.md#f-0013-reference-symbol-maps-name-library-functions-by-size-alignment) renamed 1,261 library placeholders (1,244 functions, 17 data objects) in `config/wpse-v1/symbols.txt` from public symbol maps. Each name, with address, section, old label, confidence tier, alignment method, run length and sources, is in [reference-symbols.tsv](reference-symbols.tsv); derived source-file spans are in [reference-files.tsv](reference-files.tsv). These names are medium-strength evidence: C++ names carry the reference revision's mangled signature, so rename on contrary evidence from matched source.
## HBM layout material, panes and window (#295 to #298)

For `wpse-v1` / `main.dol`, [F-295-1](findings/295-nw4hbm-lyt.md#f-295-1-unit-boundaries-from-0x801895e0-to-0x80191874) names the 103 functions from `0x801895E0` to `0x80191874` with the mangled names in CC0 xenoblade's `config/jp/symbols.txt`, matched by order and size: `Material` and its anonymous-namespace helpers `CalcTextureMtx` and `SetIndTexMtx`, the weak `ut::Color` constructor and `detail::GetTextureSize`; `Pane`; `Picture`; `ResourceAccessor`; `TextBox` with `CalcLineRectImpl<w>`, `CalcStringRectImpl<w>` and the weak `ut::Font` destructor; and `Window`. `TextBox::SetFont` (`0x8018F070`) is absent from xenoblade and named from its calls (`Font` virtual destructor, `GetWidth`, `GetHeight`) and the reference source. `Pane::GetColorElement` and `SetColorElement` are `0x24` bytes; dtk's four-byte `fn_8018CC10` and `fn_8018CC40` were their trailing `blr` and are removed. Data: `__vt__` and `__RTTI__` for `Material` (`0x803BB800`, `0x803BB840`), `Picture` (`0x803BB8B0`, `0x803BB948`), `TextBox` (`0x803BB96C`, `0x803BBA10`) and `Window` (`0x803BBA54`, `0x803BBAF8`), `__vt__Q36nw4hbm3lyt4Pane` (`0x803BB848`) and `__vt__Q36nw4hbm3lyt16ResourceAccessor` (`0x803BB950`). The `typeInfo` objects of `Pane`, `Picture`, `TextBox` and `Window` (`.bss` `0x803E9BF8`–`0x803E9C10`) are 4 bytes each; dtk guessed 8. Class layouts are in [F-295-2](findings/295-nw4hbm-lyt.md#f-295-2-class-layouts-and-header-corrections). Confidence high for names, sizes and layouts of the matching units.
### MSL allocation pools (#537)

[F-537-2](findings/537-allocator.md) records all ten allocator functions and exact pool layouts. `Block_subBlock` starts at `0x803422E0`; `malloc` is `0x80342F10`, correcting the earlier tentative `__pool_free` identification, and `free` is `0x80342FB0`. The six `fix_pool_sizes` words start at `0x80398390`. `get_malloc_pool` owns 0x38-byte static storage at `0x804B31F8` and a one-byte initialization flag at `0x804B9BC8`; its internal pool view is 0x34 bytes. All names come from the pinned matching CC0 reference. The required outside callee names `__sys_alloc` (`0x801B1370`) and `__sys_free` (`0x801B13F0`) preserve their existing boundaries and attributes.
### MetroTRK transport and event interfaces (#542)

[F-542-1](findings/542-transport-headers.md) records the complete 0x28-byte communication table at `0x804B3A70` and 0x24-byte event queue at `0x804B3A98`. The queue has count, next index, two 12-byte events and event ID at offsets 0, 4, 8 and 0x20; it has no leading mutex. UART initialization at `0x8034E808` takes three arguments, with the input-pending pointer reference in r5. The existing event type and all offsets remain unchanged. These declarations add no source-byte credit.

Canonical MetroTRK processor and monitor state types for the coordinated #541-#545 interfaces use the target-compiler sizes and native field offsets recorded in [F-541-6](findings/541-processor-state-layout.md). Packet-support and buffer APIs retain the existing 0x88C buffer layout; the verified #543 implementation supplies their declarations. These header prerequisites add no source coverage.
## NW4R g3d AnmObjTexPat (#397)

For `wpse-v1` / `main.dol`, [F-397-1](findings/397-g3d-anmtexpat.md) names 23 functions in `.text` `0x80256B40`–`0x802575DC` with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`: `nw4r::g3d::AnmObjTexPat` (`TestExistence`, `Release`, `Attach`, `Detach`, `DetachAll`, the destructor, `IsDerivedFrom`, `GetTypeName`, `GetTypeObj`), `AnmObjTexPatRes` (`Construct`, `SetFrame`, `GetFrame`, `SetUpdateRate`, `GetUpdateRate`, `UpdateFrame`, `Bind`, `GetResult`, `G3dProc`, `IsDerivedFrom`, `GetTypeName`, `GetTypeObj`, the destructor) and `ApplyTexPatAnmResult`. Data: `TYPE_NAME` of `AnmObjTexPat` (`.rodata` `0x8036C768`) and `AnmObjTexPatRes` (`0x8036C780`); vtables of `AnmObjTexPatRes` (`.data` `0x803C5AF8`, 0x48) and `AnmObjTexPat` (`0x803C5B40`, 0x48). Names are high confidence from CC0 zeldaret/ss and the exact match.

## NW4R Pane and Group interface prerequisite (#462)

[F-462-4](findings/462-pane-group-interface.md) records `Pane` (`0xD8` bytes), `detail::PaneBase` (`0x0C`), `Group` (`0x2C`), and `detail::PaneLink` (`0x0C`) for the free `nw4r::lyt::BindAnimation(Group*, AnimTransform*, bool, bool)` at `0x802B1790`. The utility walks Group's pane list at `0x0C`, reads PaneLink's target at `0x08`, and dispatches Pane's virtual at `0x44`; the idiomatic C++ function matches all native non-relocation instruction words. Header names and remaining virtual declarations follow CC0 Skyward Sword `57a709b2128eb163a18e2969d561c9abb7d1fcee`; field accesses and the called virtual slot are directly confirmed, while unrelated method bodies remain the responsibility of #456 and #457. No source-coverage credit is claimed by the header prerequisite.
## MetroTRK communication and circular buffers (#541)

[F-541-1](findings/541-metrotrk-communication.md) establishes all 17 native functions and their source-selected final addresses. Wii/1.0 and Wii/1.1 both reproduce these bodies; Wii/1.1 is selected without claiming uniqueness.

| Address | Extent | Name | Kind | Confidence | Evidence |
| --- | --- | --- | --- | --- | --- |
| `0x8034D914` | `0x3C` | `gdev_cc_initialize` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034D950` | `0x8` | `gdev_cc_shutdown` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034D958` | `0x24` | `gdev_cc_open` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034D97C` | `0x8` | `gdev_cc_close` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034D984` | `0xB4` | `gdev_cc_read` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DA38` | `0x74` | `gdev_cc_write` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DAAC` | `0x24` | `gdev_cc_pre_continue` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DAD0` | `0x24` | `gdev_cc_post_stop` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DAF4` | `0x74` | `gdev_cc_peek` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DB68` | `0x24` | `gdev_cc_initinterrupts` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DB8C` | `0x4` | `MWInitializeCriticalSection` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DB90` | `0x30` | `MWEnterCriticalSection` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DBC0` | `0x8` | `MWExitCriticalSection` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DBC8` | `0x8` | `CBGetBytesAvailableForRead` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DBD0` | `0x24` | `CircleBufferInitialize` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DBF4` | `0x108` | `CircleBufferWriteBytes` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x8034DCFC` | `0x108` | `CircleBufferReadBytes` | C function | high | F-541-1; reference, calls, exact relocated unit and frozen DOL |
| `0x804B3550` | `0x500` | `gRecvBuf` | static storage | high | F-541-1; native access offsets, compiled layout and final relocation proof |
| `0x804B3A50` | `0x1C` | `gRecvCB` | static storage | high | F-541-1; native access offsets, compiled layout and final relocation proof |
| `0x804B9BF8` | `0x4` | `gIsInitialized` | static storage | high | F-541-1; native access offsets, compiled layout and final relocation proof |

`CircleBuffer` is 0x1C bytes: read/write/start pointers at 0/4/8, size at 0x0C, signed readable count at 0x10, writable count at 0x14 and unsigned-int interrupt state at 0x18. Four following BSS bytes and four following flag bytes are alignment, not members. Names and types follow the pinned reference and established helper interfaces; final storage placement is verified.

## MetroTRK main loop and memory helpers (#541)

[F-541-2](findings/541-main-loop-memory.md) establishes four more source-selected functions. All final addresses, extents and relocated bytes agree with the frozen DOL; 343 earlier source objects remain unchanged.

| Address | Extent | Name | Kind | Confidence | Evidence |
| --- | --- | --- | --- | --- | --- |
| `0x8034DE3C` | `0x3C` | `TRK_main` | C function | high | Original name, reference call order and exact source-linked bytes |
| `0x8034DE78` | `0xEC` | `TRKNubMainLoop` | C function | high | Original name, reference event branches and exact source-linked bytes |
| `0x8034DF64` | `0x13C` | `TRK_memcpy` | C function | high | Paired program-end-trap/event-copy callers, native aligned-word copy and exact source-linked bytes |
| `0x8034E0A0` | `0x128` | `TRK_memset` | C function | high | Reply-buffer callers, native aligned-word fill and exact source-linked bytes |
| `0x804B9C00` | `0x4` | `TRK_mainError` | static error enum | high | Native SDA stores, compiled object and final ELF placement |

`TRKEvent` is twelve bytes: `eventType` at zero, `eventID` at four, `msgBufID` at eight. Its names and canonical event values follow the reference and match the native event-loop branches/accesses. The opaque `TRKBuffer` declaration introduces no guessed storage. `ReadMemoryByte` and `WriteMemoryByte` are chosen descriptive local-helper names; no original helper identity is asserted.

## MetroTRK dispatcher and cache maintenance (#541)

TRKDispatchMessage at 0x8034E1C8 owns 0x120 text bytes and its 0x6C-byte table at 0x803E6270. Its twelve command handlers and TRKSetBufferPosition are named from paired native calls and protocol table values. The proven TRKBuffer is 0x88C bytes, with isInUse at 0, length at 4, position at 8 and 0x880 data bytes at 0xC; this target has no leading mutex word. TRK_flush_cache at 0x8034DE04 is the matching original 0x38-byte assembly routine. See [F-541-3](findings/541-dispatch-cache.md) for source provenance, native layout evidence and separate assembly accounting.

## MSL shared ownership and remaining #541 wrappers

The native `std::tr1::detail::shared_ptr_deleter_common` RTTI name at 0x803997F4, descriptor at 0x804B5AB8 and base vtable at 0x803997E0 identify release at 0x8034D84C and release_weak at 0x8034D8C4. The complete 12-byte class stores ptrdiff_t use_count_ at 4 and weak_count_ at 8; virtual order is destructor, get_deleter and dispose. Both C++ member implementations and their native exception sections match. strncasecmp at 0x8034D83C extends the already owned extras.c; its target is __msl_strnicmp at 0x803450A4. The original name of the 0x8034D840 decimal wcstol wrapper remains unresolved: wide_string_to_decimal is a descriptive chosen name. TRKAccessFile/OpenFile/CloseFile/PositionFile occupy the four consecutive eight-byte assembly trap entries 0x8034D8F4–0x8034D914. See [F-541-5](findings/541-smart-pointer-traps.md) for provenance, original-name confidence, complete section proof and assembly accounting.
For `wpse-v1` / `main.dol`, [F-394-1](findings/394-g3d-resanmscn-transform.md) names 31 functions in `.text` `0x80253C80`–`0x80255724` with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`: `nw4r::g3d::ResLightSet::Bind`; `GetAnmResult` of `ResAnmAmbLight`, `ResAnmLight`, `ResAnmFog`, `ResAnmCamera` and `ResAnmShp`; 21 `ResAnmScn` accessors and `Bind`; and `detail::CalcViewNrmMtx`, `CalcViewTexMtx` and `CalcInvWorldMtx`. Names are high confidence from CC0 zeldaret/ss and the exact match. The `ResAnmScn` accessors absent from ss (`HasResLightSet`, `HasResAnmCamera`, the fog accessors and the `NumEntries` accessors) are medium confidence, named after the ss pattern and placed by their dictionary-name references. Where an `int` and a `u32` index accessor would be byte-identical, the `u32` name is low confidence. The local dictionary names are `ResNameData_LightSet`, `ResNameData_AmbLights`, `ResNameData_Lights`, `ResNameData_Fogs` and `ResNameData_Cameras` at `.data` `0x803C5940`–`0x803C59E0` (0x20 each). `sEpsilon` (`.sdata2` `0x804BAB78`, 4 bytes, `1e-36f`) is a descriptive name.

### MetroTRK transport and event interfaces (#542)

## MetroTRK Dolphin startup (#541, coordinated with #542)

[F-541-7](findings/541-dolphin-completion.md) proves the complete source-selected unit and all 39 paired relocation targets. `InitMetroTRK` at 0x8034E2E8 has corrected extent 0x98, including its original trailing return; `InitMetroTRK_BBA` at 0x8034E380 has extent 0x94. Both are original SDK assembly. The C functions are `EnableMetroTRKInterrupts` at 0x8034E414/4, `TRKTargetTranslate` at 0x8034E418/0x68, `__TRK_copy_vectors` at 0x8034E480/0x134, `TRKInitializeTarget` at 0x8034E5B4/0x48 and `__TRK_reset` at 0x8034E5FC/0x10. Their names have high confidence from the complete pinned reference, native behavior, paired calls and exact final bytes. The ISR table at 0x803E62E0 is 0x3C bytes and `lc_base` at 0x804B9C08 is four bytes. The following padding belongs to neither object. Processor state layouts and reference-name qualifications are recorded separately in [F-541-6](findings/541-processor-state-layout.md).
KPAD follow-up: [F-503-5](findings/503-kpad-reconstruction.md) identifies `KPADSetConnectCallback` (`0x80307DE0`, 0x58 bytes), `KPADDisableDPD` (`0x80307E80`, 0x1C), `KPADEnableDPD` (`0x80307EA0`, 0x1C) and `KPADiControlDpdCallback` (`0x80307EC0`, 0x94). Public names are high confidence from reference headers and behavior; the private completion name follows Petari. The connection callback occupies `KPADInsideStatus + 0x550`; requested, actual and issued DPD states occupy 0x556, 0x557 and 0x558. All four raw non-relocation instruction streams match, but whole-unit relocation/data acceptance remains pending.
KPAD follow-up: [F-503-5](findings/503-kpad-reconstruction.md) identifies `KPADSetConnectCallback` (`0x80307DE0`, 0x58 bytes), `KPADDisableDPD` (`0x80307E80`, 0x1C), `KPADEnableDPD` (`0x80307EA0`, 0x1C) and `KPADiControlDpdCallback` (`0x80307EC0`, 0x94). Public names are high confidence from reference headers and behavior; the private completion name follows Petari. The connection callback occupies `KPADInsideStatus + 0x550`; requested, actual and issued DPD states occupy 0x556, 0x557 and 0x558. All four functions now compare at 100% in the accepted source-linked KPAD unit.

KPAD connection/board follow-up: [F-503-5](findings/503-kpad-reconstruction.md) identifies `KPADiConnectCallback` (`0x80307C70`, 0x164 bytes), `KPADiBalanceControlCallback` (`0x80307E40`, 0x18) and `KPADiBalanceCalibrationCallback` (`0x80307E60`, 0x18), with descriptive medium-confidence private names. The sampling callback at `0x80307F60` is named from the connection reference and is implemented below. Board BSS arrays `sBalanceSampleAverage` (`0x8047C2E0`) and `sBalanceSmoothedWeights` (`0x8047C300`) are four doubles each. Board state: `sBalanceSampleCount` (`0x804B9AF8`, u16), `sBalanceCalibrationState` (`0x804B9AFA`), `sBalanceTgcCalibrating` (`0x804B9AFB`), `sBalanceCalibrationResult` (`0x804B9AFC`), `sBalanceControlSucceeded` (`0x804B9AFD`) and `sBalanceCommandPending` (`0x804B9AFE`), all one byte except the count. Storage sizes and roles follow target accesses; names are descriptive. These three functions and BSS compare at 100%; full source-linked frozen verification passes.

KPAD initialization follow-up: [F-503-5](findings/503-kpad-reconstruction.md) identifies `KPADInit` (`0x80307810`, 0xC bytes), `KPADInitEx` (`0x80307820`, 0x444), `sKpadInitialized` (`0x804B9AE4`, one byte), the version pointer `__KPADVersion` (`0x804B8DF8`), `idist_org` (`0x804B8DFC`), `kp_obj_interval` (`0x804B8E10`) and `kp_fs_revise_deg` (`0x804B8E74`). The eight-byte vector objects `iaccXY_nrm_hori` (`0x804B8E00`), `isec_nrm_hori` (`0x804B8E08`) and `icenter_org` (`0x804B9AD0`) replace their four-byte component aliases within KPAD ownership. Reference names are high confidence; initialized-flag and mode-field names are descriptive. The former `KPADInsideStatus.padding` at 0x55F is now `buttonProcessingMode`, with unchanged type and offset. Both initialization routines compare at 100%; full source-linked frozen verification passes.

## KPAD complete readers and board data (#503)

`read_kpad_stick` (`0x80306800`, 0x8D0), `KPADRead` (`0x803070D0`, 0xC), `KPADiRead` (`0x803070E0`, 0x72C) and `KPADiSamplingCallback` (`0x80307F60`, 0x768) complete the 33-function KPAD unit. Public names follow the SDK; private names describe proven behavior and follow the pinned references with medium naming confidence. [F-503-6](findings/503-kpad-reconstruction.md#f-503-6-complete-readers-sampling-and-source-linked-identity) records complete code comparison and source-linked frozen verification.

Freestyle/classic clamp bounds occupy `0x804B8E44`–`0x804B8E5B`. Train trigger maximum (`kp_tr_trigger_max`, 256) is `0x804B8E64`; guitar stick maximum (`kp_gtr_stick_max`, 1024) is `0x804B8E68`. Their zero minima are `0x804B9ADC`/`0x804B9AE0`, after `kp_stick_clamp_cross` at `0x804B9AD8`. Board settling count `sBalanceSettlingSamples` is the byte 50 at `0x804B8E6C`; `sBalanceAverageWindow` is the float 400.0 at `0x804B8E70`; `sBalanceTgcWeight` is a double at `0x804B9AF0`. Board names are descriptive rather than recovered. The default gravity initializer at `0x804BB5C0` is one six-byte, two-byte-aligned object. Five pool correspondence labels (`@2735`–`@2738`, `@2811`) are compiler-generated reconstruction labels backed by identical four-byte initializers, not claims of recovered original names.
## Issue #543 MetroTRK initialization and support

[F-543-1](findings/543-metrotrk-initialization-support.md) records 34 native function names and exact sizes, all eight source units, the version 0.4 request/send differences, and the complete no-mutex 0x88C-byte `TRKBuffer` layout. The original issue owns 25 functions / 3,576 bytes; nine later message-buffer functions belong to #544. Four owned storage objects have exact final addresses and sizes: `gTRKBigEndian` (`0x804B9C18`, 4), `gTRKInputPendingPtr` (`0x804B9C20`, 4), `sSequence` (`0x804B9C28`, 2), and `gTRKMsgBufs` (`0x804B3AC0`, 0x19A4). Whole-unit comparison and native final addresses validate the interfaces. The two original processor-register assembly routines contribute 808 source assembly bytes, separately from C/C++ decompilation.
## Issue #515 HID-host reconstruction

[F-515-1](findings/515-hid-host.md) records all 27 owned HID-host function names, exact addresses/sizes, the complete action/API/main unit boundaries, compiler evidence, and the 0x230-byte `bta_hh_cb` layout. It also identifies two #516 helpers in the same main file and the two preceding power callbacks delivered in PR #690. Every compared section of all three HID objects is 100%. New headers retain original SDK names and complete message/device/control-block layouts; callee names outside the block are established by exact call relocations without changing their sizes or splits.
## F-462-3: NW4R layout utilities

The thirteen functions in 0x802B0C30–0x802B1790 recover original nw4r::lyt::detail resource-name, header, TexCoordAry, vertex-format, alpha, and quad-drawing names. TexCoordAry is eight bytes (capacity0, size1, data pointer4); each four-vector coordinate set is32bytes. The native local coordinate array and guard occupy0x804524F0/32bytes and0x804B9438/1byte. The referenced Layout allocator at0x804B9410 is named without changing its outside-owned size/type/split. See [F-462-3](findings/462-lyt-common.md) for exact inventory and proof.

## NW4R resource accessors (#461, #462)

[F-462-5](findings/462-arc-resource-accessor.md) recovers ResourceAccessor (four bytes), FontRefLink (`0x8C`), ArcResourceLink (`0xA4`), and MultiArcResourceAccessor (`0x1C`) and the fifteen native functions at `0x802B0680–0x802B0C30`. Names are high confidence from CC0 Skyward Sword and exact source matches. ResourceAccessor and MultiArcResourceAccessor vtables occupy twenty bytes at `0x803C7250` and `0x803C7268`; their trailing alignment padding remains outside the split. ArcResourceLink stores ARCHandle at `0x08` and its 128-byte resource-root directory at `0x24`. MultiArcResourceAccessor stores the archive and font lists at `0x04` and `0x10`; FontRefLink stores its 128-byte name at `0x08` and font at `0x88`. Anonymous archive helpers and the directory strings are local to the source unit; they do not replace the similarly named HBM helpers.

## NW4R group animation utility (#462)

[F-462-6](findings/462-group-animation-utility.md) identifies the free `nw4r::lyt::BindAnimation(Group*, AnimTransform*, bool, bool)` at `0x802B1790`, 112 bytes, as the only linked function in `lyt_util.cpp`. It traverses Group's pane list at `0x0C` and calls Pane's virtual at `0x44`. The CC0 Skyward Sword source and exact full source-linked DOL establish high confidence. Together with F-462-3 and F-462-5, all 24 native issue functions / 3,720 code bytes are reconstructed; the shared resource-accessor units add five approved #461 functions / 612 bytes.
## NW4R ef handle, emitter form and creation queue (#380)

For `wpse-v1` / `main.dol`, [F-380-1](findings/380-ef-handle-emitter-forms.md) names, with mangled CodeWarrior names and high confidence from the CC0 Whippy reference and exact matches: `EmitterForm::CalcVelocity` (`0x80236370`), `EmitterForm::CalcLife` (`0x80236830`), `CreationQueue::AddParticleCreation` (`0x80236920`), `CreationQueue::AddEmitterCreation` (`0x80236AA0`), `EmitFormBuilder::Create` (`0x80236D60`), `__sinit_\ef_emform_cpp` (`0x80236DE0`), and the `Emission` overrides of `EmitterFormPoint` (`0x80236E40`), `EmitterFormLine` (`0x80237190`), `EmitterFormDisc` (`0x802374D0`) and `EmitterFormSphere` (`0x80237A10`). Data: the seven emitter-form vtables (`.data` `0x803C55E8` to `0x803C5648`, 0x10 each, in the order point, line, disc, sphere, cylinder, torus, cube) and the seven file-local form instances `gEmitterFormDisc` to `gEmitterFormPoint` (`.sbss` `0x804B92C8–0x804B92E4`, 4 bytes each, in the reference declaration order).
## NW4R g3d AnmScn (#401, part of #402)

For `wpse-v1` / `main.dol`, [F-401-1](findings/401-g3d-anmscn.md) names 42 functions in `.text` `0x80259780`–`0x8025ACFC` with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`: the local `MakeDiffuseLightObj` (`@unnamed@g3d_anmscn_cpp@`), `nw4r::g3d::AnmScn` (`GetLightSetting`, `Attach`, `Detach`, the destructor, `IsDerivedFrom`, `GetTypeName`, `GetTypeObj`) and `AnmScnRes` (`GetNum*` for light sets, ambient, diffuse and specular lights, fogs and cameras; `Get*MaxRefNumber` for ambient and diffuse lights, fogs and cameras; `Construct`, constructor, destructor, `SetFrame`, `GetFrame`, `SetUpdateRate`, `GetUpdateRate`, `UpdateFrame`, `G3dProc`, `GetLightSet`, `GetAmbLightColor`, `GetLight`, `GetFog`, `GetCamera`, the four `Get*Result` functions, `GetSpecularLightID`, `HasSpecularLight`, `UpdateCache`, `IsDerivedFrom`, `GetTypeName`, `GetTypeObj`). Outside the block, the weak `AnmScnRes::GetLightSetMaxRefNumber` is the game-unit copy at `0x8005ED80` (F-401-2). Data: `TYPE_NAME` of `AnmScn` (`.rodata` `0x8036C818`) and `AnmScnRes` (`0x8036C828`); vtables of `AnmScnRes` (`.data` `0x803C5D68`, 0x90) and `AnmScn` (`0x803C5DF8`, 0x90). Names are high confidence from the CC0 Whippy and ss sources, vtable order and the exact match.
## EUART (#504)

[F-504-1](findings/504-euart-usb-wud.md) identifies `EUARTInit` at `0x803086D0` (0xFC), alongside existing `InitializeUART` at `0x803087D0` (0x48) and `WriteUARTN` at `0x80308820` (0x230). Names follow the CC0 SDK reference and proven behavior. The four four-byte states at `0x804B9B08`, `0x804B9B0C`, `0x804B9B10` and `0x804B9B14` are `__EUARTInitialized`, `__EUARTLastErrorCode`, `__EUARTSendStop` and descriptive `__EUARTEnabled`, distinct from EXIUart.c's `Enabled`. The external four-byte frequency constant at `0x804BB288` receives reference name `__EXIFreq` without any split, extent or storage-type change. Text and sbss compare at 100%, and the source-linked DOL passes frozen verification.

## USB transfer unit (#504)

[F-504-2](findings/504-euart-usb-wud.md#f-504-2-usb-translation-unit-and-pooled-string-residual) names `USB_LOG` (`0x80308A50`), `USB_ERR` (`0x80308B00`), `_intrBlkCtrlIsoCb` (`0x80308CC0`), `__LongBlkMsgInt` (`0x803090E0`), `__IntrBlkMsgInt` (`0x80309440`) and `__CtrlMsgInt` (`0x80309960`). Other retained USB public names were already established by callers. Names follow pinned SDK references; private spelling confidence is medium. Behavior and sizes follow target comparison. The private request layout is documented in F-504-2.

`sUsbHeapId` at `0x804B8E78` names this unit's initialized four-byte heap ID, distinct from ISFS's `hId`. `s_usb_err` is the byte at `0x804B8E7C`; `lo` and `hi` are pointers at `0x804B9B18` and `0x804B9B1C`; `s_usb_log` is the single byte at `0x804B9B20`. The 0x830-byte compiler pool at `0x803DE888` is now described by 66 proven literal objects, replacing the earlier coarse `usbMessagePool` label. Their names are compiler-produced reconstruction correspondence labels, not recovered original names. All initializers, addresses and sizes are verified. The small-data USB prefix at `0x804B8E80` ends before WUD's format string at `0x804B8E88`. [F-504-3](findings/504-euart-usb-wud.md#f-504-3-complete-usb-source-and-exact-linked-identity) records complete 14-function code matching and exact source-linked frozen verification. The two source-only message-pointer tables are absent from the final ELF and are not claimed as target globals.

## WUD first source and message-proven names (#504)

Current acceptance is [F-504-16](findings/504-wud-importer-match.md): all 86 native functions and 1,185 relocations are exact, the normal link selects WUD source, and frozen verification passes. Earlier partial-source checkpoints below preserve the naming evidence and reconstruction history.

[F-504-4](findings/504-wud-reconstruction.md) records the complete 86-function WUD unit and its first 56 implementations. Embedded target messages establish `__wudSyncFlushCallback` (`0x8030A050`), `__wudDeleteFlushCallback` (`0x8030B6D0`), `__wudShutdownFlushCallback` (`0x8030CD70`), `__wudInitSub` (`0x8030E270`) and `__wudCleanupStackCallback` (`0x8030F140`). Their non-relocation instruction words match; whole-unit acceptance remains pending. Reference-based `WUDiTerminateDevice` is `0x8030B730`; other reconstructed public names follow the existing SDK interfaces and the table in F-504-4.

Relocated callback operands and target message spelling name `__wudDeviceStatusEventStackCallback` at `0x8030FBA0` and `__wudPowerMangeEventStackCallback` at `0x8030FF50`; preserve the original `Mange` spelling. Reference-based, medium-confidence names identify `WUDVendorSpecificCallback` (`0x8030F8F0`) and `WUDiRegisterDevice` (`0x8030E3E0`). The four-byte globals `sWudInitialized` (`0x804B9B28`), `sWudReadNand` (`0x804B9B2C`) and `_linkedWBC` (`0x804B9B40`) follow paired target SDA accesses. The existing `_wcb` remains at `0x8047C320` with size 0x750. Read-only data at `0x804BB5D8`/`0x804BB5DC` contains `Wii` and the three Bluetooth class bytes. No WUD source is selected for the final link yet.

The eight tail implementations in F-504-4 establish the working device `_work` (`0x8047CB78`, 0x60 bytes), settings `_scArray` (`0x8047CBD8`, 0x461 bytes), handle pointer array `_dev_handle_to_bda` (`0x8047D0E0`, 0x40 bytes), two 0x20-byte count arrays `_dev_handle_queue_size` (`0x8047E488`) and `_dev_handle_notack_num` (`0x8047E4A8`), and descriptive byte flag `sWudSettingsDirty` (`0x804B9B5C`). Addresses follow paired target HA/LO or SDA relocations; typed extents follow the existing SDK layouts and indexed accesses. The former settings extent included seven padding bytes; the former pointer extent also contained a separate, unreconstructed 0x160-byte object. Reference spelling confidence is medium for private data names. WUD remains NonMatching.

The implemented initialization-flush callback `__wudInitFlushCallback` at `0x8030BFD0` (0x4C bytes) follows its embedded target message spelling. F-504-4 also records the implemented device-status callback and `WUDIsBusy`. Target instructions prove initialization flush-complete state 5 and ready state 6; the older shared initialization enumeration has not yet been updated. These bodies reproduce all non-relocation words; full WUD acceptance remains pending.

The synchronization source in F-504-4 identifies private `__wudStartSyncDevice` (`0x8030D730`, 0x14C) and referenced `__wudSyncHandler0` (`0x8030B6B0`, body not implemented yet) from the pinned reference and target callback operands, medium spelling confidence. Paired SDA operands establish byte `sWudSyncTarget` (`0x804B9B5D`) and four-byte `sWudAbortSync` (`0x804B9B30`), descriptive names with high behavioral confidence. The channel exclusion call names outside `BTM_SetAfhChannels` at `0x8031C840` without changing its split, type or size. The signed-byte channel/count public declarations remain unchanged; explicit unsigned-byte release checks reproduce the original instructions. WUD remains NonMatching.

F-504-4 adds the reconstructed security callback at 0x8030F180 (0x554 bytes), reference name __wudSecurityEventStackCallback, medium spelling confidence. Its original log says __wudCleanupStackCallback: and is preserved. The callback is not matching: 32 native instruction differences remain around pooled addresses and register allocation. The referenced __wudEnableStackHandler0 at 0x8030BFB0 is named from the reference, with its body pending. Outside BTA_HhEnable at 0x803194B0 receives its SDK name without a type, extent or split change. Header prerequisite #720 names the device fields before this dependent source.

[F-504-6](findings/504-wud-hid-registration.md) identifies the native 217-byte HID descriptor at 0x803DF0B8 as reference-named _wudWiiRemoteDescriptor, medium spelling confidence. Its complete initializer matches. The old coarse extent is separated into the descriptor, three padding bytes and the remaining 0x140-byte record at 0x803DF194. WUDiRegisterDevice (0x8030E3E0, 0x154) and WUDiRemoveDevice (0x8030E540, 0x180) are reconstructed at exact sizes with four/eight literal-address differences; every differing address resolves to identical text. Both retain the target's linked Balance Board condition. Their outside calls name BTA_HhAddDev (0x80319718) and BTA_HhRemoveDev (0x803197C0) without changing extents, types or splits. WUD remains NonMatching.

[F-504-7](findings/504-wud-search-power.md) adds exact-size __wudSearchEventStackCallback (0x8030F6E0, 0x208) and __wudPowerMangeEventStackCallback (0x8030FF50, 0x12C), with seven/five folded-address differences to identical data. Search spelling follows the reference with medium confidence; preserve the target-proven Mange typo. Paired relocations identify descriptive discovery response sWudDiscoveryResponse (0x8047CA70, 0x108 bytes), signed-byte sWudDiscoveryRssi (0x804B9B5E) and byte sWudDiscoveryResultCount (0x804B9B5F). Reference-named _wudResetAuthCountCmd (0x803DF2B8, 0x1C bytes) has an exact native initializer. The outside call names BTM_VendorSpecificCommand at 0x8031D6DC without changing its extent, type or split. WUD remains NonMatching.

[F-504-8](findings/504-wud-sync-entry.md) adds reference-named __wudSyncPrepareSearch (0x8030A0D0, 0x158) with exact non-relocation words and __wudSyncTryConnect (0x8030A230, 0x170) with five folded addresses to identical literals. Private spelling confidence is medium. The latter preserves target-proven Balance Board replacement behavior and names outside BTA_HhOpen at 0x803195D4 without changing its extent, type or split. Real source restores native BSS offsets 0/0x750/0x858/0x8B8 for _wcb/discovery/work/settings, matching the target. Security now has 22 folded addresses to identical text, superseding its earlier 32-difference register/BSS diagnostic. WUD remains NonMatching.

[F-504-9](findings/504-wud-standard-pairing.md) names __wudSyncVirginStandard (0x8030A3A0, 0x378) from the reference, medium spelling confidence, and reconstructs its exact-size body with four folded literal-address differences. Private WUDWiiFitConfigProfile and WUDWiiFitPairingInfo declarations describe the verified settings-key offset 0x14 and 160-byte pairing workspace. Descriptive sWudWiiFitPairingInfo is at 0x8047D040; sWudWiiFitChecksumSeed is the proven word at 0x804B9B44. The handle array follows at 0x8047D0E0, with both native BSS positions exact. Reference-named callee WUDiMoveTopOfUnusedStdDevice at 0x8030EF20 keeps its original extent/type and awaits reconstruction. No WUD source is selected for the final link.

[F-504-10](findings/504-wud-settings-callbacks.md) adds exact non-relocation streams for __wudSyncStoredDevInfoToNand (0x8030A720/0x134), __wudOpenWiiFitCallback (0x8030A860/0x7C), __wudSeekWiiFitCallback (0x8030A8E0/0x84), __wudUpdateWiiFitCallback (0x8030A970/0x7C), __wudCloseWiiFitCallback (0x8030A9F0/0x54) and __wudSyncDone (0x8030AA50/0x168). Private spellings follow the reference with medium confidence. Paired SDA access identifies descriptive byte sWudNandLocked at 0x804B9B4C. Private states 100/101/102/103 and seek/write results 0x4AF18/132 follow the target; existing product-region code maps 0 to JP. The six functions have 80 checked call/data relocation pairs. WUD remains NonMatching.

F-504-11: `__wudSyncHandler` 0x8030ABC0/0xAE4 and `__wudSyncHandler0` 0x8030B6B0/0x20 retain native extents. NANDFileInfo 0x8047D120/0x8C, NANDCommandBlock 0x8047D1AC/0xBC, handler stack 0x8047D280/0x1000 follow paired operands and compiled canonical layouts. Private storage names are descriptive; callback `__wudLinkKeyEventStackCallback` 0x8030FBF0 and outside `BTM_WriteStoredLinkKey` 0x8031DA8C are reference-derived and target-corroborated. See [sync dispatch evidence](findings/504-wud-sync-dispatch.md). WUD remains NonMatching.

F-504-12: lifecycle native extents and names are recorded in [deletion/stack/shutdown evidence](findings/504-wud-lifecycle.md). `_spArray` 0x8047E280/0x205 is canonical SCBtCmpDevInfoArray, followed by three padding bytes and the two 0x20-byte handle count arrays. Compiled whole WUD BSS matches. Private function names are reference-derived and target-corroborated; 66 bodies remain NonMatching pending full acceptance.

F-504-13: three remaining device-list operations and vendor/stored-key callbacks retain original extents; 94 paired relocation entries agree. Panic operands prove WUD.c lines 5342/5498. No new outside symbols or shared layouts change. See [list/event evidence](findings/504-wud-list-events.md). WUD remains NonMatching with 71 native bodies.

F-504-14: six firmware-patch native extents, six small-data names and three initialized protocol arrays are recorded in [firmware transport evidence](findings/504-wud-firmware-patch.md). F-504-15 corrects the raw-split interpretation at patch-record 0x5C-0x5F: frozen/reference bytes are 80 01 01 38; raw split zeros were relocation placeholders. WUD remains NonMatching with 77 bodies; 66 raw streams are exact and eleven partial bodies retain 69 identical-data address differences.

F-504-15: all 86 WUD native extents are represented. Eighty-five instruction streams and 1,128 paired relocation entries are exact; importer 0x8030C020/0x638 retained 82 differences before F-504-16. Trace35, buffer38, weak hook3C and NAND phase48 are named within small BSS. Full data bytes and storage positions are recorded in [complete initialization evidence](findings/504-wud-full-initialization.md). WUD remains NonMatching; no linked source credit.

F-504-15 candidate: all final DOL bytes outside importer 0x8030C020/0x638 agree; 116 bytes / 87 final instruction words differed inside it before F-504-16. Both catalogs and standalone clear-device source body are stripped, all native addresses match. The inferred @etb_80010138 relocation explains the raw controller-word placeholder; see the corrected F-504-14/F-504-15 evidence.

F-504-16: the companion-tail access spelling in the allocator resolves the final importer mismatch. Normal source selection reproduces the complete frozen DOL, all 86 WUD addresses and data/storage bytes, with both catalogs and the standalone clear-device body stripped. Earlier NonMatching checkpoints are superseded by this exact full-artifact proof.

## NW4R g3d object, animation-object and platform units (#402)

For `wpse-v1` / `main.dol`, [F-402-1](findings/402-g3d-obj-anmobj-platform.md) names 19 functions in `.text` `0x8025AD00`–`0x8025B694` with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`: `nw4r::g3d::G3dObj` (`Destroy`, `DetachFromParent`, `GetTypeName`, `GetTypeObj`, beside the already named destructor), `PlayPolicy_Onetime`, `PlayPolicy_Loop`, `AnmObj::SetAnmFlag` and `TestAnmFlag`, the seven `fifo` functions, and `detail::Copy32ByteBlocks`, `ZeroMemory32ByteBlocks` (`0x8025B540`) and `ZeroMemory16ByteBlocks`. Outside the block, the weak `G3dObj::IsDerivedFrom` is the game-unit copy at `0x80053820`. Data: `G3dObj` vtable (`.data` `0x803C5E88`, 0x1C) and the local `fifo::cm2hw` (`.sdata2` `0x804BAC18`); `smBaseUpdateRate` is resized to 0x4. Names are high confidence from CC0 zeldaret/ss and Whippy, the vtable and the exact match.
## NW4R texture-transform animation (#398, part of #399)

[F-398-1](findings/398-g3d-anmtexsrt.md) records all 24 retained functions in `g3d_anmtexsrt.cpp` (`0x802575E0`–`0x802580CC`): the base and resource implementations of `AnmObjTexSrt`, both `ApplyTexSrtAnmResult` overloads and the corresponding RTTI methods. Names are high confidence from pinned CC0 ss and exact native instruction/relocation comparisons. The base class has binding count at 0x10, binding pointer at 0x14 and size 0x18; the resource class adds FrameCtrl at 0x18, resource at 0x2C and cache pointer at 0x30, size 0x34. The native type-name records at `0x8036C798` and `0x8036C7B0` are 20 and 24 bytes, respectively; four following bytes align the first record. Resource/base vtables at `0x803C5B88`/`0x803C5BD0` each contain 0x48 bytes in the declared virtual order. `CalcTexMtx` at `0x80261D10` receives only its evidenced four-argument mangled name; its extent/type/ownership remain unchanged.

## NW4R ef cylinder, torus and cube emitter forms (#381)

For `wpse-v1` / `main.dol`, [F-381-1](findings/381-ef-emitter-forms-draw-strategies.md) names, with mangled CodeWarrior names and high confidence from the CC0 Whippy reference and exact matches: `EmitterFormCylinder::EmissionSub` (`0x80238270`), `EmitterFormCylinder::Emission` (`0x802386D0`), `EmitterFormTorus::Emission` (`0x80238A20`), `EmitterFormCube::EmissionSub` (`0x80239060`) and `EmitterFormCube::Emission` (`0x80239390`).
[F-542-2](findings/542-transport-event-source.md) names `EnableEXI2Interrupts` (`0x8034E85C`), `TRKPollUART` (`0x8034E888`), `TRKReadUARTN` (`0x8034E89C`), `TRKWriteUARTN` (`0x8034E8D8`), `TRKDoNotifyStopped` (`0x8034E9A8`), `TRKPostEvent` (`0x8034EAE8`) and `TRKConstructEvent` (`0x8034EBB4`) from the pinned Petari implementation and exact native code. `gTRKEventQueue` at `0x804B3A98` is 0x24 bytes; the former 0x28-byte estimate included padding. The compiler-generated weak local trap constant `@LOCAL@InitializeProgramEndTrap__Fv@EndofProgramInstruction` at `0x804BBBF8` is four bytes containing `0x00454E44`. These names and storage extents have high confidence from the actual source objects and frozen DOL.

## NW4R g3d Draw1Mat1Shp (#775)

For `wpse-v1` / `main.dol`, [F-775-1](findings/775-g3d-draw1mat1shp.md) confirms the imported names of `nw4r::g3d::detail::LoadMaterial` (`.text` `0x8025ECB0`) and `nw4r::g3d::Draw1Mat1ShpDirectly` (`0x8025F150`) by exact match against CC0 zeldaret/ss. No symbols were renamed. Confidence: high.
## Issue #544 MetroTRK handlers and console

[F-544-2](findings/544-metrotrk-handlers-console.md) records all 25 original issue functions / 5,896 bytes across three complete source units. The #543-owned message-buffer unit contributes nine functions without duplicate ledger credit. The two new units contain 18 retained functions / 4,368 bytes, including two #545 console helpers. Handler sequence state is a four-byte local at 0x804B9C30 and connection state a four-byte local at 0x804B9C34. Source object sections, final native function addresses, actual source selection and complete frozen DOL identity are verified. [F-544-1](findings/544-metrotrk-protocol-headers.md) records the shared 64-byte reply layout: length 0, command 4, reply error 8, sequence/file-position parameter 12, payload 16. Outside callee names follow paired source relocations and native branch destinations; their sizes, types and splits remain unchanged.
For `wpse-v1` / `main.dol`, [F-355-3](findings/355-fa-pf-dir.md) names the first twelve functions of `pf_dir.c` (`.text` `0x80201200`–`0x80205CC8`). VF names without `VFi`, medium confidence: `PFDIR_IsEmptyDirectory` (`0x80201200`), `PFDIR_GetSDD` (`0x80201320`), `PFDIR_p_opendir` (`0x80201530`), `PFDIR_p_readdir` (`0x80201820`), `PFDIR_p_chdir` (`0x80202220`), `PFDIR_p_chmod` (`0x802024A0`), `PFDIR_p_rmdir` (`0x802026E0`), `PFDIR_p_fstat` (`0x80202A10`), `PFDIR_p_fsnext` (`0x80203F30`) and `PFDIR_p_fsfirst` (`0x80204240`). Descriptive, low confidence: `PFDIR_OpenDirEntry` (`0x80201430`) and `PFDIR_ChmodEntry` (`0x80204350`). Callees named for the link, VF names, medium confidence: `PFENT_findEntry` (`0x80206B10`), `PFENT_GetRootDir` (`0x80207110`), `PFENT_RemoveEntry` (`0x80207590`), `PFENT_LookupPath` (`0x80207FC0`), `PFENT_GetEntryOfPath` (`0x80209420`), `PFENT_GetParentEntryOfPath` (`0x80209430`), `PFENT_ITER_IteratorInitialize` (`0x80208520`), `PFENT_ITER_IsAtLogicalEnd` (`0x80208620`), `PFENT_ITER_MoveTo` (`0x80208730`), `PFENT_ITER_Advance` (`0x80208830`), `PFFAT_InitHint` (`0x8020C240`), `PFFAT_ResetFFD` (`0x8020C670`), `PFFAT_InitFFD` (`0x8020C6B0`), `PFFAT_FinalizeFFD` (`0x8020C700`), `PFFILE_IsOpened` (`0x80210840`), `PFPATH_DoSplitPath` (`0x80211FE0`), `PFPATH_SplitPath` (`0x802132F0`), `PFPATH_SplitPathWildcard` (`0x80213300`), `PFPATH_GetVolumeFromPath` (`0x80213310`), `VFiPFPATH_transformFromUnicodeToNormal` (`0x80213D40`), `VFiPFPATH_transformInUnicode` (`0x80213E70`), `PFPATH_CopyPattern` (`0x80214840`), `PFVOL_CheckForRead` (`0x80216000`), `PFVOL_IsCurrentDirCluster` (`0x802163F0`), `PFVOL_SetCurrentDir` (`0x802164D0`) and `PFVOL_GetCurrentDir` (`0x802166B0`). `PF_SDD` (0x280), `PF_DIR` (0x20) and the `PF_VOLUME` error and configuration fields are in `include/revolution/fa/pf_volume.h`; `PF_DIRENT`, `PF_STAT` and `PF_DTA` are in `pf_dir.h`; `PF_VOLUME_SET.last_error` (0x40) and `setting` (0x60) are in `pf_str.h`.
## Game BController (#735)

For `wpse-v1` / `main.dol`, [F-735-1](findings/735-game-compiler.md) reconstructs `game/BController.cpp`, `.text` `0x80052500`–`0x800528D8`. Class names `BController`, `IController` and `IUIInput` and the hierarchy `BController : IController : IUIInput` are original, from RTTI at `0x804B5A20`, `0x804B5A28` and `0x804B5A30` (`.sdata`, 8 bytes each). Every member and method name is descriptive, chosen from behavior and from callers in the `CController*` classes; the original spellings are unknown. The header is `include/game/BController.h`, with the interfaces in `include/game/IController.h` and `include/game/IUIInput.h`.

`BController` is at least 0x34 bytes: vtable pointer at 0, `mChannel` (`s32`) at 4, `mConnected` (`bool`) at 8, `mLostCount` (`s32`) at 0xC, `mStickLength` and `mStickAngle` (`f32`, angle in degrees) at 0x10 and 0x14, `mHold`, `mTrig`, `mRelease`, `mRepeat` and `mRepeatButton` (`u32`) at 0x18–0x28, `mRepeatWait` (`s32`) at 0x2C, and `mDisconnectSerial` (`u32`) at 0x30, which the constructor leaves uninitialized. Derived classes may extend it.

The vtable `__vt__11BController` (`.data` `0x803993B8`, 0x40 bytes) has fourteen slots after the RTTI pointer and offset: 0 `IsConnected` (`0x800528C0`), 1 `GetStickLength` (`0x800528B0`), 2 `GetStickAngle` (`0x800528A0`), 3 `GetHold` (`0x80052890`), 4 `GetTrig` (`0x80052880`), 5 `GetRelease` (`0x80052870`), 6 `GetRepeat` (`0x80052860`), 7 `GetType` (pure; `CControllerGCPad` 0, `CControllerWiiRemote` 1, `CControllerClassic` 2, `CControllerNunchaku` 3, `CControllerComposition` 4), 8 `GetChannel` (`0x800528D0`), 9 `StartMotor(const MotorPattern&)` (`0x80052840`, empty), 10 `StopMotor` (`0x80052830`, empty), 11 `GetButtonSymbol(u32)` (`0x80052820`, returns `L""` at `.sdata` `0x804B5A38`), 12 `GetDisconnectSerial` (`0x80052850`), 13 `Update` (pure). The boundary between the `IUIInput` and `IController` slots (7) is a guess; `MotorPattern` is an opaque forward declaration whose member at 8 the derived classes pass on. Non-virtual methods: `BController()` (`0x80052500`), `IsLost(bool)` (`0x80052550`), `SetButton(u32)` (`0x800525A0`), `SetDisconnected()` (`0x800525B0`), `SetStickFromCross()` (`0x800525F0`, jump table `0x80399380`), `SetStick(const nw4r::math::VEC2&)` (`0x800526D0`) and `UpdateButton(u32)` (`0x80052790`). The cross bits are up 1, right 2, down 4, left 8.

## NW4R g3d CalcView (#776)

For `wpse-v1` / `main.dol`, [F-776-1](findings/776-g3d-calcview.md) confirms the 11 imported function names in `.text` `0x8025FA00`–`0x80261C7C` (the eight `@unnamed@g3d_calcview_cpp@` helpers, `CalcView`, `CalcView_LC`, `CalcView_LC_DMA_ModelMtx`) by exact match against CC0 zeldaret/ss. Data: the local `bbFunc` table (`.rodata` `0x8036C898`, 0x1C) and `norm_epsilon` (`.sdata2` `0x804BAC88`, 0x4). Outside the unit, `detail::workmem::GetBillboardMtxTemporary` (`0x80261DB0`). Confidence: high.
## NW4R ef draw strategy builder and DrawStrategyImpl (#381)

For `wpse-v1` / `main.dol`, [F-381-2](findings/381-ef-emitter-forms-draw-strategies.md#f-381-2-nw4r-ef-draw-strategy-builder-and-drawstrategyimpl) names, with mangled CodeWarrior names and high confidence from the CC0 Whippy reference and exact matches: `DrawStrategyBuilder::Create` (`0x8023AC40`), its seven function-local strategy instances (`.bss` `0x8042F2F8–0x8042F918`, 0xE0 each, as `@LOCAL@Create__Q34nw4r2ef19DrawStrategyBuilderFUl@<name>`) and their guard bytes (`.sbss` `0x804B92E8–0x804B92EE`, `@GUARD@...`); `DrawStrategyImpl::DrawStrategyImpl` (`0x8023ADE0`), `GetGetFirstDrawParticleFunc` (`0x8023C9D0`), `GetGetNextDrawParticleFunc` (`0x8023C9F0`), `~DrawStrategyImpl` (`0x8023CD30`), `__sinit_\ef_drawstrategyimpl_cpp` (`0x8023CD70`), `__vt__Q34nw4r2ef16DrawStrategyImpl` (`.data` `0x803C5668`, 0x18) and the static constants `mXUnitVec` (`0x8042F918`), `mYUnitVec` (`0x8042F924`), `mZUnitVec` (`0x8042F930`), `mZeroVec` (`0x8042F93C`) and `mIdentityMtx` (`0x8042F948`, 0x30). Strategy constructors named for the link: `DrawBillboardStrategy` (`0x8023CE00`), `DrawDirectionalStrategy` (`0x8023EFA0`), `DrawFreeStrategy` (`0x80241320`), `DrawLineStrategy` (`0x80241D50`), `DrawPointStrategy` (`0x80242380`) and `DrawSmoothStripeStrategy` (`0x80245EF0`), from the builder call order. `DrawStrategyImpl` is 0xE0 bytes. `EmitterDrawSetting::DIR_7` keeps the reference placeholder name (low confidence).
## MetroTRK target access and serial-I/O option (#545)

For `wpse-v1` / `main.dol`, [F-545-1](findings/545-metrotrk-target.md) records 36 functions in `0x80351188`–`0x803529BC`, named from pinned Petari SDK source except `GetThreadInfo` at `0x8035291C`, a private function whose spelling agrees with the high-confidence F-0013 two-game seed and pinned xenoblade source. Its imported `OSThread.linkActive.next` access is at `0x2FC`; active-thread head/current-thread pointers are at `0x800000DC` / `0x800000E4`. `ConvertAddress` at `0x80352914` retains the reference name. `SetUseSerialIO` / `GetUseSerialIO` use `bUseSerialIO` at `.sbss` `0x804B9C40`, one byte.

`TRKStepStatus` is 24 bytes with eight-byte type alignment: `active` (`BOOL`) at 0, `type` (`DSMessageStepOptions`) at 4, `count` (`u32`) at 8, `rangeStart` (`u32`) at `0xC`, and `rangeEnd` (`u32`) at `0x10`. `gTRKStepStatus` is `.bss` `0x804B5478`. The nine-byte `gTRKRestoreFlags` at `0x804B5468` remains an opaque array view; indices 0 and 1 are the proven time-base and decrementer restoration flags, and no original field names or meanings are claimed for indices 2–8. `gTRKSaveState` (`Default_PPC`, `0x94`) is at `0x804B5490`; the 16-byte `TRKvalue128_temp` is at `0x804B5528`. CPU/debugger-state types reuse the first-owner canonical header. The initialized `TRKExceptionStatus` at `.data` `0x803E6548` is 16 bytes: `exceptionInfo.PC` at 0, `PCInstruction` at 4, `exceptionID` (`u16`) at 8, `inTRK` (`u8`) at `0xC`, and `exceptionDetected` (`u8`) at `0xD`. `TRK_saved_exceptionID` is a `u16` at `.sbss` `0x804B9C38`.

The 64-byte `CommandReply` retains its existing layout; `parameter.instruction` names the proven `u32` at offset `0xC` in stopped and exception notifications. Existing `sequence` and `filePosition` union names remain available. SPR/FPR access templates are runtime debugger instruction encoders, with native relocation and initialized-data evidence, not binary reconstruction fallbacks. Original assembly is accounted separately from C/C++ decompilation.

## NW4R g3d Camera, basic and Maya (#779)

For `wpse-v1` / `main.dol`, [F-779-1](findings/779-g3d-camera-basic-maya.md) confirms the imported names of the 21 `nw4r::g3d::Camera` members (`.text` `0x80264720`–`0x8026561C`) and the 16 Maya functions (`0x802658F0`–`0x802665F4`) by exact match, and names `detail::dcc::CalcTexMtx_Basic` (`0x80265620`) and `CalcWorldMtx_Basic` (`0x80265780`) in `config/wpse-v1/symbols.txt`. Confidence: high, from CC0 zeldaret/ss and kdc and the exact match.
## Issue #516 HID utilities and BTM ACL

[F-516-1](findings/516-hid-utils-acl.md) records the complete 30-function issue inventory and 34 retained functions across the two newly reconstructed units. It identifies the two HID main functions from PR #694, six ACL tails in #517, exact data payloads and alignment, and newly declared callback layouts. Existing ACL/control-block structures retain their definitions. All compared retained code/data sections are 100% with GC/3.0a3; outside callees/data receive evidenced names without changing their split, type, or size.

## NW4R ef free, line and point draw strategies (#383)

For `wpse-v1` / `main.dol`, [F-383-1](findings/383-ef-free-line-point-strategies.md) names, with mangled CodeWarrior names and high confidence from the CC0 Whippy reference and exact matches: `DrawQuad(const MTX34&, const _VEC3*, int)` (`0x80241360`), `DrawFreeStrategy::Draw` (`0x802414A0`), `~DrawFreeStrategy` (`0x80241D10`), `DrawLineStrategy::Draw` (`0x80241D90`), `~DrawLineStrategy` (`0x80242340`), `DrawPointStrategy::Draw` (`0x802423C0`), `~DrawPointStrategy` (`0x802427E0`), and the vtables `__vt__Q34nw4r2ef16DrawFreeStrategy` (`0x803C56C0`), `__vt__Q34nw4r2ef16DrawLineStrategy` (`0x803C56D8`) and `__vt__Q34nw4r2ef17DrawPointStrategy` (`0x803C56F0`), 0x18 each.
## NW4R character animation objects (#399, coordinated with #398 and #400)

[F-399-1](findings/399-g3d-anmchr.md) identifies all 26 retained methods of `AnmObjChr` and `AnmObjChrRes` at `0x802580D0`–`0x80258D8C`, with the two final base RTTI methods belonging to #400. Complete native class identity, constructors, the two 0x58-byte vtables at `0x803C5C18`/`0x803C5C70`, and all instruction/relocation targets establish the names with high confidence. `AnmObjChr` is 0x18 bytes with count/pointer at 0x10/0x14; `AnmObjChrRes` is 0x34 with `FrameCtrl` at 0x18, resource at 0x2C and cache pointer at 0x30. The base/resource type-name objects at `0x8036C7C8`/`0x8036C7D8` are 0x10/0x14 bytes; the latter's following four bytes are padding. Constants at `0x804BABD0`–`0x804BABE0` retain the compiler-generated local names and proven zero/one/integer-conversion meanings. The shared base destructor at `0x801C5C60` is named from its vtable identity and complete paired native proof; its original split/type/size ownership is unchanged. The source's unretained node/blend classes receive no native-progress credit.
## NW4R layout dependency interfaces (#807)

| Type | Size | Evidence |
| --- | --- | --- |
| NW4R lyt AnimationLink / AnimTransform / AnimResource / Material / DrawInfo | 0x10 / 0x14 / 0x10 / 0x5C / 0x54 bytes | Complete shared storage and native virtual-slot evidence; [F-807-1](findings/807-layout-interfaces.md). |
| NW4R lyt Size / ResBlockSet / res::MaterialList | 8 / 0x10 / 0xC bytes | Explicit Size copy constructor preserves the native hidden-result-pointer ABI; resource-block pointer order and material count at offset 8 are proven by Window construction; [F-807-1](findings/807-layout-interfaces.md#window-prerequisite-and-size-return-convention). |

## NW4R Pane and Group source (#807)

| Symbol | Native range | Evidence |
| --- | --- | --- |
| NW4R lyt Pane vtable | `0x803C6E70–0x803C6EE8` | Complete 0x78-byte table, including SetExtUserDataList slot70 and LoadMtx slot74; typeInfo4 at804B9408. [F-807-2](findings/807-pane-group.md). |
| NW4R lyt Group / GroupContainer | `0x802A5D00–0x802A605C` | Group size2C and GroupContainer sizeC; five native routines, vtableC at803C6EE8; complete storage in canonical headers. [F-807-2](findings/807-pane-group.md). |

## NW4R Window source (#810)

| Type or symbol | Size or native range | Evidence and confidence |
| --- | --- | --- |
| Window / Content / Frame | 0x108 / 0x18 / 8 bytes | Pane baseD8, content inflationD8, ContentE8, frame pointer100, frame count104; Color[4] and TexCoordAry in Content; flip byte and material pointer in Frame. High confidence from target constructor/destructor/draw accesses and pinned references; [F-810-1](findings/810-window.md). |
| Window vtable / typeInfo | `0x803C70A0–0x803C7130` / `0x804B9428–0x804B942C` | Complete native 0x90-byte virtual table; RTTI getterBC60 and initializerBC70 identify the four-byte object and Pane base. High confidence; [F-810-1](findings/810-window.md). |
| TextureFlipInfo / flipInfos | 10 bytes / `0x803C7060–0x803C709C` | Six texture-coordinate/index records, native bytes and draw accesses agree with pinned reference. Reference spelling retained; target does not establish helper/enum spelling independently. |
| NW4R ut Color constructor / destructor | `0x8006CDE0` / `0x8005C9A0` | Exact canonical-header non-relocation bodies and sizes12/64; evidenced outside names only, no split/size/type edits or source credit. |

## NW4R g3d ScnObj and ScnRoot (#782)

For `wpse-v1` / `main.dol`, [F-782-1](findings/782-g3d-scnobj-scnroot.md) names the 75 functions in `.text` `0x80267D40`–`0x8026AF9C` with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`. It corrects `SetPriorityDrawXlu` (`0x80268270`) and `ScnObj::GetTypeName` and `GetTypeObj` (`0x80269590`, `0x802695C0`), and merges `SetBoundingVolume` (`0x80268300`, 0x84) with the former `fn_80268380`. Newly named: `ScnObj::SetMtx`, `GetValueForSortOpa` and `Xlu`, `EnableScnObjCallbackTiming`, `EnableScnObjCallbackExecOp`, `ScnRoot::Construct`, `G3dProc`, `GetCamera`, `SetCurrentCamera`, `GetLightSet`, `SetGlbSettings` and the `ScnObjGather` constructor. Data: `TYPE_NAME` of `ScnObj`, `ScnLeaf`, `ScnGroup` and `ScnRoot` (`.rodata` `0x8036C960`–`0x8036C990`), vtables of `ScnGroup` (`0x803C5F20`), `ScnObj` (`0x803C5F60`, 0x34), `ScnObjGather` (`0x803C5F98`, 0x2C) and `ScnRoot` (`0x803C5FC8`), and `gpCullingFrustum` (`.sbss` `0x804B9318`). Outside: `Fog::Fog(FogData *)` (`0x80270A90`), `Fog::Init` (`0x80270AA0`) and `Fog::SetFogRangeAdjParam` (`0x80270B70`). Confidence: high, from CC0 zeldaret/ss, the call sites and the exact match.
For `wpse-v1` / `main.dol`, [F-444-1](findings/444-snd-wave-player.md) names `WsdFileReader` constructor (`0x80291820`), `ReadWaveSoundInfo` (`0x802918B0`), `ReadWaveSoundNoteInfo` (`0x802919D0`), and `ReadWaveInfo` (`0x80291AF0`). Its three pointers occupy offsets 0, 4 and 8, size 12. `WaveSoundInfo` is 12 bytes; `WaveSoundNoteInfo` is 20 bytes, including hold at offset 5 and pitch at offset 16. `WsdFile::NoteInfo` is 48 bytes with hold at 8; `WsdFile::WsdInfo` is 32 bytes. `WsdFileDefaultPitch` is the four-byte `1.0f` constant at `0x804BB090`; the former eight-byte survey extent included padding. The decoder `DecodeDspAdpcm` at `0x80292C68` retains its original AXPBADPCM interface and uses an int coefficient index. Confidence high from pinned CC0 references and source-selected exact link verification.

## Issue #444 WsdPlayer

[F-444-3](findings/444-wsd-player.md) establishes the complete 0x118-byte WsdPlayer layout, DisposeCallback vptr at 0xB4, SoundThread::PlayerCallback vptr at 0xC0, active flags at 0xCC, LfoParam at 0xF8 and Channel pointer at 0x114. The original 0x60-byte vtable at 0x803C6900 disambiguates the six adjustment thunks and repeated tiny methods. The weak WsdPlayer destructor remains at 0x80291490 in the preceding #443 WaveSound split. Constants occupy 0x804BB098-0x804BB0C0, preserving target addresses and exact bytes.
For `wpse-v1` / `main.dol`, [F-779-1](findings/779-g3d-camera-basic-maya.md) confirms the imported names of the 21 `nw4r::g3d::Camera` members (`.text` `0x80264720`–`0x8026561C`) and the 16 Maya functions (`0x802658F0`–`0x802665F4`) by exact match, and names `detail::dcc::CalcTexMtx_Basic` (`0x80265620`) and `CalcWorldMtx_Basic` (`0x80265780`) in `config/wpse-v1/symbols.txt`. Confidence: high, from CC0 zeldaret/ss and kdc and the exact match.
## Issue #516 HID utilities and BTM ACL

[F-516-1](findings/516-hid-utils-acl.md) records the complete 30-function issue inventory and 34 retained functions across the two newly reconstructed units. It identifies the two HID main functions from PR #694, six ACL tails in #517, exact data payloads and alignment, and newly declared callback layouts. Existing ACL/control-block structures retain their definitions. All compared retained code/data sections are 100% with GC/3.0a3; outside callees/data receive evidenced names without changing their split, type, or size.

## NW4R ef free, line and point draw strategies (#383)

For `wpse-v1` / `main.dol`, [F-383-1](findings/383-ef-free-line-point-strategies.md) names, with mangled CodeWarrior names and high confidence from the CC0 Whippy reference and exact matches: `DrawQuad(const MTX34&, const _VEC3*, int)` (`0x80241360`), `DrawFreeStrategy::Draw` (`0x802414A0`), `~DrawFreeStrategy` (`0x80241D10`), `DrawLineStrategy::Draw` (`0x80241D90`), `~DrawLineStrategy` (`0x80242340`), `DrawPointStrategy::Draw` (`0x802423C0`), `~DrawPointStrategy` (`0x802427E0`), and the vtables `__vt__Q34nw4r2ef16DrawFreeStrategy` (`0x803C56C0`), `__vt__Q34nw4r2ef16DrawLineStrategy` (`0x803C56D8`) and `__vt__Q34nw4r2ef17DrawPointStrategy` (`0x803C56F0`), 0x18 each.
## NW4R character animation objects (#399, coordinated with #398 and #400)

[F-399-1](findings/399-g3d-anmchr.md) identifies all 26 retained methods of `AnmObjChr` and `AnmObjChrRes` at `0x802580D0`–`0x80258D8C`, with the two final base RTTI methods belonging to #400. Complete native class identity, constructors, the two 0x58-byte vtables at `0x803C5C18`/`0x803C5C70`, and all instruction/relocation targets establish the names with high confidence. `AnmObjChr` is 0x18 bytes with count/pointer at 0x10/0x14; `AnmObjChrRes` is 0x34 with `FrameCtrl` at 0x18, resource at 0x2C and cache pointer at 0x30. The base/resource type-name objects at `0x8036C7C8`/`0x8036C7D8` are 0x10/0x14 bytes; the latter's following four bytes are padding. Constants at `0x804BABD0`–`0x804BABE0` retain the compiler-generated local names and proven zero/one/integer-conversion meanings. The shared base destructor at `0x801C5C60` is named from its vtable identity and complete paired native proof; its original split/type/size ownership is unchanged. The source's unretained node/blend classes receive no native-progress credit.
## Issue #443 wave archive reader

The existing canonical `nw4r::snd::detail::WaveArchiveReader` stores its table-block pointer at offset 0 and data-block pointer at offset 4. Its constructor at `0x802910A0` occupies `0x74` bytes; `GetWaveFile(int) const` at `0x80291120` occupies `0x88` bytes. The complete split ends at `0x802911A8`. Names, types and bounds semantics agree with pinned CC0 zeldaret/ss source and the exact native/link evidence in [F-443-2](findings/443-wave-readers.md). No layout changes or outside symbol extents are introduced.
