# F-349-1: Quantization and bit-stream units

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #349. The compiler and flags are those of [F-348-1](348-tmc-jpeg.md): Wii/1.0 with `cflags_tmcjpeg`.

| Unit | `.text` | Data | Status |
| --- | --- | --- | --- |
| `TMC_JPEG/tmcjpeg_quant.c` | `0x801F8DC4`–`0x801F9134` | `.rodata` `0x80367A18`–`0x80368118` (seven `s32[64]` tables) | Matching, 1/1 |
| `TMC_JPEG/tmcjpeg_stream.c` | `0x801F9134`–`0x801F9764` | `.rodata` `0x80368118`–`0x80368121` (`sBitMask`) | NonMatching, 8/10 |

`TMCJPEGBuildQuantTables` points `ctx->zigzag` at the zigzag table. With `quantMode` 0 or 1, it scales the Annex K luminance and chrominance tables by `50 / quality` and clamps each value to 1–255. It then derives the reciprocal tables by dividing `sAANDivisor` (the AAN scale factors times 2^13) by each quantizer. Mode 2 only derives the reciprocals from caller-supplied quantizers. Any other mode copies the precomputed quality-50 tables. The second table, `sAANScale` (the same factors times 2^14), is never referenced, but it sits inside the unit's `.rodata` and is emitted by the compiler.

The stream unit is the byte writer of `TMCJPEGStream`. Every write checks whether `current - start` has reached `size`. When it has, the writer sets `overflow`, adds the bytes to `flushed` and rewinds `current`, so the buffer behaves as a sliding window. `TMCJPEGSelectPutBits` installs `PutBitsFast` (`0x801F94C8`, `scope:local`) while at least 256 bytes remain. Otherwise it installs the checked `TMCJPEGStreamPutBits`. Both stuff a zero after every `0xFF` byte. `TMCJPEGStreamPutMarker` and `TMCJPEGStreamPutEOI` first pad the partial byte with one-bits from `sBitMask`. `TMCJPEGStreamPutMarker` reuses `TMCJPEGStreamPutWord` inline.

Unit boundaries: the quantization tables are the only `.rodata` used by `TMCJPEGBuildQuantTables`, and `sBitMask` is used only by the stream functions. Both splits follow the `.text` order. A single file for both would link identically, so the boundary between them is unproven. All names are descriptive, medium confidence.

Remaining: `PutBitsFast` and `TMCJPEGStreamPutBits` have the target's instructions and size but differ in register assignment: 6 and 26 instructions, 98.7% fuzzy in the unit. In the target, the hoisted `sBitMask` base and the temporaries take other registers (`r8`/`r31` instead of `r9`/`r12`). None of these changed the assignment:

- reordering the declarations;
- adding `mask` and `shift` temporaries, or a local table pointer;
- writing the overflow check without its helper.

Moving the `bitsFree = 8` store after the mask load reduced the fast version from 10 to 6 differences.

# F-349-2: Colour converters (not yet split)

`0x801F9764`–`0x801FB030` holds `TMCJPEGSetupRGBA8` and `TMCJPEGSetupRGB565` with six converters each, in this order:

1. normal 4:1:1, 4:2:2 and 4:2:0;
2. edge 4:1:1, 4:2:2 and 4:2:0.

Converter addresses:

| Converter | RGBA8 | RGB565 |
| --- | --- | --- |
| Normal | `0x801F9850`, `0x801F9AC4`, `0x801F9C68` | `0x801FA510`, `0x801FA740`, `0x801FA8C4` |
| Edge | `0x801F9E18`, `0x801FA010`, `0x801FA20C` | `0x801FAA54`, `0x801FAC3C`, `0x801FAE28` |

Each setup function stores the pair in `state->convert`/`convertEdge` (`0xAC`/`0xB0`) and returns `-0x70` for an unknown sampling. It then rounds `enc->width`/`height` up to a multiple of 4 into `alignedWidth`/`alignedHeight`.

Each converter has the signature `(TMCJPEGMCUBuffer* buffer, TMCJPEGEncoder* enc, s32 x, s32 y)`. It writes Y to `buffer->pixels` and Cb/Cr to `+0x100`/`+0x140`, or to `+0x80`/`+0xC0` for 4:2:2, and returns 0. The conversion is:

```
Y = (77R + 150G + 29B + 128) >> 8
Cb = ((B - Y) * 144 + 128) >> 8
Cr = ((R - Y) * 183 + 128) >> 8
```

Chroma comes from the first pixel of each horizontal group, and for 4:2:0 only on even rows.

Texture addressing:

- RGBA8 texels come from 4×4 tiles of 32 halfwords: AR at `tile * 16 + (y & 3) * 4 + (x & 3)` and GB 16 halfwords later, with `tile = ((y >> 2) * (alignedWidth / 4) + (x >> 2)) * 2`.
- RGB565 tiles are 16 halfwords: R = `(p >> 8) & 0xF8`, G = `(p >> 3) & 0xFC`, B = `(p & 0x1F) << 3`.

The MCU span is `32 / enc->scale` or `16 / enc->scale` pixels wide and `8 / enc->scale` or `16 / enc->scale` rows high. The edge variants use `remainderX`/`remainderY` when `x`/`y` equals `paddedWidth`/`paddedHeight`. They replicate the last pixel to the MCU width and copy the last row down with `memcpy`. The `x`/`y` comparisons are unsigned (`cmplw`), and the stride uses a logical shift, so `paddedWidth`, `paddedHeight`, `alignedWidth` and `alignedHeight` are probably `u32`.

A draft of the RGBA8 4:1:1 converter reproduces the target's size (`0x274`) and instruction selection. About 140 instructions still differ in register assignment and scheduling, and neither 1,500 declaration orders nor 16 structural variants closed the gap. The converters therefore stay in the automatic split.

## Traps

- **A bool-returning range check reproduces a materialised flag.** Trigger: `TMCJPEGBuildQuantTables` sets a register to 0, conditionally to 1, then compares it with 0. An inline `IsValidQuant` that sets a `BOOL` local reproduces it; a direct `if (v >= 1 && v <= 255)` folds into one unsigned compare. Evidence: F-349-1.
- **Statement order assigns registers to inlined temporaries.** Trigger: the stream functions matched only after each byte write used `*stream->current++ = value` (no reload). Store-then-increment forms reload `current`. In `PutBitsFast`, moving one independent store changed four registers. Evidence: F-349-1.
- **An unused static table can still be emitted.** `sAANScale` is never referenced but occupies `.rodata` `0x80367B18`. Keep it in the source, or the following tables shift. Evidence: F-349-1.
