# F-545-1: MetroTRK target implementation and serial-I/O option

## Scope and current evidence

Issue #545 contains 38 retained functions and 6,376 instruction bytes. The first two functions, `__write_file` and `__access_file`, belong to the console translation unit owned by #544. This issue owns the complete `Processor/ppc/Generic/targimpl.c` unit at `0x80351188`–`0x803529AC` and `Os/dolphin/target_options.c` at `0x803529AC`–`0x803529BC`.

The pinned Petari reference is commit `e0ab141914921726a595573dc48d3f13d0922efe`. The target implementation and interfaces were also cross-checked against `xbret/xenoblade` commit `ea25691b2029394ab777bda7060139e517a13cba`, `libs/PowerPC_EABI_Support/src/MetroTRK/targimpl.c` and its corresponding headers. The original SDK assembly functions remain assembly; their coverage must be reported separately from C/C++ decompilation. The SPR/FPR/paired-single access routines construct instructions from runtime register arguments and execute them as part of the debugger protocol. Those dynamic encoders are the original routine behavior, not executable-byte fallbacks.

All 34 retained target-implementation functions currently reproduce their native non-relocation instruction streams together under Wii/1.0a, Wii/1.0RC1, and Wii/0x4201_127 with the measured MetroTRK flags, eight-byte SDA threshold, and four-byte function alignment. The canonical source also passes all 253 paired relocations, exact comparisons of 136 read-only and 16 initialized bytes, and the complete 1,448-byte native BSS extent and object offsets. Both complete source objects compare at 100% in every section. After arranging definitions in native address order, the source-selected custom link, explicit actual-output frozen verifier, all 36 owned function addresses/sizes, all named storage offsets, and direct whole-DOL equality pass. The original 38-function inventory additionally requires the two console functions from #544. A fresh default build and plain Ninja also exit zero, including the hardcoded SHA1 check. The explicit frozen verifier and native final-address/source-selection audit pass on that committed source head.

## Target-specific behavior

The target uses aligned 32-bit accesses to read and update individual bytes while switching MSR translation state. It does not use the reference's byte-load assembly copy loop. Its trace-enable operation sets the trace bit and clears external-interrupt enable; disabling trace reverses those two bits. Exception dispatch uses an integer switch with ascending program/trace cases.

The stop reply is `0x4E8` bytes: the protocol header, 37 default CPU registers, five low-memory thread-queue/current-thread words, and a 1,024-byte instruction window aligned around the PC. The routine at `0x8035291C` traverses the SDK active-thread list and reports its count and current-thread index. It is not the reference's ARAM-access function. `GetThreadInfo` is the spelling already established by the two-game high-confidence F-0013 seed and independently present in the pinned xenoblade source. Its thread-count/index interface and private linkage, and the private byte-copy helper, follow that reference and reproduce the native bytes and relocations.

## Storage and protocol model

The reconstructed `TRKStepStatus` layout uses eight-byte type alignment and has the observed size `0x18`: its five observed fields occupy offsets `0`, `4`, `8`, `0xC`, and `0x10`; this alignment model accounts for the final four bytes without inventing a field. The xenoblade reference instead labels the final word as unknown; no target instruction assigns it a meaning. Explicit zero initializers on save state, the 128-bit temporary, debugger state, and CPU state preserve their native allocation order while retaining normal compiler pooling. The restore flags use a nine-byte opaque storage view. Native C and assembly prove time-base and decrementer flags at indices zero and one; the remaining bytes have no claimed field semantics. The public reference's named linker-padding field is not retained.

The canonical reply parameter union gains `instruction`, approved by the message-handler owner, for the instruction word at protocol offset `0xC` in stopped (`0x90`) and exception (`0x91`) notifications. This does not change the union layout. The stop reply's instruction store is at `0x80351EB4`; the same slot in exception notification follows its native instruction read. Thread traversal uses the imported `OSThread.linkActive.next` member at offset `0x2FC`, not a locally invented context-padding type.

The target-implementation file retains 26 C functions / 5,252 instruction bytes and eight original assembly functions / 928 bytes. The serial-I/O option adds two C functions / 16 bytes. The console prerequisite contributes the first two issue functions / 180 bytes. Assembly coverage earns no C/C++ decompilation credit.

## Traps

- Complete object sections can score 100% while function definitions occur in a different physical order. The first target link placed `__TRK_get_MSR` at `0x80352540` instead of `0x80351188`, although every function and paired relocation matched. Native address-order definitions preserve all instruction streams, storage, and relocations and put every object function at its required native offset. Always audit final addresses and the actual DOL.
- The custom-directory default Ninja CHECK reads the hardcoded `build/wpse-v1/main.dol` in `build.sha1`. In a fresh worktree that file is absent, so CHECK fails after a successful custom link. The actual custom `main.dol` and `all_source` targets plus explicit frozen verification established the candidate link, but final acceptance still requires plain Ninja to exit zero. Use a fresh default build for that complete pipeline. Do not alter the expected hash or use a previous default artifact as evidence.

