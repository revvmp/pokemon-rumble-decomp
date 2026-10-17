# F-543-1: MetroTRK initialization, packet support, and message buffers

Issue #543 owns 25 functions and 3,576 native instruction bytes at `0x8034EBD4`–`0x8034F9CC`. The complete message-buffer file starts in this issue and extends through `0x80350078`, including nine #544 functions and 1,708 additional instruction bytes. The eight complete files therefore retain 34 functions and 5,284 instruction bytes. No neighboring function receives duplicate credit.

Names, algorithms, and interfaces use Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, with target-specific differences verified against the frozen `wpse-v1` executable. The [source notice](../licenses/543-metrotrk-NOTICE.md) records provenance. Issue #544 supplies the complete message-buffer reconstruction through coordination with its owner. The common MetroTRK interfaces depend on the separate header prerequisite in PR #724.

## Boundaries and storage

| File under `MetroTRK/debugger/embedded/MetroTRK/` | Retained text | Additional sections |
| --- | --- | --- |
| `Portable/nubinit.c` | `0x8034EBD4`–`0x8034ED24` | `.data` `0x803E6428`–`0x803E6448`; `.sbss` `0x804B9C18`–`0x804B9C1C` |
| `Portable/serpoll.c` | `0x8034ED24`–`0x8034EE6C` | `.sbss` `0x804B9C20`–`0x804B9C24` |
| `Portable/string_TRK.c` | `0x8034EE6C`–`0x8034EE88` | None |
| `Portable/support.c` | `0x8034EE88`–`0x8034F4AC` | `.data` `0x803E6448`–`0x803E6490` |
| `Os/dolphin/targcont.c` | `0x8034F4AC`–`0x8034F4E0` | None |
| `Processor/ppc/Generic/mpc_7xx_603e.c` | `0x8034F4E0`–`0x8034F808` | None |
| `Portable/msg.c` | `0x8034F808`–`0x8034F86C` | `.data` `0x803E6490`–`0x803E64B8`; `.sbss` `0x804B9C28`–`0x804B9C2C` |
| `Portable/msgbuf.c` | `0x8034F86C`–`0x80350078` | `.data` `0x803E64B8`–`0x803E64E0`; `.bss` `0x804B3AC0`–`0x804B5464` |

`gTRKBigEndian` is a four-byte `BOOL`; `gTRKInputPendingPtr` is four bytes. The message sequence counter is two bytes followed by section alignment. The three `TRKBuffer` objects have 0x88C-byte strides: `isInUse` at 0, `length` at 4, `position` at 8, and 0x880 data bytes at 0xC. They occupy 0x19A4 bytes, with the following four bytes outside the source object. The reference's mutex member is absent in this target; its no-op mutex calls leave no target lock storage or instructions.

## Native differences from the reference

The target identifies itself as MetroTRK for Revolution v0.4, while the pinned reference welcome string is v0.1. Initialization has neither the reference serial-output initialization nor dispatcher initialization calls. The UART initialization interface consumes three arguments; its caller passes `1`, `0`, and the input-pending pointer address.

`TRKRequestSend` has two parameters and one send attempt. It polls until a packet arrives, handles intervening request packets, validates reply length and command/status, and releases rejected reply buffers. The reference's retry count and timeout parameters are absent. Both failure diagnostics are present in the target. `TRKMessageSend` inserts a nonzero 16-bit sequence value at packet offset 6 before sending, advances the counter, and reports write failure while returning the normal debugger status.

The source message-reply layout represents the 0x40-byte protocol header. Fields are named by their observed roles rather than the reference's numeric placeholder names. The first word is the packet length; command and reply words follow at 4 and 8; the file-operation parameter is at 0xC, followed by 48 payload bytes.

## Compiler evidence and assembly accounting

Wii/1.1, the previously measured MetroTRK options, small-data threshold 8, and four-byte function alignment reproduce all 19 retained functions outside `msgbuf.c` at their exact native sizes and with zero differing non-relocation instruction words. This is a probe result; relocation and full-artifact acceptance remain separate gates. Writable string pooling requires `-str reuse,pool,noreadonly`: the `readonly` variant emits diagnostics into `.rodata`, whereas this target places them in `.data`.

`TRKSaveExtended1Block` and `TRKRestoreExtended1Block` are legitimate original SDK assembly routines, totaling 808 instruction bytes. Their source reads and writes privileged processor registers and uses the debugger's interrupted register state. They are reconstructed assembly source, contain no embedded instruction words or copied executable arrays, and are accounted separately from C/C++ decompilation. The other 32 retained functions contain 4,476 C instruction bytes. For the original issue alone, 23 C functions contain 2,768 bytes and the two original assembly routines contain 808 bytes. None of the assembly bytes is claimed as C/C++ decompilation progress.

The target register-save routine includes additional block-address translation and processor-register banks absent in the pinned reference. Its observed storage offsets include segment registers at 0x1A8, time-base/HID/MSR state at 0x1E8, PVR and base BAT registers at 0x1FC, additional BAT registers at 0x240, processor state at 0x25C, and further implementation-specific banks through 0x32C. The restore routine preserves the target's branch boundaries and restores only the observed register banks. Numeric SPR operands avoid assembler alias differences.

## Validation state

