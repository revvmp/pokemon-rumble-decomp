# PiiPropData long-tail controls

2026-11-04: the initial remaining functions are constructor (95.778625%, 1048 bytes), Reset (96.36191%, 1260 bytes), short-vector sort (96.55556%, 900 native versus 916 rebuilt bytes), and implicit SpeciesMoves assignment (99.55382%, 2044 native versus 2040 rebuilt bytes).

## Short-vector sort

The first partition scan's postfix iterator increment causes CodeWarrior to duplicate its first comparison before the loop, adding 16 bytes. Changing only that increment from `m++` to `++m` restores the complete native 900-byte instruction stream and registers. A source-local explicit specialization first isolated the result; a scratch overlay of `include/algorithm` then confirmed the single generic-header change leaves every other PiiPropData function score unchanged. This is ordinary equivalent C++, not a D-0016 fake. The maintainer then rebuilt all affected consumers, verified the full frozen SHA, and compared previously exact functions by original address against the saved pre-change report: zero regressions. The shared-header fix is retained. PiiPropData itself remains NonMatching because its three copy/assignment functions are unfinished.

Equivalent goto/while forms retained the duplicated comparison. A forced do-while removed it but incorrectly skipped the first comparison and still produced 908 bytes; it was rejected. Boolean double-negation and explicit boolean comparisons also produced exact code, but prefix increment is the simplest equivalent source. Selected optimization pragmas did not complete the function. Scratch reproducers are `tools/local/session/prop-61-sortcasts.py`, `prop-61-header-sort.py`, and `prop-61-compile/header-sort.json`.

## Vector copy and assignment

Constructor and Reset have real copy-path differences before register allocation. The target recomputes a signed short-vector byte span after allocation; the source reuses its cached element count. Signed-short vectors use the unsigned-short storage adapter, so specializing the signed-short deleter does not affect this path. A source-local unsigned-short deleter specialization using a signed integer byte difference divided and multiplied by two restores the native instruction count, reaching 98.71756% and 98.80635%; empty-tag stack slots and registers still differ. This candidate is not retained.

The implicit SpeciesMoves assignment lacks a POD-tag byte store in the short-vector capacity-recommendation path. Adding the tag argument restores 2044 bytes, but shifts tag slots and registers. Direct recommendation bodies and zero through four inline wrappers give 99.127205% through 99.24462%; they are not retained. Explicit assignment loops and default special-member variations also failed. No shared-vector change was made. A later unit-local unsigned-short recommendation specialization with an initialized empty tag and separate count value reaches 99.97847% at native size: all instruction opcodes and registers agree, but three four-byte temporary slots and two tag slots are permuted. Reference-qualified counts, reordered tag creation, count-use masks, and scalar casts do not complete it. This remains scratch-only (`prop-61-countuse.py`, mask 6); filenames containing `assignment100` are intermediate probe labels, not acceptance.

## Assignment stack and allocation isolation

Further grouping controls narrow the assignment to two equivalent diagnostic states. The `throw-choice-assignment100-2-1.cpp` source plus `storage-choice-assignment100-2-1-0.h` scratch vector overlay gives 99.9863%: the instruction sequence and all registers agree, but the count and first max-expression four-byte slots are swapped. The `recthrow0.cpp` source with that same overlay gives the native 2044-byte sequence and every stack offset, with only ten r25/r26 operands swapped (99.892365%). Neither is retained.

The stack corrections require grouping clear/deallocate/allocate in one inline helper, adding one inline level to its length-error throw, moving the capacity core one inline level, and grouping the first max expression in a helper taking the count by const reference. This isolates actual nested inline ordering effects rather than declaration spelling. Sixty-four named/value-initialized/copy-tag combinations and twenty-five empty-tag return-helper combinations did not fix the original tag pair. Result locals, output references, return wrappers, arithmetic identities, parameter permutations, pointer/integer round trips, and template helper controls did not resolve the final register state.

The register trace for the all-slots-correct variant (`ra.txt`) assigns the capacity result virtual r65, degree 37, after the first throw locals; it takes r25. The correct-register variant (`ra-rightregs.txt`) creates it as virtual r74, degree 37, before those locals, and it takes r26. This is creation-order interference, not a remaining instruction mismatch. A pointer-to-member initializer can force implicit assignment generation under a scoped optimization pragma; `opt_dead_assignments off` then takes effect but lowers comparison to 84.2818% through extra instructions. A pragma merely around the struct declaration had no effect.

These source-local unsigned-short specializations also disturb previously exact short-vector helpers. Even an exact assignment would need those consumers restored or a narrower instantiation scope before adoption. At this diagnostic checkpoint main retained only the accepted prefix-increment change. The subsequent completed assignment correction is recorded below.

## Completed assignment correction

The final source-local variant reproduces all 2,044 bytes of the implicit SpeciesMoves assignment. The key remaining stack fix is to replace both max-expression temporaries with three separate named locals declared in this order: first growth amount, second growth amount, requested count. Declaring count first had placed its four-byte slot below the expression slots; moving it last restores the exact offsets without adding the extra inline level that changed the capacity result's virtual-register priority.

The retained PiiPropData.cpp specializations copy the existing unsigned-short storage recommendation and pointer-range assignment algorithms. An explicit initialized empty tag accounts for the missing native byte store; grouping clear/deallocation/allocation and the allocation error throw preserves the observed inline-depth slot order. A separate helper used only by the specialized assignment keeps the generic allocator unchanged. This last isolation restores the previously exact short-vector reserve function, which a global allocator specialization had moved to 99.681816%.

After formatting and removing an unnecessary max helper, `retained-assign.json` reports the assignment at 100%, with zero losses to every previously exact function. Only the pre-existing constructor (95.778625%) and Reset (96.36191%) remain nonmatching. This is D-0016 source-match progress; the unit remains NonMatching and has not been accepted as source-linked. No shared header, symbol, or build-selector edit is part of this correction. The specialization retains the original algorithms and allocation/error behavior; the extra empty tag, local ordering and helper grouping are documented compiler-shaping fakes.

Ignored evidence is `tools/local/session/prop-61-compile/retained-assign.cpp` and `retained-assign.json`. The discriminating sweep is `prop-61-namedtemps.py`; source-local isolation is `prop-61-isolatealloc.py`.

## Traps

- Prefix versus postfix increment of a class iterator can change loop rotation even when the discarded postfix result should be trivial. Verify the exact native branch path and full function; replacing a while loop with do-while is not equivalent.
- Signed-short vectors in this reconstruction use unsigned-short storage. Probe the actual instantiated storage backend when a specialization appears to have no effect.
