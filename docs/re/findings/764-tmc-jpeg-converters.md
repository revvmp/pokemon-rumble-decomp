# F-764-1: Colour converter units

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #764. The compiler and flags are those of [F-348-1](348-tmc-jpeg.md): Wii/1.0 with `cflags_tmcjpeg`. Wii/1.0a, 1.0RC1, 1.1, 1.3, 1.5 and 1.7 give the same code for every draft below; GC/3.0a5.2 does not.

| Unit | `.text` | Data | Status |
| --- | --- | --- | --- |
| `TMC_JPEG/tmcjpeg_rgba8.c` | `0x801F9764`–`0x801FA424` | none | NonMatching, 4/7 |
| `TMC_JPEG/tmcjpeg_rgb565.c` | `0x801FA424`–`0x801FB030` | none | NonMatching, 3/7 |

Each unit holds one setup function and the six converters it installs, in this order: normal 4:1:1, 4:2:2 and 4:2:0, then edge 4:1:1, 4:2:2 and 4:2:0 (F-349-2). The converters reference no data and are reached only through the pointers the setup function stores, so they are `static` (`scope:local`). The block ends at `0x801FB030`, where FA `pf_clib.c` begins. Splitting the block into one file per texture format is not proven. A single file would link identically. The two halves mirror each other, though, and each setup function refers only to its own converters. All file, function and macro names are descriptive, medium confidence.

`TMCJPEGEncoder` fields `paddedWidth`, `paddedHeight`, `alignedWidth` and `alignedHeight` are now `u32`. The edge converters compare `x`/`y` with the padded sizes using `cmplw`. The texel stride is `alignedWidth / 4` computed with `srwi`/`rlwinm`. The other TMC JPEG units compile to identical objects with the new types. The converters read the texture through `enc->source` (`0x64`).

Function state, counted as differing instructions with registers compared and, in brackets, with registers ignored:

| Function | RGBA8 | RGB565 |
| --- | --- | --- |
| Setup | 15 (2) | 15 (2) |
| Normal 4:1:1 | 75 (35) | 132 (10) |
| Normal 4:2:2 | 3 (0) | 100 (8) |
| Normal 4:2:0 | identical | 18 (4) |
| Edge 4:1:1, 4:2:2, 4:2:0 | identical | identical |

Remaining: every unmatched function has the target's size and instruction selection; the bracketed counts are scheduling differences, which change with the register assignment in every experiment so far. The setup functions differ only in the alignment arithmetic. `(w / 4 + (w % 4 != 0)) * 4` for both sizes gives 15 differences. Writing the height term as `((h % 4 != 0) + h / 4) * 4` reproduces the schedule, but the final `add` then takes its operands in the wrong order. Of 1,600 operand-order, shift and comparison variants, plus inline helpers taking `u16`, `u32`, `s32`, `int` or `u8`, none closed that last instruction. The normal converters differ only in which temporaries take which registers. decomp-permuter reached zero for RGBA8 4:2:2 and RGB565 4:2:0, but only by adding a temporary for one of two identical index subexpressions. That is not plausible source, so it was not adopted.

# F-764-2: Code generation evidence

These findings hold for this library's converters and reproduce the edge converters exactly.

