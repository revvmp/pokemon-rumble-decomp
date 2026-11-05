# PiiPropData allocation-tag continuation

## Scope and strongest comparison

This 2026-11-04 continuation follows [the property-data reproduction](vm-properties-tail-20261104.md) and [the earlier PiiPropData investigation](piipropdata-tail.md). It uses isolated native compiler outputs and the frozen original object in `tools/local/tail-20261104/vmprop-targets`. The coordinator owns shared builds and verification. No PiiPropData source or shared header change from this continuation is retained yet.

| Function | Native bytes | Previous exact-opcode and exact-slot candidate | New candidate |
| --- | ---: | ---: | ---: |
| PiiPropData constructor | 1048 | 99.0458% | 99.541985% |
| PiiPropData::Reset | 1260 | 99.46032% | 99.61905% |

The new candidate preserves every native opcode and stack offset, as well as every previously exact real function in the unit. Only the shared inner copy-count, length-error string and length-error object register cycle remains. In the constructor, the target uses r20/r22/r21 for count/string/object, while this candidate uses r22/r21/r20. Reset has the same cycle one register lower. Branch destinations must be compared relative to each function because candidate weak-function emission changes whole-section addresses.

The complete candidate is `tools/local/tail-20261104/game_PiiPropData/jpeg_pii_emptytag_0`, with its source, object, compiler log and full-unit `diff.json`. Its private `vector` overlay is `tools/local/tail-20261104/jpeg-pii-emptytag/0/vector`. `jpeg_pii_emptytag.py` reproduces the experiment. These are isolated comparisons, not source-link acceptance evidence.

## Caller-owned allocation tag

The older `vmprop-rebuild-0-0-1` constructor chain already had the correct outer registers: this in r28, remaining count in r23 and zero in r24. Its allocation tag occupied stack offset 0x30 instead of 0x8, with the other byte slots displaced by four bytes. The previous strongest exact-slot source specialized the outer vector constructor and wrapped element construction; that corrected the slots but changed the outer register priorities.

The new candidate preserves the vector-to-backend constructor chain. A source-local `PiiPropertyAllocationTag` derives from `Metrowerks::int2type<false>` and has an empty user-provided default constructor. The vector initializer creates this tag and passes it by const reference to a private backend-constructor overload. That backend initializes the tag's representation byte immediately before allocation. This moves storage ownership to the caller while keeping the store at the original execution point. It reproduces both the outer register priorities and every byte-tag slot.

The empty default constructor is material. Returning an uninitialized ordinary int2type value through a helper gives Reset the stronger register-only state, but the PiiPropData constructor retains an extra byte load/store pair and grows from 1048 to 1056 bytes. A direct temporary of the custom tag avoids that copy. Passing the custom tag by value without a user-provided copy constructor also retains the unwanted copy. The const-reference form is the smallest successful variant.

This is a D-0016 candidate, not recovered original source. A production implementation must isolate the backend overload from other translation units, retain all complete-unit comparisons, and pass the normal source link and frozen verifier before acceptance.

## Rejected controls

The first continuation reproduced the prior 99.0458%/99.46032% state. Enum copy-count outputs, allocation uses retaining the enum type, scalar and aggregate counter views, receiver address enums, full-width outer bitfields, counter operator forms, empty destructors, loop-finish helpers, and integer/unsigned union views did not complete it. Returned counter or receiver aggregates commonly introduced stack storage. Repeating pointer differences changed virtual-register priorities but did not produce the target allocation. Reusing the stored capacity as the outer count removed a required separate immediate load. Whole-vector return wrappers retained real copy operations.

Wii 1.0, 1.3 and 1.5 emit the same two residual comparisons as Wii 1.1 for the reproduced strongest source. Outer helper depths 1, 3 and 5 are identical; even depths change stack layout. Removing explicit inline from the selected copy and outer-constructor specializations does not change their residuals.

A private derived-vector wrapper reproduced the already known correct outer registers, but added a destructor null check and did not solve the tag permutation. It is discarded. Ordinary named tags, returned tags, scalar or aggregate backing storage, volatile byte locals, allocation in a new base constructor, and caller-owned tags passed through a free initialization helper did not combine the correct registers and slots. Reversing allocation and construction lexically with labels preserved unwanted branches and is rejected.

After the stronger tag candidate, hoisting copy counts into vector or signed-short adapter constructors through output references moved each count below the error temporaries but promoted the copy receiver to the wrong register. Opaque receiver variables and mutable reference parameters did not undo that promotion. Copy-count argument objects, mutable members, enum members, full-width bitfields, caller-owned capacity stores, and later count declarations did not complete the remaining cycle. No failed variant supersedes `jpeg_pii_emptytag_0`.

## Traps

- A higher comparison can still include extra instructions. The uninitialized ordinary-tag return reaches 99.61905% for Reset while adding a byte copy to the constructor; inspect both functions and native sizes independently.
- The earlier wrong-slot source already had the correct outer registers. Compare older candidates before treating a wrapper's improved percentage as a new register mechanism.
- A copied length-error helper is sensitive to explicit inline. In the controlled `jpeg_pii_errortemplate_0`/`1` pair, a template helper without explicit inline preserves the native operation stream; adding inline gives its literal a function-specific string symbol and changes cached-pointer use and register pressure. Both are automatically inlined. This is a measured local behavior, not a general rule for all helpers.
