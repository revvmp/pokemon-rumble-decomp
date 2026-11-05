# Toolchain investigation

The accepted per-library CPU compilers and reproducible build tools are pinned below and in config/toolchain.json. DSP/Thumb reconstruction tools are separately pinned by config/dsp_assembler.json. Exact original linker provenance and package tooling remain qualified; matching output does not establish an unobserved original tool version.

Investigate format metadata, compiler signatures, code generation, runtime/library patterns, section layout, relocation behavior, and link ordering. Treat resemblance as a hypothesis until comparison supports it. Record evidence in `FINDINGS.md` and accepted requirements in `config/toolchain.json`.

For every selected tool record its exact version, acquisition source, binary SHA-256, host requirements, command invocation, working directory, flags, environment variables, and findings that justify its use. Record unresolved alternatives explicitly. Do not commit proprietary tool binaries.

## Candidate tools

This historical candidate list records the initial [prior-art](PRIOR_ART.md) survey. The selected-toolchain section below and current configuration supersede these candidate versions.

| Candidate | Purpose | Reference version |
| --- | --- | --- |
| decomp-toolkit | WAD and archive extraction, DOL analysis, splitting, linker script generation | `v1.8.3` |
| objdiff | Function and object diffing, progress report and denominator | `v3.6.1` |
| Metrowerks CodeWarrior compilers and linker | Original toolchain hypothesis for a Wii title | to be identified per library |
| GNU binutils for PowerPC EABI | Assembling any assembly | `2.42-2` |
| ninja | Build execution | unpinned |
| wibo or wine | Running Windows compilers on non-Windows hosts | wibo `1.0.3` |

## Build driver prerequisites

Before implementing a build driver, establish compilation-unit boundaries, generated inputs, linker layout, object order, library dependencies, asset transformations, and packaging requirements. Document timestamp, ordering, locale, and path effects that could prevent deterministic output.

## Selected build toolchain

Pinned in `config/toolchain.json`, with sources and digests:
- decomp-toolkit 1.8.4 x86 (F-0006)
- objdiff-cli 3.8.2
- GNU binutils 2.42-2
- the CodeWarrior bundle `compilers_20251118.zip`
- sjiswrap 1.2.2
- ninja 1.13.0

The linker is Wii/1.1 `mwldeppc.exe` with `mw_comment_version: 15` (F-0008). It reproduces `main.dol` (F-0010), but whether it is the original linker version is unproven.

Use these compilers per library (F-0007):

| Library | Compiler | Alignment |
| --- | --- | --- |
| RVL_SDK | Wii/1.0 | default |
| MSL_C and Runtime | Wii/1.1 or 1.3 | `-func_align 4` |
| NW4R | Wii/1.0 or 1.1, unresolved | default |
| Home Button Menu layout copy | Wii/1.0RC1 or 1.0a | default |

## Validated reference extraction tools

`dtk.exe` 1.8.4 (commit a0c455e46cab58e1d2e0885623f85089ff0499db), SHA-256 a89f8a1ab07cde5af83218da4247dfd535ad17f2fdb69bf361a966eec7b8fe25, supports this WAD, U8 archives, NLZSS, and DOL analysis. User supplied; download provenance remains unverified. Python 3.14.0 runs `tools/reference/extract_reference.py` from the repository root. Supporting LZH8/Huff8 source comes from Plombo/vcromclaim commit 6e537af6d314c3d8a6c7353645c137572b37b4b6, cloned into ignored tools/local. F-0005 records source hashes and limitations; [extraction](EXTRACTION.md) records exact invocation and compatibility adaptations. These are analysis/extraction tools, not evidence of the original game's compiler.
