# Sprint w8cx: CSaveManager and CPlayerPii ordering

## 2026-10-22 investigation and rejected source selections

Starting revision: `4c021db844c6da77304405161b5eacf3e28b715c`. The working tree was clean. Work stayed in the current w8 checkout, without commits, changes to target manifests, or repository scratch files. Temporary scripts, backups, comparison outputs and the starting report were outside the repository. Only this findings record is retained; all source, header and configure.py experiments were restored byte-for-byte.

The requested Matching flips remain incomplete. The current report differs from the older sprint-w8 premise: CPlayerPii is already **253/254**, not 250/254. Its three data-reference functions are perfect in this checkout. CSaveManager remains **243/243**. The initial and final reports both contain **13,052** perfect functions, with no per-unit count changes according to the supplied cnt100.py.

### Rejected ordering experiments

- Moving CSaveManager's existing dont_inline pragma before its includes did not repair the function sequence.
- Including functional first moved the vector group before the function group, but also moved function_imp after the shared-pointer/allocator groups. It did not reproduce the target order and was restored.
- Defining __deque_imp::iterator::operator* inside its class did not bring it to target row 126.
- Forward-declaring the nested iterator and defining it outside __deque_imp also failed to fix that first ordering difference.
- Moving the nested iterator definitions to the end of their enclosing classes, retaining forward declarations, did not fix the sequence. Moving only the __cdeque_pointer iterator fails compilation because void_iterator embeds it by value; moving both nested definitions compiles but does not fix order.
- No vector, functional or deque header change is retained. No assembly, compiler-option workaround, artificial padding or byte substitution was used.

### GetShadowName probes

Tested a mutable pointer array, a local CPlayer reference with that array, obtaining the player number before array initialization, a local aggregate containing the array, and copying a static table through memcpy. None reproduced the target instruction schedule. The mutable-array forms interleave a store with the array loads and load mpPlayer too early; the aggregate retains the original scheduling difference; obtaining the index first moves the GetPlayerNo call before the table copy; memcpy remains an additional call. All variants were restored.

The current original source has two scheduling differences visible in direct disassembly: the target stores LR before the table's lwzu and loads mpPlayer immediately after that lwzu. The current source emits lwzu before the LR store and loads mpPlayer after the remaining three table loads. This is more precise than treating the mismatch solely as one moved mpPlayer load.

### Source-selected acceptance checks

Freshly compiled both units with the exact configured Wii/1.1 flags. The supplied var.py hard-codes another checkout, so the temporary runner explicitly set its root to this checkout. CSaveManager's UTF-8 bytes were converted to CP932 only for compilation and restored in a finally block. Compiler invocations exited 0.

Ninja stalled without output on the requested DOL build and was interrupted; no successful Ninja build is claimed. Used the generated main.elf link edge to construct an external response file and ran the configured Wii/1.1 linker and dtk elf2dol. main.dol was deleted before each direct link. Both commands exited 0 for each selection below, with the existing FORCEACTIVE warnings.

| Source selection | Frozen-manifest verification | First differing DOL byte |
| --- | --- | --- |
| CSaveManager additionally Matching | FAIL | 0xB8866 |
| CPlayerPii additionally Matching, CSaveManager restored | FAIL | 0xA935A |
| Both restored to their starting NonMatching status | PASS | None |

The prescribed verifier command was used for every check:

`python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`

For the CSaveManager source-selected ELF, unitcmp.py found no target-only or source-only functions. The first moved function is __deque_imp::iterator::operator*, expected at **0x8010A6E0**, emitted at **0x8010C330**. seq.py confirms the first **126** functions have the target relative order. Subsequent discrepancies include pointer iterators, insert overloads, vector helpers and bind helpers.

For the CPlayerPii source-selected ELF, unitcmp.py likewise found no target-only or source-only functions. Its first moved function is __deque_ops<VEC3>::buf, expected at **0x800F9D80**, emitted at **0x800FC850**. GetShadowName and the deferred function order remain unresolved.

### Final validation and reusable lessons

Restored all experimental source/header files and configure.py, regenerated configuration, recompiled both original units, deleted main.dol, linked and converted successfully, and reran the frozen-manifest verifier: **PASS main.dol; PASS: 1 declared artifact(s)**. Final SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

Regenerated report.json over 1,475 units. The existing method_layoutSetString and NPpu::ToUnicode exception-decoding warnings remain. cnt100.py reports **13,052 -> 13,052**, no per-unit changes; CSaveManager is **243/243**, CPlayerPii **253/254**. No shared-header experiment survived, so this final link validates the original currently Matching selection with both requested units still using fallbacks. It does not establish their source-link acceptance.

Check current report counts before carrying older sprint numbers forward: the three CPlayerPii data-reference scores no longer need repair in this checkout. Raw source objects contain duplicate weak RTTI/vtables that can disappear at link; their raw offsets alone do not establish a retained-data defect. The supplied comparison helper's hard-coded checkout must be overridden before compiling. The tested nested-class relocations and include reorder do not solve the remaining order problem; no successful replacement ordering model was established in this session.