- **Texel addresses are macro expressions, not a tile variable.** An edge converter that stores `((y >> 2) * stride + (x >> 2) * 2)` in a local differs in six instructions. The same expression written inline in each load matches. In the normal converters the compiler then lifts `image + (y & 3) * 8` and `(y >> 2) * stride` out of the column loop, which is the target's row-loop prologue. Assigning those to row variables gives a different schedule.
- **Long-lived locals are coloured in declaration order.** Variables needing callee-saved registers take `r31` downward in the order they are declared. Those that do not take volatile registers upward from the first free one. One declaration order (`dstY`, `image`, `r, g, b`, `px`, `dstCb`, `width`, `dstCr`, `xEnd`, `height`, `yEnd`, `i`, the texel temporaries, `tileStride`) matches all six edge converters. The normal converters need `image` before `dstY` and `px` after `dstCr`; this matches RGBA8 4:2:0.
- **The normal converters write the row skip inline.** `dstY += 16 - width` and `dstCb += (16 - width) >> 1` reproduce the target's volatile `r0`/`r3` skip values. Named `skip` variables take callee-saved registers instead.
- **Y-only pixels read the texel inside the luma expression.** Separate `ar`/`gb`/`r`/`g`/`b` assignments for the second to fourth pixels cost three to six more register differences in 4:2:2.
- **RGB565 channels go through `u8`.** The target keeps two dead `srawi` instructions per pixel, from `texel >> 8` and `texel >> 3`. They appear only when the channels are `u8` and the shifts are cast: `(u8)(texel >> 8) & 0xF8`, `(u8)(texel >> 3) & 0xFC` and `(u8)(texel << 3)`. With `s32` channels the dead shifts disappear and the pixel is one or two instructions short.

## Traps

- **Edge converters match only with one declaration order.** Trigger: correct instructions, wrong `r7`–`r12` or `r28`–`r31`. Hill-climbing the declaration block found one order that matches all six. Do not reorder the declarations. Evidence: F-764-2.
- **Commutative operand order can come from register colouring.** Trigger: one `add rD, rA, rB` with swapped sources in `TMCJPEGSetupRGBA8`. Swapping the C operands also moves the `srwi` that feeds the `add`, so the remaining difference comes from register assignment, not operand order. Evidence: F-764-1.
- **decomp-permuter needs local fixes on Windows.** `src/preprocess.py` calls `cpp`, which is not installed. Feed it source already preprocessed with `mwcceppc -EP` and make `preprocess` return the file as is. Build `target.o` from the original function bytes with `powerpc-eabi-as`. Then ignore any score that comes only from relocations. Evidence: F-764-1.

# F-764-5: bounded physical tile and channel contracts

A fresh native continuation retains both converter units unchanged. A private typed GX RGBA8 tile with sixteen AR halfwords followed by sixteen GB halfwords expresses the proven 64-byte texture format directly, but using that tile in the actual converters regresses the normal and edge instruction comparisons. This control is rejected; no typed view or layout is published. Raw controls are outside at `C:/MassStorage/GitHub/764-native-tiles`.

Making RGBA8 channel locals unsigned bytes, consistent with the neighboring RGB565 channel types and their proven 0–255 range, leaves the existing closest converter graph unchanged. It does not repair the three native register-word differences at offsets0x120/0x124/0x12C. Raw controls are `C:/MassStorage/GitHub/764-byte-channels`; the actual current entry-point comparison is `764-422-current.diff` in its parent directory.

The predecessor's outside `764-reference` already contains texel-accessor functions, luma helpers, first-pixel ordering and channel-order controls beyond the eighteen controls summarized in draft PR1024. Inspect those artifacts before repeating these families. That draft remains separate and unchanged. No source, compiler, header, split, selector, or symbol changes are adopted, and no new source coverage or original-issue completion is claimed.

# F-764-6: selective scheduling controls under D-0016

2026-11-04: the selective volatile-byte method that completed FA's MBR validator does not transfer directly to these converters. Direct compiles use the unchanged Wii/1.0 TMC flags and private output objects. No TMC source change was retained.

All sixteen RGB565 4:1:1 and four 4:2:2 texel-read volatile masks failed to improve their baseline. All 32 RGBA8 4:1:1 masks selecting the first AR/GB reads and the three later AR reads also failed to improve; duplicated GB reads inside the luma macro were deliberately not made volatile, because that would add observable accesses. Volatile aligned-width and aligned-height destinations in both setup functions leave the one swapped-add mismatch unchanged.

Selective `(u64)` casts on existing RGB565 texel values improve 4:1:1 from 89.22143% to 91.307144% and 4:2:2 from 89.25773% to 89.824745%, but do not match. Permuting first-pixel RGBA8 AR/GB declaration and load order with signed/unsigned 64-bit casts does not improve its baseline. Expanding later red-channel index expressions and adding redundant casts to their row products reaches only 77.64331% from 77.57962%. These partial candidates remain scratch only.

