# AnimCurveExecuteF32 final scheduling pair, 2026-11-04

## Scope and status

This investigation completed the final two-instruction difference in `AnimCurveExecuteF32__Q24nw4r2efFPUcPfUlUsUl`. The main function reconstruction, floating-point allocation, timing controls, and production acceptance are recorded in [the animation-curve continuation](animcurve-20261104-continuation.md). This note records the independent final scheduling investigation and its source-only control.

The accepted private candidate is `tools/local/tail-20261104/smallgame/AnimCurveF32/f32_pair_coupled_outputs_8/ef_animcurve.cpp`. Its native function has 4,044 bytes, all identical to the frozen original, including branch displacements. Its function SHA-256 is `993b77ea5821e579fffb48ad124b703be6db3f105e08617d07e7f7006238901d`.

The canonical file owner integrated the narrow control into the cleaned F32 reconstruction. The coordinator then ran the normal configure and ninja workflow successfully and verified `main.dol` against its frozen manifest with PASS. All previously exact animation-curve functions remained exact. The complete unit remains `NonMatching` only for `AnimCurveExecuteU8`; this acceptance does not claim that the remaining original-object fallback has been removed. The full production evidence is recorded in [the continuation findings](animcurve-20261104-continuation.md).

## Final difference and source control

The minimized input already matched every register and all other instructions. At offsets `+0x724` and `+0x728`, the target adjusts the second value pointer with `addi r9,r9,12`, then normalizes its cached flag with `srwi r11,r11,31`. The candidate emitted these two instructions in reverse order. Its objdiff score was 99.80218%.

The accepted control is a small inline helper that writes the normalized flag through a reference and returns its pointer argument unchanged:

```cpp
static inline const f32* PrepareFloatPair(const f32* pValues, u8 flag, BOOL& rFlag) {
    rFlag = flag != 0;
    return pValues;
}
```

At the existing second flag conversion, the caller declares `BOOL randB` and assigns `pValuesB = PrepareFloatPair(pValuesB, flagsB, randB)`. The first flag conversion, cached flag loads, pointer construction, and all later uses remain unchanged. This adds no emitted helper, instruction, data, or relocation. It is a D-0016 control: returning an unchanged pointer alongside the flag output is retained for the demonstrated compiler effect, without a claim that it reconstructs the original source structure.

## Read-only compiler evidence

The configured Wii/1.1 compiler was traced with the existing register-allocation hook and a read-only scheduling hook. The private scripts are `f32_pair_trace.py` and `animcurve_final_schedule_hook.js` under `tools/local/tail-20261104/`. No compiler memory was changed.

Both disputed instructions are in basic block 93. Before the final scheduling pass, the original candidate places the rotate/mask operation ahead of the pointer adjustment. At final cycle 32, both instructions have the same latest deadline of 26, depth of 1, no pending predecessors, and no unlocked successors. Both are eligible. The scheduler's tie retains the first ready-list node, which is the flag operation in that candidate. There is no direct false memory dependency between the two instructions.

The accepted helper creates an extra virtual-register copy before allocation: `MR r110,r66`. The pre-allocation schedule places the pointer adjustment at cycle 31 and this copy at cycle 33, after the flag's `OR` operation. In the code before final scheduling, the copy has disappeared and the pointer adjustment now occupies the later position between `OR` and `RLWINM`. The final ready list consequently presents `ADDI` first. The hook records `ADDI` selected at cycle 32, then `RLWINM` and the branch at cycle 33. Every final machine word matches.

This establishes a surviving virtual-copy mechanism rather than an arithmetic or memory-order correction. The trace demonstrates the before/after compiler states; attribution of the copy's disappearance specifically to coalescing is consistent with those states, without requiring a hook that mutates or replaces the optimizer.

## Acceptance evidence

The complete private objdiff JSON and native object are stored beside `acceptance.json` in the accepted candidate directory. `tools/local/tail-20261104/f32_pair_accept.py` compares raw ELF contents and normalizes relocations by offset, type, addend, call target, and retained literal storage and contents.

| Check | Result |
| --- | --- |
| Native compiler | Exit 0 |
| F32 function comparison | 100%; 4,044 identical bytes |
| F32 relocations | All 59 exact normalized records, including literal contents |
| Other emitted function bodies | All nine unchanged from the private control |
| Other function relocations | All unchanged |
| Retained `.data` | Original 512 bytes, identical |
| Retained `.sdata2` | Original 56 bytes, identical |
| New emitted functions | None |
| Object section extents | Unchanged |
| Changed `.text` bytes | Eight, confined to the two corrected words |
| Other changed object section | Twenty generated-name bytes in `.strtab`; same size |

The private candidate source SHA-256 is `80ffb25970671dd566ef95e5fc05b5f737d4e158e202b1fbd88abfa1c6dc3f67`. Its control source SHA-256 is `2b10e9365d969da8bde3c2ea9e9c6a66c1c6bc20a7e773a570b571bca3f24ab5`. The control is the canonical owner's `animcurve_f32_minimize_timing_3` packet, so the acceptance concerns only the final helper change; other in-progress functions in that private packet are not claimed as reconstructed by this investigation.

## Negative controls

- Replacing the second value pointer with byte or void pointer storage and matching casts and increments preserves the same final pair. A same-type pointer reassignment, immediately after construction or after either flag conversion, changes many registers and earlier scheduling groups.
- Simple boolean wrappers, enum views, integer/pointer identities, and same-type boolean reassignments preserve the pair. Explicit unsigned negation, OR, and shift forms also preserve the pair and can reverse one commutative operand order.
- One through four inline getter levels for the pointer alter earlier setup instructions but do not solve the pair. Pointer wrappers and union views similarly disturb earlier setup or allocation.
- A helper that accepts the pointer by reference and only writes the flag preserves the pair. Returning the pointer while writing the flag is the effective distinction. Adding an explicit pointer reassignment inside that successful helper regresses the result.
- A returned pointer-and-flag aggregate with an explicit fieldwise copy constructor also reaches 100%, but the single returned pointer plus flag reference is the narrower accepted control. Trivial aggregate copies retain extra instructions.

## Traps

A final adjacent instruction swap can be caused by a virtual copy that disappears before final scheduling. When both ready nodes tie on the scheduler's actual priority fields, changing boolean syntax or aliasing assumptions may leave the order untouched. Inspect the pre-allocation copy and the final ready-list order. Here, a source-only inline return and output reference created the necessary temporary copy, while producing no extra final code.
