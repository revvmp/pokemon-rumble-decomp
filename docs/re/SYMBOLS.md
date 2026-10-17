# Symbols and interfaces

## OS IPC buffer and time conversion (#470)

For `wpse-v1` / `main.dol`, [F-470-1 and F-470-2](findings/470-os-ipc.md) recover `__OSGetIPCBufferHi` at `.text:0x802BD980` (8 bytes), `__OSGetIPCBufferLo` at `.text:0x802BD990` (8 bytes), and `__OSTimeToSystemTime` at `.text:0x802BD100` (0x58 bytes). These replace the corresponding `fn_` labels with high-confidence original names corroborated by Petari and target data flow. Text file offsets equal address minus `0x8004FD40`. The getters return 32-bit pointers in r3. Time conversion takes and returns signed 64-bit time in r3/r4; its source remains fallback. OSIpc source and its two 4-byte local data objects are described in the finding.

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

`Vec` contains single-precision `x`, `y`, `z` at offsets 0, 4, 8, size 12 and alignment 4. `Mtx` is a row-major 3 by 4 single-precision matrix, size 48; `Mtx44` is 4 by 4, size 64. Both have alignment 4. These layouts follow the observed load/store offsets and the CC0 SDK reference. `C_MTXPerspective` calls the matching reference's `tan` at `0x8034D4B8`; the target spelling remains unchanged because the function belongs outside issue 474.

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

Layouts follow Petari and are confirmed by target accesses: `hd_cb` uses `conn` at 0x10 (timer parameter 0x2C), `idle_tle` 0x34, `pm_params` 0x4C, `curr_pm` 0x6A, `pm_ctrl_busy` 0x6E, `conn_substate` 0x6F, `final_pm` 0x70, `callback` 0xC4, L2CAP configs 0xC8 and 0x104 (0x3C each), `reg_flag` 0x140, `trace_level` 0x141. `hh_cb` has 16 0x34-byte devices, `sdp_busy` 0x380, `sdp_cback` 0x384, `p_sdp_db` 0x388, `sdp_rec` 0x38C (0x74), `reg_flag` 0x400, `trace_level` 0x401.

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
