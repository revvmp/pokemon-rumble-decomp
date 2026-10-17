# WUD importer alias matching

## F-504-16: Complete WUD source matches the frozen executable

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. This finding supersedes F-504-15's remaining importer mismatch. Compiler, flags, target metadata, layouts and original artifacts are unchanged.

The inlined `__wudGetNewDevInfo` helper now tests `p->syncType` through its existing control-block alias and retrieves companion information through `_wcb.smpListTail->devInfo`. These expressions refer to the same global object. Keeping the first alias and spelling the second access directly changes CodeWarrior's scheduling and register allocation: all 86 retained WUD functions, totaling 25,028 bytes, have exact non-relocation instructions at their original extents. All 1,185 paired relocations have identical positions, types, instruction operands and evidenced destinations. The importer contributes 57 of those entries.

The actual `all_source` object compiled successfully with Wii/1.0 and unchanged `cflags_sdk`. A scratch response for the real Wii/1.1 linker substitutes only this source WUD object for the original fallback. Linking and dtk `elf2dol` both exited 0. Its 4,094,880-byte DOL is identical to the frozen original at every byte. Explicit `tools/verify/verify.py verify` against the frozen manifest and this candidate directory exited 0. All 86 final native addresses agree. Both source-only message catalogs and the unretained standalone `WUDStartClearDevice` body are stripped.

The relocation-aware data audit independently confirms all 4,519 emitted data bytes, nine jump-table destinations and the final target alignment byte. Small data, BSS and small BSS retain their complete original layouts. All 340 baseline source objects remain byte-identical. Whole-unit objdiff is diagnostic: text 99.89692%, data 11.763171%, BSS/sdata/sbss 100%, sdata2 63.636364%; coarse data symbols, relocation matching and source-only objects do not replace the exact final comparison.

## Normal Matching build

After header prerequisite #720 merged, a fresh normal build selects WUD as `Matching`. `python configure.py --build-dir build/504-wud-matching-current`, full Ninja and explicit frozen verification of `build/504-wud-matching-current/wpse-v1` all exit 0. The actual link inputs select source EUART, USB and WUD objects and exclude their original fallback objects. Their 103 retained functions total 31,264 native bytes, including 86 WUD functions / 25,028 bytes. All 86 WUD final native addresses agree, and the source-only catalogs and standalone clear-device body are absent.

The raw instruction, relocation and initialized-data audit passes for the newly compiled WUD object. All 367 pre-existing `all_source` output objects remain byte-identical to the verified header-prerequisite baseline. This baseline is captured from Ninja's actual source targets rather than a recursive count of possibly stale files. Frozen acceptance compares all 4,094,880 DOL bytes and the sealed reference digest without normalization.

This WUD unit acceptance does not by itself complete #504. The original issue also includes thirteen KPAD functions supplied by #710; final issue inventory and latest-main verification follow that prerequisite. Neighboring #505-#507 acceptance audits add no duplicate unit-byte credit.

## Rejected approaches

Removing the helper's control-block alias entirely also makes the importer exact, but changes 24 instruction words in virgin-standard synchronization and its inlined dispatcher copy. Reordering declarations, integer spellings, register hints, pointer qualifiers, caller scopes and standard-allocator parameter forms did not remove those regressions. Adding a helper parameter reduced this alternative to 20 differing words but remained a regression. Those scratch alternatives are not applied. Only the single direct companion-tail expression is retained.

## Traps

- **Equivalent global accesses can affect inlined register allocation.** When all source statements and extents agree but one caller has operand/scheduling differences, test the exact access spelling within the shared helper and audit every caller. Replacing every alias access fixed this importer while regressing two other callers; retaining the condition alias and changing only the companion-tail access reproduces all native bodies and the complete candidate DOL.
