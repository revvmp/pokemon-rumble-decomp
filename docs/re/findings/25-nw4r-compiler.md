# F-25-1: NW4R is compiled with Wii/1.1

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Question from issue #25: F-0007 left NW4R between Wii/1.0 and Wii/1.1 because none of its 126 jointly matched functions separated them.

## Result

`nw4r::lyt::AnimTransformBasic::Animate(unsigned long, nw4r::lyt::Pane*)` at `0x802AF8F0` (0x1DC bytes) is reproduced only by Wii/1.1 (Ver 4.3, build 151). Together with `Animate(unsigned long, nw4r::lyt::Material*)` at `0x802AFAD0`, which F-0007 showed requires Wii/1.1 or earlier, this selects Wii/1.1 at default 16-byte alignment for NW4R. This agrees with the NW4R EF, G3D, SND, and LYT banners (`0x4302_151`). Confidence: high for lyt; the other NW4R modules are assigned by their identical banner build code.

| Compiler | `Animate(Pane*)` `0x802AF8F0` | `Animate(Material*)` `0x802AFAD0` |
| --- | --- | --- |
| GC/3.0a5.2 | no (492 bytes) | no (764 bytes) |
| Wii/1.0RC1, 1.0a, 1.0 | no, two instructions swapped | match |
| Wii/1.1 | match | match |
| Wii/1.3, 1.5, 1.6, 1.7 | no (472 bytes) | no (736 bytes) |

A match means every instruction word without a relocation is identical at that address and the function occurs exactly once in `.init` and `.text`; 113 of 119 words are compared for `Animate(Pane*)`.

## Discriminator

In the pane-visibility loop, the original stores the flag byte and then advances its two induction variables in this order: `addi r23,r23,1` before `rlwinm`, then `addi r27,r27,4` after `stb`. Wii/1.0 and earlier schedule `addi r27,r27,4` first and `addi r23,r23,1` after `stb`. Code size and register allocation are otherwise identical, so this is a scheduler difference between builds 145 and 151, not a source difference.

Petari's source differs from the target in one place: the target reads and writes the pane flag byte at offset 187, Petari's `Pane` places it at 183. The experiment added one four-byte pointer member after `mGlbMtx` in a scratch copy of the header to reproduce offset 187. That member's name and type are not evidence of the original layout; only its size is. Before the header change, the same scheduling difference was present, so it does not depend on the layout edit.

## Method

- Source: CC0 Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, `src/nw4r/lyt/lyt_animation.cpp` (blob `3ed5a57c`) and `libs/nw4r/include/nw4r/lyt/pane.h` (blob `a3b26300`, with the scratch member above).
- Flags, from F-0007's NW4R set: `-nodefaults -proc gekko -align powerpc -enum int -fp hardware -Cpp_exceptions off -O4,p -nosyspath -RTTI off -enc SJIS -pragma "ppc_no_fp_blockmove on" -inline auto -pragma "cats off" -pragma "warn_notinlined off" -fp_contract off -str reuse -ipa file`, with Petari's library include paths.
- Compilers: `tools/local/compilers/Wii/1.0/mwcceppc.exe` (SHA-256 `88b1cb54…3e71`), `Wii/1.1/mwcceppc.exe` (`14f99570…fbe5`), and Wii/1.0RC1, 1.0a, 1.3, 1.5, 1.6, 1.7 and GC/3.0a5.2 from the same bundle.
- Comparison: F-0007's relocation-masked search of `orig/wpse-v1/main.dol` (Python 3.14.2), plus GNU objdump 2.42 disassembly of the original range and of each object, normalized for whitespace and branch targets.

The six masked words are the two runtime save/restore calls, the curve-evaluation calls, and the `.sdata2` float load; they carry relocations in every build.

## Consequences

- New NW4R library entries in `configure.py` should use Wii/1.1 with the NW4R flags above and default function alignment.
- Inline and IPA settings were not varied. If an NW4R unit fails to match under Wii/1.1, test flags before reconsidering the compiler.
- The Home Button Menu's private `nw4hbm` lyt and ut copy is a separate library with the HBM banner `0x4201_142`; this result does not apply to it.

No follow-up decomp issues were needed: issues #375 to #462 already scope every NW4R block.

## Traps

- **`git pull --ff-only` can fail with "Cannot fast-forward to multiple branches".** In the main checkout this happened with no local changes. `git pull --ff-only origin main` fast-forwarded normally.
