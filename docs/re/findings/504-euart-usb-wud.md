# Issue 504: EUART, USB and WUD

## F-504-1: EUART unit and source-linked identity

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. The block begins with thirteen KPAD functions reconstructed in #710 under #503's ownership of KPAD.c. The next translation unit is `RVL_SDK/euart/euart.c`.

| Section | Start | Exclusive end |
| --- | --- | --- |
| `.text` | `0x803086D0` | `0x80308A50` |
| `.sbss` | `0x804B9B08` | `0x804B9B18` |

| Function | Address | Bytes |
| --- | --- | --- |
| `EUARTInit` | `0x803086D0` | `0xFC` |
| `InitializeUART` | `0x803087D0` | `0x48` |
| `WriteUARTN` | `0x80308820` | `0x230` |

The three functions total 884 function bytes. Text occupies 896 bytes including inter-function padding. The four four-byte uninitialized objects, in address order, are `__EUARTInitialized`, `__EUARTLastErrorCode`, `__EUARTSendStop` and `__EUARTEnabled`. The first three names follow the reference; the enabled-state name is descriptive, avoiding ambiguity with EXIUart.c's separate `Enabled` object.

Initialization checks the console type and writes two control bytes through `EXIWriteReg` under an interrupt guard. The UART initializer enables development-console output. The writer converts newlines to carriage returns in the caller's buffer, waits for the complete 32-byte hardware queue to become available, transmits bytes through EXI, and releases its lock. `QueueLength` inlines and is absent as a retained target function.

The writer loads the external four-byte `__EXIFreq` object at `0x804BB288`. Its name is established from corresponding reference calls and target SDA2 accesses; its original address, extent and storage type remain unchanged. It stays outside this unit's split.

The imported EUART header pointed to missing `export/UART.h`. It now includes the existing `MSL_C/UART.h`, whose baud-rate and error ABI declarations are already used by the repository, plus SDK types. `EUARTInit` is declared alongside the existing UART declarations. No duplicate ABI header is created.

The split-only full build and frozen verification pass before source registration. The compiled source reproduces every non-relocation instruction word at the original sizes with Wii/1.0 and `cflags_sdk`. Both text and sbss compare at 100% in objdiff. After selecting the source as `Matching`, full Ninja exits 0 and explicit frozen verification passes with DOL SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. There is no EUART original-object fallback. Verification covers the main game executable only.

## Remaining issue scope

USB runs from `0x80308A50` through `0x80309F9C`. The following WUD unit begins with `WUDIsLinkedWBC` at `0x80309FA0`, contains the allocation wrappers and setup/flush routines in this issue, and continues into later block issues. USB is now source-linked and accepted under F-504-3. WUD remains an original-object dependency; its whole-unit split and initial source are recorded in [F-504-4](504-wud-reconstruction.md). The KPAD portion depends on #710. This EUART change alone does not complete #504.

USB scratch comparisons establish exact raw instruction matches for the older reference's logging, library open/close, cleanup callback, device open and asynchronous close. The target also contains newer long-bulk handling. A licensed newer reference supplies that path, and the resulting partial source is now incorporated under F-504-2; the subsequent data reconstruction and source-linked acceptance are recorded in F-504-3.

## Provenance

EUART source follows [Petari at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/euart/euart.c), under the project's CC0 license. The file contains no separate license notice. Comments are stripped. The external frequency declaration and unique enabled-state name express target-proven accesses without importing another EXI header tree.

## Traps

- **EXIUart and EUART keep separate enabled flags.** A symbol named `Enabled` already exists at `0x804B959C` for EXIUart.c. EUART's corresponding object is at `0x804B9B14`; keep it distinct as `__EUARTEnabled`. Reusing one global metadata name would confuse the units despite identical source-local reference spelling.
- **A missing compatibility include does not require new ABI definitions.** EUART's imported `export/UART.h` is absent, but `MSL_C/UART.h` already defines the same UART interface. Reusing that existing header compiles the real unit and preserves exact code.

## F-504-2: USB translation unit and earlier pooled-string residual

| Section | Start | Exclusive end |
| --- | --- | --- |
| `.text` | `0x80308A50` | `0x80309F9C` |
| `.data` | `0x803DE888` | `0x803DF0B8` |
| `.sdata` | `0x804B8E78` | `0x804B8E80` |
| `.sbss` | `0x804B9B18` | `0x804B9B24` |

