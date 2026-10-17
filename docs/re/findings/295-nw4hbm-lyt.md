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
- `lyt_window.cpp`: `.text` 98.8%; 20 of 21 functions are 100%. `DrawFrame` is 92.5%. `NonMatching`.
- `lyt_material.cpp`: `.text` 96.5%, `.rodata` and `.data` 100%; 22 of 23 functions are 100%. The constructor is 89.9%. `NonMatching`. Superseded by F-295-4: the unit is now `Matching`.

The second pass matched `ReserveGXMem` with the allocation size written as `4 * (matColNum + chanCtrlNum + texCoordGenNum + tevSwapNum + alpCompNum + blendModeNum + indStageNum)` plus the `int`-cast sizes of the larger arrays, as in CC0 `zeldaret/tp` (`libs/revolution/src/homebuttonLib/nw4hbm/lyt/lyt_material.cpp`). `SetTexCoordGenNum`, `SetupGX` and the `Window` constructor matched once their array pointer locals were declared `const` (`TexCoordGen* const`, `TexSRT* const`, `const u32* const`).

In the remaining constructor difference, the original copies each structure (`ChanCtrl`, the material colour, `TevSwapMode`, the `math::VEC2` members of `TexSRT`) with every load followed immediately by its store in one register. The rebuild loads several fields before storing them. `Wii/1.0`, `Wii/1.0RC1`, `Wii/1.1`, GameCube 3.0a3 and 3.0a5.2, `-ipa function`, `-inline deferred`, `-opt nopeephole` and `-schedule off` all score lower than Wii/1.0a with `cflags_hbm`. Setter calls, `u32` indices, literal bounds, an explicit `mAnimList()` initializer and `const` pointer locals did not remove the difference. In `DrawFrame`, the scheduling of the inlined `GetLTTexCoord` table loads differs; a `const` texture size, `const` vertex colours, scoped blocks and a hoisted vertex-colour pointer did not help.

The `.data` sections of the matching units score below 100% in objdiff only from weak RTTI records and vtables; the linked DOL is byte-identical.

Names come from xenoblade's symbols. The code follows CC0 Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, `src/hbm/nw4hbm/lyt`).

## F-295-4: material constructor match

`lyt_material.cpp` is now `Matching`: objdiff shows 23 of 23 functions at 100%, and `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with the unit linked from source. Three source changes closed the constructor gap, and a fourth fixed the link.

- **The resource key colours are `ut::Color`.** `res::Material::tevKCols` is declared `ut::Color[GX_MAX_KCOLOR]`, and the constructor copies them with `mTevKCols[i] = pRes->tevKCols[i].ToU32()`. With the earlier `GXColor` declaration, the compiler moved the key colour loads and the `resNum` load (`pRes + 0x3C`) above the key colour stores, which then pulled the resource-count arithmetic into the colour copies. Declared as `ut::Color`, each key colour load waits for the previous store, and the `resNum` load follows the last one, as in the target. A plain `mTevKCols[i] = pRes->tevKCols[i]` uses the implicit `ut::Color` copy, which copies bytes; `.ToU32()` and `static_cast<u32>` both give the target word copy. The layout is unchanged (`0x2C`, 16 bytes).
- **The TEV colours are copied by structure assignment.** With the key colour order fixed, `mTevCols[i] = pRes->tevCols[i]`, as in CC0 `zeldaret/tp`, matches; member-by-member copying is no longer needed.
- **The constructor sets the texture coordinate generator count itself.** It takes `GetTexCoordGenAry()` once, then fills new entries with `TexCoordGen()` and stores `mGXMemNum.texCoordGen` inside `if (texCoordGenNum)`, instead of calling `SetTexCoordGenNum`. Calling the inline member merged its pointer with the constructor local as `mpGXMem + (texMap * 0x20 + texSRT * 0x14)`; the target, like the out-of-line `SetTexCoordGenNum`, computes `texMap * 0x20 + (mpGXMem + texSRT * 0x14)`, and the register numbers of the pointer and index were swapped. The out-of-line `SetTexCoordGenNum` is unchanged and still matches.
- **An unreferenced default constructor places `ut::Color::Color()`.** The target emits the weak `__ct__Q36nw4hbm2ut5ColorFv` at `0x80189910`, before the `Material` constructor. In the rebuilt object it followed the `Material` constructor, and the DOL first differed at file offset `0x139BD0`. `Material::Material()` (`Init()` and clearing `mName`), defined before the resource constructor as in CC0 `zeldaret/tp`, is the first user of `ut::Color()`, so the weak constructor follows it. The linker dead-strips the unreferenced `Material()`, and the DOL is byte-identical.

Compilers checked on the final source: Wii/1.0a and Wii/1.0RC1 give the target constructor; Wii/1.0 and Wii/1.1 do not. CC0 Kirby's Return to Dreamland (`Etrainquandt17/Whippy`, `homebuttonLib/nw4hbm/lyt/lyt_material.cpp`, Wii/1.0) has the `tp` constructor; with our headers and Wii/1.0a that form still differs in the key colour and texture coordinate generator code, so the HBM copy in this game differs from that one.

## Traps

- **A POD struct assignment is a block move that the scheduler reorders freely.** In the `Material` constructor, the original copies `mTevCols` from the resource with every load followed by its store. The same copy written as `mTevCols[i] = pRes->tevCols[i]` (a C `GXColorS10` assignment) hoists all loads above all stores, and with `SetName` moved it even removes `Init`'s dead stores. Writing the copy member by member restores the serial order. Class types such as `math::VEC3` copy member by member already. Evidence: F-295-3, the remaining constructor difference. Superseded by F-295-4: the interleaving came from the key colour declaration, and with `res::Material::tevKCols` declared `ut::Color` the structure assignment matches.
- **A `const` pointer local can change register allocation.** `Material::SetTexCoordGenNum` (97.6%), `Material::SetupGX` (99.99%) and the `Window` constructor (99.2%) differed only in register numbers. Declaring the array pointer local `const` (`TexCoordGen* const pTexCoordGens`) made each 100%. Try this before reordering declarations when only register numbers differ. Evidence: F-295-3.
- **Declaration order did not fix `Window::DrawFrame`.** A search over all orders of its five locals left the same scheduling difference. Binding the texture size to a `const Size&` temporary moved the stack slots to the target's positions but did not remove the register differences.
- **A resource member type can decide instruction scheduling.** In the `Material` constructor, declaring `res::Material::tevKCols` as `GXColor` instead of `ut::Color` let the compiler hoist later loads above the key colour stores and moved unrelated arithmetic into the copies, about 250 differing instructions from one declaration. When copies interleave with later code, try the declared type of the source member before reordering statements. Evidence: F-295-4.
- **An inline member call can give a different address expression from the same code written in place.** Calling `SetTexCoordGenNum` merged its array pointer with the constructor local into a differently associated sum and swapped two registers; writing the body in the constructor matched. Evidence: F-295-4.
- **objdiff 100% can still fail the link through weak function placement.** A weak inline (`ut::Color::Color()`) is emitted after the first function in the file that uses it. If the target has it earlier, look for an unreferenced function that the linker dead-strips, such as a default constructor. Evidence: F-295-4.
