# Reverse engineering workflow

## Investigate

Start with the identified target and frozen reference. Choose a bounded function, data structure, or subsystem. Record artifact digest, address or offset, boundaries, tool version, callers, callees, constants, and data references. Runtime addresses, section offsets, and file offsets are distinct; record their mapping.

Separate direct observations from hypotheses. Use neutral names while purpose remains unknown. Investigate calling convention, parameter passing, signedness, widths, stack layout, structure alignment, and global-data access before establishing shared interfaces. Do not make uncertain types into repository-wide dependencies.

## Reconstruct

Add supported implementations under `src/` and established declarations under `include/`. Explain constraints here or in linked findings, never in source comments or docstrings. Preserve compilation-unit and link ordering when they affect output.

Compile with the evidenced toolchain, compare instructions and associated data, then rebuild and verify the complete affected acceptance artifact. A local function match is a partial milestone. Record all assembly stubs, original-object dependencies, binary fallbacks, unresolved symbols, and uncertain boundaries.

## Diagnose a mismatch

Check release identity, unchanged reference, exact tool versions, flags, and deterministic settings first. Then inspect section sizes and addresses, symbol order, relocations, alignment, padding, constants, and instruction generation. Record reference and rebuilt digests, first differing file offset, and the narrowest confirmed cause. Do not guess compiler settings merely because they reduce a diff.

Runtime observations may help confirm behavior but never replace byte identity. Diagnostics may compare individual regions; acceptance must compare complete declared artifacts without normalization.

## Close the session

Update findings, symbols, inventory, and recovered architecture when knowledge changes. Update project progress and handoff with the exact commands, observed results, blockers, and next action. Keep permanent records concise and reproducible; leave raw outputs in ignored local storage.