The 14 retained functions total 5,352 function bytes. They are `USB_LOG`, `USB_ERR`, `IUSB_OpenLib`, `IUSB_CloseLib`, `_intrBlkCtrlIsoCb`, `IUSB_OpenDeviceIds`, `IUSB_CloseDeviceAsync`, `__LongBlkMsgInt`, `__IntrBlkMsgInt`, `IUSB_ReadIntrMsgAsync`, `IUSB_ReadBlkMsgAsync`, `IUSB_WriteBlkMsgAsync`, `__CtrlMsgInt` and `IUSB_WriteCtrlMsgAsync`. Private names follow the SDK references; original-name confidence is medium, while behavior and addresses follow the target. The logging routines begin the unit; the next function is WUD's `WUDIsLinkedWBC` at `0x80309FA0`.

`sUsbHeapId` names the four-byte initialized heap ID at `0x804B8E78`, avoiding the separate ISFS unit's local `hId`. The error-log byte `s_usb_err` is `0x804B8E7C`. Uninitialized arena pointers `lo` and `hi` are `0x804B9B18` and `0x804B9B1C`; logging byte `s_usb_log` is `0x804B9B20`. Its former eight-byte coarse label included padding and is corrected to one byte. The earlier coarse data object `usbMessagePool` described the 0x830-byte compiler pool. F-504-3 replaces that coarse record with verified individual literal objects; no binary byte array is introduced.

The private `iusb_ctxt` allocation is 0x80 bytes, with callbacks at 0/4, application callback argument at 8, transfer argument at 0xC, spare pointer at 0x10, eight cleanup pointers at 0x14, cleanup count at 0x34, and a 32-byte-aligned 64-byte union at 0x40. The union holds a device path, descriptor scratch, or a transfer buffer pointer and 16-bit length. The long-bulk helper adds a 32-bit length vector and ioctl command 10; the ordinary helper uses a 16-bit length vector. Asynchronous failure -22 also produces the target's `IoctlvAsync returned error` report.

All fourteen bodies have their original sizes with Wii/1.0 and `cflags_sdk`. Thirteen reproduce every non-relocation instruction word. The control helper has exactly five differing non-relocation words, all string offsets from the pooled-data base: function offsets 0x5C, 0x214, 0x33C, 0x36C and 0x3D0. Each target offset exceeds the source offset by 0x70. These folded offsets remain real differences; they are not masked as relocations.

The compiled string pool lacks exactly five strings found in the target, in two locations. Before the control-transfer messages are `calling short blk transfer fn: buflen = %u limit = %u\n` and its `long` counterpart. At the end are `IUSB_RegisterInsertionNotifyWithIdAsync`, `%s: Not enough memory\n` and `Failed to open %s: %d\n`. These account for the data-layout residual. The current source data section is 0x761 bytes; target data is 0x830. A complete source solution must preserve these strings without inventing unproven API bodies or dummy calls.

Before F-504-3, objdiff was text 99.723465%, data 94.80552%, sdata and sbss 100%. The split-only full build and frozen verification pass before registration; the partial-source build also passes with USB explicitly `NonMatching`. The final executable still links the original USB object, so these 14 bodies add zero linked USB source-byte credit. EUART remains `Matching` and source-linked. WUD reconstruction now begins in [F-504-4](504-wud-reconstruction.md); its original-object dependency remains explicit.

