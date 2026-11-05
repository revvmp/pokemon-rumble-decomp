# PiiPropData coupled-output continuation

This private lane follows the completed [animation-curve reconstruction](animcurve-20261104-continuation.md). Its starting Pii packet is `jpeg_pii_erroroutput_0/PiiPropData.cpp` with the private `jpeg-pii-currenttag/vector` overlay. It uses `vmprop_probe.Probe`, the frozen original `vmprop-targets/PiiPropData.o`, and configured compiler arguments with `-MMD` removed. The coordinator remains the only writer of canonical Pii source, shared headers, build configuration, and final link outputs.

The starting constructor and Reset comparisons are `99.73283%` and `99.77778%`. Their native sizes are 1,048 and 1,260 bytes, all other 46 functions are exact, and the remaining operands exchange the copy count and exception-object registers. The original exception-record sizes are 256 and 252 bytes. Only the object-register bytes at offsets `0x83` and `0xC7` differ; cleanup relocation identities agree. The [allocation-tag note](piipropdata-allocation-tag-tail.md) and [count-graph note](piipropdata-count-graph-tail.md) contain the prior source and graph evidence.

## Allocation result coupling

`animcurve_pii_allocation_results.py` tests the successful animation-curve pattern at Pii's allocation result boundary, after the ordinary size check and throw expression. Returning the count and assigning capacity in the caller is neutral, including a mutable count parameter, same-type reassignment, and a separately named result. Returning count while writing the allocated data through a mutable reference is also neutral. Returning the allocated pointer while writing capacity through a reference instead changes the original store order and slightly regresses both functions. No result improves the starting packet.

`animcurve_pii_count_width.py` widens only the allocation helper's formal input to signed or unsigned 64 bits, by value or const reference, then narrows it back to the original unsigned word before every operation. Plain initialization, separate assignment, same-type reassignment, a lossless target-pointer representation, and a full-range enum view are all neutral. These controls preserve the complete original input range; no narrowing of an unchecked count to 16 bits is treated as a valid alternative.

## Ordinary-throw receiver capture

The bounded receiver experiment keeps the real `throw length_error(...)` expression and existing exception ownership. A private constructor overload captures the real `logic_error*` into a caller-owned output reference during message initialization. A private `__error_string` overload then uses that captured receiver for the original final `strcpy` destination read. This makes the alias a real input to existing work, unlike earlier unused captures that disappeared.

`animcurve_pii_receiver_output.py` tests three caller declaration positions, plain/same-type/pointer-representation assignments, and direct versus named destination getters. Every form retains extra stack traffic and grows each function by 24 bytes. The constructor becomes 1,072 bytes and Reset 1,284 bytes. The generated code stores the captured pointer, reloads it after allocation, and then loads its text field through the object pointer. The count/object register swap remains.

Passing the captured pointer into the message constructor by value removes the reloads but still leaves one new stack store per throw path. `animcurve_pii_receiver_value.py` consequently produces 1,056/1,268-byte functions, eight bytes larger than the target. Returning the pointer by value or reference from the capture helper, direct assignment expressions, forwarding the output reference again, and late output reassignment all preserve that result. `animcurve_pii_receiver_views.py` repeats the output using `void*`, unsigned word, and full-range enum representations; none removes the stores. Reference/pointer-member proxy arguments, with explicit fieldwise copy constructors or const-reference passing, regress further in `animcurve_pii_receiver_proxy.py`.

The representative receiver audits retain the original 256/252-byte exception-record extents and cleanup relocation identities. Their instruction-offset fields shift with the added instructions, and the two cleanup receiver registers remain wrong. Thus ordinary ownership is preserved, but neither the code nor its complete exception records match. None of these private header overloads is retained.

## Message representation diagnostic

`animcurve_pii_error_inheritance.py` privately represents `__error_string` as a second private base of `logic_error` rather than its member, retaining `exception` as the first base. This preserves both function sizes but leaves the count/object pair wrong, adds address-operand differences, and regresses five other formerly exact functions. The two original exception-record register differences remain, with identical record extents and cleanup relocation identities. Reversing the base order regresses further. This structural diagnostic is rejected; it does not justify changing the recovered standard-library hierarchy.

This repeats the earlier receiver lane's structural conclusion; it does not establish a new source candidate. Subsequent investigation should use that recorded negative rather than repeat private-base variants.

## Copy range coupling

The duplicate-expression branch `jpeg_pii_countbinding_2` recomputes `last - first` at the allocation call. Its count and adjacent pointer nodes are sensitive to the simplify threshold, as described in the count-graph note. `animcurve_pii_range_coupling.py` tests helpers which return count, first, or last while assigning the other two through mutable outputs. Both the original one-field range record and a plain last-pointer local are covered. The new output aliases do not preserve the required original memory/span operations; the strongest tested member-free form still grows both functions by 12 bytes and keeps the object/count swap.

`animcurve_pii_range_identity.py` keeps the original loads in the caller and adds only an unchanged first/end pointer return paired with a computed count output. Plain forms return to the strongest packet when the cached count is passed to allocation. Repeating the pointer difference and applying same-type pointer reassignments changes priorities or retains additional work without improving the baseline. These bounded controls do not demonstrate an instruction-free way to lower the duplicate count's degree while preserving both adjacent pointer thresholds.

## Status and traps

None of the coupled-output candidates in this note supersedes `jpeg_pii_erroroutput_0`. The subsequent [primary MSL reconstruction](piipropdata-primary-msl-tail.md) resolves the two functions by restoring the original vector initialization and optimized-copy boundaries together. These negative probes have no canonical source or shared-header edits.

- A coupled receiver output can retain real storage even when all helper calls inline. Check the native store/load sequence and function extent before treating a surviving alias as an allocation-only control.
- Preserving ordinary throw ownership does not make a candidate acceptable by itself. The receiver variants retain cleanup identities but shift instruction ranges and retain the original wrong receiver-register bytes.
- A representation change that preserves two function sizes can still regress other exact bodies. The private-base diagnostic requires the complete 48-function comparison and remains rejected.
