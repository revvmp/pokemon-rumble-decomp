# Sprint a17cx: CStatistic and CPiiCollectionBox

## 2026-10-22 results

Work stayed in the current checkout. Changes are uncommitted. No target manifests, original artifacts, AGENTS.md, or other documentation were edited. No new tests or scratch files were added to the repository.

CStatistic is now `Matching` in configure.py. CPiiCollectionBox remains `NonMatching`.

| Function group | Initial | Retained result |
| --- | --- | --- |
| CStatistic Get / Add | 99.89% / 99.94% | 100% / 100% |
| CStatistic GetCounterStopValue | 79.64% | 100% |
| CStatistic Set | 92.31% | 100% |
| CStatistic SaveSnapshot / RestoreSnapshot | 98.26% | 100% |
| CStatistic ExportStatisticValues | 97.53% | 100% |
| CStatistic out-of-line vector assignment | 97.98% | 100% |
| CPiiCollectionBox SaveSnapshot / RestoreSnapshot | 91.43% / 91.38% | 100% / 100% |
| CPiiCollectionBox ExportCollectionData | 99.58% | 100% |
| CPiiCollectionBox Create | 98.16% | 99.17864% |
| CPiiCollectionBox two sort instantiations | 99.900795% | Unchanged |

The refreshed report.json contains 12,874 functions with fuzzy_match_percent == 100, versus 12,857 initially, a net increase of 17. CStatistic has 25/25 in that report; CPiiCollectionBox has 40/43. Unitdiff still displays 99.83% for CStatistic's insert_node_at because its anonymous string relocation is named lbl_803AE090 in the reference and a compiler-generated name in source. This is a label-comparison discrepancy: the final linked DOL is byte-identical, including the string and its references.

## Retained changes and evidence

- Get uses an outer `id < 0` branch and the range `-100 <= id && id < -97`. Removing the redundant `id >= 0` test after the negative branch preserves automatic inlining into Add. Both functions match exactly. The lower-bound-on-the-left form matters, as in sprint-r1 and sprint-a17.
- Set declares the stop-value local before the limited-value local, then assigns it from GetCounterStopValue. A separate unsigned index preserves `id + 100` in the target register. An explicit final negative-ID `else { return; }` reproduces the otherwise missing branch at function offset 0xB8.
- The const map_base::find overload now forwards directly to tree_.find, casting the tree reference rather than calling the nonconst map_base::find overload. Removing that inline level reproduces GetCounterStopValue's recomputed header addresses. Changing only end(), iterator argument types, or lower-bound local declaration order did not solve it.
- Non-POD vector assignment performs its range algorithm directly instead of creating an additional forward_iterator_tag temporary. The three-argument allocation helper forwards its existing pod tag rather than making another temporary. These remove the extra exception-state bytes and restore the target stack layout.
- A const-pointer construct_at_end overload advances the destination before advancing the source. Keeping the generic iterator overload unchanged avoids the CDlgPiiSelector regressions seen when changing every iterator loop. The split-buffer generic loop also remains unchanged.
- CPiiCollectionBox::Entry has separate mNormalForms and mRareForms scalars, not a two-element array. Its explicit constructor initializes the scalar fields and then zeroes mBestBRR. The target copy assignment treats the two masks separately; the user-defined default constructor also makes the implicit copy constructor emit the target memberwise operations. These changes make both snapshots exact.
- ExportCollectionData clears the packed flag and reserved bits before assigning the defeated count and masks. That ordering gives the target registers. Create reads the first-form bit through the same packed record definition, reproducing the target byte load.
- CStatistic definitions now follow the target function order. map_compact_tree::destroy is defined out of class so its recursive helper is emitted after insert_node_at rather than immediately after CStatistic's constructor. The first attempted Matching link failed at DOL offset 0x4746A because of this helper order. Moving the definition left only four differing bytes, all in CStatistic exception tables.
- Those four bytes referenced the wrong named split-buffer destructor: the original exception tables point to 0x800BC7B0 for SD_StatisticValue::Data, whereas that address had been labelled SD_BattleRoyalData. The two names at 0x800BC6B0 and 0x800BC7B0 were exchanged. Both functions in their owning CGameSession report still score 100%, and the final full link verifies. Also named the evidenced default_delete<CStatistic> RTTI object at 0x804B7398.

