# CDlgPiiSelector final helper tail

## Accepted function gains, 2026-11-04

The coordinator verified 643/645 real functions after the following two source additions and the scoped symbol correction. The shared fallback build completed successfully and full DOL SHA verification passed. CDlgPiiSelector remains NonMatching: this is function acceptance, not a source-link acceptance.

### Number conversion emission

The existing `Metrowerks::number<unsigned long, 1>` conversion operator compiled to the exact eight-byte target once an external, unused `GetSelectorAllocationCount` caller requested its emission. An unused static caller, explicit template instantiations, and a conversion specialization did not emit it. Taking the conversion address emitted it but disturbed data offsets and regressed Setup/CreateBall. The retained caller has no intended runtime role and is a D-0016 emission fake. Its removal by the final link must still be verified when this unit becomes Matching.

### Duplicate minimum

The original unit contains two identical six-instruction integer-minimum bodies. The reconstructed template emits only one. The existing global template at 0x80095280 remains unchanged. The local 24-byte body at 0x8009CEA0 is represented by `SelectorMinimum`, a force-active static C++ function returning `b < a ? b : a`. The coordinator renamed only that local target symbol to `SelectorMinimum__FRCiRCi`. The clone matches all six instructions and preserves the second original body. This is a D-0016 duplicate-function fake, not a recovered original name.

## Remaining assign and insert helpers

The list-iterator vector assign wrapper matches all instructions except the stack locations of its empty int2type tag. The target loads the original tag at stack +8 and copies it to +12; source reverses these two slots. The insert wrapper also needs that layout and retains the tag-copy address in a saved register before converting its position iterator. The source computes the address after conversion instead.

Selective inline helpers, explicit dispatch specializations, dispatch definitions delayed to the end of the unit, always_inline/noinline attributes and pragmas, and wider tag storage did not produce an accepted improvement. Changing the unit's final dont_inline state affects hundreds of emitted templates and is not a local fix. Wider tag storage shifts iterator stack slots as well as the tag. No such candidates were retained.

## Traps

- A missing conversion operator can require a live external emission request even when explicit template instantiation is present. An address-taken data object can change unrelated data references; compare all functions, not only the missing helper.
- A local duplicate template body requires explicit representation and a scoped symbol identity. Keep the existing global body and document the clone; function comparison alone does not prove the unit links at the original positions.
