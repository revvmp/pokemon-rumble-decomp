# F-775-1: NW4R g3d `g3d_draw1mat1shp.cpp` unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #775 owns the two functions of `g3d_draw1mat1shp.cpp`, `detail::LoadMaterial` (`0x8025ECB0`) and `Draw1Mat1ShpDirectly` (`0x8025F150`).

## Unit and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/g3d_draw1mat1shp.cpp` | `.text` | `0x8025ECB0–0x8025F9F4` | `detail::LoadMaterial`, `Draw1Mat1ShpDirectly` |
| | `.rodata` | `0x8036C888–0x8036C898` | a 16-byte constant table used by `Draw1Mat1ShpDirectly` |
| | `.sdata2` | `0x804BAC60–0x804BAC80` | two float and two double constants |

`g3d_state.cpp` (#774) precedes the unit and `g3d_calcview.cpp` (#776) follows it at `0x8025FA00`. objdiff reports 100% for both functions and all 48 data bytes, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1).

## Source

`src/nw4r/g3d/g3d_draw1mat1shp.cpp` is CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` `src/nw4r/g3d/g3d_draw1mat1shp.cpp`, stripped of comments, and compiles to the target bytes unchanged. Its includes are replaced with `g3d_draw1mat1shp.h`, `g3d_light.h`, `g3d_state.h`, `platform/g3d_gpu.h`, `nw4r/ut/ut_LockedCache.h`, `<revolution/base/PPCArch.h>`, `<revolution/GX.h>` and `<revolution/gx/GXBump.h>`; ss's `<cmath>` include is unused and dropped. `include/nw4r/g3d/g3d_draw1mat1shp.h` is ss's header from the same commit, stripped of comments, including `math_types.h` for `math.h`.

## Traps

- **`<revolution/GX.h>` omits several GX headers.** `GXSetIndTexMtx` is only in `<revolution/gx/GXBump.h>`, as `GXLoadTexMtxImm` is only in `GXTransform.h` (F-402-1) and the texture-object functions in `GXTexture.h` (F-397-1). Include the specific header rather than declaring the function.
