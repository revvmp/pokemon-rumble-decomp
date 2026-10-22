# Sprint f1: where `tree_imp::find_leaf` is emitted

Game flags (`Wii/1.1 -O4,p -inline auto -ipa file`). Wii/1.0, 1.1 and 1.3 give identical orders, so the target compiler is not the cause. Sandbox files are in `scratchpad/f1/sb`; `f1/uo2.py <unit>` compiles a unit with `f1/oi` ahead of `include` and marks misplaced `.text` symbols; `f1/swt/sweep.py` (with `OI=<overlay>`) runs the order sweep over all 803 units.

## Target positions

| Unit | Last sweep-1 groups | `find_leaf` | Next item |
| --- | --- | --- | --- |
| COyaInfo | tree_imp, `weak_ptr::lock`, `allocator::destroy`, `alloc_ptr` dtor | 17 | `recommend` (pass 2), then split-buffer functions |
| l10n_files | tree_imp, deleter `get_deleter`/`dispose`, `shared_ptr`, `move_ptr`, `alloc_ptr<__list_deleter>` | 48 | implicit deleter destructors (late phase) |
| MapPiiTable | tree_imp `<w>`, `<c>`, `allocator<string>::destroy`, `alloc_ptr` `<c>`, `<w>` | 26 `<w>`, 27 `<c>` | split-buffer dtor |
| BWorld | ... `move_ptr`, `allocator::destroy`, `alloc_ptr<__list_deleter>` | 182 compact, 183 tree_imp | deleter destructors (late) |
| BModelObj, CPpuEngine, CFileData, CLayout | same pattern | after the last `alloc_ptr` dtor | `length_error` dtor, deleter dtors or split-buffer dtor |

In every unit that inserts a non-POD value, `find_leaf` comes right after the last `alloc_ptr` destructor and before the late phase. Units with POD values (NPpuVM compact map, CDlgCollection `set<unsigned short>`) emit it right after the strong function that inserts.

## Instantiation phases (sandbox-confirmed)

After parsing, each iteration runs:

1. **Sweep.** Class templates are visited newest-declared first; within a template, newest specialization first; within a specialization, members in declaration order. A member requested after its position was passed waits for the next iteration.
2. **Member-template slot.** Member templates requested during the sweep, in sweep order (CLayoutManager `find_leaf<i>`).
3. **Inline pass.** `inline` or in-class functions requested after their position was passed and not inlined, in reverse order of their requesters (sandbox `p3a`/`p3b`, `il.cpp`). COyaInfo `recommend` is here.
4. **Late phase.** Implicit destructors and other vtable members of classes created while parsing.

Forward declarations and include order move groups (e1 rule 4a), and inline functions requested from a group that is visited later land in that group's position.

## Landed

- `include/memory` includes `MSL_CPP/alloc_ptr.h` before `allocator.h` (MSL's `<memory>` pulls `alloc_ptr` in through `<msl_utility>`), and `alloc_ptr.h` includes `revolution/types.h` for `nullptr`. Every target emits `allocator::destroy` before the `alloc_ptr` destructors, so `alloc_ptr` is declared first.
- `tree_imp` declares both `find_leaf` overloads before `insert_one` (the sprint-cx2 change). The request from `insert_one` then waits for the next iteration.
- Order sweep: 11 units improve, none regress (total misplaced 6701 to 6679). MapPiiTable `.text` order now matches exactly. No function score changed (12809 at 100%), and verify passes. No unit flips: MapPiiTable is 28/29 (`Clear`), and COyaInfo and l10n_files still differ, as described below.

## The rule that is still missing

The target places `find_leaf` after the sweep groups but **before** the inline pass and the late phase, ordered like a sweep. In MapPiiTable `<w>` comes before `<c>`, in BWorld compact comes before tree_imp, and in COyaInfo it precedes `recommend`. No phase of this compiler produces that position for a non-template, non-inline member:

- **Non-inline, declared before `insert_one` (landed):** the request goes to the next iteration, after the late phase. This is correct for MapPiiTable, but in l10n_files, BWorld and BModelObj it lands after the deleter and `length_error` destructors, and in COyaInfo after `recommend`.
- **`inline` but not inlined, declared before `insert_one`:** the request goes to the inline pass. l10n_files `.text` becomes ORDER OK, and BWorld tree_imp and BModelObj are correct. COyaInfo is correct when `COyaInfo.h` includes `<map>` before `<vector>`. MapPiiTable comes out reversed (`<c>`, `<w>`), because the inline pass reverses the sweep. This was measured by padding the body with 600 dead statements to stop inlining. A plain `inline` is always inlined, and `__attribute__((noinline))` is ignored.
- **Member template:** this is the only kind that lands in the target slot, in the target order (sandbox `k.py` `mtmpl`), but the target symbols carry no template arguments.
- **No effect:** member templates as callers, `static`, `const`, `virtual`, overload declaration order, definition order, `using base::find_leaf`, forward declarations of `tree_imp`, and inline or in-class `insert_one`.

Next step: find a source form that makes the target `find_leaf` behave as an out-of-line inline whose requests follow the sweep order. Two leads:
- a POD/non-POD dispatch in `insert_one`, as in r1 rule 15. Non-POD units emit `find_leaf` late and POD units emit it at the caller.
- the reason the original `find_leaf` is never inlined. The inline-pass explanation fits five of the six units; it needs a natural reason for the out-of-line copy and an explanation for MapPiiTable's order.

## Traps

- `scratchpad/e1/var.py` and `sweep.py` hard-code `rumble-a15`; the copies in `f1/swt` point at `rumble-a01` and accept an `OI` overlay include directory.
- The `uo2.py` misplacement marks are minimal-edit hints. A symbol can be wrong without a mark, so read the full order (`FULL=1 uo.py`) before concluding.

## Budget check on the inline lead

I padded an `inline find_leaf` with dead `if (gTreeProbe == N) { ... }` statements, which count toward the inliner cost. MapPiiTable (string keys) stops inlining it at 250 to 300 such statements, and COyaInfo and l10n_files (int and `const char*` keys) at 300 to 400. That is about 10000 units: `find_leaf` alone has to exceed the whole budget (sprint-i1 F-i1-1). The target body compiles to the same 24 instructions as ours. A form that is naturally heavier (a second comparator call, extra flags, a comparator built per call) would either change the code or be folded before costing. So the inliner budget does not explain why the original keeps `find_leaf` out of line. MapPiiTable's `<c>`, `<w>` reversal is the same at every padding level, so it is not a parity effect.
