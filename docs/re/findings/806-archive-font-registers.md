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

Changing the accessor's scalar return type or using member-initializer syntax did not repair this candidate's register assignment. Preserve the existing canonical declarations unless independent evidence supports a change. The prior declaration-order and loop probes recorded in F-449-2 remain relevant; repeating them without a new structural hypothesis is unlikely to resolve the remaining allocation.