The new long-bulk and updated interrupt/bulk/control bodies derive from [Whippy at ef8403b7150210639ad3a0683cd3f86e6d03dee5](https://github.com/Etrainquandt17/Whippy/blob/ef8403b7150210639ad3a0683cd3f86e6d03dee5/src/RVL_SDK/usb/usb.c), under its root CC0 license with no file-level exception found. Only actual retained function bodies are adapted, using the already established Petari context type and project headers. Empty unused functions and `DECOMP_FORCEACTIVE` constructs from that reference are excluded. The remaining USB source derives from the pinned CC0 [Petari USB implementation](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/usb/usb.c); unretained SDK reference bodies are not counted as target functions.

### Additional traps

- **A pooled address offset is still code evidence.** The control helper's five remaining words differ because two missing messages shift later string offsets by 0x70. Renaming callees or masking relocation words cannot fix those folded offsets. Preserve their diagnosis separately from the thirteen raw instruction matches.
- **Reference preservation macros may create dummy functions.** Whippy's `DECOMP_FORCEACTIVE` expands to a generated function calling `fake_function`. The project requires plausible source, so that construct and the empty unused API stubs are not imported. MH3's USB reference explicitly labels some stripped bodies as guesses; those guesses are not used to fill the target's string pool.
- **dtk auto splits require aligned edges for these data sections.** Ending the pooled-string range at its last terminator, `0x803DF0B7`, made the next automatic split unaligned and failed analysis. The original compiler pool occupies the aligned extent through `0x803DF0B8`; keep its one padding byte. The data split, initialized-state split and uninitialized-state split retain the required four-byte edges while field extents remain exact.

## F-504-3: Complete USB source and exact linked identity

All 14 retained USB functions, totaling 5,352 function bytes, now compare at 100% code. Every non-relocation instruction word is identical, including the five formerly differing control-helper offsets. Direct final-DOL comparison also reproduces all 14 complete function ranges including relocated instructions. Text, data and sbss each compare at 100% in objdiff. The compiled object's sdata diagnostic is 73.68421% because it also includes the older reference's unretained `GetStr\n` literal; the linker strips that unused small-data object. Full source-linked Ninja exits 0 and explicit frozen verification passes with DOL SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The two groups of otherwise unreferenced messages are represented by ordinary constant-pointer array definitions: `gUsbStrippedTransferMessages` and `gUsbStrippedInsertionMessages`. They contain only the five proven string literals, in their evidenced order. Their definitions have external C linkage so this compiler emits the initializer references; unused file-static definitions were removed before literal collection. Neither table is an SDK API or a claim about an original target container. Both exist solely to preserve observed pooled data. The compiler collects their strings into the ordinary writable literal pool, while the linker discards the two unreferenced pointer arrays from sdata2 and rodata. The final ELF contains neither table definition. No fake call, dummy function, executable bytes, fabricated API body, assembly fallback or compiler-policy change is used.

This produces a 0x82F-byte source data pool identical to the original pool's first 0x82F bytes. The remaining target byte is zero alignment padding supplied by the link. The prior 0x830-byte coarse label conflated 66 actual compiler string objects; objdiff annotated folded references against that entire object and reported argument mismatches despite identical instruction words. Each literal is now recorded separately in symbols.txt using its compiler-produced correspondence name, exact observed address, size and four-byte alignment. Every initializer was compared with the original before recording it. These names are reconstruction compiler labels, not recovered original names. The original bytes and frozen hashes are unchanged.

The corrected initialized-data range is `.sdata` `0x804B8E78`â€“`0x804B8E88`. It includes the six-byte `USB: ` prefix at `0x804B8E80` and its alignment padding. The following five-byte ` %s\n` format at `0x804B8E88` belongs to WUD: original WUD functions at `0x8030ABC0`, `0x8030B9E0` and `0x8030C700` reference it. It remains outside the USB split. The unused reference's `GetStr\n` object must strip rather than being mistaken for that next unit's format string.

The full USB unit is selected as `Matching`, so no original USB object is linked. EUART remains `Matching`; together these units contribute 6,236 source-linked function bytes. Other original-object dependencies, particularly WUD and the separate KPAD prerequisite branch, remain explicit. Issue #504 is still incomplete until its WUD functions and the KPAD prerequisite are accepted. Verification still covers the main game executable only.

### Additional acceptance traps

- **Unused pointer tables can preserve proven pooled strings without fake functions.** Under the established Wii/1.0 flags, external-linkage constant-pointer arrays retain initializer literals during compilation but disappear as unreferenced tables during the link. File-static unused arrays were discarded too early. Verify both the final table absence and full frozen output; do not infer success from a compiled pool alone.
- **A coarse pool label can obscure otherwise exact folded references.** USB's initial pool-sized object made objdiff attach all addends to one symbol, while the compiler emitted individual string objects. Zero-sized interior-label experiments still triggered dtk overlap errors. Recording the 66 proven literal objects resolves those annotations and gives 100% code/data comparison without changing expected bytes.
- **Adjacent small-data strings can belong to different libraries.** Extending USB through the next ` %s\n` format produced undefined references from WUD because the proposed source definition was local to USB. The verified boundary ends at `0x804B8E88`, after the USB prefix; preserve the following WUD format and let the old unretained USB literal strip.
