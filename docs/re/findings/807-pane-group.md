# F-807-2: complete Pane and Group source

## Scope and provenance

Issue #807's complete inventory is covered by two translation units. Its
41 surveyed function fragments resolve to 39 real functions because the
four-byte fragments at `0x802A4FD0` and `0x802A5000` are the trailing returns
of GetColorElement and SetColorElement. Each switch occupies `0x24` bytes.
Coalescing those fragments changes no code-byte coverage: all 5,020 surveyed
instruction bytes are reconstructed.

Source follows the CC0 Xenoblade revision
`ea25691b2029394ab777bda7060139e517a13cba`,
`libs/nw4r/src/lyt/lyt_pane.cpp` and `lyt_group.cpp`, with target adaptations
checked against native instructions and the CC0 Skyward Sword revision
`57a709b2128eb163a18e2969d561c9abb7d1fcee`, `src/nw4r/lyt/lyt_pane.cpp`.
No file-level license exception was present in the reused units. Reference
comments are not copied. The compiler and flags remain the established
Wii/1.1 `cflags_nw4r`, including `-fp_contract off` and `-ipa file`.

The prerequisite interfaces and their layout evidence are in [F-807-1](807-layout-interfaces.md).
No neighboring source or splits are taken.

## Units and coverage

| Unit | Native functions | Instruction bytes | Owned sections, half-open ranges |
| --- | --- | --- | --- |
| Pane | 34 | 4,192 | `.text` `0x802A4BC0–0x802A5CFC`; `.ctors` `0x80352A84–0x80352A88`; `.data` `0x803C6E70–0x803C6EE8`; `.sbss` `0x804B9408–0x804B940C`; `.sdata2` `0x804BB160–0x804BB180` |
| Group | 5 | 828 | `.text` `0x802A5D00–0x802A605C`; `.data` `0x803C6EE8–0x803C6EF4` |

Pane covers resource construction, destruction, child insertion, rectangle,
color access, recursive pane/material search, matrix calculation and child
traversal, drawing, animation and material animation, binding/unbinding,
animation-link management, both enable overloads, GX matrix loading, vertex
position, all material accessors, extended-user-data lookup and attachment,
and the static initializer. Group covers resource construction, destruction,
GroupContainer destruction, append, and recursive-name-compatible lookup.
The default Pane constructor and small list helpers exist as ordinary source
and are inlined or stripped where the native executable does not retain them.

The four-byte empty virtuals are native no-ops, independently visible in the
original executable. They are not unfinished implementations. Data is produced
by C++ vtables, RuntimeTypeInfo initialization, and compiler literal pools;
there are no executable-byte arrays or assembly substitutes.

## Target-specific behavior

- The pane rectangle subtracts height from the upper Y coordinate. DrawInfo's
  Y-axis test from Xenoblade is absent here; the original signature is retained.
- Draw and CalculateMtx share a gate that checks the visibility bit, nonzero
  X/Y scale, and inherited-alpha visibility. DrawSelf additionally requires
  nonzero local alpha. Animate uses the visibility bit without the scale gate.
- All three axes use PSMTXRotRad and PSMTXConcat. Skyward Sword's later
  SinCosDeg matrix construction is not this target's algorithm.
- BindAnimation forwards both recursive and disable Booleans through the
  native AnimTransform Bind slot at `0x14`.
- Extended-user-data lookup starts after the 12-byte list header, iterates
  12-byte entries, resolves entry-relative name offsets, and compares names.
- The native Pane vtable has SetExtUserDataList at `0x70` and LoadMtx at
  `0x74`. This corrects the earlier prerequisite declaration, which omitted
  the former virtual. The complete native table is `0x78` bytes.
- Pane's inline runtime-type getter is first emitted at `0x8006C860` in an
  outside game unit. Only that callee's name is recovered; its split, size,
  and type are unchanged, and its eight bytes receive no issue #807 credit.
- Group's native vtable is 12 bytes, not the survey symbol's 16-byte extent.
  Its trailing alignment remains outside the owned split. Pane's type-info
  object is four bytes; the following four alignment bytes remain outside it.

## Validation

The split-only NonMatching configuration passed configure, default Ninja, and
explicit frozen verification, all exit 0. Both source objects then scored
100% for every native function and every owned section. A source-selected
Matching build passed default Ninja and explicit frozen verification, exit 0.

An external audit independently checked:

1. Objdiff's full target function list: 34 Pane and five Group functions, each
   100%, with every owned section 100%.
2. All 39 native linked symbol addresses and exact sizes, totaling 5,020 bytes.
3. The real main.elf link dependency graph selects both compiled source
   objects and neither unit's extracted fallback object.
4. The resulting full `main.dol` is byte-for-byte equal to the immutable
   original, as well as passing the frozen manifest verifier.

The final committed head is revalidated after rebasing onto latest main;
its exact hash and command exit codes are recorded in the pull request.

After expanding the prerequisite with Window's explicit Size copy constructor
and complete canonical resource declarations, the source was restacked onto
header head `96ed0f1d`. The add/add resource-header conflict retained the
prerequisite's complete definitions. A fresh build directory passed configure,
default Ninja, explicit frozen verification, and the complete 39-function
linked-source audit, all exit 0, at source head `8e149556`. Both owned units
and every section remained 100%; the substantive Size ABI declaration did
not change their linked bytes.

## Traps

- Removing the dead-stripped default Pane constructor initially left code
  100% but `.sdata2` at 93.75%: the compiler encountered `0.5f` before `1.0f`
  and swapped two pool entries. Restoring the ordinary reference constructor
  produces the original literal pool, with all sections 100%. Keep legitimate
  unlinked source when it establishes the native translation unit's constants.
- In Draw, direct repeated member access reloaded alpha and added four bytes.
  The ordinary inline GetAlpha accessor preserves the target's retained value.
  In the user-data lookup, the reference GetExtUserData helper similarly
  preserves the target's pointer reload and exact 188-byte routine. These are
  observations for this translation unit and compiler, not general rules.
- Static initializers named from `lyt_pane.cpp` also occur in HBM. Preserve
  local symbol scope on the owned initializer rather than renaming the HBM
  initializer or changing its source unit.
