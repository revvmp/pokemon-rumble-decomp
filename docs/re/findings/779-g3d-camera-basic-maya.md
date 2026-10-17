# F-779-1: NW4R g3d camera, basic and Maya units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #779 owns `g3d_camera.cpp`, `dcc/g3d_basic.cpp` and `dcc/g3d_maya.cpp`, 39 functions from `0x80264720` to `0x802665F4`.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/g3d/g3d_camera.cpp` | `.text` | `0x80264720–0x8026561C` | 21 `Camera` members, from the constructor to `UpdateCameraMtx` |
| | `.sdata2` | `0x804BACA8–0x804BACD8` | float and double constants |
| `nw4r/g3d/dcc/g3d_basic.cpp` | `.text` | `0x80265620–0x802658E8` | `detail::dcc::CalcTexMtx_Basic`, `CalcWorldMtx_Basic` |
| | `.sdata2` | `0x804BACD8–0x804BACE4` | three float constants |
| `nw4r/g3d/dcc/g3d_maya.cpp` | `.text` | `0x802658F0–0x802665F4` | 14 anonymous-namespace texture-matrix helpers, `CalcTexMtx_Maya`, `CalcWorldMtx_Maya_SSC_Apply` |
| | `.rodata` | `0x8036C8B8–0x8036C8F0` | the two local function tables of `CalcTexMtx_Maya` (0x1C each) |
| | `.sdata2` | `0x804BACE8–0x804BACFC` | five float constants |

`g3d_xsi.cpp` (#780) follows at `0x80266600`. objdiff reports 100% for all 39 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler: Wii/1.1 with `cflags_nw4r` (F-25-1). The linker removed the camera's unused members (`SetPosition(f32, f32, f32)`, `SetCameraMtxDirectly`, `SetProjectionMtxDirectly`, `SetTexMtxParam`, `SetScissor`, `SetScissorBoxOffset`, `SetViewportZRange`, `Project`, `UpdateProjectionMtx`) and the two constants only they use, so the camera `.sdata2` split is 0x30 of the object's 0x38 bytes.

## Source

All three files are CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee` (`src/nw4r/g3d/g3d_camera.cpp`, `dcc/g3d_basic.cpp`, `dcc/g3d_maya.cpp`), stripped of comments, with their includes replaced; `include/nw4r/g3d/g3d_basic.h` and `g3d_maya.h` are ss's headers, stripped of comments. ss's camera lacks three members this executable keeps:

- `Camera::GetPosture` (`0x80264B30`) follows CC0 Swiftshine/kdc `src/nw4r/g3d/g3d_camera.cpp` (`main`, fetched 2026-10-17).
- `Camera::GetPosition` (`0x802648E0`) and `Camera::SetFrustum` (`0x80264C30`) are in no public reference. They are written after their neighbours: `GetPosition` copies `cameraPos` after the null and validity checks, and `SetFrustum` is `SetOrtho` with `GX_PERSPECTIVE` and `FLAG_PROJ_FRUSTUM`, which the target's `li r0, 0` and `ori r0, r0, 0x10` confirm. Both compile to the target bytes.

`include/nw4r/g3d/g3d_camera.h` gains the three declarations. `include/revolution/gx/GXTransform.h` gains `GX_PROJECTION_SZ`, `GX_VIEWPORT_SZ`, the `GXInterlaceField` enum and `GXSetViewportJitter`, as in ss's `rvl/GX/GXTransform.h` and the existing `src/RVL_SDK/gx/GXTransform.c`.

## Traps

None new. As F-397-1 and F-775-1 found, the imported GX headers lack declarations that NW4R uses; extend the specific SDK header.
