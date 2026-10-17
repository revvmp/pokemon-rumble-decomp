# F-808-1: remaining layout units and corrected boundaries

## Inventory reconciliation

The issue lists 46 functions and 11,544 code bytes. Five resource-accessor
functions, 612 bytes, were delivered by PR #734. Window's runtime-type getter
and initializer, 20 bytes, were delivered by PR #824. These seven functions
remain completed dependencies; this work does not count them again. The
remaining inventory is 39 functions and 10,912 code bytes.

The reference link-order filenames are survey hints. The first retained
Layout helper at `0x802A6060` belongs to Layout, before #809's mapped methods.
Material's CalcTextureMtx at `0x802ABD50` and SetIndTexMtx at `0x802ABE60`
precede #811's mapped methods. Native helper comparisons are preserved in
[F-807-3](807-adjacent-layout-boundaries.md). Those prefixes establish source
file ownership under the first-function rule; neighboring coverage must be
described explicitly when those complete files are reconstructed.

## Bounded source probes

Ordinary C++ source reconstructed from the pinned CC0 Xenoblade
`ea25691b2029394ab777bda7060139e517a13cba` and Skyward Sword
`57a709b2128eb163a18e2969d561c9abb7d1fcee` references produces exact native
non-relocation instruction streams and sizes for these thirteen functions:

| Unit | Retained functions | Code bytes | Native text extent |
| --- | --- | --- | --- |
| Bounding | constructor, destructor, DrawSelf, runtime type, initializer | 172 | `0x802ABC80–0x802ABD4C` |
| TexMap | two Get overloads, three Set overloads, ReplaceImage(TPLPalette*,u32) | 1,208 | `0x802AEB30–0x802AF000` |
| DrawInfo | constructor and destructor | 180 | `0x802AF000–0x802AF0C0` |

All three units subsequently passed full linked-source acceptance: every
native function and owned section is 100%, every linked native address and
size is unchanged, and the real link selects the three compiled source
objects without their extracted fallbacks. The 13 functions contribute 1,560
new code bytes. Together with the seven already delivered functions, this
leaves 26 issue functions and 9,352 code bytes unfinished. The issue remains
open; none of that remaining work receives source credit here.

The unchanged baseline at main `6810b58a` used a fresh build directory after
rebasing metadata under the updated take-issue procedure. Configure, default
Ninja, and the explicit frozen verifier each exited 0. Split-only and
NonMatching source phases then built the actual main.dol target and passed
the explicit verifier. After all per-function and owned-section comparisons
were 100%, the Matching configuration passed default Ninja and frozen
verification. An independent audit checked all 13 functions, exact linked
sizes/addresses, source input selection, and complete immutable DOL byte
identity. The final committed head is revalidated in the pull request.

TexMap needs the later one-bit magFilter packed layout. The Xenoblade header
uses three bits and cannot be copied unchanged. Skyward Sword's direct
Set(TPLDescriptor*) implementation omits wrap-mode assignment; adding the
evidenced wrap S/T update immediately after texture format yields the native
292-byte body and retains the native 112-byte palette overload. A wrapper
around SetNoWrap instead emits an 84-byte wrapper and changes the palette
overload to 144 bytes; it is rejected. The native descriptor setter includes
all state updates directly, while ReplaceImage preserves only image and
palette-related state.

## TextBox display-list cache evidence

The native TextBox is larger than every pinned public header checked here
(Xenoblade, Skyward Sword, Kirby's Return to Dream Land, and OGWS). Besides
the usual fields through byte 101, constructor `0x802A7690` initializes three
two-float storage pairs at 108, 110, and 118, then constructs two Colors at
120. Init at `0x802A7920` also clears a float at 128, allocates 0x2000 bytes,
stores the buffer pointer at 13C, and calls GDInitGDLObj with this+12C,
that buffer, and size0x2000. The instance therefore extends through at least
0x140, rather than the public header's 0x104. A flag word is accessed at104.

This is direct evidence of additional display-list storage, not a conclusion
that every extra pair's purpose or original name is known. Do not import the
smaller public class or fill the extra members with arbitrary padding.
The cache's complete field semantics and behavior remain under investigation.

Native `0x802A7AD0` supplies direct names for the extra text decoration:
it looks up the strings `border-width` (`0x803C6FB8`) and `border-color`
(`0x803C6FC8`) through FindExtUserDataByName. A float user-data entry sets
the width at128. A string entry is parsed as hexadecimal colors and sets
the two colors at120/124, then enables bit0x10 in the flag byte at101.
One, two, three, four, and six hex-digit forms expand into RGBA; absent or
invalid second color copies the first. TextBox's native SetExtUserDataList
override at `0x802A7DA0` stores the inherited list pointer at B4 and tail-calls
this border parser. These fields are therefore border colors and border
width, rather than speculative shadow fields. Its original method spelling
is not independently established.

## Traps

- Name-only reference spans do not establish actual file boundaries. Preserve
  proved native helper prefixes when assigning Layout and Material ownership.
- Do not infer this TextBox's size from a reference header: its native Init
  constructs a graphics display-list object and stores a buffer beyond the
  reference class's end. Follow all target accesses before declaring it.
- The reference TexMap header repeats IsCITexelFormat, which already exists
  in canonical lyt_common.h. A standalone TexMap build hides this until
  Material includes both headers and compilation rejects the duplicate.
  Include the canonical common header and reuse its existing helper.