Setup's two-argument inline sum helpers, across scalar widths, argument order, addition order and scaling inside/outside the helper, do not fix the swapped add. An additional sweep disables each of the compiler's 82 discovered `opt_*` names individually around Setup. Three require numeric arguments and fail compilation; no successful candidate improves 99.830505%. Failed compilations are excluded from binary conclusions. Scratch scripts and objects are under ignored `tools/local/session/`, prefixed `rgb565`, `rgba8`, or `tmc-setup`.

## Traps

- **A scheduling fake that fixes one byte-reading loop need not help another.** Selective volatile qualification solved the FA validator but leaves these TMC loops unchanged or worse. Keep the search bounded and retain no merely higher fuzzy score when the source construct is artificial.

## F-764-7: additional setup-only controls

The setup functions remain unchanged. The RGBA8 setup's one swapped `add` source pair at offset `0xD0` survives 467 additional source controls: 112 pointer/output-parameter/struct sum helpers, 120 unused-argument identity helpers, 64 intermediate destination stores and typed locals, 75 dependent neutral or conditional expressions, and 96 compiler rotate/mask forms. None exceeds the existing 99.830505% match. Candidate functions compile through the unchanged Wii/1.0 command, and actual object comparisons exclude failed compilations. The RGB565 setup has the same source alignment expressions and same remaining instruction discrepancy; no gain is claimed for either unit. Scratch evidence is under `tools/local/session/764-setup-vm`, with its `764-setup-*` driver scripts. No tracked source, header, flag, symbol, or selector change is retained.

## F-764-8: RGB565 normal 4:2:2 exact source

2026-11-04: the normal 4:2:2 converter now reproduces all 388 native bytes. The first pixel's blue extraction retains an explicit redundant `& 0xFF` after its unsigned-byte cast. The second pixel loads its halfword into a separate signed 32-bit `nextTexel`, declared before the image pointer, instead of reusing the unsigned-halfword temporary. Both controls are needed: removing the mask drops the function to 89.824745%. The mask is equivalent to the already bounded byte conversion, and the signed 32-bit temporary holds only values loaded from a `u16`, so neither changes values or behavior.

This D-0016 correction is retained only in `src/TMC_JPEG/tmcjpeg_rgb565.c`. Direct object comparison reports normal 4:2:2 at 100% and preserves all prior exact functions; Setup and normal 4:1:1 remain unresolved. The unit remains NonMatching pending completion and a full source-link check. Evidence is ignored `tools/local/session/rgb565-compile/422-final.c` and `422-final.json`.

The restored C decomp-permuter exposed the useful redundant narrowing mask; a targeted texel-type sweep then completed the function. The same mask on the first two normal 4:1:1 pixels and separate signed texels for its following three pixels reaches 99.71429%, with native size/opcodes and only the second pixel's texel/blue registers exchanged. That partial is scratch only. Its 48 type/expression and 80 local-declaration controls did not complete it; continued randomization starts from that near-exact state.

The old ignored `tools/local/perm/dp` dependency was missing. The repository was restored from the upstream decomp-permuter, and a private native compiler wrapper avoids shell/preprocessor issues while preserving exact TMC flags. Random mutation can introduce side effects or narrow live arithmetic incorrectly; such intermediate candidates are not valid source gains. Split-assignment, AST deletion, external/function type mutation and variable-reuse passes were disabled for the refined search, and every proposed result is recompiled inside the complete original unit before acceptance.

## F-764-9: RGB565 normal 4:1:1 exact source

The remaining normal 4:1:1 converter now reproduces all 560 native bytes. Three separately declared signed 32-bit texel locals hold its second, third, and fourth pixels. The first and second blue-byte conversions retain redundant `& 0xFF` masks after their unsigned-byte casts. The final register correction reuses the second texel local for the first pixel's blue conversion: assign the first unsigned-halfword texel to that signed local, then form blue from it. This changes compiler temporary coalescing without changing values or adding native instructions.

