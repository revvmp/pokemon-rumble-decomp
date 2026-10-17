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
| `0x80397EE8` | `0x30` | `bta_service_id_to_uuid_lkup_tbl` | `bta_dm_act.c` `.rodata` |
| `0x80397F18` | `0x60` | `bta_service_id_to_btm_srv_id_lkup_tbl` | `bta_dm_act.c` `.rodata` |
| `0x80397F78` | `0x18` | `bta_security` | `bta_dm_act.c` `.rodata` |
| `0x804AA968` | `0x30` | `bta_dm_compress_srvcs` (local) | `bta_dm_act.c` `.bss` |

The control-block and message offsets that this code checks against the target are listed under the finding's headers section.
## RVL_SDK GXMisc and GXGeometry

Names, ranges and evidence for 25 renamed functions and four statics are in [F-476-1](findings/476-gxmisc-gxgeometry.md). Statics, all local to `GXMisc.c`: `FinishQueue` `.sbss` `0x804B96E0` (OSThreadQueue, 8 bytes), `DrawDone` `0x804B96E8` (volatile GXBool), `DrawDoneCB` `0x804B96EC`, `TokenCB` `0x804B96F0`.

External GX names given for the source link, evidence in [F-476-4](findings/476-gxmisc-gxgeometry.md): `__GXSetVCD` `0x802CB930` (0xB0), `__GXCalculateVLim` `0x802CB9E0` (0x12C), `__GXSetVAT` `0x802CC080` (0x80), `__GXSetSUTexRegs` `0x802CF270` (0x164), `__GXUpdateBPMask` `0x802CFB20` (0x4), `__GXSetProjection` `0x802D0A60` (0x44), `__GXSetViewport` `0x802D0DB0` (0x90), `__GXSetMatrixIndex` `0x802D0F90` (0x88). Confidence high; sizes are dtk's and remain with the owning units.

`GXData` (`include/revolution/gx/GXTypes.h`, reached through the `.sdata2` pointer `__GXData`) is 0x600 bytes, 4-byte aligned. Offsets confirmed by target accesses: `vNumNot` 0x0, `bpSentNot` 0x2, `vNum` 0x4, `vLim` 0x6, `lpSize` 0x7C, `ambColor` 0xA8, `matColor` 0xB0, `chanCtrl` 0xB8, `texGenCtrl` 0xC8, `dualTexGenCtrl` 0xE8, `suTs0` 0x108, `peCtrl` 0x22C, `genMode` 0x254, `dlSaveContext` 0x5F9, `abtWaitPECopy` 0x5FA, `dirtyState` 0x5FC. Other fields follow Petari's layout; confidence medium until their units are matched. `GX_DIRTY_*`, `XF_REG_*`, and `PE_REG_*` names in `include/revolution/gx/GXState.h` are descriptive choices, not recovered names.
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
