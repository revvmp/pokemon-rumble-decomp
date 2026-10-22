# Sprint a14 findings (title, tutorial, UI widgets, system, file loading, main.cpp)

## Landed

- `SetOption`, `SetCurrentSlot` and `GetOption` in `SaveData` take and return `int`; `CTitleSession::Run` now matches apart from data relocations.
- `CWorldObjStatDisplay::Calc`, `Add` and the destructor are written; the unit now has 36 of 48 functions at 100% with only the two `sort` instantiations and weak-order data left.
- `main.cpp` calls `NResetMgr::Initialize`, `NResetMgr::Update`, `CPpuEngine::GetScriptRoot` and `LoadLocalizedFiles` (an inline wrapper over `LoadFiles`), as the target does.
- `LoadLocalizedFile` and `LoadResourceFile` (four-argument wrappers) take a `bool` background flag; the earlier `Ul` mangling was a reference-revision guess.
- `tools/re/name_data.py` renames `lbl_XXXXXXXX` data symbols (vtables, RTTI, constant tables) to the names the rebuilt object uses at the same relocation. `tools/re/permute_names.py` repairs permuted clone names (identical-bodied `shared_ptr`, `move_ptr`, `function` and `dispose` template members that were named in the wrong address order).

## Traps

- **Never use `git stash` in a worktree.** The stash list is shared by every worktree of the repository; a `git stash pop` applied another agent's stash and conflicted. Use commits.
- **Identical tiny template functions get named in the wrong order.** `lwz r3, 0(r3); blr` clones (`get`, `size`, `begin`, `operator->`, iterator copy constructors) and the per-class `dispose`, `move_ptr` destructor and `shared_ptr_deleter` families were named by position. When a function sits at 99.x% with a `bl` row whose two names differ, rename the target symbol to the name the rebuilt object calls (every call site must agree). `permute_names.py` votes across call sites and refuses names that already exist outside the unit range.
- **Duplicate function names break the link.** Giving a second weak copy the same name as another unit's copy produced `multiply-defined` when both units link. Keep `fn_`/`dtor_` placeholders for the second copy.
- **Data symbols named `lbl_` keep a function at 99.7%.** A relocation to a vtable or RTTI object only scores 100% once the target symbol has the same name as the rebuilt one; run `name_data.py --apply` before judging a function.
- **`fd.py`-style filters hide call-name differences.** A function can show no instruction diffs yet remain below 100% because a `bl` goes to a differently named clone.
- **`main.cpp` `SetErrorHandler` is a normal function after `main`, not inlined in the original.** Ours inlines it into `main` (`-ipa file`). `__attribute__((never_inline))`, `#pragma inline_max_size`, moving the body into the header, padding the statement count and `-ipa off` (which un-inlines `UsesAlternateErrorMessage`) did not reproduce the call. `main` stays at 88%.
- **`sort` is the shared blocker for CWorldObjStatDisplay.** The target loads `first.ptr` and `last.ptr` into registers before the length test; ours keeps both iterators in memory. Same family as the open items in sprint-a20.md.
- **Flip experiments.** Flipping `l10n_files` grows the DOL by 224 bytes: ours places `find_leaf` for the `CL10NFileSet` tree right before its `insert_node_at`, the target emits it at the very end of the unit. `CTutorialMsg` differs in `.data` and `extab` volume as well as weak order.
- **Open register-allocation puzzles (all tried with declaration order, loop forms and hoisting variants).** `CTitleSession::RefreshSlots` (99.64%, pointer registers r18/r19 swapped), `CTextWorldObjStat::CalcView` (98.7%, constant and operand FPRs swapped), `CUIPiiSpec` constructor and `SetPii` (95.5%), `CUIPiiListItem` constructor (87.7%, shared_ptr copy loads scheduled earlier), `HandleFileError` (96.6%, shared `return false` block layout).
