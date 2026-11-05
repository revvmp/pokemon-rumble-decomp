# PiiPropData allocation-tag continuation

## Scope and strongest comparison

This 2026-11-04 continuation follows [the property-data reproduction](vm-properties-tail-20261104.md) and [the earlier PiiPropData investigation](piipropdata-tail.md). It uses isolated native compiler outputs and the frozen original object in `tools/local/tail-20261104/vmprop-targets`. The coordinator owns shared builds and verification. No PiiPropData source or shared header change from this continuation is retained yet.

| Function | Native bytes | Previous exact-opcode and exact-slot candidate | New candidate |
| --- | ---: | ---: | ---: |
| PiiPropData constructor | 1048 | 99.0458% | 99.73283% |
| PiiPropData::Reset | 1260 | 99.46032% | 99.77778% |

The new candidate preserves every native opcode and stack offset, as well as every previously exact real function in the unit. The error-string registers now agree. Only the inner copy-count and length-error object registers remain swapped: the constructor expects r20/r21 and obtains r21/r20; Reset expects r19/r20 and obtains r20/r19. Both the LevelMove and short-vector copies have this swap. A separate normalized comparison confirms 262/262 constructor instructions and 315/315 Reset instructions with no non-register instruction differences, including branch destinations relative to each function. Candidate weak-function emission changes whole-section addresses, so raw branch-address strings are not a valid comparison.

The complete strongest candidate is `tools/local/tail-20261104/game_PiiPropData/jpeg_pii_erroroutput_0`, with its source, object, compiler log and full-unit `diff.json`. Its private `vector` overlay is `tools/local/tail-20261104/jpeg-pii-currenttag/vector`. `jpeg_pii_erroroutput.py` reproduces the latest improvement. The overlay adds only a generic allocation-tag constructor declaration; the source-local specialization preserves the proven allocation-tag control. The earlier tag-only candidate is `jpeg_pii_emptytag_0`, reproduced by `jpeg_pii_emptytag.py`. These are isolated comparisons, not source-link acceptance evidence.

## Caller-owned allocation tag

The older `vmprop-rebuild-0-0-1` constructor chain already had the correct outer registers: this in r28, remaining count in r23 and zero in r24. Its allocation tag occupied stack offset 0x30 instead of 0x8, with the other byte slots displaced by four bytes. The previous strongest exact-slot source specialized the outer vector constructor and wrapped element construction; that corrected the slots but changed the outer register priorities.

The new candidate preserves the vector-to-backend constructor chain. A source-local `PiiPropertyAllocationTag` derives from `Metrowerks::int2type<false>` and has an empty user-provided default constructor. The vector initializer creates this tag and passes it by const reference to a private backend-constructor overload. That backend initializes the tag's representation byte immediately before allocation. This moves storage ownership to the caller while keeping the store at the original execution point. It reproduces both the outer register priorities and every byte-tag slot.

The empty default constructor is material. Returning an uninitialized ordinary int2type value through a helper gives Reset the stronger register-only state, but the PiiPropData constructor retains an extra byte load/store pair and grows from 1048 to 1056 bytes. A direct temporary of the custom tag avoids that copy. Passing the custom tag by value without a user-provided copy constructor also retains the unwanted copy. The const-reference form is the smallest successful variant.

This is a D-0016 candidate, not recovered original source. A production implementation must isolate the backend overload from other translation units, retain all complete-unit comparisons, and pass the normal source link and frozen verifier before acceptance.

## Caller-owned error message

The source-local template `ThrowPropertyCopyError` accepts a mutable `const char*&` output, assigns the original `vector length error` literal and immediately throws `length_error(message)`. The copy constructor declares the message pointer immediately before its count local, then passes the reference through the existing allocation operations. The pointer retains that caller declaration's identity without an additional load, store or copy. Declaring it after the count returns to the previous three-register cycle. A plain named pointer inside the throw helper is also unchanged.

The fresh `jpeg-pii-erroroutputtrace.txt` shows the constructor's short-copy message as virtual register 71, colored r22; count is virtual 70, colored r21; the error object is virtual 64, colored r20. Before this change, the count was virtual 71/r22, message virtual 69/r21 and object virtual 64/r20. The complete graph and instruction streams are retained with the private candidate. All other 48 real functions still compare at 100%.

A same-type reassignment of that message is a different control: it creates an earlier optimizer temporary, at working degree 28 in simplify pass 2 with K=28. It therefore survives until the next pass and takes r24, displacing the outer zero and remaining-count registers. This regresses both functions to 99.06489%/99.09524%. Integer-pointer round trips produce the same regression. Reassigning the pointer only inside the existing `__error_string` constructor is neutral.

## Allocator graph evidence

`jpeg_pii_graphtrace.py` uses the configured native compiler and a frozen private original. `jpeg_pii_simplify_hook.js` reads the graph at simplify entry and simulates changes to JavaScript copies; it never changes compiler state. Its constants derive from the validated sort trace, with the actual register count obtained from `availableRegisters(4)`. `jpeg_pii_graphsim.py` extracts the first constructor graph and repeats the simulation offline.

