# WUD importer alias matching

## F-504-16: Complete source-selected WUD candidate matches

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. This finding supersedes F-504-15's remaining importer mismatch. Compiler, flags, target metadata, layouts and original artifacts are unchanged.

The inlined `__wudGetNewDevInfo` helper now tests `p->syncType` through its existing control-block alias and retrieves companion information through `_wcb.smpListTail->devInfo`. These expressions refer to the same global object. Keeping the first alias and spelling the second access directly changes CodeWarrior's scheduling and register allocation: all 86 retained WUD functions, totaling 25,028 bytes, have exact non-relocation instructions at their original extents. All 1,185 paired relocations have identical positions, types, instruction operands and evidenced destinations. The importer contributes 57 of those entries.

The actual `all_source` object compiled successfully with Wii/1.0 and unchanged `cflags_sdk`. A scratch response for the real Wii/1.1 linker substitutes only this source WUD object for the original fallback. Linking and dtk `elf2dol` both exited 0. Its 4,094,880-byte DOL is identical to the frozen original at every byte. Explicit `tools/verify/verify.py verify` against the frozen manifest and this candidate directory exited 0. All 86 final native addresses agree. Both source-only message catalogs and the unretained standalone `WUDStartClearDevice` body are stripped.

The relocation-aware data audit independently confirms all 4,519 emitted data bytes, nine jump-table destinations and the final target alignment byte. Small data, BSS and small BSS retain their complete original layouts. All 340 baseline source objects remain byte-identical. Whole-unit objdiff is diagnostic: text 99.89692%, data 11.763171%, BSS/sdata/sbss 100%, sdata2 63.636364%; coarse data symbols, relocation matching and source-only objects do not replace the exact final comparison.

This initial candidate is based on the existing issue branch. Normal `Matching` selection and final acceptance still require the header prerequisite, rebase onto current main and the full configure/Ninja/frozen-verification merge loop. No linked WUD credit is claimed from the still-`NonMatching` normal configuration at this checkpoint.

## Rejected approaches

Removing the helper's control-block alias entirely also makes the importer exact, but changes 24 instruction words in virgin-standard synchronization and its inlined dispatcher copy. Reordering declarations, integer spellings, register hints, pointer qualifiers, caller scopes and standard-allocator parameter forms did not remove those regressions. Adding a helper parameter reduced this alternative to 20 differing words but remained a regression. Those scratch alternatives are not applied. Only the single direct companion-tail expression is retained.

## Traps

- **Equivalent global accesses can affect inlined register allocation.** When all source statements and extents agree but one caller has operand/scheduling differences, test the exact access spelling within the shared helper and audit every caller. Replacing every alias access fixed this importer while regressing two other callers; retaining the condition alias and changing only the companion-tail access reproduces all native bodies and the complete candidate DOL.
