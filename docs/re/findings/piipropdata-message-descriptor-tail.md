# PiiPropData value and message-descriptor continuation

This independent private lane starts from `jpeg_pii_erroroutput_0` and the `jpeg-pii-currenttag/vector` overlay, following [the allocation-tag investigation](piipropdata-allocation-tag-tail.md). It uses the frozen `vmprop-targets/PiiPropData.o`, the configured native compiler command without dependency-file output, and separate `vm_pii_` scratch outputs. No production source, shared header, compiler option or build configuration is changed.

The strongest starting comparison remains 99.73283 percent for the 1,048-byte constructor and 99.77778 percent for the 1,260-byte Reset. All 46 other original real functions are exact. The unresolved count/exception-object register swap is unchanged by this continuation; there is no retained gain or source-link claim.

## Wider count and throw-argument identities

`vm_pii_count_and_throw.py` tests signed and unsigned 64-bit count locals on the improved baseline, projecting every live use back to the original unsigned word. Both direct pointer-difference initialization and an explicit unsigned-word initializer preserve the native function sizes but displace other registers. The latter reaches 99.23664/99.36508 and moves the count into registers occupied by the outer count/zero values. The high half does not supply an isolated count identity.

Moving message assignment directly into the length_error constructor argument preserves native sizes but returns to the earlier three-register cycle: the count, message and exception object all have the wrong colors. A comma expression that assigns then returns the existing message, an explicit const-reference argument, address/dereference identity, and ordinary or static non-template throw helpers are neutral. This is different from the previously rejected comma expression around the exception object itself; these experiments keep the ordinary throw operand and introduce no exception copy.

## Maximum-size and pointer-range views

`vm_pii_limit_views.py` wraps the maximum-size comparison value in a one-field class. A direct value access is neutral. Passing or returning the trivial-copy aggregate retains extra work and regresses the two functions. An explicit field-wise copy constructor removes that extra work and returns to the same strongest register state. Separate unsigned-long, unsigned-int and full-range-enum comparison helpers, by value or const reference, are also neutral.

`vm_pii_range_identity.py` tests pointer identities independently of the allocation helper's count parameter. A void-pointer reassignment for the first pointer is neutral. The same representation for the end pointer preserves the native instruction count but exchanges additional first/end pointer registers without fixing the original swap. Pointer/integer and pointer/enum union OR identities regress both functions. Giving the local CopyRange a declaration-only copy-assignment operator, independently on either copy path and in combination with those views, changes none of these outcomes. The non-POD control successful in CPlayer therefore provides no gain in this particular local range context.

## Copy-gate syntax

`vm_pii_copy_gate.py` changes the nonempty-copy gate, separately on the LevelMove copy, the unsigned-short copy, and both. Switch, boolean switch, early constructor return, single-iteration do/break and inverted if/else forms are neutral. A while/break form retains twelve extra bytes per modified copy path: changing both grows the constructor to 1,072 bytes and Reset to 1,284. That form is rejected before any register interpretation.

## Message descriptor and factory

The separate layout lane was investigating an allocation-descriptor factory. This lane instead tests a message-argument descriptor with a trivial destructor and an explicit field-wise copy constructor. Its constructor stores the message pointer and assigns the existing caller-owned output. Private throw-only length_error and logic_error overloads consume the descriptor, leaving their ordinary constructors unchanged.

`vm_pii_message_descriptor.py` covers direct construction, a prvalue-returning factory and a named-return factory; one or two constructor boundaries; and value or const-reference parameters. Direct descriptors reproduce the same three-register cycle as assignment inside the plain constructor argument. Factory forms regress both target functions and five previously exact functions. They do not bypass the allocation problem established by the ordinary-message output candidate. All twelve remain private and rejected.

The five scripts and corresponding JSON summaries are under `tools/local/tail-20261104/`. Each probe retains its source, native object, compiler output and full-unit comparison. The constructor/Reset instruction counts and relative branch destinations were inspected for the distinct wider-count, message-argument and end-pointer register states. No candidate was strong enough to justify a full-link trial or canonical edit.

## Traps

- A 64-bit local whose high half disappears can still alter several surviving virtual-register priorities. Native instruction size does not establish that only the intended count/object pair changed.
- Syntax that looks like a single conditional can retain extra control-flow instructions. The while/break copy gate adds twelve bytes per copy path even though it executes its body at most once; inspect native size before treating its score as a register-only result.
