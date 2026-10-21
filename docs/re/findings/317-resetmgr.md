# F-317-1: reset manager scope and native callback representation

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. This investigation owns issue #317 and uses an isolated `rumble-317` worktree. No reset source is selected, no reconstruction credit is claimed, and no shared header changes are published by this checkpoint.

## Ownership and source identity

The first fourteen surveyed functions, `0x801B4FC0` through the initializer ending at `0x801B5308`, belong to the preceding `pad.cpp`, whose first-function owner is #315. The #316 contributor is reconstructing that complete Pad unit with #315's authorization. The independently owned reset manager begins at `0x801B5310` and ends at `0x801B64A8`.

The original exception and RTTI strings spell `NResetMgr::@unnamed@resetmgr_cpp@::IDCallback`. The callback record has an integer identifier at zero, an integer exclusion mask at four, and a twelve-byte `std::tr1::function<void()>` at eight. A list node adds the two links before that twenty-byte value and is allocated as twenty-eight bytes. The list itself is twelve bytes. Registration allocates the list lazily, assigns identifier one initially or the last registered identifier plus one, and appends the record. The existing `RegisterResetCallback` declaration discards the native integer result; matching its definition requires retaining that result.

The manager owns the action word at `0x804B9148`, the argument at `0x804B914C`, a disable count at `0x804B9150`, and the list's shared pointer at `0x804B9158`. The setting action reads the argument's first byte, whereas manual viewing and restart consume its complete word. An ordinary union expresses these two views. The existing `CScopedActivity` constructor increments the count; its destructor clamps the decremented count against a static integer zero at `0x804B8220`. Its original class spelling remains under investigation; later RTTI mentions `NResetMgr::CDisable` but does not independently name this nonvirtual constructor.

## Reconstructed diagnostics

The independent C++ diagnostic is `C:/MassStorage/GitHub/317-reference/resetmgr.cpp`. It uses actual Wii/1.1 game flags and canonical project headers except the explicitly disclosed scratch proposals for list traversal and callable copying. Compiler, disassembly, relocation-pairing, and exception comparison entry points are `317-prepare-probe.py`, `317-audit-probe.py`, and `317-audit-eh.py` outside the checkout. The reading-aid database is a separate `317-reset.i64`; the tracked database is untouched.

Fourteen ordinary callback, initialization, enable, activity, request, predicate, and cancellation bodies reproduce every instruction and actual relocation target. Additional exact diagnostics include the callback-record destructor, shared-pointer destructor, list control-block disposal, allocation-holder destructor, and control-block destructor. These are diagnostic observations, not accepted source coverage.

The main calculation routine has the complete native `0x6C4` bytes. Its only forty-two differing instruction words are a consistent exchange of the iterator and current-record registers in each of the six reverse-traversal loops. All other instruction bytes, branches, constants, and paired targets agree. Wii/1.3 retains the same register differences. No register variables, compiler flags, pragmas, binary substitution, or manual dispatch were introduced to remove them.

Registration has the complete native `0x3FC` bytes. Its remaining eleven instruction differences belong to the inlined list-clear path and its temporary registers. A separate move-pointer list-cleanup destructor also retains register differences. These unresolved implementations prevent whole-unit acceptance.

The actual compiler output independently reproduces all four compared exception records, including their complete payloads and destructor relocation targets: initialization eight bytes, calculation `0x218` bytes, activity destruction eight bytes, and registration `0x78` bytes. Exception-index source selection and the complete native unit have not yet been accepted.

## Callable-copy representation

The scratch callable extension adds ordinary copy assignment and swap. Copy assignment constructs a temporary copy, then calls the destination's swap. Swap exchanges the target pointer and two storage words, then repairs pointers into either object's local storage. A union containing the existing eight-byte character buffer and two unsigned-long words preserves the twelve-byte class layout and naturally reproduces every instruction of the native copy-assignment sequence inside registration.

Casting the character buffer to word pointers preserved behavior but introduced different address calculations and register scheduling. A separate temporary buffer struct increased stack storage. Neither rejected diagnostic is published. The union proposal remains scratch-only: it requires complete existing-source object controls, retained ELF checks, and full frozen-DOL verification before any canonical-header change.

## Traps

- The reading-aid decompiler presents several OS-action cases as fallthrough because of inferred callee behavior. Native instructions include explicit branches to the common end after every action. Check the actual branch words before changing source control flow or adding nonreturning declarations.
- A twelve-byte callable can have a later stack or exception anchor thirty-six bytes after its start. Such an anchor is not evidence for a thirty-six-byte callable. Registration's twenty-eight-byte node and actual pointer accesses establish the twelve-byte callable here.
- An implicit copy constructor for a scratch reverse-iterator wrapper kept extra iterator temporaries on the stack. An ordinary explicit copy constructor removed those stores, but the remaining register allocation still differs; this is not evidence that a particular public iterator interface has been recovered.

## Next gate

Resolve the reverse-iterator and list-clear differences through independently justified standard-library source structure. Then publish only the required canonical-header changes with full current controls, establish the reset unit's complete section ranges, and run the original-only split check before any source-selected whole-unit and frozen-artifact acceptance. Closure of #317 additionally requires the fourteen Pad-tail functions to be accepted through the #315-owned complete Pad unit.

# F-317-2: canonical callable copy and swap prerequisite

The bounded header change supplies ordinary `std::tr1::function<void()>` copy assignment and swap. Its union adds a word view over the existing eight-byte character buffer, retaining the twelve-byte complete object. The existing callable constructor, destructor, invocation, local-buffer predicate and virtual interfaces are preserved. This is an interface prerequisite, with no source selection, split, fallback change or source-coverage credit.

