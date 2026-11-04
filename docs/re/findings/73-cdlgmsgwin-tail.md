# CDlgMsgWin deleter-copy tail

## F-73-4: mutable deleter reference restores scheduling

The final unmatched function was the 108-byte char-array shared-pointer deleter constructor. Its base constructor and compressed-pair constructor calls matched, but the empty-deleter byte load was scheduled ahead of the derived-vtable store. Earlier ordinary declaration and constructor probes are recorded in sprint-a15 and sprint-a17.

The retained local template specialization takes the deleter as a mutable reference. It preserves the same base, storage, virtual methods and constructor initializer. The body does not modify the referenced deleter. The inferred target constructor name changes from the const-reference spelling `FPcRCQ33std6detail20default_delete<A0_c>` to `FPcRQ33std6detail20default_delete<A0_c>`. The generic shared-pointer caller already passes its own mutable by-value deleter, so no caller const-cast is required.

This declaration produces the exact instruction schedule and all 218 unit functions match. A volatile mutable reference also matched in scratch; a const volatile reference retained the old mismatch. Those variants were not retained. Explicit empty-copy bodies and scoped optimization controls were unsuccessful. The scratch scripts `msgdeleter.py`, `msgdeleter-pragmas.py` and `msgdeleter-signature.py` record those controls. The last script renames only a diagnostic target object's symbol for comparison; the build uses the normal dtk symbol configuration, with no object or instruction rewriting.

The local specialization is recorded under D-0016 rather than claimed as recovery of an original standard-library revision. The complete fallback build and frozen main.dol verification pass. A source-link trial linked successfully but failed the full SHA; function emission and data ordering remain unresolved, and the unit remains NonMatching until that gate passes.

## Traps

A const-reference declaration can affect alias analysis even for a one-byte empty-class copy. Changing the reference qualifier, while preserving all actual loads and stores, repaired the scheduling difference that expression casts and ordinary local declarations did not. This is evidence for this constructor, not a project-wide change to standard-library interfaces.

## Source-link ordering trial

The coordinator tried CDlgMsgWin as Matching after the mutable-reference constructor correction. The link completed, but the frozen DOL SHA failed. The failed artifacts were copied to `tools/local/session/msg-linked.elf` and `msg-linked.dol` for independent diagnosis. The unit remains NonMatching.

The full char-deleter specialization emits its non-template constructor ahead of Draw, moving it from original address 0x80087820 to 0x80085690 and initially shifting subsequent functions by 0xC0. Making the definition inline moves it later but still requests its template dependencies a pass too early. A source-local partial specialization `template<typename D> shared_ptr_deleter<char,D>` with a mutable `D&` constructor restores both this constructor cluster and the type_info comparison to the original relative order. Its isolated object retains all 218/218 exact functions. The candidate is scratch-only (`tools/local/session/msg-order/partial.cpp`), pending full ordering and link acceptance.

The remaining relative-order differences in that candidate are a vector reallocate/append pair and seven mutable wide-string accessors requested too early by the current generic string implementation. An explicit declaration suppressing do_replace instantiation removes necessary min/get_buffer instances and moves other dependency groups; it is not a viable correction. Moving the local get_deleter definition alone does not change its dependency request order. No emission candidates have been applied to main.
