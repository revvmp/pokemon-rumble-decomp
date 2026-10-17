# F-808-5: native TextBox source and complete issue inventory

## Source and provenance

Claimed #808 owns the complete native TextBox translation unit beginning at
0x802A7690. Its ordinary C++ implementation uses the canonical 0x140 class
from F-808-3 and the bounded drawing interfaces in F-808-4. It contains real
resource construction/destruction, string allocation and mutation, color
access, line wrapping and tag processing, cached text rectangles, immediate
printing, cached GD drawing, and border parsing/drawing.

The older CC0 Skyward Sword source at
`57a709b2128eb163a18e2969d561c9abb7d1fcee`,
`src/nw4r/lyt/lyt_textBox.cpp`, supplies original names and the shared
line-measurement algorithm. Its immediate-only drawing cannot replace the
target's later caches and border/GD paths. Those paths were reconstructed
from native accesses, calls, metadata strings, and instructions. IDA on an
external database copy was a reading aid only; no decompiler output was
pasted. Inferred names and complete class offsets are distinguished in
F-808-3/F-808-4.

Init allocates an 8192-byte display-list buffer with the canonical Layout
allocator. DrawSelf uses immediate printing for a tag processor or strings
longer than 61 characters, otherwise rebuilding the cached GD list when
dirty. It draws the eight neighboring border offsets and then the main
list. Border metadata accepts the native one/two/three/four/six-digit color
forms and optional second gradient color. SetStringImpl compares copied
characters and the terminator before invalidating the cache. Rectangle
calculation compares the current Pane size with the cached Size and stores
the resulting Rect in its evidenced members.

## Native proof

Canonical-only diagnostic compilation reproduces all 23 retained functions,
8,108 code bytes and physical retained order. Independent native target
pairing proves 156 text relocations, 33 data/constructor relocations and all
240 non-text bytes. F-808-4 gives all seven exact section extents.

