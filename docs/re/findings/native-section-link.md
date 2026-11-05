# Native final-link section ordering

The compiler accepts __declspec(section ".descriptive_name") on function definitions and emits ordinary relocatable PowerPC code in that section. A final mwldeppc link can merge named input sections into a single .text output in linker-command-file order. Plain partial linking with -r ignores those placement rules; this distinction resolves the earlier section experiment limitation. A separate -r1 toy encountered ELF_gen.c line978 and is not an accepted pipeline.

The baseline main executable was linked twice with explicit .text input selectors, preserving the current object order. Both native links exited0; dtk elf2dol succeeded and the frozen main.dol verifier passed. No object instructions, relocations, section bytes or original hashes were changed. This proves the placement mechanism on the current baseline, not acceptance of a new source unit.

## Basename selector trap

The linker selects objects by basename even when the LCF selector contains a quoted full path. Listing the game main.o accidentally selected MetroTRK main.o too, moving its gdev_cc functions into the game range and changing .text size. Giving duplicate-basename inputs unique filenames through byte-identical copies restored exact output. These aliases change only input filenames; they do not alter object contents or conceal existing original-unit fallbacks.

The earlier evidence was under tools/local/session/section-main/unique.lcf, unique.log and verification.json, with repeated proof under section-main/repeat. Those scratch paths are absent on the current PC; the 2026-11-04 continuation independently repeated the baseline native-selector proof and accepted [CUIMenu](cuimenu-native-sections.md).

Production integration is now `tools/build/order_text_sections.py`, selected by the per-object `text_sections` option in `configure.py`. Ninja prepares unique byte-identical input aliases, a response file and native `.text` selectors, then invokes the pinned final linker. Custom names use the `.text.` family, and the generated objdiff unit enables `combine_text_sections` for analysis. No object content is changed. Two clean builds including CUIMenu reproduced the complete frozen DOL, ELF and report; see [validation](../../VALIDATION.md#24-function-checkpoint-and-native-section-linking). Function code, exception tables, metadata and data ownership remain separate acceptance requirements.

## Grouped source inputs

CModel proves a second native path: its four genuine compiler outputs are passed directly to the final linker in their exception/data ownership order. The per-object `link_source_order` setting names every input compiled through `partial_sources`, including the main source. The preparer writes each requested code section across the group's uniquely named inputs before advancing to the next section. It copies every input byte unchanged. The native partial-link object is still generated for objdiff; it is not used by the final executable for this group.

The real preparer CLI, native Wii/1.1 link, elf2dol and frozen verifier all passed using the four accepted CModel outputs. All1,392 aliases compared byte-for-byte to their selected inputs. Empty groups, duplicate members, a wrong first member and an invalid section name each failed through the real CLI. The normal production build then passed with CModel selected from source and no previously exact function regressions. The unit remains one progress unit; three additional raw inputs do not create extra source-coverage credit.

The rejected plain partial-link path reaches the pinned linker's internal LK_symbol.c2693 failure. Removing exception records does not restore earlier weak-owner records: an isolated attempt shrank the DOL by320 bytes and failed verification. Explicit source ownership and native input order preserve the records instead. See [the CModel evidence](329-cmodel-emission.md).
