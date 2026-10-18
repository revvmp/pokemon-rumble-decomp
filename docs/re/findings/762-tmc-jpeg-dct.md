# F-762-2: Row-pass order matches both DCTs

The encoder is `Matching`. All fifteen functions of `TMC_JPEG/tmcjpeg_encoder.c` are identical to the original in objdiff (7,436/7,436 code bytes), and the full DOL verifies with the unit linked from source. This supersedes the remaining differences in F-762-1 below.

The ten-word remainder in each DCT's column pass was caused by the row pass. Register colouring of the column-pass temporaries depends on the order in which the shared temporaries are first assigned in the row pass, not only on their declaration order or on the column pass itself. The source that matches both functions is:

- Row pass: `tmp10`, `tmp11`, `tmp13`, `tmp12`, so both sums before both differences.
- Column pass: `tmp10`, `tmp13`, `tmp11`, `tmp12`, the order of the IJG fast DCT.
- Declarations in natural order (`work`, `tmp0`–`tmp7`, `tmp10`–`tmp16`, `z1`–`z13`, `p`, `i`), except that `tmp14` comes directly after `work` in both functions. With `tmp14` in its natural place the unsigned DCT still matches but the signed DCT differs in 15 words.

This replaced the earlier hill-climbed declaration orders with an ordered list, so the source is also more readable. The search used a scratch hill-climb over the declaration order and the statement orders of both passes, rejecting any move that changed which assignment each use reads. It reached zero from the old source in under 2,000 compiles; a second search then minimised the distance from the natural order while keeping zero differences. Earlier searches had held the row pass fixed, which is why they stalled at ten words.

## Validation

`python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exited 0 after deleting the old `main.dol`. `python tools/re/unitdiff.py src/TMC_JPEG/tmcjpeg_encoder.c` reports 15/15 functions at 100%.

## Traps

- **Row-pass statement order changes column-pass registers.** Trigger: a two-pass function with shared temporaries that has only register differences in its second loop, which no reordering of that loop fixes. Search the first loop's statement order as well. Evidence: F-762-2.

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
- An exhaustive scratch search of all 5,040 orders of the seven independent column assignments (`tmp14`, `tmp15`, `tmp16`, `tmp10`, `tmp13`, `tmp11`, `tmp12`) found no candidate below ten differing words in either DCT. Eight hundred temporary-declaration permutations likewise found no improvement. These results exclude only those measured source variations; they do not prove that another readable reconstruction cannot match.
- A later bounded check of documented optimizer controls did not improve the retained source. `noloopinvariants` preserves the ten-word remainder in both DCTs; `nocse` gives 67/68 differing raw words and also changes `SetupPlanes`; `nopropagation` gives 77/55; `nolifetimes` gives 110/119 and changes header/MCU functions. `-inline noauto` leaves both DCT remainders at ten but changes the MCU entry point; combining it with `nocse` retains both regressions. These are external whole-source diagnostic objects in `762-reference/documented-controls/evidence.json`, produced by `762-probe-optimizer-controls.py`. They are not accepted configurations. No repository flags or source changed, and the baseline still retains thirteen exact bodies plus the two disclosed ten-word remainders.
