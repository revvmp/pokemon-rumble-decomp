# PiiPropData copy-count graph continuation

This private continuation follows [the allocation-tag investigation](piipropdata-allocation-tag-tail.md). It used `vmprop_probe.Probe`, the frozen private original, and the private `jpeg-pii-currenttag/vector` overlay. No PiiPropData source or shared header edit is retained from this lane.

## Starting point and graph constraint

The strongest source remains `jpeg_pii_erroroutput_0`: constructor 99.73283%, Reset 99.77778%, native sizes 1048 and 1260 bytes, and the other 46 real functions exact. Its only remaining instruction operands exchange the inner copy-count and error-object registers.

Repeating the pointer difference at the allocation call creates a distinct optimizer count temporary. In `jpeg-pii-countduplicatetrace`, the short-copy count is virtual register 35; its first visit in simplify pass 2 has working degree 28, equal to the available-register count. The first/end pointers are virtual 36 and 41, also at the threshold when visited. Removing an edge from the count can make it removable, but removing the count then lowers those later pointer visits below the threshold as well. The resulting pointer-register changes must be included when assessing that control.

A potentially useful source shape would preserve a lower-priority count identity without lowering the first/end pointer priorities. This is a graph hypothesis, not evidence that the original source used a particular helper or representation.

## Bounded source experiments

The `sort_pii_` packet family records the following rejected controls:

- `sort_pii_freshcount` and `sort_pii_freshcount_best` rename the allocation helper input and declare a fresh count local before its first use. Plain copies, same-type reassignment, signed/unsigned views, and pointer round trips leave the strongest register state unchanged. On the repeated-expression branch, they only change which early optimizer count temporary survives.
- `sort_pii_checkedaggregate` returns a one-field object with an explicit field-wise copy constructor after the size check and error branch. Four construction/return shapes and explicit inline on/off preserve native function sizes but change stack-tag placement. `sort_pii_checkdepth` adds forwarding layers: the next layer restores the exact slots and returns to the unchanged strongest register swap; subsequent layers alternate these two states. A post-check aggregate return is not an improvement over the scalar form.
- `sort_pii_bytecount_fixed` expresses pointer subtraction through signed byte distances, explicit signed/unsigned types, or an enum. These change priorities or instructions and do not improve the strongest comparison.
- `sort_pii_range` represents both pointers as fields, reverses their field order, uses a first-pointer field with a separate last pointer, or uses two ordinary pointer locals. The ordinary-pointer repeated-expression form reaches 99.19847%/99.333336%, but changes the first/end pointer colors and still does not solve the count/object pair. Its graph is `sort_pii_range14_trace`.
- `sort_pii_scheduling` confirms that final-only scheduling substantially regresses these functions; the default two-pass scheduling is required by the current source. Enabling type-based alias assumptions in this scope is neutral. The scheduling experiment also affects destructor functions in the same pragma region and is rejected as a unit-level change.
- `sort_pii_countidentity` and `sort_pii_unioncount` use a union to retain scalar identity. Plain union reads retain stack stores and enlarge the frame. An OR/AND identity obtains the desired later count/object pair but also inserts a count copy and stack store in each copy path. Those added instructions reject it. A simple one-field struct assignment is neutral; `memcpy` retains additional work.
- `sort_pii_errorcount` adds a mutable count output to the ordinary throw helper. Same-type, enum, pointer, and plain copy identities are neutral. Assigning zero adds a real immediate load and does not solve the pair.

The explicit native diff of `sort_pii_unioncount_00` shows why its apparently useful colors are not a solution: the constructor frame grows from 0x110 to 0x120, each count calculation receives a new move to the preserved count register, and each union retains a stack store. The same opcode/size requirements that reject these forms continue to apply to all future controls.

## Exception type and lifetime continuation

The subsequent source-only type lane used the same frozen target, source packet, and vector overlay. It coordinated with the separate [ordinary-throw receiver investigation](20261104-piipropdata-receiver-tail.md), which had already ruled out Runtime declaration attributes and base-exception constructor/destructor controls.

