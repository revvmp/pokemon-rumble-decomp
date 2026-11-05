# PiiPropData receiver and exception-storage investigation

## Scope

This independent 2026-11-04 lane follows the [allocation-tag continuation](piipropdata-allocation-tag-tail.md). It uses direct native compiler outputs and private header overlays under `tools/local/tail-20261104/`; no PiiPropData source, shared header, configuration, or original artifact is changed by this lane. The unit has 48 original real functions. Every comparison below checks the two remaining functions and the 46 previously exact functions.

The starting `jpeg_pii_erroroutput_0` source has the original 1,048-byte constructor and 1,260-byte Reset, all native operations and stack slots, and only the copy-count/error-object register swap. Its exception records have the original 256/252-byte sizes and cleanup identities, differing only in the two saved error-object register bytes. The separate `jpeg_pii_hoistmessage_1` candidate has the right count and error-object registers but exchanges the message and destination registers.

## Constructor lifetime evidence

The hoisted-message trace assigns the copied-vector receiver virtual r74 before the error object r67, count r66, and message r65. The receiver's value coalesces with the original destination r50 and promotes it to r22. The corresponding exception audit confirms a source lifetime difference: the hoisted outer copy constructor first completes `base()` and then initializes elements in its body. It owns a completed `__vec_constructor` base during the helper call, whereas the target's delegated copy chain owns the inner `__vec_deleter`. Two cleanup relocations differ despite the matching instruction operations.

Defaulted scalar scratch parameters on the outer copy constructors preserve delegation: the backend receives count/message by mutable reference while the outer constructor initializes it directly. Both parameter orders compile at native size, but return to the already known register states rather than completing the pair. The diagnostic generic signature changes also regress GetWazaLevels, so they are not retained. Evidence is `lr_pii_scalarcopyargs_0/1` and `lr_pii_scalarcopyargstrace.txt`.

Typed and void receiver locals, declaration before or after the scratch values, and same-type receiver reassignment do not change the hoisted-message register state. Pointer/integer/enum union receiver views retain extra stack operations. Explicit copy constructors on the caller-owned CopyState keep native function sizes but do not recover the required allocation. These controls are recorded in `lr_pii_receiverlocal_*`, `lr_pii_receiverunion_*`, and `lr_pii_copystatecopy_*`.

## Exception receiver controls

Adding a private constructor overload that captures the length_error or logic_error receiver into a caller-owned typed or void pointer has no effect: the unused output disappears. Moving length_error, logic_error, or error-string constructor definitions out of class, with or without explicit inline, also preserves the same residual. Replacing the error-string's sole shared-pointer member with an equivalent private base does not alter allocation. The corresponding probes are `lr_pii_errorcapture_*`, `lr_pii_errorctorposition_*`, and `lr_pii_errorstringbase_*`.

Const, volatile, and const-volatile typedefs for the normal thrown length_error likewise preserve the same instruction and register state. These are `lr_pii_throwqualification_0/1/2`.

Nested length_error prvalue copies are real copies in this compiler, adding 224 bytes per additional level across the two inlined throw paths. Returning a length_error through an ordinary factory causes the heavy path to stop inlining. These are rejected, not evidence of copy-elision equivalence. See `lr_pii_errorcopies_*` and `lr_pii_errorfactory_*`.

## Typed Runtime diagnostic

An isolated D-0016 diagnostic constructs the real length_error in size-derived, aligned local storage and calls the existing Runtime `__throw` interface. The source supplies the observed textual type hierarchy and the existing destructor identity; it supplies no instructions or opaque object bytes. This is not an accepted production replacement for a C++ throw expression.

Ordinary void-pointer placement new introduces null checks, placement-delete rethrow paths, 56 extra instruction bytes, and a larger frame. A named automatic length_error plus a noreturn Runtime declaration restores native instruction counts, but adds 40 exception-table bytes for the local object's cleanup. Adding a nonthrowing specification does not remove that ownership. Both paths are rejected.

A bespoke placement tag without matching placement delete preserves the original exception-record sizes and cleanup identities. Giving that one-pointer tag an explicit field-wise copy constructor, or passing it by const reference, removes its temporary stack copies. One null check per throw path still remains, leaving eight extra instruction bytes and incorrect pointer bases. Evidence is `lr_pii_taggedthrow` and `lr_pii_taggedthrowcopy_0/1`, with `exception-audit.json` alongside the first copy variant.

The native compiler's actual option is `use_simple_stdpnew`; `new_may_throw` is absent from the compiler option strings and its earlier neutral probes did not establish a supported control. The shared compiler investigation identifies the option at compiler option-state offset 0x224. Its special case recognizes the global standard placement-new signature with one void-pointer placement argument.

Scoping `#pragma use_simple_stdpnew on` around a deferred function-template definition is insufficient. Explicitly instantiating that template while the option is on, then resetting it before the remaining source, activates the special case for just the throw helper. An ordinary non-template helper parsed in that scope has the same effect. Leaving the option active for the later vector instantiations changes their required placement checks and regresses other functions.

The isolated `lr_pii_simpleplacement_scope_0` result has native 1,048/1,260-byte functions and retains all 46 other exact functions. Its exception records again differ only at the two saved object-register bytes, with all cleanup identities identical. The code still has the original count/object swap, three object-relative accesses per throw path where the target uses the frame, and a changed tag/vtable store schedule. It is not an improvement over the normal throw baseline and is not applied. Assigning the caller-owned error pointer before construction is neutral for typed, void, const-void and enum address views; an additional same-type reassignment perturbs the outer register allocation. Evidence is `lr_pii_placementreceiver_*` and `lr_pii_placementview_*`.

## Traps

- A copy-constructor helper can preserve all operations while changing the completed base that owns cleanup. Check destructor relocation identities before accepting a register-only candidate.
- A pragma around a deferred template body may be inactive when that body is instantiated. In this compiler, explicitly requesting the template under `use_simple_stdpnew` isolates the option; leaving it on for later vector instantiations changes unrelated placement construction.
- Native function size alone does not validate a Runtime exception workaround. The named-error form has the right size but adds automatic-object cleanup records; the accepted baseline retains the original ownership.
