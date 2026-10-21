# F-763-2: Native loop mask-table base recovered

Target: `wpse-v1`, issue #763. This is partial reconstruction, not issue completion. The stream unit remains `NonMatching`, and the real link still selects its original object. No additional function, byte, or linked-source credit is claimed.

## Bounded source change and measured result

A local `const u8* masks = sBitMask` is used by the checked writer's whole-byte loop. The loop applies its same arithmetic shift and low-bit mask directly through this read-only table pointer. Final partial-byte paths and the other nine functions retain the existing `MaskBits` helper. This does not change the mask data, stream layout, interfaces, integer widths, compiler or optimization flags.

The explicit loop table base reproduces the original `lis/addi` pair in saved register `r31`, after its native save. The previous source put the loop's table in `r11` and used `r31` for the overflow start pointer. The function remains exactly 432 bytes. After independently resolving every one of its six HA/LO relocation sites to the frozen table at `0x80368118`, the original split object equals the frozen DOL's complete function. The previous source differs in 26 instruction words; the candidate differs in 23. The comparison retains every opcode and register field, including in relocation-bearing instructions.

Actual unitdiff reports the checked writer at 98.65741%, whole text at about 99.63%, and all nine other functions / 1,152 bytes at 100%. The nine-byte rodata is 100%. The compiled prefix containing those nine functions and the entire rodata remain byte-identical to the previous actual source object. The exact instruction-word comparison, rather than fuzzy percentage alone, establishes the bounded three-word improvement.

## Remaining mismatch

The remaining 23 differing words are loop register operands. The original keeps the overflow marker one in `r11` and stuffing zero in `r10`; the candidate uses `r9` and `r8`. The original uses `r8`/`r9` for mask/shift temporaries and `r12` for the incremented cursor, while the candidate assigns the available temporaries differently. The code after the loop, its branch structure, all function extents and its rodata are unchanged. Source selection must remain original until the complete instruction stream and real source-selected executable match.

## Negative probes to avoid repeating

All probes use the actual pinned unit compile command and complete unitdiff, preserving the other nine matches when evaluating a candidate. No flag, volatile storage, fixed register, assembly or opaque instruction sequence is retained.

| Probe family | Result |
| --- | --- |
| Const pointer declarations, declaration/initialization orders, equivalent mask operand orders | Same best 23-word mismatch |
| Explicit loop mask/shift/partial-byte locals, including function-scope reuse in both final paths | No exact candidate; ordinary copies fold to the same graph |
| Byte/free integer widths and signedness, including `int` versus SDK `s32`/`u32`, helper count types | Same graph or regressions |
| Direct byte operations, checked-byte macros, explicit overflow code and caller-scope reused cursor counters | No exact candidate; some shared-helper forms regress other functions |
| Overflow early returns, bool return values, const/read-only locals, scalar spellings, equivalent accumulated-count expressions | Same best graph or regressions |
| Helper definition order and read-only table-helper argument order | No exact candidate |
| For/while guards, increment-list placement, early final returns, final switches and a final-byte helper | Same graph or changed instruction layout |
| Meaningful local-name changes and register hints | No change to the best allocation |
| JPEG escaped-byte helper, used only in the checked writer's loop and/or final whole-byte path | Signed form preserves the same graph; narrower/unsigned forms regress it |

The earlier F-763-1 compiler-family and optimization diagnostics remain applicable and were not repeated here. Temporary candidates and raw comparison artifacts were kept outside the repository under issue-specific names.

## Validation

Baseline and candidate `python configure.py`, plain `.venv/Scripts/ninja`, and the explicit frozen verifier each exit 0. The prior DOL was removed before the candidate workflow, and verification followed a known successful Ninja completion. Actual `unitdiff.py TMC_JPEG/tmcjpeg_stream.c` exits 0 and reports the partial state above. The outside `763-word-audit.py` resolves all six actual HA/LO sites, proves the target object against the frozen DOL and measures 26 versus 23 differing words, exiting 0. These are diagnostics plus unchanged-fallback artifact verification; they do not establish source-selected acceptance.

## Traps

- **A higher fuzzy score still needs an instruction-word comparison.** The table-base candidate improves the fuzzy score substantially but fixes only three instruction words and no complete function. Resolve relocation operands independently and keep register fields in the comparison before describing progress.
- **A cached read-only table base can recover a saved register without resolving the loop graph.** For this function it recovers `r31` and the native prologue table loads, but the overflow constants and temporary colors remain wrong. Do not mark the unit Matching from that local result.

## Current-main continuation controls

A fresh continuation on current main independently reproduces the same nine exact functions, all22 resolved relocation sites and complete nine-byte table. The checked writer retains exactly23 instruction-word differences in its loop; its size and remaining instruction stream are unchanged. The established compiler options remain unchanged.

Returning the written byte from the existing inline WriteByte and consuming it in the loop does not recover the remaining graph: signed or unsigned32-bit forms preserve nine exact functions and the same23 differences; an eight-bit return adds a truncation and grows the checked routine to436 bytes. A shared checked/unchecked bit-writer body with an inline byte-output policy, evaluated with the recovered loop table pointer present, regresses the fast routine by13 words and the checked routine by47. These are outside diagnostic candidates only; no source or flag change is retained. Raw commands and results are preserved under `763-reference/763-return-byte-results.json` and the issue-specific shared-policy candidates.

The remaining work needs evidence for the original checked-writer source/inline lifetime structure. Current public-reference and native evidence establish the algorithm and instruction layout but do not identify that structure uniquely. Preserve the nine proven functions and the complete native table; do not substitute fixed registers, volatile state, guessed compiler flags or instruction bytes to close this gap.
