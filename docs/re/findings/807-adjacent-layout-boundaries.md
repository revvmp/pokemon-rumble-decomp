# F-807-3: adjacent layout boundaries after the re-cut

This is read-only boundary research performed while #807's verified source
waited for its merge window. It does not claim #808, #809, or #811, change
their source/splits, or contribute executable source credit.

## Reconciliation with completed resource accessors

Issue #808 lists 46 functions / 11,544 bytes. Its final five functions,
`0x802B0680`, `0x802B06C0`, `0x802B06D0`, `0x802B06E0`, and `0x802B07B0`,
are the 612 bytes already delivered in #734. Its other 41 surveyed functions
/ 10,932 bytes remain unfinished. The issue must not be closed solely because
the resource-accessor tail is complete, nor counted twice for that tail.

## Proved Material helper boundary

The CC0 Xenoblade revision `ea25691b2029394ab777bda7060139e517a13cba`,
`libs/nw4r/src/lyt/lyt_material.cpp`, contains anonymous CalcTextureMtx and
SetIndTexMtx helpers before the Material methods. Compiling those ordinary
source bodies with the established Wii/1.1 NW4R flags produced:

| Reference helper | Native address | Size | Comparison |
| --- | --- | --- | --- |
| CalcTextureMtx | `0x802ABD50` | `0x108` | Every non-relocation instruction word identical |
| SetIndTexMtx | `0x802ABE60` | `0x200` | Every non-relocation instruction word identical |

This establishes the Material file's retained helper prefix before #811's
mapped range at `0x802AC060`. The re-cut attributes both helper bodies to
#808. The listed neighboring Bounding file name is a link-order hypothesis,
not contrary evidence. Final unit reconstruction must still verify all
relocations, sections, and the complete frozen executable independently.

## Layout helper and TextBox observations

The function at `0x802A6060`, size `0x1D4`, dynamically checks a Pane's runtime
type against TextBox, stores a tag processor at offset `0xF8`, sets bit 3 in
the word at `0x104`, and recursively visits the intrusive child list. The
pinned Xenoblade and Skyward Sword `lyt_layout.cpp` sources name that behavior
SetTagProcessorImpl. This is strong evidence for Layout's helper prefix before
#809's mapped Layout functions beginning `0x802A6240`; the helper itself has
not yet received a full compiled-source comparison. It is not DrawInfo's
constructor. Actual DrawInfo construction/destruction occurs at
`0x802AF000` / `0x74` and `0x802AF080` / `0x40`.

Native TextBox's constructor at `0x802A7690` initializes extra storage beyond
the pinned public headers: fields at `0x108–0x11C` and a two-element color array
at `0x120`, besides the established tag processor and flags. Their complete
types and behavior need target analysis or a closer reference. Do not import
the smaller public TextBox class unchanged.

## Traps

Re-cut filenames describe reference spans and link-order estimates. Establish
the first retained function of each actual file before choosing split/header
ownership. In particular, #808's remaining functions include first-function
evidence for Layout and Material even though most mapped bodies occur in
#809 and #811. Follow the skill's first-function rule and describe neighboring
coverage explicitly; this finding is evidence, not an issue claim.
