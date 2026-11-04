# F-45-1: BWorld long-tail reconstruction

The 2026-11-04 baseline has 191/196 real target functions exact. Direct Wii/1.1 compilation with the configured flags and original split comparison now gives 194/196, with no previously exact function regressing. BWorld remains NonMatching pending full source-link acceptance.

## Retained D-0016 fake matches

- `std::vector<BWorld::ObjEntry>(size_type)` at `0x8005B950`: a TU-local `__vec_constructor` specialization forwards through `InitializeWorldEntries`, then separate `AllocateWorldEntries` and `ConstructWorldEntries` helpers. The combined extra inline levels reproduce the empty-tag slots, exception object stack slots, and element pointer register. A single wrapper, or separate allocation/construction wrappers without their enclosing wrapper, did not match.
- `BWorld::AddObject` at `0x8005F230`: a pointer-to-u32-to-pointer conversion makes the entry-array base survive as a named register-allocation value; declaring the entry pointer before the base pointer gives the target register order. The target is 32-bit, so the round trip preserves the pointer value.
- `BWorld::RemoveObject` at `0x8005F450`: calculate next before previous, with the same pointer round trip on next; keep the ground-identity comparison in a named bool. This reproduces evaluation order and both register allocation choices without altering behavior.

The AddObject/RemoveObject conversion shapes are allocation fakes; no original source form is claimed. No shared header or generic STL change is retained. Direct evidence is under ignored `tools/local/session/45-world/combined3.json`.

## Remaining Calc and CalcMtx

Both inline GetSkipStates with one missing redundant zero load: target initializes a temporary with zero inside the first flag branch, then ORs bit 2 into the output. A redundant `states = 0` in that branch restores the instruction count but uses the output register for both instructions (CalcMtx 99.94%). Scalar casts, integer widths, helper wrappers, local state structs, full-width bitfields, repeated conditions, and caller optimization pragmas have not closed this. Calc additionally differs in one later loop iterator register. No incomplete probes were retained.

## Traps

- A pointer round trip can change which named local survives optimization despite producing no instructions. Once it survives, declaration order controls relative allocation; adding the cast alone swapped the wrong two AddObject registers.
- Merely assigning a comparison to a named bool can move the result from an inline temporary to the target register. Widening the same result to int/u32 did not work in RemoveObject.
- Stack parity fixes require examining every frame object, not just the empty tags: BWorld vector(size_type) needed an outer forwarding helper and separate inner allocation/construction helpers together.
