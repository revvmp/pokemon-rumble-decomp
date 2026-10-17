# F-808-3: complete native TextBox storage and Layout interface

## Ownership and provenance

Claimed #808 owns the TextBox source file and its class header. This header
prerequisite supplies the complete native 0x140 class required by Layout's
resource construction and tag-processor traversal (#809). It adds no source
body, split, object registration, or reconstructed-byte credit. #808 remains
unfinished until its complete native source inventory is accepted.

The declaration prefix and retained virtual APIs come from the CC0 Skyward
Sword reference `57a709b2128eb163a18e2969d561c9abb7d1fcee`,
`include/nw4r/lyt/lyt_textBox.h`, checked against native vtable slots and
callers. That older 0x104 representation lacks the target's caches, border,
and display-list storage. It cannot be used unchanged. F-808-1 records the
native border and GD evidence; the independently checked cache evidence is
also recorded in F-809-1. Existing canonical Pane, Size, Rect, Color, and SDK
GDLObj definitions are reused.

## Complete storage

| Offset | Field | Evidence |
| --- | --- | --- |
| D8 | Text buffer pointer | Native string allocation, copying, and destruction. |
| DC/E0 | Two text colors | Native color methods at 7FA0, 7FC0, 8000, and 8020. |
| E4 | Font pointer | Native GetFont at 9130 and resource construction/drawing. |
| E8 | Font Size | Native text-writer setup consumes width and height. |
| F0/F4 | Line/character spacing | Native writer setup and line-rectangle calculation. |
| F8 | Tag processor pointer | Native Layout traversal stores the pointer and marks draw-cache bit 8. |
| FC/FE | Buffer bytes/string length | Native allocation and bounded SetStringImpl. |
| 100 | Text position | Native horizontal/vertical positioning. |
| 101 | Packed font-allocation/alignment/border flags | Existing reference prefix; border parser sets mask 0x10. No guessed storage is added. |
| 104 | Mutable draw-cache flags | Tag changes set 8, string changes set 0x20, pane-size changes set 0x80. |
| 108 | Mutable cached ut::Rect | Native GetTextDrawRect copies four cached floats into its returned rectangle. |
| 118 | Mutable cached Pane Size | Native cache compares and stores Pane size at 4C/50. |
| 120/124 | Two border colors | Native border-color parser and repeated border drawing. |
| 128 | Border width | Original border-width metadata and native drawing offsets. |
| 12C | Canonical GDLObj | GDInitGDLObj receives this address; drawing pads, flushes, and calls the generated display list. |
| 13C | Display-list allocation pointer | Init allocates/clears 8192 bytes; destructor frees the block. |

The class has natural alignment between the byte-sized flags and the word
at 104. No unknown field, padding array, layout shim, or duplicate SDK record
is introduced. The cache members are mutable because the native const
GetTextDrawRect overload updates them.

At 0x802A923C through 0x802A9274, the native method compares size at 4C/50
with 118/11C, copies changed components, and sets flag 0x80. At 0x802A9498
through 0x802A94B4, it copies 108/10C/110/114 into the returned rectangle.
These observations replace the earlier unclassified-float-pair hypothesis.

## Virtual and caller evidence

The inherited SetExtUserDataList override remains at vtable slot 70 and
LoadMtx at 74. New tail slots 78/7C/80/84 are string allocation, freeing,
and the two SetString overloads. The four-byte method at 0x802A9010 is an
actual tail branch to SetStringImpl at 0x802A9020; it is not an empty method.
The two-argument wrapper at 0x802A8FB0 obtains wcslen and calls the same
implementation. No placeholder body is supplied in this prerequisite.

The inline SetTagProcessor stores F8 and sets the native word flag at 104.
A pinned Wii/1.1 compiler probe confirms sizeof(TextBox) == 0x140 and emits
the expected five instructions for this operation. The whole Layout probe
using this exact header reproduces all 19 functions / 4,144 code bytes,
98 independently paired text/vtable relocations, all 64 vtable bytes, both
owned floats, and the allocator word. BuildPaneObj includes the exact 0x140
TextBox allocation. Layout's own class remains 0x20.

The receiving prerequisite pull request must run its full baseline and
latest-main build/verifier gates and repeat the Animation and Layout native
consumer audits. The separately approved RuntimeTypeInfo null guard belongs
to #856; this header does not duplicate or change that implementation.

## Traps

- A four-byte surveyed function can be a tail branch, not a no-op. Inspect
  its actual instruction before inventing an empty virtual implementation.
- Repeated zero-initialized float pairs do not establish independent vector
  fields. Native use proves one cached Rect and one cached Size here; keep
  the complete class representation consistent with those accesses.
