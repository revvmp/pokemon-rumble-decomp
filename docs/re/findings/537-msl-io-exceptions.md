# Issue 537: exception runtime and MSL file, buffer and decimal units

## F-537-1: Six of seven translation units in 0x80340B94-0x80344EB4

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. Evidence uses dtk 1.8.4, objdiff-cli 3.8.2, MWCC Wii/1.1 diagnostic objects and full DOL verification. Reference: xenoblade `ea25691b2029394ab777bda7060139e517a13cba` (CC0) `libs/PowerPC_EABI_Support/src/Runtime/Gecko_ExceptionPPC.cp`, `src/MSL_C/MSL_Common/ansi_files.c`, `buffer_io.c`, `direct_io.c`, `src/MSL_C/MSL_Common_Embedded/ansi_fp.c` and their headers. Comments and unused stub functions were dropped, decompiler-style local names replaced, and file structures moved into `include/MSL_C/`. `div` is the standard MSL algorithm written for this target; xenoblade and Petari only stub it.

| Unit | Sections | Library and flags | Functions |
| --- | --- | --- | --- |
| `Runtime/Gecko_ExceptionPPC.cpp` | extab `0x800441A0`-`0x80044214`; extabindex `0x8005247C`-`0x800524D0`; `.text` `0x80340B94`-`0x803422E0`; `.rodata` `0x80398328`-`0x80398390`; `.data` `0x803E5D18`-`0x803E5DD0`; `.bss` `0x804B3078`-`0x804B31F8`; `.sdata` `0x804B8FA0`-`0x804B8FA8` | Runtime.PPCEABI.H plus `-Cpp_exceptions on -RTTI on` | 14 |
| `MSL_C/alloc.c` | `.text` `0x803422E0`-`0x803430E0` | not split | 10, see below |
| `MSL_C/ansi_files.c` | `.text` `0x803430E0`-`0x803431F0`; `.data` `0x803E5DD0`-`0x803E5F10`; `.bss` `0x804B3230`-`0x804B3530` | MSL_C, `cflags_msl` | `__close_all`, `__flush_all` |
| `MSL_C/ansi_fp.c` | `.text` `0x803431F0`-`0x80344A70`; `.rodata` `0x803983A8`-`0x80398488`; `.data` `0x803E5F10`-`0x803E6078`; `.sdata2` `0x804BB648`-`0x804BB680` | MSL_C, `cflags_msl` | `__ull2dec`, `__timesdec`, `__str2dec`, `__two_exp`, `__equals_dec`, `__less_dec`, `__minus_dec`, `__num2dec_internal`, `__num2dec`, `__dec2num` |
| `MSL_C/arith.c` | `.text` `0x80344A70`-`0x80344ACC` | MSL_C, `cflags_msl` | `div` |
| `MSL_C/buffer_io.c` | `.text` `0x80344ACC`-`0x80344BAC` | MSL_C, `cflags_msl` | `__prep_buffer`, `__flush_buffer` |
| `MSL_C/direct_io.c` | `.text` `0x80344BAC`-`0x80344EB4` | MSL_C, `cflags_msl` | `__fwrite` |

Gecko_ExceptionPPC functions: `__register_fragment`, `__unregister_fragment`, `ExPPC_FindExceptionFragment`, `ExPPC_FindExceptionRecord`, `ExPPC_NextAction`, `ExPPC_PopStackFrame`, `ExPPC_UnwindStack`, `__unexpected`, `std::bad_exception::~bad_exception`, `ExPPC_LongJump`, `ExPPC_ThrowHandler`, `__throw`, `__end__catch`, `std::bad_exception::what`.

Evidence:

- Every function's instruction stream matches the compiled reference, checked with relocations masked and then by objdiff at 100% for all 30 functions.
- Gecko_ExceptionPPC needs `-RTTI on`: the `bad_exception` vtable at `0x803E5DA0` starts with a pointer to its type info at `0x804B8FA0`, whose name is `std::bad_exception` at `0x80398328`. Without RTTI the code still matches but the data does not.
- The extabindex entries `0x8005247C`-`0x800524C4` cover exactly the seven Gecko functions with unwind actions; the next entry belongs to a later unit.
- `__files` (`0x803E5DD0`) holds the stdin, stdout and stderr buffers in reverse address order (`0x804B3430`, `0x804B3330`, `0x804B3230`), the order in which MWCC emits the three statics. Its mode words (`0x0A800000`) place `file_kind` at bits 24-22 with the two-bit wide orientation field present, so `file_orientation` is always part of `__file_modes`.
- `0x80344A70` returns `div_t` in r3/r4. Its sign handling and remainder product order match MSL `div`; `abs` and `labs` from the same file are not referenced and are stripped.
- Each split ends at its last object where possible. The Gecko `.rodata` and `.data` and the ansi_fp `.rodata` must end at the next 4- or 8-byte boundary (`0x80398390`, `0x803E5DD0`, `0x80398488`); ending at the object end makes dtk reject the following automatic split for alignment.

