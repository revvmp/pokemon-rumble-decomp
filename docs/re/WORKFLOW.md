# Reverse engineering workflow

## Investigate

Start with the identified target and frozen reference. Choose a bounded function, data structure, or subsystem. Record artifact digest, address or offset, boundaries, tool version, callers, callees, constants, and data references. Runtime addresses, section offsets, and file offsets are distinct; record their mapping.

Separate direct observations from hypotheses. Use neutral names while purpose remains unknown. Investigate calling convention, parameter passing, signedness, widths, stack layout, structure alignment, and global-data access before establishing shared interfaces. Do not make uncertain types into repository-wide dependencies.

## Disassembler setup

Load `build/wpse-v1/main.elf` after a successful build rather than the DOL. It matches `main.dol` byte for byte, keeps section addresses, and carries every symbol from `config/wpse-v1/symbols.txt`. IDA and Ghidra load PowerPC ELF natively, so no DOL loader is needed. Third-party DOL loaders exist for IDA and Ghidra, but the ELF route avoids them.

| Setting | Value |
| --- | --- |
| Processor | 32-bit PowerPC, big-endian, Gekko/Broadway paired singles enabled; Ghidra language `PowerPC:BE:32:Gekko_Broadway` |
| r13 (`_SDA_BASE_`) | `0x804BDA20` |
| r2 (`_SDA2_BASE_`) | `0x804C1C60` |

The SDA values were read from the ELF symbol table with binutils 2.42-2 `powerpc-eabi-nm`. Hex-Rays decompilation needs the PowerPC decompiler license, and its handling of paired-single instructions is unverified. Use decompiler output only to understand code: rewrite it into idiomatic source under the fidelity standard in root `AGENTS.md` and prove it with objdiff and full verification. Sync confirmed names back into `config/wpse-v1/symbols.txt`, keeping address, size, section, and attributes intact; no round-trip script is established yet.

## Reconstruct

Add supported implementations under `src/` and established declarations under `include/`. Explain constraints here or in linked findings, never in source comments or docstrings. Preserve compilation-unit and link ordering when they affect output.

Compile with the evidenced toolchain, compare instructions and associated data, then rebuild and verify the complete affected acceptance artifact. A local function match is a partial milestone. Record all assembly stubs, original-object dependencies, binary fallbacks, unresolved symbols, and uncertain boundaries.

## Diagnose a mismatch

Check release identity, unchanged reference, exact tool versions, flags, and deterministic settings first. Then inspect section sizes and addresses, symbol order, relocations, alignment, padding, constants, and instruction generation. Record reference and rebuilt digests, first differing file offset, and the narrowest confirmed cause. Do not guess compiler settings merely because they reduce a diff.

Runtime observations may help confirm behavior but never replace byte identity. Diagnostics may compare individual regions; acceptance must compare complete declared artifacts without normalization.

## Close the session

Update findings, symbols, inventory, and recovered architecture when knowledge changes. Update project progress and handoff with the exact commands, observed results, blockers, and next action. Keep permanent records concise and reproducible; leave raw outputs in ignored local storage.
