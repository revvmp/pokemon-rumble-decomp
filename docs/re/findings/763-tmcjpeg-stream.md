# F-763-1: TMC JPEG bit-stream register allocation

The complete ten-function stream unit is already split at text 0x801F9134–0x801F9764 and rodata 0x80368118–0x80368121. It remains NonMatching while the checked writer differs. Established Wii/1.0 cflags_tmcjpeg are unchanged. The existing source was reconstructed for this binary; no additional external code is imported. See F348/F349 for the prior unit and type evidence.

A fresh baseline configure, plain default Ninja (505 targets) and explicit frozen verification each exit zero. The source tree is clean before reconstruction edits. The original baseline reproduces six differing instruction words in PutBitsFast and twenty-six in TMCJPEGStreamPutBits when only relocation operand bits are excluded. Whole relocation instructions must not be ignored, because their register operands are part of this mismatch.

An ordinary static inline MaskBits helper returning the selected low bits reproduces all of PutBitsFast's opcode/register words while preserving the eight previously exact functions. It is used consistently for the loop and final partial-byte masking. No compiler flags, volatile storage, register binding or assembly is introduced. Checked TMCJPEGStreamPutBits retains the prior twenty-six differences; this is a diagnostic improvement, not complete source-selected acceptance.

## Traps

Changing the byte-writing helper's scalar parameter type, swapping the loop mask/shift operands, adding const to the overflow pointer or declaring the byte register does not change the baseline six/twenty-six differences. Moving the loop's count decrement earlier regresses register allocation and sometimes code size; exchanging the byte-zero/free-eight tail assignments also regresses. The MaskBits helper resolves only the fast routine so far.

The actual Ninja source-object target also compiles this candidate successfully. objdiff reports all nine retained routines including PutBitsFast at 100%, the checked writer at 95.68519%, and both rodata objects at 100% (unit text 98.823235%). Plain default Ninja and explicit frozen verification remain zero with the unit still NonMatching and its original fallback selected.