The initial eight-file split build passes `.venv/Scripts/ninja build/543-split-baseline/wpse-v1/main.dol` with exit 0, followed by `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/543-split-baseline/wpse-v1` with exit 0. The rebuilt DOL matches frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The eight files were first registered `NonMatching`, compiled through `ninja all_source`, and compared as complete original/source object pairs. Every compared section is 100%. After source selection, the same actual-DOL Ninja target exits 0, the explicit frozen verifier exits 0, and the independent final audit verifies all 34 native function addresses/sizes, all four owned storage objects, the full original issue inventory, and source-object selection for every unit in the real link. Direct DOL byte equality also passes. The files are `Matching`; final acceptance still requires a clean committed head rebased onto current `main`, the default complete pipeline, and the shared-header prerequisites.

## Retained function inventory

| Address | Size | Recovered name |
| --- | --- | --- |
| `0x8034ebd4` | `0xAC` | `TRKInitializeNub` |
| `0x8034ec80` | `0x24` | `TRKTerminateNub` |
| `0x8034eca4` | `0xC` | `TRKNubWelcome` |
| `0x8034ecb0` | `0x74` | `TRKInitializeEndian` |
| `0x8034ed24` | `0xCC` | `TRKTestForPacket` |
| `0x8034edf0` | `0x2C` | `TRKGetInput` |
| `0x8034ee1c` | `0x40` | `TRKProcessInput` |
| `0x8034ee5c` | `0x8` | `TRKInitializeSerialHandler` |
| `0x8034ee64` | `0x8` | `TRKTerminateSerialHandler` |
| `0x8034ee6c` | `0x1C` | `TRK_strlen` |
| `0x8034ee88` | `0x1F4` | `TRKSuppAccessFile` |
| `0x8034f07c` | `0x128` | `TRKRequestSend` |
| `0x8034f1a4` | `0x118` | `HandleOpenFileSupportRequest` |
| `0x8034f2bc` | `0xE4` | `HandleCloseFileSupportRequest` |
| `0x8034f3a0` | `0x10C` | `HandlePositionFileSupportRequest` |
| `0x8034f4ac` | `0x34` | `TRKTargetContinue` |
| `0x8034f4e0` | `0x1C4` | `TRKSaveExtended1Block` |
| `0x8034f6a4` | `0x164` | `TRKRestoreExtended1Block` |
| `0x8034f808` | `0x64` | `TRKMessageSend` |
| `0x8034f86c` | `0x20` | `TRKInitializeMessageBuffers` |
| `0x8034f88c` | `0x9C` | `TRKGetFreeBuffer` |
| `0x8034f928` | `0x24` | `TRKGetBuffer` |
| `0x8034f94c` | `0x28` | `TRKReleaseBuffer` |
| `0x8034f974` | `0x28` | `TRKResetBuffer` |
| `0x8034f99c` | `0x30` | `TRKSetBufferPosition` |
| `0x8034f9cc` | `0xA4` | `TRKAppendBuffer` |
| `0x8034fa70` | `0x90` | `TRKReadBuffer` |
| `0x8034fb00` | `0xD0` | `TRKAppendBuffer1_ui32` |
| `0x8034fbd0` | `0xF4` | `TRKAppendBuffer1_ui64` |
| `0x8034fcc4` | `0x64` | `TRKAppendBuffer_ui8` |
| `0x8034fd28` | `0xF0` | `TRKAppendBuffer_ui32` |
| `0x8034fe18` | `0xE0` | `TRKReadBuffer1_ui64` |
| `0x8034fef8` | `0x98` | `TRKReadBuffer_ui8` |
| `0x8034ff90` | `0xE8` | `TRKReadBuffer_ui32` |

## Traps

A two-byte `.sbss` object cannot end a split on a two-byte boundary when the section's minimum alignment is four. dtk rejected the sequence-counter split ending at `0x804B9C2A`; retaining its two-byte symbol size and ending the split at `0x804B9C2C` preserves the natural section padding without synthetic source data.

The initial message-buffer catalog included four bytes of following padding in its 0x19A8-byte symbol. The target's native indexing uses three 0x88C-byte buffers, proving the actual 0x19A4-byte object. Ending its `.bss` split at the object boundary preserves the following padding in the next object.

The diagnostic call interfaces differ by file. Message-buffer exhaustion and message-send failure use fixed-argument declarations and generate no variadic-call condition-register instruction. The support file's diagnostics use the variadic interface and do generate it. A variadic declaration in the fixed-call units adds an instruction despite otherwise identical algorithms.

The compiler string flags are cumulative: a later `-str reuse` does not cancel `readonly`. Explicit `noreadonly` selects writable storage, and explicit `pool` retains the target's adjacent support diagnostics without per-string alignment. The support symbol contains 65 payload bytes; its old 72-byte survey size included following padding. Correcting its owned size and string-table classification restores whole-object comparison without adding source padding.

### Imported symbol reconciliation

The D-0014 imported map seed is retained outside the owned units and the proven callee names required by these sources. Native instruction and relocation comparisons establish the canonical pinned-source interfaces `TRKWriteUARTN`, `TRKSuppAccessFile`, `TRKGetFreeBuffer`, and `TRKAppendBuffer`; those spellings supersede imported underscore variants only where the paired source proves the exact target entry. This does not promote unverified map names or change neighboring unit boundaries.