- `sort_pii_errorparam.py` changes the message parameter independently at the `length_error`, `logic_error`, and `__error_string` constructor boundaries among value, const reference, and mutable reference forms. Every compiling combination preserves the constructor/Reset register pair. A const-reference `length_error` parameter also regresses the previously exact `SpeciesMoves` assignment, so it is not an accepted shared-header change.
- `sort_pii_novtable.py` confirms that this compiler accepts `__declspec(novtable)`. Applying it only to `exception` is neutral. Applying it to derived error classes changes required vtable stores and regresses code; such a layout/lifetime change is rejected.
- `sort_pii_destructor_types.py` declares each previously implicit `logic_error`, `length_error`, or `__error_string` destructor explicitly. Empty inline bodies, `throw()`, declaration-only forms, inline or strong out-of-class bodies, alternate member-declaration positions, and an empty `return` preserve the same 99.73283%/99.77778% pair and the other 46 exact functions. Explicit inherited `virtual` is likewise neutral. Adding a new virtual destructor to `__error_string` changes its layout and is rejected.
- `sort_pii_message_wrappers.py` replaces the private `logic_error` message member with one through four forwarding wrappers, using either composition or private inheritance around `__error_string`, and value/const-reference/mutable-reference message parameters. Odd wrapper depth reverses the two shared-pointer deleter tag slots and slightly regresses the comparison; even depth returns to the strongest unchanged result. The target exception hierarchy and native function sizes remain unchanged. Both depth-one and depth-two EH audits retain 256/252-byte records and identical relocation targets; only the existing cleanup-register bytes at offsets 0x83 and 0xC7 differ.

These experiments do not provide a new candidate. The wrapper result is another local example of inline-depth stack-slot parity, not a way to change the cleanup receiver's register priority.

The later `sort_pii_lifetime_pragmas.py` batch retests 25 compiler-exposed optimizer controls specifically on the strongest current constructor and Reset. Lifetime computation, pointer analysis, scalar-temporary classification and related controls are neutral; disabling propagation, common subexpressions, scalarization, prelinearization, arithmetic transformations, empty-loop removal or dead assignments/code regresses. This does not supersede [the earlier compiler-option survey](vm-properties-tail-20261104.md); it only confirms that the newer message-output baseline does not turn these particular controls into a solution.

The explicit Runtime branch was also checked without changing its canonical status. `sort_pii_union_lifetime.py` starts with `lr_pii_simpleplacement_scope_0` and substitutes a directly constructed typed union. Omitted, empty, `throw()`, and supported destructor-annotation forms all retain 296/292-byte exception records, 40 bytes larger than the target. The ordinary typed union is not raw unowned storage in this compiler: its generated cleanup still destroys the nontrivial member. An empty written destructor body is not evidence of side-effect-free generated destruction. No annotation that contradicts that ownership is accepted.

`sort_pii_storage_views.py` tries aligned byte/word storage, typed references, a typed pointer-to-array view and a zero-element typed-array member. The compiling forms keep the placement variant's object-relative accesses or introduce further differences; none restores the target frame-relative access pattern with the correct exception ownership. `sort_pii_storage_exceptions.py` scopes the exceptions pragma to the explicit Runtime helper, its explicit instantiation, or its local storage declaration. Those forms are neutral: the typed union still has the extra 40 EH bytes, while raw placement storage retains the original two register-byte differences and wrong addressing. All these packets are rejected private diagnostics.

This lane moved to an independent effect-animation investigation after preserving the negative evidence. The ordinary message-output source remains the strongest Pii packet; the JPEG and VM lanes own further Pii work.

## Handoff

The JPEG lane owns typed Runtime error construction and the strongest source packet. The layout lane independently investigates ordinary-throw return boundaries and cleanup-receiver identities. The coordinator owns canonical edits, complete builds, source-link trials, and frozen verification. This count lane leaves no candidate stronger than `jpeg_pii_erroroutput_0` and makes no matching claim.
