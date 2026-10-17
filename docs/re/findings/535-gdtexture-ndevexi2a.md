# Issue 535: RVL_SDK GDTexture and NdevExi2A

## F-535-1: Unit boundaries from 0x8033E520 to 0x8033F974

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). Evidence is the dtk 1.8.4 split assembly `build/wpse-v1/asm/auto_03_802ABD4C_text.s`, the `.sdata`, `.sdata2` and `.sbss` bytes dumped from `build/wpse-v1/main.elf` with GNU objdump 2.42, and the reference sources listed below.

The issue's block covers three source files plus one function of a fourth:

| Range | Unit | Evidence |
| --- | --- | --- |
| `.text` `0x8033E520`–`0x8033E538` | `RVL_SDK/gd/GDBase.c` (owned by #534) | `fn_8033E520` loads the `.sbss` word at `0x804B9B9C` and tail-calls it if non-null: `GDOverflowed`. Every inlined `GDWriteBPCmd` in GDTexture calls it after comparing `__GDCurrentDL->ptr` (`.sbss` `0x804B9B98`) against `top`. It is the last function of GDBase.c, so it belongs to that file's split, which starts in #534's block. |
| `.text` `0x8033E540`–`0x8033EEB4`; `.sdata` `0x804B8F48`–`0x804B8F80`; `.sdata2` `0x804BB640`–`0x804BB648` | `RVL_SDK/gd/GDTexture.c` | Five 16-byte-aligned functions. The first three match Petari's `GDSetTexLookupMode`, `GDSetTexImgAttr` and `GDSetTexImgPtr` (also found by the F-0007 survey). `0x8033E990` writes BP registers from the tables at `0x804B8F60` and `0x804B8F68` and skips the second write when `size_odd` is 3 or `tmem_odd` is at least `0x100000`: `GDSetTexCached`. `0x8033EBA0` writes mask `0xFE00FFFF` before each of registers `0x30 + 2 * coord` and `0x31 + 2 * coord`: `GDSetTexCoordScale`. The `.sdata` holds six 8-byte register ID tables (`0x80`–`0xB7`) followed by `GD2HWFiltConv` `{0, 4, 1, 5, 2, 6}`; there is no TLUT ID table, so it was dead-stripped with `GDSetTexTlut`. `.sdata2` holds `32.0f` and `16.0f`. `0x804B8F40` (`"TPL.c"`) belongs to TPL. |
| `.text` `0x8033EEB4`–`0x8033F1E8`; `.sdata` `0x804B8F80`–`0x804B8F88`; `.sbss` `0x804B9BA0`–`0x804B9BB8` | `RVL_SDK/NdevExi2A/DebuggerDriver.c` | Nine functions in the order of Petari's `src/NDEV/DebuggerDriver.c`: `__DBMtrHandler`, `__DBIntrHandler` (writes `0x1000` to `0xCC003000`, installs on interrupt 25), `DBInitComm`, `DBInitInterrupts`, `DBQueryData`, `DBRead`, `DBWrite`, `DBOpen`, `DBClose`. The `.sdata` byte `0x80` is `DBWrite`'s `l_byOffsetCounter`. The five `.sbss` statics are laid out in reverse declaration order. |
| `.text` `0x8033F1E8`–`0x8033F974` | `RVL_SDK/NdevExi2A/exi2.c` | Six functions in Petari's `src/NDEV/exi2.c` order: `__EXI2Imm`, `__DBEXIInit`, `__DBEXIReadReg`, `__DBEXIWriteReg`, `__DBEXIReadRam`, `__DBEXIWriteRam`, all on EXI channel 2 at `0xCD006828`. The static `__EXI2Select`, `__EXI2Deselect` and `__EXI2Sync` are inlined. `strlen` at `0x8033F974` and `__va_arg` at `0x8033F990` start the runtime libraries. |

Confidence high for all boundaries: the three units link byte-identically with these ranges and verification passes.

## F-535-2: NdevExi2A is built with a GameCube compiler

From `0x8033EEB4` functions are packed at 4-byte alignment, unlike the 16-byte alignment of the surrounding RVL_SDK code (F-0007). Both NdevExi2A sources were compiled with every available compiler, with the RVL_SDK flags (`cflags_sdk`) with and without `-func_align 4`, and the disassembly compared with the dtk split objects after masking branch targets:

| Compiler | DebuggerDriver.c differing lines | exi2.c differing lines |
| --- | --- | --- |
| Wii/1.0, 1.0a, 1.0RC1, 0x4201_127, 1.1 | 32 | 35 (28 with `-func_align 4`) |
| GC/3.0a5.2 | 8 | 0 |
| GC/3.0a3, 3.0a3.4, 3.0a5 | 0 | 0 |

Wii compilers allocate `DBInitComm` with a 16-byte frame and two saved registers; the original uses a 32-byte frame saving r29–r31, as GC/3.0a3 to 3.0a5 do. GC/3.0a3 is chosen because it is the compiler Petari uses for the same library and it is indistinguishable here from 3.0a3.4 and 3.0a5; the exact GC build remains open, confidence medium. The library is registered as `NdevExi2A` in `configure.py` with GC/3.0a3 and `cflags_sdk`. With it both units match in objdiff 3.8.2 and the full DOL verifies. GDTexture.c matches with Wii/1.0, consistent with F-0007.
