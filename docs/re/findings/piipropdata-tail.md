# PiiPropData long-tail controls

2026-11-04: the initial remaining functions are constructor (95.778625%, 1048 bytes), Reset (96.36191%, 1260 bytes), short-vector sort (96.55556%, 900 native versus 916 rebuilt bytes), and implicit SpeciesMoves assignment (99.55382%, 2044 native versus 2040 rebuilt bytes).

## Short-vector sort

The first partition scan's postfix iterator increment causes CodeWarrior to duplicate its first comparison before the loop, adding 16 bytes. Changing only that increment from `m++` to `++m` restores the complete native 900-byte instruction stream and registers. A source-local explicit specialization first isolated the result; a scratch overlay of `include/algorithm` then confirmed the single generic-header change leaves every other PiiPropData function score unchanged. This is ordinary equivalent C++, not a D-0016 fake. The maintainer then rebuilt all affected consumers, verified the full frozen SHA, and compared previously exact functions by original address against the saved pre-change report: zero regressions. The shared-header fix is retained. PiiPropData itself remains NonMatching because its three copy/assignment functions are unfinished.

Equivalent goto/while forms retained the duplicated comparison. A forced do-while removed it but incorrectly skipped the first comparison and still produced 908 bytes; it was rejected. Boolean double-negation and explicit boolean comparisons also produced exact code, but prefix increment is the simplest equivalent source. Selected optimization pragmas did not complete the function. Scratch reproducers are `tools/local/session/prop-61-sortcasts.py`, `prop-61-header-sort.py`, and `prop-61-compile/header-sort.json`.

## Vector copy and assignment

Constructor and Reset have real copy-path differences before register allocation. The target recomputes a signed short-vector byte span after allocation; the source reuses its cached element count. Signed-short vectors use the unsigned-short storage adapter, so specializing the signed-short deleter does not affect this path. A source-local unsigned-short deleter specialization using a signed integer byte difference divided and multiplied by two restores the native instruction count, reaching 98.71756% and 98.80635%; empty-tag stack slots and registers still differ. This candidate is not retained.

The implicit SpeciesMoves assignment lacks a POD-tag byte store in the short-vector capacity-recommendation path. Adding the tag argument restores 2044 bytes, but shifts tag slots and registers. Direct recommendation bodies and zero through four inline wrappers give 99.127205% through 99.24462%; they are not retained. Explicit assignment loops and default special-member variations also failed. No shared-vector change was made. A later unit-local unsigned-short recommendation specialization with an initialized empty tag and separate count value reaches 99.97847% at native size: all instruction opcodes and registers agree, but three four-byte temporary slots and two tag slots are permuted. Reference-qualified counts, reordered tag creation, count-use masks, and scalar casts do not complete it. This remains scratch-only (`prop-61-countuse.py`, mask 6); filenames containing `assignment100` are intermediate probe labels, not acceptance.

## Traps

- Prefix versus postfix increment of a class iterator can change loop rotation even when the discarded postfix result should be trivial. Verify the exact native branch path and full function; replacing a while loop with do-while is not equivalent.
- Signed-short vectors in this reconstruction use unsigned-short storage. Probe the actual instantiated storage backend when a specialization appears to have no effect.
