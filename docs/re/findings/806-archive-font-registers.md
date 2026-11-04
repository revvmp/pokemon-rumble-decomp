# F-806-1: ArchiveFontBase remaining register allocation

Issue #806 owns the complete 25-function, 5,264-byte `ut_ArchiveFontBase.cpp` inventory. The existing split and source are described in [F-449-2](449-ut-resfont-archivefont.md). This investigation adds no reconstructed bytes and keeps the unit `NonMatching`.

## Baseline

The isolated issue worktree starts from main `a8697a5f2d0d42fef01bd094d20a2e1bb923ed42`. `python configure.py`, plain `.venv/Scripts/ninja`, and explicit frozen verification of `build/wpse-v1` each exit 0. The original object remains the link input for this incomplete unit. Direct objdiff reports `.text` 99.77203% and `.data` 100%; 24 functions compare at 100%, while `ConstructOpAnalyzeGLGR` at `0x80295FD0`, size `0x3C0`, compares at 98.75%.

## Bounded compiler probes

A prior readable accessor-based candidate with declarations grouped at the start of the function reproduces the native instruction order and extent, with 30 differing non-relocation instruction words. Its prologue and initial resource checks agree; the first register difference is at function offset `0x1BC`. The target keeps the sheet count in `r24`, the group-block pointer in `r29`, glyphs per sheet in `r31`, adjustment size in `r30`, and the adjustment-table pointer in `r22`. The candidate assigns those values to `r26`, `r30`, `r24`, `r22`, and `r31`, respectively. Differences remain through the final metadata stores.

Twenty-seven scratch combinations of `u16`, `u32`, and `int` return types for the inline sheet-count, glyph-count and set-count accessors preserve the same minimum of 30 differences. Five scratch conversions of successive accessor-constructor assignments into ordinary member initializers also leave the minimum unchanged. These experiments do not establish different target types or a reason to change the canonical header. No probe header or source has been substituted into the repository.

A separate 108-candidate scratch comparison varies only the local sheet count, glyph count, block count, adjustment size and remaining-size integer types in that same grouped-declaration candidate. It also reaches no fewer than 30 differing words. Combining the earlier grouped declarations with these local type changes therefore supplies no new acceptance evidence.

All probes use the established Wii/1.1 compiler and existing NW4R flags, with repository headers and the generated include directory. The comparison excludes only actual relocation sites and retains local branch displacements. No instruction patch, forced register, assembly replacement, artificial source padding, or compiler-flag change is accepted.

## Remaining work

Recover the source expression or lifetime structure that yields the native register allocation. After that, compare every allocated section, select the complete unit from source, check all 25 original addresses and extents, and repeat the full frozen DOL gate. The passing baseline build does not establish source reconstruction for this unit.

## Traps

2026-11-04 follow-up: 240 isolated declaration and identity-expression probes did not improve the current 99.291664% function. The matrix tested scalar types, reference-bound temporaries, identity bitwise/arithmetic operations, post-initialization assignments, and register storage hints for the sheet-count local. Both the current body and the prior glyph-count/adjustment-pointer type variant were tested. No source or flags were retained. Scratch driver: `tools/local/session/archive-font.py`.

Changing the accessor's scalar return type or using member-initializer syntax did not repair this candidate's register assignment. Preserve the existing canonical declarations unless independent evidence supports a change. The prior declaration-order and loop probes recorded in F-449-2 remain relevant; repeating them without a new structural hypothesis is unlikely to resolve the remaining allocation.

## Structural count-materialization probes, 2026-11-04

The next bounded scratch wave keeps main source and headers unchanged. Both the current body and the earlier `u16 glyphsPerSheet`/`u32 sizeAdjustTable`/integer RoundDown candidate were tested. No exact candidate was found or retained.

- Scalar wrappers with trivial/user copy constructors and empty destructors, using 16-bit, 32-bit and int storage: 24 candidates, unchanged best.
- Inline reference and pointer output parameters, with argument order and direct/zero-then-add assignment variants: 48 candidates, no improvement.
- Empty copy tags passed through an inline identity function: 36 candidates, no improvement.
- A named count load before accessor construction, using the evidenced typed glyph-group record: 16 candidates, worse instruction scheduling.
- memcpy/memmove scalar materialization from the field or an accessor-result local: eight candidates, worse.
- Scratch-only constructor caching of the count: ten candidates, no improvement.
- Scratch-only constructor output pointer: six candidates, one reached 99.395836% but introduced additional operand-order differences and did not recover the required count allocation. This partial artificial API change was discarded.
- Tiny identity helpers under individual propagation, dead-assignment, common-subexpression, lifetime, scalarization and tree-transformation pragmas: 24 candidates, no improvement.

These controls do not overcome the accessor constructor's cached load being represented as an inline temporary. The proposed wrappers are not recovered source and are not suitable to retain for partial fuzzy-score gains. Drivers and diagnostic objects use ignored `tools/local/session/archive-61-*`; only this findings record is changed.

A further36 scoped enum range/assignment controls for the sheet-count local did not improve99.291664%. Both the existing source and prior glyph-count/table-size type candidate were tested. No implementation change was retained; ignored driver archive-enum.py records these controls.
