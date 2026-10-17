# Issue 539: MSL wide-character, console, abort and fdlibm units

## F-539-1: 26 translation units in 0x80348C64-0x8034D184

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. Evidence uses dtk 1.8.4, objdiff-cli 3.8.2, MWCC Wii/1.1 diagnostic objects and full DOL verification. All units are in the MSL_C library with `cflags_msl` (Wii/1.1, `-func_align 4`, contraction off, file IPA) and are linked as `Matching`. Source provenance and the fdlibm notice are in [the notice file](../licenses/539-msl-math-NOTICE.md).

| Unit | Sections | Functions |
| --- | --- | --- |
| `MSL_C/wcstoul.c` | `.text` `0x80348C64`-`0x8034916C` | `__wcstoul`, `wcstol` |
| `MSL_C/wmem.c` | `.text` `0x8034916C`-`0x80349238` | `wmemcpy`, `wmemmove`, `wmemset`, `wmemchr`, `wmemcmp` |
| `MSL_C/wscanf.c` | `.text` `0x80349238`-`0x803492C8` | `__wStringRead` |
| `MSL_C/wstring.c` | `.text` `0x803492C8`-`0x803493E4` | `wcslen`, `wcscpy`, `wcsncpy`, `wcscmp`, `wcsncmp`, `wcschr` |
| `MSL_C/wchar_io.c` | `.text` `0x803493E4`-`0x8034945C` | `fwide` |
| `MSL_C/uart_console_io_gcn.c` | `.text` `0x8034945C`-`0x80349534`; `.sbss` `0x804B9BD8`-`0x804B9BDC` | `__write_console`, `__close_console` |
| `MSL_C/abort_exit_ppc_eabi.c` | `.text` `0x80349534`-`0x80349568`; `.sbss` `0x804B9BE0`-`0x804B9BE8` | `abort` |
| `MSL_C/math_sun.c` | `.text` `0x80349568`-`0x80349664`; `.sdata2` `0x804BB6D8`-`0x804BB6F8` | `scalbn`, `roundf` |
| `e_asin.c` | `.text` `0x80349664`-`0x803498FC`; `.sdata2` `0x804BB6F8`-`0x804BB780` | `__ieee754_asin` |
| `e_atan2.c` | `.text` `0x803498FC`-`0x80349B54`; `.sdata2` `0x804BB780`-`0x804BB7D8` | `__ieee754_atan2` |
| `e_exp.c` | `.text` `0x80349B54`-`0x80349D84`; `.rodata` `0x80398FB0`-`0x80398FE0`; `.sdata2` `0x804BB7D8`-`0x804BB850` | `__ieee754_exp` |
| `e_fmod.c` | `.text` `0x80349D84`-`0x8034A104`; `.rodata` `0x80398FE0`-`0x80398FF0` | `__ieee754_fmod` |
| `e_log.c` | `.text` `0x8034A104`-`0x8034A3B8`; `.sbss` `0x804B9BE8`-`0x804B9BF0`; `.sdata2` `0x804BB850`-`0x804BB8D0` | `__ieee754_log` |
| `e_log10.c` | `.text` `0x8034A3B8`-`0x8034A4CC`; `.sbss` `0x804B9BF0`-`0x804B9BF8`; `.sdata2` `0x804BB8D0`-`0x804BB900` | `__ieee754_log10` |
| `e_pow.c` | `.text` `0x8034A4CC`-`0x8034ACE0`; `.rodata` `0x80398FF0`-`0x80399020`; `.sdata2` `0x804BB900`-`0x804BBA10` | `__ieee754_pow` |
| `e_rem_pio2.c` | `.text` `0x8034ACE0`-`0x8034B064`; `.rodata` `0x80399020`-`0x803991A8`; `.sdata2` `0x804BBA10`-`0x804BBA68` | `__ieee754_rem_pio2` |
| `k_cos.c` | `.text` `0x8034B064`-`0x8034B174`; `.sdata2` `0x804BBA68`-`0x804BBAB0` | `__kernel_cos` |
| `k_rem_pio2.c` | `.text` `0x8034B174`-`0x8034C82C`; `.rodata` `0x803991A8`-`0x803991F8`; `.sdata2` `0x804BBAB0`-`0x804BBAF0` | `__kernel_rem_pio2` |
| `k_sin.c` | `.text` `0x8034C82C`-`0x8034C8EC`; `.sdata2` `0x804BBAF0`-`0x804BBB28` | `__kernel_sin` |
| `k_tan.c` | `.text` `0x8034C8EC`-`0x8034CB3C`; `.rodata` `0x803991F8`-`0x80399260`; `.sdata2` `0x804BBB28`-`0x804BBB60` | `__kernel_tan` |
| `s_atan.c` | `.text` `0x8034CB3C`-`0x8034CD6C`; `.rodata` `0x80399260`-`0x803992F8`; `.sdata2` `0x804BBB60`-`0x804BBB88` | `atan` |
| `s_ceil.c` | `.text` `0x8034CD6C`-`0x8034CEB8`; `.sdata2` `0x804BBB88`-`0x804BBB98` | `ceil` |
| `s_copysign.c` | `.text` `0x8034CEB8`-`0x8034CEE4` | `copysign` |
| `s_cos.c` | `.text` `0x8034CEE4`-`0x8034CFAC`; `.sdata2` `0x804BBB98`-`0x804BBBA0` | `cos` |
| `s_floor.c` | `.text` `0x8034CFAC`-`0x8034D0FC`; `.sdata2` `0x804BBBA0`-`0x804BBBB0` | `floor` |
| `s_frexp.c` | `.text` `0x8034D0FC`-`0x8034D184`; `.sdata2` `0x804BBBB0`-`0x804BBBB8` | `frexp` |

