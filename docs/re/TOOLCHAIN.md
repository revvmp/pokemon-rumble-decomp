# Toolchain investigation

The original compiler, assembler, linker, runtime libraries, extraction tools, and packaging tools are unknown. No original toolchain version or flag is assumed.

Investigate format metadata, compiler signatures, code generation, runtime/library patterns, section layout, relocation behavior, and link ordering. Treat resemblance as a hypothesis until comparison supports it. Record evidence in `FINDINGS.md` and accepted requirements in `config/toolchain.json`.

For every selected tool record its exact version, acquisition source, binary SHA-256, host requirements, command invocation, working directory, flags, environment variables, and findings that justify its use. Record unresolved alternatives explicitly. Do not commit proprietary tool binaries.

Before implementing a build driver, establish compilation-unit boundaries, generated inputs, linker layout, object order, library dependencies, asset transformations, and packaging requirements. Document timestamp, ordering, locale, and path effects that could prevent deterministic output.
