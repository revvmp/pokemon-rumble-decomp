# F-42-1: BWorld unit boundaries, descriptor and texture state

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #42 covers `0x8005A8D0`–`0x8005AC6C`, the tail of `game/BModelObj.cpp` and the first functions of `game/BWorld.cpp`.

## Unit boundaries

`game/BWorld.cpp` starts at `0x8005AAA0`, eight bytes after the last `BModelObj.cpp` function (`@72@__dt__9BModelObjFv`, `0x8005AA90`). The functions from `0x8005AAA0` to `0x8005AC6C` are not BModelObj code: `0x8005AAA0`–`0x8005AB40` build and fill the descriptor that other units pass to the BWorld constructor (callers at `0x800A4EF0`, `0x800A8680`, `0x800AC4B0`, `0x800CD800` and `0x800F60C0` fill it and then call `new BWorld` with `0x90` bytes), and `0x8005AB50`–`0x8005AC6C` are only used by the constructor at `0x8005AC70`.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x800078BC`–`0x80009DDC` | Follows `BModelObj.cpp`; `BWorldCamera.cpp` starts at `0x80009DDC` |
| `extabindex` | `0x80044514`–`0x80044AB4` | Same ordering as `extab` |
| `.text` | `0x8005AAA0`–`0x80066454` | Ends with the last function before `BWorldCamera` at `0x80066460` (`dtor_80066390`, 0xC4 bytes) |
| `.data` | `0x803999B8`–`0x80399F28` | Starts with the `dummy_capture` texture name; `BWorldCamera` data starts at `0x80399F28` |
| `.sdata` | `0x804B5AF0`–`0x804B5B70` | RTTI records for BWorld and its deleters |
| `.sdata2` | `0x804B9CA8`–`0x804B9CE8` | Constants used by the unit |

A split-only build with these ranges passes full DOL verification.

## Constructor descriptor

`BWorld::Desc` is 0x1C bytes: six words and two flags, all zeroed by its constructor (`0x8005AAA0`) and written by one setter each (`0x8005AAD0`–`0x8005AB40`). The constructor reads them through inline getters emitted after it (`0x8005B120`–`0x8005B190`, `0x8005B270`). Names are descriptive.

| Offset | Member | Use in the constructor |
| --- | --- | --- |
| 0x0 | `mDrawType` | Copied to `BWorld+0x18`; types 1 and 3 create the outline data |
| 0x4 | `mObjectCapacity` | Initial size of the object vector at `BWorld+0x58` |
| 0x8 | `mOutlineCapacity` | Capacity of the outline `ScnGroup` at `BWorld+0x24` |
| 0xC | `mModelCapacity` | Capacity of the model `ScnGroup` at `BWorld+0x2C` |
| 0x10 | `mScnObjCapacity` | Scene object capacity of the `ScnRoot` at `BWorld+0x1C` |
| 0x14 | `mEffectCapacity` | Nonzero creates the effect data and sizes its effect vector |
| 0x18 | `mCaptureEnable` | Captures the screen into the `dummy_capture` effect texture |
| 0x19 | `mSoundListenerEnable` | Sets world flag 0x80, which makes `Calc` place the 3D sound listener at the current camera |

## Texture state

`BWorld::TextureState` (0x18 bytes, at `EffectData+0x48`) saves the attributes of an effect texture before the constructor redirects it to the capture buffer, and the destructor writes them back. `Save` (`0x8005AB50`) takes the `nw4r::ef::ResTexture` by value; the original reloads the texture pointer for every field, which only a by-value parameter reproduces. It records a palette when the format is `GX_TF_C4`, `GX_TF_C8` or `GX_TF_C14X2`; the three-way comparison compiles to the original `(u8)(format + 248) <= 2` test.

## Emission order

The constructor inlines nothing: every `shared_ptr`, `vector` and descriptor accessor is a call. A `#pragma dont_inline on` region around it reproduces that. Inline functions that are not inlined are emitted after the first function that referenced them, even if that function inlined them. That explains why `ResTexture::IsValid` (`0x8005AC00`) sits after `Save`, and why the setters `SetWidth`, `SetHeight`, `SetFormat`, `SetMipmap` and `SetTexture` (`0x8005AC20`–`0x8005AC60`) sit before the constructor: they follow `TextureState::Restore`, which the destructor inlines and whose own copy the linker strips.

## Header

`include/game/BWorld.h` declares the whole class (0x90 bytes) from the constructor, destructor and member accesses across the unit; see [symbols](../SYMBOLS.md#game-bworld-42). Element types and member names follow their uses: the camera vector and weak references, the 16-byte object slot entries with a linked list through `mObjHead`/`mObjTail`, the deferred-release set and the model KD tree. `EffectData` and `OutlineData` stay forward declarations until their capture-texture member class is reconstructed.

## Status

The 9 descriptor functions and `Save` match (objdiff, game flags; no per-unit flags needed yet). `IsValid` and the five setters match too, but they are only emitted once the constructor exists (#43).

## Traps

- **Out-of-line inline copies follow their first reference.** Symptom: a weak inline function sits before the function that calls it out of line. Look for an earlier function that inlined it, possibly one the linker stripped.
- **A by-value class parameter blocks load reuse.** Symptom: the original reloads a pointer member before every field access. Pass the wrapper by value instead of by `const` reference.
- **`format + 248` is a three-way equality test.** Symptom: `addi r0, rX, 0xF8` followed by an unsigned range test. Write the comparisons against the three palette formats.
