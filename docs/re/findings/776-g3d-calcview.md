# F-776-1: NW4R g3d `g3d_calcview.cpp` unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #776 owns the 11 functions of `g3d_calcview.cpp`, from `GetModelLocalAxisY2` (`0x8025FA00`) to `CalcView_LC_DMA_ModelMtx` (`0x802616E0`).

## Unit and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/g3d_calcview.cpp` | `.text` | `0x8025FA00–0x80261C7C` | eight anonymous-namespace billboard helpers, `CalcView`, `CalcView_LC`, `CalcView_LC_DMA_ModelMtx` |
| | `.rodata` | `0x8036C898–0x8036C8B4` | the local `bbFunc` billboard function table (0x1C) |
| | `.sdata2` | `0x804BAC80–0x804BAC8C` | two float constants and the local `norm_epsilon` |

`g3d_draw1mat1shp.cpp` (#775) precedes the unit; `g3d_dcc.cpp` (#777) follows with `CalcTexMtx` at `0x80261C80`. objdiff reports 100% for all 11 functions and all data, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

`src/nw4r/g3d/g3d_calcview.cpp` is CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` `src/nw4r/g3d/g3d_calcview.cpp`, stripped of comments, and compiles to the target bytes unchanged. Its includes are replaced with `ut_algorithm.h`, `g3d_calcview.h`, `g3d_dcc.h`, `g3d_obj.h` (for `align32`), `g3d_transform.h`, `g3d_workmem.h`, `platform/g3d_cpu.h`, `ut_LockedCache.h` and `<algorithm>`. Headers from the same ss commit, stripped of comments:

- `include/nw4r/g3d/g3d_calcview.h` (new), including `g3d_resmdl.h` for `ResMdl`.
- `include/nw4r/g3d/g3d_workmem.h` (new).
- `include/nw4r/g3d/g3d_dcc.h`, extended from the two `CalcTexMtx` declarations added by #749 to ss's full header, which adds `detail::WorldMtxAttr`; the existing declarations are unchanged.

`include/algorithm` gains `std::swap`, written as a plain copy; MSL's version moves, which is the same for the pointer swaps here.

## Outside symbols

`fn_80261DB0` is renamed `GetBillboardMtxTemporary__Q44nw4r3g3d6detail7workmemFv`. It is the fifth of the `workmem` accessors after `GetScaleTemporary` and `GetMtxIDTemporary`, in ss's `g3d_workmem.cpp` order, and the only one `CalcView` calls. Its split and size are left to #777.

## Traps

- **A new header can already exist with fewer declarations.** `g3d_dcc.h` was added by #749 with only `CalcTexMtx`. Writing ss's header over it showed as a modification, not a new file; check `git status` for `M` on headers you believe are new, and extend rather than replace.
