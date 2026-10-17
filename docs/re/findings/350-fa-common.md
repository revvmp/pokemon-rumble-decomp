# F-350-1: FA library identity and compiler

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #350.

The block `0x801FB030`–`0x80225820` is the RVL_SDK **FA** library. Its `.data` banner at DOL offset `0x3C05D8` reads `<< RVL_SDK - FA \trelease build: Mar  4 2009 15:07:30 (0x4302_145) >>`. FA is built on eSOL PrFILE2, the FAT file system that the older RVL_SDK VF library also wraps. The neighbouring diagnostics name PrFILE2 kernel and SD driver routines such as `pfk_create_task()`, `pfd_sddrv_init()` and `pfd_sddrv_mount()`.

Compiler: **Wii/1.0** with `cflags_sdk`, the configuration that already builds CNT, which carries the same `0x4302_145` tag. Functions start on 16-byte boundaries, and Wii/1.0 reproduces the loop-head `nop` alignment and the eight-way unrolled copy loops. Wii/0x4201_127, Wii/1.0RC1, Wii/1.0a and Wii/1.1 give identical code for these units. GC/3.0a3 to GC/3.0a5.2 do not: with `-func_align 16` they still schedule the store before the test in `pf_strcpy` and `pf_w_strcpy`.

The PrFILE2 version differs from the VF copies decompiled in Petari and in the Wii Menu decompilation (both CC0). It has `pf_memcmp` and a complete wide-string set directly after the narrow string functions. `PF_STR` has a fourth field. `PFSTR_InitStr` performs no null checks, and the volume set holds its character-code table at `0x48` instead of `0x24`. Those projects were used as structural references. The source here was written for this binary and verified against it.

Validation: `python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0. `pf_clib.c`, `pf_code.c` and `pf_service.c` are `Matching` and linked, and objdiff reports 16/16 functions at 100%.

# F-350-2: Unit boundaries, names and remaining work

| Unit | `.text` | Data | Status |
| --- | --- | --- | --- |
| `RVL_SDK/fa/pf_clib.c` | `0x801FB030`–`0x801FB4C0` | none | Matching, 12/12 |
| `RVL_SDK/fa/pf_code.c` | `0x801FB4C0`–`0x801FB4E0` | `.rodata` `0x80368128`–`0x80368188` (`pf_valid_fn_char`) | Matching, 2/2 |
| `RVL_SDK/fa/pf_service.c` | `0x801FB4E0`–`0x801FB618` | none | Matching, 2/2 |
| `RVL_SDK/fa/pf_str.c` | `0x801FB620`–`0x801FBBDC` | none | NonMatching. The 9 functions in #350 are at 100%; `0x801FB9D0` and `0x801FBAF0` belong to #351 |

FA `.rodata` starts at `0x80368128`. The 16 bytes before it are a TMC JPEG table that is referenced only from `0x801F92E0`–`0x801F9590`. The VF reference file order is `pf_clib`, `pf_code`, `pf_service`, `pf_str`, `pf_w_clib`. Here the four wide-string functions sit directly after `pf_strncmp`, so they are placed in `pf_clib.c`. A separate `pf_w_clib.c` between `pf_clib.c` and `pf_code.c` would link identically, so the file assignment of the wide functions is unproven.

Names. The executable has no PrFILE2 symbol names. In VF, Nintendo prefixed every PrFILE2 internal with `VFi` (`VFipf_strlen`, `VFiPFSTR_InitStr`). FA's own diagnostics use bare PrFILE2 names (`pfk_create_task`, `pfd_sddrv_init`). The FA units therefore use the VF names without the `VFi` prefix: `pf_toupper` … `pf_w_strncmp`, `PFCODE_Combine_Width`, `PFCODE_Divide_Width`, `PF_GET_LE_U16`, `PF_LE16_TO_U16_STR`, `PFSTR_SetCodeMode`, `PFSTR_GetCodeMode`, `PFSTR_GetStrPos`, `PFSTR_MoveStrPos`, `PFSTR_InitStr`, `PFSTR_StrLen`, `PFSTR_StrNumChar`, `PFSTR_StrCmp`, `pf_valid_fn_char` and `pf_vol_set`. Confidence in the spelling is medium. `pf_memcmp` and `pf_w_strcmp` follow the same pattern but have no VF counterpart. `PFSTR_SetMarkPos` and the `PF_STR` field `p_mark` are descriptive. The routine at `0x801FB640` stores either the head or its argument at offset 8, and `PFSTR_GetStrPos` returns that field for targets other than head and tail.

Behaviour that differs from VF: `pf_memset` aligns the destination without testing the remaining length. `PFSTR_MoveStrPos` advances its Unicode cursor by `width` 16-bit units, but advances the head by `width` bytes. `PFSTR_StrNumChar` handles only the head and tail targets.

## Traps

- **Wii/1.0 folds `c >= 'a' && c <= 'z'` into one unsigned compare.** Trigger: `pf_toupper` emitted `addi`/`cmplwi` where the target has two signed compares. `'a' <= c && c <= 'z'` keeps both compares. Evidence: F-350-1.
- **A signed-char cast creates a separate induction pointer.** Trigger: `PF_LE16_TO_U16_STR` was four instructions short and used indexed loads. Reading the saved byte through `((s8*)str)[cnt]` reproduces the second pointer. Evidence: F-350-2.
- **`const` on a struct parameter changes prologue scheduling.** Trigger: `PFSTR_StrCmp` loaded `code_mode` after the parameter copies. Declaring `const PF_STR* p_str` moves the load into the prologue, as in the target. Evidence: F-350-2.
