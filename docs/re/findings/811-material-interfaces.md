# F-811-1: Material, packed GX state, and resource interfaces

## Ownership and scope

Issue #811 owns 75 retained functions at `0x802AC060` through `0x802AEB30`,
totalling 10,412 code bytes. The complete native source file begins earlier:
CalcTextureMtx at `0x802ABD50` (264 bytes) and SetIndTexMtx at `0x802ABE60`
(512 bytes) belong to claimed #808. The same contributor owns both claims
and coordinates the full Material file under #808's first-function ownership.
The whole-file source acceptance inventory is 77 functions and 11,188 code
bytes. Neither a header prerequisite nor a diagnostic probe contributes
reconstructed executable-byte credit.

The shared prerequisite supplies interfaces required by Material itself,
Picture (#809), and Animation (#812). It retains Material's existing 0x5C
layout, protected fields, virtual order, Size copy-constructor ABI, and
canonical resource records. It extends the existing Material header and
resource header, and moves the unchanged BitGXNums declaration to the new
Material-specific packed-state header. There is one declaration of each
type. TexMap's complete 0x1C interface is the same header already proved
with the six source functions in #848, not a second layout or implementation.

## Provenance and native evidence

The CC0 Skyward Sword reference is pinned at
`57a709b2128eb163a18e2969d561c9abb7d1fcee`, with Material source under
`src/nw4r/lyt` and interfaces under `include/nw4r/lyt`. Comparison with the
CC0 Xenoblade reference `ea25691b2029394ab777bda7060139e517a13cba` confirms
the older shared class layout. This target uses the later packed resource
wrap/filter representation and constructor-specific deferred inlining.
The Xenoblade resource-loading loop cannot be substituted unchanged.

The external source probe compiled using only the proposed canonical include
tree, the pinned Wii/1.1 compiler, and existing cflags_nw4r options. All 77
native function bodies reproduce their exact sizes and non-relocation words;
75 match their imported names, and the first two match the evidenced
anonymous helper names. This includes the 1,636-byte resource constructor,
1,676-byte ReserveGXMem, and 3,592-byte SetupGX. The twelve retained offset
converter specializations now have their reference signatures (F-808-2).
This diagnostic does not replace final relocation, section, source-selection,
and frozen-artifact verification in the source pull request.

| Interface | Size | Native evidence |
| --- | --- | --- |
| Material | `0x5C` | Animation list at 4, TEV signed colors at 10, constant colors at 28, capacity/count at 38/3C, GX storage pointer at 40, name at 44, allocation flag at 59. Constructor, allocation, state setup, and animation methods reproduce those accesses. Existing virtual order is preserved. |
| TexMap | `0x1C` | Two pointers at 0/4, dimensions at 8/A, LOD floats at C/10, bias/palette count at 14/16, packed flags at 18. Six separately audited native methods prove the complete representation; magFilter is one bit. |
| BitGXNums | `4` | Existing packed capacity/count declaration is unchanged; ReserveGXMem and SetupGX reproduce every extraction and allocation offset. |
| TexCoordGen, ChanCtrl | `4` each | Native retained assignments at AC880/AC8B0 and state setup prove their four byte-sized fields. |
| AlphaCompare, BlendMode | `4` each | Native retained assignments at AC9F0/ACA20 and GX setup prove packed comparison/blend records. |
| IndirectStage | `4` | Native assignment at AC950 and indirect setup reproduce stage, coordinate, map, and scale bytes. |
| TexSRT | `0x14` | Translation pair, rotation, scale pair; native matrix helpers and retained assignment at AC920 prove order and size. |
| TevStageInOp, TevStage, TevSwapMode | `4`, `0x10`, `1` | Native stage assignment at AC980, stage initialization, GX setup, and swap assignment at AC910 prove packed selectors and operations. |
| res::TexMap | `4` | Texture index at 0, wrap/filter bytes at 2/3; constructor retains native accessors at AC800/AC810/AC840/AC860. Original GetWarpMode spelling is preserved. |
| res::Material | `0x40` | Twenty-byte name, three GXColorS10 values, four GXColor values, packed resource-count word. Native constructor's first variable resource offset is 40. |
| res::Texture, res::TextureList | `8`, `0xC` | Name offset and type record; block header, count, padding. Native constructor performs the matching table and string offset calculations. |

An external compiler layout probe independently confirmed every table size.
No executable layout shim, unknown field, extra overload, or speculative
padding was introduced. Unused reference getters were omitted. The ordinary original inline color
and render-state setters are required by the retained native Material
assignment-function definitions as well as the Picture/Animation consumers.
Their exact signatures and implementations come from the library references;
no synthetic emitter or ordering helper is used.

## Validation

The expanded prerequisite uses a fresh full baseline at main 7a1a3b81, with
the already-proved offset-type correction applied. Configure, default Ninja,
and explicit frozen verification exited 0 before extending the interfaces.
Captured copies cover all 479 existing source objects. After expansion,
477 remain byte-identical. Pane and Window differ only in numeric compiler
local names in `.strtab`; their text, data, relocations, symbol table, section
names, and compiler metadata remain byte-identical. Recompiling both with
the captured original headers reproduces their exact baseline SHA-256 values.
The final native audits and latest-main committed-head gate are recorded in
the prerequisite pull request.

## Traps

- More inline definitions in a shared header advance CodeWarrior's numeric
  local-label counter in consumers. Compare actual ELF sections and reproduce
  the original-header object before treating an object hash difference as
  harmless. Here only `.strtab` numeric labels changed; native section and
  complete DOL checks remain mandatory.
- The Skyward Sword scratch source contains a float-ordering helper and an
  unused empty default-constructor body. Omit both. The independently pinned
  Petari implementation supplies a real default constructor calling Init
  and clearing the material name; that implementation preserves the native
  retained LinkList constructor placement without a placeholder body.
- Removing the original inline SetTevColor, SetChanControl, SetTevKColor,
  SetTevSwapMode, SetTexSRT, SetIndirectStage, SetTevStage, SetAlphaCompare,
  and SetBlendMode definitions delayed CodeWarrior implicit assignment
  definitions despite all 77 raw function streams matching. Restoring these
  reference APIs gives all 77 native functions their original order. Whole
  linking and frozen verification are still required; raw objdiff scores
  cannot establish this ordering.