The math units live under `MSL_C/MSL_Common_Embedded/Math/Double_precision/`.

Evidence:

- objdiff reports 100% for every function and data section of all 26 objects (38 functions), and the full DOL verifies.
- Fifteen of the seventeen Petari math files compile to the target instruction streams unchanged, once SDA and relocated offsets are masked.
- Two Petari files needed target-specific changes:
  - `__ieee754_asin` stores `33` to `errno` before returning NaN for `|x| > 1`. This is `EDOM`, matching the value used by the log and pow functions.
  - `__ieee754_fmod` adds an early return of the signed zero inside and after the subtraction loop. It fires when the high words are equal and both low words, shifted right by `n + 2` (where `n` is the exponent difference), are equal. This shift is computed once before the loop (`addi r11,r7,2`, then `srw r8,r6,r11`). With both checks added the function is `0x380` bytes, which matches the target.
- `__ieee754_exp` is not in Petari. Sun fdlibm `e_exp.c` compiles to the 140 target instructions unchanged, and its `halF`, `ln2HI` and `ln2LO` arrays form the `0x30`-byte `.rodata` object at `0x80398FB0`.
- `roundf` (`0x803495A8`) calls `__fpclassifyf`, returns non-finite input unchanged, then truncates through `(int)` and rounds half away from zero. It sits directly after `scalbn` and matches the position of `roundf` in MSL `math_sun.c`; the functions between them there are unused and stripped.
- `wmemset` is a plain store loop; MWCC unrolls it eight times. `wmemmove` and `wmemcpy` are both two-instruction tail calls, and the call targets (`memmove` at `0x803455C4`, `memcpy` at `0x80004364`) tell them apart.
- `__write_console` and `abort` match xenoblade. Their outside callees are named from the call order in the reference: `InitializeUART` (`0x803087D0`, called with `57600`), `WriteUARTN` (`0x80308820`), `__TRK_write_console` (`0x80351064`) and `raise` (`0x80345BB8`, called with `1`).

## F-539-2: Correcting the math wrapper names from issue 540

