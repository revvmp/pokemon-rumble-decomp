# OS DSP initializer reconstruction, 2026-11-04

The owner approved this concrete 128-byte assembly reconstruction under [D-0017](../../DECISIONS.md#d-0017-two-owner-approved-embedded-firmware-assembly-exceptions). The coordinator integrated the frozen source packet and reported normal `configure.py` and `ninja` exits of zero followed by frozen `main.dol` verification PASS. Its original authoring language remains unresolved. Two final complete clean builds now reproduce the executable and all thirteen compared artifacts; see [final acceptance](main-source-completion-20261104.md).

## Separate executable dependency

`src/RVL_SDK/os/OSAudioSystem.c` contains the 128-byte `DSPInitCode` program. The earlier SDK ledger and [OS audio acceptance](465-os-arena-audio-cache.md) explicitly disclosed the retained DSP table without claiming its source reconstruction. Its PowerPC storage address is `0x803C7770`, and its file offset in the frozen `main.dol` is `0x003C3870`. Direct extraction after verifying the frozen DOL SHA-256 reproduced the former inline array exactly. The image SHA-256 is `5b873ee91f33b0884574b814d7f46d81e283394de91b8b2ea9073ac218dee350`.

This is distinct from the 8,192-byte AX program. Finishing AX did not remove this embedded executable dependency, even though all three PowerPC functions in OSAudioSystem already matched and linked from source. The coordinator now integrates its symbolic assembly through a second normal DSP generation rule with `--size 128`; OSAudioSystem consumes the generated `OSDspInit.inc`.

## Recovered behavior

The eight entries at DSP word addresses `0x0000` through `0x000F` jump to initialization and the seven exception handlers. Initialization at `0x0010` clears four status bits, disables address wrapping for `AR0`, and reads all 4,096 words of instruction ROM starting at `0x8000`. It then clears 4,096 words of data RAM from address zero, reusing the same loop count, and reads 2,048 words of data ROM starting at `0x1000`.

At `0x0028` it waits for the outgoing mailbox to become available. It writes high halfword `0x0054` and low halfword `0x4348`, then halts. The host sees the mailbox-full bit as part of `0x80544348`. Every exception handler is a single `RTI`; the final six zero words remain within the transferred 128-byte image. Their original intended role is not established, and they are not discarded as padding.

The matched PowerPC owner temporarily copies this program to `0x81000000`, flushes it, performs the DSP/ARAM initialization sequence, waits for the outgoing mail, resets the DSP, and restores the overwritten work memory. The pinned Dolphin [initialization implementation](https://github.com/dolphin-emu/dolphin/blob/66df2b4e18ae6169f9718b869da334022914e305/Source/Core/Core/HW/DSPHLE/UCodes/INIT.cpp) independently sends the same completion mail. Its [control-register handler](https://github.com/dolphin-emu/dolphin/blob/66df2b4e18ae6169f9718b869da334022914e305/Source/Core/Core/HW/DSPHLE/DSPHLE.cpp) selects this implementation from the initialization trigger without hashing the input program. This is behavioral corroboration, not a separate content-hash or original-source identification.

## Private readable source and exact encoding

`tools/local/tail-20261104/smallgame/DSP/os_init/os_dsp_init.ds` is a small symbolic reconstruction of these operations. It has named vectors, hardware-memory extents, mailbox fields, loop ends, and exception handlers. No raw instruction array, generated disassembly, or numeric branch target is used. The source SHA-256 is `2172effab61b1f5b6d40c5923e703907fbe72cfdec834b164db5c973a5535499`.

Actual assembly with both the private MSVC inspector and the reproducible Zig inspector exits zero without diagnostics. Both outputs are exactly 128 bytes with the original image SHA-256. `original-range.json` records the sealed DOL extraction, and `source-comparison.json` records the two real assembler results. These private comparisons support the approved D-0017 replacement; they do not establish its original source language or complete production acceptance.

## Typed embedding candidate

The existing build wrapper also assembles this source with `--size 128` and emits a 64-word C include. A private OSAudioSystem candidate replaces only the embedded initializer with:

```c
u16 DSPInitCode[] = {
#include "OSDspInit.inc"
};
```

The only source reference to the array is a 128-byte `memcpy`; no shared header declares its element type. The storage remains 128 bytes and the target compiler gives the data section the same eight-byte alignment. Both control and candidate were compiled with the configured Wii/1.0 command, with dependency output removed and all outputs isolated. All three PowerPC functions compare at 100%.

More strongly, the complete resulting objects are byte-identical, including `.text`, `.data`, `.rela.text`, symbol/string tables, section metadata, and the compiler comment section. Both object SHA-256 values are `372f44d621f0bdc3cc9615b03dabdec49c6fa7ddf3cdeb9fe308ff52c954d4a6`. `typed-wrapper-comparison.json` records the complete comparison. The private `os_init/production_packet/` stages the readable DSP source and candidate C owner with a source-hash inventory. No full-link trial was performed by this investigator.

The installed canonical files retain the frozen packet's paths and exact source hashes:

| Path | Source bytes | SHA-256 |
| --- | ---: | --- |
| `src/RVL_SDK/os/dsp/os_dsp_init.ds` | 1,540 | `2172effab61b1f5b6d40c5923e703907fbe72cfdec834b164db5c973a5535499` |
| `src/RVL_SDK/os/OSAudioSystem.c` | 4,342 | `f4fcf84fcc0ddba4552ad7afad0caeaf0817b7ab794ff7657c81a9f5c9858a9c` |

The first file is a standalone assembly input with no include dependencies. The second changes only the initializer's element type and generated-include body. The existing DSP generator produces `OSDspInit.inc` from it with `--size 128`; the frozen source bytes do not depend on a new assembler or generator implementation. Both installed source hashes were checked against the packet after integration. The coordinator's successful normal build and frozen-DOL comparison establish production acceptance for this replacement; final repeated clean-build evidence will follow the remaining WUD integration.

## Bounded original-language search

The pinned [DIAG4RVL SDK file metadata](https://github.com/bowsersbigbeanburrito/N/blob/81da5adc12d863744af0906e21bc175c6ff80a8d/DIAG4RVL/trunk/SDK/build_file_tree.json) lists `libraries/os/src/OSAudioSystem.c` as 13.08 KB with SHA-256 `6fb95d8ce634fe02159a39e8aa204df876014ad10c7ff82a9112e56ff364306d`. The OS source/include inventory contains no separate DSP initializer assembly or generated-code include. The DSP library directory lists the PowerPC `dsp_boot.c`, `dsp_task.c`, and related files. The build-tools directory lists the DSP assembler, linker, and converters, but their existence does not connect this initializer to an assembly input.

The separately listed RVL SDK 0.2 and BOST package inventories likewise contain OSAudioSystem.c without a named OS DSP assembly companion. A bounded scan of assembly filenames and `dspinit`, `initcode`, `romcode`, and `touch` candidates found the AX assembly inputs and `card_dec.asm`, not this initializer. These negative results cover only the examined metadata and do not establish that an assembly source never existed.

The existing [Petari source](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/os/OSAudioSystem.c) and [ogws source](https://github.com/doldecomp/ogws/blob/e5a4d369011062fe243aa63f6c2e2853854c395d/src/revolution/OS/OSAudioSystem.c) retain an inline C byte array without original generator or assembly-filename metadata. GitHub code searches for the initializer and concrete assembly-name candidates found other community copies of the byte array and Dolphin's behavioral implementation. None establishes whether Nintendo authored assembly which was converted into the C initializer, or authored the instruction encodings directly.

The fixed vectors and explicit hardware-memory walk are consistent with a small hand-written DSP routine. That observation is not proof of its original source language. The AX-family assembly inventory must not be treated as original-language evidence for this distinct OS program. After reviewing this concrete candidate and its provenance limits, the owner explicitly approved the narrow assembly exception recorded in D-0017. That exception resolves the original-language policy gate for this payload without asserting provenance that the evidence does not establish. Normal build and complete frozen artifact verification subsequently passed as recorded above.

## Traps

- A matching PowerPC unit can still embed another processor's retained executable. Inventory such tables separately before claiming all embedded code is reconstructed.
- A behavioral emulator's initialization selector is not necessarily an image fingerprint. Dolphin selects this initializer from a control-register transition without hashing its input, so its special identity must not be presented as a content match.
- Exact assembly and a byte-identical typed C wrapper establish technical replacement feasibility, not original-language provenance. Keep these gates separate.