## Validation

Ran python configure.py. Ninja stalled while compiling, so compilation used the configured commands obtained with ninja -t commands, invoking mwcceppc directly instead of sjiswrap. All 156 configured game source units were rebuilt after the final shared-header changes. No non-game source directly includes these game/STL headers. Generated the normal linker response file from the main.elf link rule's inputs, deleted build/wpse-v1/main.dol, and ran the configured linker command. Link exit status was 0. Then ran dtk elf2dol (exit 0) and:

`python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`

Result: `PASS main.dol`, `PASS: 1 declared artifact(s)`. SHA-256 of the verified output is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

Regenerated build/wpse-v1/report.json with objdiff-cli report generate. git diff --check passes. The final indentation-only cleanup of map's destroy definition was recompiled in CStatistic and produced an identical object hash.

## Traps and remaining work

- Adding zero-count guards to both non-POD default-construction loops, together with the corrected Entry constructor, makes CPiiCollectionBox::Create reach 100%. However, rebuilding the project lowered the global perfect-function count to 12,854; retaining just the split-buffer guard gave 12,855. Both fail the user's 12,857 baseline constraint, and both guards were removed. They also regress CStatistic's already exact expansion paths. Do not reintroduce blanket guards without resolving these regressions.
- Three separate assignments or a for loop in Entry's constructor increase the flattened inline cost enough to change Create's generated code substantially. The chained array initialization preserves the desired inlining and store order.
- The remaining Create differences are the two zero-count guards (four instructions total). The two sort instances differ only in five register operands around `m - first < last - m`. The extensive prior experiments are recorded in sprint-r1's std::sort status; further comparison/iterator-signature/local-order variants here did not solve it.
- A successful full DOL comparison is needed after moving template definitions or correcting clone names. Perfect function scores did not catch the early recursive-destroy placement or the exception-table clone references.
- After a direct dtk split, ninja -n can list only regeneration steps; running those dry-run commands does not establish a fresh object baseline. The final validation explicitly rebuilt all game compilation commands before linking and regenerating the report.

The requested two-unit flip is incomplete: CStatistic is verified Matching; CPiiCollectionBox still needs Create and the shared sort register allocation resolved.


## 2026-10-22 post-rebase repair

Starting revision: `c9811c98f`. This session stayed in the current a17 worktree, made no commits, and retained changes only in `include/vector`, `configure.py`, and this appended note. No target manifests or original artifacts changed. No scratch files were added inside the repository.

### Retained repair

CStatistic is again `Matching`. Restoring the direct non-POD `assign` algorithm alone is insufficient: the allocation helper and const-pointer construction order also need the forms recorded in the earlier session, which were absent from the rebased header.

- `__vec_constructor::assign(first, last, int2type<false>)` performs the existing range algorithm directly, removing the two additional dispatch-tag temporaries.
- `__vec_deleter::allocate(n, capacity, pod_type tag)` forwards `tag` to `allocate(capacity, tag)` instead of constructing another tag.
- A const-pointer overload of `__vec_constructor::construct_at_end` advances the destination pointer before advancing the source pointer. The generic iterator overload and split-buffer loop are unchanged.
- Public `vector::assign` still uses `assign_dispatch` and `assign_range`. CGameResult, homebutton, and main source changes from `9e3b1386f` remain intact. No type-specific exceptions, compiler flags, assembly, or source-byte substitutions were introduced.

### Validation and regression gate