The wrapper names from issue 540 were off by one. The wrappers at `0x8034D530`, `0x8034D534` and `0x8034D538` were called `acos`, `asin` and `atan2`, but they tail-call `asin`, `atan2` and `exp`:

- `0x80349664` is `__ieee754_asin`: it is the Petari source plus the `errno` store, and its `|x| == 1` case returns `x*pio2_hi + x*pio2_lo`.
- `0x803498FC` is `__ieee754_atan2`: it matches Petari `e_atan2.c` exactly.
- `0x80349B54` is `__ieee754_exp`: it matches fdlibm exactly.

The game has no `acos`. The math kernels are linked in alphabetical file order (`asin`, `atan2`, `exp`, `fmod`, `log`, `log10`, `pow`), and the wrappers follow the same order. This issue renames the three symbol pairs, renames `w_acos.c`, `w_asin.c` and `w_atan2.c` to `w_asin.c`, `w_atan2.c` and `w_exp.c`, and removes the `acos` and `__ieee754_acos` declarations. The wrapper bytes are unchanged.

## Remaining

`0x80347904`-`0x80348C64` contains two functions: the wide floating-point parser at `0x80347904` (`0x12A8`; it calls `nan` and reads `__float_huge`) and a `0xB8` wrapper at `0x80348BAC`. By MSL naming these would be `__wcstold` and `wcstod`. Neither reference has a wide version. Petari's narrow `__strtold` is `0x12F8`, but the target's narrow parser at `0x80345FD4` is `0x1324`, so even the narrow reference differs from this MSL revision. The unit's data are `.rodata` `0x803989A0` (`0x10`), `.data` `0x803E6078` (`0x38`), `.sdata` `0x804B8FB8` and `.sdata2` `0x804BB6B8`-`0x804BB6D8`. The parser has to be reconstructed from the target before this unit can be split.

## Recovered names

High confidence where the code matches a reference. The wrapper and kernel corrections rest on F-539-2. The outside callees are named from the reference call sequence.

| Address | Name |
| --- | --- |
| `0x80348C64`, `0x8034907C` | `__wcstoul`, `wcstol` |
| `0x8034916C`-`0x80349204` | `wmemcpy`, `wmemmove`, `wmemset`, `wmemchr`, `wmemcmp` |
| `0x80349238` | `__wStringRead` |
| `0x803492E4`-`0x803493B8` | `wcscpy`, `wcsncpy`, `wcscmp`, `wcsncmp`, `wcschr` |
| `0x80349568`, `0x803495A8` | `scalbn`, `roundf` |
| `0x80349664`, `0x803498FC`, `0x80349B54` | `__ieee754_asin`, `__ieee754_atan2`, `__ieee754_exp` (were `__ieee754_acos`, `__ieee754_asin`, `__ieee754_atan2`) |
| `0x8034D530`, `0x8034D534`, `0x8034D538` | `asin`, `atan2`, `exp` (were `acos`, `asin`, `atan2`) |
| `0x8034B174`, `0x8034CB3C`, `0x8034CD6C`, `0x8034CFAC` | `__kernel_rem_pio2`, `atan`, `ceil`, `floor` |
| `0x804B9BD8` | `initialized` (local to `uart_console_io_gcn.c`) |
| `0x804B9BE0` | `__aborting` |
| `0x803087D0`, `0x80308820` | `InitializeUART`, `WriteUARTN` |
| `0x80351064` | `__TRK_write_console` |
| `0x80345BB8` | `raise` |
| `0x80345A04` | `__fpclassifyf`, called by `roundf` and returning `FP_NAN` to `FP_SUBNORMAL` for a float |

## Traps

- dtk-inferred object sizes include alignment padding (`initialized` at `0x804B9BD8` was `0x8`). A split that ends at the real object end then fails with "ends within symbol" until the symbol is resized.
- Do not trust neighbouring wrapper names for library kernels. Compile the candidate sources and compare the kernel bodies first; see F-539-2.
