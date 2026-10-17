# F-399-1: Character animation object reconstruction

## Unit and issue boundaries

`nw4r/g3d/g3d_anmchr.cpp` owns `.text` `0x802580D0`–`0x80258D8C`, 26 retained functions and 3,092 function bytes. Its text extent includes 168 bytes of native inter-function alignment. The last two methods, `AnmObjChr::GetTypeName` at `0x80258D50` and `GetTypeObj` at `0x80258D80`, belong to #400 and are supplied through this first-owner unit by coordination with its owner. Conversely, #398 supplies #399's first two `AnmObjTexSrt` RTTI methods at `0x80258090` and `0x802580C0`, sixty bytes, through the preceding source unit. No shared method receives duplicate source-byte credit.

| Section | Range | Native contents |
| --- | --- | --- |
| `.rodata` | `0x8036C7C8`–`0x8036C7F0` | `AnmObjChr` and `AnmObjChrRes` type-name objects, followed by four alignment bytes |
| `.data` | `0x803C5C18`–`0x803C5CC8` | Resource and base class vtables, 0x58 bytes each |
| `.sdata2` | `0x804BABD0`–`0x804BABE0` | Zero, one, and the signed-integer conversion double |

The resource type-name object is twenty bytes, not the survey's twenty-four: the final four zero bytes align the following unit. Its corrected symbol extent leaves those bytes as padding. The existing shared play-policy table is already owned by `g3d_anmvis.cpp`; this unit references its weak definition without taking another split or receiving data credit for it.

## Source fidelity and class layouts

Source and complete class declarations adapt [zeldaret/ss g3d_anmchr.cpp](https://github.com/zeldaret/ss/blob/57a709b2128eb163a18e2969d561c9abb7d1fcee/src/nw4r/g3d/g3d_anmchr.cpp) and its header at pinned commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`, CC0-1.0 with no file-specific exception found. Comments are removed, existing canonical NW4R headers replace umbrella includes, and C++ null pointers use `nullptr`. The existing Wii/1.1 `cflags_nw4r` profile reproduces the retained code; no new compiler flags or assembly substitutions are introduced.

Native RTTI, constructor stores and both complete vtables identify the original `AnmObjChr` and `AnmObjChrRes` names and virtual method order. `AnmObjChr` is 0x18 bytes, with inherited object/animation state through 0x0F, binding count at 0x10 and binding pointer at 0x14. `AnmObjChrRes` is 0x34 bytes: protected `FrameCtrl` starts at 0x18, resource pointer is at 0x2C and result-cache pointer at 0x30. Frame, rate, start/end and play-policy fields occupy 0x18, 0x1C, 0x20, 0x24 and 0x28. The native allocator constructs these exact sizes; cached results have 0x4C-byte stride.

The reference's complete node/blend hierarchy and related methods remain ordinary library source but are stripped from the native final executable. Their absence is checked in the final ELF, and they receive no matching or progress credit. The native partial-binding helper is inlined into the retained resource methods.

`TestDefined` uses the equivalent explicit comparison `(mpBinding[idx] & BINDING_UNDEFINED) == 0`. The reference's logical negation produces a 28-byte routine with `cntlzw`; the explicit comparison produces the native 24-byte bit extraction and inversion. Several shifts, casts and relational forms were rejected before this ordinary C++ expression matched.

## Shared weak destructor

The base vtable points to a 64-byte destructor at `0x801C5C60`, earlier in the executable. Native class identity and a separate paired instruction/relocation audit establish that this is `AnmObjChr::~AnmObjChr`. Its placeholder name is changed to the canonical mangled name so the library vtable resolves to the existing earlier weak implementation. Its split, function type and size remain owned by its original issue. No earlier code is replaced or counted again. The compiler-generated duplicate in this source object is discarded by the link.

## Validation

The baseline and split-only configure/Ninja workflows exit zero, followed by explicit frozen verification. After source registration and naming, all four compared sections and all 26 retained functions score 100%. The source-selected full build and explicit verifier also exit zero:

```text
python configure.py
.venv/Scripts/ninja
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
```

A separate native audit reconstructs every instruction using the actual relocation target addresses: 57 text relocations and 30 vtable relocations in the new unit, plus two relocations in the shared earlier destructor, all agree. Both RTTI objects, both vtables and all sixteen constant bytes agree with the native data. A final ELF audit checks all native addresses/extents and the earlier destructor address; the actual Ninja link rule selects the source object and excludes its original fallback. On the initial verified source head `899c7b4`, all 376 unrelated baseline source objects remained byte-identical. After rebasing onto `2c94c838`, an input-preservation audit proves all 432 existing main object registrations and ten flag profiles unchanged, with every existing source/header unchanged. The complete 4,094,880-byte DOL matches the frozen reference with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. No original or expected hashes change.

PR #749 merged the coordinated #398 unit as `1c53d81e`. A fresh full build in `build/399-final-main/wpse-v1` on source head `cf74130c` atop `2c94c838` passed Ninja and explicit frozen verification, followed by all four sections at 100% and the complete native audit. The original #399 inventory is now proven complete: the two source-selected TexSrt methods at `0x80258090`/`0x802580C0` plus twenty-four Chr methods cover all twenty-six issue functions and 3,092 bytes. Both actual link inputs are source objects, and the supplied TexSrt methods independently match every instruction and relocation target. The two following #400 Chr RTTI methods at `0x80258D50`/`0x80258D80` are supplied by this unit and separately inventoried by that owner without duplicate credit.

## Traps

- Logical negation and an explicit comparison can select different bit-test sequences under Wii/1.1. Compare native code before assuming equivalent C++ expressions compile identically.
- An inline weak destructor can first appear far before its library source unit. Resolve its native vtable reference and prove the complete body/callees before inventing a new destructor or taking another issue's code split.
- An inferred RTTI symbol can include trailing alignment. Use the compiler's actual type-name object size and preserve alignment separately; padding is not another class member.
- The SDK `RoundUp` macro collides with the NW4R C++ template. This source undefines the macro after its includes so the existing `ut::RoundUp` template remains usable; shared headers are unchanged.
- Compiler-generated constant labels changed after upstream header additions, while every instruction and retained constant stayed identical. Match each retained constant by its proven value and native relocation target, then update only this unit's local names. The unretained blend implementation's NaN constant is stripped and receives no native-byte credit.
