# F-297-1: Window first-quadrant instruction scheduling

## Scope and baseline

Issue #297 lists 30 functions and 7,076 code bytes. Its Picture tail,
ResourceAccessor, and TextBox functions already belong to matching source
units described in [F-295-1](295-nw4hbm-lyt.md#f-295-1-unit-boundaries-from-0x801895e0-to-0x80191874).
The remaining four survey entries start the Window unit at `0x8018F380`.
First-function ownership assigns the complete existing Window file to #297;
its later functions belong to the neighboring #298 inventory.

On main `a914962b655f8e9c6c9436436112405437d46e95`, a fresh default-directory
`python configure.py --map`, plain `.venv/Scripts/ninja` (512 steps),
`ninja all_source`, and the explicit frozen-manifest verifier all exit 0.
Window remains `NonMatching`; this passing baseline selects its original
object and establishes no new source contribution.

The existing Window source produces the exact native sizes and every
non-relocation instruction word for 20 of its 21 retained functions.
`DrawFrame` at `0x8018FD30` has its native size of `0x5DC` bytes but differs
in 52 non-relocation instruction words. The differences lie at function
offsets `0x84` through `0x180`, then `0x1A8` and `0x1AC`, inside the first
left-top quadrant. Later quadrant instruction words match. The native
stream uses `r0` for the four white vertex colors while the candidate uses
`r5`; table-byte conversion scheduling and register allocation also differ.
These are diagnostic comparisons with relocation words excluded, not
complete relocated-object acceptance.

## Reference comparisons and probes

The candidate follows the already recorded CC0 Kirby's Epic Yarn source
`Swiftshine/key` at `7f2a9ab9`. The analogous Twilight Princess Window,
Color, Size, and VEC2 declarations were inspected as additional source
context. Both references use the same byte-table representation and VEC2
texture-size local. Their existence does not prove this target's precise
expression spelling.

Scratch-only probes retained canonical repository headers and the actual
Ninja compiler command. Wii/1.0RC1 produces the same DrawFrame instruction
stream as the established Wii/1.0a compiler. The following equivalent
source forms also produce the same 52-word difference: value or const-value
texture size, const table reference, const or narrower index locals,
reversing the multiplication operands, splitting the material setup
expression, explicit zero Size construction, and the reference's global
TextureFlipInfo declaration.

All 24 orders of the four left-top coordinate assignment statements and
all 15 nontrivial reversals of their chained assignments were measured.
They fail to match and generally damage the otherwise matching DrawFrame4
and DrawFrame8 streams. No probe changed production source, compiler flags,
metadata, or matching status. The remaining cause is unresolved.

## F-297-2: resumed owner controls and texture-origin locals

The released clean issue worktree was reused with its prior owner's approval
after a fresh helper claim. The neighboring #298 owner approved independent
Window investigation and eventual acceptance of its separate original inventory
in the same first-owned unit. The canonical source remains unchanged and
NonMatching.

On the current canonical source, `unitdiff.py` identifies 20 of 21 retained
functions as exact. The first-quad `DrawFrame` gap remains 52 non-relocation
words. Const color arrays, const setup results, explicit white initialization,
direct word-assignment Color constructors, and explicit inline hints retain the
gap. Widening the setup-result local or moving color construction across the
texture-size call changes the function size and is rejected. Splitting chained
coordinate assignments damages the previously exact callers.

Naming the two unsigned-byte texture origins inside the existing left-top
coordinate helper reduces `DrawFrame` to ten differing words at the unchanged
1,500-byte size. It also increases `DrawFrame4` and `DrawFrame8` by four bytes
and damages their instruction streams. This is a rejected diagnostic candidate,
not a unit match or evidence for changing production source. Const-reference,
signed-int, and unsigned-int origin locals also fail the complete caller check.
The cause remains unresolved; native byte conversions and alias behavior merit
further investigation.

The command is `297-probe-owned-window.py`, with distinct outputs in
`297-reference/owned/`. In particular,
`297-owned-coordinate-origins.cpp/.o/.json` preserves the ten-word candidate;
the base snapshot and all earlier #298 proofs remain intact. These diagnostics
exclude relocation words and do not substitute for the required complete
source-selected, relocation, data, and frozen artifact acceptance gates.

## Traps

- The first-quadrant mismatch does not establish that the shared coordinate
  helper is wrong. Changes that reorder its table computations can damage
  multiple otherwise exact callers. Compare all retained callers before
  accepting a local improvement; the measured probes above did not improve
  the target match.
- The fresh full DOL passes while Window is still an original fallback.
  Its exact function sizes and 20 matching bodies cannot justify marking
  the complete unit Matching or closing either issue.
