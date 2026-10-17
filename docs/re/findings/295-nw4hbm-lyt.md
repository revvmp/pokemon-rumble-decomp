# Issues 295 to 298: HBM nw4hbm::lyt, blocks 3 to 6

## F-295-1: unit boundaries from 0x801895E0 to 0x80191874

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). After `lyt_layout.cpp` (F-294-2), the rest of the HBM layout library is six units. Their function order and sizes match CC0 xenoblade's `config/jp/symbols.txt` and `splits.txt`, shifted by `0x1A4EF0`, with the exceptions below.

| File | `.text` | `.ctors` | `.rodata` | `.data` | `.bss` | Issues |
| --- | --- | --- | --- | --- | --- | --- |
| `lyt_material.cpp` | `0x801895E0`–`0x8018C850` | none | `0x80366E78`–`0x80366EA8` | `0x803BB7C0`–`0x803BB848` | none | #295, first nine functions of #296 |
| `lyt_pane.cpp` | `0x8018C850`–`0x8018D840` | `0x803529FC`–`0x80352A00` | `0x80366EA8`–`0x80366EC8` | `0x803BB848`–`0x803BB8B0` | `0x803E9BF8`–`0x803E9BFC` | #296 |
| `lyt_picture.cpp` | `0x8018D840`–`0x8018DD94` | `0x80352A00`–`0x80352A04` | `0x80366EC8`–`0x80366ECC` | `0x803BB8B0`–`0x803BB950` | `0x803E9C00`–`0x803E9C04` | #296, first eight functions of #297 |
| `lyt_resourceAccessor.cpp` | `0x8018DDA0`–`0x8018DDF8` | none | none | `0x803BB950`–`0x803BB968` | none | #297 |
| `lyt_textBox.cpp` | `0x8018DE00`–`0x8018F374` | `0x80352A04`–`0x80352A08` | `0x80366ED0`–`0x80366EE8` | `0x803BB968`–`0x803BBA18` | `0x803E9C08`–`0x803E9C0C` | #297 |
| `lyt_window.cpp` | `0x8018F380`–`0x80191874` | `0x80352A08`–`0x80352A0C` | `0x80366EE8`–`0x80366F00` | `0x803BBA18`–`0x803BBB00` | `0x803E9C10`–`0x803E9C14` | last six functions of #297, #298 |

Differences from xenoblade and from dtk's analysis:

- `lyt_textBox.cpp` keeps `TextBox::SetFont` (`0x8018F070`, `0x124` bytes), which xenoblade's linker stripped. It is called from the `TextBox` destructor and is followed by the weak `ut::Font` destructor (`0x8018F1A0`).
- dtk split the trailing `blr` of `Pane::GetColorElement` and `Pane::SetColorElement` into separate four-byte functions (`0x8018CC10`, `0x8018CC40`). Each real function is `0x24` bytes.
- The four `typeInfo` objects are 4 bytes each; dtk guessed 8. Each `.bss` split ends at its object, and the padding goes to the next unit.
- `lyt_picture.cpp` `.rodata` is one 4-byte `0.0f`. The next 4 bytes are padding before `lyt_textBox.cpp`'s 8-byte-aligned constants, so the split ends at `0x80366ECC`.
- `lyt_pane.cpp` `.rodata` ends with a second, unreferenced `1.0f / 255.0f`. `CalculateMtx` produces it from a local `const f32 invAlpha = 1.0f / 255.0f`, which the compiler emits once for the initializer and once for the use.

## F-295-2: class layouts and header corrections

