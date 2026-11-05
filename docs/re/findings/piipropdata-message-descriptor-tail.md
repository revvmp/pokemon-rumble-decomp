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

## Count lifetime after exception construction

The next targeted pass asks whether source can recreate the count immediately after the ordinary throw expression without retaining native instructions. It keeps the same message-output baseline and vector ownership. The compiler represents the throw runtime call with a fallthrough edge, so operations written after the throw still participate in its register analysis. This is a compiler observation, not a claim that a C++ throw returns at runtime.

`vm_pii_postthrow_views.py` passes the count by mutable reference into the existing throw helper. Same-type assignment, a full-range enum value round trip, an ordinary word-reference helper and scalar helper-return identities are neutral. Reinterpreting the scalar as an enum reference retains stack operations and grows both functions by 24 bytes. Returning that reference, forwarding it through another helper, or copying its value back does not remove the storage. `vm_pii_postthrow_enum_storage.py` then uses an actual enum count from the copy constructor through both helpers, avoiding a reference reinterpretation. Its ordinary reference identities remain neutral; a reference-based rotation again retains unwanted work.

The read-only constructor traces establish the relevant barrier. In the fresh baseline, the short-copy receiver, count and message are virtual registers 64, 70 and 71, with initial degrees 37, 42 and 36. After the post-throw enum-value round trip, they are 66, 73 and 75 with the same respective degrees. The count is still one surviving virtual register across exception construction. The written intermediate aliases have disappeared before allocation; changed local names or enum flags do not establish a split live range.

A diagnostic assignment of the message address to the count after the throw retains native function sizes but merges those values into one graph node, changes the surrounding allocation and regresses both functions. It confirms that the fallthrough participates in liveness; it is not a candidate implementation. Assigning zero adds actual immediate loads.

The direct full-word intrinsic identity after the throw retains one `clrrwi count,count,0` per copy path, growing the functions to 1,056 and 1,268 bytes. The count is again one surviving node, now with multiple definitions. Moving that same expression through a local, enum value or one or two scalar helper returns eliminates the extra operations and restores the original unresolved pair. The source does not preserve a useful second alias through allocation.

The four traces are `vm_pii_postthrow_baseline_trace`, `vm_pii_postthrow_enum_trace`, `vm_pii_postthrow_clobber_trace` and `vm_pii_postthrow_rotate_trace` under the private unit directory. The three variant trace objects are byte-identical to their ordinary probe objects. The baseline trace is byte-identical to a fresh ordinary compile in `vm_pii_trace_baseline_current`; the older cached JPEG object predates unrelated header changes. No trace modifies compiler state.

## Checked-count and continuation boundaries

`vm_pii_checked_message.py` puts the whole size comparison and ordinary throw inside a helper that returns the checked count while preserving the caller-owned message output. Scalar returns, full-range enum parameters/returns and a separate output reference all preserve the strongest unchanged result. Returning a reference boxes the value and adds 24 bytes. A conditional throw expression either fails to compile or retains additional instructions, depending on its operand form.

`vm_pii_count_join.py` places the full-word rotation after the complete size-check branch and uses the result for both allocation and capacity, rather than only the allocation argument. Assignment to the existing count, a fresh scalar, an enum and a returned scalar helper all collapse to the original register state. Keeping separate assignments on both sides of an explicit branch retains 16 bytes with a local and 40 bytes with a reference. Thus moving the intrinsic to the join does not retain the desired alias without changing the instruction stream.

The configured compiler exposes `__builtin_expect`; it does not expose an unreachable or assume intrinsic in the inspected string table. `vm_pii_expect_count.py` checks the supported expect identity only at the post-throw and post-check boundaries, with ordinary and signed-word views. All four are neutral. This is a bounded identity probe, not a toolchain or control-flow change.

## Coupled scalar storage and alias assumptions

The earlier delegated state test owned its fields in a higher constructor. `vm_pii_shared_state.py` instead declares an actual two-field count/message state inside each existing backend copy constructor and forwards that state by reference through allocation and throw. Both field orders, a declaration-only copy-assignment operator and a count-initializing constructor produce native function sizes but regress the allocation to 99.00764/99.04762 percent. They retain the same two wrong cleanup-register bytes. Enum members retain 16 or 32 extra bytes. Moving the field ownership inward does not provide the missing count priority.

`vm_pii_postthrow_alias_scope.py` combines type-based alias assumptions with the post-throw enum-reference and rotation views. It does not remove the enum reference's 24 extra bytes or the direct rotation's eight extra bytes. Leaving that pragma enabled through the unit additionally regresses four sort functions. No pragma change is retained.

## Complete-unit audit for this pass

`vm_pii_audit.py` compares all 48 original real functions and all 26 original exception records, including normalized cleanup relocations. `vm_pii_postthrow_inventory.py` records 51 compiling candidates from the new targeted families; each retains a `vm-full-audit.json` next to its native object. The best results remain 99.73283/99.77778 percent. No candidate exceeds the starting comparison.

For neutral candidates, all 46 surrounding original functions and 24 surrounding exception records remain exact. The constructor and Reset records retain their original 256/252-byte extents and cleanup identities, differing only at offsets 0x83 and 0xC7 in the already known saved exception-object register. The native objects also contain 17 additional weak-helper exception records already present in the starting candidate. This audit does not claim their final ownership or emission order; that source-link work remains separate.

The continuation changes no production source, shared header, configuration or original object. The next independent coupled-output and receiver-capture lanes have the private negative inventory, so they can avoid repeating these boundaries.

## Traps

- A 64-bit local whose high half disappears can still alter several surviving virtual-register priorities. Native instruction size does not establish that only the intended count/object pair changed.
- Syntax that looks like a single conditional can retain extra control-flow instructions. The while/break copy gate adds twelve bytes per copy path even though it executes its body at most once; inspect native size before treating its score as a register-only result.
- An identity expression can be neutral at a control-flow join but survive on one predecessor. Here a full-word rotation disappears after the size-check join yet remains as `clrrwi` immediately after the throw call. Compare the aligned native instruction pair: objdiff aligns that identity against the target's following shift and labels the real allocation shift as inserted.
- A new source local is not evidence of a new register live range. The post-throw enum-value local still produces one count node at allocation, while the enum-reference form retains stack operations. Inspect surviving nodes and native bytes before inferring a lifetime split.
