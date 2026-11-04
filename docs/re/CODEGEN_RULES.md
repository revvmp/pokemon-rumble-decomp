# Codegen rules

Compiler behaviour of CodeWarrior Wii/1.1 (`-O4,p -inline auto -ipa file -str reuse`) confirmed in sandbox compiles and real units. Each rule links to its evidence. Entries marked hypothesis or unresolved are not confirmed.

## Inlining

- **The budget is all-or-nothing for the function being compiled.** If the flattened cost of its direct inline callees exceeds about 10000 units, it inlines nothing, not even accessors. A nested level over budget prunes its own children into calls. No pragma or flag changes the threshold. Non-inline functions auto-inline only when small (about 14 statements). Fix by moving the heavy part into an inline helper whose own level overflows. [sprint-i1](findings/sprint-i1.md#f-i1-1-the-rule)
- **Pruned callees look like non-inline functions.** Check whether the same helper is inlined elsewhere in the target function before concluding it is out of line. [sprint-i1](findings/sprint-i1.md#traps)
- **Deleting a statement can change inlining everywhere in a function.** [sprint-i1](findings/sprint-i1.md#traps)

## Stack frame slots

- **Size buckets, smallest at the bottom; first-created highest within a bucket.** Creation order is named locals, then temporaries, then inline expansions in call order. [sprint-t1](findings/sprint-t1.md#the-slot-model)
- **Each inline level reverses its callee's objects, so only parity matters**, relative to the closest common inline ancestor. A named tag versus an unnamed one is a parity flip. [sprint-t1](findings/sprint-t1.md#the-slot-model)
- **Extra or misplaced stack objects often mean an inline helper with by-value parameters.** [sprint-w10](findings/sprint-w10.md)
- **A dead zero byte store at the frame bottom is a value-initialised empty tag.** Declare the tag as a local to remove it. [sprint-r1](findings/sprint-r1.md#rules) rule 12

## Register allocation and scheduling

The allocator and scheduler are recovered in [sprint-ra](findings/sprint-ra.md). `tools/re/ra_trace.py` prints a function's colouring order, and `tools/re/regscan.py` separates register-only near-misses from source differences.

- **Colouring order is descending virtual-register number**, except that a node whose degree is still K or more (29 GPRs, 28 with a frame pointer, 32 FPRs) when the ascending simplify scan reaches it is coloured before all others. Lower-numbered neighbours removed earlier in the same scan lower that degree. Each node takes the lowest free register (r0, r3 ... r12), else the lowest open non-volatile, else opens the next from r31 down. [sprint-ra](findings/sprint-ra.md#select)
- **Numbering:** parameters in order, then named locals in reverse declaration order, then `@` inline and compiler variables in reverse creation order, then expression temporaries in generation order, then registers created by the machine-code optimiser. So named locals get r31, r30 ... in declaration order, above the parameters; inline results and temporaries rank above named locals; among temporaries the latest is coloured first. [sprint-ra](findings/sprint-ra.md#virtual-register-numbering) rules 1 to 3
- **To colour a value after a named local, make it a named local declared later.** Use compound assignment to keep a result in that local's node (`outer *= outer`). [sprint-ra](findings/sprint-ra.md#rules) rule 4
- **Named variables coalesce only with physical registers**; temporaries and `@` variables merge into the lowest number of a copy chain. sprint-ra rule 7.
- **Mask register numbers before chasing a swap.** A parameter one interference short of K moves from the top to the bottom of the non-volatile registers; in homebutton the cause was an unrelated source difference. sprint-ra rules 5, 6.
- **Scheduling ties keep generation order.** The priority is deadline, then uncovered successors, then height, then (before allocation only) opcode rank. Register arguments are generated left to right. sprint-ra rule 9.
- One definition reaching every use keeps one register; full reassignment starts a new live range. [sprint-r1](findings/sprint-r1.md#rules) rules 1, 6
- `x + c` and `x += c` differ in operand order; the last operand of an `||` early return changes branch shape. Rules 2, 3.
- A negated inline `bool` across two levels materialises the compare. Rule 9.
- Iterator subtraction needs `long operator-(iterator rhs) const` loading `mPointer` into a named local. [sprint-r1](findings/sprint-r1.md#follow-up-iterator-subtraction-fixes-the-sort-comparison-2026-10-22), [sprint-cx](findings/sprint-cx.md)
- Mixing the engine parameter and its `CPpuEngine*` cast moves a string pop's frame address; `#pragma scheduling off` reproduces the target in a sandbox. [sprint-p1](findings/sprint-p1.md#rules)
- Open cases (tried, not solved): [sprint-r1](findings/sprint-r1.md#open-cases-tried-not-solved), [sprint-ra](findings/sprint-ra.md#diagnosed-not-fixed).

- **An enum-typed local can preserve a distinct value when ordinary integer locals collapse during optimization.** CPauseMenu's const area enum restores area-first extraction with mode-first bitfield insertion; BWorld's const state enum preserves a separate zero-load operand before OR. Both fixes retain all native instructions; integer-local controls did not. Use the actual enum when available, or cover the complete input range in a documented local type. This is a tested source control, not a guarantee for every register mismatch. [CPauseMenu tail](findings/167-cpausemenu-tail.md#leavestage-solved), [BWorld tail](findings/45-bworld-tail.md#2026-11-04-enum-zero-value-identity).

## Loads and constants

- **A `const T&` or const pointer lets loads be cached across stores.** Non-const references, pointers and by-value class parameters reload after each store. In const member functions, loads from `this` may hoist above the prologue. Fix by making the parameter non-const or by-value, and rename the mangled symbol. [sprint-w1o](findings/sprint-w1o.md#rule-a-const-reference-lets-codewarrior-cache-loads-across-stores)
- **Equal constant arguments (`li` versus `mr`).** A byte argument merges with an earlier equal constant only in functions with an exception-specification or try region. The original differs per function with no source form found yet. Unresolved. [sprint-p1](findings/sprint-p1.md#equal-constant-arguments-li-versus-mr)
- **A flag passed as equal constants may need an `int` type through the call chain** (ScriptLayout `LoadFileList`), giving word-sized `li`. [sprint-a03cx](findings/sprint-a03cx.md#2026-10-22-equal-constant-calls-use-an-integer-loading-flag)

## Emission order of weak functions and data

- **Template and inline instances are created at parse time; out-of-class non-`inline` members are instantiated after parsing.** An inline body in a header creates its instances at include time, so include order moves `.data` and `.text`. [sprint-e1](findings/sprint-e1.md#rules) rules 1, 2
- **Class templates are walked newest first**, by first declaration (a forward declaration of the primary counts); within a template, newest specialization first, members in declaration order. [sprint-e1](findings/sprint-e1.md#rules) rules 4, 4a; [sprint-e2](findings/sprint-e2.md#model)
- **Vtable-only inline functions come after all strong functions, in reverse class declaration order.** An implicit destructor is generated late; a user-declared one is strong at its definition. [sprint-e1](findings/sprint-e1.md#rules) rules 6, 7
- A request made while another template is instantiated goes to a later round (rule 5). `inline` on an out-of-class template member can change code (rule 8).
- String literals of inline functions become `@STRING@<function>@n` objects after the vtables (rule 3).
- **Unresolved: `tree_imp::find_leaf` position.** No source form reproduces the target slot in all six units. [sprint-f1](findings/sprint-f1.md#the-rule-that-is-still-missing)
- Hypothesis: a lower-priority duplicate weak copy explains second `max<int>` and `DynamicCast` copies. [sprint-e1](findings/sprint-e1.md#open)

## Tooling traps

- `name_callees.py` renames applied outside the unit being worked can break the link and the DOL; restrict applied `symbols.txt` changes to the unit's range. `permute_names.py` repairs permuted identical-clone names. [sprint-a13](findings/sprint-a13.md), [sprint-a17](findings/sprint-a17.md), [sprint-a14](findings/sprint-a14.md)

## Same-type floating-point reassignment

A redundant assignment such as posZ = static_cast<f32>(posZ) can prevent a coalescing transformation that an equivalent initializer cast does not affect. This reproduced BModelObj inline float copies and both TextWriterBase display-list functions. Once instruction structure matches, trace surviving named locals: TextWriterBase required moving posLeft before posZ in declaration order to swap their final register priority. These are measured local D-0016 controls, not a general guarantee. See [TextWriterBase](findings/textwriter-final-tail.md) and [BModelObj](findings/329-bmodel-final.md).
