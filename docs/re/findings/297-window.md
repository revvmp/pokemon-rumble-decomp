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

## Traps

- The first-quadrant mismatch does not establish that the shared coordinate
  helper is wrong. Changes that reorder its table computations can damage
  multiple otherwise exact callers. Compare all retained callers before
  accepting a local improvement; the measured probes above did not improve
  the target match.
- The fresh full DOL passes while Window is still an original fallback.
  Its exact function sizes and 20 matching bodies cannot justify marking
  the complete unit Matching or closing either issue.

## 2026-11-04 isolated first-quadrant controls

This continuation leaves canonical source and headers unchanged. Direct compiles use the existing Wii/1.0a HBM flags and a private cloned LT-coordinate helper so the other exact callers remain isolated.

Explicit white-color array initialization and assignment controls do not improve 92.541336%. Selective volatile loads from the flip table reach 97.554665%, but introduce additional scheduling differences; those artificial partial forms are discarded. Caching the four used table bytes in ordinary u8 locals, in order origin-Y, origin-X, opposite-X, opposite-Y, while preserving the original ix-then-iy declarations, reaches 99.76%. Its complete native extent remains 1500 bytes, and every instruction is in target order. Sixteen register-operand differences remain: two temporary allocation swaps. This is not exact acceptance and is not retained in production.

Additional controls on the cached form include 256 mixed scalar-type combinations, all 24 coordinate declaration permutations with index orders and uniform types, 48 pointer/reference/read-helper variants, 24 helper argument orders, eight individual optimization-pass controls, explicit byte-offset addressing, equivalent table-address spellings, and selective volatile masks. None closes the remaining swaps. Failed probes that declared an offset after its first use are excluded from binary conclusions.

The closest complete candidate and objdiff are preserved in ignored `tools/local/session/window-61-compile/perm-win.cpp` and `perm-win.json`; the reusable drivers are prefixed `window-61`. No split, compiler configuration or Matching status is changed.

## Exact DrawFrame origin-coordinate lifetime

The first quadrant now uses a private GetLTSingleTexCoord helper, leaving the existing shared GetLTTexCoord and its exact callers unchanged. The private helper caches originY then originX as u8 values, with originY = static_cast<u8>(originY) immediately after its load. The same-type reassignment prevents originY from sharing the register selected for the scaled X index. Loading the opposite Y coordinate directly inside its final expression, rather than retaining the previous cached y2 local, resolves the remaining table-pointer versus X-difference register swap. The opposite X coordinate also stays in its expression; caching it is unnecessary. The equations, texture table and numeric conversions remain identical.

This private helper and redundant u8 reassignment are intentional D-0016 source shaping. They follow the documented honest reference, type, ordering and lifetime attempts, and do not claim original helper names or original source structure. A fresh Wii/1.0a direct compilation of the formatted tracked source gives all 21/21 functions exact and function order 21/21. Evidence is tools/local/session/window-ppu/accepted.cpp and accepted.json. The private helper emits no out-of-line symbol. Native Matching selection and frozen-SHA validation remain with the coordinating maintainer.

### Additional traps

The cached four-byte candidate improves from 99.76% to 99.89333% after only the originY reassignment, but still has six operand differences. Dedicated enums, coordinate permutations, index reassignment, pointer and table views, and integer-difference helpers do not close those differences. Removing the opposite-Y cache does. Retaining additional pointer self-assignments or several byte reassignments often crosses the inline budget and regresses DrawFrame to roughly 58.4%; such variants are scratch-only. The exact retained helper caches only the two origins and keeps the opposite-coordinate loads in their expressions.

The coordinating maintainer selected Window Matching and ran a fresh native build: ninja exited 0 and the complete rebuilt main.dol passed the frozen SHA verifier. The original duplicate metadata and weak destructor ownership resolve correctly in the native link; no symbol, split, header or compiler-flag adjustment was needed. All twenty-one functions and owned data are now source-linked.