The starting report contained **12,938** functions with `fuzzy_match_percent == 100`. The final report contains **12,951**, a gain of **13**. Comparing the identities of every initially perfect function against the final report found **zero losses**, not merely a nondecreasing aggregate count. CStatistic is **25/25**, including its four repaired assignment functions. Its `insert_node_at` still shows the anonymous-label-only discrepancy in standalone unitdiff; the report and exact linked DOL establish the retained result.

The other nine newly perfect functions are CPiiCollectionBox SaveSnapshot/RestoreSnapshot, CDlgPiiSelector's three-argument vector allocation helper, five SaveData vector assignment instantiations, and CUIMenu's shared-pointer vector assignment. The final report counts are CGameResult 71/75, homebutton 48/51, and main 133/145; none of their initially perfect functions regressed. These units remain NonMatching as they were at session start.

Ran `python configure.py`, then rebuilt all **156 game source units** using their configured compiler commands from `ninja -t commands all_source`. The sjiswrap invocation stalled again, so the compiler commands were executed directly. For the six game sources containing non-ASCII text (CDlgMsgWin, CDlgPiiList, CDlgPiiSelector, CMsgBalloon, CSaveManager, CTitleSession), source bytes were converted to CP932 in place only during compilation and the original UTF-8 bytes restored in a `finally` block. All compilation commands exited 0. This preserves the wrapper's required source encoding without retaining source changes or temporary source files.

Generated the normal linker response file from the configured link inputs, deleted `build/wpse-v1/main.dol`, and executed the configured `mwldeppc` and `dtk elf2dol` commands. Both exited 0. With CStatistic selected as Matching, ran:

`python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`