In the tag-only candidate, the short-copy object, message and count visit simplify pass 1 at degrees 37, 36 and 42. All wait for pass 2, where their visit degrees are 25, 24 and 23 respectively. Their creation order, rather than a single missing edge at the first pass, determines the residual cycle. A later repeated pointer-difference expression moves the count into an optimizer temporary at degree 28 in pass 2, but pushing it earlier also changes adjacent pointer nodes' degrees. A one-edge threshold change cannot be assumed to leave those neighbors' colors unchanged.

The alternative `jpeg_pii_hoistmessage_1` keeps exact native operations and slots and obtains the correct count and error-object registers. It declares both count and message in the higher vector-copy or signed-short adapter constructor and forwards them by output reference through a free initialization helper. Its remaining swap is instead message versus copy destination: constructor target r22/r19 becomes r19/r22, and Reset target r21/r18 becomes r18/r21. The fresh `jpeg-pii-hoistmessagetrace.txt` identifies the promoted copy receiver as virtual 74, which coalesces with the actual destination. This alternative scores 99.561066%/99.63492% and is retained only for diagnosis.

## Rejected controls

The first continuation reproduced the prior 99.0458%/99.46032% state. Enum copy-count outputs, allocation uses retaining the enum type, scalar and aggregate counter views, receiver address enums, full-width outer bitfields, counter operator forms, empty destructors, loop-finish helpers, and integer/unsigned union views did not complete it. Returned counter or receiver aggregates commonly introduced stack storage. Repeating pointer differences changed virtual-register priorities but did not produce the target allocation. Reusing the stored capacity as the outer count removed a required separate immediate load. Whole-vector return wrappers retained real copy operations.

Wii 1.0, 1.3 and 1.5 emit the same two residual comparisons as Wii 1.1 for the reproduced strongest source. Outer helper depths 1, 3 and 5 are identical; even depths change stack layout. Removing explicit inline from the selected copy and outer-constructor specializations does not change their residuals.

A private derived-vector wrapper reproduced the already known correct outer registers, but added a destructor null check and did not solve the tag permutation. It is discarded. Ordinary named tags, returned tags, scalar or aggregate backing storage, volatile byte locals, allocation in a new base constructor, and caller-owned tags passed through a free initialization helper did not combine the correct registers and slots. Reversing allocation and construction lexically with labels preserved unwanted branches and is rejected.

After the stronger tag candidate, hoisting copy counts into vector or signed-short adapter constructors through output references moved each count below the error temporaries but promoted the copy receiver to the wrong register. Opaque receiver variables and mutable reference parameters did not undo that promotion. Copy-count argument objects, mutable members, enum members, full-width bitfields, caller-owned capacity stores, later count declarations, and separate unchecked-allocation continuations did not complete the remaining cycle. Bounded count enums and same-type count reassignments remain unchanged in this stronger context. Private error-string constructor layers preserve the same register state at even depths and disturb tag slots at odd depths. Named thrown error objects retain real exception copies and are rejected. No failed variant supersedes `jpeg_pii_emptytag_0`.

A returned one-field value or two-field allocation result can avoid its otherwise retained stack copies when it has an explicit field-wise copy constructor. `jpeg_pii_returncopy_0` and `jpeg_pii_returnstorage_1` retain the native function sizes and operation streams; their corresponding trivial-copy forms retain unwanted storage. The one-field form changes register priority, while the allocation-result form returns to the strongest candidate's register state. Neither is retained as a PiiPropData solution.

Combining an explicitly copied one-field count return with a mutable scalar count output does not change the tag-only graph: `jpeg-pii-returncountout-trace.txt` has the same surviving virtual registers, degrees and coloring order. Returning an end-pointer record while writing the count output loses useful pointer priorities and is rejected. Signed byte-span updates retain an extra subtraction after memmove. Full-width count bitfields retain unwanted operations. Register-qualified declarations and conditional declarations are neutral. Count-predicate helpers that both write a scalar or enum output and return the original zero-count test are also neutral on the stronger message-output candidate.

The delegated mutable `PiiPropertyCopyState` argument preserves native instructions only when its count member precedes the message member; that variant still promotes both values into earlier optimizer temporaries and regresses the allocation. Reversing its fields or binding additional references retains 16 extra bytes in each function. Changing a higher helper's receiver argument through the address of its first member, a void pointer, an integer pointer representation, the second member's offset or a const-qualified view does not correct the alternate message/destination swap.

## Traps

- A higher comparison can still include extra instructions. The uninitialized ordinary-tag return reaches 99.61905% for Reset while adding a byte copy to the constructor; inspect both functions and native sizes independently.
- The earlier wrong-slot source already had the correct outer registers. Compare older candidates before treating a wrapper's improved percentage as a new register mechanism.
- A copied length-error helper is sensitive to explicit inline. In the controlled `jpeg_pii_errortemplate_0`/`1` pair, a template helper without explicit inline preserves the native operation stream; adding inline gives its literal a function-specific string symbol and changes cached-pointer use and register pressure. Both are automatically inlined. This is a measured local behavior, not a general rule for all helpers.
- A mutable output reference can move one named value while leaving the operation stream unchanged. Here its declaration must precede the count, and adding a same-type reassignment creates a different optimizer temporary with a different simplify threshold. Trace the resulting node identity rather than treating all three forms as equivalent.
