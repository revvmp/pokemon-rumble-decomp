# Toolchain investigation

The original compiler, assembler, linker, runtime libraries, and packaging tools are unknown. Reference extraction tools are pinned below. No original toolchain version or flag is assumed.

Investigate format metadata, compiler signatures, code generation, runtime/library patterns, section layout, relocation behavior, and link ordering. Treat resemblance as a hypothesis until comparison supports it. Record evidence in `FINDINGS.md` and accepted requirements in `config/toolchain.json`.

For every selected tool record its exact version, acquisition source, binary SHA-256, host requirements, command invocation, working directory, flags, environment variables, and findings that justify its use. Record unresolved alternatives explicitly. Do not commit proprietary tool binaries.

## Candidate tools

Except for decomp-toolkit, now validated at 1.8.4 below, no tool below is selected or acquired. These are candidates from [prior art](PRIOR_ART.md) to evaluate against our own evidence; versions are those Petari pinned at the reviewed commit, not requirements.

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

## Validated reference extraction tools

`dtk.exe` 1.8.4 (commit a0c455e46cab58e1d2e0885623f85089ff0499db), SHA-256 a89f8a1ab07cde5af83218da4247dfd535ad17f2fdb69bf361a966eec7b8fe25, supports this WAD, U8 archives, NLZSS, and DOL analysis. User supplied; download provenance remains unverified. Python 3.14.0 runs `tools/extract_reference.py` from the repository root. Supporting LZH8/Huff8 source comes from Plombo/vcromclaim commit 6e537af6d314c3d8a6c7353645c137572b37b4b6, cloned into ignored tools/local. F-0005 records source hashes and limitations; [extraction](EXTRACTION.md) records exact invocation and compatibility adaptations. These are analysis/extraction tools, not evidence of the original game's compiler.
