# F-762-1: Encoder column-pass temporary scheduling

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #762 owns all fifteen functions of `TMC_JPEG/tmcjpeg_encoder.c`, `.text` `0x801F6C5C–0x801F8968`, 7,436 code bytes. The established split and Wii/1.0 `cflags_tmcjpeg` configuration from [F-348-1](348-tmc-jpeg.md) are unchanged.

The encoder remains `NonMatching`. This change contributes no additional linked source bytes and does not complete #762. The original split object still supplies the entire unit to the final link.

## Retained source and remaining differences

Computing the second even pair (`tmp11`, `tmp12`) before the first pair (`tmp10`, `tmp13`) in each DCT's column pass reduces its differing native instruction words from thirteen to ten. The arithmetic expressions, stores, function extents, and first passes remain unchanged.

| Function | Original address | Native extent | Differing words |
| --- | --- | --- | --- |
| `TMCJPEGForwardDCT` | `0x801F855C` | `0x1F8` | 10 |
| `TMCJPEGForwardDCTSigned` | `0x801F8754` | `0x214` | 10 |

The other thirteen functions retain exact non-relocation instruction streams. All fifteen source function sizes equal their original extents. All 121 code relocations retain their offsets, kinds, target names and addends. Neither object has allocated data outside `.text`.

The remaining differences are in column-pass even-part scheduling and the physical registers used for `tmp13` and `tmp11`. In the unsigned function, the original assigns these values to `r29` and `r11`; the candidate assigns them to `r11` and `r29`. Exact raw comparison still rejects both DCT bodies. Objdiff scores are 96.5873% and 96.766914%, respectively; fuzzy scoring is a separate diagnostic from the differing-word count.

## Validation

On base `8135db476759709280e0e680a3c25ccfb63fd8ff`, a fresh `python configure.py --map`, plain `.venv/Scripts/ninja`, and explicit frozen-manifest verification all exited 0. The actual Ninja-built encoder object was checked separately because the unit remains `NonMatching`. All fifteen native extents, all 121 paired relocations, the thirteen retained exact bodies and both ten-word remainders passed the bounded audit. The full verifier checks the source-linked executable with this unit's disclosed original fallback; it does not establish that either DCT matches.

## Traps

- Wii `0x4201_127`, `1.0RC1`, `1.0`, `1.0a`, `1.1`, `1.3`, `1.5`, `1.6` and `1.7` produce the same thirteen-word baseline and ten-word reordered candidate in these two functions. Compiler revision changes do not fix this observed difference.
- Reordering the four independent even-part assignments can affect allocation even when moving the complete odd-part block does not. Both outcomes were measured on compiled objects; do not infer an exact match from algebraic equivalence.
- Separate column-scope temporaries, simple destructive temporary reuse, `register` hints, `int` versus `s32` temporaries, and fresh column pointers or counters did not produce exact bodies. Scratch variations remain outside the repository. Further investigation must preserve all thirteen existing exact functions and prove the entire source-linked unit before changing its status.
