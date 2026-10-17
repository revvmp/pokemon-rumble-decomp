# F-808-4: TextBox drawing and text-writer interfaces

## Scope and ownership

Claimed #808 owns the native TextBox file beginning at 0x802A7690. This
prerequisite completes the interfaces needed by that file without adding a
source body, split, object registration, or executable-byte credit. The
canonical 0x140 TextBox from F-808-3, its field order and virtual order remain
unchanged. Existing Material, Font, CharWriter, and TextWriter storage is
preserved. Neighboring Layout/Picture (#809), Window (#810), and Material
(#811) retain their existing ownership and source inventories.

## Native evidence

An ordinary C++ TextBox candidate compiled using only these canonical
headers reproduces all 23 retained native functions, 8,108 instruction
bytes, and their physical order. An independent relocation audit pairs
156 text relocations and 33 data/constructor relocations to the original
executable. It reconstructs all 240 bytes in six non-text sections.

| Section | Native extent | Evidence |
| --- | --- | --- |
| .text | 802A7690-802A96DC | 23 retained functions, including the four-byte SetString tail branch |
| .ctors | 80352A8C-80352A90 | TextBox type-info initializer |
| .rodata | 80371608-8037162C | Nine border-drawing offsets |
| .data | 803C6FB8-803C7060 | Two border-property names and complete virtual table |
| .sdata | 804B8A18-804B8A1C | Wide newline |
| .sbss | 804B9420-804B9424 | Type information |
| .sdata2 | 804BB190-804BB1A8 | Native floating-point constants |

The separate TextBox source tree subsequently passed actual source selection
and full frozen-link verification: split-only default Ninja 517 targets,
NonMatching pipeline, and Matching source-link pipeline each exited 0,
with explicit frozen verification 0 after each. All 23 native final addresses
and sizes, all seven objdiff sections at 100%, source-only link selection,
and complete immutable DOL identity were checked by
`808-textbox-full-source-audit.py`. Independent pairing of the actual compiled
object passes 156 text plus 33 data relocations and 240 data bytes, with no
unresolved callee addresses. F-808-5 records this source evidence separately.
This header prerequisite still contributes no source or byte credit.

The subsequent helper claim #450 establishes the complete native CharWriter
unit at 802983D0-8029A78C. Four private static display-list setup declarations
are added under the same canonical class, with owner approval. They change
no storage, virtual order, or public API beyond the already evidenced public
GD setup method. All 13 retained functions / 9,116 instruction bytes and
physical retained order match using the canonical class. Independent native
pairing proves 429 text relocation targets and all 40 bytes of bss, sbss and
sdata2. The names are descriptive inferences from native GX calls. The two
last helpers at 8029A170/8029A4E0 belong to original #451 and are coordinated
with its TextWriter owner; their source credit belongs to the separate
complete CharWriter source delivery. No duplicate bytes are counted here.

The complete CharWriter source subsequently passes its actual split-only,
NonMatching and Matching pipelines, with explicit frozen verification 0
after each. All thirteen functions and all four sections are 100%; all
native final positions, 429 actual targets and forty data bytes are exact.
The actual six-unit source delivery at 9626374226fe96b6f58e1f4c05d5e9889dae43c3
passes configure, default Ninja and explicit frozen verification on its
historical base. Its canonical CharWriter header blob is identical to this
prerequisite's. Original #450's 36 rows and both #451 tails are audited in
the separate F-450-3 source finding. The final latest-main baseline remains
a separate required gate for this header prerequisite.

| Interface | Native evidence and naming confidence |
| --- | --- |
| Material::GetTevColor | Returns the existing signed-color record by value; original reference API |
| Font::GetCharStrmReader | 8029C320, 56 bytes; ordinary out-of-line call in native text measurement |
| CharWriter::SetupGXForDisplayList | 802990C0, 0x9F8 bytes; descriptive inferred name for the GD setup routine |
| TextWriterBase<wchar_t>::PrintMutable | 8029F6C0, eight-byte forwarder sets the third bool argument and branches to PrintImpl at 8029FF50; original reference name |
| TextWriterBase<wchar_t>::PrintMutableToDisplayList | 8029F6D0, eight-byte forwarder sets the third bool argument and branches to the GD implementation at 802A09D0; descriptive inferred name |
| TextWriterBase::PrintImpl | Correct three-argument signature includes bool mutableCursor; no alternative overload retained |
| TextWriterBase::PrintToDisplayListImpl | Descriptive inferred private counterpart with the same three arguments |
| GDSetCurrent/GDSetCurrOffset | Original SDK inlines update the canonical GDLObj pointer and cursor |
| strtoul | Standard unsigned-long conversion at 80347798, used by native border-color parsing |

The two public forwarders and Font reader method were compiled independently;
their exact instruction bytes and callee targets match. Compile-time size
probes establish TextBox 0x140, Material 0x5C, Font 0x10, CharWriter 0x4C,
wide TextWriter 0x64, and resource TextBox/Font/FontList 0x74/8/0xC.
Resource records and TextBox accessors follow native constructor accesses,
string copying, positioning, and color calls. SetTextColor takes Color by
value, as demonstrated by the retained caller's stack copy. The ordinary
SetFont and text-magnification helpers are declared for their original
implementations; unused bodies are dead-stripped in the diagnostic unit.

## Provenance

The original TextBox/resource helpers and text-writer signatures were checked
against the CC0 Skyward Sword reference at
`57a709b2128eb163a18e2969d561c9abb7d1fcee`, files
`include/nw4r/lyt/lyt_textBox.h`, `lyt_resources.h`,
`include/nw4r/ut/ut_TextWriterBase.h`, and `ut_Font.h`.
That Font reference explicitly prevents inlining GetCharStrmReader. Here its
header is declaration-only, preserving the target's actual out-of-line call.
The SDK inlines come verbatim from Petari
`e0ab141914921726a595573dc48d3f13d0922efe`,
`libs/RVL_SDK/include/revolution/gd/GDBase.h`.
No original spelling was recovered for the new GD-specific class methods;
their descriptive names are inferred from native behavior, not asserted as
recovered original names.

## Existing-source regression evidence

The unchanged 7f00715b baseline passed configure, the actual default Ninja
pipeline (513 targets), and explicit frozen verification, all exit 0.
After the eight-header proposal, the actual default Ninja pipeline and
explicit frozen verification again exited 0. All 506 existing source
objects were compared: 490 are byte-identical; 16 differ only in numeric
local identifiers in .strtab. Every allocated section, relocation, symbol
table, section-name table, and compiler metadata section is identical.
Each of those 16 objects was independently rebuilt with its actual pinned
Ninja compiler command and the original headers; all 16 control objects
reproduce their captured baseline hashes exactly.

The strict comparison permits only numeric anonymous/local names and numeric
suffixes of __FUNCTION__ identifiers. It does not mask instruction bytes,
data, relocations, addresses, or arbitrary strings. External proof files are
`808-textbox-header-baseline-proof.py`,
`808-textbox-header-control.py`, and the captured
`808-reference/textbox-header-baseline-7f00715b/proof.json`.
Canonical-only candidate compilation and independent paired audit are
`808-compile-textbox-canonical.py` and `808-textbox-paired-native.py`.
The API probes are `808-textbox-api-proof.py`.

The complete Material candidate was also recompiled against only these
headers: all 77 functions / 11,188 bytes, 221 text plus ten data relocations,
and 152 non-text bytes remain exact. This supplies regression evidence only;
its source credit remains with the separate #811 source delivery.

Independent owners recompiled their actual candidates against the exact
committed header tree at 9a4d16f4. Layout retains all 19 functions / 4,144
bytes and 98 paired targets; Picture retains ten / 1,336 and 68 targets.
All initialized data, constants, and constructor bytes remain exact. Picture
uses only its own pending canonical header in addition to this tree.
Animation retains all 27 / 5,396, 74 paired targets, its 36-byte vtable and
32-byte constants. These probes mutate no production source. External
scripts are `809-reprove-header9a4d.py`, `812-compile-tracked.py`,
`812-audit-probe.py`, and `812-paired-audit.py`, all exit 0.

After the four private CharWriter declarations, both independent consumers
repeated their individual audits against the exact final header candidate
6b26fc71: Layout19/Picture10 retain all instruction/data/relocation evidence,
and Animation27 retains all 5,396 bytes, 74 targets and 68 data bytes. The
repeat script is `809-reprove-header6b26.py`; the Animation scripts above
were rerun unchanged. No global build or production source was mutated.

## Final current-main header gate

The final current-main baseline is the exact verified #882 head
`c7dd9a00978a2e54153e90e1588c4de85cb0a911`. Its complete `all_source`
inventory contains 534 objects. The baseline was captured read-only after
the owner's plain Ninja, explicit `all_source`, and frozen verification
passed. Independent comparison with actual merged main
`1d16cf44e80e9219de742c53e0969e1298c4982a` finds every tracked file
identical except the expected progress image. Raw source objects, linked
DOL/ELF, linker configuration, Ninja file, complete tracked-tree inventory
and their hashes are preserved outside the repository at
`808-reference/879-final-baseline-c7dd9a00`.

After a clean rebase, the proposed header head
`9c387d244a3d2e57a9729120ceccdc8c19767277` built from a fresh output
directory: `python configure.py`, plain `.venv/Scripts/ninja` (541 targets),
explicit `.venv/Scripts/ninja all_source`, and the manifest-based frozen
verifier each exited 0. The Ninja-declared 534-object inventory exactly
equals the actual object files, and `ninja -n all_source` reports no pending
work. This explicitly includes NonMatching objects rather than assuming
the object directory is complete.

`879-header-baseline.py compare` and `control` both exit 0. All 534 objects
retain every allocated byte, section metadata and relocation. Of these,
515 are whole-file identical; the other nineteen differ only in strictly
numeric local `.strtab` identifiers. Recompiling all nineteen with the
original eight headers reproduces their exact baseline SHA-256 hashes.
The raw control objects and `control-proof.json` preserve that independent
check. All eight proposed header files are byte-identical to the earlier
native-consumer-proved `6b26fc71` definitions; the fresh complete build
also preserves the now-canonical Layout, Picture, Window and Animation
source consumers. This header prerequisite changes no source selection,
split or executable-byte credit.

## Traps

- An inline Font reader body adds 28 bytes to the native retained line-rect
  template. The target calls the original 56-byte out-of-line method.
- A ternary string-buffer length calculation changes narrowing and produces
  268 bytes instead of the native 272-byte SetStringImpl. The original
  two-return accessor preserves its native code generation.
- Direct color assignment omits the native by-value helper's stack copy.
  Keep the evidenced Color parameter rather than forcing instruction order.
- GD headers define RoundUp as a macro. Include the canonical layout common
  header before GDBase so its algorithm templates remain well formed.
- The border offsets are the target's 0.707f constant, not sqrt(0.5).
  Preserve the evidenced constant and normal row/column traversal.
- Header line additions can renumber local __FUNCTION__ symbols in SDK C
  objects. Compare all sections and reproduce the original-header control
  before classifying a difference as identifier-only.
