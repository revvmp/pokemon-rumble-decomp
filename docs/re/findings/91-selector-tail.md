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

## Explicit tag references, 2026-11-04

A fresh comparison found a third residual in Clamp: it still referenced the global integer minimum after the duplicate local body had been named SelectorMinimum. An explicit Clamp<int> specialization now binds the call to the retained local clone.

The two vector range-dispatch templates now take their unused int2type tag by const reference only under GAME_DLGPIISELECTOR_RANGE_TAG_REFERENCE. Their generated 68-byte bodies remain identical. The ABI already passed the empty by-value tag indirectly, so the changed source signature preserves its physical argument register. The scoped target names at 0x800929F0 and 0x80091A40 must use the const-reference mangling. This is a documented D-0016 interface reconstruction, not evidence of original source spelling.

The assign specialization declares its copy tag before the source tag and explicitly copies one object-representation byte through volatile unsigned-character lvalues. This reproduces the target's stack +8 to +12 copy. The insert specialization uses the address of that assignment expression as the dispatch argument, converts it back to the original tag pointer, and dereferences it. The address remains the address of the same tag object; there is no hard-coded frame address or instruction stream. Retaining the assignment lvalue forces the compiler to preserve the tag pointer in r31 across iterator conversion, exactly as the target does. Computing the byte copy and pointer separately rematerializes the pointer too late and loses one saved register.

Direct compilation of the retained source and normal headers matches all 645 real functions after the two diagnostic signature renames. Scratch evidence is tools/local/session/282-selector/tracked-final-diff.json. This records function acceptance evidence only; the coordinator's full source-link trial is pending.

Negative controls included four one-byte struct/union tag types, explicit copy constructors, reference forwarding helpers, pointer/integer casts, named temporaries, placement new, and reference-bound temporary tags. A reference-bound temporary restored the required r31 lifetime but reversed the two tag slots. Placement new emitted an unwanted call. The successful assignment-lvalue expression avoids both problems. The volatile byte copies and unusual argument expression are D-0016 fakes after the documented natural-source attempts; no original bytes or assembly enter the source pipeline.

### Additional trap

An expression that yields the same pointer can have a different register lifetime: a named tag address was rematerialized after a call, while the lvalue result of its byte assignment survived across that call. Compare the entire unit and the link, rather than assuming equivalent C++ expressions share allocation behavior.
