# OS and WUD embedded-firmware source review

## Scope and conclusion

This independent review covers the proposed 128-byte OS DSP initializer and the WUD controller patch: a 180-byte payload, its eight-byte transport header, and seven thirteen-byte installation records. It reviews source readability, semantic naming, reference independence and technical replacement evidence separately from original-language provenance. The earlier AX DSP acceptance does not establish provenance for either program.

The reviewed sources are readable symbolic reconstructions, not opcode arrays or generated address-label listings. Both encode-only paths independently reproduce the corresponding frozen DOL ranges. The OS typed C replacement produces a byte-identical complete PowerPC object; the WUD typed C replacement preserves all allocated object contents and has a passing private full-executable comparison. These facts support a concrete replacement proposal, but do not prove how Nintendo or Broadcom originally authored the programs.

The owner explicitly approved the narrow OS/WUD assembly exceptions during this review, recorded in [D-0017](../../DECISIONS.md#d-0017-two-owner-approved-embedded-firmware-assembly-exceptions). It is an exception to the original-language requirement, not new evidence that either program was originally assembly. Production integration, normal frozen verification and two complete clean builds now pass; [final acceptance](main-source-completion-20261104.md) records all thirteen identical artifacts and the unchanged OS/WUD compiler objects.

## DSP initializer review

Reviewed source: `tools/local/tail-20261104/smallgame/DSP/os_init/production_packet/src/RVL_SDK/os/dsp/os_dsp_init.ds`. Its companion C proposal replaces the retained initializer in `OSAudioSystem.c` with the generated `OSDspInit.inc` and uses 64 big-endian DSP words.

The source expresses eight vector entries, initialization, memory walks, mailbox completion and seven exception returns as named blocks. Every branch and loop end names a label. Memory extents and completion-mail halves are named constants; hardware mailbox operands use the assembler's `DMBH` and `DMBL` symbols. The final reserved extent is represented explicitly and retains all six zero words. No instruction stream is pasted into an integer directive.

The memory sequence is internally consistent: the instruction-ROM walk begins at `0x8000` for 4096 words, the data-RAM clear starts at zero for 4096 words, and the following data-ROM walk begins at `0x1000` for 2048 words. The outgoing mailbox wait and `0x0054`/`0x4348` writes post completion mail; the host waits for the full flag and consumes the low half without checking the message value. The pinned Dolphin register and exception definitions independently support the mailbox names and the stack, accelerator-overflow and external-interrupt vector labels. They do not establish Nintendo's source language or original author-chosen routine names. [The OS investigation](os-dsp-init-20261104.md) records the bounded original-language search and its negative result.

The candidate's `u8[]` to `u16[]` change does not alter its only host use: a 128-byte copy. No shared declaration imposes the old element type. An independent whole-file comparison confirms the control and candidate objects are identical, including symbols, relocations, metadata and alignment, with SHA-256 `372f44d621f0bdc3cc9615b03dabdec49c6fa7ddf3cdeb9fe308ff52c954d4a6`.

## WUD firmware review

Reviewed source: `tools/local/tail-20261104/vm_wud_firmware/controller_patch.s`, seven `patch_*.s` files, and `patches.json`. The controller's fixed ROM entry points and memory addresses are named constants, while local transfers use labels. The payload separates the four hook bodies, the callable reset helper and its literal pool. The JSON records separately describe patch slots and destinations. The code and records remain editable at the level of instructions, named operations and protocol fields.

The names are reconstructed descriptions, not recovered Broadcom symbols. In particular, the full meanings of the existing ROM routines are not established without their bodies. The labels should not be used as evidence for a broader controller subsystem interpretation. The explicit operation sequence and fixed addresses are the proven facts.

Two details received specific review:

- The three reset-helper masks operate on 32-bit words. Each mask keeps only selected low sixteen bits and clears every upper-half bit. The original `ClearControlBit*` labels understated that effect; the author corrected them to `KeepLow16Except*`.
- The zero halfword between the reset entry and helper has an inferred alignment role, not proven original authoring intent. The author confirmed that representing it as `movs r0, r0` preserves the exact encoding and avoids both a raw halfword and a claim that it cannot execute. The earlier private assembler's alignment-directive behavior is documented in [the executable-data audit](embedded-code-audit-20261104.md).

The typed `WUDPatchCmd` proposal makes the thirteen-byte record explicit: one slot byte, four little-endian address bytes, and eight instruction bytes. Keeping the address as bytes preserves the wire byte order on the big-endian PowerPC host and introduces no alignment padding. Copying the address of the whole record replaces the old address of its thirteen-byte array without changing transport behavior. The generated initializer supplies these fields from the manifest and assembled source; it does not turn retained opcode input into purported semantic data.

The WUD version gate, little-endian decoding, vendor-command consumers and controller instruction/data partition are independently documented in [the embedded-code audit](embedded-code-audit-20261104.md). The inline hooks are compatible with assembly patching, but the 76-byte callable helper could have been compiled from C. Neither its shape nor exact reassembly satisfies D-0015's original-language requirement by itself.

## Independent technical evidence

The reviewer ran the existing DSP build wrapper against the staged DSP source and the WUD encode-only generator against its source manifest. Both commands exited zero and wrote separate outputs under `tools/local/tail-20261104/jpeg-firmware-review`. Neither generation command accepts an original executable, original object or reference payload as an input.

After generation, `jpeg_embedded_source_review.py` independently verifies the original DOL against its frozen size and SHA-256, maps the three relevant address ranges, and compares the generated bytes directly. It also rechecks the complete OS objects and the private WUD native-link executable. Its receipt is `jpeg-firmware-review/review-evidence.json`.

| Compared output | Bytes | SHA-256 | Result |
| --- | ---: | --- | --- |
| OS DSP initializer | 128 | `5b873ee91f33b0884574b814d7f46d81e283394de91b8b2ea9073ac218dee350` | Exact frozen range |
| WUD payload and transport header | 188 | `fde41feb3ac0bab076e24e613fd40768e3c099bcd7f34c1f746129188990ebae` | Exact frozen range |
| WUD installation records | 92 | `3a5f6a9f6a120cb10a4224a4bcc35db8e75459d62ebaf5b9879180b9e8de5f2c` | Exact frozen range |
| Private WUD native `main.dol` | 4094880 | `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da` | Exact frozen executable |

The encode-only WUD generator is distinct from the earlier diagnostic `build.py`, which reads references only after assembly to compare results. No reference binary is part of the production proposal's generation inputs.

## Production WUD generator review

The final private packet is `tools/local/tail-20261104/vm_wud_production`. The reviewer read its complete `tools/firmware/build_wud.py`, all eight assembly files, linker script, manifest, typed C/header changes and generated depfile. The final reviewed generator hashes to `76ef7a016cecc19872498a238bdeaf55aff4d207f45978eb9e2d2027a4a4c635`; its final guard also rejects the GNU linker `STARTUP` input command. The native sources retain the corrected mask names and express the zero halfword as `movs r0, r0`. Their only integer word directives are the named literal-pool values, not instructions.

Generation uses the existing pinned Zig 0.13.0 installation. The helper verifies the acquisition receipt against `config/dsp_assembler.json` and hashes the actual compiler executable before running it. Eight ARM7TDMI Thumb objects are assembled, linked by `ld.lld` at their manifest addresses, and extracted by `objcopy`. Real payload entries have Thumb function symbols; fixed ROM targets are linker symbols with the Thumb-state bit. The generator performs no instruction rewriting, relocation repair, reference-byte copying or object patching.

Before extraction, the linked ELF must contain exactly the eight expected nonempty allocated sections. Every section must have its manifest address, its expected 180-byte or eight-byte extent, and executable flags. The resulting download and installation records must be 188 and 92 bytes. Their little-endian destination fields and typed installation initializers are derived from the manifest and assembled outputs. The C diff consists of the two generated includes and the whole-record address passed to the existing transport copy; the header diff only replaces the opaque thirteen-byte array with its three byte-aligned fields.

The depfile names all four generated binary/include outputs and fifteen inputs: the controller manifest, tool manifest, acquisition receipt, compiler executable, generator, shared publication helper, linker script and eight assembly files. No original executable, object or reference binary is an input. Source filenames are confined to the manifest directory; assembly file imports and linker imports are rejected. Duplicate JSON keys, invalid record fields, overlapping code ranges and duplicate patch slots are also rejected.

Review found a concrete input-stability gap in the first helper: validation read each input before a second read established the build snapshot. A change between those reads could therefore replace the validated bytes without failing the later comparison. The author corrected this with a shared `capture` operation that retains the bytes actually parsed, scanned or hashed, rejects a differing repeat read, and compares the same snapshots again before publication. The reviewer inspected this correction directly. The separate command-line validation lane also tightened the import-directive guard to cover labelled and semicolon-separated forms.

Publication reuses the accepted DSP helper without modifications; the private and canonical `build_dsp.py` files both hash to `2412ea532274c68b9ae3c8a23dad101cce81bc789b496726d37c015668e36447`. The helper stages every output, checks input/output and output/output aliases, preserves unchanged files, and restores already replaced outputs if publication fails. If restoration itself fails, it retains the original backup and reports its location. All assembly and linking happen in a private temporary directory before publication. The independent validation lane owns actual CLI, Ninja dependency, mutation and failure-recovery exercises; their results and normal production acceptance are recorded separately when complete.

## Traps

- Instruction-byte equality and readable assembly do not prove the original source language. Record an explicit narrow exception when that evidence is unavailable; do not infer it from the already accepted AX program.
- A mask's descriptive name must include its width effects. ANDing a 32-bit value with `0x0000FFFE` clears the entire upper half as well as bit zero.
- A zero word's role may be uncertain even when its bytes are known. Preserve its possible instruction semantics or document the fixed extent without asserting unsupported original padding intent.
- An input-stability snapshot must retain the bytes that were validated. Rereading the file afterward can silently accept an intervening change; capture parsed, scanned and hashed bytes once and compare later reads against them.
