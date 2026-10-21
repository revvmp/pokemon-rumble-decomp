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

# F-764-3: Native source continuation control

The 2026-10-20 continuation rebased the unchanged converter sources onto `ca14c79b3104e852def3a3a03aeb12cfd10f613b`. The closest unmatched normal converter remains `ConvertRGBA8MCU422`, with its original 420-byte extent and three differing instruction words. Its temporary tile index uses `r23` instead of `r26` at function offsets `0x120`, `0x124` and `0x12C`; the other words match before relocation handling.

Eighteen additional source probes did not improve it. Equivalent spellings of `px + 1`, incrementing `px` between the two pixel writes, reversing the two texel declarations, and commutative or shift spellings of the tile expression retain the three differences. Replacing the two `u16` texel locals with `u32` or `int` increases the difference count to seven. Making the AR temporary signed leaves four differences. Using division instead of the signed coordinate shifts changes the function size. Factoring both tile coordinates before doubling increases the difference count. These were scratch compilations with the existing Wii/1.0 command and flags; no probe was adopted, no source coverage increased, and both units remain `NonMatching`.

The reproducible scratch evidence is `764-reference/764-followup-proof.json` outside the checkout. At the control revision, `python configure.py`, `.venv/Scripts/ninja` and the frozen-manifest verifier each exited 0 after deleting the old `build/wpse-v1/main.dol`. The rebuilt DOL SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This validates the unchanged fallback build, not native completion of these converters. No new project-wide trap was found. A future continuation should seek a different native source structure rather than repeat these coordinate spellings or force a register assignment.

## Traps

- **Edge converters match only with one declaration order.** Trigger: correct instructions, wrong `r7`–`r12` or `r28`–`r31`. Hill-climbing the declaration block found one order that matches all six. Do not reorder the declarations. Evidence: F-764-2.
- **Commutative operand order can come from register colouring.** Trigger: one `add rD, rA, rB` with swapped sources in `TMCJPEGSetupRGBA8`. Swapping the C operands also moves the `srwi` that feeds the `add`, so the remaining difference comes from register assignment, not operand order. Evidence: F-764-1.
- **decomp-permuter needs local fixes on Windows.** `src/preprocess.py` calls `cpp`, which is not installed. Feed it source already preprocessed with `mwcceppc -EP` and make `preprocess` return the file as is. Build `target.o` from the original function bytes with `powerpc-eabi-as`. Then ignore any score that comes only from relocations. Evidence: F-764-1.
