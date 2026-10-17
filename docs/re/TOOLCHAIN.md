# Toolchain investigation

The original compiler, assembler, linker, runtime libraries, extraction tools, and packaging tools are unknown. No original toolchain version or flag is assumed.

Investigate format metadata, compiler signatures, code generation, runtime/library patterns, section layout, relocation behavior, and link ordering. Treat resemblance as a hypothesis until comparison supports it. Record evidence in `FINDINGS.md` and accepted requirements in `config/toolchain.json`.

For every selected tool record its exact version, acquisition source, binary SHA-256, host requirements, command invocation, working directory, flags, environment variables, and findings that justify its use. Record unresolved alternatives explicitly. Do not commit proprietary tool binaries.

## Candidate tools

No tool below is selected or acquired. These are candidates from [prior art](PRIOR_ART.md) to evaluate against our own evidence; versions are those Petari pinned at the reviewed commit, not requirements.

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
