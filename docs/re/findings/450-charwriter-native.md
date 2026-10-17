# F-450-3: complete native CharWriter reconstruction

## Ownership and scope

Helper-claimed #450 owns the first actual CharWriter constructor at
802983D0, hence its complete split, object line and canonical class. The
unit ends at 8029A78C, before TextWriterBase starts at 8029A790. Its last
two functions belong to original #451 and are coordinated with that issue's
TextWriter contributor. No TextWriter source or split is taken over.

| Section | Exact native extent | Contents |
| --- | --- | --- |
| .text | 802983D0-8029A78C | 13 retained functions / 9,116 code bytes |
| .bss | 804524C8-804524D8 | Canonical LoadingTexture, sixteen bytes |
| .sbss | 804B93D8-804B93E0 | Fog initializer guard byte, alignment, Color word |
| .sdata2 | 804BB0C8-804BB0D8 | Zero, one, signed integer conversion double |

| Address | Size | Method |
| --- | --- | --- |
| 802983D0 | 380 | constructor |
| 80298750 | 40 | destructor |
| 80298790 | 930 | SetupGX |
| 802990C0 | 9F8 | SetupGXForDisplayList |
| 80299AC0 | C4 | SetFontSize(width,height) |
| 80299B90 | 78 | SetFontSize(height) |
| 80299C10 | 60 | GetFontWidth |
| 80299C70 | 60 | GetFontHeight |
| 80299CD0 | 60 | GetFontAscent |
| 80299D30 | 150 | UpdateVertexColor |
| 80299E80 | 2EC | SetupGXWithColorMapping |
| 8029A170 | 370 | SetupGXForDisplayListWithColorMapping |
| 8029A4E0 | 2AC | SetupGXForDisplayListForI |

Sizes in this table are hexadecimal. Existing CharWriter storage remains
0x4C, with all original canonical members and their order unchanged. The
four additional private static helper declarations and previously evidenced
public GD setup declaration belong to the bounded header prerequisite #879.
The GD-specific names are descriptive inferences; original spellings have
not been recovered. Existing names, including SetFontSize, are preserved.

## Source and provenance

The shared implementation comes from CC0 zeldaret/ss
`57a709b2128eb163a18e2969d561c9abb7d1fcee`,
`src/nw4r/ut/ut_CharWriter.cpp`. Canonical existing member names and SDK
interfaces are reused, including the SDK's actual GXSetFog argument order.
No source comments or raw decompiler output are imported.

Native instructions, calls and constant arguments establish the later
display-list setup path. It clears the real texture cache, selects the
existing texture-format paths, and sets the text-gradient colors in TEV
registers. Its ordinary static setup helpers reuse SetupGXCommon and the
original vertex-format routine. The default path interpolates the two text
colors and modulates by the texture. The intensity path preserves the
interpolated color and modulates texture alpha. The color-mapping path uses
three TEV stages: text gradient into REG2, texture remapping, then their
product. All calls use typed SDK constants and ordinary class methods.

This supersedes F-450-2's conclusion that the shared reference cannot match
the target's constructor and normal GX setup: with the complete existing
canonical headers, their bodies are exactly 0x380 and 0x930 bytes and match
the native functions. The isolated cause of the earlier reference profile
was not established, so it is not attributed to a particular header or
compiler behavior. No flags or canonical class layouts were changed.

## Native evidence

Canonical-only compilation proves all 13 native instruction streams, sizes
and physical retained order. Independently decoded native calls and SDA
accesses pair all 429 text relocations to their actual targets. All forty
non-text bytes match exactly. No callee address remains inferred or
unresolved. External scripts are `450-compile-charwriter-probe.py` and
`450-paired-native.py`, each exit 0. These are diagnostic compiler/native
checks. The subsequent split-only, NonMatching, and actual Matching source
pipelines in the combined layout delivery each exit 0, followed separately
by explicit frozen verification 0. The actual CharWriter object has all
thirteen functions and all four sections at 100%. Every final native
function address and size is exact, as are all 429 actual relocation targets
and forty data bytes. The real Ninja graph selects the source object and
excludes its fallback. The complete DOL matches the immutable reference.
These first acceptance runs use the combined delivery's historical base;
final latest-main verification remains required before merge.

`450-full-source-audit.py` also checks all 36 original #450 rows at their
native addresses and sizes, plus source-only selection for ArchiveFont,
PairFont and CharWriter. The two #451 tails match independently. The existing
PairFont .data objdiff score is 85.714%, also present in the historical
unchanged-header baseline: its source object contains 208 bytes, including
an extra 96-byte weak base Font vtable, versus the native unit's 112 bytes.
The retained 112 bytes and all 23 pointer relocations were independently
reconstructed against the frozen-native ELF baseline and are exact. The
weak base Font vtable resolves at the already canonical 803C6A88. All 23
PairFont functions are 100%, and the entire DOL remains exact. No existing
Font/PairFont code is changed or extraneous source-object data claimed as
new reconstruction. This diagnostic limitation is preserved explicitly.

