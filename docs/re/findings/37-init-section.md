# Issue 37: init section startup and runtime memory routines

## F-37-1: Three translation units cover `.init` `0x80004000`-`0x800046E4`

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. Evidence uses dtk 1.8.4 split assembly, objdiff-cli 3.8.2 and full DOL verification. Reference source is Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` (CC0): `src/RVL_SDK/os/init/__start.c`, `src/RVL_SDK/os/init/__ppc_eabi_init.cpp`, `src/RVL_SDK/os/__ppc_eabi_init.c`, `src/Runtime/__mem.c` and `libs/Runtime/include/__ppc_eabi_linker.h`. The hand-written gekko `memcpy` follows xenoblade (`xbret/xenoblade` commit `ea25691b2029394ab777bda7060139e517a13cba`, CC0) `libs/PowerPC_EABI_Support/src/Runtime/__mem.c`, whose instruction stream equals this target's; labels were renamed and comments dropped.

| Unit | Sections | Compiler | Functions |
| --- | --- | --- | --- |
| `RVL_SDK/os/__start.c` | `.init` `0x80004000`-`0x800042F8`; `.sbss` `0x804B9538`-`0x804B9540` | RVL_SDK, Wii/1.0, `cflags_sdk` | `__check_pad3`, `__set_debug_bba`, `__get_debug_bba`, `__start`, `__init_registers`, `__init_data` |
| `RVL_SDK/os/__ppc_eabi_init.c` | `.init` `0x80004300`-`0x80004364`; `.text` `0x802C0A00`-`0x802C0ABC` | RVL_SDK, Wii/1.0, `cflags_sdk` | `__init_hardware`, `__flush_cache`, `__init_user`, `__init_cpp`, `exit` |
| `Runtime/__mem.c` | `.init` `0x80004364`-`0x800046E4`; `.text` `0x8033F974`-`0x8033F990` | Runtime.PPCEABI.H, Wii/1.1, `cflags_runtime` | `memcpy`, `__fill_mem`, `memset`, `strlen` |

Boundary evidence:

- Functions up to `__flush_cache` start on 16-byte boundaries with zero padding, the SDK default; `memcpy` follows `__flush_cache` with no padding, the runtime's `-func_align 4`.
- `Debug_BBA` sits between the OSStateTM statics (`0x804B9520`-`0x804B9530`) and OSPlayRecord's (`0x804B9540`), and `__ppc_eabi_init.c`'s `.text` sits between OSIpc-region OS code and EXIBios (`SetExiInterruptMask` at `0x802C0AC0`). Both orders match Petari, so these two units link among the OS objects. The `.sbss` split must end at `0x804B9540`; ending at `0x804B9539` makes dtk reject the following automatic split for alignment.
- `strlen` (`0x8033F974`, weak) is followed by the 0xC8-byte `__va_arg` routine at `0x8033F990`, the same layout as Petari's `__mem.c` and `__va_arg.c`. The issue that owns a file's first function owns its split, so `strlen` and the `.text` part of `__ppc_eabi_init.c` were included here although their addresses fall in #536 and #471.
- `exit` inlines `__fini_cpp`; `__init_data` inlines `__copy_rom_section` and `__init_bss_section`.

objdiff reports all 15 functions at 100%, `.sbss` 1/1 byte, and the full source-linked DOL verifies against the frozen manifest.

## F-37-2: OSInit loads 0x80004000 as a constant

`OSInit` builds the value `0x80004000` with `lis r4, 0x8000` at `0x802B25D8` and `addi r3, r4, 0x4000` at `0x802B264C`. dtk resolved those two instructions as relocations against the first `.init` symbol, which forced the static `__check_pad3` to be exported as a global `__check_pad3_80004000`; linking `__start.c` from source then failed with an undefined symbol. The value is the MEM1 arena default, not a code address. Both sources are listed under `block_relocations` in `config/wpse-v1/config.yml`; dtk then emits the raw immediates, `__check_pad3` stays local, and the DOL is unchanged.

## Traps

- `#pragma section code_type ".init"` only affects functions first declared after it. `<string.h>` declares `memcpy` and `memset`, so including it before the pragma put both into `.text`. Put the pragma before any include in `.init` units.
- `__fill_mem` matches only when the `dst` parameter itself is the walking pointer, as in the original MSL source with lvalue casts. Separate `unsigned char*` and `unsigned long*` locals swap r6 and r7.
- A data or code reference to a section's first address can be attributed by dtk to the first function there. If a static function appears as `name_<address>` and global in split assembly, look for such a constant and block the relocation instead of changing the function's linkage.
