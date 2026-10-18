# F-539-3: Source-linked wide floating-point scanner

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. This completes the two functions left open by [F-539-1](539-msl-wide-math.md#remaining). The final acceptance inventory covers all forty original issue rows: 22,656 instruction bytes in 27 source-selected units. The new unit contributes two ordinary C functions and 4,960 instruction bytes; the previously completed 38 functions receive no new source credit.

## Boundary and interfaces

| Section | Native range | Evidence |
| --- | --- | --- |
| `.text` | `0x80347904`-`0x80348C64` | Scanner `0x12A8` bytes and wrapper `0xB8` bytes, ending immediately before `__wcstoul` |
| `.rodata` | `0x803989A0`-`0x803989B0` | Nine-byte `INFINITY` literal followed by seven zero alignment bytes |
| `.sdata2` | `0x804BB6B8`-`0x804BB6D8` | Five-byte `NAN(` literal, three alignment bytes, then zero, DBL_MIN and DBL_MAX |

The filename `MSL_C/wstrtold.c` is descriptive and follows the existing MSL scanner organization; no original filename string was recovered. `__wcstold` is inferred from the established `__strtold`/`__wcstoul` interfaces and the target's wide reader ABI. Its callback returns and accepts `wint_t`; the scanner retains `wchar_t c` and returns a 64-bit floating-point value. The unsigned radix-marker type is established by four unsigned comparisons in the native code.

`wcstof` at `0x80348BAC` retains the canonical name and float-return declaration introduced by [F-284-2](284-nppu-natives-luckyhelper.md#f-284-2-the-sixteen-natives-of-nppulibrarycpp). That independent caller reconstruction identifies it beside `strtof` in the real-number script conversion paths; naming confidence remains medium. It initializes `__wInStrCtrl`, invokes the scanner with `INT_MAX` and `__wStringRead`, advances a supplied end pointer by twice the consumed character count, sets ERANGE on the observed scanner/range conditions, and rounds the result to single precision before returning it. The native wrapper checks **double-precision** minimum and maximum values, which this reconstruction preserves. The original #539 guess `wcstod` is superseded. The C float-return interface reproduces all 184 native instruction bytes.

The nearby `__lconv` object at `0x803E6078` and positive-infinity storage at `0x804B8FB8` are shared inputs, not owned data of this scanner. No `.data` or `.sdata` extent is taken into its split. The latter symbol is named `__double_huge` from the MSL reference and its verified `7FF00000 00000000` contents. Its incomplete external array declaration preserves the target's absolute-address loads.

## Reconstruction and provenance

The implementation follows CC0 [Petari at e0ab141914921726a595573dc48d3f13d0922efe, src/MSL_C/strtold.c](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/MSL_C/strtold.c), adapted to the target's wide stream and native branches. The initial lead was CC0 [InusualZ/mhtri-decomp at ac489be692a1273afff8574b2402aecba2ae2b8f, src/MSL_C/strtold.c](https://github.com/InusualZ/mhtri-decomp/blob/ac489be692a1273afff8574b2402aecba2ae2b8f/src/MSL_C/strtold.c). Its reported narrow parser has the same native size as this game's narrow parser, but that did not establish a compatible implementation. Its decimal range check and other details differ from this target. It was a reading lead, not a substitute for instruction comparison. See [the provenance notice](../licenses/539-wide-parser-NOTICE.md).

The C source uses named scan-state enums, the canonical `decimal` and wide-reader definitions, and ordinary byte operations for the IEEE mantissa. No assembly, decompiler output, original instruction arrays, or executable-byte fallback was added. Constant-folded portability branches and the reference's inline-assembly workaround were removed; the native infinity path is expressed with an ordinary float value. Existing MSL compiler flags are unchanged.

Target-specific details established by the original instructions:

- Most character tests and case conversions use the wide locale maps. The NaN model/tag checks use the narrow maps. Hexadecimal fractional-digit classification and the first hexadecimal exponent-digit test also use narrow maps, while the integer-digit and subsequent exponent-digit tests use wide maps. Preserve this mixture.
- Decimal exponent markers compare directly against `E` and `e`; hexadecimal `P` recognition still uses the wide upper-case map.
- Decimal exponents and adjusted decimal exponents use the signed-short range, rather than the alternate reference's 308-based check. The native code combines the decimal exponent adjustment before checking the final range.
- Hexadecimal exponent digits accumulate into `exp_value`; the separate normalization exponent remains zero until mantissa normalization. The encoded exponent combines these two values. The sign flag applies to that normalization exponent, as in the target. This is a reconstruction of the observed implementation, not a standards correction.
- The `NAN(` object is one five-byte string, including its terminator. Its prior two-symbol interpretation made the null-byte reference appear to target a different object. Correcting that owned symbol extent changes no bytes and gives the original relocation its proper addend.

## Validation

The isolated worktree passed the baseline and split-only builds with the original scanner selected. Both new functions then reached exact 100% in objdiff, and the unit was switched to `Matching`. Default Ninja exited 0 after removing only the old DOL; explicit frozen verification exited 0 with `PASS main.dol`.

The temporary `539-source-audit.py` independently checks all forty original issue rows. Its receipts record:

| Check | Result |
| --- | --- |
| Raw function bodies, without masking instruction words | All forty exact; 22,656 bytes |
| Paired relocation positions, types and resolved destinations | 374 text targets exact; 37 in the new unit |
| Actual linked ELF symbols and map entries | All forty native addresses, sizes and source-object owners exact |
| Actual linker dependency graph | All 27 source objects selected; none of their original objects selected |
| Native data in the 27 units | 2,164 bytes including alignment checked against actual linked ELF |
| Complete DOL | Byte-identical to the immutable original and accepted by the frozen manifest |

The new source object has nine `.rodata` bytes, versus the native split's sixteen. Objdiff consequently reports 72% for that section's padded extent. The nine literal bytes match, the native seven-byte tail is all zero, and the audit checks all sixteen actual linked bytes at `0x803989A0`. The map assigns nine bytes there to `wstrtold.o`; the remaining seven are linker alignment before the next native object at `0x803989B0`. No fabricated source padding or extra progress credit is used. `.text` and the 32-byte `.sdata2` section are 100%.

The final merge head is checked through the official land loop with the same independent source audit after its build and frozen verification. The temporary adapter only retains `--map` during configure and adds that audit to the verification gate; rebase, clean-tree, push, head/ancestry checks and merge behavior remain those of `tools/collab/land.py`.

## Original issue inventory

| Original address | Instruction bytes | Accepted name | Source unit |
| --- | --- | --- | --- |
| `0x80347904` | `0x12A8` | `__wcstold` | `MSL_C/wstrtold.c` |
| `0x80348BAC` | `0xB8` | `wcstof` | `MSL_C/wstrtold.c` |
| `0x80348C64` | `0x418` | `__wcstoul` | `MSL_C/wcstoul.c` |
| `0x8034907C` | `0xF0` | `wcstol` | `MSL_C/wcstoul.c` |
| `0x8034916C` | `0x8` | `wmemcpy` | `MSL_C/wmem.c` |
| `0x80349174` | `0x8` | `wmemmove` | `MSL_C/wmem.c` |
| `0x8034917C` | `0x60` | `wmemset` | `MSL_C/wmem.c` |
| `0x803491DC` | `0x28` | `wmemchr` | `MSL_C/wmem.c` |
| `0x80349204` | `0x34` | `wmemcmp` | `MSL_C/wmem.c` |
| `0x80349238` | `0x90` | `__wStringRead` | `MSL_C/wscanf.c` |
| `0x803492C8` | `0x1C` | `wcslen` | `MSL_C/wstring.c` |
| `0x803492E4` | `0x1C` | `wcscpy` | `MSL_C/wstring.c` |
| `0x80349300` | `0x44` | `wcsncpy` | `MSL_C/wstring.c` |
| `0x80349344` | `0x34` | `wcscmp` | `MSL_C/wstring.c` |
| `0x80349378` | `0x40` | `wcsncmp` | `MSL_C/wstring.c` |
| `0x803493B8` | `0x2C` | `wcschr` | `MSL_C/wstring.c` |
| `0x803493E4` | `0x78` | `fwide` | `MSL_C/wchar_io.c` |
| `0x8034945C` | `0xD0` | `__write_console` | `MSL_C/uart_console_io_gcn.c` |
| `0x8034952C` | `0x8` | `__close_console` | `MSL_C/uart_console_io_gcn.c` |
| `0x80349534` | `0x34` | `abort` | `MSL_C/abort_exit_ppc_eabi.c` |
| `0x80349568` | `0x40` | `scalbn` | `MSL_C/math_sun.c` |
| `0x803495A8` | `0xBC` | `roundf` | `MSL_C/math_sun.c` |
| `0x80349664` | `0x298` | `__ieee754_asin` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/e_asin.c` |
| `0x803498FC` | `0x258` | `__ieee754_atan2` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/e_atan2.c` |
| `0x80349B54` | `0x230` | `__ieee754_exp` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/e_exp.c` |
| `0x80349D84` | `0x380` | `__ieee754_fmod` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/e_fmod.c` |
| `0x8034A104` | `0x2B4` | `__ieee754_log` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/e_log.c` |
| `0x8034A3B8` | `0x114` | `__ieee754_log10` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/e_log10.c` |
| `0x8034A4CC` | `0x814` | `__ieee754_pow` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/e_pow.c` |
| `0x8034ACE0` | `0x384` | `__ieee754_rem_pio2` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/e_rem_pio2.c` |
| `0x8034B064` | `0x110` | `__kernel_cos` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/k_cos.c` |
| `0x8034B174` | `0x16B8` | `__kernel_rem_pio2` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/k_rem_pio2.c` |
| `0x8034C82C` | `0xC0` | `__kernel_sin` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/k_sin.c` |
| `0x8034C8EC` | `0x250` | `__kernel_tan` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/k_tan.c` |
| `0x8034CB3C` | `0x230` | `atan` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/s_atan.c` |
| `0x8034CD6C` | `0x14C` | `ceil` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/s_ceil.c` |
| `0x8034CEB8` | `0x2C` | `copysign` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/s_copysign.c` |
| `0x8034CEE4` | `0xC8` | `cos` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/s_cos.c` |
| `0x8034CFAC` | `0x150` | `floor` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/s_floor.c` |
| `0x8034D0FC` | `0x88` | `frexp` | `MSL_C/MSL_Common_Embedded/Math/Double_precision/s_frexp.c` |

## Traps

- **A same-sized parser can still implement another revision.** The Monster Hunter Tri narrow parser's `0x1324` size was a useful lead, but its 308-based range logic does not reproduce this wide parser. Compare the branch conditions and variable updates before adopting a related implementation.
- **Converting every character helper to its wide counterpart changes this target.** Native NaN and two hexadecimal sub-states retain narrow locale-map accesses. A wide-only first draft reached only 83.92%; mixed-map reconstruction, native exponent logic and the proper scalar types reproduce the complete function.
- **A missing C math prototype can look like extra conversion work.** In this compiler invocation, including only `MSL_C/math.h` left the top-level `math.h` absent from Ninja's dependency list. `nan` was then treated as returning an integer, adding conversions and an eight-byte bias constant. Explicitly including the top-level `math.h` first restores the correct declaration and native calls. This finding is for the observed include context; it does not justify changing unrelated units.
- **A rounded return does not recover a standard function name.** The wrapper's `frsp` coexists with double-range checks. Record that behavior, and use independent caller/interface evidence when choosing a name. Here the concurrent F-284-2 change supplies the canonical `wcstof` interface; a speculative local wrapper name was replaced with it during rebase.
