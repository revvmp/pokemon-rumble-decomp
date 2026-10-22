# Sprint e3: nested classes and the three parked ordering blockers

Game flags (`Wii/1.1 -O4,p -inline auto -ipa file`). Sandbox sources and the `gen.py`/`gen2.py`/`sb.sh` runners are in the session scratchpad (`e3/`); `e3/tl.py <obj> a b` prints a unit's `.text` order with template arguments stripped, and `e3/uo.py <unit>` marks misplaced symbols. No unit was flipped and no source or header changed; this file records the evidence.

## Compiler source evidence

[doldecomp/mweppc](https://github.com/doldecomp/mweppc) (CC0) decompiles the GameCube 2.x C++ front end. It is an older compiler without `-ipa`, so its code-generation order is not ours, but its template bookkeeping matches every sandbox result below:

- `CTemplClass_DefineNestedClass` (`src/frontend/CTemplateClass.c`) creates a separate `TemplClass` for a class nested in a class template and prepends it to `ctempl_templates` at the nested declaration, so a nested class is always newer than its enclosing template.
- `CTemplClass_CopyNestedClass` gives a non-template nested class one instance per enclosing specialization, created when the enclosing specialization is instantiated. A nested class template (one with its own parameters) instead gets a new `TemplClass` copy, prepended at that moment.
- `CTempl_Instantiate` (`src/frontend/CTemplateNew.c`) is one pass: every `TemplClass` newest first, each instance (and partial-specialization instance), each referenced and undefined member in scope order; then every function template (`ctempl_templatefuncs`), which is the "member-template slot" of sprint-f1. `CParser_GlobalCleanup` alternates that pass with `CInline_GenerateDeferredFuncs`, whose action list is LIFO, which is the drain ("inline pass").
- `CMangler_MangleFunction` appends `<args>` and the return type only when the object is a function template instance. The target's value-type `find_leaf` has neither, so it is a plain member, not a member template.

## Rules (sandbox, Wii/1.1)

1. **A nested class of a class template is its own walk group, positioned at its first declaration inside the enclosing class, so it is walked before the enclosing class.** Two specializations walk `In<s>`, `In<i>`, `Outer<s>`, `Outer<i>`; nested members never join the enclosing group. Unchanged by: in-class or out-of-class nested members, declaring the nested class first or last, forward-declaring it and defining it after the enclosing members or after another template, a dependent or non-dependent base, a dependent member, `class` versus `struct`, friend declarations (in the nested class, in a later class, in a non-template class), redeclaring the enclosing template later, nesting in a partial specialization, and the compiler (Wii 1.0, 1.1, 1.3, 1.5, 1.7, 0x4201_127, GC 3.0a5.2 give the same order).
2. **A nested member requested by a walked member of its enclosing class waits:** out of class to the next pass, in class to the drain. A non-template `inline` free function requested by a walked member is generated right after that member. Member templates, function templates and nested member templates requested from the walk go to the function-template loop after the whole walk.
3. **An in-class enclosing member called from a strong function is generated at parse, with its nested callees depth first.**

## What the targets show

- **CSaveManager:** `__deque_imp` group, then `__deque_imp::iterator::operator*`, then `__cdeque`, `__cdeque_pointer`, then the `__cdeque_pointer::iterator` constructor, `*`, `++`, `-`, then `__vec_deleter`. Each nested iterator follows its enclosing group in the same pass. Ours walks the nested group first (rule 1), so `operator*` waits (rule 2).
- **CPlayerPii:** `__deque_ops` group, then `__deque_ops::iterator::operator*` and `__throw_deque_length_error` (both after the whole group, in request order), then `__cdeque`, `__cdeque_pointer`, `__cdeque_buf`, `__cdeque_common`, and only then `__cdeque_pointer::iterator` and `__cdeque_buf::iterator`, followed by the allocator groups. The `__cdeque_pointer` iterator constructor is called by `begin`, which comes 23 functions earlier.
- In both units the free inline `__throw_deque_length_error` comes after the requesting group, not after its requester (`allocate_back`), unlike rule 2. So the target generates requests made during a group after that group, while this compiler generates them at the requester or at the end of the pass. This is the one mechanism the three blockers share: CSaveManager's iterators, `find_leaf` after the last walked group (COyaInfo, l10n_files, MapPiiTable, BWorld) and CMsgBalloon's per-element interleaving of `__vec_deleter` and `__vec_constructor` all look like items generated after the group or pass that requested them. No source form tested here reproduces it, so the cause is probably the requester's form (which function makes the request, and whether it is in class), not the nested class.

## Prior art checked

- KooShnoo/pokemon-rumble `libs/PowerPC_EABI_Support/MSL/MSL_C++`: `tree` is the 2006 MSL (`__tree_deleter`, revision 1.36.2.3) with `find_leaf` only as `template <class Key>`; there is no `deque`; `vector` is a 224-line stub. It has no `tree_imp`, `__cdeque` or `__deque_imp`.
- yannicksuter/smstrikers-decomp `msl_tree.h`: a reduced `__tree` with `find_leaf` as a member template. No deque.
- The CodeWarrior for Wii 1.1 primary `tree` (2007, revision 1.39.2.1, in `198-reference/primary-header-research/wii11-primary-headers`) is also `__tree_deleter` with member-template `find_leaf`.

None has the `std::detail::tree_imp` / `__cdeque_buf` / `__deque_imp` structure that the game's mangled names show, so none can be adopted. The game's library is a later or vendor revision.

## Traps

- **mweppc's `CInline_GenFunc` defers a function whose body references an uninstantiated inline template member; Wii/1.1 with `-ipa file` does not.** A sandbox member calling an undefined in-class member of another template is emitted in place. Do not use mweppc's code-generation order to predict ours; use only its template list and pass structure.
- **A nested-class probe needs a marker group older than the enclosing class.** Without one, "waits for the next pass" and "walked after the enclosing class" give the same order.
- `scratchpad/f1/uo2.py` and `e1/var.py` hard-code other worktrees; the `e3` copies point at `rumble-cx` and take `OI` for an overlay include directory.

## Follow-up: searching for an "after the group" path

- **No compiler we have emits a walk request after the group.** A sandbox where a walked member requests a free `inline` function and an in-class nested member gives the same order on Wii 1.0RC1, 1.0, 1.0a, 1.1, 1.3, 1.5, 1.6, 1.7, 0x4201_127 and GC 3.0a3 and 3.0a5.2. The free function is emitted after its requester and the nested member after the whole pass. GC 2.7 has no `-ipa`. Switching the game compiler would not help, so it was not tried.
- **mweppc has no per-group drain.** `CTempl_Instantiate` walks every group, then function templates. Deferred work runs only in `CParser_GlobalCleanup`, after each top-level declaration and after each whole pass. Its queues are the LIFO action list (in-class template members through `CTempl_InlineFunctionCheck`; friend functions defined in class templates through `CTemplClass` friend instantiation) and the export list (functions whose references are not yet defined). Nothing runs between groups. So in that model, emission right after a group can only come from walk order, from the next group in the list.
- **Constructs tested as walk-time requests, none emitted after the group.** A nested member declared in class and defined out of class `inline`, a friend defined in the nested class, the address of a nested member, the address of a free inline function, a nested static function used in a default argument, and the enclosing class's own out-of-class `inline` member. All went to the end of the pass, except the free function's address, which was emitted after its requester.
- Remaining hypothesis, untested: in CPlayerPii and CSaveManager the requesting group is the last group of its pass, and the "after group" functions are drain items; the groups that follow (`__cdeque`, `__cdeque_pointer`, ...) would then belong to the next pass. To test, find which function first requests each `__cdeque` member in the target.
