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

# F-58-2: complete callback construction and disconnect fiber proofs

The initial704-byte Calc result in F-58-1 is superseded by a740-byte producer with every native instruction and all21 actual relocation operands exact. Its216-byte exception record also matches after resolving all9 cleanup targets. Run independently matches788 instruction bytes, all52 actual relocation operands, and its456-byte exception record with18 cleanup targets. The producer uses unchanged cflags_game and the native Wii/1.1 compiler. These are individual native function proofs, not acceptance of the whole source unit or issue closure.

Three ordinary construction contracts explain the earlier Calc gap. The one-element tuple accepts its pointer element by value. function_imp accepts its captured callable by value; bound_func retains const-reference parameters. The CFiber callback/stack-size constructor has a throw() specification, initializes its members normally, then calls Initialize and Start. The native unexpected-handler path after Start proves the exception boundary. Omitting that specification removes sixteen instruction bytes and changes the stack frame. Moving copies into bound_func instead reaches740 bytes but leaves68 instruction-word differences; matching length alone does not establish the original construction context.

Run pauses game audio, retains the default font map, creates the disconmsg layout from5:/game2d.arc, hides its PROMPT pane, and starts its loop motion. It yields until initialized, then repeatedly reads the disconnect reason, chooses message393449/393450/393451/393448 for reasons1/2/3/5 or65557 otherwise, displays TEXT, starts begin and queues loop, waits for the reason to change and another60 frames, and waits for end motion. On completion it resets the layout and resumes game audio. Ordinary explicit PauseGameAudio/ResumeGameAudio calls reproduce the native exception record; an invented audio guard is unnecessary. The reason lives inside the outer loop, and the message lookup completes before reloading the layout for SetText. These source lifetimes reproduce the native register use without forced registers or added temporaries without a behavioral purpose.

GetMessageText's second parameter is a wide-character fallback returned unchanged when the resource or message is absent. The empty fallback is the native two-byte object at0x804B5C48. SetText returns the last updated TextBox, matching the existing header. The new SetPaneVisible declaration follows the native name lookup, visibility-bit update, and attached-layout propagation at0x801D40D0; its descriptive name is inferred. No outside unit's source, split, size, or data type is changed.

The immutable outside producer, objects, and fully resolved receipts are in58-reference/58-run-context, with58-audit-run.py performing complete code and exception comparisons. A fresh same-base all_source comparison guards the bounded shared-header proposal before any merge; each landing round repeats it against that round's actual main. The full source/data/helper ordering, split integration, source-only build graph, and frozen linked DOL remain required before marking this file Matching.

The initial same-base preflight against f5d78c9a94235c2242862172bc99d761aa1ff65d declares718 actual all_source objects. Baseline and proposal configure, all_source, ordinary Ninja, and frozen verification all exit0. Of these objects,708 are whole-file identical and10 differ only in numeric compiler-local symbol names. All other sections, symbol attributes, and actual relocation identities match. The linked ELF's non-symbol sections and symbol attributes match, and the entire frozen DOL is identical. The six callback helper proofs are retained as well: together these bounded consumers prove8 functions /1976 code bytes and720 exception bytes, with exact recovered callback RTTI. Landing repeats the same tests after rebasing; these initial counts do not substitute for the final current-main gate.

## Traps

A correct-sized callback producer can still model copies at the wrong boundary. Preserve the native constructor and exception context, and compare every operand and cleanup record before accepting a construction change. The rejected by-value bound_func profile and the corrected by-value function_imp plus CFiber throw() profile are distinguished above.

# F-58-3: remaining complete-unit integration

Prerequisite PR1063 merged as0b86a45207 after exact-head verification of0f9f698c5343da95ea4479f66d4097184ddae8a6 on main2e494056643a3ed7b36312577dd232df81e51241. The final dynamic consumer gate again covered718 objects,708 whole-file identical and10 differing only in numeric compiler-local names; linked non-symbol ELF sections and frozen DOL bytes remained identical. The claim remains active and the merged worktree was cleaned through land.py before recreating a fresh own worktree.

Additional strict individual proofs cover the280-byte factory with8 actual relocations/EH56, the180-byte concrete destructor with5 actual relocations/EH96, and the six small Draw/CalcView/CalcMtx/CanShow/AdjustSuppressCount/IsActive bodies totaling172 bytes without relocations or exception entries. Together with F-58-2 this establishes16 individually verified native functions /2608 code bytes and872 exception bytes. It does not establish complete-unit source selection. The destructor exception record identifies shared_ptr<CFiber> cleanup at0x80070560 and shared_ptr<CLayout> cleanup at0x800705F0. Run identifies move_ptr<CLayout> at0x80070A60; Calc identifies move_ptr<CFiber> at0x80070AE0 and shared_ptr<u8> cleanup at0x800657E0. Earlier speculative helper labels are superseded by these exact cleanup targets.

The complete-unit continuation must preserve native data and retained helper ordering, not just these functions. Bounded outside experiments in58-reference/58-unit-context show that an implicit concrete destructor alone does not correct data order. Marking all methods inline outside the class moves helper RTTI before member-pointer and layout literals; moving the strong factory after those definitions moves its concrete deleter first but leaves other helper ordering wrong. Defining all methods inside the class produces the same remaining ordering problem. Keeping Calc and Run out of line restores the earlier wrong order. None of these unit-context variants is accepted or copied into selected source, and no flags or padding were changed. The accepted individual producer and immutable landing receipts remain in58-run-context and58-profile-gates.

Further bounded controls did not resolve that ordering: an explicit inline destructor at the beginning or end of the class, moving its constructor declaration to the end, defining the constructor normally before/after the factory or before Calc/after all methods, naming the factory's concrete shared owner, explicitly constructing the returned interface owner, or moving shared_ptr_deleter get_deleter/dispose definitions outside their template class. Their source/header variants and objects remain outside; none changes a canonical header or selected source. Making the otherwise unreferenced public factory inline causes the compiler to omit the entire private implementation, so it is not an acceptable retention mechanism.

The compiler discrimination control uses the same accepted individual producer and headers under Wii/1.1 and Wii/1.3. Their .text, .data, .sdata, .sdata2, extab and extabindex sections are byte-identical, preserving the same ordering gap. Removing -ipa file as a controlled check of F-735-1's medium-confidence inference contradicts native function sizes: factory228 versus280, Calc756 versus740, Run660 versus788, and concrete destructor104 versus180. This supports keeping the canonical IPA setting for this producer; it does not establish a universal compiler rule. Objects, exact source copies and section hashes are retained in58-unit-context/compiler-1.1, compiler-1.3, noipa-original-source and58-compiler-controls.json. No compiler or flag change is adopted.
