# Issue 207: CShadowCamera unit and the block boundary

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #207 lists 33 functions in `0x8010BD30`-`0x8010D7D0`. They belong to two different units, so the issue cannot be completed by reconstructing `CShadowCamera` alone. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md).

## F-207-1: the first 25 functions belong to the CSaveManager unit

`0x8010BD30`-`0x8010C488` are weak MSL template instantiations (a container that indexes 4-byte elements in 256-element blocks through a vector of block pointers, with its allocator, `swap`, `max_size`, capacity growth and `__throw_vector_length_error` call). Their only external callers are `0x8010A8F8` and `0x8010B8E4`, both inside the CSaveManager range (#199 to #206); nothing in the shadow camera code calls any of them. The estimate in the issue (`CShadowCamera` from `0x8010BD30`) was taken from the class evidence of the next function. The functions `0x8010BD30`, `0x8010BDD0`, `0x8010BE10`, `0x8010BE60`, `0x8010BF40`, `0x8010C050`, `0x8010C080`, `0x8010C090`, `0x8010C0D0`, `0x8010C100`, `0x8010C160`, `0x8010C190`, `0x8010C220`, `0x8010C230`, `0x8010C2A0`, `0x8010C2B0`, `0x8010C300`, `0x8010C330`, `0x8010C360`, `0x8010C370`, `0x8010C390`, `0x8010C3A0`, `0x8010C3D0`, `0x8010C3E0`, `0x8010C450` and `0x8010C480` are emitted by the compiler only when the CSaveManager source that instantiates them is reconstructed. CSaveManager has no split yet, so these are left to its owner.

`0x8010C490`, the `CShadowCamera` constructor, is the first function of the next unit.

## F-207-2: game/CShadowCamera.cpp unit boundaries

Registered as `game/CShadowCamera.cpp` (descriptive file name; no source-file string is embedded). The unit holds `CShadowCamera` and `CSimpleGround`.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80028E54`-`0x800290E8` | Entries of the `extabindex` records for `0x8010C490` to `0x8010DD70`; `0x800290E8` is the entry of `0x8010DE20` |
| `extabindex` | `0x8004BC9C`-`0x8004BD44` | First record names `0x8010C490`, the next after the unit names `0x8010DE20` |
| `.text` | `0x8010C490`-`0x8010DDD8` | `0x8010DDC0` and `0x8010DDD0` are the last `CSimpleGround` vtable slots; `0x8010DDE0` starts the `CStatistic` unit continued in #209 (F-209-4) |
| `.data` | `0x803ADA30`-`0x803ADD58` | Three `mat '%s' has more than %d ...` messages, `SHADOW_RGBA`, the `shared_ptr_deleter<unsigned char, void (*)(void *)>` type names, the `CShadowCamera` vtable `0x803ADB34`, its RTTI name and base lists, the `CSimpleGround` vtable `0x803ADC90` and name. `0x803ADD58` is the `"vector length error"` literal that only `0x8010F150` of the next unit uses |
| `.sdata` | `0x804B7358`-`0x804B7368` | `CShadowCamera` and `CSimpleGround` type info; `0x804B7368`-`0x804B736B` are used by `0x801104AC` and `0x801108A0` of the next unit |
| `.sdata2` | `0x804BA0D8`-`0x804BA108` | Floats and the integer conversion double used by `0x8010C490`-`0x8010DCC0`; `0x804BA108` starts `CStageCamera` |

The unit has no `.ctors`, `.bss` or `.sbss` contribution. `0x8010DCC0` stores a byte through `0x804B9110`, but that address lies between `sCntInitialized` (`0x804B9100`) and `sCurrent__6CFiber` in the library `.sbss`; giving it to the unit produced a link-order cycle (`CShadowCamera.cpp -> ... -> cnt.c -> auto_10_804B9108_sbss -> CShadowCamera.cpp`) and the build rejected it.

## F-207-3: reconstructed functions

`CShadowCamera` derives from `BWorldCamera` and is 0x100 bytes (layout in [F-51-2](51-bworldlightset-unit.md)). Vtable `0x803ADB34` has the `BWorldCamera` slot order; it overrides only the destructor and `UpdateVisible`, which calls virtual `SetVisible`.

| Address | Name | Match |
| --- | --- | --- |
| `0x8010C490` | `CShadowCamera::CShadowCamera(u32)` | 100% |
| `0x8010C520` | `CShadowCamera::~CShadowCamera()` | 100% |
| `0x8010C5A0` | `CShadowCamera::Setup(u16 width, u16 height, int textureLevels, bool useTextureAlpha)` | 100% |
| `0x8010C750` | `CShadowCamera::UpdateVisible()` | 100% |
| `0x8010C7A0` | `CShadowCamera::CalcLightProjection()` (descriptive) | 94.8% |
| `0x8010D140` | file-local `SetupShadowMaterial` (descriptive) | 99.8% |
| `0x8010D630` | `CShadowCamera::SetupShadowModel(shared_ptr<CModel>)` (descriptive) | 94.6% |

Behaviour:
- `Setup` stores the level count and texture-alpha mode, allocates one 32-byte aligned I8 buffer for the full size (`GXGetTexBufferSize(width, height, GX_TF_I8, 0, 0)`), flushes it, and initializes `mTextures[i]` with `width >> i` by `height >> i`, clamp wrap, linear filter, sharing the buffer. The deleter is `GameMemory::Free`.
- `CalcLightProjection` rebuilds the shadow projection. It reads the current camera, the light set `mLightSetIndex` and light `mLightIndex`, normalizes the light position with `SafeNormalize` (default `(-1, 3, -1)`) and returns false if any step fails. It computes the near-plane frustum corners (frustum or perspective projection; orthographic returns false), transforms them to world space and intersects the rays from the eye through them with the plane `y = -mPlaneHeight`. When both upper rays miss the ground it requires both lower rays to hit, flattens those two points to `y = 0` and replaces the upper corners with points five units past the lower ones along the ground-plane direction from `(eye.x, 0, eye.y)` (the original reads `eye.y` for the third component; the reconstruction keeps it). It then sets the shadow camera to an orthographic `(1, -1, -1, 1)` projection with the view camera's near and far planes, the viewport to the first texture's size, the position to the corner centroid moved by `lightDir * (far - near) / 2`, an aim posture at the centroid, twists it so the longest edge of the corner quadrilateral in view space is horizontal, and finally fits the orthographic bounds to the corners' view-space extent. `UpdateVisible` passes its result to `SetVisible`.
- `SetupShadowMaterial` appends a shadow-texture stage to a copied model material: it refuses with an `OSReport` when the material would exceed 7 textures, 7 texture coordinate generators or 16 TEV stages, copies the capture texture's `GXTexObj` to texture map `numTextures`, adds a projective texture coordinate generator (map mode 2, camera index `GetIndex()`, identity flags), sets constant color 3 to the material's `SHADOW_RGBA` user value, and programs one or two TEV stages (two when texture alpha is used). Bytes `0x1E` and `0x1F` of `ResTevData` record the first added stage and the marker `195` that stops the material being processed twice.
- `SetupShadowModel` is called with a by-value `shared_ptr<CModel>` from `0x80053640` and from `BModelObj::OnEnterWorld` (`0x80057210`). It visits every material of the model's `ScnMdl`, reads the `SHADOW_RGBA` integer user datum (a `GXColor`), clamps `mAlpha` to its alpha when texture alpha is off, and calls `SetupShadowMaterial` with the last capture texture.

`SelectLight` (`0x8010D7D0`) and `CSimpleGround` belong to #208.

## F-207-4: remaining differences

- `CalcLightProjection`: the frustum extents take callee-saved registers `f31 f29 f28 f27` for top, bottom, left, right where the original has `f28 f31 f29 f27` (24 declaration orders, tan-before-near and local-tan variants did not change this), and the two constants of the first `SetOrtho` call are loaded after the near and far calls where the original loads them first into callee-saved registers.
- `SetupShadowModel`: the `TypeObj`, `ResDic` and two by-value argument temporaries sit at different stack offsets (0x08 to 0x14).
- `SetupShadowMaterial`: instruction scheduling of the post-matrix address arithmetic and of the `GXSetTevKColor` arguments (two instructions).

## Method

GNU objdump 2.42 with `-M gekko` and Python scans of `build/wpse-v1/main.elf` (call sites, small-data references, `extabindex`), dtk splits, and `tools/re/unitdiff.py`. No decompiler output was used.

## Traps

- **Library `.sbss` addresses can be referenced from game code.** A store to `0x804B9110` looked like a unit-local static; adding it to the split made dtk report a cyclic link order through `cnt.c`. Check the neighbouring symbols' owners before putting small-data bytes in a game split.
- **Check string-cluster owners before ending `.data`.** The `"vector length error"` literal directly after the last class belongs to whichever function uses it (`0x8010F150`, the next unit), not to the preceding class; ending the split there needed a reference scan, because dtk only rejected the first (wrong) end.
- **A weak-template run at the start of an issue range may belong to the previous unit.** The container instantiations at `0x8010BD30` had only CSaveManager callers; call-site scans across the whole `.text` settle which unit emits a weak function.
- **CodeWarrior stack layout.** Compiler temporaries are placed above or below named locals by creation order: earlier temporaries get higher addresses, inlined-function locals sit lowest, and aggregate-initialized `VEC3` arrays elide their temporaries. Assigning array elements from `VEC3(...)` temporaries (`corners[0] = VEC3(l, t, -n)`) reproduces the four dead temporaries and the `frsp` forwarding of the original; brace initialization does not.
- **`LINE3` copies by word only through a constructor.** `ray.P = eye` copies floats (`lfs`/`stfs`); a `LINE3(const VEC3&, const VEC3&)` constructor with member initializers copies words (`lwz`/`stw`), as the original does. The header gained that constructor.
- **`VEC3::operator/` is not folded.** `sum / 4.0f` keeps the runtime `fdivs` of `1.0f / 4.0f` that the original has; `sum * 0.25f` does not.
- **`std::min` takes references.** `std::min(mAlpha, color.a)` on `u8` reproduces the reference-returning compare (`cmplw`, select a stack address); `Min<u8>` by value does not.
- **A `GXPTTexMtx` cast changes scheduling.** `static_cast<GXPTTexMtx>(GX_PTTEXMTX0 + texCoord * 3)` keeps the matrix index in a callee-saved register before the texture copy; a plain `u32` is computed at the call.

## F-207-5: SelectLight and CSimpleGround

`SelectLight` and the `CSimpleGround` members at `0x8010D7D0`-`0x8010DDD8` are reconstructed in the same source file; see [F-208-1](208-csimpleground.md#f-208-1-selectlight-and-csimpleground).
