# Script camera safe-frame bindings (#234)

Target `wpse-v1`. Issue #234 lists 22 functions in `.text` `0x80133210`–`0x80133E94`, inside `game/ScriptCamera.cpp` ([F-231-1](231-script-camera-unit.md#f-231-1-unit-boundaries)). `src/game/ScriptCamera.cpp` reconstructs all 22, and objdiff shows each at 100%. The unit stays `NonMatching` until #235 and #236 are written.

## F-234-1: the functions

| Address | Identity | Evidence |
| --- | --- | --- |
| `0x80133210` | `RAY3::Normalize` | Tail call to `VEC3Normalize(&d, &d)` |
| `0x80133220` | `nw4r::math::VEC3Normalize` | `PSVECNormalize` |
| `0x80133280` | `PLANE(f32, f32, f32, f32, bool)` | Stores `n` and `d`, normalizes unless the flag is set |
| `0x801332E0` | `PLANE::Normalize` | `FrSqrt(VEC3LenSq(&n))`, scales `n` and `d` |
| `0x80133340` | `VEC3LenSq` | Paired-single inline |
| `0x80133360` | `method_camGetXYZOf` | Binding string; pops the result reference, a corner index 0–7 and a camera, and calls `GetSafeFrameGroundPos` with aspect 0 or 1 and corners (±1, ±1) through the jump table `0x803B65A0` |
| `0x801334D0`, `0x80133500` | `PopReference`, `PopInt` | Existing inline members |
| `0x80133530`, `0x80133570` | Pointer `DynamicCast<BWorldCamera>`, `IsKindOf<BWorldCamera>` | `GetType() & 2` |
| `0x801335C0` | `IsTV16x9` | Binding string; pushes `SCGetAspectRatio() == SC_ASPECT_RATIO_16x9` |
| `0x80133610` | File-local `GetSafeFrame(BWorldCamera*, u32 aspect)` | Returns a 0x18-byte `SSafeFrame` (top, bottom, left, right, near, far) of the safe display area at the near plane |
| `0x801337B0` | File-local `MakePlane(PLANE*, const VEC3&, const VEC3&, const VEC3&)` | Normal from `Cross(p2 - p0, p1 - p0)`, rejects zero and non-finite normals, `d = -n.Dot(p0)` |
| `0x801338A0`, `0x80133900` | Game `Cross(const VEC3&, const VEC3&)`, `nw4r::math::VEC3Cross` | `PSVECCrossProduct`; `VEC3Cross` is also called from `DrawDirectionalBillboard` |
| `0x80133980` | Game `SafeNormalize(VEC3*, const VEC3*)` | Zero test, `VEC3Normalize` into a local, three `isfinite` tests (`__fpclassifyf > 2`), then assignment; also called by `method_moveAlongWall` |
| `0x80133A40`, `0x80133A50` | `VEC3::Dot`, `nw4r::math::VEC3Dot` | Tail call; paired-single dot product |
| `0x80133A70` | File-local `IsInFrontOfPlane(const VEC3&, const PLANE&, f32 margin)` | `-margin <= pos.Dot(n) - (-d)` |
| `0x80133AE0` | File-local `IsObjInSafeFrame(NPpu::CEngine*, u32 aspect)` | See F-234-2 |
| `0x80133E40`, `0x80133E70` | `PLANE()` and its implicit assignment | Default constructor calls `VEC3()`; assignment copies four words |

Names of the file-local functions, `SSafeFrame`, `SSafeFrameCache`, `Cross`, `SafeNormalize`, `BWorldObj::GetPositionRef` (`0x80098850`, returns `&mPosition` at 0x30) and `VEC3::Dot` are descriptive, medium confidence. Binding names are original.

## F-234-2: the safe-frame cache

`IsObjInSafeFrame` pops a margin, an object and a camera. It keeps the four side planes of the safe frame in a file-scope object at `0x803B65C0` (0x58 bytes): the aspect (`u8`, initialized to 0xFF in `.data`), the camera projection flags, top, bottom, fovy and near, then four `PLANE`s, which the static initializer `0x801352A0` (#236) default-constructs. When the aspect or projection differs from the cache, it rebuilds the planes from `GetSafeFrame` with `MakePlane` and stores them. It then transforms the object position into view space and pushes whether it is in front of all four planes.

The bindings `method_camIsObjInSafeFrame`, `method_camIsObjInSafeFrame4x3` and `method_camIsObjInSafeFrame16x9` (`0x80133EA0`–`0x80133EF0`) belong to #235. They are written here because `IsObjInSafeFrame` and its helpers are emitted only when a binding calls them; all three are at 100%.

## Method

GNU objdump on the split object and `build/wpse-v1/main.elf`, `tools/re/unitdiff.py`, and `configure.py`, `ninja` and `tools/verify/verify.py`. No decompiler output was used.

## Traps

- **Static helpers of one issue can be called only from the next.** `IsObjInSafeFrame` and seven helpers in #234 have no caller before `0x80133EA0` (#235). Write the calling binding before judging them.
- **Statement order reorders loads of adjacent fields.** In `GetSafeFrame` only `frame.near = ...; scale = ...; frame.far = ...;` reproduced the original load order and registers; aggregate initialization and adjacent assignments reached 97.6% to 99.8%.
