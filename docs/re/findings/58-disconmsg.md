# F-58-1: CDisconMsg callback and interface contracts

Original issue #58 contains25 functions /3388 instruction bytes starting at0x8006FBF0. Native RTTI identifies the anonymous CDisconMsg in CDisconMsg.cpp and its IDisconMsg interface. The complete file continues through seven helpers in #59 before CDisplayPhaseCtrl starts at0x80070C20. The #58 owner owns this source-file boundary; no neighboring source split is taken over.

## Bound callback prerequisite

The callback implementation at0x8039B430 carries the exact recovered type `std::tr1::detail::function_imp<void (*)(), std::tr1::detail::bound_func<void, std::tr1::detail::mem_fn_0<void, CDisconMsg>, std::tr1::tuple<CDisconMsg*, nat, nat, nat, nat, nat, nat, nat, nat, nat> >, false, false>`, with the class's original anonymous-namespace spelling. Its allocation is20 bytes: a vptr, a twelve-byte member-function pointer, and the captured four-byte object pointer. The captured tuple has one actual element; the remaining nat parameters do not occupy storage.

The independently authored bounded headers supply one-element tuple/get, zero-argument member-function bind, and the native variadic is_null fallback. Names and type nesting are recovered from RTTI; private member/access-helper names are inferred. Implementations use ordinary C++ member-function invocation and typed ownership, with no manual vtable dispatch, instruction bytes, forced register declarations, or compiler flag changes. No external implementation body is imported. The pinned reference functional header was checked and has no TR1 declarations for these types; it is not evidence for their bodies.

The previous generic is_null(const F&) predicate could inline to false. The native fallback at0x80070700 is variadic: it saves the eight argument GPRs and floating argument registers and returns false. Ordinary `inline bool is_null(...)` emits all84 native bytes exactly. The pointer overload is retained.

Independent comparisons resolve every actual relocation operand and compare all instruction bytes:

| Address | Function | Bytes |
| --- | --- | --- |
| 0x800704A0 | function_base destructor | 64 |
| 0x80070700 | variadic null predicate | 84 |
| 0x80070860 | bound callback heap clone | 100 |
| 0x800708D0 | bound callback placement clone | 88 |
| 0x80070930 | bound callback invocation | 48 |
| 0x80070BE0 | bound callback destructor | 64 |

All six functions /448 bytes are exact, including native member-pointer invocation and vtable destinations. Their six exception records /48 bytes and indexed function lengths also match the frozen executable. The producer and strict comparison are retained outside in58-reference/CDisconMsg.cpp and58-audit-bind.py. These are bounded header-consumer proofs; no game source unit is selected by the prerequisite.

## Existing-consumer controls

On baseline be07caad1bd022822d0c797d8201bd7f5db5788b, the actual Ninja all_source target declares716 objects. Both baseline and proposed complete source builds, the default build, and frozen verification exit0. Of those objects,712 are whole-file identical. The other four differ only in compiler-generated numeric local-symbol names; every other section, raw relocation, symbol attribute, and resolved relocation identity is identical. The linked ELF's non-symbol sections and symbol attributes are unchanged; its symbol names differ. The entire frozen DOL remains identical.

The source-object list comes from Ninja's declared target, not a glob of seeded objects. The per-round landing guard repeats the complete same-base comparison after each rebase, restores the proposal after baseline builds, and reruns the ordinary build and frozen verifier before accepting it. Raw controls are retained in58-reference/58-bind-before and58-bind-gates. A baseline/proposal gate failure stops the merge.

## Game source continuation

The concrete class is24 bytes: vptr0, shared CLayout owner4/8, shared CFiber ownerC/10, and signed suppression count14. Vtable slots after the inherited five ICalcDraw methods are IsActive, CanShow, and suppression-count adjustment; descriptive method names and const qualifiers are inferred. The factory converts a concrete shared owner to the public interface. Calc resumes an active fiber, clears it when finished, otherwise creates a bound Run fiber if a disconnect reason exists and the suppression count is zero; it then updates the layout. The Run routine builds the disconmsg layout, displays reason-dependent text, queues loop motion, waits for reason changes and an end motion, and restores paused game audio.

The first outside Calc producer is704 bytes versus the native740; it is unaccepted. It needs the original callback construction/copy context. The Run body and full owned data/exception/unit integration remain to be reconstructed. No Matching classification, split, expected hash, or source-selected credit is changed by this finding.

## Traps

IDA simplified the variadic null predicate to a constant false return, concealing its84-byte argument-save prologue. Inspect the native instructions and calling convention before replacing library predicates with a template that seems behaviorally equivalent.

The seeded worktree does not necessarily contain build.ninja. Run configure.py before asking Ninja for a compiler command; no build-directory deletion is needed.
