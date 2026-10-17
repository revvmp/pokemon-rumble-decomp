# F-807-1: interfaces required by Pane and Group

## Scope and provenance

Issue #807 owns `lyt_pane.cpp` and `lyt_group.cpp`. Their existing canonical
headers describe the complete Pane and Group layouts. This prerequisite adds
the complete storage layouts and required virtual interfaces of their external
dependencies: DrawInfo (#808), Layout (#809), Material (#811), and animation
types (#812). It takes no source bodies, source splits, or object registration
from those issues. It contributes no reconstructed executable bytes.

Declarations were checked against the CC0 Xenoblade reference at
`ea25691b2029394ab777bda7060139e517a13cba`, under
`libs/nw4r/include/nw4r/lyt`, and the CC0 Skyward Sword reference at
`57a709b2128eb163a18e2969d561c9abb7d1fcee`, under `include/nw4r/lyt`.
The Skyward Sword animation Bind signatures retain the disable argument, as
the target requires. The Xenoblade signatures omit it and cannot be copied
unchanged. Only interfaces needed by the owned source are declared; omitted
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
