# F-213-1: CTutorialMsg contracts and containing unit

## Scope

Issue #213 owns the first function of `CTutorialMsg` at `0x8011AB90`.
The containing translation unit also includes all of #214 and #215 and
the first 32 functions of #216. Its final function ends at `0x8011D088`.
The next function, `0x8011D090`, constructs a different menu callback class.
The tutorial unit therefore contains 114 original functions and 8,712 code
bytes. Completing its tutorial contribution alone does not complete #216.

## Shared layout prerequisite

The native tutorial loader and independently reconstructed #214 methods
establish the following contracts:

| Entry | Contract | Evidence |
| --- | --- | --- |
| `0x801D3830` | `CLayout::CalculateMtx(bool)` | The loader passes `true`; the complete native #214 calculation body passes the same argument before reading pane translation. |
| `0x801CB0E0` | `CLayout::CMotionProgram::GetFrame() const` | The native #214 insertion body reads the current motion frame and passes its float result to the replacement motion. |
| `0x8011AF20` | `Pane::GetTranslate() const` | An eight-byte accessor returns the address of the pane's translation member. |
| `0x802A4E80` | `Pane::GetPaneRect() const` | The producer consumes only the pane pointer after the hidden rectangle-result pointer. It derives the rectangle from pane size and origin. Native tutorial and numeric-input callers provide no draw-info argument. |

The old NW4R rectangle declaration imported a `const DrawInfo&` argument
that this target does not consume. The prerequisite corrects the declaration,
definition, and corresponding symbol together. There are no existing NW4R
source callers. The separate `nw4hbm` interface is unaffected. The layout
method names are descriptive reconstructions, rather than recovered debug
names.

The first same-base control used main `0d5b446a`. Ninja's actual `all_source`
inventory contained 742 objects. All 742 retained identical allocated section
bytes, exception data, relocation targets, and symbol attributes. Of those,
716 were wholly byte-identical; 26 changed only compiler-generated numeric
names or the corrected rectangle function's mangled name. The merge verifier
repeats the comparison against its current base after every rebase, followed
by the frozen executable verification.

The source-selected tutorial fixture independently reproduces all 26
original #213 functions, 1,980 code bytes, every relocation target, and
18 exception records totaling 248 bytes. The independently audited #214
fixture reproduces all 25 original functions, 2,648 bytes, and 13 exception
records totaling 236 bytes. These fixtures establish the prerequisite's
caller contracts; they do not constitute final whole-unit acceptance.

## Compiler and source-context controls

Canonical Wii/1.1 and Wii/1.3 both expanded the factory to 356 bytes rather
than the native 140. Removing file IPA still produced 284 bytes. The native
unit retains calls through its tiny template and accessor layers throughout.
A scratch-only inlining-disabled control reproduces the factory and its
constructor, along with the other original #213 functions. Distinct native
allocations for repeated motion-name literals also require disabling string
reuse in the scratch control. No production compiler flags are changed by
the shared prerequisite.

The loader must be an inline class member in the proven fixture. An
out-of-line definition introduces a common base for three long string
addresses, changing its frame and instructions. The member definition
avoids that transformation without changing optimization level. Reading
the stored initial scroll multiplier, rather than substituting its constant
value in the later expression, preserves the native floating multiply.

Ordering the inline method definitions as loader, insertion, hiding, and
calculation reproduces the native retained literal order. The current
containing-unit fixture matches 37 retained data objects, 527 logical bytes,
including all pointer relocations and their physical order. Full unit
registration and executable acceptance remain pending.

## F-213-2: Unaccepted containing-unit producer

The complete outside producer reconstructs all 114 original instruction
streams, 8,712 code bytes, and all 72 exception records, 988 bytes, when
relocations are assigned their native locations. The containing unit's
retained data has 37 objects and 527 logical bytes. An actual pipeline
object reproduces the same instruction and exception records. These are
producer checks, not whole-file acceptance.

The real split has text `0x8011AB90..0x8011D088`, extab
`0x8002A3C8..0x8002A7A4`, index `0x8004C170..0x8004C4D0`, data
`0x803AEDD0..0x803AEF4C`, small data `0x804B74B8..0x804B7574`, and
float constants `0x804BA140..0x804BA148`. The last data and small-data
strings consume their ordinary four-byte allocation padding; four further
bytes before each following unit remain outside this split. The actual
NonMatching split passes the frozen executable check.

The temporary source-selected link fails. Its in-class and instantiated
template definitions do not yet emit in the complete native order. An
implicit concrete constructor, with the explicit interface constructor
retained, fixes the early constructor/accessor order while preserving all
114 instruction streams and 72 exception records. Replacing the factory's
arrow with `get()`, marking its out-of-class constructor inline, or spelling
out the nested message's three special members does not fix the remaining
order. Moving the false erase implementation out of class defers it, but
introduces other ordering differences. Merely moving that out-of-class
definition before or after reallocation does not change the result.

The real link also exposes a binding problem hidden by assigning relocation
targets independently: the current namespace-static vector length-error
helper is local to the new object, whereas the native calls in allocation
and growth reach `0x80088550`, outside this unit. Making it an ordinary inline
global lets it share the existing helper and removes 960 extra output bytes.
However, that change also changes the register allocation of existing
Matching controller code. It is rejected. Neither an error-helper binding
change nor a source-selected tutorial unit is accepted.

The initial backend preflight derived 747 source objects from Ninja and
rebuilt all 45 consumers of vector or allocator. Thirty-eight retained
identical allocated bytes and semantic relocations. The seven changed
objects were all NonMatching: BWorld, CGameSession,
CGloballyReusableTexBuf, CMsgBalloon, CSaveManager, SaveData, and ScriptCamera.
The changed previously exact accessors remain byte-identical after their
storage-base ownership correction. Other mapped changed functions already
failed their native streams; some allocation and destruction streams improve.
Several provisional helper associations remain unresolved. This is not a
claim that every NonMatching object is preserved. The later global-helper
trial additionally fails Matching-consumer preservation and is not part of
the accepted layout prerequisite.

A recovered MSL declaration/emission and common error-helper contract is
still required to establish a faithful whole-file link. The complete source
and rejected controls are preserved privately for continuation. No tutorial
issue is completed, no partial-source pull request is opened, and the
unaccepted backend is not merged.

## Traps

- Repeated literal symbols require a single consistent native address for
  each symbol and addend. Independently choosing an address for each use
  falsely accepts pooled strings. The strengthened audit also checks one
  inferred native base per literal section.
- Reserve emits additional allocator-taking constructors. A broad template
  name prefix incorrectly assigns these to the issue's default constructors.
  Audit exact constructor parameter types against the original inventory.
- A nested message containing one wide string explains the native default,
  copy, and destruction wrapper calls. A raw `vector<wstring>` removes these
  genuine layers and does not match.
- The surveyed #216 start is not a translation-unit boundary. Vector growth
  helpers from the tutorial class continue through `0x8011D088`.
- The first vector clear forwarding proposal changed existing BWorld
  register allocation and failed the current-consumer gate. Those changes
  remain outside the prerequisite; matching isolated tutorial helpers is
  insufficient to publish a shared backend change.
- A source-defined local callee cannot be assigned an external native
  address. Verify symbol binding and the actual linked address, rather than
  accepting only normalized call instructions. A weak definition may reuse
  an existing global definition; a namespace-static definition cannot.
- Do not end data splits at an odd logical string end. Include only that
  object's normal allocation padding, and leave the following unit's gap
  outside. The odd tutorial data ends otherwise create invalid automatic
  split alignment.
