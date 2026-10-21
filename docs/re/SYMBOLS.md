# Symbols and interfaces

## CDisconMsg complete source interface (#58, #59)

[F-58-6](findings/58-disconmsg.md#f-58-6-actual-source-unit-integration-and-rejected-retained-order) records the complete 24-byte anonymous CDisconMsg: IDisconMsg vptr at0, shared_ptr<CLayout> at4, shared_ptr<CFiber> at12 and signed suppression count at20. IDisconMsg inherits the five ICalcDraw slots and adds IsActive, CanShow and AdjustSuppressCount in that order. Factory0x8006FBF0 returns shared_ptr<IDisconMsg>. The private class/source spelling comes from native RTTI; method and member names describe proven behavior. All32 actual source functions and22 exception records are individually exact, but retained function/data ordering prevents source-selected acceptance. The seven tail helpers belong to #59; vtable destinations distinguish CFiber deleter destructor0x80070B60 from CLayout deleter destructor0x80070BA0 despite their identical bodies. Native vtable/RTTI chains also correct the prior function_base<void(*)()> aliases at0x80131600/0x803B5800/0x804B7900 to the Home Button enum callback implementation; the zero-argument base destructor/vtable/RTTI reside at0x800704A0/0x8039B6A8/0x804B5E28. No outside sizes, types or splits change.

## CResFont resource loader (#336)

The recovered CResFont class is a complete 0x24-byte object: canonical nw4r::ut::ResFont at 0x00–0x1B and shared_ptr<CFileData> at 0x1C/0x20. Earlier constructor 0x8012D910, destructor 0x801313E0, vtable 0x803B5F38 and RTTI 0x804B7948 establish the full layout and inherited SDK virtual order. Only the derived destructor differs from SDK ResFont's vtable. These earlier records stay outside the loader source unit. The descriptive Load(const char*, bool) method at 0x801D9A10 assigns the resource owner, validates it and installs its data. The class spelling is native RTTI; member mFile and method Load are descriptive. [F-336-3](findings/336-resource-font.md) records corrected file ownership, three-section exact acceptance, all cleanup targets and source-only selection. No #335 implementation is taken.

## CRomFont and CAnyEncFont (#336)

Native RTTI recovers CRomFont, CAnyEncFont<nw4r::ut::RomFont>, and nw4r::ut::RomFont. The complete font is 0x70 bytes. Native virtual slot order is the canonical SDK RomFont order; CAnyEncFont overrides character conversion and GetEncoding, while CRomFont supplies resource construction/destruction. Method and member spellings not recovered through RTTI are descriptive.

| Offset | Member | Evidence and confidence |
| --- | --- | --- |
| `0x00–0x1B` | `nw4r::ut::RomFont` base | Constructor, SDK interface and RTTI; high |
| `0x1C–0x5F` | `CEncodingConverter mConverter` | Canonical 0x44-byte converter, initialization and conversion member-pointer accesses; high |
| `0x60` | `FontEncoding mEncoding` | UTF16 initialization and GetEncoding return; high |
| `0x64` | `int mConversionDepth` | Signed positive comparison and scoped increment/decrement; high |
| `0x68–0x6F` | `shared_ptr<u8> mBuffer` | Native custom-deleter specialization, canonical cleanup 0x800657E0 and full source-selected match; high |

The four-byte scoped guard holds a reference to the signed counter. Its generic source form CFontConversionGuard<int> and name are inferred from the native lifetime and retained emission order, not recovered original symbols. Its destructor is 0x801DA070. CRomFont constructor/destructor are 0x801D9B40/0x801D9CE0; CAnyEncFont destructor is 0x801D9C80. Conversion virtuals are 0x801D9DA0, 0x801D9E20, 0x801D9EA0, 0x801D9F40 and 0x801D9FE0; GetEncoding is 0x801DA060. Own vtable is 0x803C0FB0, and RTTI records are 0x804B8430/0x804B8438/0x804B8440. [F-336-1](findings/336-romfont-ui-list.md) records all-section, cleanup, link-input and frozen-executable evidence. The preceding resource-font unit and following IUIListItem/CUIList context remain outside this source split.

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

For `wpse-v1` / `main.dol`, [F-796-1](findings/796-snd-seqtrack.md) names the `nw4r::snd::detail::SeqTrack` vtable (`.data` `0x803C6660`, 0x10). The 17 linked `SeqTrack` functions from `0x80280FA0` to `0x802823F8` already carry names from the reference symbol import, confirmed by exact code. High confidence.
For `wpse-v1` / `main.dol`, [F-797-1](findings/797-snd-sound3d-soundarchive.md) names the 79 functions from `0x802832A0` to `0x80285400` in `snd_Sound3DEngine.cpp`, `snd_Sound3DListener.cpp`, `snd_Sound3DManager.cpp`, `snd_SoundActor.cpp`, `snd_SoundArchive.cpp`, and `snd_SoundArchiveFile.cpp` by their mangled names, the vtables of `Sound3DEngine` (`.data` `0x803C66A0`), `Sound3DManager` (`0x803C66C0`), `SoundActor` (`0x803C66F8`), and `SoundArchive` (`0x803C6718`), the default engine `sDefaultEngine` in the unit's unnamed namespace (`.bss` `0x8044D708`, 0x14), and the game-side first copy of `Sound3DManager::~Sound3DManager` (`0x801E7790`). Layouts: `Sound3DEngine` vtable pointer 0x0 and `mCalcPanParam` 0x4 (`speakerAngleStereo`, `frontSpeakerAngleDpl2`, `rearSpeakerAngleDpl2`, `initPan`); `Sound3DListener` `mMtx` 0x0, `mPosition` 0x30, `mVelocity` 0x3C, `mInteriorSize` 0x48, `mMaxVolumeDistance` 0x4C, `mUnitDistance` 0x50, `mUserParam` 0x54, `mResetMatrixFlag` 0x58, `mUnitBiquadFilterValue` 0x5C, `mUnitBiquadFilterMax` 0x60, list node 0x64 (size 0x6C); `Sound3DManager` vtable pointers 0x0 and 0x4, `mParamPool` 0x8, `mListenerList` 0xC, `mpEngine` 0x18, `mMaxPriorityReduction` 0x1C (32), `mPanRange` 0x20 (0.9), `mSonicVelocity` 0x24 (0.0), `mBiquadFilterType` 0x28; `Sound3DParam` `position` 0x0, `velocity` 0xC, `ctrlFlag` 0x18, `decayCurve` 0x1C, `decayRatio` 0x1D, `dopplerFactor` 0x1E, `actorUserParam` 0x20, `soundUserParam` 0x24 (size 0x28); `SoundActor` `mSoundArchivePlayer` 0x4, `mActorPlayer[4]` 0x8, `mActorParam` 0x48; `SoundArchivePlayer::mSoundArchive` 0x10. Function, class, and vtable names are high confidence (reference symbols and exact code). The member names `mPanRange`, `mSonicVelocity`, `mUserParam`, `mResetMatrixFlag`, `ctrlFlag`, `dopplerFactor`, `actorUserParam`, and `soundUserParam` follow the NintendoWare CTR successor classes and their use in `Sound3DCalculator` and `Sound3DActor`; medium confidence.

For `wpse-v1` / `main.dol`, [F-796-2](findings/796-snd-seqtrack.md) renames `0x80282770` to `nw4r::snd::Sound3DActor::detail_UpdateAmbientArg(void*, const detail::BasicSound*)`, its `@84@` thunk at `0x802827B0`, and the `@84@` destructor thunk at `0x802827C0`, correcting the swapped imported names, and names the `Sound3DActor` vtable (`.data` `0x803C6670`, 0x30). The other `Sound3DActor` and all `Sound3DCalculator` functions already carry names from the reference symbol import, confirmed by exact code. `Sound3DActor` layout: `SoundActor` base 0x0, `AmbientArgUpdateCallback` base 0x54, `m3DManager` 0x58, `mSoundArchivePlayer` 0x5C, `mUserParam` 0x60, `mPosition` 0x64, `mVelocity` 0x70, `mResetPositionFlag` 0x7C (size 0x80). Function and vtable names are high confidence; `mResetPositionFlag` is a descriptive name of medium confidence.

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

For `wpse-v1` / `main.dol`, [F-764-1](findings/764-tmc-jpeg-converters.md) names the twelve `scope:local` colour converters in `.text` `0x801F9850`–`0x801FB030`, all descriptive and medium confidence: `ConvertRGBA8MCU411`, `ConvertRGBA8MCU422`, `ConvertRGBA8MCU420`, `ConvertRGBA8EdgeMCU411`, `ConvertRGBA8EdgeMCU422` and `ConvertRGBA8EdgeMCU420` (`0x801F9850`–`0x801FA424`), and `ConvertRGB565MCU411`, `ConvertRGB565MCU422`, `ConvertRGB565MCU420`, `ConvertRGB565EdgeMCU411`, `ConvertRGB565EdgeMCU422` and `ConvertRGB565EdgeMCU420` (`0x801FA510`–`0x801FB030`). `TMCJPEGEncoder` `paddedWidth` (`0x24`), `paddedHeight` (`0x28`), `alignedWidth` (`0x48`) and `alignedHeight` (`0x4C`) are `u32`, proven by unsigned compares and logical shifts in the converters.
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
For `wpse-v1` / `main.dol`, [F-295-4](findings/295-nw4hbm-lyt.md#f-295-4-material-constructor-match) records the HBM `nw4hbm::lyt::res::Material` resource as 0x40 bytes: `name` (`char[20]`, `0x00`), `tevCols` (`GXColorS10[3]`, `0x14`), `tevKCols` (`ut::Color[4]`, `0x2C`) and `resNum` (`MaterialResourceNum`, `0x3C`). The `ut::Color` type of `tevKCols` is proven by the constructor's code generation, not by a symbol; confidence medium. `Material::Material()` (`__ct__Q36nw4hbm3lyt8MaterialFv`) is defined before the resource constructor and dead-stripped from the DOL; its position places the weak `ut::Color` constructor at `0x80189910`. The name and body follow CC0 `zeldaret/tp`; confidence medium.
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

## NW4R Bounding, TexMap, and DrawInfo (#808)

| Type or symbol | Size or native range | Evidence and confidence |
| --- | --- | --- |
| Bounding | 0xD8 bytes / `0x802ABC80–0x802ABD4C` | Complete Pane-derived class with five native functions, vtable78 at803C7130, runtime type4 at804B9430, ctor entry80352A94. High confidence; [F-808-1](findings/808-layout-units.md). |
| TexMap | 0x1C bytes / `0x802AEB30–0x802AF000` | Image/palette pointers0/4, dimensions8/A, min/maxLOD C/10, bias14, palette count16, packed flags18 including one-bit magFilter. Six native functions and all sections100%; high confidence. |
| DrawInfo vtable | `0x803C7218–0x803C7224` | Twelve-byte table; constructor802AF000 and destructor802AF080 agree with complete canonical54-byte layout. The next four bytes are alignment padding. |
| TextBox border metadata | Colors120/124, width128, enabled mask0x10 in byte0x101 | Native border-width and border-color string lookups identify the fields. A GDLObj at12C and buffer pointer13C prove the larger target class; other cache fields remain under investigation. |

## NW4R g3d ScnObj and ScnRoot (#782)

For `wpse-v1` / `main.dol`, [F-782-1](findings/782-g3d-scnobj-scnroot.md) names the 75 functions in `.text` `0x80267D40`–`0x8026AF9C` with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`. It corrects `SetPriorityDrawXlu` (`0x80268270`) and `ScnObj::GetTypeName` and `GetTypeObj` (`0x80269590`, `0x802695C0`), and merges `SetBoundingVolume` (`0x80268300`, 0x84) with the former `fn_80268380`. Newly named: `ScnObj::SetMtx`, `GetValueForSortOpa` and `Xlu`, `EnableScnObjCallbackTiming`, `EnableScnObjCallbackExecOp`, `ScnRoot::Construct`, `G3dProc`, `GetCamera`, `SetCurrentCamera`, `GetLightSet`, `SetGlbSettings` and the `ScnObjGather` constructor. Data: `TYPE_NAME` of `ScnObj`, `ScnLeaf`, `ScnGroup` and `ScnRoot` (`.rodata` `0x8036C960`–`0x8036C990`), vtables of `ScnGroup` (`0x803C5F20`), `ScnObj` (`0x803C5F60`, 0x34), `ScnObjGather` (`0x803C5F98`, 0x2C) and `ScnRoot` (`0x803C5FC8`), and `gpCullingFrustum` (`.sbss` `0x804B9318`). Outside: `Fog::Fog(FogData *)` (`0x80270A90`), `Fog::Init` (`0x80270AA0`) and `Fog::SetFogRangeAdjParam` (`0x80270B70`). Confidence: high, from CC0 zeldaret/ss, the call sites and the exact match.
## NW4R shape animation (#400, part of #401)

[F-400-1](findings/400-g3d-anmshp.md) records 24 retained functions in `g3d_anmshp.cpp` (`0x80258D90`–`0x8025977C`). The canonical names of the `AnmObjShp` base and resource methods agree with the pinned CC0 reference and exact native instruction/relocation comparisons. Native vtable slots distinguish the empty `SetWeight` at `0x80258E10` from `DetachAll` at `0x80258E30`. Resource/base vtables at `0x803C5CC8`/`0x803C5D18` are each 0x50 bytes; type-name records at `0x8036C7F0`/`0x8036C800` are 16/20 bytes. The base binding count and pointer are at 0x10/0x14, size 0x18. Resource members follow FrameCtrl at 0x18: resource at 0x2C, vertex-set pointer at 0x30 and cache pointer at 0x34, size 0x38. Vertex-set and cache records occupy 12 and 0x218 bytes. These retained layouts and names have high confidence; node-only members retain reference evidence because the node class is stripped.
Issue #777 recovers `CalcWorld` with its eighth root-attribute parameter and the five retained `detail::workmem` accessors at `0x80261D70`–`0x80261DBC`. The single `sTemp` union is `0x18000` bytes at `0x80434F80`, aligned to 128, with the matrix-ID view at `0x6000`. `FuncObjCalcWorld` is eight bytes (callback pointer 0, timing byte 4, node ID 6); `WorldMtxManip` is twelve bytes with matrix, scale and attribute pointers. Native control flow proves callback bits 0–2, the low-byte billboard attribute and bit-27 translation flag. The two outside basic-transform callees receive names only. See [F-777-1](findings/777-g3d-world-work-memory.md) for complete boundaries, provenance and strict native evidence.

## NW4R g3d CalcMaterial, Init and Fog (#792)

For `wpse-v1` / `main.dol`, [F-792-1](findings/792-g3d-calcmaterial-init-fog.md) names `CalcMaterialDirectly` (`0x80270750`), `G3dInit(bool)` (`0x80270990`), `G3dReset` (`0x80270A80`), `Fog::CopyTo` (`0x80270B10`) and `Fog::SetGP` (`0x80270B90`) with their mangled CodeWarrior names in `config/wpse-v1/symbols.txt`. `lbl_804B89C8` (the G3D version pointer) and `lbl_804BAD40` are resized to 4 bytes to fit their unit splits. `0x8026F880`–`0x8027074C` is `ScnGroupEx` (type name at `0x8036C9C8`) and remains unnamed. Confidence: high, from CC0 zeldaret/ss, the call sites and the exact match.
## Issue #780 DCC matrix interfaces

[F-780-2](findings/780-g3d-xsi-3dsmax.md) records all 31 original C++ function names, addresses, and sizes in the XSI and 3ds Max DCC files. Canonical `TexSrt` contains Su, Sv, R, Tu, and Tv at offsets 0, 4, 8, 12, and 16; the native functions reproduce all accesses without layout changes. The XSI world calculation reads canonical `ChrAnmResult.flags` at 0, scale at 4, and matrix at 0x1C. [F-780-1](findings/780-g3d-dcc-headers.md) records the five empty-class static WorldMtxAttr helpers and the proven high-bit scale masks. Two seven-entry dispatch tables per file occupy 56 read-only bytes; source constant extents are 12 and 20 bytes with eight-byte alignment. Naming confidence is high because complete pinned source compilation reproduces all retained function streams.
## Issue #517 BTM device and controller

[F-517-2](findings/517-btm-device-completion.md) records all 37 original issue function addresses/sizes and complete device/controller unit extents. Forty retained functions / 8,276 bytes include nine #518 tails. Existing security-device and controller structures retain their definitions; F-517-1 records ready value 5 from target instructions and preserves the complete reset enum already merged through F-519-2. Exact diagnostic/class literal payloads and eight-byte alignment are recorded in symbols and the finding. The ambiguous empty helper candidate at 0x8031E048 is rejected in favor of the existing evidenced `btm_discovery_timeout` name.

## Issue #518 acceptance inventory

[F-518-3](findings/518-btm-completion-audit.md) reconciles the original 22 functions / 4,456 bytes across the controller tail (nine), discovery file (three), and inquiry prefix (ten). It records each complete source delivery and the exact original addresses/sizes. This audit introduces no new reconstructed bytes or unit rows; existing source naming and provenance stay with F-517-2, F-518-1/F-518-2, and F-519-1/F-519-2.
## Issue #426 sound archive interfaces

[F-426-3](findings/426-snd-archive-interfaces.md) records the original SoundArchive, SoundArchiveFileReader, archive structures, and NandSoundArchive interfaces. Five archive and 23 reader native instruction matches support the canonical field accesses; headers alone claim no code progress. The byte at offset six of the two Sound3DParam structures is named spatialParameter: its copying and width are proven, but its original name and precise meaning remain unknown. The ten methods at 0x8027E210–0x8027E2A0 are bound to ut::NandFileStream by native vtable 0x803C6988 and RTTI 0x804B93D0. Their original host being snd_NandSoundArchive.cpp remains an explicitly documented hypothesis supported by pinned unit order and natural weak emission.
## Issue #795 sequence sound interface evidence

The canonical SeqSound layout is 0x4B4 bytes: BasicSound 0x110, embedded SeqPlayer at +0x110 (0x164 bytes), temporary handle +0x274, manager +0x278, sequence offset +0x27C, start type +0x280, start offset +0x284, volatile loading/prepared bools +0x288/+0x289, file stream +0x28C, 0x200-byte buffer +0x290, and 0x24-byte SeqLoadTask +0x490. Task is 0x10, TaskManager 0x3C, SoundInstanceManager<SeqSound> 0x28, and SeqFileReader 8 bytes. Original reference names and native method accesses support these declarations; the constructor store order remains unaccepted. See [F-795-1](findings/795-seqsound-interfaces.md).

## Issue #803 sound Voice

[F-803-2](findings/803-snd-voice.md) establishes the complete Voice unit at 0x8028DBB0-0x80290414, including CalcMixParam at 0x8028F730, native size 0xB88. The local nine-entry pan-curve jump table starts at 0x803C6880, followed by the 0x14-byte Voice vtable at 0x803C68A4. Thirteen consumed four-byte constants occupy 0x804BB028-0x804BB05C; numeric local names are correspondence labels only. The last float ends at 0x804BB05C, leaving following alignment padding separate. Target Setup uses SRC_4TAP_AUTO (5), direct ADPCM channel fields and 0x34 channel stride. F-803-1/F-804-1 document the full class and shared-field layout evidence.

## Issue #804 sound Util

[F-804-2](findings/804-snd-util.md) records the complete eight-function unit from CalcPitchRatio at `0x80290BC0` through GetDataRefAddressImpl ending `0x80291094`, including seven #443 functions. The eight editable reference lookup arrays occupy `0x8036E178`-`0x803705F8`, with original/reference table names; SinCos is one 0x404-byte array despite the survey interior label. The three-element pan table is at `0x803C68B8`, native size12, and CalcRandom local unsigned-long state is at `0x804B89F0`, native size4. The twelve floating literals occupy `0x804BB060`-`0x804BB090`; numeric compiler-local labels are reconstruction correspondence labels, with no semantic-original-name claim. Typed original SDA operands and referenced values establish their mapping; complete source-selected artifact identity confirms resolved operands.

## Issue #804 VoiceManager

[F-804-3](findings/804-voice-manager.md) records eleven retained methods at `0x80290420`-`0x80290BB4`. The local manager is 0x1C bytes at `0x8045248C`, preceded by the compiler's 12-byte destructor-chain node at `0x80452480`; the native one-byte initialization guard is at `0x804B93A8`. The typed original address operands establish these destinations. Numeric `@1233` is a compiler correspondence label. Canonical Voice size 0x104, node offset 0xFC and the explicit GetRequiredMemSize(int) parameter are evidenced in F-804-1. All original #804 entries are covered by VoiceManager and CalcPitchRatio from the complete Util unit.

## Issue #443 wave archive reader

The existing canonical `nw4r::snd::detail::WaveArchiveReader` stores its table-block pointer at offset 0 and data-block pointer at offset 4. Its constructor at `0x802910A0` occupies `0x74` bytes; `GetWaveFile(int) const` at `0x80291120` occupies `0x88` bytes. The complete split ends at `0x802911A8`. Names, types and bounds semantics agree with pinned CC0 zeldaret/ss source and the exact native/link evidence in [F-443-2](findings/443-wave-readers.md). No layout changes or outside symbol extents are introduced.

## Issue #443 wave file reader

The existing canonical `WaveFileReader` stores its wave-info pointer at offset 0. Its file-header constructor at `0x802911B0` occupies `0x20` bytes, wave-info constructor at `0x802911D0` occupies `0x8`, ReadWaveInfo at `0x802911E0` occupies `0x1CC`, and const GetWaveDataAddress at `0x802913B0` occupies `0x64`. The latter replaces the survey placeholder with the original reference name; the complete split ends at `0x80291414`. WaveInfo channel parameters have the existing 52-byte stride, including 40-byte ADPCM parameters and 6-byte loop parameters. [F-443-3](findings/443-wave-readers.md) records the target accesses, per-unit compiler setting, native and final-link evidence. No outside extents or layouts change.

## Issue #443 WaveSound

[F-443-4](findings/443-wave-sound.md) records the 0x234-byte WaveSound class: BasicSound base, WsdPlayer at 0x110, temporary special handle at 0x228, manager at 0x22C, prepared flag at 0x230. Its 0x38-byte vtable starts at 0x803C68C8; RuntimeTypeInfo is four bytes at 0x804B93B0, initialized through the ctor entry at 0x80352A68. The first retained WsdPlayer destructor remains at 0x80291490. SoundInstanceManager has MemoryPool/list/mutex fields, size 0x28, with priority node offset 0xF0 from the existing BasicSound layout.

## Issue #444 WsdFile and ADPCM

For `wpse-v1` / `main.dol`, [F-444-1](findings/444-snd-wave-player.md) names `WsdFileReader` constructor (`0x80291820`), `ReadWaveSoundInfo` (`0x802918B0`), `ReadWaveSoundNoteInfo` (`0x802919D0`), and `ReadWaveInfo` (`0x80291AF0`). Its three pointers occupy offsets 0, 4 and 8, size 12. `WaveSoundInfo` is 12 bytes; `WaveSoundNoteInfo` is 20 bytes, including hold at offset 5 and pitch at offset 16. `WsdFile::NoteInfo` is 48 bytes with hold at 8; `WsdFile::WsdInfo` is 32 bytes. `WsdFileDefaultPitch` is the four-byte `1.0f` constant at `0x804BB090`; the former eight-byte survey extent included padding. The decoder `DecodeDspAdpcm` at `0x80292C68` retains its original AXPBADPCM interface and uses an int coefficient index. Confidence high from pinned CC0 references and source-selected exact link verification.

## Issue #444 WsdPlayer

[F-444-3](findings/444-wsd-player.md) establishes the complete 0x118-byte WsdPlayer layout, DisposeCallback vptr at 0xB4, SoundThread::PlayerCallback vptr at 0xC0, active flags at 0xCC, LfoParam at 0xF8 and Channel pointer at 0x114. The original 0x60-byte vtable at 0x803C6900 disambiguates the six adjustment thunks and repeated tiny methods. The weak WsdPlayer destructor remains at 0x80291490 in the preceding #443 WaveSound split. Constants occupy 0x804BB098-0x804BB0C0, preserving target addresses and exact bytes.

## Issue #444 WaveSoundHandle

[F-444-4](findings/444-wave-handle.md) establishes the four-byte handle with WaveSound pointer at offset zero and DetachSound at 0x802917E0, native size 0x38. This unit ends at 0x80291818, before following alignment padding; its unused constructor strips normally. Combined source checkpoint supplies 52 functions / 6,900 native bytes across the seven reader/player/sound/handle/decoder units, with separate original-inventory accounting.
## Original script bindings (#29)

[F-29-1](findings/29-script-bindings.md) verifies all669 original binding names and native function pointers in the670-slot table at0x80352EB8–0x80354E20, including null index0 and count670 at0x804B72B4. The complete [binding catalogue](script-bindings.tsv) records every address, file offset, measured function size, exact name, raw metadata and existing owning issue. Symbols use the descriptive prefix ScriptBinding_ plus the exact original binding spelling; this does not claim recovered native C++ mangling or signature. DEBUG_GetFixedPopPokemonMonsno maps to0x8013FFB0 and GetPokemonVoiceId to0x8015D080. The two table/count data labels are descriptive; every existing size/type/split is unchanged. This names141,152 existing code bytes with zero reconstruction credit. All669 entries map uniquely into40 existing source-decompilation issues; no new source-unit boundary is asserted.

## Material and packed layout resources (#808, #811)

[F-811-1](findings/811-material-interfaces.md) records complete canonical Material (5C), TexMap (1C), packed GX records, and material/texture resource layouts with native offsets, sizes, and naming confidence. Existing Material storage and virtual order are preserved. Its companion [F-808-2](findings/808-offset-interface.md) establishes the explicit unsigned-int offset converter parameter; u32 is unsigned long here and gives a different retained template name. These header prerequisites contribute no reconstructed executable-byte credit.
Issue #800 establishes the eight-byte `nw4r::snd::detail::StrmFileReader` and twelve-byte `StrmFileLoader`, their complete RSTM layouts, and the native `ReadAdpcmInfo` parameter type from the existing `detail::AdpcmParam`. Nine retained functions at `0x80289600`–`0x80289F84` match all 2,400 instruction bytes and twelve native relocation targets. See [F-800-1](findings/800-stream-file.md) for offsets, provenance and the imported-map signature correction.
Issue #802 identifies `StrmSoundHandle::DetachSound` at `0x8028D150` and `Task::~Task` at `0x8028D190`. Full native source/relocation evidence confirms Task size `0x10` (list node 4, busy byte C), TaskManager size `0x3C` (three lists 0, current task 24, cancel flag 28, queues 2C/34), TaskThread size `0x320` (thread 0, stack pointer 318, flags 31C/31D), and StrmSound size `0x1094` (player 100, temporary handle E08, manager E0C, volumes E10, stream E90, buffer E94). The player size is `0xD08`, prepared byte F3 and track array B48; StrmChannel records are `0x34`. Static instance/guard and type records retain their native addresses. See [F-802-1](findings/802-stream-sound-tasks.md) for complete scope, section extents, canonical header coordination and pinned provenance.

## NW4R ef billboard strategy (#381)

For `wpse-v1` / `main.dol`, [F-381-3](findings/381-ef-emitter-forms-draw-strategies.md#f-381-3-nw4r-ef-billboard-strategy-first-half) names, with mangled CodeWarrior names and high confidence from the vtable, call targets and exact matches: `DrawBillboardStrategy::Draw` (`0x8023CE40`, vtable slot 3), `~DrawBillboardStrategy` (`0x8023EF60`, vtable slot 2) and `__vt__Q34nw4r2ef21DrawBillboardStrategy` (`.data` `0x803C5680`, 0x1C). `DrawNormalBillboard` (`0x8023CE80`) and `DrawYBillboard` (`0x8023D880`) keep their existing names. `DrawBillboardStrategy` adds no members to `DrawStrategyImpl`.
### Debug text queue templates, issue 453

[F-453-2](findings/453-debug-text-queue.md) records the 62 matched methods at `0x802A1950`-`0x802A346C`, their chosen descriptive `nw4r::db::DbgPrintBase<char/wchar_t>` names, the complete 0x1C-byte class and 0x18-byte entry prefix, and original-name confidence. Font pointer 0, list 4, GXColor 0x10, requested font height 0x14, visibility 0x18. Entry x/y/frames/length 0/4/8/0xC, link 0x10, text 0x18. Two independent heap/singleton pairs occupy sbss `0x804B93F8`-`0x804B9408`. Constants occupy sdata2 `0x804BB0F8`-`0x804BB10C`; the following four bytes are alignment padding. The directly called outside TextWriter and CharWriter names come from matching established interfaces and native behavior, without changing their split or source coverage.

## Issue #355 FA directory creation, rename and move

For `wpse-v1` / `main.dol`, [F-355-4](findings/355-fa-pf-dir-create-rename-move.md) names `PFDIR_p_mkdir` (`0x80201950`), `PFDIR_p_rename` (`0x80202BA0`) and `PFDIR_p_move` (`0x80203570`), low confidence, after PrFILE2's `pf_mkdir`, `pf_rename` and `pf_move`. Callees named for the link, VF names, medium confidence: `PFENT_CalcCheckSum` (`0x802065B0`), `PFENT_StoreEntryNumericFieldsToBuf` (`0x80206720`), `PFENT_storeLFNEntryFieldsToBuf` (`0x802069A0`), `PFENT_allocateEntry` (`0x80206E00`), `PFENT_AdjustSFN` (`0x80207420`), `PFENT_ITER_Retreat` (`0x80208CC0`), `PFENT_ITER_FindCluster` (`0x80208F20`), `PFFAT_GetSectorSpecified` (`0x8020B290`) and `PFPATH_AdjustExtShortName` (`0x80214E50`). Descriptive, low confidence: `PFENT_SearchEntry` (`0x80206DD0`), `PFENT_ITER_IsInDirectory` (`0x80208930`) and `PFENT_ITER_FindEntryOfPath` (`0x80209260`). Layouts: `PF_FFD.start_cluster` at 4; `PF_CUR_DIR` (0x248: `stat`, `context_id`, `dir_entry` at 8) and `PF_VOLUME.current_dir[4]` at `0x1654`, followed by the opaque 12-byte `tail_entry`, in `pf_volume.h`; `PF_CUR_VOLUME` (12 bytes: `stat`, `context_id`, `p_vol`) and `PF_VOLUME_SET.current_vol[4]` at 0 in `pf_str.h`.

## NW4R ef billboard strategy, second half (#382)

For `wpse-v1` / `main.dol`, [F-382-1](findings/382-ef-billboard-directional-strategies.md#f-382-1-nw4r-ef-billboard-strategy-second-half) confirms by exact match `DrawBillboardStrategy::DrawDirectionalBillboard` (`0x8023E0F0`), `InitGraphics` (`0x8023E8B0`), `GetCalcAheadFunc` (`0x8023E980`) and the weak `DrawStrategyImpl` copies `CalcAhead_Speed` (`0x8023EA00`), `CalcAhead_EmitterCenter` (`0x8023EAE0`), `CalcAhead_EmitterDesign` (`0x8023EBA0`, three parameters), `CalcAhead_Particle` (`0x8023EBC0`) and `CalcAhead_ParticleBoth` (`0x8023ECE0`, renamed from `fn_8023ECE0`; high confidence from the reference header and the `AHEAD_BB_PARTICLE_BOTH` case of `GetCalcAheadFunc`). `EmitterDrawSetting::typeOption0` (`+0xB0`) selects the speed stretch and `typeDir` (`+0xAE`) the ahead function. The `.sdata` table at `0x804B8900` is the eight-byte billboard texture-coordinate array.

## HBM HBMBase.cpp (#756)

For `wpse-v1` / `main.dol`, [F-756-1](findings/756-hbm-base.md#f-756-1-hbmbasecpp-unit-boundaries-and-the-first-22-functions) names the 22 `.text` functions from `initgx__10homebuttonFv` (`0x8017C470`) to `update__Q210homebutton10HomeButtonFPC17HBMControllerData` (`0x80180A20`), with high confidence from key, xenoblade and their bodies. It also names `spHomeButtonObj` (`.bss` `0x803E9BA0`, 4 bytes), `sMutex` (`0x803E9BA8`, 0x18 bytes), the local `scAnmTable` (`.rodata` `0x80366AA8`, 0x60 bytes) and `scGroupAnmTable` (`0x80366B08`, 0x250 bytes), the nine `HomeButton::sc*` constants at `0x80366D58`–`0x80366D7C`, the `HomeButton` string tables from `scCursorLytName` (`.data` `0x803BA6E0`) to `scButtonBarMenuGroupName` (`0x803BB298`), and `__vt__Q210homebutton22HomeButtonEventHandler` (`0x803BB418`, 0x10 bytes). Outside callees named for comparison, with high confidence: `update_controller` (`0x80181380`), `updateTrigPane` (`0x80181D10`), `reset_btn` (`0x80185540`) and `destroyInfoEx` (`0x80186190`). `HBMDataInfoEx` fields: `layoutBuf` `0x0`, `msgBuf` `0x4`, `texImage` `0x8`, `reserved` `0xC` (never accessed, descriptive), `texImageWidth` `0x10`, `texImageHeight` `0x12`, `texImageFormat` `0x14`. The dead-stripped global `setBlackPlateColor` is a descriptive, low-confidence name.
### Effect utility constants and algorithms, issue 770

[F-770-1](findings/770-effect-utility.md) records the complete `ef_util.cpp` at `0x80235320`-`0x80236298`, fifteen matching functions with unchanged existing names and typed interfaces. Seven C++ routines (2,460 bytes) and eight paired-single assembly routines (1,396 bytes) are accounted separately. The writable `{3.0f,0.5f}` pair at `0x804B88F0` is eight bytes, followed by eight bytes of alignment; the read-only pool at `0x804BA730`-`0x804BA770` is 64 bytes. Assembly-facing constants receive descriptive `s` names, with original spelling unknown. The C++ and assembly radians conversions share the observed factor, and the sine/cosine helpers reference existing `math::detail::gSinCosTbl`; no outside split or type changes are needed.
## Issue #790/#791 ScnMdl interfaces

[F-790-1](findings/790-g3d-scnmdl.md) establishes all 27 retained original C++ names and their native function sizes. The first ten functions are the eight material resource accessors, CopiedMatAccess constructor and CopiedVisAccess::IsVisible; the next seventeen complete the same source unit. [F-790-2](findings/790-scnmdl-headers.md) records the inherited class layouts, nested resource-view offsets and virtual order. The twelve-byte ScnMdl type-name object is at `0x8036C9B8` and its 72-byte vtable at `0x803C6060`; naming confidence is high from the pinned source and paired native relocations. GetValueForSortOpa and GetValueForSortXlu at `0x80268250`/`0x80268260`, and the outside ScnObj/ScnLeaf/ScnMdlSimple/AnmObjShp type-name objects, receive name-only metadata updates from the vtable and type tests. Their owners retain all size/type/split decisions. BUFOPTION_RESTORE_VTX_ON_DETACH is a descriptive project name for the proven bit24 restore behavior, not a recovered embedded spelling.

### Issue #812: complete NW4R layout animation

The 27 original functions at `0x802AF0C0–0x802B0674` retain their imported mangled names, independently reproduced by the pinned original C++ interfaces. AnimTransformBasic owns the 36-byte vtable at `0x803C7228`, named `__vt__Q34nw4r3lyt18AnimTransformBasic`; its original forty-byte placeholder included four following alignment bytes. The complete constant pool is `0x804BB200–0x804BB220`. Class/resource layouts and confidence are recorded in [F-812-1](findings/812-animation-interfaces.md), complete unit acceptance in [F-812-2](findings/812-lyt-animation.md). IsSupportedMinorVersion is a descriptive project helper name for the proven minor-version range eight through ten; no recovered spelling is claimed.

## HBM HBMBase.cpp pointer events (#757)

For `wpse-v1` / `main.dol`, [F-757-1](findings/757-hbm-base-pointer-events.md#f-757-1-update_controller-updatetrigpane-and-startpointevent) confirms by exact match `update_controller__Q210homebutton10HomeButtonFi` (`0x80181380`, 0x984 bytes) and `updateTrigPane__Q210homebutton10HomeButtonFv` (`0x80181D10`, 0x484 bytes). It renames `fn_801821A0` to `startPointEvent__Q210homebutton10HomeButtonFPCQ36nw4hbm3lyt4PanePv` (0x1084 bytes), with high confidence from `HomeButtonEventHandler::onEvent` (`0x80185B00`) and key. It names the outside callee `fn_80185870` as `getPaneNo__Q210homebutton10HomeButtonFPCQ36nw4hbm3lyt4Pane`, with high confidence from its use in `startPointEvent`. `HomeButton::eSeq` gains `eSeq_Seq3` (3) and `eSeq_Seq4` (4). These are key's descriptive names and carry medium confidence. The stripped inline `setForcusSE` resets `mForcusSEWaitTime` (`+0x70`). The `startPointEvent` jump table is at `.data` `0x803BB374`.
### Emitter-form vtable extents, issue 380 completion

[F-380-2](findings/380-complete-inventory-audit.md) establishes 12-byte single-method vtables at `0x803C55D8`, `0x803C55E8`, `0x803C55F8`, `0x803C5608`, and `0x803C5618`: two prefix words followed by the virtual Create or Emission pointer. Each prior 16-byte inference included four following alignment bytes. The five owned data splits now end at the actual object end, and all nine issue units have 100% function and section comparisons with exact full-DOL identity. No source coverage is added or double counted by this boundary correction.

## HBM HBMBase.cpp pointer leave event (#758)

For `wpse-v1` / `main.dol`, [F-758-1](findings/758-hbm-base-left-event.md#f-758-1-startleftevent) renames `fn_80183230` to `startLeftEvent__Q210homebutton10HomeButtonFPCQ36nw4hbm3lyt4Pane` (0xC14 bytes), with high confidence from `HomeButtonEventHandler::onEvent` (`0x80185B00`) and key, confirmed by exact match. Its switch jump table is at `.data` `0x803BB3A4`.
## NW4R ef directional strategy (#382)

For `wpse-v1` / `main.dol`, [F-382-2](findings/382-ef-billboard-directional-strategies.md#f-382-2-nw4r-ef-directional-strategy) names, from the CC0 Wii Sports map (same function order) and the target's call and vtable references, with high confidence: `ef::DrawQuad(const MTX34&, const _VEC3*, int)` (`0x8023EFE0`, file-local), the weak `math::VEC3Transform` (`0x8023F120`), `math::MTX34Mult` (`0x80240340`) and `DrawStrategyImpl::Normalize` (`0x80240380`), `DrawDirectionalStrategy::DrawDirectionalBillboard` (`0x80240B10`), `InitGraphics` (`0x80241150`), `GetCalcAheadFunc` (`0x80241210`), `DrawStrategyImpl::CalcAhead_NoDesign` (`0x802412B0`, three parameters), `~DrawDirectionalStrategy` (`0x802412E0`) and `__vt__Q34nw4r2ef23DrawDirectionalStrategy` (`.data` `0x803C56A0`, 0x1C). The vector `ef::DrawQuad` at `0x8023F160` is file-local. `EmitterDrawSetting::typeOption2 & 3` (`+0xB2`) selects `DrawDirectionalBillboard` when it is 1; `typeOption1` (`+0xB1`) selects the local-transform pivot mode and `typeAxis` (`+0xAF`) the rotation axes.
## NW4R complete graphics state (#774)

[F-774-2](findings/774-g3d-state.md) proves all 42 retained G3DState functions at `0x8025B6A0`–`0x8025ECB0`, the original anonymous state classes and all 32 data allocations with high confidence from the pinned original source, native field accesses, all eight sections at 100%, 451 relocation targets and source-selected frozen DOL identity. IndTexMtxInfo is `0x94` bytes with flags at zero and three matrices at four. IndMtxOpStd is `0x98`, with vptr at zero, validity bytes at four, a constructor-cleared reserved byte at seven and three matrices at eight; its vtable at `0x803C5EA8` orders operator(), destructor, Reset and SetNrmMapMtx. The original LightObj/LightSetting empty destructors are folded aliases at `0x8025C050`; the native 128-element constructor-array call directly proves the LightObj identity, which is the primary symbol. Four Fog callees outside this unit are named without changing their original splits/types/sizes. The complete state storage table and the reserved byte's limited naming confidence are documented in the finding.

## NW4R complete model draw (#778)

[F-778-2](findings/778-g3d-draw.md) proves all thirteen retained G3D draw and sorting functions at `0x80262880`–`0x80264720`, totaling 7,772 bytes, with high naming and layout confidence from the pinned original source, 103 native relocation targets, three sections at 100% and source-selected frozen DOL identity. DrawResMdlReplacement is `0x40` bytes with flag at zero and fifteen consecutive original pointer fields at offsets four through `0x3C`; the complete field list is recorded in [F-778-1](findings/778-g3d-draw-headers.md). Main's canonical MdlZ is twelve bytes with float depth at zero and four u16 identifiers/priority at four through ten. Both shuffle statics occupy four bytes at `0x804B8978` and `0x804B897C`, initialized to -4. Three consumed float constants occupy twelve bytes at `0x804BAC98`; six unused sorting template constants are stripped from the final ELF. The workmem accessor at `0x80261D90` receives its evidenced canonical name without changing its outside split, size or type.

## NW4R simple model scene object (#783)

[F-783-1](findings/783-g3d-scnmdlsmpl.md) confirms all 23 retained ScnMdlSimple methods at `0x8026AFA0`–`0x8026CFEC`, the 72-byte vtable at `0x803C6018`, and the type-name object at `0x8036C9A0` through exact native instruction, relocation and data evidence. The former placeholder at `0x8026CB10` is EnableScnMdlCallbackTiming(Timing). The complete class is `0x138`, with the canonical ScnLeaf base through `0xE8`, resource/matrix state through `0x104`, bytecode/draw state through `0x11C`, callback state through `0x124` and five animation pointers through `0x138`. The findings record every consumed member offset and seven outside name-only identities; confidence is high from the pinned original library source and paired native proof.

## Issue #809: complete Layout and Picture units

[F-809-1](findings/809-lyt-layout-picture.md) records the complete native, relocation, allocated-section and source-selected frozen evidence. Layout remains the canonical 0x20 class with unchanged storage and virtual order; the first helper at `0x802A6060` (468 bytes) is supplied under #808's exact first-owner review and counted once. Layout's sixteen-entry vtable is `0x803C6EF8`, allocator storage is `0x804B9410`, and its two float constants begin at `0x804BB180`. Helper identities and new resource/header declarations follow the pinned original SS source and are corroborated by all nineteen matching functions.

Picture is the complete 0xF0 class extending canonical Pane (0xD8). Four `ut::Color` values begin at `0xD8`, followed by the eight-byte `detail::TexCoordAry` at `0xE8`; its 31-entry vtable is `0x803C6F38`, typeInfo storage is `0x804B9418`, float zero is `0x804BB188`, and the constructor-table entry `0x80352A88` targets `0x802A7680` (12 bytes). The 0x60-byte resource record extends res::Pane (0x4C), with colors at `0x4C`, material index u16 at `0x5C`, texture-coordinate count u8 at `0x5E` and original one-byte alignment padding. Constructor accesses, allocation sizes, all consumed member accesses and all native vtable targets corroborate the original declaration and virtual order.

Both anonymous Layout helpers and Picture's generated initializer retain compiler-proven local symbol scope. Outside TextBox/Bounding identities are named only with owner approval; their size/type/split metadata is preserved. TextBox's complete canonical 0x140 storage and RTTI null guard come from their merged owner-reviewed prerequisites, without duplicate declarations here.

## HBM HBMBase.cpp trigger event and event handler (#759)

For `wpse-v1` / `main.dol`, [F-759-1](findings/759-hbm-base-trigger-event.md#f-759-1-starttrigevent-to-update_sound) renames `fn_80183E50` to `startTrigEvent__Q210homebutton10HomeButtonFPCQ36nw4hbm3lyt4Pane` (0x16E8 bytes) and `fn_80185B00` to `onEvent__Q210homebutton22HomeButtonEventHandlerFUlUlPv` (0xB8 bytes), with high confidence from the `HomeButtonEventHandler` vtable, the event dispatch and key, confirmed by exact match. The `startTrigEvent` switch jump table is at `.data` `0x803BB3D4`. The sound constants `scSoundHeapSize_but2`, `scSoundHeapSize_but3`, `scSoundThreadPrio` and `scDvdThreadPrio` are at `.rodata` `0x80366E00`–`0x80366E10`.
## Issue #356 FA directory API and first entry functions

For `wpse-v1` / `main.dol`, [F-356-1](findings/356-fa-pf-dir-api.md) names the last 23 functions of `pf_dir.c`. `PFDIR_FinalizeAllDirs` (`0x80204E20`) is VF's name, high confidence. Descriptive, low confidence: `PFDIR_RemoveEntry` (`0x80204550`), `PFDIR_OpenFileEntry` (`0x80204810`), `PFDIR_OpenDirEntryForDTA` (`0x802048F0`) and `PFDIR_p_fsexec` (`0x802049C0`). The API wrappers, low confidence after the PrFILE2 API, are `PFDIR_fsfirst` (`0x80204EB0`), `PFDIR_fsnext` (`0x80204F60`), `PFDIR_fsexec` (`0x80204FE0`), `PFDIR_fstat` (`0x802050B0`), `PFDIR_rename` (`0x80205140`), `PFDIR_move` (`0x80205230`), `PFDIR_opendir` (`0x80205320`), `PFDIR_closedir` (`0x802053C0`), `PFDIR_readdir` (`0x802054A0`), `PFDIR_telldir` (`0x80205570`), `PFDIR_seekdir` (`0x80205640`), `PFDIR_rmdir` (`0x802056F0`), `PFDIR_mkdir` (`0x802057A0`), `PFDIR_createdir` (`0x80205880`), `PFDIR_chdir` (`0x80205980`), `PFDIR_fchdir` (`0x80205A10`), `PFDIR_chmod` (`0x80205AF0`) and `PFDIR_chdmod` (`0x80205BE0`). Callees named for the link: `PFVOL_CheckForWrite` (`0x802161F0`, VF name, medium); descriptive, low: `PFVOL_SetCurrentVolume` (`0x80216890`), `PFFILE_OpenEntry` (`0x80210580`), `PFENT_ITER_FindEntry` (`0x802093D0`), `PFENT_ITER_GetEntry` (`0x80209440`), `PFENT_ITER_LoadLongName` (`0x802094B0`), `PFPATH_CmpLongName` (`0x80212D90`) and `PFPATH_CmpShortName` (`0x80212DC0`). `PF_DTA` offsets 0 and 4 are `p_file` and `p_dir` (`pf_dir.h`).

[F-356-2](findings/356-fa-pf-dir-api.md#f-356-2-pf_entryc-boundaries-and-first-functions) names `PFENT_IsNotMatchedEntry` (`0x80205CD0`) and `PFENT_LoadMatchedEntry` (`0x80205EE0`), descriptive, low confidence, and the callees `PFENT_LoadLFNEntryFieldsFromBuf` (`0x802067C0`), `VFiPFPATH_putShortName` (`0x802136A0`) and `VFiPFPATH_getShortName` (`0x80213830`), VF names, medium confidence. `lbl_804B8878` carries `align:8` as the first `.sdata` object of `pf_entry.c`.

## Issue #760: HomeButton extension methods

| Address or offset | Name | Evidence and confidence |
| --- | --- | --- |
| `0x80185E00`, size `0x390` | `homebutton::HomeButton::createInfoEx(const HBMDataInfoEx*)` | Existing symbol, native resource and animation construction, CC0 key NAND method; high behavioral confidence |
| `0x80186190`, size `0x120` | `homebutton::HomeButton::destroyInfoEx()` | Existing symbol, native destruction and release order, key NAND method; high |
| `0x801862B0`, size `0x84` | `homebutton::HomeButton::set_text_ex()` | Quote-delimited wide-message parser, key NAND method, original spelling inferred from reference; high behavior, medium spelling |
| `HomeButton + 0x2DC`, extent `0x28` | `mpTextEx[10][1]` | Native independent row and language indices and following font pointer at `0x304`; high; header prerequisite #896 |

All three methods compare at 100%; all 37 functions and every section in the completed HBMBase unit compare at 100% and the source-linked DOL verifies. See [F-760-2](findings/760-hbm-extension.md).

## Issue #756: completed first HBMBase block audit

The complete original issue inventory contains 22 functions and 20,124 instruction bytes at `0x8017C470` through `0x80181374`. Every address, size, and existing reconstructed name is confirmed in a fresh objdiff report at 100%. The full native HBMBase unit is source linked and all 37 functions and all sections compare at 100%; frozen executable verification passes. No symbols or class layouts change. See the [complete address inventory and acceptance evidence](findings/756-complete-inventory-audit.md).
## NW4R ef smooth stripe strategy (#384)

For `wpse-v1` / `main.dol`, [F-384-1](findings/384-ef-smooth-stripe-strategy.md#f-384-1-nw4r-ef-smooth-stripe-strategy-first-half) names, from the riptl/wii-symbols retail maps (`RZTP01`, `SUPE01`; mangled names with equal sizes, high confidence) and the CC0 Wii Sports map order (medium): `DrawSmoothStripeStrategy::CalcVertex` (`0x80245F30`), `CalcVertexTube` (`0x802462D0`, signature from source), `DrawSection_BSpline` (`0x80246730`), `DrawSectionEnd_BSpline` (`0x80246A00`), `DrawSectionTube_BSpline` (`0x80246C20`), the local weak `GXEnd` (`0x802476E0`), `DrawSmoothStripe_BSpline_ConnectNone` (`0x802476F0`), `GetDrawOrder` (`0x80247A40`), `GetStripeTexmapType` (`0x80247A50`), `Vertex::Vertex()` (`0x80247A60`) and `__vt__Q34nw4r2ef24DrawSmoothStripeStrategy` (`.data` `0x803C5728`, 0x1C); and in the stripe file `DrawStripeStrategy::DrawTubeSegment` (`0x802434A0`), `DrawStripeTube_ConnectNone`/`_ConnectRing`/`_ConnectEmitter` (`0x80243880`, `0x80243B40`, `0x80243E80`), `DrawStripeTube` (`0x80244240`), `Draw` (`0x802449F0`), `GetCalcAheadFunc` (`0x80245540`) and `~DrawStripeStrategy` (`0x80245EB0`), the last three also from the stripe vtable at `0x803C5708`. Layouts: `Vertex` is 0x1C (`pos0`, `pos1`, `texT`), `VertexTube` 0x28 (`pos`, `axisX`, `axisZ`, `texT`), `Trigonometric` 8 (`cos`, `sin`); `AheadContextStripe` extends `AheadContext` (0xBC) with `mEmitterAxisX` (`+0xBC`) and `mViewAxisZ` (`+0xC8`). Member, flag and enum names other than the mapped functions are descriptive (low confidence).
## Issue #426: NW4R player heap

`nw4r::snd::detail::PlayerHeap` is 32 bytes: virtual table 0, sound 4, player 8, heap start 12, heap end 16, allocation cursor 20, list node 24. Constructor stores, allocator arithmetic, disposal calls, and vtable `0x803C6598` establish this layout and the eight named functions; see [F-426-1](findings/426-snd-mml-playerheap-remote.md).

`MmlSeqTrack` is `0xD0` bytes, with `SeqTrack` occupying `0xCC` and the parser pointer at `0xCC`. Its vtable at `0x803C6570` retains destructor `0x8027E060` and parser dispatch `0x8027E040`. Constructor `0x8027E000` calls the native sequence-track constructor at `0x80280FB0`; the destructor calls its counterpart at `0x80281050`. Complete interface evidence is in [F-426-2](findings/426-snd-shared-interfaces.md).

`MmlSeqTrackAllocator` is 12 bytes: virtual table 0, parser pointer 4, instance pool 8. Vtable `0x803C6580` retains destructor `0x8027E1D0`, allocation `0x8027E0C0`, release `0x8027E130`, and available count `0x8027E1C0`. Native pool creation proves the `0xD0` element stride, and all six retained functions plus the complete vtable match; see [F-426-1](findings/426-snd-mml-playerheap-remote.md).

`RemoteSpeaker` is `0xA8` bytes: state at 8, user/internal commands at 12/16, callback at `0x38`, and alarms at `0x40/0x70`. All eleven retained methods match, including all four #426 functions; the manager accessor independently proves array stride `0xA8`. [F-426-1](findings/426-snd-mml-playerheap-remote.md) records the native ownership inventory, while [F-426-2](findings/426-snd-shared-interfaces.md) records the complete canonical interface. Names follow the pinned original-library reference and native paired relocations, with high confidence.

## Issue #371: PrFILE2 kernel adapter

The ten `pfk_` function names at `0x8021DFE0` through `0x8021E3D0` are recovered directly from native diagnostics. Their exact addresses, sizes, proven parameter structures, and SDK call evidence are recorded in [F-371-1](findings/371-fa-kernel.md). `PFK_TASK_PARAMS` is `0x18` bytes; `PFK_MAILBOX_PARAMS` and `PFK_SEMAPHORE_PARAMS` are `0x0C` bytes. Structure and member names are descriptive; offsets and types are target-proven. All ten C functions, 1,048 instruction bytes, and both sections compare at 100%, with a source-linked exact DOL. The remaining eighteen issue functions are outside this native unit and remain unreconstructed.
## NW4R lighting and vertex blending (#793)

[F-793-1 and F-793-2](findings/793-g3d-light.md) establish `nw4r::g3d::CalcVtx` at `0x80271670`, size `0x79C`, from the complete match to pinned CC0 ss. The preceding 18 lighting functions retain their existing names. `LightObj` has its flags at offset zero and a `GXLightObj` at offset four, size `0x44`. `LightSetting` contains 16-bit light and set counts at offsets zero and two, then light, ambient-light and set-array pointers at four, eight and `0xC`, size `0x10`. `LightSet` contains setting and set-data pointers at zero and four, size eight. Each `LightSetData` has eight signed light indices, a signed ambient index at offset eight and three trailing bytes, size `0xC`. Existing layouts are unchanged; constructor, accessors and all native function sizes/instructions verify them. `GetLightObj` at `0x80271530` returns a selected nonnegative index from the light-object array with a `0x44` stride. Constant extents are `0x804BAD48–0x804BAD74` and `0x804BAD78–0x804BAD7C`; following alignment is excluded. Function names and layouts have high confidence from the pinned reference and exact source-linked native comparison.
## NW4R sound handle, player, system, thread and stream pool (#799)

[F-799-1](findings/799-sound-system-player-thread.md) identifies all 41 retained functions / 6,316 bytes in six units from 0x80287C70 through 0x80289600. High-confidence original class/member names come from pinned CC0 references and exact instruction, relocation and linked-address evidence. The finding records priority lists/guard/constants, the SoundSystem version pointer/string, task-thread object and constructor, and SoundThread singleton/registration/guard extents. SoundThread is 0x398 bytes with mutex0x354 and native create/pause bytes0x394/0x395; mPauseFlag remains a descriptive behavior name. StrmBufferPool is0x18 bytes: pointer0, buffer size4, block size8, block count0xC, allocation count0x10, four allocation flag bytes0x14. SoundSystemParam is0x10 bytes with sound priority/stack size/DVD priority/stack size at0/4/8/0xC. Every owned symbol size is measured independently of surrounding alignment.
### FA utility conversions, issue 365

[F-365-1](findings/365-fa-api.md) records seven functions at `0x8021A2B0–0x8021A464` and the complete 40-integer table at `0x803C4278–0x803C4318`. All seven names are independently supported by the pinned CC0 Wii Menu FA symbol map; five and `pf_error_to_api_error` also follow pinned CC0 VF source with the established FA prefix convention. Every native function size, both address relocations, full table payload, actual ELF placement and source input are verified. No PF_VOLUME layout or preceding codepage ownership changes.

### FA standard chdir, issue 365

[F-365-2](findings/365-fa-api.md#f-365-2-standard-chdir-wrapper) records original map-supported `pf2_chdir` at `0x8021A5D0`, 80 bytes. The separately owned `PFDIR_chdir` callee at `0x80205980` gains only its evidenced name and one-argument PF_STR pointer declaration; original size/type/split/source coverage are preserved. The native five relocations, actual source-selected unit and entire DOL are exact. No shared record layout changes.
[F-357-2](findings/357-fa-pf-entry.md#f-357-2-the-357-functions) names the rest of `pf_entry.c` and the first four functions of `pf_entry_iterator.c` (F-357-1). VF names without `VFi`, medium confidence: `PFENT_searchEmptyTailSFN` (`0x80206120`), `PFENT_findEmptyTailSFN` (`0x80206490`), `PFENT_LoadShortNameFromBuf` (`0x80206620`), `PFENT_loadEntryNumericFieldsFromBuf` (`0x80206670`), `PFENT_MakeRootDir` (`0x80207190`), `PFENT_ITER_DoFindEntry` (`0x80207880`); high confidence: `PFENT_ITER_RecalcEntryIterator` (`0x802076D0`). Descriptive, low confidence: `PFENT_AllocateEntry` (`0x802070E0`). `PFENT_findEntry` takes a `u8` excluded-attribute mask as its sixth argument, and `PFENT_LookupPath` takes `wildcard` before `is_parent`. Callees named for the link: `PFVOL_LoadVolumeLabelFromBuf` (`0x801FBBE0`), `PFFAT_MakeRootDir` (`0x8020BEF0`), `VFiPFPATH_InitTokenOfPath` (`0x80212F50`), `VFiPFPATH_GetNextTokenOfPath` (`0x80212F60`), `PFPATH_CheckExtShortNameSignature` (`0x80214960`) and `PFSYS_TimeStamp` (`0x8021C8B0`), VF names, medium; `VFiPFPATH_cmpTailSFN` (`0x80212F20`), VF name, high; `PFPATH_MatchFileNameWithPattern` (`0x80213390`) and `PFPATH_ParseEntryIndex` (`0x80214CD0`), low. `PF_DIR_TAIL` (12 bytes: `tracker_size`, `tracker_buff[1]`, `tracker_bits`) replaces the opaque `PF_VOLUME.tail_entry` at `0x1F74` in `pf_volume.h`; `PF_SYS_DATE` and `PF_SYS_TIME`, with 32-bit fields, are in `pf_system.h`. `lbl_804BA5C0` is one byte, and `lbl_804B8878` is now the first `.sdata` object of `pf_entry_iterator.c`.
## TextBox drawing interfaces (#808)

[F-808-4](findings/808-textbox-draw-interfaces.md) records the unchanged 0x140 TextBox and the native drawing signatures. Font::GetCharStrmReader at 0x8029C320 is an out-of-line 56-byte method. The original PrintMutable wide forwarder at 0x8029F6C0 passes a third bool to PrintImpl at 0x8029FF50. The GD counterpart at 0x8029F6D0 and CharWriter GD setup at 0x802990C0 receive descriptive inferred names PrintMutableToDisplayList and SetupGXForDisplayList; original spellings are unknown. Resource TextBox/Font/FontList sizes are 0x74/8/0xC. Native signatures, layout checks, all 23 TextBox functions and paired relocations, and the complete existing-source baseline comparison support these interfaces without executable-byte credit.

## NW4R ef smooth stripe strategy, second half (#385)

For `wpse-v1` / `main.dol`, [F-385-1](findings/385-ef-smooth-stripe-strategy-second-half.md#f-385-1-nw4r-ef-smooth-stripe-strategy-second-half) names `DrawSmoothStripeStrategy::GetInitialPrevAxis` (`0x8024A220`, medium; returns `VEC3` by value, takes the draw setting and `AheadContextStripe*`), `GetCalcAheadFunc` (`0x8024A4D0`, high, vtable slot 0x18), `CalcAhead_Particle_Stripe` (`0x8024A5A0`), `CalcAhead_ParticleBoth_Stripe` (`0x8024A6C0`), `CalcAhead_ParticleBoth_Ring` (`0x8024A940`), `CalcAhead_ParticleBoth_Origin` (`0x8024AC10`; the four medium, from the CC0 Wii Sports map order and the `GetCalcAheadFunc` cases, with `CalcAheadFunc` signatures) and `~DrawSmoothStripeStrategy` (`0x8024AE40`, high, vtable slot 0x8). Outside the block, for the link: the retained weak `nw4r::math::VEC3::VEC3()` (`0x80096810`), `VEC3::operator+=` (`0x80098CF0`), `VEC3::operator+` (`0x8009CA00`) and `VEC3::operator-` (`0x8009CAE0`), high for the bodies and medium for the operator identities. Layouts: `AheadContextStripe` adds `mTrigonometric` (`+0xD4`, `const Trigonometric*`); `ActivityList::mNumActive` (`+0x18`, manager `+0x50`) is the count `GetNumDrawParticle` returns. `EmitterDrawSetting` roles: `typeOption0` tube side count, `typeOption1` curve subdivisions, `typeOption2` connection type (bits 0–2), initial previous axis (bits 3–5) and texture mapping (bits 6–7). The enums `StripeConnectType`, `PrevAxisType`, `EmitterDrawSetting::AHEAD_ST_*`, the file-local `BeginTubeSide` and the static edge vectors `sEdgeX0`, `sEdgeX1`, `sEdgeZ0`, `sEdgeZ1` (`.bss` `0x8042F978–0x8042F9A8`) are descriptive (low confidence).

## Issue #371: NAND-backed disk driver

The complete unit at `0x8021E420` to `0x8021FFB4` contains 18 C functions, all 6,976 instruction bytes and all five sections at 100%. [F-371-2](findings/371-fa-nand-driver.md) records every address, size, chosen descriptive name, parameter evidence, and exact static layout. The private `PFD_NAND_DEVICE` record is `0xF0` bytes with 23 entries; `NANDFileInfo` is at `0x58`, flags at `0xE4`, opaque reserved storage at `0xE8`, and last error at `0xEC`. The inferred record-array symbol was divided at `0x80422BC8` into its proven `0x1590` bytes plus separate semaphore parameter and semaphore objects. The semaphore handle at `0x804B9220` is four bytes; following padding belongs outside this unit. Full source-linked DOL verification passes. Function and member spelling is descriptive; behavior and offsets are proven.

## Issue #355: Cluster deletion acceptance

[F-355-5](findings/355-complete-inventory.md) completes `PFCLUSTER_DeleteChain` at `0x80201050` (432 instruction bytes) and the entire eight-function `pf_cluster.c` unit. Its descriptive name and proven interfaces/layouts are unchanged. A separate available-byte-count local reproduces the native offset/size register assignment. Every function and section is 100%, and the source-linked full executable matches the frozen reference. The same finding records all 25 original issue addresses and sizes across matching cache, cluster, and directory units.
SeqSoundHandle::DetachSound occupies 0x80280F60–0x80280F98 and is the only retained function in its separate source unit. Its four-byte mSound member points to SeqSound, whose temporary special-handle pointer at +0x274 is cleared only when it equals this handle. The source-selected linked function has the exact native address, extent, and all 56 instruction bytes; no relocations or retained data belong to this unit. See [F-795-2](findings/795-seqsound-handle.md).
## Game CDlgMsgWin (#73)

For `wpse-v1` / `main.dol`, [F-73-2](findings/73-cdlgmsgwin.md#f-73-2-cdlgmsgwin-classes-and-layout) records the original RTTI names `ICalc` (`.sdata` `0x804B5AA0`), `ICalcDraw` (`0x804B5B40`), `IDlg` (`0x804B5FF0`), `IDlgMsgWin` (`0x804B61C0`), `CDlgMsgWin` (`0x804B61B0`) and its `std::tr1::enable_shared_from_this<CDlgMsgWin>` base at offset 4 (`0x804B61B8`), with vtables `ICalc` `0x80399E38`, `IDlg` `0x8039C3FC`, `IDlgMsgWin` `0x8039DC90`, `CDlgMsgWin` `0x8039DBC0` (24 slots) and the `shared_ptr_deleter<CDlgMsgWin, default_delete<CDlgMsgWin>>` vtable `0x8039DB40`. `CDlgMsgWin` is 0x120 bytes: vtable pointer at 0, weak pointer of `enable_shared_from_this` at 4, registration `bool` at 0xC, the 0xE8-byte layout screen object at 0x10, open `bool` at 0xF8, a `vector` of 0x18-byte entries at 0xFC, `int`s at 0x108, 0x10C (cursor), 0x110 (mode), a two-word animation handle pointer at 0x114 and the controller slot `int` at 0x11C. Constructor `0x80084DE0`, initializer `0x800851E0` and factory `0x80084D40` are identified by behavior; no method or member name is original, so none is entered in `symbols.txt`.
## Issue #798 SoundArchivePlayer interface

[F-798-1](findings/798-soundarchiveplayer-interfaces.md) establishes SoundArchivePlayer's complete 0xEC-byte layout: bases at 0/0xC, archive pointer 0x10, callbacks 0x20/0x28, three instance managers 0x44/0x6C/0x94, Mml allocator 0xBC, stream pool 0xC8, parser 0xE0 and setup storage 0xE4/0xE8. Native constructor/update/preparation accesses and the canonical compiler size probe support the original reference members. Callback vtables 0x803C6768/0x803C6778 identify both retained destructors. All 34 retained diagnostic streams/10,172 bytes reproduce initial native words/extents, with relocation and actual source-link acceptance still pending; no code credit is claimed.
## F-811-2: Native Material source and outside assignments

| Address | Name/type | Evidence | Confidence |
| --- | --- | --- | --- |
| `802ABD50` | anonymous `CalcTextureMtx(MTX34*, const TexSRT&)` | Native264bytes, matrix arithmetic and paired relocations | Proven |
| `802ABE60` | anonymous `SetIndTexMtx(GXIndTexMtxID, const f32 (*)[3])` | Native512bytes, exponent normalization and GX call | Proven |
| `803C71A8`, `803C71C8`, `803C71E8` | `SetupGX::kColSels`, `SetupGX::kAlpSels`, Material vtable | Complete112data bytes,10paired relocations | Proven |
| `804BBC10` | `const GXColorS10 DefaultBlackColor` | Native8zero bytes and original SDA2 reference | Proven |
| `800969A0`, `800969D0` | `VEC2::operator=(const VEC2&)`, `_VEC2::operator=(const _VEC2&)` | Independent48/20byte bodies, paired wrapper+14 branch, unchanged canonical headers | Proven names; no outside ownership change |
## Native CharWriter display-list setup (#450)

[F-450-3](findings/450-charwriter-native.md) establishes the complete `0x4C` canonical CharWriter and thirteen retained functions at `802983D0-8029A78C`. LoadingTexture occupies sixteen bytes at `804524C8`; the local fog guard/Color occupy `804B93D8-804B93E0`; zero/one/signed-conversion constants occupy `804BB0C8-804BB0D8`. The public display-list setup at `802990C0` and private color-mapping/intensity helpers at `8029A170/8029A4E0` have descriptive inferred names, supported by complete native GX call/TEV argument evidence, 429 paired relocation targets and forty data bytes. Storage and virtual order remain unchanged. All thirteen functions are linked from source with exact native positions and complete frozen DOL identity. Original450's 36 rows and the two neighboring451 tails are audited separately; previously delivered ArchiveFont/PairFont receive no new byte credit.
## MSL wide printf (#345)

[F-345-1](findings/345-printf.md) records the 13 functions in `0x801EB9F0`–`0x801EE468`, private helpers, formatter descriptor layout and exact section boundaries. Reference-derived names (`parse_format`, `long2str`, `longlong2str`, `double2hex`, `round_decimal`, `float2str`, `__wpformatter`, `__wStringWrite`, `swprintf`, `vswprintf`) are high confidence. The default custom callback `__wDefaultFormat`, count writer `__wCountWrite`, count wrapper `__wCountPrintf` and four-byte callback pointer `__wprintf_custom_format` at `0x804B8670` are descriptive, medium confidence. The callback returns zero; the count writer returns one. Private helpers retain local scope. The `print_format` structure is 0x28 bytes with the positional index at 0x10, argument union at 0x18 and delimiter pointers at 0x20/0x24.

Owned string metadata separates five UTF-16 literals at `0x803C32E8`, `0x803C32F4`, `0x803C3300`, `0x803C330C` and `0x803C3318`; empty wide/narrow strings at `0x804B8698`/`0x804B869C` have payload sizes two/one. Outside names needed by the link are `__lconv` (`0x803E6078`), `towupper` (`0x801EF670`), `toupper` (`0x801EF680`), `mbtowc` (`0x803453CC`), `mbstowcs` (`0x8034544C`) and `memchr` (`0x80345690`), with their original sizes and ownership preserved. The additive `lconv` layout is 0x38 bytes. The narrow printf first-owner344 was subsequently claimed through the helper; its complete reconstruction is recorded below.

## MSL narrow printf (#344, #345)

[F-344-1](findings/344-narrow-printf.md) establishes all17 functions in801E8DD0–801EB9E8. Standard/private reference names retain high confidence. Descriptive medium-confidence names are __DefaultFormat (returns0), __CountWrite (returns1), __CountPrintf and the four-byte callback pointer __printf_custom_format at804B8628. Private helpers have local scope. The print_format descriptor retains the proven0x28 positional/range layout, with one-byte narrow conversion character. The final empty narrow string at804B8668 has one payload byte. No outside callee extent, split or storage definition is changed. Both actual source objects and every final native function extent are checked independently; the full frozen DOL remains exact.
## Game BModelObj (#39)

For `wpse-v1` / `main.dol`, [F-39-1](findings/39-bmodelobj.md) splits `game/BModelObj.cpp`, `.text` `0x800528E0`–`0x8005AA98`. Class names `BModelObj`, `BSoundSource`, `BKDTreeLink`, `IKDTreeLinkRemoved`, `BWorldObj` and `ICalc` and the base `std::tr1::enable_shared_from_this<BWorldObj>` are original, from RTTI at `.sdata` `0x804B5A68`–`0x804B5AA0`. `CModel`, `ICalcDraw` and `ScnRoot` names come from RTTI in other units; `DrawLayerSet` and `IDrawLayer` are descriptive (low confidence). Script binding strings confirm `getMotionLength`, `play`, `pause`, `resume` and `setOutlineType`; all other member and method names are descriptive.

`BWorldObj` (0x48 bytes): vtable pointer at 0 (`ICalc`), `enable_shared_from_this` weak pointer at 4, `mState` at 0xC (bit 0 visible), `mType` at 0x10 (`TYPE_WORLD_OBJ` 1 or-ed into every type, `TYPE_MODEL_OBJ` 0x20), `mWorld` (`BWorld*`) at 0x14, `mId` at 0x18, `weak_ptr<BWorld>` at 0x1C, `shared_ptr` owner reference at 0x24, `mRotation` (`f32`) at 0x2C, `mPosition` (`VEC3`) at 0x30 and `mScale` (`VEC3`) at 0x3C. Constructor `__ct__9BWorldObjFUl` (`0x8006A100`) and destructor (`0x8006A200`) are out of line. Virtual order after the destructor: `Calc`, `CalcMtx`, `SetPosition`, `SetScale`, `SetRotation`, `GetSize`, `GetPosition`, `SetVisible`, `GetOwner`, `GetEffect`, `SetEffectEnable`, `GetDrawLayers`, `OnEnterWorld`, `OnLeaveWorld`, `UpdateVisible`.

`BSoundSource` (0x1C bytes): sound handle at 0, `shared_ptr` sound actor at 4, position `VEC3` at 0xC, vtable pointer at 0x18. Constructor `0x801E8740`, destructor `0x801E87A0`, `SetPosition(VEC3)` `0x801E8A10`. `BKDTreeLink` (0xC bytes): next, previous and owner (`IKDTreeLinkRemoved*`); unlinking self-links the node and calls the owner's `OnLinkRemoved`.

`BModelObj` (0xF8 bytes): `BWorldObj` at 0, `BSoundSource` at 0x48, `BKDTreeLink` at 0x64, `mFlags` at 0x70 (dirty matrix 1, hidden 2, base matrix 4, KD tree 8, outline 0x10, size mode 0x20, no bounding 0x40, shadow 0x80, model-scale size 0x100), `shared_ptr<CModel>` at 0x74, model scale `VEC3` at 0x7C, size `VEC3` at 0x88, bounding box (two `VEC3`) at 0x94, `mVelocity` `VEC3` at 0xAC, base `MTX34` at 0xB8, `mOutlineType` at 0xE8 (initially 1) and `DrawLayerSet` at 0xEC. The vtable `__vt__9BModelObj` (`.data` `0x80399640`, 0x80 bytes) appends `SetModel`, `SetTransform`, `SetModelScale`, `SetBaseMtx`, `ClearBaseMtx`, `SetTransformDirect`, `SetScaleDirect`, `SetModelScaleDirect`, `SetBoundingEnable`, `SetSizeMode` and `AddDrawLayer` at slots 0x54–0x7C. Functions renamed for the link: constructor `0x800528E0`, destructor `0x80052F60`, `UpdateOutline` `0x800561D0`, `CalcSize(int) const` `0x80056B80`, the thunk `@72@__dt__9BModelObjFv` `0x8005AA90`, and in `CModel`: `GetMotionLength` `0x801B9000`, `Play` `0x801B9450`, `PlayLoop` `0x801B9610`, `PlayOnce` `0x801B96F0`, `Pause` `0x801B9970`, `UpdateFrame` `0x801B9A70`.

`CModel` (partial): privately derives `ICalcDraw` and `nw4r::g3d::ICalcWorldCallback` (its RTTI base list is empty); `shared_ptr<ScnObj>` at 8, `vector<shared_ptr<CFileData>>` at 0x10, `shared_ptr<AnmObjChrRootScale>` at 0x1C. `CFileData` (partial): flags at 0 (type mask 0xC00, BRRES 0x800), `std::string` at 4, `shared_ptr` at 0x14, `ResFile` data pointer at 0x1C, `shared_ptr` at 0x20.

### F-370-1: Public FA asynchronous work-area ABI

`FACommandBlock` has size `0x170` and alignment 4: user pointer at 0, opaque SDK work array of 70 `u32` words at 4, and `FANandInfo` at `0x11C`. The NAND record has a 78-byte path and a 32-bit size at `0x50`, size `0x54`. Native attach stores the outer NAND size at `0x16C`; actual game consumers place a semaphore at `0x170` and callback result at `0x17C`. Ten Async API/callback declarations use native-proven argument positions and canonical opaque PF handle tags. The array contract is corroborated by a pinned public interface, without importing its restricted header or implementation. The internal operation qualifier remains unproven and absent. See [F-370-1](findings/370-fa-async-interface.md) for native consumer proof, provenance and complete remaining issue inventory.
## Game BModelObj model setup (#39)

For `wpse-v1` / `main.dol`, [F-39-2](findings/39-bmodelobj.md#f-39-2-model-setup-outline-detection-and-root-scale) names `BModelObj::SetModel(const shared_ptr<CModel>&, u32)` (`0x80053840`), `BModelObj::IsOutlineModel(const shared_ptr<CModel>&)` (`0x800536B0`, static) and `BModelObj::SetModelScaleDirect` (`0x80055980`), all descriptive. The `SetModel` flag word is `ModelFlag`: `MODEL_TYPE_MASK` 3, `MODEL_TYPE_SHADOW` 1, `MODEL_TYPE_OUTLINE` 2, `MODEL_SCALE_SIZE` 4. Outside the block, for the link and objdiff: `GetDefaultAllocator()` (`0x801B1360`, low confidence; returns the `MEMAllocator` at `.data` `0x803BF3A8`), `nw4r::g3d::AnmObjChrRootScale::Construct(MEMAllocator*, u32*, ResMdl)` (`0x801C6A80`, high for the NW4R construct signature), `AnmObjChrRootScale::SetScale(const VEC3&)` (`0x801C65E0`, medium; stores the scale at +0x28 and the result flags at +0x24), `DestroyG3dObj(G3dObj*)` (`0x801C7540`, low; tail-calls `G3dObj::Destroy`) and `CModel::SetRootScale` (`0x801B90A0`, medium). The class name `nw4r::g3d::AnmObjChrRootScale` and its base `AnmObjChr` are original, from RTTI and the vtable at `.data` `0x803C0030`. `ResUserDataEntryData` and `ResUserDataData` describe the user-data layout read by `IsOutlineModel`; their member names are descriptive. Template instances in the unit: `shared_ptr_deleter_common::~shared_ptr_deleter_common` (`0x80055BB0`), `shared_ptr_deleter<AnmObjChrRootScale, void (*)(G3dObj*)>` `get_deleter` (`0x8005A0E0`), `dispose` (`0x8005A130`), destructor (`0x8005A8D0`) and vtable (`.data` `0x803995BC`), the guard destructor `shared_ptr_holder<...>::~shared_ptr_holder` (`0x8005A6A0`) and the function-pointer type record `__RTTI__PFPQ34nw4r3g3d6G3dObj_v` (`.sdata` `0x804B5AC8`).
## Game CDlgPiiList (#82)

[F-82-2](findings/82-cdlgpiilist.md#f-82-2-unit-end-and-first-matched-functions) records `@unnamed@CDlgPiiList_cpp@::CDlgPiiList` (0x14 bytes, original RTTI name): `IDlgPiiList` vtable pointer at 0, `enable_shared_from_this<CDlgPiiList>` weak pointer at 4, `mImpl` (`shared_ptr<CDlgPiiListImpl>`, descriptive member name, medium confidence) at 0xC. `IDlgPiiList` and `IDlg` are original RTTI names; `IsClosable`, `Close`, `GetType` and `Show` are descriptive (`Show` medium confidence from the `ShowDlgPiiList` script binding). Renamed for objdiff: `Create__11IDlgPiiListFv` `0x800896E0`, `__dt__11IDlgPiiListFv` `0x80089850`, the `CDlgPiiList` destructor `0x8008FFD0`, `Metrowerks::move_ptr<CDlgPiiList>` destructor `0x8008F5D0`, and `shared_ptr_deleter<CDlgPiiList>` `get_deleter` `0x8008F070`, `dispose` `0x8008F0C0` and destructor `0x80090080`.
[F-82-2](findings/82-cdlgpiilist.md#f-82-2-unit-end-and-first-matched-functions) records `@unnamed@CDlgPiiList_cpp@::CDlgPiiList` (0x14 bytes, original RTTI name): `IDlgPiiList` vtable pointer at 0, `enable_shared_from_this<CDlgPiiList>` weak pointer at 4, `mImpl` (`shared_ptr<CDlgPiiListImpl>`, descriptive member name, medium confidence) at 0xC. `IDlgPiiList` and `IDlg` are original RTTI names; `IsClosable`, `Close`, `GetType` and `Show` are descriptive (`Show` medium confidence from the `ShowDlgPiiList` script binding). Renamed for objdiff: `Create__11IDlgPiiListFv` `0x800896E0`, `__dt__11IDlgPiiListFv` `0x80089850`, the `CDlgPiiList` destructor `0x8008FFD0`, `shared_ptr<CDlgPiiList>` destructor `0x8008F540`, `Metrowerks::move_ptr<CDlgPiiList>` destructor `0x8008F5D0`, and `shared_ptr_deleter<CDlgPiiList>` `get_deleter` `0x8008F070`, `dispose` `0x8008F0C0` and destructor `0x80090080`.
## Issue #366 FA standard wrappers

[F-366-1](findings/366-fa-standard-wrappers.md) records all twenty-six original FA public names and one-function source boundaries at `0x8021AAA0–0x8021B184`. Original spelling/file order and extents agree with pinned IPL `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`; every actual source instruction and all 71 callee/data relocation targets match this executable. Each wrapper has its own public header. The functions forward existing native PF_FILE, PF_DIR, PF_DTA and PF_STAT types; no existing record layout changes. PF_VOL_INF, PF_VOLUME_CONFIG and PF_CLUSTER_LINK_CONFIG are documented descriptive opaque forwarding tags, not recovered original spellings or invented storage layouts. The shared declarations are the owner-coordinated #882 prerequisite.


## Game-linked CNT stream wrapper (313)

`nw4r::ut::CNTHandleWithMutex` and `nw4r::ut::CntFileStream` are original names embedded in the executable. The standalone unit begins at `0x801AF780`, before the survey's stream-constructor boundary `0x801AF870`, and ends at `0x801AFC7C`. It uses the established game compiler and exception/RTTI flags despite its NW4R namespace. See [F-313-1](findings/313-cnt-file-stream.md).

| Type | Members in order | Confidence |
| --- | --- | --- |
| CNTHandleWithMutex, `0x44` bytes | initialized boolean `0`; OSMutex `4`; CNTHandle `0x1C` | High: constructor, initializer, release and stream call sites |
| CntFileStream, `0x68` bytes | FileStream base `0`; FilePosition `0x14`; CNTHandleWithMutex pointer `0x1C`; CNTFileInfo `0x20`; close-on-destroy boolean `0x64`; close-enable boolean `0x65` | High: all retained methods and virtual table |
| CNTHandleWithMutex::ScopedLock, `4` bytes | handle reference `0` | High layout/behavior; nested type name is descriptive |

The class virtual order follows canonical IOStream/FileStream. The derived table is `0x803BF2E0`; CodeWarrior RTTI for CntFileStream, FileStream and IOStream occupies `0x804B81D8`–`0x804B81F0`. CntFileStream's independent NW4R RuntimeTypeInfo object is `0x804B9130`, initialized to reference FileStream::typeInfo at `0x804B93C0`. `Init` and private member names are descriptive; class names and canonical inherited method names are original.
[F-85-1](findings/85-cdlgpiilist-instances.md) identifies two `CDlgPiiList.cpp` instances by their exception-table references and renames them for objdiff: `~shared_ptr<CDlgPiiListImpl>` `0x8008F0F0` and `~shared_ptr<CDlgPiiList>` `0x8008F180` (high confidence).

## Game BWorld (#42)

For `wpse-v1` / `main.dol`, [F-42-1](findings/42-bworld-unit.md) splits `game/BWorld.cpp`, `.text` `0x8005AAA0`–`0x80066454`. The class names `BWorld`, `ICalcDraw`, `ICalc`, `BWorld::EffectData` and `BWorld::OutlineData` and the base `std::tr1::enable_shared_from_this<BWorld>` (offset 4) are original, from RTTI and deleter type strings. Everything else is descriptive. The vtable at `.data` `0x80399DA0` has the destructor `0x8005C9F0`, `Calc` `0x80062200`, `CalcMtx` `0x80062980`, `CalcView` `0x80062C80` and `Draw` `0x80062D20`.

`BWorld` (0x90 bytes): `mFlags` at 0xC (1 in calc, 2 calc disabled, 4 and 8 skip objects by state, 0x10 pause requested, 0x20 resume requested, 0x40 paused, 0x80 sound listener), `mLastObjHandle` at 0x10, `mPendingFrames` at 0x14, `mDrawType` at 0x18, `shared_ptr<ScnRoot>` at 0x1C, the outline and model `shared_ptr<ScnGroup>` at 0x24 and 0x2C, `vector<weak_ptr<BWorldCamera>>` at 0x34 (at most 32), `weak_ptr<BWorldCamera>` current camera at 0x40, `weak_ptr<CShadowCamera>` at 0x48, `weak_ptr<BWorldObj>` ground at 0x50, `vector<ObjEntry>` slots at 0x58 with list head and tail at 0x64 and 0x68, `set<shared_ptr<BWorldObj>>` of objects removed during `Calc` at 0x6C, `shared_ptr<CKDTree<BModelObj>>` at 0x78, `shared_ptr<EffectData>` at 0x80 and `shared_ptr<OutlineData>` at 0x88. `ObjEntry` (0x10 bytes): flags (object type flags or-ed with 0x40000 until first calc), 1-based next and previous slot indices (`u16`) and the `shared_ptr<BWorldObj>`.

`BWorld::Desc` (0x1C bytes): constructor `0x8005AAA0`, setters `SetDrawType` `0x8005AAD0`, `SetObjectCapacity` `0x8005AAE0`, `SetOutlineCapacity` `0x8005AAF0`, `SetModelCapacity` `0x8005AB00`, `SetScnObjCapacity` `0x8005AB10`, `SetEffectCapacity` `0x8005AB20`, `SetCaptureEnable` `0x8005AB30` and `SetSoundListenerEnable` `0x8005AB40`; inline getters at `0x8005B120`–`0x8005B190` and `0x8005B270`. `BWorld::TextureState` (0x18 bytes, at `EffectData+0x48`): `Save(nw4r::ef::ResTexture)` `0x8005AB50`; `Restore` is inlined into the destructor and its own copy is stripped. Weak `nw4r::ef::ResTexture` members emitted here: `IsValid` `0x8005AC00`, `SetWidth` `0x8005AC20`, `SetHeight` `0x8005AC30`, `SetFormat` `0x8005AC40`, `SetMipmap` `0x8005AC50`, `SetTexture` `0x8005AC60`.

`EffectData` (0x64 bytes): last effect handle at 0, effect group IDs at 4 and 8, `vector<shared_ptr<CEffect>>` at 0xC, capture flag at 0x18, capture texture at 0x1C (0x2C bytes), `TextureState` at 0x48 and the capture group ID at 0x60. `OutlineData` (0x38 bytes): capture texture at 0, outline width at 0x2C, alpha at 0x30, `nw4r::ut::Color` at 0x34.

## FA sector I/O (#363)

[F-363-1](findings/363-fa-sector-volume.md) establishes the complete native pf_sector.c unit at 0x80214EC0–0x80215608: PFSEC_ReadFAT (0xC4), PFSEC_ReadData at0x80214F90 (0x2F4), PFSEC_WriteFAT at0x80215290 (0xE8), and PFSEC_WriteData at0x80215380 (0x288). Names and file order have high confidence from the pinned original FA map and all1,832 native bytes/34 original-name paired calls. FAT offset/size parameters are u16; data routines retain u32 byte offset/size and an output success count. Complete actual text allocation including alignment is1,864 bytes, with no owned data sections. Name-only outside PFDRV_IsInserted at0x801FBF20 is independently proven with a u32 return and PF_VOLUME* argument; its outside size/type/split are preserved. All516 existing source objects retain their allocated sections and canonical relocations, and the actual source-selected executable passes the frozen manifest. Four path tails and fifteen volume functions remain outside this source checkpoint.
### FA narrow standard API, issues 364 and 365

[F-365-3](findings/365-fa-api.md#f-365-3-remaining-narrow-standard-api-units)
records fifteen complete original-name standard wrappers at
`0x8021A470–0x8021AA94`, excluding the separately reconstructed chdir file.
The pinned FA symbol and file-order maps support every `pf2_` spelling;
actual lower native calls support each operation and argument width.
Public chdmod/chmod attributes and buffering mode are 32-bit, with u8 lower
contracts. `PF_NAND_FILE` is a descriptive 84-byte record: path at 0 with
chosen extent 78 and naturally aligned size at 80. The native 78-byte copy,
size access, public NAND argument names, and actual target-compiler layout
are evidenced; an original path[80] would be ABI-indistinguishable, so the
original bound and type/member spellings remain uncertain. Outside lower
functions receive name-only aliases and compatible declarations, with their
original extents, splits, types and source status preserved.

### FA CP932 codepage, issues 364 and 365

[F-364-1](findings/364-fa-cp932.md) establishes the complete six-function
CP932 unit at `0x80219D80–0x8021A2A8` and its original names from the pinned
FA map. The first three functions belong to #364 and the final three to
#365. The original cp932_to_unicode spelling and complete `const u16[45][189]`
shape come from byte-identical CC0 references; its native extent is
`0x803681C8–0x8036C43A`, 17,010 bytes. Six following zeros are alignment,
not additional array storage. The previous internal symbol at 0x803691CE
was a false pointer interpretation of two Unicode values; no native source
object or external reference requires that boundary. Every function size,
paired relocation, actual ELF placement, and complete source-selected DOL
is exact. No PF_VOLUME layout or preceding PFVOL unit changes.

### FA volume API extension, issue 364

[F-364-3](findings/364-fa-cp932.md#f-364-3-independent-volume-api-extension-unit)
identifies the separate original pf_volume_apiadd.c and its sole function
PFVOL_iswriteprotected at 0x80219D00, 120 bytes. Its signed-drive argument,
u32 output, external PFVOL_GetVolumeFromDrvChar call, mounted flag 8,
write-protection flag 2 and error 9 follow the native instructions. The
three paired relocations, actual source-selected object and complete DOL
are exact. This publishes no complete volume record-size change and earns
no credit for the preceding main-volume functions.
## F-363-3: Complete native volume storage

PF_VOLUME is0x1FAC, with cluster_link1F94, partition1FA0, signed-error callback1FA4 and format parameter1FA8; PF_VOLUME_SET is0x33800, with current_context_id64, four context records68 and26 volumes88. PF_CACHE_SETTING20 and PF_DRV_TBL12 are established by attach. PF_DEV_INF16, PF_VOL_INF24, PF_VOLUME_CONFIG16 and PF_CLUSTER_LINK_CONFIG12 cover all native-used output/configuration fields. Exact field tables, reference-name confidence, actual20-word layout proof,37-function735-target consumer proof and535-object preservation controls are recorded in [F-363-3](findings/363-fa-volume-interface.md). This is a header prerequisite, with source-selected acceptance pending.

For `wpse-v1`, `PF_FILE.open_mode` at offset `0x4` is `s32`, preserving the record's `0x30` size. The pinned TFP2 declaration and native `PFFILE_fclose` signed comparison establish the type; complete close code and all paired targets match, while the complete volume object and two existing entry consumers are byte-identical under original-versus-signed header controls. See [F-363-3](findings/363-fa-volume-interface.md).
## Game quarter-turn interpolation (#318)

[F-318-2](findings/318-game-utilities.md#f-318-2-bounded-quarter-turn-unit-proposal) records `SinQuarterTurn(float)` at `0x801B64B0` (20 bytes) and `OneMinusCosQuarterTurn(float)` at `0x801B64D0` (56 bytes). Both names and the filename are descriptive, high confidence in behavior and float calling convention; no original spelling is claimed. The ordinary canonical NW4R SinDeg/CosDeg consumers reproduce all resolved instructions. The float constants at `0x804BA3B8`, `0x804BA3BC`, `0x804BA3C0` are the 256/360 conversion, 90.0f and 1.0f. The last constant is four bytes; the following four zeros are alignment outside this unit. The complete source object and frozen source-linked DOL are exact. No class layout or render-mode interface is inferred from this bounded unit.

[F-318-3](findings/318-game-utilities.md#f-318-3-value-return-render-mode-interface) establishes the descriptive `ChooseDefaultRenderMode()` declaration at `0x801B66A0`, returning the canonical 60-byte `GXRenderModeObj` by value. Confidence in the physical typed ABI is high from independently exact producer and caller consumers; the original function spelling and exact source text are unrecovered. The caller takes the temporary address within a full expression, a supported CodeWarrior extension whose warnings remain visible. The producer remains original fallback until its complete owned unit is selected; this interface adds no source credit.
## Game BWorld constructor (#43)

[F-43-1](findings/43-bworld-ctor.md) matches `BWorld::BWorld(const Desc&)` `0x8005AC70`. Weak functions emitted after it: `Desc` getters `0x8005B120`–`0x8005B190` and `IsSoundListenerEnable` `0x8005B270`, `ScnRoot::Construct` overload `0x8005B140`, `WorkBuffer::GetBuffer` `0x8005B1A0` and `GetSize` `0x8005B1D0`, `ResTexture` copy constructor `0x8005B1E0`, `CaptureTexture::GetWidth`, `GetHeight`, `GetFormat`, `GetBuffer` `0x8005B1F0`–`0x8005B220`, `ut::Color::operator=(u32)` `0x8005B230`, `BWorld::SetFlag` `0x8005B240`, the `shared_ptr` `reset` instances `0x8005B280`–`0x8005B570`, `shared_ptr<CKDTree<BModelObj>>` constructor `0x8005B640`, `ICalcDraw` and `ICalc` constructor and destructors `0x8005B710`–`0x8005B760` and the `enable_shared_from_this<BWorld>` constructor and destructor `0x8005B7A0`, `0x8005B7B0`. Member constructors called from it: `0x8005B820`–`0x8005BC20`, `CKDTree<BModelObj>` `0x8005C170`, `EffectData` `0x8005C1A0`, `OutlineData` `0x8005C960`, `vector<shared_ptr<CEffect>>::resize` `0x8005C2D0`. External: `BWorld::EnableCalc` `0x8005D300`, `AllocEffectGroupID` `0x800A0970`, `FreeEffectGroupID` `0x800A0BE0`, `GetWorkBuffer` `0x800BE320`, `CaptureTexture` constructor `0x801C4910`, destructor `0x801C4930` and `Init` `0x801C4B30`. `CaptureTexture` (0x2C bytes): `GXTexObj` at 0, `shared_ptr<u8>` at 0x20, size at 0x28. `CKDTree<BModelObj>` (0x38 bytes): root `shared_ptr` at 0, `AABB` at 8, origin at 0x20, scale at 0x2C. All names are descriptive except the RTTI-backed class names in the #42 section; `CaptureTexture::Init` parameter types have medium confidence.
## Issue #314: Platform timing and NAND waits

[F-314-3](findings/314-platform-async-waits.md) proves four timing wrappers at `0x801AFC80` (`GetVideoRetraceCount`, 4), `0x801AFC90` (`WaitForVideoRetrace`, 4), `0x801AFCA0` (`GetVideoRetraceTicks`, `0x38`) and `0x801AFCE0` (`GetNominalVideoRetraceTicks`, 8). Nine asynchronous NAND waits and their completion callback occupy `0x801B0640`–`0x801B0EE8`; their exact entry addresses/sizes are in the finding's inventory. Both complete owned units are source-selected, with fourteen functions / 2,220 native instruction bytes, all six allocated sections at 100 percent, independently paired relocations and an exact frozen DOL.

`NandWaitContext` is `0xCC` bytes: canonical `NANDCommandBlock` base at zero, `OSSemaphore mSemaphore` at `0xBC`, and signed `s32 mResult` at `0xC8`. The native callback stores `mResult` and signals `mSemaphore`. `CScopedActivity` is an empty scope interface: its constructor at `0x801B5A80` increments the global activity count, and destructor at `0x801B5A90` decrements/clamps it; no object fields are accessed. `YieldFiber` at `0x8019E770`, size `0x11C`, yields through the existing fiber condition-variable path. Only these outside callee names change, leaving their sizes, source and split ownership intact. All new spellings are descriptive, medium spelling confidence; their behavior, argument types and caller-visible layout have high native confidence. The SDK function spellings come from existing canonical declarations. At the original F-314-3 checkpoint, eleven PrFILE2 entries remained; the complete source-selected acceptance is now recorded in F-314-5.


### Issue #314: FA wait consumers

[F-314-4](findings/314-platform-async-waits.md) proves all eleven native FA wait entries at `0x801AFCF0`–`0x801B0638`, totaling 2,308 instruction bytes, with 155 independently paired text/exception/index targets. Their source and method names are descriptive, with medium spelling confidence and high behavior/layout confidence. `FaWaitContext` is `0x180` bytes: canonical owner-approved `FACommandBlock` at zero, `OSSemaphore mSemaphore` at `0x170`, and signed `s32 mResult` at `0x17C`. The SDK base retains its independently evidenced opaque work-area contract rather than exposing guessed private fields. The ten actual SDK Async callees receive only canonical interface names in `symbols.txt`; their sizes, splits and source ownership remain unchanged. F-314-5 records the subsequent complete source-selected link, all 25 original issue entries / 4,528 bytes, all nine allocated sections, 218 code sites and 78 exception/index targets, and exact frozen DOL.
## Issue #315: game memory allocation interfaces

`GameMemory` and `memory.cpp` are descriptive names, not recovered original spellings. [F-315-1 and F-315-3](findings/315-memory-new-pad.md) establish the twelve-function unit at `0x801B0EF0-0x801B1450`, private `MEMAllocator` at `0x803BF3A8` (16 bytes), two-function `MEMAllocatorFunc` at `0x804B81F0` (8 bytes), and private heap handles at `0x804B9138/0x804B913C` (4 bytes each). Callback signatures follow the canonical SDK allocator interface. Public initialization, allocation/free, per-heap allocation, and allocator-accessor names describe the paired native behavior; they do not claim recovered symbol spellings. `__sys_alloc` and `__sys_free` retain existing names.

The standard operators at `0x801B1450`, `0x801B14E0`, `0x801B1580`, and `0x801B1590` are scalar new, array new, scalar delete, and array delete respectively. Every instruction and target is proven by the source-produced object and frozen linked DOL. The four-byte `__new_handler` pointer at `0x804B9BC0` retains its original automatic-object storage. The seventeen-byte `!std::bad_alloc!` type filter at `0x803BF3B8` is named `stdBadAllocExceptionName`; native pointer identity, rather than the compiler's numeric local-label spelling, establishes exception equivalence.


## FA FAT32 and initialization (#360)

[F-360-3](findings/360-fa-fat32.md) establishes the original names from the pinned FA map: `PFFAT32_ReadFATEntry` at `0x8020D420` (size `0x124`), `PFFAT32_WriteFATEntry` at `0x8020D550` (`0x10C`), `PFFAT32_WriteFATEntryWithBuf` at `0x8020D660` (`0x140`), `PFFAT32_ReadFATEntryWithBuf` at `0x8020D7A0` (`0x180`) and `PFFATFS_initializeFATFS` at `0x8020D920` (`4`). Naming confidence is high. The buffered functions take a direct `PF_CACHE_PAGE*`; all operations retain the canonical volume/cache layout. The initialization signature is `s32(u32, void*)` and its native body tail-calls `PFVOL_InitModule` at `0x80215EB0` with both arguments unchanged. No new record layout or global storage is introduced.
## Encoding converter reconstruction

[F-328-6](findings/328-encoding-converter.md#f-328-6-source-selected-converter-acceptance) establishes the descriptive `CEncodingConverter` class and seven methods at `0x801CA450`, `0x801CA690`, `0x801CA6A0`, `0x801CA6F0`, `0x801CA770`, `0x801CA7C0` and `0x801CA830`. Its complete size is `0x44`: canonical ENCContext members at `0x00` and `0x10`, followed by three twelve-byte member pointers at `0x20`, `0x2C` and `0x38`. The native local guards are `0x804B9178` and `0x804B9179`, derived from the actual ELF SDA base; earlier proposed `0x804B94B8` addresses were erroneous. SDK encoding names and types have high confidence; class/member/method spelling is descriptive. All seven compiled source functions, initialized data, guards, exception records, actual relocations and final linked addresses are exact; the two- and six-byte trailing alignments are not source storage.
For the first-owned FA FAT units, FAT12 cluster and write-value parameters are `u16`; FAT16 uses `u32`. Both buffered interfaces receive a direct `PF_CACHE_PAGE*`, as evidenced by their native fourth-argument accesses. The eight original names, exact function extents and source-unit boundaries follow the IPL inventory and native reference; canonical volume callback1FA4 is reused without a declaration change. See [F-359-1/2/4](findings/359-fa-fat-units.md).
## Complete native Stripe interface and retained local identities (#383, #384)

[F-383-4](findings/383-ef-stripe-native.md#f-383-4-source-selected-stripe-and-coordinated-smooth-stripe-acceptance) records complete source-selected identity. `DrawStripeStrategy::CalcVertexTube` (`0x80243040`), `GetInitialPrevAxis` (`0x80245290`) and its four ahead callbacks (`0x80245610`, `0x80245730`, `0x802459B0`, `0x80245C80`) have high-confidence function roles and medium-confidence original names from the identifier map/order and native bodies. The owned vtable at `0x803C5708` is `0x1C`; the last owned float at `0x804BAA58` is four bytes, excluding the following alignment. Two local SDK `GXEnd` functions have distinct native identities at `0x80243030` and `0x802476E0`; they require both unit and address when resolving ELF/map symbols.

The complete Stripe `AheadContextStripe` extends its `0xBC` base with emitter axis X at `0xBC`, view axis Z at `0xC8`, and a trigonometric-table pointer at `0xD4`, total `0xD8`. Its pointer is initialized to null before transforming the emitter axis. `VertexTube` is `0x28`: center at0, axis X at0xC, axis Z at0x18 and longitudinal texture coordinate at0x24. These roles and offsets are high confidence from exact source/native accesses; member spelling is descriptive. Stripe's private table is expressed as adjacent sine/cosine floats, and its source-derived `DrawTubeSegment` mangling ends `PCf`. The older unaccepted `Trigonometric` signature model for that Stripe helper was inferred, not an original frozen-target symbol; Smooth retains its independent canonical `Trigonometric` type unchanged.

`DrawParticle`'s ordinary inline declaration and earlier definition, and Smooth's later out-of-class inline `Vertex()` definition, preserve the original bodies and ABI while recovering native retained emission order. Their placement is inferred from native layout, not uniquely recovered original source spelling. No compiler-control pragma, SDK shadow helper or function-alignment change is introduced.

## F-368-1: Wide standard FA wrappers

The 24 original `pf2_w_*` functions at `0x8021B890–0x8021C330` are mapped to their individual source files in [F-368-1](findings/368-fa-wide-wrappers.md). Their combined native code is 2,616 bytes with 123 paired relocations and no allocated data. Original names/order/extents are independently confirmed against pinned CC0 Wii IPL's separate FA inventory. Paths and mode strings are `const u16*`, string mode is `PF_CODE_MODE_UNICODE`, and the opener uses `PFWAPI_ParseOpenModeString`. Public divide and xdivide wrappers share original `PFFILE_divide`.

`PF_InitLockFile` at `0x8021C330` remains original fallback. [F-368-2](findings/368-fa-wide-wrappers.md) establishes the typed `PF_SYSTEM_SET` size `0x23A0`, alignment 8, stub record size `0x2398`, and `flock_count` offset `0x2398`. The stub contains initialization word 0, thread handle 4, queue handle 8, stack `0xC`/`0x2000`, `OSThread` at `0x2010`, queue at `0x2328`, and 20 messages at `0x2348`. Original `pf_sys_set` names the existing `0x8041F170` object with unchanged extent and attributes. The external canonical record and initializer source define no storage; the complete first-owned file remains `NonMatching` and original fallback. The same source file's next two functions belong to assigned #369 and are untouched.

### F-368-3: Complete shared file lock

`PF_LockFile` at `0x8021C350` has native-proven `(PF_FILE* p_file, PF_FILE* p_owner)` arguments, including an unused owner value supplied by two independent callers. `PF_UnLockFile` at `0x8021C410` takes one file pointer. Canonical `PFK_SEMAPHORE_PARAMS`, embedded `OSSemaphore`, existing `PF_LOCK` offsets and system counter offset `0x2398` reproduce the entire shared `0x170` text extent. No storage extent changes. See [F-368-3](findings/368-fa-wide-wrappers.md#f-368-3-complete-shared-file-lock-source).
## F-367-1: Standard FA wrappers

The complete 25-function mapping, original file boundaries and target extents are recorded in [F-367-1](findings/367-fa-standard-wrappers.md). Original names have high confidence from the separate FA symbols and splits in pinned CC0 Wii IPL, corroborated by native lower-callee traces and exact source comparison. All 25 functions contribute 1,608 code bytes, with 75 paired relocations and no allocated data.

`PFWAPI_ParseOpenModeString` at `0x8021B7D0` is the wide parser in its own `pf_w_api_util.c` unit. `pf2_setupfsi` at `0x8021B5C0` forwards public `u32` flags to the lower `u16` parameter, reproducing the explicit native truncation. `PF_CLUSTER_LINK_CONFIG`, `PF_TAIL_BUF` and `PF_VOLUME_CONFIG` are only forwarded opaque C pointers; no storage layout or size is inferred here. Existing canonical volume, file, directory and cache layouts are unchanged. The findings preserve the five rejected native-exact identities and the caller/callee evidence that corrected their names and public argument counts.
### F-369-5: FA system storage and calendar records

`PF_SYSTEM_SET` has size `0x23A0` and alignment8, with `flock_count` at `0x2398`. The complete native-proven typed declaration is shared with #368; only `pf_system.c` defines storage. `PF_SYS_DATE` consists of three native32-bit year/month/day fields, size12; `PF_SYS_TIME` has four32-bit hour/minute/second/millisecond fields, size16. Related VF16-bit calendar records are another ABI. `PF_VOLUME_SET.current_context_id` extends only the existing external prefix at `0x64`, approved by its #363 owner; no volume storage or extent changes. Exact actual-source evidence is [F-369-5](findings/369-fa-filelock-stubs.md#f-369-5-split-baseline-and-independent-system-source).

## F-369-7: Private FA command and unchanged outside identities

The reconstructed private PF_STUB_COMMAND is 0x40 bytes with ordinary operation at4. Full member offsets, argument semantics, name confidence, all22 original function inventory proof and canonical dependencies are recorded in [F-369-7](findings/369-fa-stub-probes.md#f-369-7-remaining-source-prepared-without-source-selection). It overlays the evidenced public FACommandBlock asyncData work area without changing that public declaration. Six name-only metadata updates preserve all native extents: pf2_init_prfile2 8021B030/40, pfstub_call_function_unicode 8021D480/240, pfd_sddrv_is_media_insert 80220A50/98, pfd_mscdrv_is_media_insert 80222FC0/4, gOpenDisk 80421510/68 and gOpenPartition 80421578/68. The pinned FA map independently contains all six names; source ownership and storage remain with their original units. The fourteen newly materialized units remain NonMatching pending actual canonical/full-DOL acceptance.
## Issue #326: graphics state and display interface

[F-326-10](findings/326-graphics-utilities.md#f-326-10-bounded-graphics-translation-unit-acceptance) establishes six free functions in the descriptive `Graphics` namespace at `0x801C6DB0-0x801C7538`: Initialize, CopyFrameBuffer, DisplayFrameBuffer, ResetState, SetPixelFormat and SetCopyClear. The source filename and spellings are descriptive, with high behavior confidence and medium spelling confidence. GXPixelFmt, GXZFmt16, GXColor and GXRenderModeObj come from canonical SDK declarations; the native call sites and complete strict source object prove their use. The complete public interface is in `include/game/Graphics.h`.

Private state is closed within this unit: bool first-frame at `0x804B82F8` (one byte), cached GXPixelFmt at `0x804B82FC` and GXZFmt16 at `0x804B8300` (four bytes each), FIFO buffer and GXFifoObj pointers at `0x804B9160/0x804B9164`, two framebuffer pointers at `0x804B9168` (eight bytes) and current framebuffer pointer at `0x804B9170` (four bytes). The following small-data and BSS alignment gaps are outside these actual objects. Both static storage and all natural text alignment reproduce the native object. The outside `ChooseDefaultRenderMode()` name at `0x801B66A0` is coordinated with the same #318 owner, preserves the original size/type/split and implements no producer source here; its value-return ABI is paired independently with the actual graphics caller.

## Game BWorld member helpers (#44)

[F-44-1](findings/44-bworld-vector.md) names the vector destructors `0x8005B860` (`vector<weak_ptr<BWorldCamera>>`), `0x8005BB40` (`vector<BWorld::ObjEntry>`) and `0x8005C1F0` (`vector<shared_ptr<CEffect>>`), the `set<shared_ptr<BWorldObj>>` destructor `0x8005BC40`, `BWorld::~BWorld` `0x8005C9F0` (vtable slot 2), `BWorld::RequestPause` `0x8005D330` and `RequestResume` `0x8005D350`. `operator new[]` `0x801B14E0` and `operator delete[]` `0x801B1590` sit next to `operator new` and `operator delete` and are called for the `std::logic_error` message buffer. `BWorld::ObjEntry` has a constructor that zeroes only the flags. MSL class names `__vec_deleter` and `__vec_constructor` follow MSL; `__vec_split_buffer` and the helper names are descriptive.

## Game BWorld cameras and objects (#45)

[F-45-1](findings/45-bworld-cameras.md) names `GetCamera` `0x8005D370`, `AddCamera(shared_ptr<BWorldCamera>, u32)` `0x8005D400`, `SetCamera` `0x8005D510`, `DynamicCast<CShadowCamera>` `0x8005DC70`, `static_pointer_cast<CShadowCamera>` `0x8005DD90`, `AddCamera(shared_ptr<BWorldCamera>)` `0x8005DE90`, `AttachCamera` `0x8005DFB0`, `RemoveCamera` `0x8005E140`, `ClearCamera` `0x8005E160`, `GetShadowCamera` `0x8005E360`, `SetCurrentCamera` `0x8005E3D0`, `GetCurrentCamera` `0x8005E410`, `GetG3dCamera` `0x8005E480`, `SetOutlineAlpha` `0x8005E520`, `SetOutlineWidth` `0x8005E540`, `AddGround` `0x8005ED90`, `SetGround` `0x8005EEA0`, `Add` `0x8005EEE0`, `DynamicCast<BWorldCamera>` `0x8005F010`, `static_pointer_cast<BWorldCamera>` `0x8005F130`, `AddObject` `0x8005F230` and the weak `BWorldObj::OnEnterWorld` `0x8005F440`. `BWorldCamera` (RTTI name; derives from `BWorldObj`): slot index `int` at 0x48 (-1 when unassigned), first own virtual at vtable slot 0x48 (`0x80067DD0`, named `UpdateIndex`), `GetG3dCamera` `0x800666D0`. `CShadowCamera` (RTTI name; derives from `BWorldCamera`). `BWorldObj` type flags 2 (camera) and 4 (shadow camera); `BWorldObj::Pause` `0x8006A4B0`. `BWorld::ObjEntry` links: next slot at 4 and previous slot at 6, 1-based. Method names other than the RTTI class names are descriptive.

## Game BModelObj tail (#40)

For `wpse-v1` / `main.dol`, [F-40-1](findings/40-bmodelobj-tail.md) places `.text` `0x800561B0`–`0x80058B44` inside `game/BModelObj.cpp` (#39 to #42). Original standard-library names, from the `std::exception` and `std::logic_error` vtables (`.data` `0x80399820`, `0x803997B0`) and RTTI: `__dt__Q23std9exceptionFv` (`0x80058560`, 0x40), `what__Q23std11logic_errorCFv` (`0x800585A0`, 0x8), `__dt__Q23std11logic_errorFv` (`0x800585B0`, 0x90), `what__Q23std9exceptionCFv` (`0x80058640`, 0xC). Class names `BModelObj`, `BSoundSource` (base at 0x48), `BKDTreeLink` (base at 0x64), `IKDTreeLinkRemoved` and `KDTreeZXNode<2, 9>` are original RTTI spellings. The other 22 functions keep address labels; their vtable slots and behaviour are recorded in the finding. Confidence: high for the standard-library names and class names.
## FA SD card driver (#372)

For `wpse-v1` / `main.dol`, [F-372-3](findings/372-fa-sd-driver.md#f-372-3-names-layouts-and-reconstruction) names `pfd_st_inter_callback` (`0x8021FFC0`), `pfd_st_removal_callback` (`0x802200E0`), `pfd_sddrv_init` (`0x802201F0`) and `pfd_sddrv_mount` (`0x80220440`), all high from native diagnostics. The table entries `pfd_sddrv_format` (`0x80220640`), `pfd_sddrv_pread` (`0x802206E0`), `pfd_sddrv_pwrite` (`0x80220720`), `pfd_sddrv_unmount` (`0x80220760`) and `pfd_sddrv_finalize` (`0x802207A0`) are descriptive, low. Named for the link in #373's range: `pfd_sddrv_get_disk_info`, `pfd_sddrv_physical_read`, `pfd_sddrv_physical_write` and `pfd_sddrv_full_format` (high, diagnostics); `pfd_sddrv_init_drv_tbl`, `pfd_sddrv_set_media_callback` (medium); `SdGetDriveLetters` (`0x80222E90`, low). In #374's range: `ISD_Init`, `ISD_Open`, `ISD_Close`, `ISD_GetStatus`, `ISD_Reset`, `ISD_SendCommand`, `ISD_GetCardSize`, `ISD_RegisterDeviceIntrHandler` and `ISD_UnregisterDeviceIntrHandler`. The `ISD_` prefix comes from the native `ISD_WriteBlock()` diagnostic; the suffixes are low confidence. Data: `sSdFunctions` (`.rodata` `0x8036C460`, `PDM_FUNCTBL`), `sSdDriver` (`.bss` `0x80422BE0`, volatile `PFD_SD_DRIVER`: `status` `0x00`, `sector_size` `0x04`, `p_handle` `0x08`, `p_disk` `0x0C`, `inserted` `0x10`, `media_changed` `0x14`, `drive` `0x18`), `sSdHandle` (`0x80422C00`, `ISD_HANDLE` of `0x28`: `fd` `0x00`, `rca` `0x10`, `capacity` `0x18`–`0x20`, `opened` `0x24`), `sSdSectorBuffer` (`0x80422C40`, `0x200`, 32-byte aligned), `sSdInsertCallback` (`.sbss` `0x804B9240`), `sSdEjectCallback` (`0x804B9244`) and `sSdIntrEvent` (`0x804B9260`). The state, handle, and callback names are descriptive, low.

[F-358-1](findings/358-fa-pf-fat.md) names the rest of `pf_entry_iterator.c`: `PFENT_ITER_IsAtLogicalEnd` (`0x80208620`), `PFENT_ITER_MoveTo` (`0x80208730`), `PFENT_ITER_Advance` (`0x80208830`), `PFENT_ITER_Retreat` (`0x80208CC0`) and `PFENT_ITER_FindCluster` (`0x80208F20`), VF names, high; `PFENT_ITER_FindEntry` (`0x802093D0`) and `PFENT_GetEntryOfPath` (`0x80209420`), medium; `PFENT_ITER_IsInDirectory` (`0x80208930`), `PFENT_ITER_FindEntryOfPath` (`0x80209260`), `PFENT_GetParentEntryOfPath` (`0x80209430`), `PFENT_ITER_GetEntry` (`0x80209440`) and `PFENT_ITER_LoadLongName` (`0x802094B0`), low. [F-358-2](findings/358-fa-pf-fat.md#f-358-2-pf_fatc-boundaries-and-first-functions) names the first 17 functions of `pf_fat.c`, VF names without `VFi`, medium, except `PFFAT_ClearCluster` (`0x80209970`) and `PFFAT_ReadCluster` (`0x80209DD0`), low; the static `fat_special_values` table (`0x80368188`, 0x3C bytes); the callees `PFFAT_GetBeforeChain` (`0x8020BB30`), `PFFAT_RefreshFSINFO` (`0x8020C0C0`), `PFSEC_ReadFAT` (`0x80214EC0`), `PFSEC_WriteFAT` (`0x80215290`), medium.
## Issue #385: Complete smooth-stripe acceptance

[F-385-3](findings/385-complete-inventory.md) independently records all 17 original addresses and sizes / 13,224 instruction bytes in the already source-selected smooth-stripe unit. Every retained native function has its original actual ELF/map address, size and source owner; all 28 unit functions and all five sections compare at 100%, with a frozen exact full DOL. Existing names, signatures, layouts, shared interfaces, original function bodies and weak-helper placement remain canonical; this audit adds no symbol recovery or source-byte credit.

## Game CDividedGround first block (#60)

[F-60-3](findings/60-cdividedground-unit.md#f-60-3-cdividedground-layout-and-the-first-blocks-sources) names `CDividedGround::CDividedGround(u32)` `0x80071220`, `~CDividedGround` `0x80071830` (vtable `__vt__14CDividedGround` `0x8039BB68`, slot 0) and its `BSoundSource` thunk `0x800776A0`; the `list<shared_ptr<T>>` destructors for `CFileData` `0x80071410`, `nw4r::g3d::ScnGroupEx` `0x80071590`, `CModel` `0x80071670` and `ScnMdlSimpleData` `0x80071750`, the `__list_deleter<shared_ptr<CFileData>>` destructor `0x80071310` and that list's `begin` `0x800713F0` and `end` `0x80071400`; `vector<const char*>::~vector` `0x800714F0`; the `shared_ptr` default constructors for `CFileData` `0x80071570` and `CBSPTree` `0x80071580`; and the `shared_ptr` destructors for `CFileData` `0x800652D0`, `nw4r::g3d::ScnGroup` `0x800656C0`, `CBSPTree` `0x80076EF0` and `CPpuEngine` `0x80076F80`. `CDividedGround` (RTTI name, derives from `BModelObj`, 0x16C bytes, type flag 0x8000) members are in the finding; member names, `StageID` and `BSP_COUNT` are descriptive. `__list_deleter` and `__list_imp` follow the MSL naming of the vector layers; the list element types are inferred from their uses (medium confidence for `CFileData`).
## Issue #351: Original driver label names

[F-351-2](findings/351-driver-label-interface.md) corrects the provisional names at `0x801FBBE0` and `0x801FBC20` to the primary FA map spellings `PFDRV_LoadVolumeLabelFromBuf` and `PFDRV_StoreVolumeLabelToBuf`. Their native copy directions, two 60-byte extents, unchanged signatures and actual caller targets independently support the correction. The shared declarations and two clients are updated together, with 678 other complete source objects unchanged and only the expected symbol-name changes in the two affected objects and final ELF. Frozen DOL verification passes; this prerequisite adds no reconstructed-source credit.

## Game CDlgPiiSelector construction (#92)

For `wpse-v1` / `main.dol`, [F-92-2](findings/92-cdlgpiiselector-construction.md#f-92-2-identities-of-the-23-functions) names 22 of the 23 functions at `0x80092F70`–`0x80093534` with the mangled names the reconstruction produces: the `CDlgPiiSelector`, `IDlgPiiSelectorInternal` and `IDlgPiiSelector` constructors, the two interface destructors, the `enable_shared_from_this`, `weak_ptr` and `shared_ptr` helpers for `CDlgPiiSelector`, `CDataPerPlayer` and `CDlgPiiSelector::MultiPlayerData`, and the `move_ptr`, `compressed_pair`, `default_delete` and `shared_ptr_deleter` members used by `shared_ptr<CDlgPiiSelector>(CDlgPiiSelector*)`. Class names are original RTTI spellings, high. The storage class name `CompressedPairStorage` inside two symbols is the repository's, low. `0x800931F0`, an empty member taking the factory's `bool`, keeps its address label. `CDlgPiiSelector` is 0x40 bytes: `enable_shared_from_this` base at 4, `shared_ptr<CDataPerPlayer>[4]` at 0xC, `shared_ptr<MultiPlayerData>` at 0x2C, `bool`s at 0x34 and 0x35, words at 0x38 and 0x3C; member names not chosen.
## Issue #351: Complete FA driver and disk manager

[F-351-3](findings/351-driver-reconstruction.md) records all 22 original issue addresses and sizes / 2,888 bytes, accepted across three source-selected units. The seventeen original `PFDRV_*` names and five `pdm_*` wrapper names are primary FA map facts with exact native sizes and behavior. The driver spans `0x801FBBE0` to `0x801FC544`; the disk manager spans `0x801FC550` to `0x801FC620` and owns the proven `0x8F4`-byte disk-set global at `0x803EAFA8`. The preceding 13-iteration callback loop processes 26 complete `0x1FAC`-byte volume records; it does not establish a `0x3F58` record type. All 33 native functions in the containing units, allocated sections, actual ELF/map addresses, source-only link inputs and frozen DOL pass. The private inlined result-converter name is descriptive and carries no separate native function credit.

## Game CDividedGround second block (#61)

[F-61-1](findings/61-cdividedground-second-block.md) names `CDividedGround::LoadAttr(const char*, u32)` `0x80073640` (`LoadAttr__14CDividedGroundFPCcUl`, public, called only from `ScriptBinding_LoadGroundAttr`), the overload `LoadAttr(const StageID*, int, const char*, u32)` `0x80073660` (`LoadAttr__14CDividedGroundFPC7StageIDiPCcUl`, private, loads the two BSP files and trees) and the static `FindIndexSuffix(const char*)` `0x80073F70` (`FindIndexSuffix__14CDividedGroundFPCc`, private). The names are descriptive; the type of the last `LoadAttr` parameter follows the unresolved loader parameter of F-326. The finding identifies the other 25 functions of the block, which keep their `fn_` names until reconstructed.
## Issue #765: Complete disk-manager inventory

[F-765-2](findings/765-complete-inventory.md) independently accepts `pdm_open_partition` at `0x801FC5B0` / 56 bytes and `pdm_close_partition` at `0x801FC5F0` / 48 bytes, from the complete unit accepted in #970. Both original rows, all five native functions, text/BSS sections, actual ELF/map addresses and source owner, source-only link dependencies, disk-set storage extent and frozen full DOL pass. No interface, symbol, layout or implementation changes and no new source-byte credit accompany this audit.

## Game session and selector interfaces (#91)

[F-91-2](findings/91-cdlgpiiselector.md#f-91-2-factory-and-session-interface) names, in `.text`: `IDlgPiiSelector::Create(bool)` `0x80092EB0` (high, matching the `IDlgPiiList::Create` pattern), `CDlgPiiSelector::Init(bool)` `0x800931F0` (low), `IGameSession::GetInstance()` `0x800BAD40` (medium, the getter of the `.sbss` word `0x804B9000`), `CRemoteMemory::WaitUntilIdle(bool)` `0x801081E0` (medium), `IDlg()` `0x80084E90` and `~IDlg()` `0x80077820` (high), the weak `shared_ptr_deleter_common` members `retain` `0x80085350`, `attach_weak` `0x800854B0` and its constructor `0x80085600` (high), and the selector template members `0x80093540`, `0x8009B910`, `0x8009BCF0`, `0x8009BDD0`, `0x8009C250`, `0x8009C2E0` and `0x8009C340` (high, mangled names the source reproduces). In `.data` it names the vtables `CDlgPiiSelector` `0x8039F9A8`, `IDlgPiiSelectorInternal` `0x8039FAE0` and `IDlgPiiSelector` `0x8039FD20` (high). `include/game/ISession.h` declares `ISession` (`ICalcDraw` plus `enable_shared_from_this<ISession>` at 4, one slot, `Run`). `include/game/IGameSession.h` declares `IGameSession` slots 6 to 24. Slots 25 to 41 are not declared yet. The slot names and their confidence are in F-91-2. The member names of `CDlgPiiSelector` are `mDataPerPlayer` (0xC), `mMultiPlayerData` (0x2C), `mIsPausing` (0x34), `mInputLocked` (0x35), `mErrorState` (0x38) and `mError` (0x3C), all low except the first two (medium).
## Game CNoticeDisplay split (#159)

[F-159-2](findings/159-cnoticedisplay-split.md#f-159-2-the-cnoticedisplay-functions-of-159) names `WaitMotion(shared_ptr<CLayout::CMotionProgram>, bool)` `0x800CFF50` and `WaitMotionStart(shared_ptr<CLayout::CMotionProgram>)` `0x800D0030` (descriptive, low spelling confidence, high behaviour confidence) and the outside getter `GetUIInput()` `0x80124A40`, which returns the 0x40-byte input object at `.bss` `0x803E9088` used through `IUIInput` slots 0 and 4 (descriptive, medium). `CLayout::CMotionProgram` (`include/game/CLayout.h`, class names original from the deleter RTTI at `0x803C0760`) is 0x18 bytes: `f32 mFrame` 0, `f32 mRate` 4, `nw4r::lyt::AnimTransform* mpAnimTransform` 8, `mpTarget` 0xC (result of `0x801D2180`), `u16 mLastFrame` 0x10 (frame size, minus one unless looping), `int mState` 0x14 (0 before start, 2 or 3 finished); member names descriptive, offsets from its constructor in `0x801D1FE0`. `INoticeDisplay` (`include/game/CNoticeDisplay.h`) follows the twelve slots of F-160-2; the names of slots 5 to 11 (`IsShown`, `Show`, `Close`, `Clear`, `SetWinner`, `SetWinnerParam`, `GetType`) and of `CreateNoticeDisplay` are descriptive, low confidence.

## Game CGameEnding first block (#116)

[F-116-3](findings/116-cgameending-unit.md#f-116-3-the-15-functions-of-116) names the interface destructors `IGameMap::~IGameMap()` `0x800A5980` (`__dt__8IGameMapFv`, slot 0 of the `IGameMap` vtable `0x803A1D00`) and `IGameEnding::~IGameEnding()` `0x800A59F0` (`__dt__11IGameEndingFv`, slot 0 of `0x803A1BF8`). Class names are original RTTI spellings, high confidence. `IGameMap` derives from `ICalcDraw` and has `std::tr1::enable_shared_from_this<IGameMap>` at 4; `IGameEnding` derives from `IGameMap`; `@unnamed@CGameEnding_cpp@::CGameEnding` is 0x310 bytes and derives from `IGameEnding`. The factory `0x800A4EF0`, the `list<wstring>` destructor `0x800A5A60` and the eleven inline virtuals keep their address labels.
## Issue #763: Checked stream loop mask-table base

[F-763-2](findings/763-mask-table-base.md) recovers the checked writer's saved `r31` loop table base at `0x80368118` through a read-only local pointer. Its 432-byte extent, interfaces, data layout and compiler remain unchanged. Independent resolution of all six HA/LO sites measures 23 remaining native register-word differences, down from 26, with no additional whole-function match or source-selected code credit. The nine preceding functions and rodata remain exact, and the unit remains NonMatching.

## Game CDlgPiiSelector update and draw block (#93)

[F-93-1](findings/93-cdlgpiiselector-calc-block.md#f-93-1-identities-of-the-27-functions) names, in `.text`, with the mangled names the source produces: `CDlgPiiSelector::CalcEntrance()` `0x80093A10` and `AssignControllers()` `0x80093A70` (low), the anonymous-namespace `CControllerList` constructor `0x80093B20`, `Collect()` `0x80093B40`, `AddType(IController::Type)` `0x80093C50`, `Add(IController*)` `0x80093CD0` and `Get(int)` `0x80093DA0` (class and method names descriptive, low), `shared_ptr<CDlgPiiSelector::MultiPlayerData>::operator->` `0x800936E0` (high), `GetController(IController::Type, int)` `0x80124A20` (descriptive, medium; returns entry `[type][channel]` of the `IController*` table `0x803B3030`) and `CPlayer::SetController(IController*)` `0x800F81C0` (descriptive, high; stores `CPlayer` + 0xC). `CControllerList` is 0x14 bytes: the count at 0 and four `IController*` at 4. `CDlgPiiSelector::MultiPlayerData` is 0x180 bytes; the source declares `mState` (0), `mEntrancePPD[4][4]` (4) and `mPlayerNum` (0x84), low, and F-93-1 records the `CLayout` at 0x88 and the shared pointers at 0x170 (`IDlgMsgWin`) and 0x178. `0x80093630` is the `IGameMap` slot 15 inline default (the notice-display getter) and keeps its address label.

[F-93-4](findings/93-cdlgpiiselector-calc-block.md#f-93-4-calcview-calcmtx-and-their-helpers) adds `CDlgPiiSelector::CalcView()` `0x80093700` and `CalcMtx()` `0x800937E0` (ICalcDraw slot names, high), `CDataPerPlayer::CalcView()` `0x800937A0` and `CalcMtx()` `0x80093880` (high, they forward the same slots), the `shared_ptr<CDataPerPlayer>` and `shared_ptr<MultiPlayerData>` boolean conversions `0x80093640` and `0x800936C0`, `shared_ptr<CDataPerPlayer>::operator->` `0x800936B0` and `shared_ptr<IDlgMsgWin>::operator->` `0x800936F0` (high), the implicit `~CDlgPiiSelector()` `0x8009E060` (high; vtable slot 0) and `~shared_ptr<MultiPlayerData>` `0x8009BC10` (high; called only by that destructor, on the member at 0x2C). `MultiPlayerData` now declares `mLayout` (`CLayout`, 0x88) and `mMsgWin` (`shared_ptr<IDlgMsgWin>`, 0x170); both member names are descriptive, medium.

## Game CDlgPiiSelector entrance block (#94)

[F-94-1](findings/94-cdlgpiiselector-entrance-block.md#f-94-1-identities-of-the-24-functions) names, in `.text`, with the mangled names the source produces: `CDlgPiiSelector::IsShown(int)` `0x80094130` and `IsMultiPlayerEntranceShown()` `0x80094100` (high, script bindings), `GetEntrancePPD(IController*)` `0x80094C00` and `SetEntrancePPD(IController*, const shared_ptr<CPiiPersonalData>&)` `0x80094CF0` (medium, F-91-2), `CDataPerPlayer::IsShown()` `0x800941D0` (low; returns the `bool` at 0x50, declared but not defined), `CDataPerPlayer::IsFinished()` `0x80093F80` (low; forwards the `CFiber` at 0xD8), `CFiber::IsFinished() const` `0x80093F90` (high, the header body), `shared_ptr<CUIPiiSpec>::operator->` `0x80093F70` (high, member 0x9C), and the `shared_ptr<CPiiPersonalData>` default constructor `0x80094CE0`, copy constructor `0x80089680`, `operator=` `0x8009C0E0` and `swap` `0x8009C130` (high). The other fifteen functions of the block keep their address labels; F-94-1 records what each does.

## Game NPpu VM (#281)

[F-281-2](findings/281-nppu-vm.md#f-281-2-classes-and-the-first-blocks-sources) names, in `.text`: `NPpu::CCode::CCode()` `0x8016FC10`, `~CCode()` `0x8016FC30` (vtable `__vt__Q24NPpu5CCode` `0x803B842C`: destructor, `AddRef` `0x80170920`, `Release` `0x801706B0`), `CCode::Load` `0x8016FCD0`; `NPpu::CValue::ToInt` `0x8016FEB0`, `IsFloat` `0x8016FF10`, `IsNull` `0x8016FF30`, `ToBool` `0x8016FF60`, `ToFloat` `0x8016FFF0`, `GetReference` `0x80170060`, `Increment` `0x80170180`, `SetReference` `0x80170270`, `Decrement` `0x80170280`; `NPpu::CEngine::ParseInt` `0x80170070` (0x7C bytes, absorbing `0x801700E8`) and `ParseWInt` `0x801700F0` (0x84 bytes, absorbing `0x80170170`), `CEngine()` `0x801704C0`, `~CEngine()` `0x80170510`, `Init` `0x801706C0`; `NPpu::CEnvironment::CEnvironment(int, const SNativeFunction*, u32)` `0x80170370`, `~CEnvironment()` `0x80170420` (vtable `__vt__Q24NPpu12CEnvironment` `0x803B8418`: destructor, `AddRef` `0x80170930`, `Release` `0x801706A0`), `GetNative` `0x80170490`. Data: `CEngine::sNextID` `0x804B7B58` (initial 1), `CEngine::sStrings` `0x804B90E8`, `CEngine::sWStrings` `0x804B90EC`, `CEngine::sInstances` `0x804B90F0`. `CCode`, `CEnvironment` and `CEngine` are original RTTI names (`CEngine` from the `CPpuEngine` base list, [F-282-1](findings/282-nppu-engine.md#f-282-1-the-interpreter-class-is-nppucengine)); `CValue` and every member and method name are descriptive (medium confidence for the value conversions, low for `TYPE_FUNCTION`, `SNativeFunction::mArgCount` and the `CEngine` state values).

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `CValue` | `0x0` | `mType` | `int` | type tests in every conversion |
| `CValue` | `0x4` | payload union | `int`, `f32`, string pointers, `CValue*` | `ToInt`, `ToFloat`, `Increment` |
| `CCode` | `0x0`–`0x1C` | `mName`, `mStackSize`, `mCodeSize`, `mStringCount`, `mWStringCount`, `mCode`, `mStrings`, `mWStrings` | see header | `Load` header copy and string tables; caller `0x801045B8` passes the name |
| `CCode` | `0x20` | vptr | | constructor |
| `CEnvironment` | `0x0`–`0xC` | `mGlobalCount`, `mGlobals`, `mNativeCount`, `mNatives` | `int`, `CValue*`, `u32`, `const SNativeFunction*` | constructor, `GetNative`, F-29-1 |
| `CEnvironment` | `0x10` | vptr | | constructor |
| `CEngine` | `0x0`–`0x14` | `mID`, `mCode`, `mEnv`, `mFrameCapacity`, `mBaseCount`, `mHandlerCapacity` | | constructor, `Init` (caller passes 64, 32, 8) |
| `CEngine` | `0x18`–`0x44` | `mBuffer`, `mState`, `mPC`, `mSP`, `mStack`, `mFrameCount`, `mFrames`, `mReturnCount`, `mReturns`, `mBases`, `mHandlerCount`, `mHandlers` | | `Init` layout; frame push/pop `0x80170B50`/`0x80170BD0`, handler push `0x801735C8` |
| `CEngine` | `0x48`, `0x50` | `mThis`, `mThrown` | `CValue` | `ScriptBinding_GetThisObject` via `0x80170960`, `ScriptBinding_GetThrownValue` via `0x801741F0` |
| `CEngine` | `0x58`, `0x5C` | `mInterrupted`, `mInterruptValue` | `bool`, `CValue` | `ScriptBinding_Interrupt` via `0x80171010` |

## Game NPpu engine (#282)

[F-282-2](findings/282-nppu-engine.md#f-282-2-identities-of-the-27-functions) names, in `.text`, with the mangled names the source produces: `NPpu::CEngine::SetThis` `0x80170940`, `GetThis` `0x80170960`, `PopArray` `0x80170980`, `IsIntArg` `0x80170A20`, `IsStringArg` `0x80170A60`, `GetArg` `0x80170AA0`, `GetArgInt` `0x80170AD0`, `PushFrame` `0x80170B50`, `OnLineEnd` `0x80170BB0`, `OnLineBegin` `0x80170BC0`, `PopFrame` `0x80170BD0`, `Run` `0x80170C70`, `Reset` `0x80170CE0`, `Call` `0x80170D20`, `PushReturn` `0x80170FD0`, `Interrupt` `0x80171010`, `GetVariable` `0x80171030`, `UnaryOp` `0x80171080`, `BinaryOp` `0x80171400`, `BitwiseOp` `0x80171CE0`, `CompareOp` `0x80172170`, `Execute` `0x80172980`, `Unwind` `0x801737A0`; `NPpu::CValue::SetFunction` `0x801713F0`, `SetString` `0x80173780`, `SetWString` `0x80173790`. Weak copies kept by earlier files: `CValue::operator=` `0x801100D0`, `IsString` `0x801100A0`, `IsWString` `0x80110070`, `SetInt` `0x800C98C0`, `SetFloat` `0x801328F0`; `CEngine::Push` `0x801319F0`, `Pop` `0x801323D0`, `Fetch` `0x80132380`, `PushInt` `0x801319B0`, `PushFloat` `0x801328B0`, `PopInt` `0x80133500`, `PopFloat` `0x801323A0`, `PopReference` `0x801334D0`, `Drop` `0x8014FB00`. Jump tables: `0x803B8298` and `0x803B82BC` (`CompareOp`), `0x803B82E0` (`Execute`, 78 opcodes). Method, opcode and state names are descriptive (high confidence for stack, frame and handler operations, medium for opcodes).

| Struct | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `CEngine::SFrame` | `0x0`–`0x8` | `mSP`, `mBaseIndex`, `mSavedBase` | `int` | `PushFrame`, `PopFrame` |
| `CEngine::SHandler` | `0x0`–`0x8` | `mPC`, `mSP`, `mFrameCount` | `int` | opcode 0x45 in `Execute`, `Unwind` |
## Issue #358: Exact FAT core prefix

[F-358-3](findings/358-sector-registers.md) independently verifies the existing seventeen native core functions from `0x802095E0` through `PFFAT_GetSectorSpecified` at `0x8020B290`, including all 113 prefix relocation targets. The quotient-derived remainder restores the native `PFFAT_GetSector` register assignment without changing any symbol name, signature, layout or boundary. All 29 original issue addresses and sizes are recorded; twelve iterator functions are already source selected, while the exact core prefix remains NonMatching pending eighteen #359 tail functions. The iterator data audit distinguishes its 22 exact payload bytes from two zero alignment bytes. No additional source-selected credit is claimed.

## Issue #792: ScnGroupEx and native gather helpers

[F-792-2](findings/792-scngroupex.md) establishes ScnGroupEx (original type spelling, high confidence), derived from ScnGroup, with a gather pointer at `0xE8` and total size `0xEC`. Its Construct at `0x80270040`, G3dProc at `0x802701E0`, destructor at `0x80270510`, and three RTTI methods at `0x802706A0`, `0x80270710`, `0x80270740` reproduce their native addresses and sizes. The type record is `0x8036C9C8`, vtable `0x803C6110`. The helper names `detail::ModelDrawGather` and `detail::GatherProxy`, their method and field names, and the source basename are descriptive, low confidence. Their proven vtables are `0x803C60A8` and `0x803C60E4`; sizes are `0x24` and `0xC`. The model resource and bytecode fields occupy `0x18`, `0x1C`, `0x20`; the proxy's two gather pointers occupy 4 and 8. Its ninth virtual slot is evidenced without changing the shared IScnObjGather declaration. All24 native bodies and105 paired relocation targets are independently exact; all32 original issue rows, ELF/map ownership, source-only link inputs and the frozen DOL pass. Seven discarded helper copies receive no credit.
## Game CPpuEngine (#195)

[F-195-2](findings/195-cppuengine.md#f-195-2-cppuengine-layout-and-virtual-functions) and [F-195-3](findings/195-cppuengine.md#f-195-3-the-functions-of-195) name, in `.text`: `CPpuEngine::CPpuEngine()` `0x800FE5F0`, `~CPpuEngine()` `0x800FF0A0` and its `ICalc` thunk `0x801035C0`, `Calc()` `0x80100C90` and its thunk `0x801035B0`, `GetEnv() const` `0x800FFA70`, `SetDefaultEnvironment` `0x800FFA80`, `SetStageDescTarget` `0x800FFAC0`, `SetDefaultStageDescTarget` `0x800FFAD0`, `SetPPD` `0x80100650`, `LeaveInitPart` `0x801006D0`, `Load(const char*, int, const shared_ptr<NPpu::CEnvironment>&)` `0x80100920`, `Load(BWorldObj*, ...)` `0x80100AF0`, `Execute` `0x80100BA0`; the weak destructors of `enable_shared_from_this<CPpuEngine>` `0x800FE6A0`, `map<int, list<int> >` `0x800FE710`, `set<shared_ptr<CFileData> >` `0x800FEAF0` and `vector<MTX34>` `0x800FF020`, the `set` node destroy `0x801025E0` and the `pair<const int, list<int> >` allocator destroy `0x80103360`. Outside the block, for the link: `enable_shared_from_this<BWorldObj>::shared_from_this()` `0x800951D0` (high, same call shape), `BWorld::RemoveObject` `0x8005F450` (F-44), and the descriptive `NPpu::CEngine::Run` `0x80170C70`, `SetThis` `0x80170940`, `SetID` `0x801738B0` and `SetNextID` `0x801738C0` (medium). Statics in `.sbss`: `sDefaultEnv` `0x804B9048`, `sDefaultStageDescTarget` `0x804B9050`, `sScriptRoot` `0x804B9058` and the temporary-ID guard `0x804B9060`. `CPpuEngine` (0xC4 bytes) derives from `NPpu::CEngine` at 0, `ICalc` at 0x64 and `enable_shared_from_this<CPpuEngine>` at 0x68; vtable `0x803A9730`. Class and base names are RTTI names; method names follow the calling script bindings where noted in F-195-3 and are otherwise descriptive. `StageID` (4 signed bytes) follows `ScriptBinding_MakeStageID`; `CPPD` is descriptive (low).

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `CPpuEngine` | `0x70` | `mFlags` | `u32` | `LeaveInitPart` sets bit 0; `Calc` |
| `CPpuEngine` | `0x74` | `mOwnerObj` | `BWorldObj*` | `Load(BWorldObj*, ...)`, `Calc`, `GetWorld`, `0x80100DB0` |
| `CPpuEngine` | `0x78` | `mCallbacks` | `std::map<int, std::list<int> >` | constructor, destructor `0x800FE710`, `ScriptBinding_InsertCallback` via `0x801012D0` |
| `CPpuEngine` | `0x88` | `mOwner` | `std::tr1::shared_ptr<BWorldObjOwner>` | out-of-line constructor `0x801119B0` and destructor `0x801119C0`; passed to `BWorldObj` 0x24 (F-231-2) |
| `CPpuEngine` | `0x90` | `mStageDescTarget` | `StageID` | constructor, `SetStageDescTarget`, `Load(const char*, ...)` |
| `CPpuEngine` | `0x94` | `mFiles` | `std::set<std::tr1::shared_ptr<CFileData> >` | `0x800FFAE0`, destructor `0x800FEAF0` |
| `CPpuEngine` | `0xA0` | `mPlacements` | `std::vector<nw4r::math::MTX34>` | `0x800FFC50`, destructor `0x800FF020` |
| `CPpuEngine` | `0xAC` | `mPPD` | `std::tr1::shared_ptr<CPPD>` | `SetPPD` from `ScriptBinding_NewPPD` |
| `CPpuEngine` | `0xB4`, `0xB8` | `mCachedPPDValue`, `mCachedPPD` | `int`, `std::tr1::weak_ptr<CPPD>` | `0x80100F70` |
| `CPpuEngine` | `0xC0` | vptr | | constructor; slots `~CPpuEngine` (0x1C) and `Calc` (0x20) |
## Game CMultiRanking split (#158)

[F-158-2](findings/158-cmultiranking-split.md#f-158-2-class-shape-and-the-helper-at-0x800cd730) names `GetNodePosition(shared_ptr<BModelObj>, const char*)` `0x800CD730` (`GetNodePosition__FQ33std3tr122shared_ptr<9BModelObj>PCc`; name descriptive, low; behaviour high). `CMultiRanking` (RTTI `0x804B6CA0`, original) derives from `IMultiRanking` (RTTI `0x804B6CA8`, original) and `IGameMap`; it is 0x144 bytes, its vtable is `0x803A563C` and its deleter vtable `0x803A55B8`. The shared pointer array at 0x40 holds four `BModelObj` (high); the other member types are not yet recorded.

## Game CMultiRanking layout (#159)

[F-159-3](findings/159-cmultiranking-layout.md#f-159-3-members-virtual-slots-and-emission-order) records the 0x144-byte `CMultiRanking` layout: `shared_ptr` members `BWorld` 0x10, `BWorldCamera` 0x18, `CUIManager` 0x20, `BUILayout` 0x28 and 0x30, `IPauseMenu` 0x38, arrays of four `BModelObj` 0x40, `CPii` 0x60, `BUILayout` 0x80 and `CUIPlayerID` 0xA0 (high, from deleter vtables and constructors), `nw4r::snd::SoundHandle` 0x134 (high) and the active flag 0x141 (medium). Class names are original RTTI spellings; member names are not recovered. Slot overrides: 0 `0x800CF650`, `Calc` `0x800CF1D0`, `CalcMtx` `0x800CF070`, `CalcView` `0x800CF000`, `Draw` `0x800CEFA0`, `IGameMap` slots 5 `0x800CEB20`, 6 `0x800CF410`, 7 `0x800CF420` and 20 `0x800CF400`; all keep their address labels.

## Game CStageCamera (#209)

[F-209-2](findings/209-cstagecamera.md#f-209-2-cstagecamera-layout-and-virtual-functions) and [F-209-3](findings/209-cstagecamera.md#f-209-3-the-functions) name, in `.text`: `CStageCamera::CStageCamera(u32)` `0x801128C0`, `~CStageCamera()` `0x80112910`, `LoadScript(int, const shared_ptr<NPpu::CEnvironment>&)` `0x801129B0`, `GetStickWorldAngle(const IUIInput*)` `0x80112B00`, `UpdateLightPosition()` `0x80112CA0`, `Calc()` `0x80112D90`, `UpdateIndex(int)` `0x80112DF0`, `OnLeaveWorld()` `0x80112E90` and `GetPpuEngine() const` `0x80112EE0`; vtable `__vt__12CStageCamera` `0x803ADF44` and type info `__RTTI__12CStageCamera` `0x804B7380`. `CStageCamera` is an RTTI name; method and member names are descriptive (medium, `mReserved` low). For the link, outside the block: `BWorldCamera` constructor `0x80066460`, destructor `0x800665B0`, `GetG3dCamera` `0x800666D0`, `SetPosition` `0x80067CD0`, `Calc` `0x80067D60`, `OnEnterWorld` `0x80067DB0`, `OnLeaveWorld` `0x80067DC0`, `UpdateIndex` `0x80067DD0` and type info `0x804B5B78`; `BWorldObj` `GetSize` `0x800605C0`, `CalcMtx` `0x80062C70`, `GetDrawLayers` `0x80067E90`, `SetVisible` `0x80067EA0`, `SetRotation` `0x80067ED0`, `SetScale` `0x80067EE0` (all from the `BWorldObj` vtable `0x8039A2B8`, high) and slot 0x2C renamed from `GetOwner` to `GetPpuEngine` `0x80057CF0`; `GetDisplayAspectRatio()` `0x801B65E0` and `NormalizeSignedAngle(f32)` `0x801B7280` (descriptive, F-318-1); the `shared_ptr_deleter<CPpuEngine, default_delete<CPpuEngine> >` vtable `0x8039BAEC`, type info `0x804B5F00`, `get_deleter` `0x80076C30`, `dispose` `0x80076C80` and destructor `0x80077560`, and the `Metrowerks::move_ptr<CPpuEngine, ...>` destructor `0x800773B0` (high, from the vtable slots and `LoadScript`'s exception actions). The type info at `0x804B5A90` is now spelled `__RTTI__Q33std3tr135enable_shared_from_this<9BWorldObj>`; the previous length prefix 39 was wrong.

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `BWorldCamera` | `0x48` | `mIndex` | `int` | constructor (-1), `BWorld::SetCamera`, `GetG3dCamera` |
| `BWorldCamera` | `0x4C` | `mNextMotionID` | `u32` | constructor (1), `0x800677C0` |
| `BWorldCamera` | `0x50` | `mAnmFile` | `std::tr1::shared_ptr<CFileData>` | `0x80066720`, `0x800674B0` |
| `BWorldCamera` | `0x58` | `mMotionPrograms` | `std::list<std::tr1::shared_ptr<BWorldCamera::CMotionProgram> >` | constructor, destructor `0x800665B0`, `0x80067550` |
| `BWorldCamera` | `0x64` | `mMotionFrame` | `f32` | `0x80067550`, `0x800677C0` |
| `CStageCamera` | `0x68` | `mReserved` | `u32` | constructor only (low) |
| `CStageCamera` | `0x6C` | `mEngine` | `std::tr1::shared_ptr<CPpuEngine>` | `LoadScript`, `Calc`, `OnLeaveWorld`, `GetPpuEngine`, destructor |
## Game NPpu VM third block and library (#283)

[F-283-2](findings/283-nppu-library.md#f-283-2-identities-of-the-24-functions) names, in `.text`, with the mangled names the source produces: `NPpu::CEngine::ToString` `0x801738D0`, `ToWString` `0x801739E0`, `ToStdString` `0x80173AF0`, `ToStdWString` `0x80173E70`, `GetThrown` `0x801741F0`, `AllocString` `0x80174210`, `AllocWString` `0x801743B0`; `NPpu::ToUnicode` `0x80174B30` and `NPpu::ToSjis` `0x80174C50`; `Format(CEngine*, int, const wchar_t*)` `0x80175090` and `Format(CEngine*, int, const char*)` `0x801753B0`; the natives `ScriptBinding_GetThrownValue` `0x80174D70` through `ScriptBinding_SetInitialDebugLevel` `0x80175010` and `ScriptBinding_Printf` `0x801756C0` now carry their C++ mangling (`__FPQ24NPpu7CEngine`). Outside the block, for the link: MSL `basic_string<char>::reserve` `0x8006D8B0`, `basic_string<wchar_t>::reserve` `0x80082020`, and the `replace(size_type, size_type, const T*, const T*, forward_iterator_tag)` instances `0x80067F70` (`char`) and `0x80082260` (`wchar_t`) (template names medium). Statics in `.sbss`: `CEngine::sGCMark` `0x804B90E0` and `sGCAllocCount` `0x804B90E4`. All names are descriptive.

| Type | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `NPpu::SGCString<T>` | `0x0` | `mString` | `std::basic_string<T>` | `AllocString`, collector |
| `NPpu::SGCString<T>` | `0xC` | `mMark` | `int` | stamped with `sGCMark`; collector compares it |
| `NPpu::SGCStringRef<T>` | `0x0`, `0x4` | `mKey`, `mString` | `const T*`, `SGCString<T>*` | set element, ordered by `mKey` (`find_leaf` `0x80174AD0`) |
| `std::basic_string<T>` | `0x0` | long flag (bit 31), capacity (bits 0–30) or short size (byte 0, 7 bits) | bitfields | `reserve` `0x8006D8B0` |
| `std::basic_string<T>` | `0x4`, `0x8` | long size, long data; short characters from 1 (`char`) or 2 (`wchar_t`) | | `c_str` inlines |

## Game script camera bindings (#231, #232)

[F-231-4](findings/231-script-camera-unit.md#f-231-4-the-source-of-231) and [F-232-1](findings/232-script-camera-bindings.md#f-232-1-the-functions) name, in `.text`, the file-local `PopStageCamera(CPpuEngine*)` `0x80131780` (descriptive, medium), the pointer `DynamicCast<CStageCamera>` `0x801317B0` and `IsKindOf<CStageCamera>` `0x801317F0`, `BWorldObj::GetType` `0x80131840`, and the bindings `NewStageCamera` `0x80131850`, `NewWorldCamera` `0x80131D90`, `GetCurrentCamera` `0x80132210`, `method_camSetLookAt` `0x80132270`, `method_camSetRotate` `0x80132460` and `method_camSetAim` `0x80132500`. The binding names are original, and each binding is a global function on `NPpu::CEngine*`. Inline members: `CPpuEngine::GetOwner` `0x80131990`, `CStageCamera::LoadScript(const shared_ptr<NPpu::CEnvironment>&)` `0x801319A0`, and `NPpu::CEngine::FetchArgCount` `0x80132370` and `PushNull` `0x80132400` (descriptive, medium). The `nw4r::g3d::Camera::PostureInfo` constructor is at `0x80132410`. TR1 `shared_ptr`, `weak_ptr`, `move_ptr` and deleter instances for `CStageCamera` (`0x80131A10`–`0x80131D80`) and `BWorldCamera` (`0x80131EB0`–`0x801321C0`) carry their mangled names. Outside the block: `BWorldObj::SetOwner` `0x8006A4A0` (descriptive, low), `BWorldObj::GetId` `0x800C98B0`, `CPpuEngine::GetWorld` `0x800FFA20` and `PopWorldObj` `0x80100DB0`, and `BWorld::SetCurrentCamera` `0x8005E3D0`, whose mangled name now takes the pointer by value (F-232-2).
## Game CMoneyLifeDisplay and CMsgBalloon start (#141)

[F-141-3](findings/141-cmoneylifedisplay-cmsgballoon.md#f-141-3-the-cmsgballooncpp-functions-of-141) names, in `.text`: `GetMapRecord(MapIndex)` `0x800C59D0`, `IsStageCleared(MapIndex)` `0x800C5A60`, `GetStageClearCount(MapIndex)` `0x800C5B10`, `CMapRecord::IsFlagSet` `0x800C5AE0`, `CMapRecord::GetClearCount` `0x800C5BC0`, `Clamp<int>` `0x800C5BD0`, `std::max<int>` `0x800C5C10`, `std::min<int>` `0x80095280`, `IMsgBalloon::Create(bool)` `0x800C5C30`, `CMsgBalloon::Init(bool)` `0x800C5CC0`, `CMsgBalloon::CMsgBalloon()` `0x800C66F0` and `GetSaveData(int)` `0x80167E10` (free-function, `CMapRecord`, `Clamp` and `GetSaveData` names descriptive, low; the `std` templates high). Callees renamed for the `Show` and factory links: `CLayout::SetVisible(bool)` `0x801D04A0` (sets the root pane's visible bit, medium), `CLayout::CreateMotion(const char*)` `0x8006E240` (medium), `BUILayout::GetLayout()` `0x8008B7D0` (returns `this + 0x4C`, high), `PlaySound(u32, SoundHandle*, SoundStartable*)` `0x801E7F30` (descriptive, medium) and `std::type_info::operator==` `0x800871B0`.

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `MapIndex` | `0x0`–`0x3` | `mMode`, `mArea`, `mStage`, `mMap` | `int : 8` each | word load with `extlwi`/`srawi`; `rlwimi` store; names low |
| `SaveData` | `0x18` | `mMapRecords` | `CMapRecord[3][5][8][10]` | strides 0x640, 0x140, 0x28, 4 in `GetMapRecord`; slot size 0x1360 |
| `StageDesc` | `0xC` | `mMapDescs` | `std::vector<std::tr1::shared_ptr<MapDesc> >` | `IsStageCleared` reads `size()` (F-91 slot 14) |
| `CMsgBalloon` | `0xC` | `mIsShown` | `bool` | `Show` |
| `CMsgBalloon` | `0x10`, `0x14` | `mShowWait`, `mHideWait` | `int` | `Show` sets 0x10 to 5; `Calc` counts both down |
| `CMsgBalloon` | `0x18` | `mUIManager` | `std::tr1::shared_ptr<CUIManager>` | `Init`, destructor `0x800CBD00` |
| `CMsgBalloon` | `0x20`, `0x28`, `0x30` | `mLayout`, `mMsgLayout`, `mWarpLayout` | `std::tr1::shared_ptr<BUILayout>` | `Init` loads `msg_balloon`, `msg_balloon_msg`, `msg_balloon_warp` |
| `CMsgBalloon` | `0x38` | `mModelLayouts` | `std::vector<std::tr1::shared_ptr<BUILayout> >` | 18 `MODEL%02d` layouts in `Init` |
| `CMsgBalloon` | `0x44` | `mModelTextures` | `std::vector<std::tr1::shared_ptr<CPiiTex> >` | element built through `0x800F6580`; `Calc`, `CalcMtx` and `CalcView` call the `CPiiTex` members of the same names ([F-145-2](findings/145-cmsgballoon-calc.md#f-145-2-the-model-textures-are-cpiitex)); type medium-high |
| `CMsgBalloon` | `0x50` | `mEntries` | `std::list<CMsgBalloon::Entry>` | list constructor `0x800C6D10`; entry constructor `0x800C9B30` |
| `CMsgBalloon` | `0x5C`, `0x60` | `mCurrentEntry`, `mCurrentIndex` | `Entry*`, `int` | `Show`, `IsShown` (slot 11) |
| `CMsgBalloon` | `0x64` | `mShownEntries` | `Entry*[4]` | `Show`, `IsShown` |
| `CMsgBalloon` | `0x74` | `mOffset` | `nw4r::math::VEC3` | constructor `0x800C66F0`; name low |
| `CMsgBalloon` | `0x80` | `mMotion` | `std::tr1::shared_ptr<CLayout::CMotionProgram>` | `Show`; deleter type from `0x8006E4B0` |

## F-769-1: particle-manager field records and post-field call

`ParticleManager::Calc` (`0x80232930`, `0x1558` bytes) dispatches on animation-header byte 2, accepts baked/keyed magic, and treats ordinary U8/F32 target bytes as offsets into `ParticleParameter`. Curve type 7 carries a 28-byte field record; particle curve type 2 carries a post-field transform and a 112-byte post-field resource record. Proven offsets, kinds, spaces, and provenance are in [F-769-1](findings/769-particlemanager-fields.md).

The new `detail::ParticleField`, `detail::ParticlePostFieldTransform`, and `detail::ParticlePostField` names are descriptive (low naming confidence); their accessed layouts are instruction-backed. `PostFieldInfo` is an original type name present in the adjacent cross-check manglings and remains opaque here. External `0x80230DA0` is descriptively named `CalcPostFieldParticle`, with the observed particle/transform/info/resource/matrix/previous-position/by-value-offset/velocity/killed interface. Existing `AnimCurveExecuteTexture` and `AnimCurveExecuteChild` interface names now label `0x8022C430` and `0x8022CB80`. No ownership, split, or size outside #769 changes. The unit remains `NonMatching`; these names and reconstructed paths do not establish source coverage.
## Game NPpu natives and lucky number helper (#284)

[F-284-2](findings/284-nppu-natives-luckyhelper.md#f-284-2-the-sixteen-natives-of-nppulibrarycpp) gives the natives `ScriptBinding_Sprintf` `0x80175810` through `ScriptBinding_Dump` `0x80176750` their C++ mangling (`__FPQ24NPpu7CEngine`) and names, for the link, `NPpu::CEngine::CollectGarbage(int, int)` `0x80174550` (descriptive), the MSL `basic_string` `operator+` instances `0x80072830` (`char`) and `0x8008FD30` (`wchar_t`), the out-of-line `basic_string` destructors `0x80067F00` (`char`) and `0x8006BAD0` (`wchar_t`), and MSL `strtof` `0x803472F8` and `wcstof` `0x80348BAC` (medium). [F-284-1](findings/284-nppu-natives-luckyhelper.md#f-284-1-the-last-two-functions-form-their-own-unit) names `ScrambleLuckyNumber(u16*)` `0x801768E0`, `CalcLuckyNumberCheck(const u16*, const u8*)` `0x80176C60` and the `.rodata` table `gLuckyNumberTable` `0x80366760` (`const u8[100]`, resized from 0x68 to 0x64); these names are descriptive. `CValue::SetStringPtr`/`SetWStringPtr` and `CEngine::PushString`/`PushWString` are inline helpers with no out-of-line copies.

| Type | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| lucky number digits (`u16[5]` in `ScriptBinding_GetLuckyNumber`) | `0x0`–`0x6` | four base-10000 digits, most significant first | `u16` | `0x80139770` stores them from successive divisions by 10000; `0x801768E0` |
| lucky number digits | `0x8` | check value | `u16` | `0x80139770` stores the result of `0x80176C60` |
## Game CFiber (#307)

[F-307-2](findings/307-cfiber.md#f-307-2-cfiber-layout) and [F-307-3](findings/307-cfiber.md#f-307-3-functions) name, in `.text`: `CFiber::Initialize()` `0x8019E110`, `~CFiber()` `0x8019E180`, `Start(const std::tr1::function<void()>&, u32)` `0x8019E320`, `Resume()` `0x8019E460`, `ThreadMain(void*)` `0x8019E9C0`, and the free functions `TerminateFiber(int)` `0x8019E5A0`, `YieldFiber()` `0x8019E770` (#314) and `YieldFiberStarted()` `0x8019E890`; the vtable `__vt__6CFiber` `0x803BE468`, type info `0x804B8100` and `CFiber::sCurrent` `0x804B9120`. Outside the unit, for the link: `std::tr1::function<void()>::~function` `0x80070400`, the `std::tr1::bad_function_call` destructor `0x800E1A50`, `what` `0x800E1810`, vtable `0x803A89F0` and type info `0x804B7030`, the `shared_ptr_deleter<unsigned char, void (*)(void *) throw()>` destructor `0x800BE770`, `get_deleter` `0x800BE3A0`, `dispose` `0x800BE3F0`, vtable `0x803A42AC` and type info `0x804B6940`, and the `shared_ptr_holder<unsigned char, void (*)(void *) throw()>` destructor `0x800BE5D0` (all high: the compiler emits these names for the same calls and data). `CFiber` (0x370 bytes) and `CFiber::Terminated` are RTTI and throw-type names; method and member names other than `YieldFiber` are descriptive (medium).

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `CFiber` | `0x0` | `mFlags` | `u32` | Started, finished, terminating and running bits |
| `CFiber` | `0x4` | `mStack` | `std::tr1::shared_ptr<u8>` | `Start`, `Resume` |
| `CFiber` | `0xC` | `mOnSuspend` | `std::tr1::function<void()>` | `YieldFiber` |
| `CFiber` | `0x18` | `mOnResume` | `std::tr1::function<void()>` | `YieldFiber` |
| `CFiber` | `0x24` | `mMutex` | `OSMutex` | `Initialize`, `Resume`, `ThreadMain` |
| `CFiber` | `0x3C` | `mResumeCond` | `OSCond` | `Resume` signals, `YieldFiber` waits |
| `CFiber` | `0x44` | `mSuspendCond` | `OSCond` | `YieldFiber` and `ThreadMain` signal, `Resume` waits |
| `CFiber` | `0x50` | `mThread` | `OSThread` | `Start`, `Resume` |
| `CFiber` | `0x368` | vptr | | Vtable `0x803BE468` |

## Game loop, random numbers and CFileData split (#307)

[F-307-5](findings/307-cfiber.md#f-307-5-gameloopcpp-and-randomcpp) names, in `.text`: `RunGameLoop(IGameLoopCallback*)` `0x8019DC50`, `InitRandom()` `0x8019DEB0`, `ShutdownRandom()` `0x8019DEE0`, `RandInt(int, int)` `0x8019DEF0`, `Rand32()` `0x8019DFC0`, `RandReal(f32, f32)` `0x8019E010` and `RandBool(f32)` `0x8019E0A0`; and MSL `rand` `0x80345B08` and `srand` `0x80345B28`. `IGameLoopCallback` is an RTTI name (high); the nine slot names and the function names are descriptive (medium to low). `.sbss`: `gGameFrame` `0x804B9108` (read by `ScriptBinding_GetGameFrame`), `gFrameStep` `0x804B910C`, `gFrameTimingReset` `0x804B9110`, file-local `sRenderCount` `0x804B9114` and `sRenderHistoryIndex` `0x804B9118`; `.bss`: file-local `sRenderHistory` `0x803EA7F8`, 60 `bool`.

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `IGameLoopCallback` | `0x8` | `IsRunning` | virtual, returns `bool` | `RunGameLoop` loop condition |
| `IGameLoopCallback` | `0xC` | `ResetTiming` | virtual | Called after a timing reset |
| `IGameLoopCallback` | `0x10` | `Update` | virtual | Pad, controllers, sound |
| `IGameLoopCallback` | `0x14` | `ShouldRender` | virtual, returns `bool` | Gates `Prepare` and the frame counter |
| `IGameLoopCallback` | `0x18` | `Prepare` | virtual | Called only when rendering |
| `IGameLoopCallback` | `0x1C` | `Calc` | virtual | After the history update |
| `IGameLoopCallback` | `0x20` | `Draw` | virtual | |
| `IGameLoopCallback` | `0x24` | `Render` | virtual | GX state reset and frame buffer copy in `CMain` |
| `IGameLoopCallback` | `0x28` | `Present` | virtual | `DisplayFrameBuffer` in `CMain` |

## Game script camera projection and safe frame (#233, #234)

[F-233-1](findings/233-script-camera-projection.md#f-233-1-the-functions), [F-233-2](findings/233-script-camera-projection.md#f-233-2-shared-inline-helpers) and [F-234-1](findings/234-script-camera-safe-frame.md#f-234-1-the-functions) name the bindings `method_camSetFovy` to `method_camGetFar` (`0x801325D0`–`0x80132E80`), `method_camGetXYZOf` `0x80133360`, `IsTV16x9` `0x801335C0` and `method_camIsObjInSafeFrame`, `4x3` and `16x9` (`0x80133EA0`–`0x80133EF0`), all original. File-local, descriptive (medium): `GetSafeFrameGroundPos` `0x80132F00`, `GetSafeFrame` `0x80133610`, `MakePlane` `0x801337B0`, `IsInFrontOfPlane` `0x80133A70`, `IsObjInSafeFrame` `0x80133AE0` and the object `sSafeFrameCache` `0x803B65C0` (`SSafeFrameCache`, 0x58 bytes: `u8` aspect at 0, projection flags 0x4, top 0x8, bottom 0xC, fovy 0x10, near 0x14, four `PLANE`s from 0x18). `SSafeFrame` is six floats: top, bottom, left, right, near, far. NW4R inlines: `RAY3(const VEC3&, const VEC3&, bool)` `0x801331A0`, `RAY3::Normalize` `0x80133210`, `VEC3Normalize` `0x80133220`, `PLANE(f32, f32, f32, f32, bool)` `0x80133280`, `PLANE::Normalize` `0x801332E0`, `VEC3LenSq` `0x80133340`, `VEC3Cross` `0x80133900`, `VEC3::Dot` `0x80133A40` (descriptive), `VEC3Dot` `0x80133A50`, `PLANE()` `0x80133E40` and its assignment `0x80133E70`, and outside the block `ref` of `ResCommon<CameraData>` `0x80098660`, `MTX34()` `0x80074A30`, `MTX34Inv` `0x800986A0`, `TanDeg` `0x80098700`, `TanFIdx` `0x80098710`, `VEC3(f32, f32, f32)` `0x80096990`, the `VEC3` copy constructor `0x8009A050` and assignments `0x80096820` and `0x80096850`, and `VEC3Scale` `0x8009CAC0`. Game inlines, descriptive (medium): `Transform` `0x80096870`, `Inverse` `0x80098670`, `Cross` `0x801338A0`, `SafeNormalize` `0x80133980` and `BWorldObj::GetPositionRef` `0x80098850`. `GetSafeDisplaySize` `0x801B6BE0` and `GetPixelAspectRatio` `0x801B6590` follow F-318-1.

## F-539-3: wide floating-point scanner and rounded wrapper

`0x80347904` (`0x12A8` bytes) is named `__wcstold` by inference from MSL scanner interfaces and its wide-reader ABI. The name has medium confidence; the implementation and callback/64-bit value interface are instruction-backed. `0x80348BAC` (`0xB8` bytes) retains the canonical `wcstof` interface introduced with independent script-caller evidence in F-284-2; naming confidence is medium. It returns a single-rounded value after the target's double-range checks. Both functions are source-linked C with exact bodies and resolved destinations; [F-539-3](findings/539-wide-parser.md) supplies all forty accepted #539 rows.

The five-byte `NAN(` literal at `0x804BB6B8` includes the byte at `0x804BB6BC`; that interior reference now uses the same object plus addend four. The nine-byte `INFINITY` literal at `0x803989A0` is followed by seven zero linker-alignment bytes. `0x804B8FB8` is named `__double_huge` from the MSL reference and its positive-infinity bits; it and `__lconv` at `0x803E6078` remain shared external data outside the scanner split. No original hashes or compiler flags change.

## Issue #377: effect post-field interfaces

The inferred `GetTypePositionSphere` at `8022DFB0` has the same `const math::VEC3&`, `u8` interface as the five existing named classifiers; behavior and neighboring manglings give medium spelling confidence. The first nine post-field routines occupy 5388 instruction bytes and use the canonical C++ math types. `PostFieldInfo` is 72 bytes, aligned four, with scale/rotation/translation at 00/0C/18, speed limit24, speed mode28, shape29, option2A, collision mode2B, flags2C, start age2E, collision scale30, and existing ten-byte inheritance setting3C. Field names are descriptive; nine candidate functions directly prove option/flag accesses. Full offset evidence, native sections, source provenance, candidate relocation checks, and unaccepted remainder are in [F-377-1](findings/377-curves-postfields.md). No progress credit is assigned while the whole unit remains NonMatching.

## Issue #767: post-field collision dispatch ABI

The existing `crossCheck` mangling is retained. Native arguments after the two matrix references, PostFieldInfo pointer, resource pointer and particle pointer are previous/current position references, velocity by value, bool killed output, and local-hit/world-hit/result-position/result-velocity pointers. The canonical declaration parameter labels from #377 are provisional; its reference/value types are correct. The source definition now uses the proven argument meanings. Creation queues use mCalcRemain at particle E2. All four #767 bodies/6284 instruction bytes are exact candidates, awaiting #768 for source-linked whole-unit acceptance; see [F-767-1](findings/767-postfield-crossings.md).

## Issue #768: complete post-field particle step

The descriptive CalcPostFieldParticle ABI from #769 now has a complete exact source-linked definition at 80230DA0/2208 bytes. The existing transform type is36 bytes (scale/rotation/translation vectors), PostFieldInfo remains72 bytes, and all14 post-field functions/13880 C++ instruction bytes and56 constant bytes are accepted as one unit; see [F-768-1](findings/768-postfield-particle.md). Particle tickDC,calcRemainE2,momentumC4,collision status9D,velocityA0 andpositionAC accesses all resolve through canonical members. This acceptance covers the nine #377 and four #767 routines once, while #377 animation remains unaccepted.
## Game CMsgRes and message text (#346)

[F-346-2](findings/346-cmsgres.md#f-346-2-cmsgres) and [F-346-3](findings/346-cmsgres.md#f-346-3-message) name, in `.text`: `CMsgRes::CMsgRes(void*, bool)` `0x801EE470`, `~CMsgRes()` `0x801EE4F0`, `ConvertEscape()` `0x801EE530`, `ConvertEscapeW()` `0x801EE760`, `Setup()` `0x801EE9A0`, `GetGroupID() const` `0x801EEA80`, `GetMessage(int) const` `0x801EEAA0`, `MakeCollationKeys` `0x801EEAC0`, `CompareCollationKeys` `0x801EEE90`, `CompareCollation` `0x801EF360`, `SetTextLocale` `0x801EF470`, `ConvertCase` `0x801EF480`, `towupper` `0x801EF670` and `toupper` `0x801EF680` (original: MSL code calls them by these names), `InitMsgResList` `0x801EF6F0`, `SetMsgRes` `0x801EF970`, `GetMessageText` `0x801EFA20`, `SetDateMessageIDs` `0x801EFAD0`, `SetTimeMessageIDs` `0x801F0530`, the `vector<shared_ptr<CMsgRes>>` deleter, `shared_ptr` and `move_ptr` instances `0x801F0DE0`–`0x801F1150` and `__sinit_\Message_cpp` `0x801F1190` (compiler names, high). Data: the deleter vtable `0x803C3BAC` and RTTI `0x804B8748`, `default_delete` RTTI `0x804B8750`; outside the units `__vt__Q23std12length_error` `0x80399778`, its destructor `0x8005A830`, `__vt__Q23std11logic_error` `0x803997B0` and the `shared_ptr_deleter<char, default_delete<char[]>>` vtable `0x803998B4` (high). `CMsgRes` is an RTTI name; other function, member and file names are descriptive (medium).

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `CMsgRes` | `0x0` | `mpHeader` | `FileHeader*` | `Setup`, encoding byte at 0x10 |
| `CMsgRes` | `0x4` | `mpInfo` | `InfoBlock*` | `INF1`; `GetGroupID`, `GetMessage` |
| `CMsgRes` | `0x8` | `mpResource` | `void*` | Constructor argument |
| `CMsgRes` | `0xC` | `mpData` | `DataBlock*` | `DAT1`; escape conversion, `GetMessage` |
| `CMsgRes` | `0x10` | `mpString` | `BlockHeader*` | `STR1` |
| `CollationKey` | bits 31–29 | `mCategory` | `u32 : 3` | 1 symbol, 2 private use, 3 digit, 4 kana, 5 letter |
| `CollationKey` | bits 28–21 | `mPrimary` | `u32 : 8` | Level 1 |
| `CollationKey` | bits 20–13 | `mSecondary` | `u32 : 8` | Level 1 |
| `CollationKey` | bits 12–9 | `mAccent` | `u32 : 4` | Level 2 |
| `CollationKey` | bits 8–5 | `mCase` | `u32 : 4` | Level 3; 8 for the prolonged sound mark |
| `CollationKey` | bits 4–2 | `mWidth` | `u32 : 3` | Level 4 |
## Game sound (#343)

[F-343-1](findings/343-sound.md#f-343-1-unit-boundaries) to [F-343-5](findings/343-sound.md#f-343-5-names-outside-the-unit) split `game/sound.cpp` (`.text` `0x801E6D50`–`0x801E8738`) and name, in `.text`: the file-local `OnReset()` `0x801E6D50`, `InitSound(u32, u32, u32)` `0x801E6D80`, `ShutdownSound()` `0x801E7830`, `UpdateSound()` `0x801E78A0`, `LoadSoundArchive(const char*)` `0x801E78B0`, `SetBGMVolume(f32, f32)` `0x801E7C30`, `StopBGM(f32)` `0x801E7C90`, `GetBGMID()` `0x801E7D00`, `ContinueBGM(u32, f32)` `0x801E7D40`, `PlayBGM(u32, f32, bool)` `0x801E7D50`, `StopSound(SoundHandle*, SoundActor*, f32)` `0x801E7F70`, `GetSoundArchivePlayer` `0x801E7FF0`, `GetSound3DManager` `0x801E8000`, `GetSound3DListener` `0x801E8010`, `GetFxReverb` `0x801E8020`, `GetFxDelay` `0x801E8030`, `GetFxChorus` `0x801E8040`, the `SoundWork` deleter, `shared_ptr` and `move_ptr` instances `0x801E8050`–`0x801E86D0` and `__sinit_\sound_cpp` `0x801E8710`; the weak `FxBase` destructor `0x801E7600`, `NonCopyable` destructor `0x801E7640`, `SoundHandle()` `0x801E7780` and `Sound3DListener` destructor `0x801E77F0`. Data: deleter vtable `0x803C2BE0`, type info `0x804B8608` and `0x804B8610`, `sBgmIndex` `0x804B91E0`, `sSoundWork` `0x804B91E8`, `sBgmDisabled` `0x804B91F0`. Outside the unit, for the link: `SoundHandle::~SoundHandle` `0x800BCAE0`, the `function_imp<void (*)(), void (*)(), false, false>` members `0x8012FAB0`, `0x8012FB00`, `0x8012FB40`, `0x80131640`, vtable `0x803B5748` and type info `0x804B78F8`, the `function_base<void (*)()>` destructor `0x80131600`, vtable `0x803B5800` and type info `0x804B7900`, the `shared_ptr_deleter_common` type info `0x804B5AB8` (high), and the descriptive `LoadFile(const char*, u32, bool)` `0x8005ED70` and `RegisterResetCallback(const function<void()>&, int)` `0x801B5C30` (low). `SoundWork` and `sound_cpp` are RTTI names; all other names are descriptive (medium unless noted).

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `SoundWork` | `0x0` | `mSystemMemory` | `std::tr1::shared_ptr<u8>` | `InitSound` |
| `SoundWork` | `0x8` | `mFxReverb` | `nw4r::snd::FxReverbStd` | `GetFxReverb` |
| `SoundWork` | `0x130` | `mFxChorus` | `nw4r::snd::FxChorus` | `GetFxChorus` |
| `SoundWork` | `0x1FC` | `mFxDelay` | `nw4r::snd::FxDelay` | `GetFxDelay` |
| `SoundWork` | `0x280` | `mFxReverbMemory`, `mFxChorusMemory`, `mFxDelayMemory` | `std::tr1::shared_ptr<u8>` (three) | `InitSound` |
| `SoundWork` | `0x298` | `mArchiveFile` | `std::tr1::shared_ptr<CFileData>` | `LoadSoundArchive` |
| `SoundWork` | `0x2A0` | `mArchive` | `nw4r::snd::MemorySoundArchive` | `LoadSoundArchive` |
| `SoundWork` | `0x3F0` | `mPlayer` | `nw4r::snd::SoundArchivePlayer` | `GetSoundArchivePlayer` |
| `SoundWork` | `0x4DC` | `mPlayerMemory`, `mStrmBuffer` | `std::tr1::shared_ptr<u8>` (two) | `LoadSoundArchive` |
| `SoundWork` | `0x4EC` | `m3DManager` | `nw4r::snd::Sound3DManager` | `GetSound3DManager` |
| `SoundWork` | `0x518` | `m3DListener` | `nw4r::snd::Sound3DListener` | `GetSound3DListener` |
| `SoundWork` | `0x584` | `m3DManagerMemory` | `std::tr1::shared_ptr<u8>` | `LoadSoundArchive` |
| `SoundWork` | `0x58C` | `mSeHandle` | `nw4r::snd::SoundHandle` | `PlaySound`, `StopSound` |
| `SoundWork` | `0x590` | `mBgmHandles` | `nw4r::snd::SoundHandle[2]` | BGM functions |
| `SoundWork` | `0x598` | `mSpareHandle` | `nw4r::snd::SoundHandle` | Constructor and destructor only (low) |
| `CFileData` | `0x10` | `mSize` | `u32` | `ARCGetLength` store `0x8019F0C0` |
| `CFileData` | `0x1C` | `mData` | `void*` | `ARCGetStartAddrInMem` store `0x8019F0B4` |
## Game script camera adjustment (#235)

[F-235-1](findings/235-script-camera-adjust.md#f-235-1-methodcamadjustpos-and-its-helper-class) and [F-235-2](findings/235-script-camera-adjust.md#f-235-2-pointer-vectors-share-one-void-implementation) name, in `.text`: the file-local `ProjectToHeight` `0x80133F00`, `method_camAdjustPos` `0x80134000` (original), `CCameraAdjuster::Init` `0x80134150`, `Adjust` `0x80134370`, `AdjustPii` `0x80134890`, `GetPosition`, `GetDepth`, `IsClipped` `0x80134CA0`–`0x80134CC0` and the constructor `0x80134D60` (descriptive, medium); `BWorld::GetGround` `0x80134310`, `BWorldObj::GetWorld` `0x80134360`, `CPlayerPii::GetPositionHistory` `0x80134B00`; the `vector<CPlayerPii*>`, `__vec_pointer`, `__vec_constructor<void*>`, `__vec_deleter<void*>`, `deque<VEC3>` and ring-buffer instances `0x80134710`–`0x80134C80`, `0x80134DB0`–`0x80134EB0`, `0x80135150`, `0x801351A0` and `0x80135230`; the `shared_ptr_deleter<CStageCamera>` members `0x80134F40`, `0x80134F90`, `0x801351D0`, `default_delete<CStageCamera>::operator()` `0x80135100`, its `second` accessors `0x80135130` and `0x80135140`, and `__sinit_\ScriptCamera_cpp` `0x801352A0`. Outside the unit: `CPlayer::IsJoined` `0x80092980` (low), `CPlayer::GetPii` `0x800F8320` (medium), `CDividedGround::IsOutOfBounds` `0x80075FB0` (low), `VEC3::operator*(f32)` `0x8009CA60`, the shared pointer-vector and deque helpers listed in F-235-2, and in `.sdata` `__RTTI__Q43std3tr16detail25shared_ptr_deleter_common` `0x804B5AB8` (high, referenced by every `shared_ptr_deleter` base list). `lbl_804BA1E0` is resized to 4 bytes.

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `CCameraAdjuster` | `0x00` | `mViewMtx` | `nw4r::math::MTX34` | `Init`, `AdjustPii` |
| `CCameraAdjuster` | `0x30` | `mInvViewMtx` | `nw4r::math::MTX34` | `Init`, `Adjust`, `AdjustPii` |
| `CCameraAdjuster` | `0x60` | `mTopSlope`, `mBottomSlope`, `mLeftSlope`, `mRightSlope` | `f32` | `Init`, `AdjustPii` |
| `CCameraAdjuster` | `0x70` | `mTopLimit`, `mBottomLimit`, `mLeftLimit`, `mRightLimit` | `f32` | slopes times the margin; `AdjustPii` |
| `CCameraAdjuster` | `0x80` | `mMaxDepth` | `f32` | `Init`, `AdjustPii` |
| `CCameraAdjuster` | `0x84` | `mDepth` | `f32` | `AdjustPii`, `GetDepth` |
| `CCameraAdjuster` | `0x88` | `mCount` | `int` | `Adjust` |
| `CCameraAdjuster` | `0x8C` | `mSum` | `nw4r::math::VEC3` | `Adjust` |
| `CCameraAdjuster` | `0x98` | `mClipped` | `bool` | `AdjustPii`, `IsClipped` |
| `CCameraAdjuster` | `0x9C` | `mPosition` | `nw4r::math::VEC3` | `Adjust`, `GetPosition` |
| `CCameraAdjuster` | `0xA8` | `mGround` | `CDividedGround*` | `Init`, `AdjustPii` |
| `CPlayerPii` | `0x14C` | `mPlayerNo` | `int` | constructor argument (low) |
| `CPlayerPii` | `0x150` | `mPositionHistory` | `std::deque<nw4r::math::VEC3>` | `GetPositionHistory`, `Adjust` (medium) |

[F-347-2](findings/347-locale.md#f-347-2-locale) names, in `.text`: `GetRegionName(Region)` `0x801F11C0`, `GetConsoleRegion()` `0x801F11E0`, `GetGameRegion()` `0x801F12A0`, `GetConsoleLang()` `0x801F1340`, `GetGameLang()` `0x801F1380` (after their script bindings, medium-high), `GetGameLocale()` `0x801F1520` and `GetConsoleLocale()` `0x801F16F0` (descriptive, medium). Data, all local: `sRegionNames` `.data` `0x803C3F78` (14 pointers), `sLocaleNames` `0x803C3FB0` (11 pointers), `sLocaleEnUS` `.sdata` `0x804B87E4`, `sLocaleEnAU` `0x804B87EC`, `sLocaleFrCA` `0x804B87F4` (6-byte `char` arrays), `sConsoleRegion` `0x804B87FC` and `sGameRegion` `0x804B8800` (`Region`, initially 13), `sConsoleLang` `0x804B8804` and `sGameLang` `0x804B8808` (`int`, initially -1), `sGameLocale` `.sbss` `0x804B9200` and `sConsoleLocale` `0x804B9204`. `Region` values: `JPN` 0 to `ALL` 12, unknown 13. File and data names are descriptive (medium).

## Issue #377: animation records and child creation helper

The retained8022CA10 routine is descriptively named detail::CreateCurveChild, confirmed by its exact356-byte body and canonical child-creation calls. Both AnimCurveExecuteBakedU8 at8022D910 and BakedF32 at8022DC60 take data/destination pointers,tick andlife (four args). Those suffix spellings and channel table names are inferred, low confidence. The256-byte tables at803C5378 and803C5478 contain byte population count and highest set bit. Value key prefix12,texture key4/record16/random record8,child setting12/key24,and4-byte table prefixes are declared in the private ef_animcurveinternal.h; exact order, field widths, provenance and unaccepted differences are in [F-377-2](findings/377-curves-postfields.md). All8 animation routines have candidate source, but the unit remains NonMatching; no source credit is claimed.
## Game CPauseMenu (#167)

[F-167-1](findings/167-cpausemenu-split.md) splits `game/CPauseMenu.cpp` (file name original, from the allocation-failure string and the `@unnamed@CPauseMenu_cpp@` RTTI). [F-167-2](findings/167-cpausemenu-split.md#f-167-2-classes) records `@unnamed@CPauseMenu_cpp@::CPauseMenu` (0x5B8 bytes, vtable `0x803A7B24`, base `IPauseMenu` with vtable `0x803A7C18` and type info `0x804B6F60`): `shared_ptr` members at 0x4, 0xC, 0x14, 0x1C, 0x24 and 0x3A0, `bool` 0x2C (returned by slot 5), `CFiber` 0x30, `CaptureTexture` 0x3A8, `bool` 0x3D5 (capture request) and two 0xE8-byte layout objects at 0x3D8 and 0x4C0 (high, from the destructor `0x800DC770` and the overrides `Calc` `0x800DC2A0`, `CalcMtx` `0x800DC230`, `CalcView` `0x800DC1C0`, `Draw` `0x800DC120`). `@unnamed@CPauseMenu_cpp@::CPauseMenuInput` (vtable `0x803A7B8C`) implements the seven `IUIInput` slots `0x800DC4B0` to `0x800DC350` by forwarding to the object returned by `0x80124A50` and mapping button bit 0x100 to 0x10. Member names are not recovered; all functions keep their address labels.

[F-167-4 to F-167-6](findings/167-cpausemenu-reconstruction.md) reconstruct the unit as `src/game/CPauseMenu.cpp`. `IPauseMenu` (vtable `0x803A7C18`): slot 5 `IsOpen`, slot 6 `TakeScreenshot`, `Create(const MapDesc*, bool, bool)` `0x800D83B0`. `@unnamed@CPauseMenu_cpp@::CPauseMenu` members (all high for offsets, medium for names): `mBackManager` 0x4, `mFrontManager` 0xC, `mBackgroundLayout` 0x14, `mBlackLayout` 0x1C, `mRemoteMemoryLayout` 0x24, `mIsOpen` 0x2C, `mFiber` 0x30, `mDialog` 0x3A0, `mCapture` 0x3A8, `mScreenshotBusy` 0x3D4, `mCaptureRequested` 0x3D5, `mHideBackground` 0x3D6, `mScreenshotLayout` 0x3D8, `mCopyrightLayout` 0x4C0, `mScreenshotScale` 0x5A8, `mPictureSize` 0x5B0. Methods: `Run` `0x800D9110`, `LeaveStage` `0x800DA2D0`, `TakeScreenshot` `0x800DA610`, `RunRemoteMemoryMenu` `0x800DB160` (names descriptive, medium-low). `CUIMenuCallbacks` (constructor `0x8011D090`, deleting destructor `0x800D9D70`) derives from `CUICallbackLists` (three `BUI::CCallbackListOwner`, destructor `0x800D9FB0`); `UIMenuItem` is `{mText, mKind}`, `UIMenuParam` has eight words, `RunUIMenu` is `0x801DD850` (high for layout, low for names). `IGameSession` slots 30 to 41 are named `GetPlayTimeSeconds`, `GetMapChangeType`, `SetFade`, `GetFadeFrames`, `RequestMapChange`, `RequestSceneChange`, `StartMapTransitionOut`, `StartMapTransitionIn`, `UpdateMapTransition`, `IsMapChangeFinished`, `GetPendingMapChange`, `IsRegisteredID` (low to medium); slots 25 to 29 and 42 keep the placeholder spelling `UnanalyzedSlotNN` and still need analysis. `IDlgMsgWin` slots 0x48 to 0x64 are `SetOption`, `SetDefaultOption`, `OpenOptions`, `IsOptionsClosed`, `GetSelectedOption`, `OpenFootnote`, `BeginClose`, `IsClosed` (low to medium). `IBattleRoyalDisplay` slots 5 to 10 are `Show`, `Hide`, `Finish`, `SetRound`, `GetRoundIndex`, `GetRound` (medium). Outside names: `IsGameFlagSet` `0x80165BC0`, `GetPixelAspectRatio()` `0x801B6510`, `IsBattleRoyalFirstPart` `0x801251C0`, `SaveScreenshotToRemoteMemory` `0x8016CB90`, `OpenRemoteMemoryDialog` `0x8016E420`, `CaptureTexture::GetBufferOwner` `0x800DB130`, `gStageKindMessageIDs` `0x8035B9C0`, `gSharedLayoutArchives` `0x804B9098`, `GetDefaultFontMap` `0x80126280`.

## Game save data (#276)

[F-276-1](findings/276-savedata.md#f-276-1-unit-boundaries) to [F-276-3](findings/276-savedata.md#f-276-3-functions) split `game/SaveData.cpp` (`.text` `0x8015EEB0`–`0x8016CB18`) and name, in `.text`: `WriteBanner(int)` `0x8015EEB0`, `Save(int)` `0x8015FA80`, the `list<shared_ptr<FileList>>` destructor `0x8015F9A0`, `SerializeStruct<T>` and `SerializeVector<T>` instances `0x801601A0`–`0x801613F0`, `SaveData::GetSlotData` `0x801603B0` and the `SlotData` getters `0x80160CB0`–`0x80160D10` (descriptive, low to medium; template and class names from RTTI where stated). Data: `sVectorTypes` of `SaveData` `.sdata` `0x804B7AC8`, `SlotData` `.data` `0x803B73E0`, and of `SD_PiiPersonalData`, `SD_BattleRoyalData`, `SD_StatisticValue`, `SD_PiiCollectionData`, `UsedPassword` `.sbss` `0x804B90B8`–`0x804B90C8`; `sSaveData` `.sbss` `0x804B90D0`. Outside: `ShowSystemMessage` `0x801E09B0`, `gEmptyText` `0x804B5C48`, `FormatString` `0x800721A0`, `LoadFileList` `0x80127CF0`, `LoadResourceFileList` `0x80072DD0` (low) and the `shared_ptr_deleter<NANDBanner, void (*)(void*) throw()>` vtable `0x803B7878` (high).

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `SlotData` | `0x8` | `mData` | `SlotData::Data` (0x1300, 8-aligned) | `SerializeStruct<SlotData>` copy source |
| `SlotData` | `0x18` | `mData.mMapRecords` | `CMapRecord[3][5][8][10]` | F-141 |
| `SlotData` | `0x1308`–`0x1350` | `mPiiPersonalData`, `mPiiCollectionData`, `mBattleRoyalData`, `mStatisticValue1` to `3`, `mUsedPassword` | `std::vector` | Getters `0x80160CB0`–`0x80160D10` |
| `SaveData` | `0x4` | `mData.mCurrentSlot` | `u8` | `GetSaveData(-1)` |
| `SaveData` | `0x8` | `mData.mSlotDigests` | `u32[3][5]` | Digest array passed by `Save` |
| `SaveData` | `0x44` | `mSlotData` | `std::vector<SlotData>` | `GetSaveData`, getter `0x801603B0` |

### Issue #377 curve-time API provenance

`AnimCurveCalcTime(u8*,u32,u32)` and `AnimCurveCalcFrameStep(u8*,u32,u32)` are public names from the pinned CC0 Forecast and News references in [F-377-3](findings/377-curves-postfields.md). Neither has a retained address in this target's eight-function animation inventory. Their definitions before the executors produce the exact 56-byte native constant section; no new target symbol, address or code coverage is claimed. Source-only linker stripping remains an acceptance requirement.

## Game shadow-camera interface for #51

[F-51-2](findings/51-bworldlightset-unit.md) establishes the 0x100-byte `CShadowCamera` layout over the existing 0x68-byte `BWorldCamera`: texture-level count `0x68`, three `CaptureTexture` objects `0x6C-0xF0`, texture-alpha mode `0xF0`, alpha `0xF1`, automatic light selection `0xF2`, light-set index `0xF4`, light index `0xF8` and projection-plane height `0xFC`. RTTI and class names are original; member and accessor spellings are descriptive. The destructor and UpdateVisible override existing slots; SelectLight (`0x8010D7D0`) is nonvirtual and remains original code under #208. `CFileData::IsValid` reads the already declared failed byte `0x28` and data pointer `0x1C` without changing storage or virtual interfaces. No source coverage is claimed by these declarations.
## Game save data loading (#277)

[F-277-1](findings/277-savedata-load.md#f-277-1-functions) and [F-277-2](findings/277-savedata-load.md#f-277-2-class-and-library-evidence) name, in `.text`: `GetDataSize()` of `SD_PiiCollectionData` `0x801615F0`, `SlotData` `0x80161600`, `SD_StatisticValue` `0x80161610`, `UsedPassword` `0x80161620`, `SaveData` `0x80161630`; `Load(bool)` `0x80161640`; `DeserializeStruct<T>` `0x80161E90`, `0x80162670`, `0x80162E50`, `0x80163270`, `0x80163A90`, `0x80164360`, `0x80165580`; `DeserializeVector<T>` `0x80162050`, `0x80162830`, `0x80163010`, `0x80163430`, `0x80163C50`, `0x80164F60`; `ClearData()` of the five types `0x80165B00`, `0x80165B20`, `0x80165B30`, `0x80165B50`, `0x80165B70` (all descriptive, low; template argument from the getter slot each instance calls). Outside: `LoadResourceFile(const char*, u32, bool)` `0x80066930` (descriptive, low) and `std::operator+(const string&, const char*)` `0x8006DCA0` (high: MSL signature and body). `CMapRecord()` zeroes `mValue`; `CFileData::IsLoaded()` tests `mFailed` (0x28) and `mData` (0x1C).
## Game CDropPii and CEffect (#113)

[F-113-2](findings/113-cdroppii-ceffect.md#f-113-2-classes) records `CEffect` (0x8C bytes, vtable `0x803A13C4`, bases `ICalc` and `enable_shared_from_this<CEffect>` at 4): flags 0xC, `BWorld*` 0x10, handle 0x14, `Param` 0x18 (0x14 bytes: `EffectInfo*`, enable flag, `u16` emit span and life or `f32` scale, `f32` radius and height), `Handle<Effect>` 0x2C, local `MTX34` 0x34, position 0x64, follow and target `weak_ptr<BWorldObj>` 0x70 and 0x84, target position 0x78 (high from code; member names descriptive). `EffectInfo` (0x18 bytes, table `0x803550E0`): index, name, node name, target node name, type, capture-group flag (medium). `nw4r::ef::MemoryManager` (0x3C bytes, vtable `0x803A11C0`, game-defined in `CEffect.cpp`), `MemoryManagerTmp<T>` (0x24 bytes, vptr at 0x20) and `@unnamed@CEffect_cpp@::EmitterWithParam` (0x168 bytes, `Param` at 0x154) are named from RTTI; their member names are descriptive. Function names: `InitializeEffect` `0x800A0150`, `FinalizeEffect` `0x800A0960`, `CEffect::Create` overloads `0x800A0DB0`–`0x800A19B0`, `Finalize` `0x800A1C40` (from `effFinalize`), `PrevEmissionLife` and `PrevEmissionLifeFollow` `0x800A12F0`, `0x800A1350` are descriptive except where noted; outside the block `BWorld::GetEffectGroupID` `0x80060C10`, `GetCaptureEffectGroupID` `0x80060C60`, `RemoveOldestEffect` `0x80061750`, `Lock(weak_ptr<BWorldObj>)` `0x8006A370` (descriptive) and `RandReal` `0x8019E010` (from `ScriptBinding_RandReal`).

[F-113-4](findings/113-cdroppii-ceffect.md#f-113-4-cdroppii-source-acceptance) names `CDropPii` (RTTI): constructor `0x8009FD40` (type bit 0x400), destructor `0x8009FDF0`, `Initialize(bool)` `0x8009FE50` (from the `CPii` vtable slot it overrides), thunk `@72@__dt__8CDropPiiFv` `0x800A0140` and vtable `__vt__8CDropPii` `0x803A1030`. `CDropPii` adds no members to `CPii` (high).

## Game BWorldLightSet and motion programs (#51)

[F-51-5](findings/51-bworldlightset-unit.md#f-51-5-complete-source-selected-light-animation-unit) proves all 27 retained functions and allocated storage. BWorldLightSet and its nested CMotionProgram are original RTTI spellings. The 0x68-byte owner derives from the canonical 0x48-byte BWorldObj, preserves inherited virtual order, and overrides the destructor, Calc, SetPosition, OnEnterWorld and OnLeaveWorld.

| Owner offset | Declaration | Evidence and confidence |
| --- | --- | --- |
|0x48|`u32 mReserved`|Uninitialized four-byte opaque storage; purpose and original type unknown|
|0x4C|`u32 mNextMotionID`|Initialized 1; incremented while skipping zero; high behavior confidence|
|0x50|`shared_ptr<CFileData> mAnmFile`|Two-word resource-file handle; high|
|0x58|`MotionList mMotionPrograms`|Size at 0x58, previous/next sentinel links at 0x5C/0x60; high|
|0x64|`f32 mMotionFrame`|Floating arithmetic and per-motion frame pointer; high|

The native motion allocation is 0x38 bytes:

| Motion offset | Declaration | Evidence and confidence |
| --- | --- | --- |
|0x00|`u32 mFlags`|Created 1, queued 2, playing 4, looped 8, stopped 32, paused 64; state mask 0x3E; loop count in upper 16 bits; high|
|0x04|`u32 mId`|Nonzero owner-generated identifier; high|
|0x08|`f32 mStartFrame`|Negative means carry current time when played directly, or zero when reached as a queued request; high|
|0x0C|`f32 mRate`|Frame-step multiplier and surplus-time divisor; high|
|0x10|`u32 mReservedState`|Four-byte zero store; further semantics and original signedness unknown|
|0x14|`std::string mName`|Twelve-byte assigned name and native string destruction; high|
|0x20|`u32 mReserved`|Uninitialized four-byte opaque storage; purpose and original type unknown|
|0x24|`f32* mpCurrentFrame`|Points to owner 0x64; script frame query dereferences it while playing/looped; high|
|0x28|`nw4r::g3d::ResAnmScn mResource`|Scene lookup and policy/length accesses; high|
|0x2C|`shared_ptr<AnmScnRes> mAnimation`|Two-word handle with DestroyG3dObj deleter; high|
|0x34|`int mLength`|Resource frame count, signed conversion and script query; high|

Member spellings are descriptive. Reserved words preserve unknown storage without claiming a recovered meaning or original type. Public method names follow behavior and corresponding script bindings. Private file-handle Load and ApplyFrame are reconstruction helpers with no retained symbol. Mutable MotionPtr reference signatures are supported by exact pointer reloads in the 496-byte factory and all wrapper comparisons. Own vtables are0x8039A208 and0x8039A160; own RTTI records are0x804B5B98/0x804B5BA0/0x804B5BA8. Outside names preserve existing sizes, types, bodies and split ownership: canonical LoadResourceFile0x80066930, scene-animation holder destructor0x80065980 and vtable0x80399A88, and CShadowCamera::SelectLight0x8010D7D0.

## F-372-4: SDI original API names

The six provisional names at `0x80224BB0`, `0x80224A90`, `0x80224B70`, `0x80224030`, `0x80224490`, and `0x80224D30` are corrected respectively to `ISD_InitCard`, `ISD_MountCard`, `ISD_UnmountCard`, `ISD_GetDeviceStatus`, `ISD_ResetDevice`, and `ISD_ReadCardRegister`. Confidence is high from pinned original IPL names plus all native consumer and complete SDI object controls. The last API consumes a `u32*` response. Record layout and function metadata remain unchanged; see [F-372-4](findings/372-fa-sd-driver.md#f-372-4-original-sdi-api-correction).
## F-374-6: USB memory wrappers

`uhf_ker_get_memory_block` (`0x80225420`, 32 bytes), `uhf_ker_release_memory_block` (`0x80225440`, 60 bytes), and `uhf_ker_check_cacheable` (`0x80225480`, eight bytes) retain their original IPL names and canonical MEM heap calls. Complete source object and final ELF native extents, instructions and actual targets are exact. There is no allocated unit storage; see [F-374-6](findings/374-fa-sdi-usb.md#f-374-6-accepted-memory-helper-source-unit).

## F-374-7: Complete SDI source state

The twenty retained original SDI names and all source extents follow the pinned IPL reference plus complete native comparisons. `__reqMutex` is 24 bytes at `0x80422FC0`; `__sdPaths` is a 96-byte array at `0x80422FE0`. Signed `__sdHeapId` is four bytes at `0x804B88E0`, initialized to -1. Seven four-byte shared words/pointers occupy `0x804B9288` through `0x804B92A0`; the original function-static heap-base pointer follows at `0x804B92A4`. Four anonymous initialized literals at `0x803C5280/0x803C52A0/0x803C52B0/0x803C52C0` have extents 32/16/16/16 and descriptive metadata names only. All twenty native function starts/extents, complete allocated payloads, real targets, source-only inputs and final ELF/MAP agree; see [F-374-7](findings/374-fa-sdi-usb.md#f-374-7-accepted-complete-sdi-source-unit).

## FA MSC callback bridge (#373)

[F-373-1](findings/373-fa-usb-boundaries.md) establishes the independent original `msc_drv.c` unit. Original names are `pfd_mscdrv_registar_callback` at `0x80222FB0`/`0x10`, `pfd_mscdrv_is_media_insert` at `0x80222FC0`/`0x04`, and local four-byte callback pointers `g_attach_func` at `0x804B9268` and `g_detach_func` at `0x804B926C`. The callback contract is `void (*)(s8 drive)` from actual FA callers. `uhf_msc_is_media_insert` at `0x80223C70`/`0x90` receives only a name-only outside-symbol update; its body/split/type/size remain with #374. All original spellings are corroborated by the pinned CC0 FA map; no new storage layout or implementation is published by this split-only preparation. Actual build and source-selection acceptance remain pending.

## F-373-8: Common driver and USB bridge/sense units

`pfd_get_media_drv_char` at `0x80222E90` has a byte capacity and byte return count, consumes the canonical PDM partition and PF volume records, and retains its original FA name. `pfd_mscdrv_registar_callback` (`0x80222FB0`, sixteen bytes) preserves the original misspelling and stores signed-byte callbacks in `g_attach_func`/`g_detach_func` at `0x804B9268/0x804B926C`; `pfd_mscdrv_is_media_insert` (`0x80222FC0`, four bytes) forwards its disk pointer to `uhf_msc_is_media_insert`. `__uhf_msc_cmd_check_sense` (`0x80223250`, 988 bytes) maps the three-byte sense result through the native-derived cases. All three complete source units and actual final ELF/MAP/source-only inputs are exact; see [F-373-8](findings/373-fa-usb-boundaries.md#f-373-8-three-independent-units-source-accepted).
# Pad controller sampling and rumble (#315–#317)

[F-316-1](findings/316-pad.md) establishes the complete `pad.cpp` unit at
`0x801B15A0–0x801B5308`. `PadData`, `BRumble`, `CWiiRemoteRumble` and
`CGCPadRumble` are original RTTI spellings. Public operation and member names
are descriptive. `PadData` is `0xC860` bytes: 48-byte alarm, signed ring indices
at 48/52, sixty four-channel direction/probe/device/KPAD samples, working and
public Wii state, four Wii rumble objects, per-channel activity masks and
connection callbacks, channel count, sixty four-channel GC samples, working
and public GC state, four GC rumble objects, activity minima, clamp settings,
and integer reset callback ID at `0xC858`. The callback retains canonical
`RegisterResetCallback`, whose int-return declaration was corrected in #1036.

`WiiPadState` is 240 bytes and derives from 176-byte `KPADStatus`: repeat at
176; hold/trigger/release/button-repeat/repeating fields at 180/184/188/192/196;
unsigned-short delay at 200; processing byte at 202; stable `Vec2` at 204;
idle counters at 212/216/220; shake hold/trigger at 224/228; signed probe result
at 232 and device type at 236. `GCPadState` is 68 bytes and derives from
12-byte `PADStatus`: button fields at 12–28, four signed-short stick deltas at
32–38, unsigned-short delay at 40, `Vec2` sticks at 44/52, float triggers at
60/64. Inheritance and flat button fields are established by exact aggregate
copy instruction streams.

`BRumble` is 88 bytes, with enabled/block-count/channel/state/pattern/cooldown
at 0/4/8/12/16/20, `OSAlarm` at 24, `OSTime` at 72, and vptr at 80. Its virtual
order is destructor, const availability predicate, start, stop and hard stop.
`RumbleStep` is four bytes: on-time 12 bits, hard-stop one bit, flags three
bits, off-time 12 bits, end flags four bits. Only the hard-stop flag semantics
are established. `CDisablePadRumble` is the original empty scope guard for the eight channel
block counters. `InterruptLock` has a four-byte saved interrupt state and an
empty nontrivial base, modeled as inferred `NonCopyable` with inaccessible
copy operations; native destructor and exception lifetimes establish its
presence. These declarations reproduce all native code and exception records.

# Game color-button tag processor (#54)

`CColorBtnTagProc` is an original RTTI spelling. The complete class is 16 bytes: inherited wide-character tag-processor vptr at zero, saved `CharWriter::GradationMode` at four, and saved start/end `ut::Color` values at eight/twelve. The inherited destructor, `Process` and `CalcRect` virtual order is preserved. `Attach` and `SetColor`, static-storage names and member names are inferred descriptions; their signatures and native bodies are established by [F-54-2](findings/54-boss-pii-color-tag.md#f-54-2-complete-color-tag-source-acceptance). `SetColor` takes two colors by value; attachment returns void.

The singleton is `0x803E6590` (16 bytes), start colors `0x803E65AC` (five four-byte values), and end colors `0x803E65CC` (five values). Compiler-generated twelve-byte registration nodes occupy `0x803E6580`, `0x803E65A0` and `0x803E65C0`; their numeric local names follow the actual compiler output rather than recovered original names. The source-selected constructor entry, RTTI, vtable, jump table and exception ranges retain every native value. Three inferred additive `CharWriter` accessors expose start/end colors and gradation at existing offsets `0x18`, `0x1C` and `0x20`; the four existing rebuilt consumers preserve their allocated sections and relocations.

## F-329-2: Complete CLayout contract

`CLayout` is an original RTTI spelling, with ICalcDraw inheritance and complete size `0xE8`. Constructor `0x801CB1C0`, destructor `0x801CC070`, Calc `0x801D62F0` and Draw `0x801D63B0` follow vtable `0x803C0AF4`; inherited CalcMtx/CalcView remain in their canonical slots. The complete typed declaration, fourteen member offsets, archive-path array and shared FontMap Load contract are established in [F-329-2](findings/329-clayout-contract.md). Member names, FontMap typedef, Load and FindPane names are inferred descriptions where native mangled spelling is unavailable. The initial declaration introduces no reconstructed function bodies; its follow-up supplies the native one-name inline wrapper without source-unit credit. [The #56 motion contract](findings/56-ccombodisplay.md#shared-motion-and-text-contracts) corrects CMotionProgram's `mpTarget` at `0x0C` to `PaneMP*`, the mapped owner returned by `0x801D2180`, preserving its `0x18` size. Additive motion, text, frame-length and resource declarations have native argument/result evidence; their descriptive spellings remain inferred.

`IGameMap` is an original RTTI spelling. Its twelve-byte layout has `ICalcDraw` at zero and `enable_shared_from_this<IGameMap>` at four, with forty-three virtual slots in the 180-byte vtable at `0x803A1D00`. [The #56 complete interface contract](findings/56-ccombodisplay.md#complete-map-interface-prerequisite) records all slot identities and supporting bindings/factory RTTI. Method spellings and otherwise unobservable const qualifiers are inferred descriptions. A canonical-header producer matches all five pure entries and all thirty-eight concrete entry bodies and targets, including destructor `0x800A5980`. No opaque vtable gaps or synthetic members are introduced.
### F-54-3: CPii and CEnemyPii prerequisite contracts

`CPii` remains `0x14C` bytes and `CEnemyPii` adds no storage. Its complete reconstructed members, offsets, pointee evidence and virtual contract are recorded in [F-54-3](findings/54-boss-pii-color-tag.md#f-54-3-boss-prerequisite-contracts). `Initialize(bool background)` is supported by forwarding to the existing resource-loader mangling. The prior invented `BWorldObjEffect` forward declaration is replaced by the original RTTI-supported `CMiniMapMarker` class; the inherited `GetEffect` spelling and slot remain unchanged. Member and method spellings are descriptive, medium confidence. Independent boss constructor/destructor, helper and full-vtable controls reproduce resolved native bytes; this header prerequisite adds no source-completion credit.
### F-54-4: Original HUD helper contracts

`IWorldObjStat` is the original four-byte six-slot base of `CHPBar`; its RTTI spelling replaces the earlier inferred `IDrawLayer` identity in `DrawLayerSet` and the native `BModelObj::AddDrawLayer` callee mangling at `0x800570D0`. The old name remains a compatibility typedef for the external #39 owner's untouched source. The complete `CHPBar` (`0x13C`) and `CMiniMapMarker` (`0xF0`) member layouts and original nested `CHPBar::CDamage` spelling are recorded in [F-54-4](findings/54-boss-pii-color-tag.md#f-54-4-complete-hud-helper-header-prerequisites). Constructor/destructor and full HP-vtable consumers prove native layout and relocations using canonical `CLayout`. New member/method spellings are descriptive, medium confidence; no outside source body or split is reconstructed by these header prerequisites.

## Complete FA SD driver storage and format records (#372/#373)

| Address or record | Layout / identity | Evidence |
| --- | --- | --- |
| `sSdDriver`, `0x80422BE0` | 28-byte volatile `PFD_SD_DRIVER`; seven existing status/size/handle/disk/media/drive fields, final ordinary alignment | [F-373-10](findings/373-fa-usb-boundaries.md#f-373-10-complete-shared-sd-driver-source-acceptance); real object and final ELF exact |
| `sSdHandle`, `0x80422C00`; `sSdSectorBuffer`, `0x80422C40` | Forty-byte canonical `ISD_HANDLE`, then 512-byte sector buffer aligned to 32 | Same; source-selected complete DOL exact |
| `sSdInsertCallback`, `0x804B9240`; `sSdEjectCallback`, `0x804B9244`; `sSdIntrEvent`, `0x804B9260` | Two four-byte callbacks and a four-byte event word aligned to 32; complete small BSS 36 bytes | Same; no invented intervening fields |
| `SD_FORMAT_PARAMETERS` | 32 bytes: partition start `0x00`, partition count `0x04`, cluster byte `0x08`, FAT sectors `0x0C`, volume ID `0x10`, FAT type `0x14`, reserved sectors `0x18`, total sectors `0x1C` | Native geometry/BPB accesses and full source-selected stream; descriptive record/member names |
| `SD_FORMAT_GEOMETRY` / `SD_FORMAT_CAPACITY_RULE` | Sixteen-byte geometry: alignment `0`, heads `4`, track sectors `8`, cluster byte `12`; rule prefixes two capacity words, total 24 bytes | Fourteen exact rules / 336 bytes at `0x8036C480`; lower bound exclusive, upper inclusive |
| `sSdFunctions`, `0x8036C460` | Eight-entry native `PDM_FUNCTBL`, 32 bytes, exact source function positions and absolute relocations | Complete real-object and final-ELF controls |
### F-54-5: Source-selected CBossPii

`CBossPii` is a fieldless `CEnemyPii` subclass, size `0x14C`, type bit `0x200`. Its complete unit has 20 functions / 2,108 instruction bytes, a native 144-byte vtable with the secondary destructor thunk at `this-0x48`, and exact retained EH/data/small-data ranges. `CPii`'s HP/personal-data getters are non-const; the HP/marker setters take shared pointers by value. The marker flag is `0x4` and the boss HP-bar constructor argument is `0x6A`. Class names are original RTTI; method/member spellings are descriptive with medium confidence. [F-54-5](findings/54-boss-pii-color-tag.md#f-54-5-complete-boss-source-and-original-issue-acceptance) records complete native accounting, discarded duplicate metadata, and all 22 original issue functions across the boss and color-tag units.
## F-336-4: Complete BUI and BUILayout base declarations

Original RTTI names BUI (`0x4C`) and BUILayout (`0x134`) identify ICalcDraw/enable_shared_from_this inheritance and an embedded CLayout at `+0x4C`. Complete typed storage, nine base virtual slots, four Load overload slots and the preserved nonvirtual GetLayout are recorded in [F-336-4](findings/336-bui-contract.md). Callback/hook/input spellings and the private noncopyable CCallbackListOwner are descriptive source inferences; no standard-library owner identity or transfer semantics are asserted. Native exception tables bind all three callback-list owners to the same retained deletion helper at `0x800DA1E0`. Independent instruction, target, EH, vtable and RTTI controls establish the contract; source selection remains with the owning issues.

## CLayout attachment owner correction (#329)

[F-329-3](findings/329-clayout-attachment.md) supersedes the DC/E0 exception-callback interpretation: native allocation and control-block vtable `0x803C0640`, RTTI `0x804B83A0` and name `0x803C0654` prove `shared_ptr<CLayout::CAttach>`. CAttach is an original nested name; `mAttach` is descriptive. The complete CLayout remains E8. CAttach allocates twenty bytes and stores a parent CLayout pointer and SDK Pane pointer at zero/four; its remaining tree contract is not yet declared.

| `CPii::LoadScript` / `CEnemyPii::LoadScript` | `0x800DCDF0` / `0x800A4BE0` | Nonvirtual `bool` members; generic filename, integer mode and const shared environment reference; enemy supplies filename | Native caller and canonical engine loader forwarding; descriptive spelling inferred | F-115-2 |
## F-769-4: Complete post-field resource record

`nw4r::ef::PostFieldInfo` is 112 bytes. The shared collision prefix ends with `EmitterInheritSetting` at `0x3C`; the effect-resource name index is an unsigned halfword at `0x46`, wrap flags are at `0x48`, and three wrap vectors begin at `0x4C`, `0x58`, and `0x64`. The original type identity comes from retained crossing-function signatures. Added member spellings are descriptive; native accesses establish their types and behavior, and five frozen BREFF records independently declare the 112-byte extent. The duplicate private `ParticlePostField` view is removed. [F-769-4](findings/769-particlemanager-fields.md#f-769-4-complete-original-postfieldinfo-resource-layout) records native addresses, resource provenance, ABI layout and full object/ELF/DOL controls.

| `CItem` / `CFacility` | `0x800C0C60` / `0x800A4C10` | Original RTTI classes; BModelObj-derived CItem size 0x104 with flags and shared engine, Facility size 0x110 with flags and shared marker | Complete native constructor/destructor/vtable and seven-function Facility consumer; descriptive members/methods inferred | F-115-4 |
| `ICalc::CalcMtx` | `0x80063DC0` | Inherited empty matrix slot, including ICalcDraw consumers | Direct ICalc-derived CDisplayPhaseCtrl native RTTI/vtable and source-selected proof; corrects header-derived redundant override identity | [F-59-2](findings/59-icalc-inheritance.md) |

| `CPiiPersonalData` / `CPpuRegister` | `0x800EC3D0` / `0x801119B0` | Complete native 0x80 personal-data layout and 8-byte shared vector register wrapper | F-173-3/F-209-4 plus exact CEnemyInfo native consumer; wrapper name and accessor/constructor types inferred | F-115-6 |
| `PiiSpeciesData` / `CPiiTex` | `0x80124EE0` / `0x800F6580` | 28-byte packed species record and 28-byte vector/texture/world owner | Exact native typed UI consumer; texture class spelling original RTTI, record/member/method names descriptive | F-115-6 |
| `IColorConst` / `GetColorConstants` | `0x8006D2E0` | Original six-slot color interface without virtual destructor; singleton accessor | F-55 plus exact native slot-0x18 UI consumer; accessor name inferred | F-115-6 |
| `CLayout::AppendMotion` / pane visibility / named texture | `0x801D10F0` / `0x801D3DF0` / `0x801D4590` | Additive typed UI methods; append differs from QueueMotion replacement behavior | Native implementation/call chain and exact 14-function consumer; descriptive names inferred | F-115-6 |

## Game controller interfaces (#57)

[F-57-1](findings/57-controllers.md) establishes the original RTTI spellings `CControllerClassic`, `CControllerComposition`, `CControllerGCPad`, `CControllerNunchaku` and `CControllerWiiRemote`. All derive from the canonical 0x34-byte `BController` without adding virtual slots. Classic, GCPad and Nunchaku remain 0x34 bytes; Composition is 0x40 with vector data/size/capacity at 0x34/0x38/0x3C; WiiRemote is 0x38 with its Classic-input selection Boolean at 0x34. Method and field names are descriptive. Native global constructors independently agree with these sizes and layouts.

`MotorPattern` is 12 bytes: signed script pattern index at 0, Wii `RumbleStep` pointer at 4, GameCube `RumbleStep` pointer at 8. The table at 0x803550B8 contains three indexed records. Only nonnegative index constants are visible, so their stored signedness is not independently distinguished. `GetControllerTable` at 0x80124A50 returns the native composition singleton at 0x803E9088; `IsExtensionControllerEnabled` at 0x80124970 reads the Boolean written at 0x80124960. `GetControllerDisconnectState` at 0x8006FA00 returns the signed maximum disconnection state. `BController::SetChannel` exposes the existing signed channel at offset 4, which 0x801248C0 initializes for all sixteen controllers.

The GameCube glyph pool has six twelve-byte wide literals at `0x8039B0E0`, `0x8039B0EC`, `0x8039B0F8`, `0x8039B104`, `0x8039B110` and `0x8039B11C`. Their descriptive symbol names identify the direction glyph and button masks 32, 16, 128, 256 and 2048 respectively. Each contains two control characters, the observed glyph, two closing control characters and its terminator. These exact string extents replace the former opaque 72-byte metadata block; see [F-57-4](findings/57-controllers.md#f-57-4-concrete-controller-source-acceptance).

[F-60-4](findings/60-cdividedground-unit.md#f-60-4-pod-vector-destructor-formatstring-and-the-pkc-loader) names `CDividedGround::LoadPkc(int, const shared_ptr<NPpu::CEnvironment>&, const char*)` `0x80071B80` (public, called from `0x800A98E0`), `VFormatString(std::string&, const char*, va_list)` `0x801F5A50`, `CPpuEngine::LoadScriptFile(const char*, u32)` `0x80100720` (static; no `this`), and the `.rodata` tables `gStageRanks` `0x8035B9D0` and `gStageWorlds` `0x8035B9F8`. `StageID` is four signed 8-bit bitfields (`mKind`, `mRank`, `mWorld`, `mReserved`). All names are descriptive.
One-argument CUIList member callback RTTI and native helper contract: see [F-336-6](findings/336-bind-one.md). Native specialization flags are false,true; helper spelling and placeholder resolution source form remain inferred.
## Game CMsgBalloon helpers and UI declarations (#142)

[F-142-5](findings/142-cmsgballoon-start.md#f-142-5-the-12-helpers-are-emitted-by-a-reconstructed-init) records the reconstructed `CMsgBalloon::Init` `0x800C5CC0` and the declarations it needs. `CUIManager` (`include/game/CUIManager.h`) derives from `BUI` and adds a `weak_ptr<BUI>` at 0x4C (size 0x54, constructor `0x801C9680`). `BUI::AddChild(const Child&)` is `0x801C8E40`. `BUILayout::AttachChild(const char*, shared_ptr<BUILayout>)` is `0x801C9FC0`, and `BUILayout::SetText` is an inline forward to `CLayout::SetText` (`0x801D60B0`). `GetMessageString(int, const wchar_t*)` (`0x80086620`) drops its second argument and returns `GetMessageText(id, gEmptyText)`. All five names are descriptive, medium confidence.
### CComboDisplay complete layout (#56)

Original RTTI identifies `@unnamed@CComboDisplay_cpp@::CComboDisplay` and its public `IComboDisplay` base. The 260-byte private object has state at 4, CLayout at 8 (232 bytes), shared motion program at 240 (8 bytes), signed combo count at 248, signed gauge at 252, and signed end-animation frame count at 256. Virtual overrides preserve ICalcDraw order followed by gauge getter/setter, count getter/setter and ShowComboCount. Factory, state/member and previously unnamed method spellings are descriptive. All eighteen original functions, every target, eleven EH records and all retained owned data are exact in the selected-source full frozen-DOL build; see [F-56-1](findings/56-ccombodisplay.md#f-56-1-complete-source-selected-combo-display).
## F-115-8: CEnemyInfo and IEnemyInfo

Original RTTI names IEnemyInfo and CEnemyInfo. IEnemyInfo derives ICalcDraw with virtual destructor and appends Show(shared personal data reference, bool, int), then Hide. CEnemyInfo size 0x118: CLayout at 0x4 (0xE8 bytes), six pane pointers at 0xEC, int frame count at 0x104, bool flash at 0x108, shared CPiiTex at 0x10C, bool collection state at 0x114. Natural alignment fills the remaining bytes. Class spellings are original; method and member spellings descriptive and inferred. The complete native 14-function, data and EH evidence is [F-115-8](findings/115-enemy-info-pii-facility.md#f-115-8-complete-original-issue-source-acceptance).

## Game CMsgBalloon constructor block (#143)

[F-143-1](findings/143-cmsgballoon-constructor.md#f-143-1-the-23-functions-of-0x800c6690-0x800c6be4) names `CMsgBalloon::CMsgBalloon()` `0x800C66F0` and the `IMsgBalloon`, `enable_shared_from_this`, vector constructor chain and `allocator` constructors in `0x800C6780`-`0x800C6BE0` by their compiler names, with `lbl_803A5208` and `lbl_803A52AC` as `__vt__11CMsgBalloon` and `__vt__11IMsgBalloon`. `CMsgBalloon::Entry` (`weak_ptr<BWorldObj>` at 0, `wstring` at 8, `VEC3` at 0x14) is declared in `include/game/CMsgBalloon.h` (medium confidence, [F-143-3](findings/143-cmsgballoon-constructor.md#f-143-3-cmsgballoonentry)).
| `CDisplayPhaseCtrl` | `0x80070C20` | Original RTTI class; ICalc-derived size 28, phase at 4, requested/forced bools at 8/9, CLayout/Pane pointers at 12/16 and shared motion at 20 | Complete native constructor/member/vtable and nine-function source-selected proof; member/method names descriptive | [F-59-1](findings/59-display-phase-controller.md) |

## Game controller-table state (#221)

[F-221-2](findings/221-controller-table.md#f-221-2-complete-controller-table-translation-unit) recovers `ControllerTable.cpp`, a descriptive filename, at `0x801248C0`–`0x80124C80`. The canonical concrete class names and complete declarations are owned by #57. Four arrays have the original element counts and sizes: Wii Remote `0x803E8D28`, 4 × 56; Nunchaku `0x803E8E08`, 4 × 52; Classic `0x803E8ED8`, 4 × 52; GC Pad `0x803E8FA8`, 4 × 52. Their static pointer table is `0x803B3030`, 4 × 4 pointers in `IController::Type` order. The compiler destructor-registration record is `0x803E9078`, twelve bytes; the composition object is `0x803E9088`, 64 bytes. The extension-enable flag is a one-byte boolean at `0x804B77D8`, initialized true; the following seven bytes are separate native alignment. Private static names are descriptive, with high layout/behavior confidence and no original-spelling claim. Public manager functions, all addresses and native body sizes are listed in F-221-2. `ShutdownControllers` is an empty cleanup hook called at `main` address `0x8012C508`; its spelling is descriptive.
[F-199-1](findings/199-csavemanager.md) splits `game/CSaveManager.cpp` (file name original, from the `@unnamed@CSaveManager_cpp@` RTTI at `.data` `0x803AD6D0`) over `.text` `0x80108290`-`0x8010C488`. `@unnamed@CSaveManager_cpp@::CSaveManager` (vtable `0x803AD69C`, type info `0x804B7340`) derives from `ISaveManager` (vtable `0x803AD780`, type info `0x804B7348`, an `ICalcDraw` with six pure slots `LoadSync`, `RequestSave`, `RequestSaveWithoutBackup`, `RequestSaveCommitOnly`, `RequestDelete`, `IsIdle`; descriptive, low) and has `shared_ptr<CFiber>` at 0x4, `shared_ptr<CLayout>` at 0xC, `std::queue<WorkType>` (a `deque`, 0x18 bytes) at 0x14 and `shared_ptr<IDlgMsgWin>` at 0x2C; the constructor `0x80109880` stores `this` in `.sbss` `0x804B9070`. `Work(WorkType)` `0x80108290` is the fiber body (original name unknown, descriptive, medium); `WorkType` 1 to 5 are load, save, save without backup, save commit only and delete (descriptive, low). `IDlgMsgWin` slots 7 to 15 are `SetControllerSlot`, `OpenMessage`, `OpenImageMessage`, `IsMessageClosed`, `IsAnimationDone`, `SetDefaultYes`, `OpenYesNo`, `IsYesNoClosed`, `IsYesSelected`, with non-virtual inline `AskYesNo` `0x80108600` and `ShowMessage` `0x80108670` and the factory `IDlgMsgWin::Create(bool, bool)` `0x80084D40` (descriptive, low to medium). Renamed outside the unit for the link: `SetControllers__8CUIInputF...` `0x8006EC10` (the 0x40-byte input object `0x803E9088`, `CUIInput` is descriptive, low), `QueueMotion__7CLayoutFPCc` `0x800703C0`, `GetMessageTextOrEmpty` `0x80086620` (ignores its fallback text), `GetUIInputManager` `0x80124A50`, `GetDefaultFontMap` `0x80126280`, `CheckNandSpace` `0x80167E90` and `DeleteSaveFiles` `0x80165B80` (descriptive, low), `gSharedLayoutArchives` `0x804B9098`, and the `shared_ptr<CLayout>`, `shared_ptr<IDlgMsgWin>` and `vector<void*>` instances that this file is first to emit.
[F-199-1](findings/199-csavemanager.md) splits `game/CSaveManager.cpp` (file name original, from the `@unnamed@CSaveManager_cpp@` RTTI at `.data` `0x803AD6D0`) over `.text` `0x80108290`-`0x8010C488`. `@unnamed@CSaveManager_cpp@::CSaveManager` (vtable `0x803AD69C`, type info `0x804B7340`) derives from `ISaveManager` (vtable `0x803AD780`, type info `0x804B7348`, an `ICalcDraw` with six pure slots `LoadSync`, `RequestSave`, `RequestSaveWithoutBackup`, `RequestSaveCommitOnly`, `RequestDelete`, `IsIdle`; descriptive, low) and has `shared_ptr<CFiber>` at 0x4, `shared_ptr<CLayout>` at 0xC, `std::queue<WorkType>` (a `deque`, 0x18 bytes) at 0x14 and `shared_ptr<IDlgMsgWin>` at 0x2C; the constructor `0x80109880` stores `this` in `.sbss` `0x804B9070`. `Work(WorkType)` `0x80108290` is the fiber body (original name unknown, descriptive, medium); `WorkType` 1 to 5 are load, save, save without backup, save commit only and delete (descriptive, low). `IDlgMsgWin` slots 7 to 15 are `SetControllerSlot`, `OpenMessage`, `OpenImageMessage`, `IsMessageClosed`, `IsAnimationDone`, `SetDefaultYes`, `OpenYesNo`, `IsYesNoClosed`, `IsYesSelected`, with non-virtual inline `AskYesNo` `0x80108600` and `ShowMessage` `0x80108670` and the factory `IDlgMsgWin::Create(bool, bool)` `0x80084D40` (descriptive, low to medium). Renamed outside the unit for the link: `CControllerComposition::SetControllers` `0x8006EC10` and `GetControllerTable` `0x80124A50` (names from another contributor; the object is the 0x40-byte input `0x803E9088`), `QueueMotion__7CLayoutFPCc` `0x800703C0`, `GetMessageString` `0x80086620` (ignores its fallback text), `GetDefaultFontMap` `0x80126280`, `CheckNandSpace` `0x80167E90` and `DeleteSaveFiles` `0x80165B80` (descriptive, low), `gSharedLayoutArchives` `0x804B9098`, and the `shared_ptr<CLayout>`, `shared_ptr<IDlgMsgWin>` and `vector<void*>` instances that this file is first to emit.

## Game CMsgBalloon list block (#144)

[F-144-1](findings/144-cmsgballoon-list.md#f-144-1-the-25-functions-of-0x800c6bf0-0x800c70f4) names the `shared_ptr<BUILayout>` vector destructors `0x800C6BF0` and `0x800C6C50`, the `list<CMsgBalloon::Entry>` constructor chain `0x800C6CB0`-`0x800C6EC0`, the `__list_deleter` destructor `0x800C6ED0`, its `begin`/`end` iterator functions `0x800C6F50`, `0x800C6FA0` and iterator constructors `0x800C6F90`, `0x800C6FE0`, the `__list_imp` and `list` destructors `0x800C6FF0`, `0x800C7050` and `CMsgBalloon::Draw` `0x800C70C0` (slot 4) by their compiler names. Outside the block, `erase` `0x800CA010` and the `__vec_deleter<shared_ptr<BUILayout> >` destructor `0x800CA170` are named so the links resolve.
## Game CGameEnding second block (#117)

[F-117-1](findings/117-cgameending-block.md#f-117-1-the-21-functions) names the `IGameMap` defaults of `0x800A5B40`-`0x800A5CE0` and their earlier copies (`0x8006BAB0`, `0x80078480`, `0x80078F80`, `0x80078F90`, `0x8008D2B0`, `0x80093630`) after the `IGameMap.h` slots, high confidence, and the `@unnamed@CGameEnding_cpp@::CGameEnding` overrides `RunAction` `0x800A5CF0`, `Draw` `0x800A5D90`, `CalcView` `0x800A5E10`, `CalcMtx` `0x800A5E60`, `Calc` `0x800A5EB0` and the getters `GetPauseMenu` `0x800A6450`, `GetMapDesc` `0x800A6460`, `GetWorld` `0x800A6470`. `0x801D40D0` is `CLayout::SetPaneVisible(const char*, bool)` (high; it follows `SetVisible(Pane*, bool)` and `Calc` passes pane names). CGameEnding members: `GetMapDesc` pointer 0xC, `shared_ptr<BWorld>` 0x10, `shared_ptr<IPauseMenu>` 0x18 (this corrects the shadow camera guess of F-116-3), `CLayout` 0x20/0x114/0x220, motion pointers 0x108/0x1FC/0x308, state 0x110, line count 0x204, `list<wstring>` 0x208/0x214 (high for offsets, medium for the member spellings). The nine `IGameEnding` slot names are low confidence, see [F-117-5](findings/117-cgameending-block.md#f-117-5-igameending).
## F-160-3: CNoticeDisplay.cpp first block

[F-160-3](findings/160-cnoticedisplay-first-block.md#f-160-3-the-20-functions-of-160-reconstructed) reconstructs the 20 functions of `0x800D00B0`-`0x800D1024` in `src/game/CNoticeDisplay.cpp`. Class names are original (`@unnamed@CNoticeDisplay_cpp@::CNoticeDisplay`, `BNoticeDisplayImpl`, `BOther`, `CSimpleText`, `CYouWin`, `CFight`, `CLose`). `BNoticeDisplayImpl` (0x460): `CLayout` at 4, `CFiber` at 0xF0. `BOther` (0x550): `int mType` 0x460, `CLayout mSubLayout` 0x464, `int mEnemyImageCount` 0x54C. `CSimpleText` (0x560): `int mCloseRequested` 0x550, `int mAutoCloseFrames` 0x554 (-1), `int mFrameCount` 0x558, `bool mPauseReleased` 0x55C. `CYouWin` (0x728): `int mWinner` 0x550, `f32 mWinnerParam` 0x554, `CLayout mWinnerLayout` 0x558, `CLayout mResultLayout` 0x640. `CLose` (0x468): `int mState` 0x460, `int mType` 0x464. `CFight` (0x468): `int mType` 0x460. `CNoticeDisplay` (0x14): `shared_ptr<BNoticeDisplayImpl> mpImpl` 4, `int mWinner` 0xC, `f32 mWinnerParam` 0x10. Member names, the `Init` helper names `StartNotice`, `SetupLayouts` and `LoadLayouts`, `FormatAreaName` `0x800CF8A0`, `IsStage7EvenMap` `0x80125180` (world 7 and an even map number), `gAreaPaneNames` `0x803A58E8` and `MapDesc` (`include/game/MapDesc.h`, first member the `MapIndex`) are descriptive, medium confidence.

### CColorConst complete layout (#55)

Original RTTI and the shared-pointer deleter type names identify `@unnamed@CColorConst_cpp@::CColorConst`, derived from `IColorConst`. The object is 468 bytes: the vtable pointer at 0, then 29 palettes of four `nw4r::ut::Color` values at offset 4 with a 16-byte stride. Virtual order is `GetTextColor`, `GetVertexColor`, `ApplyTextColor(Pane*)`, `ApplyTextColor(CLayout*)`, `ApplyVertexColor(Pane*)`, `ApplyVertexColor(CLayout*)`, with no virtual destructor. Text palettes 0 to 10 and picture palettes 11 to 28 come from the `colorconst` layout; palettes 6 to 10 supply the five tag-processor color pairs. A text palette holds its start color in slots 0 and 1 and its end color in slots 2 and 3, so `GetTextColor(index, end)` reads slot `2 * end`. The single instance is held in a `shared_ptr` at `0x804B8FC0` (`sColorConst`) created by `InitializeColorConstants(bool)` and released by `FinalizeColorConstants()`. Class and layout spellings are original; the free-function and member-variable names are descriptive with medium confidence. Evidence and remaining differences: [F-55-2](findings/55-colorconst.md#f-55-2-source-reconstruction-and-remaining-differences).

IUIListItem: abstract4-byte interface, vtable803C14FC20 bytes, first retained destructor801DA0C0; two final slots801DA100/801DA110. Class name original, method spellings/constness inferred. Full native declaration and725-consumer proof: [F-336-7](findings/336-list-item-interface.md).
## F-200-1: CSaveManager.cpp second block

[F-200-1](findings/200-csavemanager-2.md#f-200-1-the-26-functions-of-200-reconstructed) reconstructs `0x80108B90`-`0x801090E4`. `CSaveManager::WaitIdle` and `StartWork(WorkType)` are descriptive names (medium confidence) for `0x80108D30` and `0x80108DC0`. `CFiber::CFiber(const char*, const function<void()>&, u32)` at `0x80108EA0` takes an unused name, "CSaveManager" here. `__deque_imp::first` (`0x80108BE0`) is a descriptive name for the body of `front`. `0x800FC0C0` is `__cdeque_buf<void*>::cap`; `0x800FC080` lost that name (see Traps in the finding).
### CPiiListFilterSort layout (#173)

[F-173-4](findings/173-cpiilistfiltersort.md#f-173-4-complete-cpiilistfiltersort-layout) records the complete 0x20-byte `CPiiListFilterSort` (no vtable): filter type 0, type index 4, prefix 8, invert-owner flag 0xC, sort type 0x10, `shared_ptr<CPiiPersonalData>` 0x14, player index 0x1C. Constructor `0x800E5D00` and destructor `0x800E5D20` are named and match at 100%. The pointee type of the `shared_ptr` and the role of 0x1C are unproven.
## F-139-4: Complete CModel allocation contract

CMiniMapMarker original constructor 0x800C4810, destructor 0x800C4860 and Initialize(int, const char*, bool) 0x800C48C0 now have complete source-selected native proof. The class is 240 bytes with signed type at 0, CLayout at 4 and Pane pointer at 236. Its separate source-unit boundary precedes CMoneyLifeDisplay and follows CMiniMap's deleter destructor; see [F-140-3](findings/140-minimap-marker.md#f-140-3-complete-marker-source-acceptance). Class spelling is original RTTI; method/member/source-basename spellings are inferred descriptive names.

Original RTTI identifies CModel, ScnMdlSimpleData, BPartialAnm, TPartialAnm<true>, PartialAnmManager and CMotionProgram. CModel is 220 bytes: private canonical IScnObjCallback at 0, private ICalcWorldCallback at 4, ScnMdlSimpleData at 8 (20 bytes), shared AnmObjChrRootScale at 28, vector of shared AnmObj at 36, int motion serial at 48, TPartialAnm<true> at 52 (28 bytes), shared PartialAnmManager at 80, Boolean animation change at 88, twelve-byte function<void(CModel*, const shared_ptr<CMotionProgram>&)> at 92, Boolean aiming modes at 104/105, ModelAim at 108 (72 bytes), VEC3 target at 180, Boolean pending target at 192, and canonical AABB at 196. Natural alignment supplies intervening bytes. The callback signature is embedded original RTTI; the primary SDK base is proven by native vtable signatures and Load storing this into ScnObj::mpFuncObjExec. Existing ICalcDraw remains unchanged.

ScnMdlSimpleData contains shared ScnMdlSimple at 0 and vector of shared CFileData at 8. BPartialAnm contains shared current motion at 0, motion list at 8, frame at 20 and vptr at 24, naturally emitted by declaring fields before its first virtual method. The native destructor plus three pure motion-operation slots define its interface. GetBounds and motion-operation spellings are inferred descriptive names. The constructor at 0x801B7AA0 reproduces all 196 resolved code bytes, 112 EH bytes and nine referenced data objects. This declaration prerequisite claims no reconstructed CModel translation unit; see [F-139-4](findings/139-item-layout-manager.md#f-139-4-complete-cmodel-allocation-prerequisite).

ModelAim is a descriptive inferred class name for the native node-aim controller: ScnMdlSimple pointer at 0, parent node ID at 4, MTX34 bind transform at 8, yaw/pitch at 56/60, Boolean limit flag at 64 and owned Parameters pointer at 68. Parameters is twelve floats: maximum pitch/yaw steps at 0/4, pitch/yaw follow rates at 8/12, pitch inner limits at 16/20, pitch outer limits at 24/28, yaw inner limits at 32/36 and yaw outer limits at 40/44. Names are descriptive; native angle calculations, constructors, destructor, parameter setter and ownership establish types and layout. Native producers 0x801C4EC0, 0x801C4EE0 and 0x801C5040 and their EH match independently in the #194 consumer investigation.

## Game CShadowCamera unit (#207)

[F-207-3](findings/207-cshadowcamera.md#f-207-3-reconstructed-functions) names `CShadowCamera::CShadowCamera(u32)` `0x8010C490`, `~CShadowCamera` `0x8010C520`, `Setup(u16, u16, int, bool)` `0x8010C5A0`, `UpdateVisible` `0x8010C750`, `CalcLightProjection` `0x8010C7A0`, `SetupShadowModel(shared_ptr<CModel>)` `0x8010D630` and the file-local `SetupShadowMaterial` `0x8010D140`, with `lbl_803ADB34` as `__vt__13CShadowCamera` and `lbl_804B7358` as `__RTTI__13CShadowCamera`. `CShadowCamera` layout is [F-51-2](findings/51-bworldlightset-unit.md); the class name is original (RTTI), the method names other than the constructor, destructor and `UpdateVisible` are descriptive. `ResUserDataEntry::GetNumData` and `GetColor`, `CaptureTexture::GetTexObj` and `LINE3(const VEC3&, const VEC3&)` are inline additions to shared headers (low confidence in original spelling).

CUIList complete180-byte typed declaration,14 virtual slots, four input/selection flags and three function<void(BUI*)> callback owners: [F-337-1](findings/337-cuilist-unit.md). Actual insertion+RTTI proves174/178/17C payloads. The17C owner is scrolling notification, superseding the earlier cancel interpretation. Native unitDA0C0–DC6C0 remains NonMatching with explicit original-unit fallback.

## Game CMsgBalloon update block (#145)

[F-145-1](findings/145-cmsgballoon-calc.md#f-145-1-the-22-functions-of-0x800c7100-0x800c7ee4) names `CMsgBalloon::CalcView` `0x800C7100` (slot 3), `CalcMtx` `0x800C7370` (slot 2), `Calc` `0x800C7410` (slot 1), the private helper `UpdateBalloon(BWorldCamera*, CLayout*, bool*)` `0x800C7D90` (descriptive, medium: it projects `mOffset` through the camera and moves the `root` pane), the `vector<shared_ptr<CPiiTex> >::iterator` functions `0x800C71A0`-`0x800C7300`, `begin` `0x800C71C0`, `make_iterator` `0x800C7200`, `end` `0x800C7310`, `__vec_constructor::begin`/`end` `0x800C7240`, `0x800C7350`, `list::size() const` `0x800C7D40` with `compressed_pair::first() const` `0x800C7D70` and its storage `first() const` `0x800C7D80`, and the `nw4r::math::MTX44` default constructor `0x800C7EE0`. `CMsgBalloon::Entry` is 0x2C bytes: `mObject` at 0, `mText` at 8, `mPosition` at 0x14, `mDistanceScale` (`f32`, the squared distance multiplier) at 0x20, `mFlags` (`u32`, bit 0 cleared when the balloon leaves) at 0x24 and a `StageID`/`u32` union at 0x28 (all ones means a text balloon, otherwise a warp zone; `mWorld` selects the `gStageWorlds` title); names descriptive, medium. Outside the block, `CPlayer::GetController` `0x800859B0`, `CMotionProgram::IsEnded(bool)` `0x800859E0` (true for state 3, or 2 as well when the argument is set), `shared_ptr<CMotionProgram>` bool conversion `0x800859C0`, `basic_string<wchar_t>::c_str`/`data` `0x80085A40`, `0x80085A50`, `shared_ptr<CPiiPersonalData>::operator->` `0x8008BA40`, `BWorldObj::GetRotation` `0x8009A040`, `CPpuEngine::HasCallback(int) const` `0x80101C30`, `CPpuEngine::SendMessage(const Message&)` `0x80101CA0` (`Message` is sender, message id and one `NPpu::CValue`), `BUI::RemoveChild` `0x801C8F80`, `CLayout::UpdateAttachedLayouts` `0x801D3620` and `CLayout::DetachChildren` `0x801D57E0` are named so the links resolve (descriptive, low to medium).
## F-201-1: CSaveManager.cpp third block

[F-201-1](findings/201-csavemanager-3.md#f-201-1-the-27-functions-of-201-reconstructed) reconstructs `0x801090F0`-`0x80109754`. `CSaveManager::RequestWork(WorkType)` (`0x80109550`, medium) is a descriptive name for the helper behind `RequestSave`, `RequestSaveWithoutBackup`, `RequestSaveCommitOnly` and `RequestDelete`: it queues the work item while the fiber runs and starts it otherwise. `std::tr1::mem_fn` (`0x80109270`), `queue::push` and the `__deque_imp::append` helper (`0x801095F0`) are descriptive names; `push_back(const T&, int2type<false>)` at `0x8010A260` is named for the link and owned by #202. `tuple<T0, T1>` takes const references.
[F-61-2](findings/61-cdividedground-second-block.md#f-61-2-reconstruction-of-the-second-block) names `CDividedGround::Load(const StageID*, int, bool)` `0x80072340` (public, called once from `0x800A8BA4`), `LoadFileLists` `0x80072AE0`, `LoadAttr(const StageID*, int, const char*, bool)` `0x80073660` and `IsLayerFogName(const std::string&)` `0x80074000` (all descriptive; the last parameter of both `LoadAttr` overloads is the background-loading flag of `LoadResourceFile`), `CDividedGround::BuildScene` `0x80074100` (descriptive; body is #62), `BModelObj::ChangeModel` `0x80053520`, `CModel::CModel()` `0x801B7AA0`, `CModel::Load(const LoadParams&, bool)` `0x801B8BB0` (the five-word record is `LoadParams`: file-name array, flag, options `0xD15`, mode `1`, model name; all descriptive), `CModel::Play(int, f32, f32)` `0x801B9530` (reads `GetResAnmChr(int)`), `CBSPTree` (4 bytes) `CBSPTree()` `0x801E6700`, `~CBSPTree()` `0x801E6710` and `SetData(void*)` `0x801E6750`, and the byte `gFrameTimingReset` `0x804B9110` (low confidence). Weak template instances renamed to their mangled forms: the `basic_string` `c_str`, `operator+` pair and `basic_string(const char*)` `0x80076910`, the `vector<const char*>` `reserve`, `push_back`, `operator[]`, the `shared_ptr<FileList>` default constructor, `operator bool`, `operator->`, `operator=` and destructor, `list<shared_ptr<CFileData>>::insert` and its iterator `operator*`, `++` and `!=`, `ResFile::ResFile(void*)`, `CFileData::GetData`, `CModel::GetScnObj`, `shared_ptr<CModel>::shared_ptr(CModel*)` and `operator->`. `CModel` is 0xDC bytes (`mTail` covers the unmodelled 0xB8); `LoadResourceFileList` is the `0x80072DD0` wrapper.
## Game CGameEnding third block (#118)

[F-118-1](findings/118-cgameending-block.md#f-118-1-the-30-functions) renames the `IGameEnding` slots 43 to 51 after their bodies: `ShowCongratulations` `0x800A6D30`, `StopCongratulations` `0x800A6D20`, `ParseStaffRollText(const wchar_t*)` `0x800A6730`, `StartStaffRoll` `0x800A6650`, `HideStaffRoll` `0x800A65C0`, `IsStaffRollFinished` `0x800A6600`, `ShowNext` `0x800A64A0`, `StopNext` `0x800A6490`, `IsNextVisible` `0x800A6480` (descriptive, medium; this replaces the low-confidence guesses of F-117-5). Outside the block: `CLayout::StopMotion(Pane* = spDefaultMotionPane)` `0x801D13A0` and `CLayout::IsVisible() const` `0x801D07B0` (descriptive, medium), the one-argument `CLayout::AppendMotion` `0x8008E3B0` (high; not `QueueMotion`), and the weak instances `shared_ptr_deleter`, `shared_ptr`, `move_ptr` and `alloc_ptr` of `CGameEnding`, `BWorld`, `CShadowCamera`, `IPauseMenu` and the `list<wstring>` node holder at `0x800A6DF0`-`0x800A7610` (high for the type, from the factory extab).
## Game CGameResult first block (#123)

[F-123-2](findings/123-cgameresult-unit.md#f-123-2-class-hierarchy-and-layout) records the 0x98-byte layout of `@unnamed@CGameResult_cpp@::CGameResult` (offsets 0x0C map description, 0x10 `CUIManager`, 0x18 `BUILayout`, 0x20 `CUIPiiSpec`, 0x30 `IPauseMenu`, 0x38 and 0x48 `BWorld` and `BWorldCamera` pairs, 0x58 `vector<ResultPii>`, 0x64 window centre, 0x6C `CaptureTexture`), high confidence for the offsets and descriptive names for `ResultPii` (0x38 bytes: player at 0, positions at 4 and 16, personal data at 28, `CPii`s at 36 and 44, scale at 52). [F-123-3](findings/123-cgameresult-unit.md#f-123-3-the-26-functions-of-123) names `IGameResult::~IGameResult` `0x800AD0F0` (now `__dt__11IGameResultFv`, high), and gives the other 25 functions' identities without renaming them: the `shared_ptr<BWorld>` and `shared_ptr<BWorldCamera>` default constructors `0x800AD160` and `0x800AD170`, `vector<ResultPii>` members `0x800AD180`, `0x800AE160`, `0x800AE590`, `0x800AEDE0`, `0x800AEDF0`, `0x800AF480`, `ResultPii` `0x800AE450` and `0x800AE4B0`, the `map<const void*, unsigned long>` members `0x800AD800`, `0x800AD820`, `0x800AE0D0`, `vector<shared_ptr<CPiiPersonalData>>::push_back` `0x800ADAF0`, and the descriptive `SetPiiPositions` `0x800AED50` and `SyncPiis` `0x800AEE00`.
## Game CGameEnding factory (#116)

[F-116-5](findings/116-cgameending-unit.md#f-116-5-the-factory-is-reconstructed-and-the-unit-now-emits) names the factory `0x800A4EF0` `IGameEnding::Create(const MapDesc*, bool)` (`Create__11IGameEndingFPC7MapDescb`, descriptive name, medium confidence), the `CPauseMenu` factory `0x800D83B0` `IPauseMenu::Create(const MapDesc*, bool, int)`, the `CShadowCamera` constructor `0x8010C490` and `CShadowCamera::Setup(u16, u16, int, bool)` `0x8010C5A0`, and the template instances of the factory's cleanup actions: `list<wstring>` destructor `0x800A5A60`, `shared_ptr<CGameEnding>` `0x800A6F80`, `shared_ptr<IPauseMenu>` `0x800A7010`, `shared_ptr<BWorld>` `0x800A70A0`, `move_ptr<CGameEnding>` `0x800A7130`, `move_ptr<BWorld>` `0x800A71B0`, `move_ptr<CShadowCamera>` `0x800A7230` and the list node `alloc_ptr` `0x800A72B0`. Names come from the mangled symbols of the compiled source, high confidence.
## Game BWorld calc, view and draw (#47)

[F-47-1](findings/47-bworld-draw.md) names, in `.text`: `BWorld::CalcMtx` `0x80062980`, `FrameStepScope::~FrameStepScope` `0x80062C20`, `BWorld::CalcView` `0x80062C80`, `BWorld::Draw` `0x80062D20`, file-local `DrawFrame(int, int)` `0x800633D0` and `DrawTexture(int, int, const GXTexObj*)` `0x800636B0`, `BWorld::DrawShadow(GXColor, GXPixelFmt)` `0x800639F0`, `BWorld::DrawOutline` `0x8005E560` (callee), and the weak template instances `insert_node_at` `0x80063DE0` (`map<ulong, CEffect*>`) and `0x80064240` (`set<shared_ptr<BWorldObj>>`), `destroy` `0x80063FB0` and `0x80064420`, `find_leaf` `0x80065DC0` and `0x80065E20`, `tree_balance_after_insert` `0x80059AF0`, the `shared_ptr_deleter` `get_deleter`/`dispose` pairs for `CEffect` `0x80064980`/`0x800649D0`, `CKDTree<BModelObj>` `0x80064A70`/`0x80064AC0` and `ScnRoot` `0x80064B40`/`0x80064B90`, and the two `weak_ptr::lock` instances `0x80064EB0` (`CShadowCamera`) and `0x80064FE0` (`BWorldCamera`).

| Class | Offset | Member | Type | Evidence |
| --- | --- | --- | --- | --- |
| `BWorld` | `0x14` | `mPendingFrames` | `u32` | copied to `gFrameStep` by `CalcMtx` and `CalcView`; `CalcView` clears it |
| `BWorld` | `0x1C`, `0x24`, `0x2C` | `mScnRoot`, `mOutlineGroup`, `mModelGroup` | `shared_ptr` | rebuilt as the root's children by `CalcMtx` and each `Draw` pass |
| `BWorld` | `0x40`, `0x48` | `mCurrentCamera`, `mShadowCamera` | `weak_ptr` | `lock` calls in `Draw`, `DrawShadow` |
| `BWorld` | `0x80`, `0x88` | `mEffectData`, `mOutlineData` | `shared_ptr` | `Calc`, `Draw` effect and outline passes |
| `CShadowCamera` | `0x68` | `mTextureLevels` | `int` | level count in `DrawShadow` |
| `CShadowCamera` | `0x6C` + 44 * i | `mTextures[3]` | `CaptureTexture` | `DrawShadow` copies each level from the EFB |
| `CShadowCamera` | `0xF0`, `0xF1` | `mUseTextureAlpha`, `mAlpha` | `bool`, `u8` | `DrawShadow` colour pass |
| `BWorld::EffectData` | `0x04`, `0x08`, `0x60` | `mGroupID`, `mPauseGroupID`, `mCaptureGroupID` | `u32` | `Draw` calls `EffectSystem::Calc` and `Draw` per group, skipping `0xFFFFFFFF` |
| `tree_imp`, `map_compact_tree` | `0x00`, `0x04`, `0x08` | size, node allocator with root pointer, leftmost | `unsigned long`, `compressed_pair`, `tree_node_base*` | `insert_node_at` passes `this + 4` as the allocator |
# Issue 217 interface prerequisites

Original CPlayer RTTI and its 116-byte ICalc-derived layout, including two 40-byte states, are recorded in [F-217-1](findings/217-pii-ui.md#f-217-1-scope-and-native-player-contract). CPiiBox is 20 bytes with two integer constructor arguments and a list of shared CPiiPersonalData. Native constructors/destructors, allocation calls, six exact instruction streams and four exception records establish the complete representation. Existing nonconst GetController and const IsJoined APIs are preserved.

Original IIconCollection has four virtual texture-return methods, no virtual destructor, and the native concrete vtable at `0x803A4880`; CRemoteMemory is 64 bytes with its transfer/counter fields established by constructor and read/write producers. [F-217-2](findings/217-pii-ui.md#f-217-2-remote-memory-and-icon-contracts) records types, offsets, descriptive naming confidence, and the independent exact CUIPlayerID Update consumer. BUILayout::GetLayout returns its CLayout member at `+0x4C`, matching the native eight-byte accessor at `0x8008B7D0`. These are header contracts without source-selection credit; the full current-consumer control is [F-217-4](findings/217-pii-ui.md#f-217-4-current-consumer-prerequisite-control).

[F-181-2](findings/181-cplayer.md#f-181-2-classes) records `CPlayer` (0x74 bytes, `ICalc` base; flags 4, player number 8, `IController*` 0xC, `State` at 0x10 and 0x38, `weak_ptr<CPlayerPii>` 0x60, words 0x68, 0x6C accumulated hold and 0x70 current hold) and `CPlayer::State` (0x28 bytes: joined flag 0, `CPiiBox` 4, two `shared_ptr<CPiiPersonalData>` 0x18 and 0x20). `CPiiBox` is 0x14 bytes: type 0, capacity 4 (1200 for player boxes), `list<shared_ptr<CPiiPersonalData>>` 8. All member names are descriptive (low). The unit's 111 functions and four data symbols carry the mangled names its source produces in `config/wpse-v1/symbols.txt`.

## F-202-1: CSaveManager.cpp fourth block

[F-202-1](findings/202-csavemanager-4.md#f-202-1-the-32-functions-of-202-reconstructed) reconstructs `0x80109760`-`0x80109FC4`: `CSaveManager::Draw` `0x801097A0`, `ISaveManager::Create()` `0x80109820` (descriptive, low), the `CSaveManager` constructor `0x80109880` storing `sInstance` (`.sbss` `0x804B9070`, descriptive, medium), the `ISaveManager` constructor and destructor `0x801098E0`, `0x80109920`, the `queue<WorkType>` constructor chain `0x801099A0`-`0x80109B50` and destructor chain `0x80109B80`-`0x80109E20` through the descriptive classes `__deque_container` and `__deque_storage` (low), and `shared_ptr<ISaveManager>(CSaveManager*)` `0x80109E80` with its `move_ptr`, `compressed_pair`, `default_delete` and `shared_ptr_deleter` constructors. The vtables `0x803AD780` (`ISaveManager`), `0x803AD69C` (`CSaveManager`), `0x803AD2C8` (`shared_ptr_deleter`) and `0x803AD380` (`function_imp`) are named for the link, as are the `__cdeque_buf<void*>` constructor `0x800F8F70` and destructor `0x800F9070`.
| BUI::IsFocused | `0x801C8DE0`,88 bytes | Existing BUI layout/slots preserved; coordinated additive declaration; ordinary outside native producer88 bytes+1target exact; method spelling inferred; F-337-2 |

For `wpse-v1` / `main.dol`, [F-57-6](findings/57-controllers.md#f-57-6-complete-original-controller-inventory) identifies the 96-byte cleanup guard at `0x8005A9F0` as `std::tr1::detail::shared_ptr_holder<char, std::detail::default_delete<char[]> >::~shared_ptr_holder`, with native body and delete targets verified. Composition assignment EH uses this existing instance twice; the outside rename preserves its size and split.
## F-137-8: CGameStage and CGloballyReusableTexBuf

Anonymous-namespace `CGameStage : IGameStage`, size 0x30 (high confidence for class name via RTTI): `IGameStage::Param mParam` at 0xC, `MapIndex mMapIndex` at 0x18, `int mLife` at 0x1C (ctor 3), `shared_ptr<IGameMap> mMap` at 0x20, `int mState` at 0x28, `SoundHandle mSound` at 0x2C. Member and method spellings other than the class are descriptive. `IGameSession` virtual slots 25-35 (`GetOyaInfo`, `GetEnvironment`, `AddPPDToPlayerPiiBox`, `CanAddPPDToPlayerPiiBox`, `IsReplacePiiAvailable`, `GetTotalPlaySec`, `GetStageChangeParam`, `Fade`, `GetRemainingFadeFrames`, `BeginStage`, `EndStage`) have low-confidence descriptive names. `sWork` at `0x804B9008` is a `shared_ptr<Work>` holding a `vector<WorkBuffer>`; evidence in [F-137-1](findings/137-cgamestage-texbuf.md).
## F-96-1: CDlgPiiSelector.cpp CDataPerPlayer helper block

[F-96-1](findings/96-cdlgpiiselector-block.md#f-96-1-identities-of-the-25-functions) names `CPiiPersonalData::IsFainted() const` `0x800962F0` (`mHP <= 0`, medium), `IsSpecHidden() const` `0x80096750`, `HasFlag(u32) const` `0x80096760`, `SetNewlyDisclosed(bool)` `0x80096780`, `SetFlag(u32, bool)` `0x80096790`, `SetSpecHidden(bool)` `0x800967C0`, `IsFavorite() const` `0x800967D0` and, outside the block, `SetFavorite(bool)` `0x800ECA80`, with the flag enum of F-173-3 in `include/game/CPiiPersonalData.h`. It also names `CDataPerPlayer::OnSelectionChanged()` `0x80096510` (non-virtual, 0x240 bytes, descriptive name, low-medium), whose callees are `shared_ptr<CUIPiiList>::operator->` `0x80093F40`, `shared_ptr<IUIListItem>::operator bool` `0x80093F50`, `static_pointer_cast<CUIPiiListItem, IUIListItem>` `0x8008FE10`, `shared_ptr<CUIPiiListItem>::operator->` `0x8008BA60`, `CUIPiiListItem::GetPPD() const` `0x8008BA50` and the two `shared_ptr` destructors `0x8008F4B0` and `0x8007C440`. `CUIList` offsets: `mSelectionCallbacks` 0x174, `mAcceptCallbacks` 0x178, size 0x180. `CDataPerPlayer` facts: `IDlgPiiSelectorInternal*` at 0, player index at 8, favourite-mode `bool` at 0x10, silent `bool` at 0x51, `shared_ptr<CUIPiiList>` at 0xA4, vtable pointer at 0x448 because the virtual destructor is declared after the members.
## Game BWorld deleter and smart-pointer instances (#48)

[F-48-1](findings/48-bworld-weak-instances.md) names, in `.text`: the `shared_ptr_deleter` `get_deleter`/`dispose` pairs for `ScnGroup` `0x80064BB0`/`0x80064C00`, `BWorld::EffectData` `0x80064C20`/`0x80064C70` and `BWorld::OutlineData` `0x80064D50`/`0x80064DA0`; the `weak_ptr` destructors `CEffect` `0x80064DF0`, `CShadowCamera` `0x80064E50`, `BWorldCamera` `0x80064F80` and `BWorld` `0x800650B0` (the first and last by emission order, medium confidence); and the `shared_ptr` destructors `AnmScnRes` `0x80065240`, `CEffect` `0x80065360`, `OutlineData` `0x800653F0`, `EffectData` `0x80065480`, `CKDTree<BModelObj>` `0x80065510`, `CShadowCamera` `0x800655A0` and `ScnRoot` `0x80065750`. All are compiler-mangled names of standard template instances.


## F-93-5: CDlgPiiSelector Draw, Calc and CDataPerPlayer members

[F-93-5](findings/93-cdlgpiiselector-calc-block.md#f-93-5-draw-calc-and-the-cdataperplayer-members) completes the 27 functions of `0x80093540`-`0x80093F64` at 100%. `CDataPerPlayer` (0x450 bytes, vtable pointer at 0x448): `IDlgPiiSelectorInternal* mpDlg` 0x0, `int mState` 0x4, `int mPlayer` 0x8, `bool mFavoriteMode` 0x10 (name from F-96-2), `vector<shared_ptr<CUIPiiListItem> > mItems` 0x14, `CPiiListFilterSort mFilterSort` 0x20, `bool mSilent` 0x51, `shared_ptr<CUIManager> mUIManager` 0x84, `shared_ptr<CUIPiiSpec> mPiiSpec` 0x9C, `shared_ptr<CUIPiiList> mPiiList` 0xA4, `shared_ptr<CUIPlayerID> mPlayerID` 0xAC, `IController* mpController` 0xC4, `u32 mDisconnectSerial` 0xC8, `bool mDisconnected` 0xCC, `CFiber mFiber` 0xD8. Offsets and types are high confidence, names medium. The `Reserved` members (0xC, 0x40, 0x48, 0x50, 0x52, 0x54 to 0x80, 0x8C, 0x94, 0xB4, 0xBC, 0xD0) have established offsets and unestablished purpose. `CUIPiiListItem` (0x1C bytes: flags 0x4, `shared_ptr<CPiiPersonalData> mPPD` 0x8, player 0x10, type masks 0x14 and 0x18), `CUIPiiList` (`CUIList` subclass), `CUIPiiSpec` (`BUILayout` subclass, `SetPii` slot 13) and `CUIPlayerID` (`BUILayout` subclass, two words at 0x134 and 0x138, `Update`) have minimal headers in `include/game/`. `CDlgPiiSelector::UpdateEntrance` is a descriptive, low-confidence name for `0x80094240`. Outside functions named in `config/wpse-v1/symbols.txt` for the link: `CUIPlayerID::Update` `0x8011F9C0`, `CUIPiiListItem::GetPPD` `0x8008BA50`, `shared_ptr<CUIPiiListItem>::operator->` `0x8008BA60`, `static_pointer_cast<CUIPiiListItem, IUIListItem>` `0x8008FE10`, the `shared_ptr<CUIPiiListItem>` and `shared_ptr<IUIListItem>` destructors `0x8008F4B0` and `0x8007C440`, and `shared_ptr<CDataPerPlayer>::reset` and `swap` `0x8009BD80` and `0x8009DBF0`.
## F-161: CNoticeDisplay.cpp second block

[F-161-1, F-161-2](findings/161-cnoticedisplay-second-block.md) name, in `.text`: `CSimpleText::Run()` `0x800D1030`, `CFight::Run()` `0x800D15D0` and `CLose::Run()` `0x800D19F0` (descriptive, low: the fiber bodies bound by `Init`), `CLayout::AppendMotion(const char*)` `0x8008E3B0` (weak default-pane overload, high), `CBattleRoyalData::UpdateBestTime(f32)` `0x8006A550` (high: it keeps a running minimum of the 4-byte float), `CBattleRoyalBox::GetData(int, int)` `0x8006A820` (medium: copies `shared_ptr` entry `[mode][area]`, stride 0x28 and 8), `IsBattleRoyalFirstPart(MapIndex)` `0x801251C0` (low: true for map numbers below 3) and `NIntl::basic_formatter<wchar_t>::operator%(const int&)` `0x801F34E0` (spelling `__md__...FRCi`; #167 had `Fi`) (medium: formats the value with the stored specification and appends it; five sibling instances at `0x801F26E0`, `0x801F2DE0`, `0x801F3BF0`, `0x801F4300`, `0x801F4A10`). Layouts: `basic_formatter<wchar_t>` 0x34 bytes (pattern `+0x00`, argument vector `+0x0C`, specification `+0x18`, output `+0x24`, formatted flag `+0x30`). `IGameSession` slot 10 is `GetBattleRoyalBox`. `SlotData::mData.mStatus[19]` (`0x12FB` from the object) is read as a signed area number in battle royal mode 2 (low).
## Game CMsgBalloon warp block (#146)

[F-146-1](findings/146-cmsgballoon-warp.md#f-146-1-the-32-functions-of-0x800c7ef0-0x800c88c8) names `CMsgBalloon::SetupBattleWarp(StageID)` `0x800C8030` and `SetupStageWarp(StageID)` `0x800C8760` (descriptive, medium) and the declared-only `SetupStageModels(StageID)` `0x800C88D0` (owned by the next block). `StageID` and `MapIndex` are the same 32-bit value; the record lookup reads it as a `MapIndex`. `NIntl::basic_formatter<wchar_t>` (original name, from the type-name strings at `0x803A5160`-`0x803A51D0`) is 0x34 bytes: `mFormat` `0x0`, `mArgs` (`vector<wstring>`) `0xC`, `mSpec` `0x18`, `mResult` `0x24`, `mParsed` `0x30`; `format(const wchar_t*)` `0x800C8420` (descriptive), `str()` `0x800C8460` and `parse()` `0x800C84A0` are the copies in this unit, and the constructor `0x80089330`, destructor `0x800893F0`, `operator%(const int&)` `0x801F34E0` and `c_str()` `0x8008D4E0` are defined elsewhere. `basic_string<wchar_t>::operator[]` `0x800C8650`, its private `buffer` `0x800C8690`, `operator+=(const basic_string&)` `0x800C8730`, `vector<wstring>::size`/`operator[]` `0x800C8720`/`0x800C8740` with the `__vec_constructor` layer `0x800C8750`, `isdigit(int)` `0x800C86E0`, `ceil(float)` `0x800C83E0` and `ceilf` `0x800C83F0`, and the `list<Entry>::generic_iterator<false>` functions `0x800C7F40`-`0x800C8000` are named by their compiler names. `FormatMessage` (`0x800CB570` two arguments, `0x800CB5F0` one, named by #167; descriptive, medium) wraps the formatter and returns a copy of `str()`. `CBattleRoyalBox::GetData(int, int)` `0x8006A820` (named by #161) returns a `shared_ptr<CBattleRoyalData>` whose only member `mBestTime` (`f32`, seconds) is read by `GetBestTime` `0x800C83C0`; `IsGameFlagSet(int)` `0x80165BC0` is the native of the `CheckFlag` script binding (descriptive, medium). `SlotData::mData.mStatus[18]` is the signed byte read at slot offset `0x12FA`.
## Game CGameSession first block (#126)

For `wpse-v1` / `main.dol`, [F-126-2](findings/126-cgamesession.md#f-126-2-cgamesession-layout) records the layout of `@unnamed@CGameSession_cpp@::CGameSession` (size `0xC78`, vtable `0x803A395C`, instance pointer `.sbss` `0x804B9000`) and [F-126-4](findings/126-cgamesession.md#f-126-4-igamesession-slots-25-to-42) ties the `IGameSession` slots 25 to 42 to this block. `.text` names are the mangled spellings the source produces: `CGameSession::CGameSession(int)` `0x800B2040`, `MakeMapIndex(int, int, int, int)` `0x800B2370`, `ISession::~ISession` `0x800B2390`, `IGameSession::~IGameSession` `0x800B2400`, `StageDesc::StageDesc` `0x800B2490`, `StageDesc::~StageDesc` `0x800B25B0`, `~CGameSession` `0x800B26B0`, `InitStageDescs` `0x800B28F0`, `GetSnapshot` `0x800B29A0` (descriptive), `SaveToSlot` `0x800B2A50` (descriptive), `LoadFromSlot` `0x800B5670` (descriptive), and the `SD_*` constructors and `vector<SD_*>` members from `0x800B2FE0` to `0x800B5650`. Callees renamed for the link: `GetPersonalData` `0x80089650` and `GetPendingPersonalData` `0x800896B0` (`CPlayer`, inline accessors of `mState.mPersonalData` and `mPendingPersonalData`), `CRemoteMemory::CRemoteMemory(int, COyaInfo*)` `0x80106770`, `CRemoteMemory::~CRemoteMemory` `0x80106880`, `CRemoteMemory::Update` `0x801069A0`, `COyaInfo` constructor `0x800ED1B0` and destructor `0x800EE2B0`, `WazaDescData` `0x8010DDE0`/`0x8010DE20`, `PiiPropData` `0x800F2630`/`0x800F2B30`, `DescParamTable` `0x801119B0`/`0x801119C0`, `StageDesc::Init(MapIndex)` `0x80125230`, `CPiiBox::ExportPersonalData` `0x800E0780`, `CPiiCollectionBox::ExportCollectionData` `0x800E1CE0`, `CBattleRoyalData::ExportBattleRoyalData` `0x8006A860`, `CStatistic::ExportStatisticValues` `0x80116260` and `CPiiPersonalData::GetSpeciesData` `0x8008E7F0` and `PiiSpeciesData::GetSpecies` `0x80124F10`, all descriptive; `CPiiPersonalData::GetCreationTime` `0x800B30D0` returns the 64-bit value at `+0x40`. The vtables `__vt__` of `CGameSession` (`0x803A395C`), `SD_PiiCollectionData` (`0x803A3A68`), `SD_StatisticValue` (`0x803A3AB8`), `SD_BattleRoyalData` (`0x803A3B00`), `SD_PiiPersonalData` (`0x803A3B48`) and the `CPlayer` and `CRemoteMemory` deleters (`0x803A3864`, `0x803A38D8`) are named. Member names of `CGameSession` are low confidence.

## F-207-6: vector<void*> split-buffer chain names

[F-207-6](findings/207-cshadowcamera.md#f-207-6-the-vectorvoid-half-of-the-csavemanager-template-run) renames the `vector<void*>` reallocation functions of the CSaveManager unit (`0x8010B8A0`, `0x8010B8D0`, `0x8010BE60`, `0x8010BF40`, `0x8010C050`-`0x8010C480`) and their weak callees (`0x800887B0`, `0x800FBE40`-`0x800FD4E0`, `0x80066040`) to the mangled names of the reconstructed `include/vector` templates. Names are template-instance names, so confidence is high where the instruction bodies match and the roles are descriptive.
| `IGameSession` | slot 26 | `GetEnvironment` | virtual | returns `shared_ptr<NPpu::CEnvironment>` by value from `CGameSession+0x84`; high ([F-116-6](findings/116-cgameending-unit.md#f-116-6-runaction-calc-and-the-matching-trial)) |

## CStrapReminder and CTextWorldObjStat (#211)

[F-211-2](findings/211-strap-reminder-text-stat.md#f-211-2-cstrapreminder) and [F-211-3](findings/211-strap-reminder-text-stat.md#f-211-3-ctextworldobjstat) record the layouts: `CStrapReminder` is 0x468 bytes (`CLayout` at 0x04, `CFiber` at 0xF0, `mCloseQueued` at 0x460) and `CTextWorldObjStat` is 0x108 bytes (flags 0x04, type 0x08, position 0x0C, `CLayout` 0x18, motion 0x100). Member and method names are descriptive (medium confidence); class names are RTTI spellings.
[F-203-1](findings/203-csavemanager-5.md#f-203-1-the-24-functions-of-203-reconstructed) reconstructs `0x80109FD0`-`0x8010A7D4`: `CSaveManager::GetInstance` `0x8010A0A0` (descriptive, low; unreferenced, kept by `force_active`), the real `~__deque_imp` `0x8010A150`, `__deque_imp::alloc` `0x8010A250`, `back_capacity` `0x8010A2F0`, `allocate_back` `0x8010A330` and `pop_front(n, int2type<false>)` `0x8010A5D0`, `destroy_all` and `destroy_all(pod_type)` `0x8010A6A0`/`0x8010A6D0`, `__deque_imp::iterator::operator*` `0x8010A6E0`, and the `__cdeque<T*>`/`__cdeque_pointer<T>` forwarding layers `0x8010A6F0`-`0x8010A7D0` (all descriptive, low to medium). The `__deque_imp` members are `start_` at 0, `size_` at 4 and `compressed_pair<__cdeque<T*>, Allocator>` at 8; the block pointer ring buffer ends in a null sentinel. Named for the link outside the block: the `__cdeque_buf<void*>` members `0x800FA400`-`0x800FC0E0` and its iterator `0x800FAAE0`-`0x800FB870`, `std::max<unsigned long>` `0x800887B0`, `__throw_deque_length_error` `0x800FA240`, `allocator::allocate`/`deallocate`/`construct` `0x8010ABA0`-`0x8010ABC0`, the pointer iterator `0x8010A7E0`-`0x8010A850`, `insert` `0x8010AC50`, `second` `0x8010AC40`, and `front_capacity`, `max_size`, the spare constructor, `capacity`, `__deque_imp::end` and the iterator `operator!=` at `0x8010B960`, `0x8010B970`, `0x8010B9C0`, `0x8010BBD0`, `0x8010BC50`, `0x8010BC60`.
### CPiiListFilterSort helpers and CUIPiiListItem (#174)

[F-174-1](findings/174-cpiilistfiltersort-menus.md#f-174-1-reconstructed-functions) names `HasPrefixedItem` `0x800E7A50`, `HasItemWithPrefix` `0x800E7AA0` and `IsNameBefore` `0x800E9300` (free functions, descriptive names, medium confidence, 100% in objdiff). The scans read `CUIPiiListItem::GetPPD` (declared by #93); the constructor `0x8011D7E0` belongs to #217.
## Game CGameMap first block (#119)

[F-119-2](findings/119-cgamemap.md#f-119-2-unnamedcgamemap_cppcgamemap-layout) gives the layout of `@unnamed@CGameMap_cpp@::CGameMap` (0xD8 bytes, base `IGameMap`): `IGameMap::Param mDesc` at 0xC (`MapParam::mMapDesc`, `shared_ptr<NPpu::CEnvironment> mEnvironment` at 0x10), `bool mPausing` 0x18, `weak_ptr<IDlg> mCurrentDialog` 0x1C, `int mTimerFrame` 0x24, `int mTimerStep` 0x28, `int mHitstopFrames` 0x2C, `bool mModelObjsHidden` 0x30, `vector<u32> mHiddenObjIDs` 0x34, then 19 `shared_ptr` members from 0x40 to 0xD0 in the order `mWorld`, `mPostEffect`, `mWorldObjStatDisplay`, `mMiniMap`, `mDlgPiiSelector`, `mDlgPiiList`, `mDlgCollection`, `mDlgInfo`, `mMoneyLifeDisplay`, `mBattleRoyalDisplay`, `mComboDisplay`, `mNoticeDisplay`, `mLayoutManager`, `mDlgMsgWin`, `mPauseMenu`, `mTutorialMsg`, `mMsgBalloon`, `mEnemyInfo`, `mNumericInput`. The vtable getters fix each pairing (high confidence); member names are descriptive. `MapDesc` (0x38 bytes) gains `CPpuRegister mParams` at 4 (`GetValue(unsigned)` `0x80111D20`); the remaining fields are unrecovered. Descriptive names with medium confidence: `GetStageNumber` `0x800A85B0`, `Initialize` `0x800A8680`, `LoadStageResources` `0x800A8FF0`, `IsBattleRoyalFirstPart` `0x801251C0`, `CPlayer::BeginHoldCollection` `0x800F8540`, `CPlayer::EndHoldCollection` `0x800F85A0`, `CShadowCamera::Setup` `0x8010C5A0`, `CDividedGround::Load` `0x80072340`, and the interface creators `IPostEffect::Create` `0x800FD500`, `IMiniMap::Create` `0x800C26D0`, `IMoneyLifeDisplay::Create` `0x800C4960`, `IBattleRoyalDisplay::Create` `0x8006B470`, `ILayoutManager::Create` `0x800C1220`, `IWorldObjStatDisplay::Create` `0x80121340`, `ITutorialMsg::Create` `0x8011AB90`, `INumericInput::Create` `0x800D6790`, `IPauseMenu::Create` `0x800D83B0`, `IDlgCollection::Create` `0x800776B0`, `IDlgInfo::Create` `0x8007E5D0`. `IDlgPiiList::Create` takes a bool (`Create__11IDlgPiiListFb`).
[F-184-2](findings/184-cplayerpii.md#f-184-2-cplayerpii) records `CPlayerPii` (0x168 bytes, `CPii` base; `CPlayer*` at 0x14C, `deque<VEC3>` position history at 0x150) and its virtuals `Calc` `0x800F9AB0`, `Initialize` `0x800F9380`, `GetShadowName` `0x800F9BC0`, `IsVisible` `0x800F9C20`, plus `LoadScript` `0x800F9A20` and the empty `SetEnvironment` `0x800F9A90` (descriptive, low); `GetActiveGameMap` `0x800AA2F0` (low), `CPlayer::GetPlayerNo` `0x800F9510`, `BWorldObj::IsVisible` `0x800C7F90` and `GetStateFlags` `0x800C7FC0`. [F-184-3](findings/184-cplayerpii.md#f-184-3-the-msl-deque-constructor-and-destructor-chains) names the MSL `deque<VEC3>` constructor and destructor chains `0x800F8DF0–0x800F9310`, the `__cdeque_buf<void*>` destructor `0x800FA370` and the `__deque_deleter` destructor `0x800F9C80` (medium, class chain from the PS2 layout and the extab records).
[F-119-2](findings/119-cgamemap.md#f-119-2-unnamedcgamemap_cppcgamemap-layout) gives the layout of `@unnamed@CGameMap_cpp@::CGameMap` (0xD8 bytes, base `IGameMap`): `IGameMap::Param mDesc` at 0xC (`MapParam::mMapDesc`, `shared_ptr<NPpu::CEnvironment> mEnvironment` at 0x10), `bool mPausing` 0x18, `weak_ptr<IDlg> mCurrentDialog` 0x1C, `int mTimerFrame` 0x24, `int mTimerStep` 0x28, `int mHitstopFrames` 0x2C, `bool mModelObjsHidden` 0x30, `vector<u32> mHiddenObjIDs` 0x34, then 19 `shared_ptr` members from 0x40 to 0xD0 in the order `mWorld`, `mPostEffect`, `mWorldObjStatDisplay`, `mMiniMap`, `mDlgPiiSelector`, `mDlgPiiList`, `mDlgCollection`, `mDlgInfo`, `mMoneyLifeDisplay`, `mBattleRoyalDisplay`, `mComboDisplay`, `mNoticeDisplay`, `mLayoutManager`, `mDlgMsgWin`, `mPauseMenu`, `mTutorialMsg`, `mMsgBalloon`, `mEnemyInfo`, `mNumericInput`. The vtable getters fix each pairing (high confidence); member names are descriptive. `MapDesc` (0x38 bytes) gains `CPpuRegister mParams` at 4 (`GetValue(unsigned)` `0x80111D20`); the remaining fields are unrecovered. Descriptive names with medium confidence: `GetStageNumber` `0x800A85B0`, `Initialize` `0x800A8680`, `LoadStageResources` `0x800A8FF0`, `IsBattleRoyalFirstPart` `0x801251C0`, `CPlayer::BeginHoldCollection` `0x800F8540`, `CPlayer::EndHoldCollection` `0x800F85A0`, `CShadowCamera::Setup` `0x8010C5A0`, `CDividedGround::Load` `0x80072340`, and the interface creators `IPostEffect::Create` `0x800FD500`, `IMiniMap::Create` `0x800C26D0`, `IMoneyLifeDisplay::Create` `0x800C4960`, `IBattleRoyalDisplay::Create` `0x8006B470`, `ILayoutManager::Create` `0x800C1220`, `IWorldObjStatDisplay::Create` `0x80121340`, `ITutorialMsg::Create` `0x8011AB90`, `INumericInput::Create` `0x800D6790`, `IPauseMenu::Create` `0x800D83B0`, `IDlgCollection::Create` `0x800776B0`, `IDlgInfo::Create` `0x8007E5D0`. `IDlgPiiList::Create` takes a bool (`Create__11IDlgPiiListFb`). `SlotData::Data::mFlags` is `s8[4]` and `mLastArea[2]` (`s8`, after `mLeadPiiMonsno`, last area for mode 0 and 1) fills the former padding; `BWorld::LoadLightScene(const char*, const char*, bool)` `0x8005EB70` and `CFileData::GetPath()` are descriptive.
## Game CMsgBalloon stage models and std::map layers (#147)

[F-147-1 to F-147-3](findings/147-cmsgballoon-stage-models.md) name `CMsgBalloon::SetupStageModels(StageID)` `0x800C88D0` (descriptive, medium) and the `std::map<unsigned long, bool>` chain `0x800C8FE0`-`0x800C9420`: the `map`, `detail::map_base`, `detail::tree`, `detail::red_black_tree` and `detail::tree_imp` constructors and destructors, the `compressed_pair` and `CompressedPairStorage` constructors of `tree_imp`'s size and root members, `tree_root_ptr` `0x800C9210`, `tree_begin_holder` `0x800C9220`, `begin_node` `0x800C9260`, `end_node` `0x800C9280` and `map_base::iterator::operator==` `0x800C9420` (names from the compiled source; layout high, spelling medium). Outside names for the link: `GetPiiSpeciesData(u16, int)` `0x80124EE0`, `CreatePiiPersonalData(const COyaInfo*, const PiiSpeciesData*, u32)` `0x800EE800`, `CPiiCollectionBox::GetCollectionType(u16)` `0x800E1C10`, `MapPiiTable::GetCell(int, int)` `0x80110100` and `FormatMessage<int, unsigned long>` `0x800CB4F0` (descriptive, medium). `MapDesc` is `MapIndex` at 0, two unknown words and the 0x2C-byte `MapPiiTable` at `0xC` (cells 0, row count `0x24`, column count `0x28`; low confidence for the field names). `std::map` is 12 bytes with an empty compare class and 16 bytes with a non-empty one (`CPpuEngine::mCallbacks`).


## BWorldObj parent and register ownership (#52)

| Address / offset | Evidence | Confidence |
| --- | --- | --- |
| BWorldObj size 0x48; weak parent 0x1C/0x20; CPpuRegister 0x24/0x28 | Complete constructor/destructor, all forwarders, 72-byte native vtable; F-52-2 | Layout/type proven; field names inferred |
| CPpuRegister Get 0x80111D20 / Set 0x80111D80 / Swap 0x80112590 | Complete native forwarding/caller traces and Slice-vector control RTTI | Signatures proven; method/class names descriptive |
| BWorldObj SetParent 0x8006A290 / GetParent 0x8006A2D0 | Exact60/160-byte producers; by-value parent and nonconst getter contexts | Names inferred; native behavior proven |
| BWorldObj SetPosition 0x80069E60 | BWorldObj and derived vtable targets; exact28-byte common body retained in preceding source | Base ownership proven; replaces derived override interpretation |
## Tutorial layout accessors (#213 prerequisite)

| Address | Declaration | Evidence |
| --- | --- | --- |
| `0x801D3830` | `void CLayout::CalculateMtx(bool)` | Native tutorial loader and complete calculation caller; descriptive method name. |
| `0x801CB0E0` | `f32 CLayout::CMotionProgram::GetFrame() const` | Native insertion caller consumes the frame as a float when replacing a motion. |
| `0x8011AF20` | `const nw4r::math::VEC3& nw4r::lyt::Pane::GetTranslate() const` | Exact eight-byte native member accessor; returns pane translation at `+0x2C`. |
| `0x802A4E80` | `nw4r::ut::Rect nw4r::lyt::Pane::GetPaneRect() const` | Producer reads pane size/origin; tutorial and numeric-input callers pass no DrawInfo. Corrects the imported extra parameter. |

See [F-213-1](findings/213-tutorial-message.md) for native caller proofs and current-consumer controls.
## Game ScriptItem.cpp (#245)

[F-245-1 to F-245-3](findings/245-script-item.md) split `game/ScriptItem.cpp` (`.text` `0x80140C40`-`0x801420E4`) and name, in `.text`: the file-local `AddItem(NPpu::CEngine*, shared_ptr<CItem>)` `0x80140C40` (descriptive, medium), the bindings `NewItem` `0x80140EC0`, `NewWarpZone` `0x80141050`, `NewFacility` `0x801411E0`, `method_CItem_loadModel` `0x80141370`, `method_loadModel` `0x80141440`, `IsPasswordUsed` `0x80141530`, `SetPasswordUsed` `0x80141620`, `IsSamePassword` `0x80141BC0` (original spelling from the binding table, mangled as `FPQ24NPpu7CEngine`), and the weak `shared_ptr_deleter`, `move_ptr` and `__vec_split_buffer<UsedPassword>` functions `0x80141DF0`-`0x80142070` (names from the compiled source). Outside: `method_CSimpleGround_loadModel` `0x801409A0`, `BWorldObj::SetParent` `0x8006A290` and `SwapRegister` `0x8006A4A0` (names from #45), `CItem::LoadScript(const char*, int, const shared_ptr<NPpu::CEnvironment>&)` `0x800C0D70`, `CItem::LoadModel(const char*, bool)` `0x800C0EC0`, `CPpuEngine::PeekWorldObjType(int)` `0x80101240` (descriptive, medium) and the `CWarpZone(u32)` constructor `0x80120000` (medium; the class name comes from `NewWarpZone` and the vtable at `0x803AF948`). Data: the `shared_ptr_deleter` vtables of `CItem` `0x803A01CC`, `CFacility` `0x803B67F4` and `CWarpZone` `0x803B686C`, the `UsedPassword` vtable `0x803B68E8`, and their type-info records `0x804B6540`, `0x804B79E8`-`0x804B7A08` (high). `CWarpZone` is 0x110 bytes: `CItem` and three words at 0x104 named `mDestination` (low). `UsedPassword::Data::mPassword` is `int`.