## Original inventories and credit

The original #450 table has 36 functions / 11,064 code bytes. The first
two ArchiveFont tails (368 bytes) and all 23 PairFont methods (3,144 bytes)
are already accepted canonical source, as recorded in F-449-2/F-450-1.
The remaining eleven CharWriter functions total 7,552 bytes. This whole
CharWriter delivery additionally supplies the first two original #451 rows,
8029A170/0x370 and 8029A4E0/0x2AC, totaling 1,564 bytes. Thus its complete
native thirteen functions total 9,116; previous ArchiveFont/PairFont bytes
receive no new reconstruction credit.

`450-audit-original-inventory.py` reads both actual issue bodies, checks
every original #450 address/size once, verifies the exact two neighboring
#451 rows, and requires the 36/11,064 and 13/9,116 totals. It exits 0.
Issue #451 remains the TextWriter owner's work beyond those two tails.

## Traps

- A different reference size profile does not prove the source algorithm
  cannot match. Compile against the complete canonical target interfaces
  before discarding the shared implementation; the common ten methods
  reproduce the target here without code-generation workarounds.
- The native CharWriter constant pool ends at 804BB0D8. That next address
  starts TextWriter's maximum-float constant. Importing the older reference's
  unretained out-of-line Print/PrintGlyph/LoadTexture bodies adds a half and
  unsigned-conversion double absent from this unit. These bodies are not
  supplied here. Their placement/inlining in the later TextWriter revision
  remains that owner's separate investigation; no guessed padding or pool
  emitter replaces them.
- Matching relocation-masked instructions alone cannot validate TEV colors,
  static fog state, texture cache or callees. Pair every relocation against
  the native target and compare the complete owned non-text sections.
- A source object can contain an additional weak base-class vtable that the
  linker coalesces into another native unit. Keep its diagnostic score
  visible and independently compare the retained native data and targets;
  do not rewrite the shared class or count the extra weak data as progress.

## Original issue rows

| Address | Size | Native unit | Delivery |
| --- | --- | --- | --- |
| 802975A0 | 50 | ArchiveFont | previously accepted |
| 802975F0 | 120 | ArchiveFont | previously accepted |
| 80297710 | 90 | PairFont | previously accepted |
| 802977A0 | 40 | PairFont | previously accepted |
| 802977E0 | 68 | PairFont | previously accepted |
| 80297850 | 68 | PairFont | previously accepted |
| 802978C0 | 90 | PairFont | previously accepted |
| 80297950 | 90 | PairFont | previously accepted |
| 802979E0 | 90 | PairFont | previously accepted |
| 80297A70 | E8 | PairFont | previously accepted |
| 80297B60 | 90 | PairFont | previously accepted |
| 80297BF0 | 90 | PairFont | previously accepted |
| 80297C80 | 68 | PairFont | previously accepted |
| 80297CF0 | 8 | PairFont | previously accepted |
| 80297D00 | 14 | PairFont | previously accepted |
| 80297D20 | 68 | PairFont | previously accepted |
| 80297D90 | C0 | PairFont | previously accepted |
| 80297E50 | 60 | PairFont | previously accepted |
| 80297EB0 | A0 | PairFont | previously accepted |
| 80297F50 | A4 | PairFont | previously accepted |
| 80298000 | E8 | PairFont | previously accepted |
| 802980F0 | 118 | PairFont | previously accepted |
| 80298210 | 108 | PairFont | previously accepted |
| 80298320 | 84 | PairFont | previously accepted |
| 802983B0 | 14 | PairFont | previously accepted |
| 802983D0 | 380 | CharWriter | new source delivery |
| 80298750 | 40 | CharWriter | new source delivery |
| 80298790 | 930 | CharWriter | new source delivery |
| 802990C0 | 9F8 | CharWriter | new source delivery |
| 80299AC0 | C4 | CharWriter | new source delivery |
| 80299B90 | 78 | CharWriter | new source delivery |
| 80299C10 | 60 | CharWriter | new source delivery |
| 80299C70 | 60 | CharWriter | new source delivery |
| 80299CD0 | 60 | CharWriter | new source delivery |
| 80299D30 | 150 | CharWriter | new source delivery |
| 80299E80 | 2EC | CharWriter | new source delivery |
