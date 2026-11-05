# MSL math source notices for issue 539

The fdlibm-derived sources added in issue 539 follow Petari at commit `e0ab141914921726a595573dc48d3f13d0922efe` (root LICENSE CC0-1.0, no additional file-level notices in the consulted files): `e_asin.c`, `e_atan2.c`, `e_fmod.c`, `e_log.c`, `e_log10.c`, `e_pow.c`, `e_rem_pio2.c`, `k_cos.c`, `k_rem_pio2.c`, `k_sin.c`, `k_tan.c`, `s_atan.c`, `s_ceil.c`, `s_copysign.c`, `s_cos.c`, `s_floor.c` and `s_frexp.c`. `e_exp.c` is adapted from [Sun fdlibm `e_exp.c` revision 1.6, 2004-04-22](https://netlib.org/fdlibm/e_exp.c), which Petari does not contain. The `errno` assignment in `e_asin.c` and the early equality test in `e_fmod.c` were written for this target from its instructions.

The notice of `e_exp.c` is preserved below.

> Copyright (C) 2004 by Sun Microsystems, Inc. All rights reserved.
>
> Permission to use, copy, modify, and distribute this
> software is freely granted, provided that this notice
> is preserved.

The same 1993 Sun notice recorded in [the issue 540 notices](540-msl-math-NOTICE.md) accompanies the other fdlibm-derived files.

The wide-character, console, abort and `math_sun.c` sources follow xenoblade at commit `ea25691b2029394ab777bda7060139e517a13cba` (CC0-1.0) and Petari (`wmem.c`, `wstring.c`). `wmemmove`, `wmemset`, `wmemcmp`, `wcsncmp` and `roundf` are empty stubs in both references and were written for this target.