- `ut::Color` is 4-byte aligned. Without the alignment, `Picture::mVtxColors` fell at `0xD1`, in `Pane`'s tail padding, instead of `0xD4`. All previously matched units still verify with the aligned `Color`.
- `Pane` is `0xD4` bytes: `mBasePosition` at `0xCC`, `mAlpha` `0xCD`, `mGlbAlpha` `0xCE`, `mFlag` `0xCF`, `mbUserAllocated` `0xD0`. `detail::PaneBase`'s constructor and destructor are inline; the DOL has neither.
- `Picture` is `0xEC` bytes: `mVtxColors` at `0xD4`, `mTexCoordAry` at `0xE4`. Its two new virtuals, in vtable order (`0x803BB914`), are `Append(TPLPalette*)` and `Append(const GXTexObj&)`. The imported `Picture(u8)`, `Picture(const TexMap&)` and `Append(const TexMap&)` belong to a later library and were removed.
- `TextBox` follows the layout in F-294-2. `GetVtxColorElement` indexes `mTextColors` by `idx / 8`, as the target does.
- `Window` is `0x104` bytes: `mContentInflation` at `0xD4`, `mContent` (`vtxColors`, `texCoordAry`) at `0xE4`, `mFrames` at `0xFC`, `mFrameNum` at `0x100`. `Frame` is a plain 8-byte struct. `Window` does not override `GetMaterial`; its vtable (`0x803BBA54`) holds `Pane::GetMaterial`, then `GetContentMaterial`, `GetFrameMaterial`, `DrawContent`, `DrawFrame`, `DrawFrame4` and `DrawFrame8`.
- `Material` stores textures as `GXTexObj` (`0x20` bytes each), not `TexMap`; `lyt_texMap.h` was a later-library header and is removed. The out-of-line members in the DOL are `GetTexSRTAry`, `GetTexCoordGenAry`, `GetIndTexSRTAry`, `SetTextureNum`, `SetTexCoordGenNum`, `SetTextureNoWrap(u8, TPLPalette*)` and `SetTexture(u8, const GXTexObj&)`. `SetupGX` treats `GX_TF_C4` and `GX_TF_C8` as palette formats.
- `NW4R_MATH_DEG_TO_RAD` uses a single-precision pi. `Pane::CalculateMtx` multiplies by the `f32` constant `pi / 180`.
- The material resource types (`ChanCtrl`, `AlphaCompare`, `BlendMode`, `IndirectStage`, `TevStageInOp`, `TevStage`, `TevSwapMode`, `TexCoordGen`) are classes with private data, as in the reference.
- Anonymous-namespace helpers are at global scope (`CalcLineRectImpl<w>__25@unnamed@lyt_textBox_cpp@...`, `CalcTextureMtx__26@unnamed@lyt_material_cpp@...`).
- `revolution/gx/GXGet.h` gains `GXGetTexObjUserData`, `GXGetTexObjWidth`, `GXGetTexObjHeight`, `GXGetTexObjWrapS` and `GXGetTexObjWrapT`; `GXTexture.h` gains `GXInitTexObjWrapMode`. The signatures are those of the matched `src/RVL_SDK/gx/GXTexture.c`.

## F-295-3: match status

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0. objdiff:

- `lyt_pane.cpp`: 28/28 functions 100%, `.rodata` 100%. `Matching`.
- `lyt_picture.cpp`: 11/11 functions 100%. `Matching`.
- `lyt_resourceAccessor.cpp`: 3/3 functions 100%. `Matching`.
- `lyt_textBox.cpp`: 17/17 functions 100%, `.rodata` 100%. `Matching`.
- `lyt_window.cpp`: `.text` 98.8%; 19 of 21 functions are 100%. `DrawFrame` is 92.5% and the constructor 99.2%. `NonMatching`.
- `lyt_material.cpp`: `.text` 94.7%, `.rodata` and `.data` 100%; 19 of 23 functions are 100%. The constructor is 86.5%, `ReserveGXMem` 96.3%, `SetTexCoordGenNum` 97.6% and `SetupGX` 99.99%. `NonMatching`.

The `.data` sections of the matching units score below 100% in objdiff only from weak RTTI records and vtables; the linked DOL is byte-identical.

Names come from xenoblade's symbols. The code follows CC0 Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, `src/hbm/nw4hbm/lyt`).

## Traps

- **A POD struct assignment is a block move that the scheduler reorders freely.** In the `Material` constructor, the original copies `mTevCols` from the resource with every load followed by its store. The same copy written as `mTevCols[i] = pRes->tevCols[i]` (a C `GXColorS10` assignment) hoists all loads above all stores, and with `SetName` moved it even removes `Init`'s dead stores. Writing the copy member by member restores the serial order. Class types such as `math::VEC3` copy member by member already. Evidence: F-295-3, the remaining constructor difference.
- **Declaration order did not fix `Window::DrawFrame`.** A search over all orders of its five locals left the same scheduling difference. Binding the texture size to a `const Size&` temporary moved the stack slots to the target's positions but did not remove the register differences.