- Wii/1.0, Wii/1.1, Wii/1.3, and later compiler revisions reproduce 33 target functions but schedule the first floating-point access template copy differently in 13 instruction words. Earlier Wii/1.0a, Wii/1.0RC1, and Wii/0x4201_127 reproduce all 34 together. No volatile storage or scheduling pragma was needed. Limit the compiler conclusion to this complete unit until further units discriminate these otherwise equivalent candidates.
- The public reference includes a nine-byte restore-flag structure with a field named as linker padding. Native code currently proves only the first two flag bytes. Do not turn the reference padding into a claimed original layout; retain an explicit opaque view and record only the two proven meanings; do not claim a recovered original structure for the remaining bytes.
- Native `.text` addresses in this region map with the established text delta, but `.rodata` addresses do not. Read the DOL segment table when comparing initialized data; applying the text delta to `0x80399358` reads unrelated game data.

- A tentative zero-storage declaration lets CodeWarrior allocate variables by later use. Explicit aggregate zero initialization preserves the required native declaration order without disabling pooling. Turning pooling off repaired storage order but changed native address-generation instructions, so that attempt is rejected.
- CodeWarrior distinguishes a type alignment attribute immediately after the closing struct brace from an attribute after the typedef name. The former gives the measured 24-byte, eight-byte aligned step-status type; the latter only aligns the object and changes its pooling and code generation. Whole-function and storage comparisons validated the type form.

## Retained target-unit inventory

| Address | Function | Instruction bytes |
| --- | --- | --- |
| `0x80351188` | `__TRK_get_MSR` | 8 |
| `0x80351190` | `__TRK_set_MSR` | 8 |
| `0x80351198` | `TRKValidMemory32` | 248 |
| `0x80351290` | `TRK_ppc_memcpy` | 204 |
| `0x8035135C` | `TRKTargetAccessMemory` | 336 |
| `0x803514AC` | `TRKTargetAccessDefault` | 248 |
| `0x803515A4` | `TRKTargetAccessFP` | 316 |
| `0x803516E0` | `TRKTargetAccessExtended1` | 356 |
| `0x80351844` | `TRKTargetAccessExtended2` | 368 |
| `0x803519B4` | `TRKInterruptHandler` | 404 |
| `0x80351B48` | `TRKExceptionHandler` | 156 |
| `0x80351BE4` | `TRKPostInterruptEvent` | 184 |
| `0x80351C9C` | `TRKSwapAndGo` | 196 |
| `0x80351D60` | `TRKInterruptHandlerEnableInterrupts` | 84 |
| `0x80351DB4` | `TRKTargetInterrupt` | 92 |
| `0x80351E10` | `TRKTargetAddStopInfo` | 512 |
| `0x80352010` | `TRKTargetAddExceptionInfo` | 156 |
| `0x803520AC` | `TRKTargetCheckStep` | 292 |
| `0x803521D0` | `TRKTargetSingleStep` | 136 |
| `0x80352258` | `TRKTargetStepOutOfRange` | 116 |
| `0x803522CC` | `TRKTargetGetPC` | 16 |
| `0x803522DC` | `TRKTargetSupportRequest` | 500 |
| `0x803524D0` | `TRKTargetStopped` | 16 |
| `0x803524E0` | `TRKTargetSetStopped` | 16 |
| `0x803524F0` | `TRKTargetStop` | 24 |
| `0x80352508` | `TRKPPCAccessSPR` | 220 |
| `0x803525E4` | `TRKPPCAccessPairedSingleRegister` | 168 |
| `0x8035268C` | `ReadFPSCR` | 36 |
| `0x803526B0` | `WriteFPSCR` | 36 |
| `0x803526D4` | `TRKPPCAccessFPRegister` | 456 |
| `0x8035289C` | `TRKPPCAccessSpecialReg` | 104 |
| `0x80352904` | `TRKTargetSetInputPendingPtr` | 16 |
| `0x80352914` | `ConvertAddress` | 8 |
| `0x8035291C` | `GetThreadInfo` | 144 |
| `0x803529AC` | `SetUseSerialIO` | 8 |
| `0x803529B4` | `GetUseSerialIO` | 8 |

## Reference-map reconciliation

The `GetThreadInfo` name at `0x8035291C` agrees with the high-confidence two-game, 34-function seed recorded by F-0013 and with the pinned xenoblade source. Its two output arguments are `int*`; `ConvertAddress` retains the reference's `u32` address interface. Both reproduce the native instruction streams and all paired relocations. The complete 38-function / 6,376-byte issue inventory has also been reconciled, including the two console candidates independently checked against the coordinated #544 source object. Their final main source selection remains part of acceptance.

Per D-0014, source-proven canonical spellings supersede imported map variants where needed: `TRKSuppAccessFile` and `TRKAppendBuffer` are the referenced source interfaces, rather than `TRK_SuppAccessFile` and `TRK_AppendBuffer`. Outside symbol changes preserve their owning splits, sizes, and types.

## Full source-link verification

The fresh default pipeline compiled 424 targets. `python configure.py`, `.venv/Scripts/ninja`, `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`, and the outside native audit all exit zero. Both owned units are selected from `src` objects, all 36 native function addresses and sizes and all named storage addresses are exact, and the complete rebuilt DOL equals the frozen original. The whole target object has `.text`, `.rodata`, `.data`, `.bss`, and `.sbss` at 100%; the option object has `.text` and `.sbss` at 100%. No new general compiler or ABI assumptions are made beyond this unit.
