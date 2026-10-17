# F-763-1: TMC JPEG bit-stream register allocation

The complete ten-function stream unit is already split at text 0x801F9134–0x801F9764 and rodata 0x80368118–0x80368121. It remains NonMatching while the checked writer differs. Established Wii/1.0 cflags_tmcjpeg are unchanged. The existing source was reconstructed for this binary; no additional external code is imported. See F348/F349 for the prior unit and type evidence.

A fresh baseline configure, plain default Ninja (505 targets) and explicit frozen verification each exit zero. The source tree is clean before reconstruction edits. The original baseline reproduces six differing instruction words in PutBitsFast and twenty-six in TMCJPEGStreamPutBits when only relocation operand bits are excluded. Whole relocation instructions must not be ignored, because their register operands are part of this mismatch.

An ordinary static inline MaskBits helper returning the selected low bits reproduces all of PutBitsFast's opcode/register words while preserving the eight previously exact functions. It is used consistently for the loop and final partial-byte masking. No compiler flags, volatile storage, register binding or assembly is introduced. Checked TMCJPEGStreamPutBits retains the prior twenty-six differences; this is a diagnostic improvement, not complete source-selected acceptance.

## Traps

Changing the byte-writing helper's scalar parameter type, swapping the loop mask/shift operands, adding const to the overflow pointer or declaring the byte register does not change the baseline six/twenty-six differences. Moving the loop's count decrement earlier regresses register allocation and sometimes code size; exchanging the byte-zero/free-eight tail assignments also regresses. The MaskBits helper resolves only the fast routine so far.

The actual Ninja source-object target also compiles this candidate successfully. objdiff reports all nine retained routines including PutBitsFast at 100%, the checked writer at 95.68519%, and both rodata objects at 100% (unit text 98.823235%). Plain default Ninja and explicit frozen verification remain zero with the unit still NonMatching and its original fallback selected.


### Further bounded diagnostics

The actual Ninja source object reproduces every frozen native word in nine functions after applying all 22 relocation sites, and the complete nine-byte rodata table is exact. The checked writer alone retains twenty-six differing words. These diagnostics neither change the NonMatching status nor select its source at link time.

All nine pinned Wii compiler variants (0x4201_127, 1.0, 1.0a, 1.0RC1, 1.1, 1.3, 1.5, 1.6 and 1.7) preserve the same nine exact routines and the same twenty-six checked-writer differences for this source form. Compiler-family substitution therefore does not resolve the present register graph. Diagnostic O4,s/O3,p/O3,s/O2,p variants regress other previously matching routines and often change code sizes; the established O4,p flags remain unchanged.

Permuting the overflow-case stores changes otherwise exact byte writers, while changing inline scalar typedef spellings or expanding equivalent flushed assignments does not improve the checked routine. Extracting the loop operation into a second helper adds no improvement; packing the byte OR into a helper regresses the already exact fast writer. The accepted low-bit helper is retained without adding speculative wrappers or flags.


A further bounded helper-input check varied MaskBits input types without changing the actual source. Signed32-bit bits with unsigned32-bit count preserves nine exact functions and the same26 checked-writer differences. Narrow8/16-bit bit or count parameters regress one previously exact function and leave103/104 checked-writer differences. These ordinary type variants do not recover the checked-writer register allocation; the committed helper keeps its original full-width types, and the unit remains NonMatching.


Three further ordinary WriteByte pointer-expression variants were diagnostic only: assigning the postincrement destination first, indexed store plus increment, and a cached pointer reduced the exact count to five, five and seven respectively. The original single `*stream->current++ = value` expression remains necessary. A diagnostic `-inline deferred` compilation retains nine exact functions and the same26 checked-writer differences, so there is no evidence for changing the established `-inline auto` profile. Actual source and configuration remain unchanged.
