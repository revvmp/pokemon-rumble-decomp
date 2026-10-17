# F-453-1: single-argument CharWriter font sizing

Issue #453's debug text renderer calls `nw4r::ut::CharWriter::SetFontSize(f32)` at `0x80299B90` when its configured size is nonnegative. The callee divides its argument by the font height from virtual slot `0x10` and stores that quotient to both scale components at `0x24` and `0x28`. This establishes uniform scaling to the requested font height, rather than setting the scale directly.

The declaration is present in both CC0 references: xbret/xenoblade `ea25691b2029394ab777bda7060139e517a13cba`, `libs/nw4r/include/nw4r/ut/ut_CharWriter.h`, and zeldaret/ss `d3d8e3cbf3d353b2231f06690965cac5366e28a1`, `include/nw4r/ut/ut_CharWriter.h`. Only that missing declaration is imported. The shared-header prerequisite is submitted separately as required by the collaboration rules. No source coverage is claimed for this declaration or the existing fallback callee.

## Validation

The pull request records configure, clean Ninja, and exact verification results for the committed header prerequisite against the latest main. The source implementation remains in the separate #453 worktree.

## Traps

A single font-size argument is a requested height. Treating it as a raw scale produces incorrect source behavior and replaces the observed call with two inline stores.