The full DOL verifies with all six units `Matching`.

## Remaining: `MSL_C/alloc.c`

`0x803422E0`-`0x803430E0` holds ten functions of `alloc.c`. Four match the xenoblade reference in size and code: `Block_link` (`0x803424E4`), `SubBlock_merge_next` (`0x80342634`), `deallocate_from_fixed_pools` (`0x80342D24`) and `free` (`0x80342FB0`). The other six are real code in this game but empty stubs in every CC0 reference checked (xenoblade, Petari, mkdd, pikmin2, Mario Party 4, BFBB): `0x803422E0` (0x204), `0x803426DC` (0x240), `0x8034291C` (0xDC), `0x803429F8` (0x124), `0x80342B1C` (0x208) and `0x80342F10` (0xA0). By MSL file order they are `Block_subBlock`, `allocate_from_var_pools`, `soft_allocate_from_var_pools`, a fixed-pool constructor, `allocate_from_fixed_pools` and `__pool_free`; these names are not yet confirmed. Their data are `.rodata` `0x80398390`-`0x803983A8` (`fix_pool_sizes`, six words), `.bss` `0x804B31F8`-`0x804B3230` and `.sbss` `0x804B9BC8`-`0x804B9BD0`. The unit needs these six functions written from the target before it can be split and linked.

## Recovered names

High confidence from reference correspondence and identical code, except the outside callees, whose evidence is given.

| Address | Name |
| --- | --- |
| `0x80340C08` | `ExPPC_FindExceptionFragment__FPcP12FragmentInfo` |
| `0x80340CA0` | `ExPPC_FindExceptionRecord__FPcP15MWExceptionInfo` |
| `0x80340E50` | `ExPPC_NextAction__FP14ActionIterator` |
| `0x80341000` | `ExPPC_PopStackFrame__FP12ThrowContextP15MWExceptionInfo` |
| `0x80341554` | `ExPPC_UnwindStack__FP12ThrowContextP15MWExceptionInfoPv` |
| `0x80341A60` | `__unexpected` |
| `0x80341C18` | `__dt__Q23std13bad_exceptionFv` |
| `0x80341C58` | `ExPPC_LongJump__FP12ThrowContextPvPv` |
| `0x80341D5C` | `ExPPC_ThrowHandler__FP12ThrowContext` |
| `0x803422A8` | `__end__catch` |
| `0x803422D4` | `what__Q23std13bad_exceptionCFv` |
| `0x803E5DA0` | `__vt__Q23std13bad_exception` |
| `0x803E5DBC` | `@STRING@what__Q23std13bad_exceptionCFv` |
| `0x804B8FA0` | `__RTTI__Q23std13bad_exception` |
| `0x80343184` | `__flush_all` |
| `0x803431F0`-`0x80344324` | `__ull2dec`, `__timesdec`, `__str2dec`, `__two_exp`, `__equals_dec`, `__less_dec`, `__minus_dec`, `__num2dec_internal`, `__num2dec`, `__dec2num` |
| `0x80344A70` | `div` |
| `0x804B3230`, `0x804B3330`, `0x804B3430` | `stderr_buff`, `stdout_buff`, `stdin_buff` |
| `0x80342FB0` | `free`, called by `__close_all` for dynamically allocated files |
| `0x80344EB4` | `fclose`, called by `__close_all` for each open file |
| `0x80344F70` | `fflush`, called by `__flush_all` |
| `0x80350FFC`, `0x8034945C`, `0x8034952C` | `__read_console`, `__write_console`, `__close_console`, the read, write and close procedures stored in every `__files` entry |
| `0x80345A64` | `__signbitd`, the first call in `__num2dec_internal` |
| `0x8034D0FC` | `frexp`, called by `__num2dec_internal` before `__two_exp` |
| `0x804B8FB4` | `__float_huge`, read by `__dec2num` for `INFINITY`; it follows `__float_nan` |

## Traps

- With `-gccinc`, `#include <math.h>` inside `include/MSL_C/` finds `include/MSL_C/math.h` itself, not `include/math.h`. The standard declarations then disappear silently: `frexp` and `ldexp` became implicit `int` functions and two functions stopped matching. Include `<math.h>` from the source file, before `"MSL_C/math.h"`.
- A `lbl_` symbol whose dtk-inferred size includes padding (`0x8039833C`, `0x803983A8`) must be resized to the real object before a split can end after it.
