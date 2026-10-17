# F-808-2: offset conversion template parameter

## Scope and provenance

The canonical `lyt_types.h` introduced by #820 declared both
`detail::ConvertOffsToPtr` overloads with a `u32` offset. This prerequisite
corrects that parameter to `unsigned int`, retaining the existing pointer
arithmetic and every other lyt_types.h declaration. It takes no source split, object
registration, or reconstructed byte credit from #808 or #811.

The pinned CC0 Skyward Sword reference
`57a709b2128eb163a18e2969d561c9abb7d1fcee`,
`include/nw4r/lyt/lyt_types.h`, declares both overloads with explicit
`unsigned int`. This repository's `include/revolution/types.h` defines `u32`
as `unsigned long`. Both are 32-bit for this target, but CodeWarrior encodes
them as `Ui` and `Ul` respectively in mangled names.

## Target and source evidence

The Material constructor at `0x802AC080` retains calls to twelve const
converter specializations at `0x802ACA50` through `0x802ACB10`. Their types
are BlendMode, AlphaCompare, TevStage, IndirectStage, TevSwapMode, ut::Color,
ChanCtrl, char, res::Texture, TexCoordGen, TexSRT, and res::TexMap. The source
constructor reproduces its 1,636-byte native instruction stream with the
reference's constructor-specific `dont_inline` pragma. The template bodies
are each eight bytes; identical generic bodies alone cannot prove which
specialization a call references. Named relocation and whole-section checks
remain mandatory in the Material source integration.

An unsigned-long offset emits `FPCvUl` names for those source calls. The
explicit unsigned-int declaration emits the reference `FPCvUi` signatures
recorded for the target specializations. This correction belongs to the
canonical helper, rather than an additional overload or a local competing
definition. The mutable overload follows the same evidenced reference
interface. Existing class layouts and Size's explicit copy constructor are
preserved.

## Validation

The unchanged baseline at main 5e251aeb passed configure, full default Ninja,
and the explicit frozen verifier, all exit 0. Captured copies and SHA-256
comparisons covered all 477 existing source objects; every object remained
byte-for-byte identical after the two parameter corrections. The unchanged
Pane/Group/Window audits also passed all 59 native functions, every owned
section, exact linked addresses and sizes, source selection, and immutable
DOL identity. A Material compiler probe with the corrected canonical header
matched all 77 native-range function bodies and sizes by their actual names,
except the two still-unnamed native anonymous helpers. It now emits all
twelve expected converter specialization names without ambiguous generic
body-based pairing. This probe does not replace Material full-section and
source-link acceptance.

The separate prerequisite pull request records
all existing source-object comparisons, native Pane/Group/Window audits,
and the latest-main build and immutable-artifact verification. The #848
Bounding/TexMap/DrawInfo source branch is audited separately against the
same correction before this prerequisite is accepted. Material remains a
scratch reconstruction until its independent full source acceptance gate.

## Traps

- A 32-bit typedef can preserve code generation while changing retained
  C++ template names. Check the actual typedef and the reference signature;
  matching a generic eight-byte body does not establish the specialization.
- Preserve the canonical integer-pointer arithmetic during this parameter
  correction. Material's independently evidenced typed byte-pointer array
  getters are a source reconstruction concern, not a reason to change this
  shared helper's arithmetic or add an overload.
