# F-807-1: interfaces required by Pane, Group, and Window

## Scope and provenance

Issue #807 owns `lyt_pane.cpp` and `lyt_group.cpp`; issue #810 owns
`lyt_window.cpp`. Their existing canonical
headers describe the complete Pane and Group layouts. This prerequisite adds
the complete storage layouts and required virtual interfaces of their external
dependencies: DrawInfo (#808), Layout (#809), Material (#811), and animation
types (#812). Window additionally needs the Material texture-count accessor,
GetTextureSize declaration, allocator construction templates, and resource
block declarations. It takes no source bodies, source splits, or object registration
from those issues. It contributes no reconstructed executable bytes.

Declarations were checked against the CC0 Xenoblade reference at
`ea25691b2029394ab777bda7060139e517a13cba`, under
`libs/nw4r/include/nw4r/lyt`, and the CC0 Skyward Sword reference at
`57a709b2128eb163a18e2969d561c9abb7d1fcee`, under `include/nw4r/lyt`.
The Skyward Sword animation Bind signatures retain the disable argument, as
the target requires. The Xenoblade signatures omit it and cannot be copied
unchanged. Only interfaces needed by the owned sources are declared; omitted
nonvirtual methods can be added by their source owners.

## Target evidence

| Type | Size | Proven storage and interface |
| --- | --- | --- |
| AnimationLink | `0x10` | Intrusive node at 0, transform pointer at 8, unsigned-short index at C, disable byte at E; native AnimateSelf at `0x802A5650` reads all three payload members. |
| AnimTransform | `0x14` | Vptr at 0, node at 4, resource pointer at C, frame at 10. Pane BindAnimation at `0x802A5710` dispatches through slot 14 and preserves both Boolean arguments; AnimateSelf dispatches through slot 1C. |
| AnimResource | `0x10` | Four resource pointers in reference order. FindAnimationLink at `0x802B05B0` compares the resource-block pointer at 4. |
| Material | `0x5C` | Animation list at 4; three signed TEV colors at 10; four constant colors at 28; packed capacity/count words at 38/3C; GX storage pointer at 40; 21-byte name at 44; user-allocation flag at 59. Pane destructor, name lookup, animation, unbind, and both enable overloads agree with the corresponding native virtual slots. |
| DrawInfo | `0x54` | Vptr at 0, view matrix at 4, view rectangle at 34, adjustment scale at 44, global alpha at 4C, packed flags at 50. Native CalculateMtx at `0x802A51A0` and LoadMtx at `0x802A5AA0` prove these accesses and flag positions. |
| Layout | `0x20` | Existing complete layout retained. NewObj obtains storage from the existing allocator, checks for null, and uses placement construction. Group's constructor at `0x802A5D00` inlines this operation for a 12-byte PaneLink. |

A temporary external compiler probe asserted all six sizes with the pinned
Wii/1.1 compiler and established NW4R flags; exit 0. Scratch Pane and Group
source compiled with these interfaces reproduces the native non-relocation
instruction streams for constructor/destructor, rectangle, recursive lookup,
matrix calculation, drawing, animation, bind/unbind, enable, matrix loading,
vertex positioning, material access, extended-user-data lookup, and all five
Group functions. This is interface evidence, not final source acceptance;
source integration must independently prove full-section and frozen-DOL
identity before any unit is marked Matching.

## Validation

The unchanged baseline at `f33890ff` passed configure, the full default Ninja
entry point, and explicit frozen verification, all exit 0, with a clean tree.
The prerequisite branch must also pass those checks after rebasing onto the
latest main, as recorded in its pull request.

## Window prerequisite and Size return convention

The target Window constructor at `0x802A96E0` passes the resource block and
ResBlockSet to Material construction. The latter is four pointers, at offsets
0, 4, 8, and C: texture list, font list, material list, and accessor. MaterialList
is C bytes with the material count at offset 8. The new canonical resource
header also records Pane (4C), Group (1C), ExtUserDataList (C), and ExtUserData
(C), required by the Pane and Group source. Window's own resources remain
with its source issue. Pane's missing SetExtUserDataList virtual belongs at
slot 70, before LoadMtx at 74; this ordering is also required by Window's
native virtual dispatches.

The Size copy constructor is an ABI requirement, not merely a convenience.
Native GetTextureSize at `0x802AEAC0` consumes a hidden result pointer in r3,
Material in r4, and texture index in r5. Without an explicit copy constructor,
Wii/1.1 returns the eight-byte Size in registers and introduces extra stores
in the Window drawing routines. Adding the reference copy constructor makes
DrawSelf, DrawFrame, DrawFrame4, and DrawFrame8 reproduce all native
non-relocation words and exact sizes (472, 1552, 1844, and 3624 bytes).
All 18 functions in the #810 inventory then match their native instruction
streams and sizes. Linked relocation and whole-artifact proof remains the
source pull request's separate acceptance gate.

The expanded header baseline was committed head `e4910aca`, rebased onto
main `49eaab74`. Configure, full default Ninja, and explicit frozen verifier
each exited 0 before and after the change. SHA-256 comparisons covered all
451 existing source objects: 450 were byte-identical. The sole difference,
lyt_common.o, was reproduced from the captured baseline and inspected by ELF
section: text, data, relocations, symbol table, section names, and compiler
metadata were identical. Only four digits in two compiler-generated local
constant names in the string table changed. Its owned-section objdiff
comparison remained 100%, and the full immutable DOL still verified.

## Traps

- Including Layout's allocator header before `lyt_common.h` exposes the SDK
  RoundUp/RoundDown macros before the NW4R templates of those names, causing
  compiler syntax errors. Put the NW4R common header first in these units;
  no shared SDK macro declarations were changed.
- The reference Pane visibility flag alone is insufficient for this target's
  draw/matrix gate: zero scale suppresses processing, and zero alpha suppresses
  descendants when alpha influence is enabled. Native Animate still tests the
  visibility flag directly. Preserve the distinction in the owned source.
- Two four-byte survey functions at `0x802A4FD0` and `0x802A5000` are trailing
  return instructions of the preceding color-element switches, not independent
  source routines. Their ownership and byte coverage remain in Pane.
- A trivially copyable Size silently selects a different aggregate return
  convention. Preserve the evidenced explicit copy constructor before
  diagnosing Window drawing differences as source arithmetic or flags.
