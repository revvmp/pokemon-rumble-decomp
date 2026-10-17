# F-397-1: NW4R g3d `AnmObjTexPat` unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #397 owns `.text` `0x80256B00–0x80257594`. Its first two functions, `AnmObjMatClr::GetTypeName` and `GetTypeObj`, end `g3d_anmclr.cpp` and were matched with #396 ([F-396-1](396-g3d-anmclr.md)). The rest is `g3d_anmtexpat.cpp`, which ends with the first two functions of #398.

## Unit and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/g3d_anmtexpat.cpp` | `.text` | `0x80256B40–0x802575DC` | 23 functions of `AnmObjTexPat` and `AnmObjTexPatRes`, and `ApplyTexPatAnmResult` |
| | `.rodata` | `0x8036C768–0x8036C798` | `TYPE_NAME` of `AnmObjTexPat` and `AnmObjTexPatRes` (0x18 each with padding) |
| | `.data` | `0x803C5AF8–0x803C5B88` | vtables of `AnmObjTexPatRes` and `AnmObjTexPat` (0x48 each) |
| | `.sdata2` | `0x804BABA0–0x804BABB0` | float and double constants |

`AnmObjTexPat::GetTypeName` (`0x802575A0`) and `GetTypeObj` (`0x802575D0`) are the first two functions of #398, which continues at `0x802575E0` with `g3d_anmtexsrt.cpp`. Because #397 owns the unit's first function, this change carries those two functions. objdiff reports 100% for all 23 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

`src/nw4r/g3d/g3d_anmtexpat.cpp` is CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` `src/nw4r/g3d/g3d_anmtexpat.cpp`, stripped of comments, and compiles to the target bytes unchanged. Its includes are replaced with `nw4r/ut/ut_algorithm.h`, `nw4r/g3d/g3d_anmtexpat.h`, `<revolution/gx.h>`, `<revolution/gx/GXGet.h>` and `<revolution/gx/GXTexture.h>`. `include/nw4r/g3d/g3d_anmtexpat.h` is ss's header from the same commit, stripped of comments. The linker stripped `AnmObjTexPatNode`, `AnmObjTexPatOverride`, `AnmObjTexPat::TestDefined`, and the out-of-line `AnmObjTexPatRes` constructor and `UpdateCache`; the source keeps them.

`include/revolution/gx/GXGet.h` gains declarations of `GXGetTexObjWrapS` and `GXGetTexObjWrapT`, which `src/RVL_SDK/gx/GXTexture.c` already defines.

## Traps

- **The imported `GXGet.h` is incomplete.** It declares `GXGetTexObjLODAll` but not the wrap getters, and `<revolution/gx.h>` includes neither `GXGet.h` nor `GXTexture.h`. Include both explicitly and add missing getters from the `GXTexture.c` definitions.
