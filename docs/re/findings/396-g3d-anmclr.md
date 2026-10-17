# F-396-1: NW4R g3d `AnmObjMatClr` unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #396 owns `.text` `0x80255E80–0x80256AF4`. Its first two functions, `AnmObjVis::GetTypeName` and `GetTypeObj`, end `g3d_anmvis.cpp` and were matched with #395 ([F-395-1](395-g3d-anmvis.md)). The rest is `g3d_anmclr.cpp`, which ends with the first two functions of #397.

## Unit and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/g3d_anmclr.cpp` | `.text` | `0x80255EC0–0x80256B3C` | 23 functions of `AnmObjMatClr` and `AnmObjMatClrRes`, and `ApplyClrAnmResult` |
| | `.rodata` | `0x8036C738–0x8036C768` | `TYPE_NAME` of `AnmObjMatClr` and `AnmObjMatClrRes` (0x18 each with padding) |
| | `.data` | `0x803C5A68–0x803C5AF8` | vtables of `AnmObjMatClrRes` and `AnmObjMatClr` (0x48 each) |
| | `.sdata2` | `0x804BAB90–0x804BABA0` | float and double constants |

`AnmObjMatClr::GetTypeName` (`0x80256B00`) and `GetTypeObj` (`0x80256B30`) are the first two functions of #397, which continues at `0x80256B40` with `g3d_anmtexpat.cpp`. objdiff reports 100% for all 23 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

`src/nw4r/g3d/g3d_anmclr.cpp` is CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` `src/nw4r/g3d/g3d_anmclr.cpp`, stripped of comments, and compiles to the target bytes unchanged. It also includes `nw4r/ut/ut_Color.h`. `include/nw4r/g3d/g3d_anmclr.h` is ss's header from the same commit, stripped of comments. The linker stripped `AnmObjMatClrNode`, `AnmObjMatClrOverride` and `AnmObjMatClr::TestDefined`; the source keeps them.

## Traps

None new. The trap from F-395-1 applies: include `nw4r/ut/ut_algorithm.h` before the g3d object headers, which pull in `<revolution/mem.h>`.
