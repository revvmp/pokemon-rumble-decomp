# Issue 536: MSL runtime support units 0x8033F974-0x80340B24

## F-536-1: Six runtime translation units follow `__mem.c`

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. Evidence uses dtk 1.8.4, objdiff-cli 3.8.2, the Runtime.PPCEABI.H library settings (Wii/1.1, `cflags_runtime`, `-func_align 4`) and full DOL verification. References: Petari `e0ab141914921726a595573dc48d3f13d0922efe` (CC0) `src/Runtime/__va_arg.c`, `global_destructor_chain.c`, `ptmf.c`, and xenoblade `ea25691b2029394ab777bda7060139e517a13cba` (CC0) `libs/PowerPC_EABI_Support/src/Runtime/runtime.c`, `NMWException.cp`, `MWRTTI.cp` and `include/stl/exception`, `typeinfo`. Comments and unused stubs were dropped and names tidied; the algorithms are the references'.

`strlen` at `0x8033F974` is the `.text` part of `Runtime/__mem.c`, already linked by #37 (F-37-1).

| Unit | Sections | Extra flags | Functions |
| --- | --- | --- | --- |
| `Runtime/__va_arg.c` | `.text` `0x8033F990`-`0x8033FA58` | none | `__va_arg` |
| `Runtime/global_destructor_chain.c` | `.text` `0x8033FA58`-`0x8033FAB8`; `.sbss` `0x804B9BB8`-`0x804B9BBC` | none | `__register_global_object`, `__destroy_global_chain` |
| `Runtime/NMWException.cpp` | extab `0x80044150`-`0x80044190`; extabindex `0x80052434`-`0x80052464`; `.text` `0x8033FAB8`-`0x8034003C`; `.sdata` `0x804B8F88`-`0x804B8F90` | `-Cpp_exceptions on` | `std::dthandler`, `std::duhandler`, `std::terminate`, `std::unexpected`, `__throw_catch_compare`, `__construct_new_array`, `__partial_array_destructor::~__partial_array_destructor`, `__construct_array`, `__destroy_arr` |
| `Runtime/ptmf.c` | `.text` `0x8034003C`-`0x80340094`; `.rodata` `0x803982A8`-`0x803982B4` | none | `__ptmf_test`, `__ptmf_scall` |
| `Runtime/MWRTTI.cpp` | extab `0x80044190`-`0x800441A0`; extabindex `0x80052464`-`0x8005247C`; `.text` `0x80340094`-`0x8034033C`; `.rodata` `0x803982B8`-`0x80398310`; `.data` `0x803E5CF0`-`0x803E5D18`; `.sdata` `0x804B8F90`-`0x804B8F98` | `-Cpp_exceptions on -RTTI on` | `__dynamic_cast`, `std::bad_cast::~bad_cast`, `std::bad_cast::what` |
| `Runtime/runtime.c` | `.text` `0x8034033C`-`0x80340B24`; `.rodata` `0x80398310`-`0x80398328` | none | `__cvt_fp2unsigned`, `__save_fpr`, `__restore_fpr`, `__save_gpr`, `__restore_gpr`, `__div2u`, `__div2i`, `__mod2u`, `__mod2i`, `__shl2i`, `__cvt_sll_dbl`, `__cvt_dbl_usll`, `__cvt_dbl_ull` |

Boundary evidence:

- The link order matches xenoblade's runtime library: `__mem`, `__va_arg`, `global_destructor_chain`, `NMWException`, `ptmf`, `MWRTTI`, `runtime`, `__init_cpp_exceptions`. Petari has no `MWRTTI` between `ptmf` and `runtime`, so its addresses do not transfer.
- The extabindex entries name exactly `__construct_new_array`, the partial array destructor, `__construct_array` and `__destroy_arr` (NMWException), and `__dynamic_cast` and the `bad_cast` destructor (MWRTTI). The extab records they point to give the extab ranges.
- The sources define functions the target dead-strips (`set_terminate`, `set_unexpected`, `__destroy_new_array`, `__ptmf_cmpr`, `__get_typeid`, `__shr2u`, `__shr2i`, `__cvt_ull_dbl`, `__cvt_sll_flt`, `__cvt_ull_flt`). The original pooled string `!std::exception!!std::bad_typeid!!` at `0x803982CC` shows `__get_typeid` was compiled into this unit, so the full sources are kept and the linker strips the unused functions.
- The type info for `std::exception` is not defined here. The `bad_cast` base list at `0x803E5D00` points at `0x804B5AC0` in game `.sdata`. Naming that object `__RTTI__Q23std9exception` lets the linker keep the earlier definition and drop this unit's copy. Without the name, `.rodata` grew by 0x10 and `.sdata` overlapped `fragmentID`.
- `ptmf.c` `.rodata` ends at `0x803982B4` after the 12-byte `__ptmf_null`; MWRTTI's `.rodata` is 8-byte aligned. MWRTTI's `.data` must end at `0x803E5D18`; ending at the string end `0x803E5D15` makes dtk reject the next automatic split for alignment.

objdiff reports every function at 100%. In objdiff the data sections differ only by objects of the stripped functions. With all six units `Matching`, the full DOL verifies against the frozen manifest.

## Recovered names

All are high confidence from reference correspondence and identical instruction streams.

| Address | Name |
| --- | --- |
| `0x8033F990` | `__va_arg` |
| `0x8033FAB8` | `dthandler__3stdFv` |
| `0x8033FABC` | `duhandler__3stdFv` |
| `0x8033FAC0` | `terminate__3stdFv` |
| `0x8033FACC` | `unexpected__3stdFv` |
| `0x8033FAD8` | `__throw_catch_compare` |
| `0x8033FD0C` | `__construct_new_array` |
| `0x8033FE10` | `__dt__26__partial_array_destructorFv` |
| `0x8033FECC` | `__construct_array` |
| `0x8033FFC4` | `__destroy_arr` |
| `0x8034003C` | `__ptmf_test` |
| `0x8034006C` | `__ptmf_scall` |
| `0x80340094` | `__dynamic_cast` |
| `0x803402F0` | `__dt__Q23std8bad_castFv` |
| `0x80340330` | `what__Q23std8bad_castCFv` |
| `0x803404C8`-`0x80340A7C` | `__div2u`, `__div2i`, `__mod2u`, `__mod2i`, `__shl2i`, `__cvt_sll_dbl`, `__cvt_dbl_usll`, `__cvt_dbl_ull` |
| `0x803982A8` | `__ptmf_null` |
| `0x80398310` | `__constants` |
| `0x803E5CF0` | `__vt__Q23std8bad_cast` |
| `0x803E5D0C` | `@STRING@what__Q23std8bad_castCFv` |
| `0x804B8F88`, `0x804B8F8C` | `thandler__3std`, `uhandler__3std` |
| `0x804B8F90` | `__RTTI__Q23std8bad_cast` |
| `0x804B5AC0` | `__RTTI__Q23std9exception`, outside this block; target of the `bad_cast` base list |
| `0x80342164` | `__throw`, outside this block; called by `__dynamic_cast` with the throw-type string, object and destructor |
| `0x80349534` | `abort`, outside this block; the call in `std::dthandler` |

## Traps

- A vague-linkage object that the original takes from an earlier unit, such as `__RTTI__Q23std9exception`, must carry its mangled name in `symbols.txt`. Under a `lbl_` name the source copy is linked too and shifts every later section, which fails verification in the DOL header.
- C++ units that throw need `__throw` named in `symbols.txt` before the source link resolves.
