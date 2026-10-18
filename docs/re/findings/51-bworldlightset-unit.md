# F-51-1: BWorldLightSet.cpp unit split

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #51 covers `0x80068610`–`0x8006A100`, which is exactly one source unit, `game/BWorldLightSet.cpp`. The unit is split and registered `NonMatching`; the full DOL still verifies.

| Section | Range |
| --- | --- |
| `extab` | `0x8000A26C`–`0x8000A6DC` |
| `extabindex` | `0x80044C04`–`0x80044D00` |
| `.text` | `0x80068610`–`0x8006A100` |
| `.data` | `0x8039A160`–`0x8039A2B8` |
| `.sdata` | `0x804B5B98`–`0x804B5BB0` |
| `.sdata2` | `0x804B9D10`–`0x804B9D28` |

## Evidence

- `0x80068610` stores the `BWorldLightSet` vtable (`.data` `0x8039A208`, 0x48 bytes, RTTI `.sdata` `0x804B5BA0`) and calls the `BWorldObj` constructor at `0x8006A100`. RTTI bases: `BWorldObj`, `ICalc`, and `std::tr1::enable_shared_from_this<BWorldObj>`.
- The previous unit, `BWorldCamera.cpp`, ends at `.text` `0x80068610` and `.data` `0x8039A160` ([F-50-1](50-bworldcamera-unit.md)). This unit's data begins with the `shared_ptr_deleter<BWorldLightSet::CMotionProgram, default_delete<...>>` vtable at `0x8039A160`. Its entries are the destructor `0x8006A0C0`, `get_deleter` `0x80069E80` and `dispose` `0x80069ED0`. The data then holds the `BWorldLightSet` and `enable_shared_from_this<BWorldObj>` vtables and the `default_delete<BWorldLightSet::CMotionProgram>` type name, which ends at `0x8039A2B8`. That address is the `BWorldObj` vtable, used by the `BWorldObj` constructor `0x8006A100`, which starts #52's unit.
- `.sdata` holds the deleter type info (`0x804B5B98`), the `BWorldLightSet` type info (`0x804B5BA0`) and the `default_delete` type info (`0x804B5BA8`, used by `0x80069E80`). `.sdata2` `0x804B9D10`–`0x804B9D20` is used only by `0x80068B50`, `0x800696A0` and `0x80069D80`. There is no `.bss`, `.sbss` or `.ctors` contribution.
- The only foreign data reference is the `shared_ptr_deleter<nw4r::g3d::AnmScnRes, ...>` vtable at `0x80399A88`, which is `BWorld.cpp`'s weak first copy.

## Blocker

Reconstruction needs the same MSL C++ `tr1::shared_ptr` and `enable_shared_from_this` templates as `BWorldCamera.cpp` (#53, #327), plus the `ICalc` and `BWorldObj` declarations (PR #906 for `ICalc`). Source is deferred until those land.

## Method

Python reading the linked `main.elf` (byte-identical to the original DOL) for vtables, RTTI, `extabindex`, and `lis`/`addi`, `r13` and `r2` references. No decompiler output was used.

## Traps

No new trap beyond [F-50-1](50-bworldcamera-unit.md#traps).

# F-51-2: Shared interfaces required by light-set reconstruction

The shared smart-pointer, list, `ICalc` and `BWorldObj` declarations now exist. Native instruction review establishes two additional shared interfaces before adding the light-set implementation. This prerequisite adds no source unit or code credit and leaves both existing source selections and all original inputs unchanged.

`CFileData::IsValid() const` is an inline descriptive accessor returning `!mFailed && mData != nullptr`. Both the light-set loader at `0x80068880` and length query at `0x80069600` first require a nonempty shared pointer, then read the existing failed byte at `0x28` and data pointer at `0x1C`. It adds no storage or virtual slot. The existing complete `CFileData` storage declaration is preserved.

`CShadowCamera` derives from the existing 0x68-byte `BWorldCamera`. Its constructor `0x8010C490` ORs type bit4, calls the base constructor and constructs three 0x2C-byte `CaptureTexture` objects at `0x6C`. Its destructor `0x8010C520` destroys that exact array before calling the base destructor. The complete directly evidenced storage is:

| Offset | Type and descriptive member | Native evidence |
| --- | --- | --- |
| `0x68` | `int mTextureLevels` | `0x8010C5A0` stores an integer level count, then loops over that many textures with 0x2C stride |
| `0x6C-0xF0` | `CaptureTexture mTextures[3]` | Constructor/destructor array records and texture-initialization calls |
| `0xF0` | `bool mUseTextureAlpha` | Initialized true; passed to the shadow-material helper and bypasses alpha minimization when true |
| `0xF1` | `u8 mAlpha` | Initialized 255; material update minimizes it against the observed material alpha when texture-alpha mode is false |
| `0xF2` | `bool mAutoSelectLight` | Initialized true; both `BWorldLightSet::Calc` and its frame-transition path test it before `0x8010D7D0` |
| `0xF4` | `int mLightSetIndex` | Written by the light-selection scan and passed to `ScnRoot::GetLightSet` |
| `0xF8` | `int mLightIndex` | Written by the same scan and passed to `LightSet::GetLightObj` |
| `0xFC` | `f32 mPlaneHeight` | Initialized zero; the visibility calculation negates it into the fourth component of the `(0,1,0,d)` projection plane |

Implicit alignment after the bool places the first light index at `0xF4`; the complete class is 0x100 bytes. No padding array or opaque offset class is added. All member spellings and new accessor/method names are descriptive, medium confidence in behavior and low in original spelling. Existing class names are original RTTI spellings.

The vtable at `0x803ADB34` has the unchanged `BWorldCamera` slot order. Only its destructor at `0x8010C520` and `UpdateVisible` at `0x8010C750` override inherited entries. The direct `SelectLight` method at `0x8010D7D0` scans light sets and selects the first diffuse light, storing its set/light indexes; on failure it stores both indexes zero. It does not add a virtual slot. #207 owns the constructor and #208 owns this selection routine; neither routine is implemented or credited by this header prerequisite.

## Validation

The established game compiler and flags remain unchanged. `python configure.py --map`, the normal default Ninja build after deleting only `main.dol`, and the explicit frozen-manifest verifier all exit0. Of703 existing source objects,700 retain identical whole-file hashes. The three consumers BWorld, SaveData and sound have different compiler metadata/local labels; independent control compilations with only the two original headers substituted prove every allocated section and every relocation target identical. Local relocation targets are compared by section plus offset, external ones by name plus addend, without dropping opcode/register bits. The complete DOL is unchanged. The later light-set implementation must prove native uses of the two accessors and the selection call, in addition to its whole-unit source acceptance.

## Traps

- An earlier split-only blocker can become stale after shared templates land, but inspect the full unit before assuming it is independent: `BWorldLightSet` also consumes the file readiness byte and the complete shadow-camera interface.