The native C permuter found the last assignment from the 99.71429% candidate. Its unrelated `3 & 0xFFu` mutation was removed, the locals were named `secondTexel`, `thirdTexel`, and `fourthTexel`, and the full source unit was compiled again. `411-retained.json` reports every RGB565 converter at 100%; only Setup remains 99.830505%. No previously exact function regresses. The corrected function is retained in the same source file; the unit remains NonMatching until Setup is completed and its complete source link passes the frozen SHA.

The masks, mixed-width texel views, and extra value copy are recorded under D-0016. All signed texel values originate in unsigned-halfword loads, so the additional views preserve the 0�65535 range. The exact winning permuter evidence is `tools/local/session/dp61-ConvertRGB565MCU411/output-0-1/`; the cleaned whole-unit evidence is `tools/local/session/rgb565-compile/411-retained.c` and `411-retained.json`.

Reusable lesson: redundant masks after a narrowing cast can preserve a distinct compiler operation even when no additional machine instruction remains. Separately typed temporaries and an earlier assignment to a reused temporary can then change register coalescing. Inspect actual remaining opcodes before treating the large initial fuzzy-score gap as missing behavior: both RGB565 loops become register-only after the masking correction.

## F-764-10: remaining RGBA8 normal 4:1:1 controls

After the RGB565 gains, the maintainer completed two clean builds with identical DOL, ELF and report outputs and a frozen-hash verification PASS. Both RGB565 converter gains remain retained; neither whole converter unit is marked Matching while its remaining functions are unfinished.

The RGBA8 normal 4:1:1 converter improves from 77.57962% to 88.01911% in private scratch, retaining its native 628-byte size. Reusing the existing halfword `ar` for the second pixel's red load and third pixel's green/blue load repairs the first pixel's complete load schedule. Independent arithmetic-term orders then improve the remaining pixels. This partial is not retained. Evidence is `tools/local/session/rgba8-compile/411-orders.c` and `411-orders.json`; the smaller structural step is `411-reusebest.c`.

The continuation also tested 256 green-channel narrowing combinations, 108 independent texel-scalar variants, 343 halfword-reuse combinations, 100 mixed-width/reordered first-read controls on the strongest reuse form, 512 byte-mask combinations, 216 arithmetic-term permutations, 85 fourth-coordinate placements, 324 inline luma helper forms, 144 scoped channel-local forms, 512 existing-channel reuse combinations, and the compiler's discovered individual optimization switches. These did not produce an exact function. Ordinary nested loops for the three remaining pixels generate a 440-byte loop instead of the native unrolled body; enabling loop-unroll controls does not recover the target. Failed numeric pragma compilations are not treated as comparisons.

A private native permuter continues from the 88.01911% candidate and the setup function's single swapped-add state. The last recorded RGBA8 permuter score is 1590 from a 1615 baseline, a search metric rather than an objdiff acceptance percentage. No partial RGBA8 candidate, setup change, shared-header change or compiler-setting change is retained.

## F-764-11: setup cast-lifetime follow-up

A fresh isolated setup pass tested the recently successful local-enum and same-type reassignment mechanisms. Caching height as u16 and reassigning it through a u16 cast leaves the one swapped add unchanged. Separately typed quotient/remainder locals, enum quotient/remainder types, and casted u8/u16 intermediates regress scheduling. Redundant masks on the bounded quotient and Boolean carry likewise either preserve the original 99.830505% result or move instructions. Algebraic double-negation/complement forms do not recover the target add operands.

The mismatch remains exactly add r0,r0,r5 versus add r0,r5,r0 at setup offset0xD0, with the rest of the native instruction sequence identical. These controls use only the current RGBA8 setup expression and do not alter any converter. The parallel RGB565 setup uses the same alignment expression and identical mismatch; no speculative source update was applied to either file. Evidence is tools/local/session/jpeg-ppu and the jpeg-ppu-lifetime.py/masks.py drivers.
