# F-43-1: BWorld constructor and its inline helpers

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #43 covers `0x8005AC70`–`0x8005B814` in `game/BWorld.cpp`: the `BWorld` constructor and the 28 weak functions emitted after it.

## Constructor

`BWorld::BWorld(const Desc&)` (`0x8005AC70`, 0x4A8 bytes) matches. In order it:

- constructs `ICalcDraw` and `enable_shared_from_this<BWorld>`, stores the vtable `0x80399DA0`, zeroes the flags, handle and pending frame count, and copies the draw type;
- sizes the object slot vector from `Desc::GetObjectCapacity` and allocates the KD tree (`new`, 0x38 bytes, `CKDTree<BModelObj>` constructor `0x8005C170`);
- calls `EnableCalc(true)` (`0x8005D300`);
- resets the `ScnRoot` and the outline and model `ScnGroup` pointers, each constructed from the default allocator and owned with the `DestroyG3dObj` deleter;
- with a nonzero effect capacity, allocates `EffectData` (0x64 bytes), takes two effect group IDs from `AllocEffectGroupID` (`0x800A0970`) and sizes the effect vector. With capture enabled it looks up the `dummy_capture` texture (`.data` `0x803999B8`), initializes the capture texture over work buffer 0 (`0x800BE320`) at the EFB size in `GX_TF_RGB565`, `GX_CLAMP`, `GX_LINEAR`, saves the texture's attributes and points the texture at the capture buffer with mipmaps off, and takes a third group ID;
- for draw types 1 and 3, allocates `OutlineData` (0x38 bytes) with an `GX_TF_I8`, `GX_NEAR` capture texture over the same work buffer, width 1, alpha 0xFF and color `0xFF000000`;
- sets flag 0x80 from `Desc::IsSoundListenerEnable`.

## Supporting classes

- `CaptureTexture` (0x2C bytes): `GXTexObj` at 0, `shared_ptr<u8>` buffer at 0x20, buffer size at 0x28. Constructor `0x801C4910`, destructor `0x801C4930` and `Init(const shared_ptr<u8>&, u32, u16, u16, GXTexFmt, GXTexWrapMode, GXTexFilter)` `0x801C4B30` are in another unit; the width, height and format getters are inline calls to the GX texture-object getters.
- `WorkBuffer`: `GetBuffer` returns `shared_ptr<u8>` by value, `GetSize` the size word; `GetWorkBuffer(int)` is `0x800BE320`.
- `AllocEffectGroupID` (`0x800A0970`) and `FreeEffectGroupID` (`0x800A0BE0`) manage effect group IDs; `0xFFFFFFFF` is the invalid ID.
- `CKDTree<BModelObj>` (0x38 bytes): `shared_ptr` root node at 0, bounds at 8, origin at 0x20 and scale at 0x2C.

All of these names are descriptive.

## Code-generation rules

- The constructor calls even trivial accessors. A `#pragma dont_inline on` region around the constructor alone reproduces that; the template members it instantiates still inline their own callees, as in the original.
- `shared_ptr::enable_from_this` must be a non-varargs empty function, `(const void*, const void*)`. The varargs form is never inlined, so every `reset` grows by a call.
- `OutlineData::mColor` sits at 0x34, after the `u8` alpha at 0x30. `nw4r::ut::Color` has byte alignment, so the member needs `__attribute__((aligned(4)))`.
- The draw type comparison is signed (`cmpwi`), so `mDrawType` is `int`.

## Status

objdiff reports 59 of 196 unit functions at 100%, including the constructor and 26 of its 28 helpers. Two are left:

- `shared_ptr<EffectData>::reset` (`0x8005B360`, 69.8%): the dead deletion path in the original destroys `EffectData` inline, including the effect vector. That needs the MSL `vector` destructor and `CaptureTexture` destructor to inline; a hand-written vector destructor did not reproduce it.
- `shared_ptr<CKDTree<BModelObj>>(CKDTree<BModelObj>*)` (`0x8005B640`, 98%): the original has one more null-check branch in the dead deletion path. An explicit `~CKDTree` and an out-of-class `~shared_ptr` did not change it.

The unit stays `NonMatching`; the full DOL verification passes with the original object.

## Traps

- **A constructor that inlines nothing needs a `dont_inline` region.** Symptom: the original calls one-instruction getters out of line, while the same getters inline elsewhere in the unit. Wrap only the constructor in `#pragma dont_inline on` / `reset`.
- **Varargs empty functions do not inline.** Symptom: a `shared_ptr` constructor or `reset` is a few instructions too long, with an extra `bl`. Give `enable_from_this` fixed parameters.
- **`ut::Color` members can land on odd offsets.** Symptom: a color store at offset 0x31 instead of 0x34. Align the member to 4 bytes.
