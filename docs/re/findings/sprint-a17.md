# Sprint a17 findings

## Traps

- CModel weak functions can be identified by vtable and exception-table context: a `fn_`/`dtor_` placeholder that appears in a named `__vt__` slot or in a `DESTROYMEMBER` entry is that class member; size alone is ambiguous among identical shared_ptr/deleter instantiations.
- `CModel::TPartialAnm<0>` and `<1>` differ in code: `QueueMotion`, `AppendMotion` and `Update` for the non-main partial only test `mMotions.size() == 0`, while the main partial uses the idle check (empty, or one finished motion).
- Defining `TPartialAnm<Main>::~TPartialAnm() {}` out of line emits both destructors at the target sizes; leaving it undeclared makes `CModel::~CModel` call an undefined symbol and score 73%.
- A `CMotionProgram::SetBindFailed(bool)` inline taking `!Bind(mdl)` reproduces the `cntlzw`/`srwi.` pattern and the hoisted `get()` load in `BPartialAnm::CreateAnimObj`.
- `CPath`: a lower-bound constant on the left (`512 <= d && d < 538`) keeps two signed compares; `(int)c >= '0' && (int)c <= '9'` folds to an unsigned range. `const char` for a one-letter `std::string(&c, &c + 1)` lets CodeWarrior fold the length to 1.
- Python text-mode writes on Windows add `\r` to files that are later read by shell loops; strip them before passing names to symbol rename scripts or `symbols.txt` lines split.
- Unresolved: the nested `IsIdle()` temporaries use swapped registers (outer r6, inner r7 in the target) in `QueueMotion<1>`, `AppendMotion<1>` and `Update<1>`; moving the check into helpers, locals, ternaries or reordered operands did not change it.
