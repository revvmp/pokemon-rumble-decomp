# F-545-1: MetroTRK target implementation and serial-I/O option

## Scope and current evidence

Issue #545 contains 38 retained functions and 6,376 instruction bytes. The first two functions, `__write_file` and `__access_file`, belong to the console translation unit owned by #544. This issue owns the complete `Processor/ppc/Generic/targimpl.c` unit at `0x80351188`–`0x803529AC` and `Os/dolphin/target_options.c` at `0x803529AC`–`0x803529BC`.

The pinned Petari reference is commit `e0ab141914921726a595573dc48d3f13d0922efe`. The original SDK assembly functions remain assembly; their coverage must be reported separately from C/C++ decompilation. The SPR/FPR/paired-single access routines construct instructions from runtime register arguments and execute them as part of the debugger protocol. Those dynamic encoders are the original routine behavior, not executable-byte fallbacks.

All 34 retained target-implementation functions currently reproduce their native non-relocation instruction streams together under Wii/1.0a, Wii/1.0RC1, and Wii/0x4201_127 with the measured MetroTRK flags, eight-byte SDA threshold, and four-byte function alignment. This is candidate evidence only: complete unit comparison, relocation/data proof, source-selected linking, native final addresses, and frozen-DOL verification remain required before acceptance.

## Target-specific behavior

The target uses aligned 32-bit accesses to read and update individual bytes while switching MSR translation state. It does not use the reference's byte-load assembly copy loop. Its trace-enable operation sets the trace bit and clears external-interrupt enable; disabling trace reverses those two bits. Exception dispatch uses an integer switch with ascending program/trace cases.

The stop reply is `0x4E8` bytes: the protocol header, 37 default CPU registers, five low-memory thread-queue/current-thread words, and a 1,024-byte instruction window aligned around the PC. The routine at `0x8035291C` traverses the SDK active-thread list and reports its count and current-thread index. It is not the reference's ARAM-access function. `TRKTargetGetThreadInfo` is a descriptive name justified by those accesses; no original name has been recovered for it.

## Traps

- Wii/1.0, Wii/1.1, Wii/1.3, and later compiler revisions reproduce 33 target functions but schedule the first floating-point access template copy differently in 13 instruction words. Earlier Wii/1.0a, Wii/1.0RC1, and Wii/0x4201_127 reproduce all 34 together. No volatile storage or scheduling pragma was needed. Limit the compiler conclusion to this complete unit until further units discriminate these otherwise equivalent candidates.
- The public reference includes a nine-byte restore-flag structure with a field named as linker padding. Native code currently proves only the first two flag bytes. Do not turn the reference padding into a claimed original layout; inspect the complete native storage and linker allocation before deciding its declaration and extent.
- Native `.text` addresses in this region map with the established text delta, but `.rodata` addresses do not. Read the DOL segment table when comparing initialized data; applying the text delta to `0x80399358` reads unrelated game data.
