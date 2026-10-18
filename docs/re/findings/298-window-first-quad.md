# F-298-1: independent Window first-quad comparison

## Scope and ownership

Issue #298 inventories seventeen functions and 8,076 native bytes from `0x8018F890` through `0x80191874`. Issue #297 owns the first function of the complete `lyt_window.cpp` unit and its existing split, object registration and class header. Its owner approved an independent body-only investigation for #298. This finding changes none of those files and accepts no additional reconstructed bytes.

The inspected canonical source is commit `c23bc9b97f82a17206c8ac8d4d90c5cb11039487`. The existing unit remains `NonMatching`. The prior complete-unit comparison and unsuccessful source variants are recorded in [F-297-1](297-window.md) and [F-295](295-nw4hbm-lyt.md).

## Independent comparison

An isolated Wii/1.0a compilation of the canonical source reproduces the existing comparison: twenty complete-unit functions have matching sizes and non-relocation instruction streams; `Window::DrawFrame` at `0x8018FD30` has the correct `0x5DC` size and 52 differing non-relocation words. These individual instruction comparisons do not establish source selection, relocation correctness, or complete-unit acceptance.

Within the original #298 inventory, sixteen functions, totaling 6,576 bytes, have exact non-relocation instruction streams. Its remaining 1,500-byte DrawFrame body is unfinished. No additional source byte credit is claimed for the already-canonical bodies.

A separate strict comparison of those sixteen owned functions resolves all 99 code relocation sites through independently named native functions and the existing unit's evidenced data bases. It verifies each actual native target and then compares every remaining instruction bit, including branch opcode/control bits. All sixteen pass. This strengthens the individual-function evidence without accepting the unfinished whole unit or proving a source-selected final link.

The differences are confined to offsets `0x84` through `0x180`, followed by indexed loads at `0x1A8` and `0x1AC`. The first range covers the first LT quad's table-byte conversions, white vertex-color initialization and register allocation. The native function reserves `r0` for the four white-color stores, whereas the current compilation uses `r5`; the native table-byte intermediates consequently occupy different registers. The other three quads match. This describes the symptom, rather than proving that the color constructor causes it.

Both the preserved CC0 KEY `7f2a9ab9` and TP Window reference snapshots were inspected alongside the canonical Size, Color and VEC2 declarations. The canonical Color default constructor already uses the reference's ordinary `*this = WHITE` assignment, and VEC2's two-float construction and pointer conversions agree. The references' explicit anonymous-namespace forward declarations for all nine local helpers were restored in an isolated diagnostic. Every comparison result remained unchanged: DrawFrame still has the same 52 differences and all other compared functions remain exact. No canonical header or source modification was justified by this experiment.

## Commands and results

The external diagnostic was copied under a distinct issue prefix and reads the claimed worktree's canonical source and headers. It writes only external `298-reference` objects and reports. It reuses the established #297 compiler invocation and native split object without changing either.

| Command, from the #298 worktree | Exit | Result |
| --- | --- | --- |
| `python C:/MassStorage/GitHub/298-probe.py base` | 0 | DrawFrame 1,500 bytes, 52 non-relocation differences; other twenty functions exact |
| `python C:/MassStorage/GitHub/298-probe.py referenceprototypes` | 0 | Same result with the reference's nine helper forward declarations |
| `python C:/MassStorage/GitHub/298-audit-owned-native.py` | 0 | Sixteen original functions, 6,576 native bytes and 99 independently resolved relocation sites exact |

No full build or frozen artifact verification was performed for this findings-only checkpoint. There are no source, header, split, compiler flag, object-registration or progress changes. The issue remains open.

## Traps

Matching the later three quads does not establish a whole-function match. Do not promote the current same-sized DrawFrame body to Matching or infer that choosing its white constant's register is a legitimate source fix. Preserve the canonical Color/Size/VEC2 interfaces and require independent evidence before changing a shared type. The previously measured declaration, chained-assignment, operand-order and explicit color-initializer variants should not be repeated without new structural evidence.
