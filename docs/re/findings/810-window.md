# F-810-1: complete Window translation unit

## Scope and provenance

Issue #810 lists 18 Window functions, 9,496 code bytes, beginning with its
constructor at `0x802A96E0`. The native file also contains its runtime-type
getter at `0x802ABC60` (8 bytes) and static initializer at `0x802ABC70`
(12 bytes), listed in the adjacent #808 survey. The first-function ownership
rule assigns the complete Window source file to #810. Those two neighboring
functions are included explicitly; #808 still has other unfinished units.
No Layout, TextBox, Material, or Bounding source is taken by this work.

The implementation is reconstructed from the CC0 Skyward Sword reference
`57a709b2128eb163a18e2969d561c9abb7d1fcee`, `src/nw4r/lyt/lyt_window.cpp`
and `include/nw4r/lyt/lyt_window.h`, with the CC0 Xenoblade reference
`ea25691b2029394ab777bda7060139e517a13cba` used to cross-check resources
and allocator construction. The latter's Window virtual interface differs
from the target; its additional animation-link overrides are not imported.
Original names are retained where supported by these references and the
native mangled symbols. TextureFlip names are descriptive reference names;
the executable does not independently establish their original spelling.

## Native boundaries and storage

| Section | Native extent | Evidence |
| --- | --- | --- |
| text | `0x802A96E0–0x802ABC7C` | Constructor through retained static initializer; the next Bounding constructor starts at BC80. |
| ctors | `0x80352A90–0x80352A94` | Entry points to Window's initializer at BC70. |
| data | `0x803C7060–0x803C7130` | Six ten-byte texture-flip records, four alignment bytes, and the 0x90-byte Window vtable at 70A0. |
| sbss | `0x804B9428–0x804B942C` | Four-byte runtime-type object; the following four bytes are padding, not an eight-byte object. |
| sdata2 | `0x804BB1A8–0x804BB1C0` | Zero, alignment padding, and the two integer-to-double conversion constants referenced by drawing routines. |

Window is 0x108 bytes: Pane base at 0 (0xD8), content inflation at D8
(0x10), Content at E8 (0x18), Frame pointer at 100, frame count at 104,
and final alignment padding. Content contains four colors at 0 and the
eight-byte texture-coordinate array at 10. Frame is eight bytes: flip byte
at 0 and Material pointer at 4. The constructor, destructor, content/frame
drawing, and animation access these members at the stated offsets.

The 0x90-byte vtable preserves Pane's SetExtUserDataList slot 70 and LoadMtx
slot 74, then adds GetContentMaterial (78), GetFrameMaterial (7C),
DrawContent (80), DrawFrame (84), DrawFrame4 (88), and DrawFrame8 (8C).
Its runtime-type getter returns the object at `0x804B9428`; the initializer
sets that object's base pointer to Pane's runtime type at `0x804B9408`.
These references prove the two adjacent #808 functions belong to Window.

## Reconstruction evidence

The established Wii/1.1 NW4R flags reproduce all non-relocation instruction
words and exact native sizes for the issue's 18 functions: constructor
644, destructor 324, recursive material lookup 252, color accessors
20/44/24/24, DrawSelf 472, AnimateSelf 144, UnbindAnimationSelf 144,
DrawContent 268, DrawFrame 1552, DrawFrame4 1844, DrawFrame8 3624,
GetMaterialNum 16, GetMaterial 44, GetFrameMaterial 40, and
GetContentMaterial 16 bytes.

The complete split baseline passed configure, full default Ninja, and
explicit frozen verification, each exit 0. The initial split rejected the
survey's eight-byte runtime-type symbol crossing the four-byte sbss extent;
correcting that owned symbol to its evidenced four-byte size restored the
baseline. Shared interface prerequisites are isolated in PR #820, with
the source-bearing Pane/Group dependency in PR #824. Final linked-source
acceptance passed with those declarations: all 20 native functions and every
owned section scored 100%, the Matching build selected the compiled Window
source object without its extracted fallback, and all linked addresses and
sizes remained native. Default Ninja and the explicit frozen verifier exited
0; an independent coverage audit checked all 9,516 code bytes and full DOL
byte identity. The exact committed head is revalidated and recorded in the
pull request before merging.

Two outside weak Color helpers are named solely to resolve the source's
array-construction and destruction callbacks: default constructor at
`0x8006CDE0` (12 bytes) and destructor at `0x8005C9A0` (64 bytes).
Their canonical header bodies reproduce the native non-relocation words and
exact sizes. Only their names change; their original splits, sizes, and
types are retained, and their 76 bytes receive no #810 source credit.

## Traps

- Without the explicit Size copy constructor, Wii/1.1 returns Size in
  registers while native GetTextureSize expects a hidden result pointer.
  DrawSelf grows by 28 bytes, DrawFrame and DrawFrame4 by 16 each, and
  DrawFrame8 by 32. Restoring the reference declaration removes every
  non-relocation difference; see [F-807-1](807-layout-interfaces.md#window-prerequisite-and-size-return-convention).
- The two-argument allocator construction template takes its parameters
  by value. Replacing that evidenced interface with guessed reference
  parameters changes Window constructor code generation.
- GetContentMaterial dispatches through Pane's no-argument virtual material
  getter. Replacing that original virtual call with a direct member access
  changes the native behavior and instruction stream.
- HBM also has a lyt_window.cpp initializer. Renaming the owned initializer
  without local symbol scope initially caused a duplicate-symbol link error
  in the NonMatching baseline. Preserve its compiler-evidenced local scope;
  do not rename or alter the HBM unit to hide the collision.
