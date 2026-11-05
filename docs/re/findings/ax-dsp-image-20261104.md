# AX DSP instruction image

## Confirmed scope

The remaining data fallback at `0x803D95E0` is `axDspSlave`, an 8,192-byte DSP instruction image. Its SHA-256 is `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`. It is executable code for the audio coprocessor even though the PowerPC DOL stores it in `.data`; treating it as a C integer array would not reconstruct that program.

The source-owned `AXDSPConfig.c` defines initialization vector `0x0010`, resume vector `0x0037`, and transfer length `0x2000`. `AXOut.c` transfers the full image through `__AXDSPTask`. The final 24 zero bytes are part of that transfer. Their interpretation as removable padding has not been established.

Applying Dolphin's byte-wise HashEctor algorithm produces `4cc52064`. At Dolphin commit `66df2b4e18ae6169f9718b869da334022914e305`, [UCodes.cpp](https://github.com/dolphin-emu/dolphin/blob/66df2b4e18ae6169f9718b869da334022914e305/Source/Core/Core/HW/DSPHLE/UCodes/UCodes.cpp) dispatches this identity to AXWii, and [AXWii.cpp](https://github.com/dolphin-emu/dolphin/blob/66df2b4e18ae6169f9718b869da334022914e305/Source/Core/Core/HW/DSPHLE/UCodes/AXWii.cpp) selects its newer filter behavior. These are independent classification evidence, not source that rebuilds this instruction image.

No matching payload hash appears in the 3,420-record extraction inventory. The original DOL remains the accepted build's input for this fallback.

## Private tooling experiment

The small GPL-2.0-or-later [devkitPro/gamecube-tools](https://github.com/devkitPro/gamecube-tools/tree/3b7f914df771d4b779d88607ceed2ec9264ff93a) assembler/disassembler was acquired at commit `3b7f914df771d4b779d88607ceed2ec9264ff93a` into ignored `tools/local/dsp-tools/gamecube-tools/`, preserving its original source and license. MSVC compiled its four implementation files with `/EHsc /O2`, `_MSVC_`, and the package-version definition. The local executable SHA-256 was `52c184f3dab038e9e7a4d243be22ccfc2e5e8456757c88bd93fc58a030b9f27e`. This diagnostic tool has not been added to the production toolchain.

The disassembly agrees with the two entry vectors: both establish status/register mode, write the `0xDCD1` mailbox identifier and the respective initialization/resume message, interrupt the host, and wait for mailbox acknowledgment. The image begins with its DSP exception jumps. One word, `0x024F` at DSP word address `0x0D04`, is unrecognized by this old tool's opcode table; this does not establish that the word is data.

Three real command-line roundtrip attempts failed during assembly:

| Disassembly options | Reassembly result |
| --- | --- |
| `-n -r -s` | Its generated dotted register-name `EQU` declarations are rejected by its own label parser, beginning at line 22. |
| `-s` | Numeric full-accumulator operands fail the accumulator-register check at line 25. |
| `-r -s` | An extended load emits an accumulator combination rejected by the corresponding `ACX0` parameter check at line 430. |

Disassembly exited zero in all three cases; assembly exited one. No output was accepted, repaired or linked. The diagnostic inputs and listings are under ignored `tools/local/dsp-tools/`.

## Current-tool exact roundtrip

The current Dolphin assembler, disassembler, instruction tables and label map at the pinned commit above were built as a private standalone inspector with MSVC 2022. Their instruction encoding implementations are unchanged. A local driver supplies binary input/output, and minimal host shims replace filesystem, logging and string helpers. `DSPCore.h` retains its original register and constant enums without the emulator state. The build uses Dolphin's pinned fmt commit `e424e3f2e607da02742f73db84873b8084fc714c`, `/std:c++latest`, `/utf-8` and header-only fmt. All fetched license notices remain in the ignored tool directory.

The executable `tools/local/dsp-tools/dolphin-assembler/dsp-inspect.exe` has SHA-256 `22f7d1df9088188a391f81af3f26bfa4694c2980938069148564fc95456db7ab`. The actual disassemble, assemble and annotated-list commands each exit zero. Reassembly of the generated `ax-wpse-v1-modern.ds` reproduces all 8,192 bytes, with zero differences and the original image SHA-256. This establishes an exact tool roundtrip, not a source reconstruction. The generated listing remains private and is not a production input.

The apparently unknown word `0x024F` at DSP address `0x0D04` is a command-dispatch table entry. At `0x0079`, the dispatcher loads table base `0x0D02`, adds a command bounded to 0 through 14, reads instruction memory and jumps through the result. The third table entry is therefore a handler address, not an unsupported instruction. Its `COMMAND_SUB_TO_LR` identity follows from the source-owned `AXCL.c` command enum and producer. Further handler and argument evidence is recorded in the separate DSP identity investigation.

## Remaining work and acceptance boundary

A bounded primary-source search found the Dolphin AXWii implementation, SDK-side image declarations and DSP tooling, but no readable source that rebuilds this revision. That search is not proof that such source does not exist. The successful current-tool roundtrip above provides reliable instruction inspection for the next reconstruction work. Original assembly-language provenance is still unresolved.

Before accepting an assembly reconstruction, establish original-language provenance under [D-0015](../../DECISIONS.md#d-0015-assembly-source-only-where-the-original-was-assembly), recover readable routine/data identities and symbolic control flow, and validate the assembler itself against every word of this image. Generated disassembly and a copied opcode array are not source completion. Neither this investigation nor identifying its DSP family changes any progress numerator.

## Traps

- A successful disassembler exit does not establish a working assembler roundtrip. This pinned old tool rejects three forms of its own output. Validate an exact roundtrip before using it in a reconstruction build.
- Storage in a DOL data section does not make a coprocessor instruction stream an asset. Keep the DSP dependency visible in source-completion claims.
- Linear disassembly decodes jump tables as instructions. The dispatcher's instruction-memory load proves that the reported unknown opcode at `0x0D04` is an address in the command table; classify reachable control flow before inventing an instruction encoding.
