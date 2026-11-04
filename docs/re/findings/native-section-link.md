# Native final-link section ordering

The compiler accepts __declspec(section ".descriptive_name") on function definitions and emits ordinary relocatable PowerPC code in that section. A final mwldeppc link can merge named input sections into a single .text output in linker-command-file order. Plain partial linking with -r ignores those placement rules; this distinction resolves the earlier section experiment limitation. A separate -r1 toy encountered ELF_gen.c line978 and is not an accepted pipeline.

The baseline main executable was linked twice with explicit .text input selectors, preserving the current object order. Both native links exited0; dtk elf2dol succeeded and the frozen main.dol verifier passed. No object instructions, relocations, section bytes or original hashes were changed. This proves the placement mechanism on the current baseline, not acceptance of a new source unit.

## Basename selector trap

The linker selects objects by basename even when the LCF selector contains a quoted full path. Listing the game main.o accidentally selected MetroTRK main.o too, moving its gdev_cc functions into the game range and changing .text size. Giving duplicate-basename inputs unique filenames through byte-identical copies restored exact output. These aliases change only input filenames; they do not alter object contents or conceal existing original-unit fallbacks.

Evidence is under tools/local/session/section-main/unique.lcf, unique.log and verification.json, with the repeated proof under section-main/repeat. The reusable scratch runner native-section-link.py obtains native inputs from ninja, applies optional compiled-object and section-order overrides, creates unique aliases where necessary, links with the original compiler toolchain, converts the ELF and invokes the frozen verifier. Production integration awaits full acceptance of an actual source unit and clean repeat verification. Function code, exception tables, metadata and data ownership must all match.
