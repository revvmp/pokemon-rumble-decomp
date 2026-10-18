# Script camera projection bindings (#233)

Target `wpse-v1`. Issue #233 lists 21 functions in `.text` `0x801325D0`–`0x80133204`, inside `game/ScriptCamera.cpp` ([F-231-1](231-script-camera-unit.md#f-231-1-unit-boundaries)). `src/game/ScriptCamera.cpp` reconstructs all 21, and objdiff shows each at 100%. The unit stays `NonMatching` until #235 and #236 are written.

## F-233-1: the functions

| Address | Identity | Evidence |
| --- | --- | --- |
| `0x801325D0`–`0x80132710` | `method_camSetFovy`, `method_camSetNear`, `method_camSetFar` | Binding strings; pop one float and call `Camera::SetPerspective` with the other three values from `CameraData` (`projFovy` 0xAC, `projAspect` 0xB0, `projNear` 0xB4, `projFar` 0xB8) |
| `0x801327B0` | `method_camGetPostureType` | `Camera::GetPosture`, pushes `tp` or 0 |
| `0x80132830`, `0x80132900`–`0x80132D00` | `method_camGetUpX` to `method_camGetTwist` | Push one `PostureInfo` field; the null-camera default is 0.0f, except `GetUpY` (1.0f) |
| `0x801328B0`, `0x801328F0` | `NPpu::CEngine::PushFloat`, `NPpu::CValue::SetFloat` | Existing inline members; first used by `method_camGetUpX` |
| `0x80132D80`–`0x80132E80` | `method_camGetFovy`, `method_camGetNear`, `method_camGetFar` | Push a `CameraData` field, or 20.256166f, 0.01f and 1000.0f |
| `0x80132F00` | File-local `GetSafeFrameGroundPos(CValue*, BWorldCamera*, u32 aspect, int x, int y)` | Called only by `method_camGetXYZOf` (#234). Projects a safe-frame corner through the camera onto the plane y = 0 and stores X, Y and Z in three `CValue`s |
| `0x801331A0` | `nw4r::math::RAY3(const VEC3&, const VEC3&, bool)` | Copies both vectors and normalizes the direction unless the flag is set |

`method_camGetFar` reads `projFovy` (offset 0xAC), not `projFar`. This is the original behavior and is reproduced as written.

## F-233-2: shared inline helpers

The unit is compiled with `dont_inline`, so every helper below is called out of line. They are weak functions emitted first by `CDlgPiiSelector.cpp`, so `symbols.txt` gives them the names this unit emits: `ref` of `ResCommon<CameraData>` `0x80098660`, the `MTX34` constructor `0x80074A30` and conversions `0x800986F0` and `0x80096950`, the `VEC3` constructors `0x80096990` and `0x8009A050` and conversions `0x80096960` and `0x80096970`, `TanDeg` `0x80098700`, `TanFIdx` `0x80098710`, `MTX34Inv` `0x800986A0` and `VEC3Scale` `0x8009CAC0`.

- `0x80098670` is a `bool` wrapper of `MTX34Inv` (`neg`/`or`/`srwi` of the `u32` result). It is the game inline `Inverse` in `include/game/MathUtil.h` (descriptive, medium).
- `0x80096870` builds a `VEC3` and calls `VEC3Transform`. It is the game inline `Transform(const MTX34&, const VEC3&)`, moved from a static copy in `CStageCamera.cpp` to `MathUtil.h`; `CStageCamera.cpp` inlines it either way and still verifies.
- `TanFIdx` calls `tanf` (`0x80098720`, `tan` and `frsp`), so the NW4R header now uses `tanf`.
- `0x801B6BE0` and `0x801B6590` are declared `GetSafeDisplaySize(u16*, u16*, u32, u32)` and `GetPixelAspectRatio(u32, u32)` in `include/game/RenderMode.h`, after the diagnostic names of [F-318-1](318-game-utilities.md).

`0x800968D0` stays unnamed: `VEC3Transform` is already the name of `0x8023F120`.

## Method

GNU objdump on the split object and `build/wpse-v1/main.elf`, `tools/re/unitdiff.py`, and `configure.py`, `ninja` and `tools/verify/verify.py`. No decompiler output was used.

## Traps

- **Commutative operand order did not follow the source.** `TanDeg(x) * near` and `near * TanDeg(x)` both emitted `fmuls f1,f0,f1`; the original `fmuls f1,f1,f0` came from `top = TanDeg(x); top *= near;`. Likewise `/ 2.0f` reproduced `diff / height * 0.5f` where `* 0.5f` and `0.5f *` did not.
- **A named temporary moves the stack frame.** The `VEC3` passed to `Transform` had to be a named local to sit between the ray and the transformed point.
- **`VEC3Transform` is already taken.** Renaming the weak `0x800968D0` to it created a duplicate symbol; check `symbols.txt` for an existing name before renaming a weak helper.