The actual split-only pipeline built 517 targets and exited 0, followed by
explicit frozen verification 0. The NonMatching source pipeline and explicit
verifier also exited 0. After selecting Matching source, the actual default
Ninja pipeline and explicit frozen verifier again exited 0. Objdiff reports
all 23 functions and all seven owned sections at 100%. Every function's final
linked address and size equals the native inventory. Ninja's real main.elf
input graph selects src/nw4r/lyt/lyt_textBox.o and excludes its original
fallback. The full main.dol is byte-identical to the frozen reference, SHA-256
`ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.
The actual compiled source object also passes the independent paired audit
with no unresolved or inferred callee addresses. External commands
`808-textbox-full-source-audit.py` and `808-textbox-paired-native.py
build/wpse-v1/src/nw4r/lyt/lyt_textBox.o` exit 0.
This first source-selected proof is based on 7f00715b plus the bounded header
prerequisite. Final latest-main verification remains required before merge.

## Complete original #808 inventory reconciliation

The issue's original REST inventory contains exactly 46 address/size rows
and 11,544 instruction bytes. Every row belongs to exactly one group below;
survey file labels are superseded by native source boundaries.

| Native unit | Original issue functions | Code bytes | Delivery |
| --- | --- | --- | --- |
| Layout first helper, 802A6060 | 1 | 468 | Canonical PR831, including coordinated PR865 |
| TextBox, 802A7690-802A96DC | 23 | 8,108 | This source delivery |
| Window tail, 802ABC60/802ABC70 | 2 | 20 | Previously merged PR824 |
| Bounding, 802ABC80-802ABD4C | 5 | 172 | PR884, retaining PR848 source |
| Material first helpers, 802ABD50/802ABE60 | 2 | 776 | PR884, retaining complete PR866 source |
| TexMap, 802AEB30-802AF000 | 6 | 1,208 | PR884, retaining PR848 source |
| DrawInfo, 802AF000-802AF0C0 | 2 | 180 | PR884, retaining PR848 source |
| ResourceAccessor/ArcResourceAccessor, 802B0680-802B08F0 | 5 | 612 | Previously merged PR734 |
| Total | 46 | 11,544 | No duplicate byte credit |

The first Layout helper is 468 bytes, the first two Material helpers total
776, and both corrected whole-file boundaries are owned by #808. Coordinated
#809 and #811 source deliveries retain their own original inventories and
explicitly attribute these neighboring functions once. Prior PR734/824
already supplied seven / 632; their bytes are not newly reconstructed here.
The external `808-audit-original-inventory.py` reads the actual issue body,
checks every original address and size, requires the groups to be disjoint,
and proves the full 46 / 11,544 sum. The final actual source-selected
build now includes canonical PR831 and all remaining deliveries in PR884.
The independent final inventory audit below proves every original row
against the final linked executable; #808 is complete without duplicate
credit for inherited source.

## Traps

- The original survey rodata object at 80371608 was 0x28 bytes and sbss type
  word at 804B9420 was eight. Native code/data prove 0x24 and four; the rest
  belongs to alignment before the next object. Correct the owned symbols
  before the split or dtk rejects a split ending inside a surveyed symbol.
- The four-byte SetString overload at 802A9010 is a real tail branch to
  SetStringImpl, not a no-op or empty virtual body.
- Preserve the native two-return buffer accessor and by-value color helper;
  equivalent-looking rewrites change retained narrowing or copy code.
- SetStringImpl sets its changed flag before memcpy. A changed terminator
  independently dirties the cache; comparing only the characters loses
  native behavior.

## Original inventory rows

The later combined source branch retains the exact source blobs from PR848
(Bounding/TexMap/DrawInfo) and PR866 (Material), plus this accepted TextBox.
Together with the complete CharWriter unit from #450, the six complete
units contain 126 retained functions / 29,972 newly delivered code bytes
and 600 allocated non-text bytes. The combined actual default Ninja pipeline and explicit frozen
verifier exit 0. All five actual objdiff reports, final native function
positions, source-only link inputs and full immutable DOL equality pass.
The actual combined TextBox and Material objects repeat their independent
paired-target/data proofs with no inferred addresses. This consolidation
does not create additional byte credit; PR848/866 remain provenance until
the combined delivery is canonical and their replacement is confirmed.

| Address | Size | Native unit |
| --- | --- | --- |
| 802A6060 | 1D4 | Layout |
| 802A7690 | 288 | TextBox |
| 802A7920 | 1A8 | TextBox |
| 802A7AD0 | 2CC | TextBox |
| 802A7DA0 | 8 | TextBox |
| 802A7DB0 | 1E8 | TextBox |
| 802A7FA0 | 14 | TextBox |
| 802A7FC0 | 3C | TextBox |
| 802A8000 | 18 | TextBox |
| 802A8020 | 18 | TextBox |
| 802A8040 | 58 | TextBox |
| 802A80A0 | A08 | TextBox |
| 802A8AB0 | 3E0 | TextBox |
| 802A8E90 | C8 | TextBox |
| 802A8F60 | 4C | TextBox |
| 802A8FB0 | 5C | TextBox |
| 802A9010 | 4 | TextBox |
| 802A9020 | 110 | TextBox |
| 802A9130 | 8 | TextBox |
| 802A9140 | 84 | TextBox |
| 802A91D0 | 308 | TextBox |
| 802A94E0 | 1D4 | TextBox |
| 802A96C0 | 8 | TextBox |
| 802A96D0 | C | TextBox |
| 802ABC60 | 8 | Window |
| 802ABC70 | C | Window |
| 802ABC80 | 3C | Bounding |
| 802ABCC0 | 58 | Bounding |
| 802ABD20 | 4 | Bounding |
| 802ABD30 | 8 | Bounding |
| 802ABD40 | C | Bounding |
| 802ABD50 | 108 | Material |
| 802ABE60 | 200 | Material |
| 802AEB30 | EC | TexMap |
| 802AEC20 | 1C | TexMap |
| 802AEC40 | 14C | TexMap |
| 802AED90 | 70 | TexMap |
| 802AEE00 | 124 | TexMap |
| 802AEF30 | D0 | TexMap |
| 802AF000 | 74 | DrawInfo |
| 802AF080 | 40 | DrawInfo |
| 802B0680 | 40 | ResourceAccessor/ArcResourceAccessor |
| 802B06C0 | 10 | ResourceAccessor/ArcResourceAccessor |
| 802B06D0 | 8 | ResourceAccessor/ArcResourceAccessor |
| 802B06E0 | CC | ResourceAccessor/ArcResourceAccessor |
| 802B07B0 | 140 | ResourceAccessor/ArcResourceAccessor |

## Final current-main acceptance

PR884 was restacked onto actual merged header prerequisite PR879 at
813a7c745d081a82bf86e3ef76d7fbd54fc02e68. Temporary header carriers were
removed: its include tree is identical to canonical main. Its six source
files retain their previously reviewed native-proven contents. A fresh
output directory was used after all merged metadata changes. Configure,
actual default Ninja (548 targets), explicit all_source, and the separate
frozen verifier each exited 0 on clean committed source head 058728c7.
The declared all_source inventory includes 541 compiled source objects.

All six actual-object native/source audits pass: 126 functions / 29,972
code bytes, every owned section at 100%, exact final native addresses and
sizes, source link inputs selected and their original fallbacks excluded.
Actual TextBox, Material and CharWriter objects independently repeat all
paired text/data targets and initialized-byte proofs with no inferred
addresses. The complete DOL has the immutable hash recorded above.

The external 884-final-inventory-audit.py reads all three original REST
issue tables and proves every original #808 row (46 / 11,544), #811 row
(75 / 10,412) and #450 row (36 / 11,064) once. It checks source-only link
inputs, exact final function addresses/sizes, and every instruction byte
against the frozen DOL. The inherited Layout, Window, ResourceAccessor,
ArchiveFont and PairFont rows receive no new byte credit. This audit
justifies completing #808, #811 and #450; only the two supplied CharWriter
tails belong to still-separate #451. Its JSON evidence is preserved outside
the checkout alongside all native/source audits.
