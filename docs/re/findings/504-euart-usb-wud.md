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

USB runs from `0x80308A50` through `0x80309F9C`. The following WUD unit begins with `WUDIsLinkedWBC` at `0x80309FA0`, contains the allocation wrappers and setup/flush routines in this issue, and continues into later block issues. These units remain original-object dependencies in this branch. The KPAD portion depends on #710. This EUART change alone does not complete #504.

USB scratch comparisons establish exact raw instruction matches for the older reference's logging, library open/close, cleanup callback, device open and asynchronous close. The target also contains newer long-bulk handling. A licensed newer reference supplies that path, but no USB source or split is incorporated yet; its identity and full-link acceptance remain investigation work.

## Provenance

EUART source follows [Petari at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/euart/euart.c), under the project's CC0 license. The file contains no separate license notice. Comments are stripped. The external frequency declaration and unique enabled-state name express target-proven accesses without importing another EXI header tree.

## Traps

- **EXIUart and EUART keep separate enabled flags.** A symbol named `Enabled` already exists at `0x804B959C` for EXIUart.c. EUART's corresponding object is at `0x804B9B14`; keep it distinct as `__EUARTEnabled`. Reusing one global metadata name would confuse the units despite identical source-local reference spelling.
- **A missing compatibility include does not require new ABI definitions.** EUART's imported `export/UART.h` is absent, but `MSL_C/UART.h` already defines the same UART interface. Reusing that existing header compiles the real unit and preserves exact code.