Result: **PASS main.dol; PASS: 1 declared artifact(s)**. SHA-256: `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This verifies all currently source-linked Matching units together with CStatistic; it does not claim all game source units are reconstructed. Regenerated the normal objdiff report and checked `git diff --check`.

### CPiiCollectionBox follow-up and traps

Both sort instantiations already score **100%** with this checkout's headers. CPiiCollectionBox now has **42/43** perfect functions; Create remains **99.17864%**, missing the two zero-count guards described above. No CPiiCollectionBox source/header changes were retained. Constructing `mEntries(496)` directly instead of default construction followed by resize reduced Create to 30.58%; value-initializing `mBestBRR` in Entry's initializer list gave 99.15% and changed store ordering. Both experiments were reverted and the original source recompiled.

A direct compiler invocation must preserve Shift-JIS source encoding. Compiling UTF-8 unchanged caused apparent regressions in CDlgMsgWin OpenYesNo, CMsgBalloon Init, and CSaveManager Work, including shifted string references. Correct CP932 input restored all three to 100%. Do not attribute those differences to vector changes. Also, the initial CStatistic object/report here scored about 99.04-99.05% in its affected routines; 99.39/99.46% describes the intermediate direct-assign-only form, not the fully repaired form.


## 2026-10-22 Create resolved; Matching integration remains blocked

This follow-up used only the current a17 checkout and made no commits. The retained code change is in `include/game/CPiiCollectionBox.h`: Entry initializes its three `mBestBRR` elements with `for (int i = 0; i < 3; ++i)` instead of a chained assignment. The vector header, algorithm header, CPiiCollectionBox source ordering, BModelObj source, and configure.py are restored to their starting contents. No target manifests or original artifacts were edited. Experimental backups were outside the repository; no scratch files were added inside it.

### Create and the regression gate

The indexed array-initialization loop changes the compiler's treatment of the surrounding vector construction loops: Create now contains both zero-count guards without an explicit guard in the vector header. This is a constructor/inlining interaction, not evidence that the original used a different vector API. The retained source is ordinary C++ and leaves `mEntries.resize(496)` intact. All **43/43** CPiiCollectionBox functions score **100%**. The starting report contained **12,958** perfect functions; a rebuild of all 156 game units with the retained loop produced **12,959**, with **zero losses** by function identity. The sole new perfect function is Create.

The earlier note that a for loop necessarily damages Create's inlining is superseded for this checkout: the exact increasing-index loop above is verified to match. `std::fill_n(mBestBRR, 3, value)` with a local zero-valued int also reached 100%, but was not retained because the indexed loop is simpler. Passing literal `0` to fill_n produced a static constant load and scored 99.38398%; an explicit pointer loop scored 93.702255%. The chained assignment remains 99.17864%.

### Rejected Matching link

After obtaining 43/43, CPiiCollectionBox was set to Matching, main.dol was deleted, and all game objects were rebuilt with their configured compiler commands. The configured linker and dtk elf2dol both exited 0. The prescribed frozen-manifest verification **failed**, first at DOL header offset **0x1E**. The source-linked DOL was 4,095,072 bytes rather than the required 4,094,880. Matching was therefore reverted as required. Perfect function scores do not establish matching layout or exception tables.

Observed integration problems:

- The existing source defines Create before ExportCollectionData, whereas target addresses put ExportCollectionData at 0x800E1CE0 and Create at 0x800E23A0.
- Defining CPiiCollectionBox's constructor in its source before RecordCollection, and moving ExportCollectionData before Create, preserved 43/43 and placed the Entry-vector destructor first, but did not make the full link match. These experimental ordering changes were reverted.
- The emitted vector subscript helper precedes GetCollectionType; the target puts GetCollectionType at 0x800E1C10 and the helper at 0x800E1C20. The two Entry/Entry-pointer iterator inequality helpers also appear in a different order relative to sort and other vector helpers.
- ExportCollectionData's source exception table references `__vec_deleter<SD_PiiCollectionData::Data, allocator<...>>`, which has no matching name in symbols.txt and is emitted as an extra 0x70-byte routine. The corresponding target relocation names the allocator-reference BattleRoyalData destructor at 0x800BADF0. This is evidence of a naming/type discrepancy to investigate, not a retained rename.
- ExportCollectionData's split-buffer cleanup points to target `dtor_800BC530`, while the current named SD_PiiCollectionData::Data split-buffer destructor is at 0x800BC5B0. Confirm the identities of these clones from their callers and exception tables before renaming. No symbol names were changed in this session.

Moving the three affected vector member definitions out of class merely moved their emission to the end of the object. Adding explicit inline restored the old order. An explicit instantiation after implicit use, including an attempted extern-template declaration, was rejected as a redefinition by this compiler. Adding inline to std::sort changed its code and was reverted. No template-order workaround was retained.

### Other rejected investigations

A fill-resize overload and `resize(496, Entry())` scored 85.67967%; forcing per-element split-buffer filling reached 88.54209%. Guards at append call sites scored 96.704315%. Explicit guards inside both default-construction functions reached 100% only with destination-pointer calculation before the guard, but the all-game perfect-function count fell to 12,941 with 18 losses. All vector changes were rejected.

BModelObj CalcSize was investigated but no change was retained. Compound assignments after copying the resource-volume components fixed the twelve multiply-operand differences. The reference's second branch also uses absolute maximum Y, model-scale Y, and object-scale Y for height; replacing the current helper result improved an experimental function to 93.15951%, but left redundant computation and other differences. That partial rewrite was reverted. SaveData and CDlgCollection source were not edited.

### Final retained-state validation

Restored CPiiCollectionBox to NonMatching and ran `python configure.py`. Recompiled all **156 game source units** with the configured commands from `ninja -t commands all_source`, bypassing sjiswrap as in the preceding sessions. Non-ASCII source was converted to CP932 only during compilation and restored in a finally block. Every compilation exited 0. Regenerated the normal linker response file, deleted main.dol, ran the configured linker (exit 0), and ran dtk elf2dol (exit 0). Regenerated report.json: **12,959 perfect functions, zero baseline losses**, CPiiCollectionBox **43/43**.

Ran `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`: **PASS main.dol; PASS: 1 declared artifact(s)**. SHA-256: `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This PASS uses CPiiCollectionBox's original fallback; the Matching flip remains incomplete. `git diff --check` passes. Retained changes are only the Entry array-initialization loop and this appended note.
