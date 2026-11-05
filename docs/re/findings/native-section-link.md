# Native final-link section ordering

The compiler accepts __declspec(section ".descriptive_name") on function definitions and emits ordinary relocatable PowerPC code in that section. A final mwldeppc link can merge named input sections into a single .text output in linker-command-file order. Plain partial linking with -r ignores those placement rules; this distinction resolves the earlier section experiment limitation. A separate -r1 toy encountered ELF_gen.c line978 and is not an accepted pipeline.

The baseline main executable was linked twice with explicit .text input selectors, preserving the current object order. Both native links exited0; dtk elf2dol succeeded and the frozen main.dol verifier passed. No object instructions, relocations, section bytes or original hashes were changed. This proves the placement mechanism on the current baseline, not acceptance of a new source unit.

## Basename selector trap

The linker selects objects by basename even when the LCF selector contains a quoted full path. Listing the game main.o accidentally selected MetroTRK main.o too, moving its gdev_cc functions into the game range and changing .text size. Giving duplicate-basename inputs unique filenames through byte-identical copies restored exact output. These aliases change only input filenames; they do not alter object contents or conceal existing original-unit fallbacks.

The earlier evidence was under tools/local/session/section-main/unique.lcf, unique.log and verification.json, with repeated proof under section-main/repeat. Those scratch paths are absent on the current PC; the 2026-11-04 continuation independently repeated the baseline native-selector proof and accepted [CUIMenu](cuimenu-native-sections.md).

Production integration is now `tools/build/order_text_sections.py`, selected by the per-object `text_sections` option in `configure.py`. Ninja prepares unique byte-identical input aliases, a response file and native `.text` selectors, then invokes the pinned final linker. Custom names use the `.text.` family, and the generated objdiff unit enables `combine_text_sections` for analysis. No object content is changed. Two clean builds including CUIMenu reproduced the complete frozen DOL, ELF and report; see [validation](../../VALIDATION.md#24-function-checkpoint-and-native-section-linking). Function code, exception tables, metadata and data ownership remain separate acceptance requirements.