## Native consumer proof

The reconstructed reset registration consumer, compiled with the canonical proposed callable header and a separately disclosed scratch-only list traversal extension, has exactly the native `0x3FC` bytes. Every instruction and actual call/data target from offset `0x1A0` through the end agrees, including callback copy construction, assignment, storage exchange, local-pointer repair, temporary destruction, list insertion and final cleanup. The eleven remaining differences are all earlier, in the unrelated inlined list-clear path at offsets `0xAC` through `0x14C`. Thus this is a complete block-level consumer proof, not a claim that registration or its unit is matching.

The complete eight-byte initialization, `0x218`-byte calculation, eight-byte activity-destruction, and `0x78`-byte registration exception records independently retain exact bytes and actual destructor relocation targets. The control and proposed header's existing callback destructor also retain their native instruction stream. No original code bytes, hand-encoded vtable, calling-convention wrapper, register variable, flag adjustment or weak helper replacement is supplied by this proposal.

## Complete current existing-source control

Actual base `5f56ce3a517a5a31940798ed006903f81e112bd7` is captured from the isolated worktree after configure, explicit `all_source`, plain default Ninja and the actual-directory frozen verifier all exit zero. The saved control contains the exact 707 objects declared by Ninja's `all_source`, plus the complete linked ELF, frozen DOL, linker script, build configuration, Ninja file and tracked-tree identity. No donor objects are inserted into the proposal.

With only the bounded callable header changed, configure, explicit `all_source`, plain default Ninja and the actual-directory frozen verifier again exit zero. All 707 objects preserve their complete section metadata, every allocated payload, raw relocation records and normalized symbol attributes. Of those, 705 whole files are identical; `game/CFiber.o` and `game/sound.o` differ only in numeric compiler-local names within `.strtab`. Their nonnumeric names and all other payloads agree.

The complete linked ELF preserves its header, section headers, every non-NOBITS payload and symbol attributes except twenty numeric compiler-local string-table names. The entire DOL is byte-identical to the control and frozen original: SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The reproducible control is `C:/MassStorage/GitHub/317-reference/control-5f56ce3a`; `317-compare-control.py` checks the complete actual declared-object set, rejects any difference except recorded numeric names, and writes `317-functional-control-proof.json`. The verified control ELF SHA-256 is `8a1684725d77d667dd5b901d166ba650fd326b75c82ecddee2827a7b5231c792`. The final official landing loop must repeat the current-head verification after rebasing; this historical base does not waive that gate.

## Refreshed current-main control

An independent fresh baseline on actual main `db91b1168abdb1f040f916e1cb0241c6a0ff3fab`, after the accepted ROM-font source and FA changes, repeats configure, explicit `all_source`, default Ninja and frozen verification with all exits zero. Switching back to the rebased proposal and repeating those commands again exits zero. The exact declared set is now 708 objects. All retain section metadata, allocated bytes, raw relocations and symbol attributes; 706 entire object files agree and only the same two numeric string tables differ. The complete ELF again differs only in twenty numeric local-symbol names, and the whole frozen DOL remains exact. The immutable baseline is `317-reference/control-db91b116`.

The subsequent `b6b29538` BModelObj source change also receives its own clean, detached-main baseline in the same isolated worktree. Its actual `all_source`, default Ninja and frozen verifier pass before capturing `317-reference/control-b6b29538`. This does not reuse a live contributor build. The final proposed-head comparison and official landing results belong in the pull request after that baseline is captured.

## Final 709-object control and canonical consumer

The independent baseline repeats once more on actual main `359c1c27577f8dfc94c82596b65f9640fa56c9ba`, including the accepted resource-font source: all 709 source objects compile, default Ninja links, and the frozen verifier passes. Its immutable output is `317-reference/control-359c1c27`.

When main advances to `4c953d37e60403881564bd47c113df5883145210`, the accepted official build seed `1adacb88c8e0b6d58586c92577c436986541baff` supplies the actual owner-built control. An empty Git tree diff and byte-identical `git ls-tree -r -z` output independently establish complete tracked-tree equivalence. The owner confirms its actual `all_source` completed; the snapshot captures exactly the current Ninja-declared set of 709 source objects and independently rereads every saved hash. The saved DOL also independently passes the frozen verifier. No control object enters the proposal's input graph. `317-capture-accepted-control.py` records this provenance in `317-reference/control-4c953d37`.

The actual proposed configuration runs configure, explicit `all_source`, plain Ninja and the actual-directory frozen verifier, with all exits zero. The complete 709-object comparison proves 707 entire files identical and the same two numeric-only string-table changes. All section metadata, allocated bytes, raw relocations and symbol attributes agree. The whole ELF differs only in twenty numeric local-label spellings; every other payload and all metadata agree. The complete DOL retains the frozen SHA-256.

The native diagnostic now consumes the actual canonical proposed callable header directly, with no scratch callable copy. Its compiler command comes from Ninja's actual `BController.o` producer, retaining its wrapper, compiler and all game flags; the only disclosed extra include path supplies the scratch list extension. Compilation and the strict native instruction/target audit exit zero, rejecting any changed registration word beyond the eleven explicitly unresolved list-clear positions. All four complete exception records again compare exactly. The earlier scratch generator's duplicate insertion after the header became canonical was corrected before this repeat; failed compilation was not accepted as a fresh consumer proof.
