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

### Complete relative function order candidate

The applied candidate uses `CDLGMSGWIN_EMISSION_LAYOUT` in this source only. It makes the char-deleter specialization partial in its deleter type and defines the mutable-reference constructor as an inline template. Under that macro, string declares its mutable data/size/allocator accessors and is_long/long_cap setters before the constructors, retaining their private access. Vector declares reallocate before append_realloc. The generic string do_replace caches its allocator reference immediately before computing maximumSize, then uses that reference for its existing allocate/deallocate/temporary-construction operations. This preserves behavior while requesting the allocator helpers before numeric_limits::max.

The isolated gated build compares 218/218 real functions at 100%, and all 218 occur in the original relative order. The coordinator approved and applied these narrow gates for the next Matching link trial. Full unit acceptance still requires that trial to pass the frozen SHA; relative function order excludes extra weak functions that may be removed by the linker.

Failed alternatives included forwarding accessors, moving definitions without moving first declarations, delaying all allocator max_size definitions, and suppressing do_replace. Those either retained the ordering error or moved unrelated dependencies. Moving the allocator request to the very beginning of do_replace also emits it too early; its position just before maximumSize is material to this compiler's dependency order.

### Literal-pool follow-up

The gated Matching trial linked successfully with unchanged DOL size but 11,570 differing bytes. All 218 function addresses and both exception sections were exact. The remaining differences came from .data/.sdata literal placement and their relocated addresses.

Moving Initialize's intact in-class body before the constructor places its dlgmsgwin/off literals first without changing any function bytes or relative function order. Inspection of the original relocation target for Dismiss additionally revealed an incorrect reconstructed literal: the target is `end`, while the source said `off`. Function-only comparison had not detected this literal-content mismatch. With that correction, the complete original .sdata literal span 0x28?0x1AC equals candidate span 0x98?0x21C, and the complete .data literal span beginning at original 0x284 equals candidate span 0x564. Candidate offsets include other units' weak RTTI/data that the linker removes. Full SHA verification remains the final acceptance check.

## Full acceptance

After applying the Initialize placement and Dismiss literal correction, the coordinator ran the shared build successfully (ninja exit 0) with CDlgMsgWin marked Matching and obtained a PASS against the frozen full DOL SHA. All 218 functions, their linked addresses, exception tables, literal pools and resulting linked artifact are accepted. The prior failed trials above are historical diagnostics. No original hashes were changed.
