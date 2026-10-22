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

## 2026-10-22 CPiiCollectionBox Matching acceptance

CPiiCollectionBox is now selected as Matching. Work stayed in the current checkout, with no commits, no repository scratch files, no changes to config/targets, and notes appended only here. The starting working tree was clean. Final changes are configure.py, src/game/CPiiCollectionBox.cpp, include/game/CPiiCollectionBox.h, include/vector, config/wpse-v1/symbols.txt, and this record.

### Retained source and emission changes

- Move the existing constructor body from the header to the beginning of the source. Its resize behavior is unchanged; this makes the retained Entry-vector destructor the first function, at 0x800E1AD0. The unused standalone constructor is discarded by the linker.
- Put ExportCollectionData before Create, matching 0x800E1CE0 and 0x800E23A0.
- Add the private GetEntry accessor, defined after GetCollectionType, and use it in RecordCollection. File IPA inlines the accessor and the unused standalone accessor is discarded. This preserves RecordCollection's exact instructions while making the first parsed direct subscript use belong to GetCollectionType. The retained subscript helper consequently follows GetCollectionType at 0x800E1C20 rather than preceding it. No additional function survives the link.
- In GetManyDefeatedPii explicitly select the free std::operator!= for its three iterator comparisons. The generic overload compares iterator base pointers. Both vector iterator classes expose a comparison_result typedef, so substitution excludes unrelated types; the pointer iterator gains the same base accessor already present on the value iterator. Existing member operators are retained for other consumers and for sort. No CPiiCollectionBox-specific template specialization, compiler option, assembly, padding injection, or byte substitution is used.
- Correct the two provisional member-operator symbol names to the emitted free-template names. The pointer-iterator comparison is at 0x800E2D40, immediately before sort; the Entry-iterator comparison is at 0x800E3680, immediately after __sort132 and before the remaining vector helpers. These free-template emission points reproduce all target instructions and retained addresses. This is a validated source model, not proof that the original source used the same spelling or comparison_result typedef.
- Keep the implicit class destruction and the existing Entry constructor loop. No primary-template forward-declaration change from sprint-e1 rule 4a was necessary. Moving the member operators out of class, with inline on their declarations, reproduced their old wrong order; changing class-template group order was not the solution to these free-function emission points.

### Destructor identities and exception tables

The initial ordered-source link retained an extra 0x70-byte vector-storage destructor. Exception-table relocations in ExportCollectionData and in CGameSession's reserve/push_back routines identify the actual types:

| Address | Correct identity | Evidence |
| --- | --- | --- |
| 0x800BADF0 | __vec_deleter<SD_PiiCollectionData::Data, allocator<SD_PiiCollectionData::Data>> destructor | Collection-data reserve cleanup; previously misnamed as BattleRoyalData with an allocator reference |
| 0x800BC530 | __vec_split_buffer<SD_PiiCollectionData::Data, allocator<SD_PiiCollectionData::Data>> destructor | Collection-data push_back cleanup and ExportCollectionData; previously dtor_800BC530 |
| 0x800BC5B0 | __vec_split_buffer<SD_PiiPersonalData, allocator<SD_PiiPersonalData>> destructor | CGameSession push_back<vector<SD_PiiPersonalData>> cleanup, extab offsets 0xC44 and 0xC60; previously misnamed as collection-data storage |

The last identity is SD_PiiPersonalData itself, not its nested Data record. Naming it also raises CGameSession's perfect-function count by one. These corrections eliminate the additional destructor and restore ExportCollectionData's exception cleanup targets.

All existing split boundaries are correct and remain unchanged:

| Section | Start | End |
| --- | --- | --- |
| extab | 0x80023AA0 | 0x80023E58 |
| extabindex | 0x8004A190 | 0x8004A274 |
| .text | 0x800E1AD0 | 0x800E5084 |
| .data | 0x803A8A68 | 0x803A8B6C |
| .sdata | 0x804B7040 | 0x804B7058 |
| .sdata2 | 0x804BA058 | 0x804BA05C |

After the destructor corrections and subscript-order correction, the remaining exception differences were one relocated byte at 0x80023C63 and seven extabindex bytes. They followed the misplaced comparison/vector functions; they were not extra exception entries or bad boundaries. Correcting the comparison emission order removes them. Anonymous data labels differ in raw object naming, but their retained addresses and bytes match, including sort statics, deleter RTTI/vtable, and exception strings.

### Rejected alternatives and reusable lessons

Replacing iterator member inequalities globally with a generic free inequality initially passed the CPiiCollectionBox-only full link, but failed the shared-consumer gate. An unconstrained generic overload conflicted with allocator inequality. Even after constraining it, removing members changed other units' sort inlining and removed equality helpers, reducing the perfect-function count. That replacement was rejected. The retained overload is explicitly selected only by CPiiCollectionBox; existing member operators and unrelated call sites are preserved.

A standard rel_ops-style inequality implemented through operator== produced the right order, but the pointer iterator's by-value friend equality reversed load/register selection in the emitted comparison. Direct comparison of base pointers matches both 0x1C-byte routines. Making GetCollectionType inline removed its externally required symbol and failed to link. Using begin()+monsno in RecordCollection preserved its instructions but incorrectly moved begin and iterator-copy helpers ahead of GetCollectionType. None of those alternatives is retained.

### Final validation

Ran python configure.py and rebuilt all 156 configured game source units with their generated Wii compiler commands. Every compilation exited 0. As in the earlier sessions, direct compiler execution bypassed the stalled wrapper, with non-ASCII sources converted to CP932 only during compilation and restored byte-for-byte in a finally block. The final whitespace cleanup in vector was followed by recompiling CPiiCollectionBox.

The requested Ninja DOL build was attempted after deleting main.dol, but stalled without output and was stopped. No successful Ninja exit is claimed. The configured linker was invoked using the actual generated main.elf input list, Wii/1.1, -fp hardware -nodefaults, and build/wpse-v1/ldscript.lcf. Its response file was outside the repository and explicitly selected src/game/CPiiCollectionBox.o. main.dol was deleted again before that link. Linker and dtk elf2dol both exited 0; only the pre-existing FORCEACTIVE warnings remained.

The final prescribed command returned PASS main.dol and PASS: 1 declared artifact(s):

`python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`

- Final DOL size: 4,094,880 bytes.
- SHA-256: ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da.
- Live unitdiff: 43/43 functions at 100%.
- seq.py over 0x800E1AD0..0x800E5084: one equal sequence of all 43 functions.
- unitcmp.py over that range: no target-only functions, no source-only functions, and no moved function.
- Refreshed report.json and ran the supplied cnt100.py against the saved starting report: 12,983 -> 12,984. No per-unit count decreased; CGameSession changed from 208 to 209, and CPiiCollectionBox remains 43.
- git diff --check passes. No original hashes or split boundaries changed. All changes are uncommitted.


## 2026-10-22: ScriptGame and ScriptWorld register-allocation follow-up

Worked only in the current `rumble-a17` checkout. No commits, target-manifest changes, assembly, compiler-option changes, or retained shared-header changes. The requested two unit flips were not achieved. Two of the four named functions now match; both units remain `NonMatching` in `configure.py`.

### Retained source changes

- `ScriptGame.cpp`: replaced the single-option inline helper with `SetScriptOptions`, which owns the whole option loop. Within that helper, declare `label` before `text`, construct the option label string locally, and call `SetOption` directly. Declare and initialize `player` before `itemArgs` in `OpenScriptChoices`. Add `OpenOptionsWithTitle`, an inline wrapper around the final `GetMessageText` and `OpenOptions` call. `OpenScriptChoices` improves from 99.0991% to 100% (1,332 bytes).
- `ScriptWorld.cpp`: name `count - 2` as `lastColumnArg`. Declare it before `row`, assign it after `value.ToInt()`, and use it in `GetArg(lastColumnArg - i, value)`. `SetMapDescEnemyRow` improves from 99.27419% to 100% (248 bytes).

### Verified lessons

- The option-loop inline boundary is material to allocation. Moving the loop into `SetScriptOptions`, removing the former inner helper, and declaring `label` before `text` fixes the loop registers and frame slots. Declaring `player` before `itemArgs` fixes their r21/r20 assignment. The resulting function is 99.84985%; the final title conversion still exchanges r18/r20. The additional `OpenOptionsWithTitle` boundary fixes that last exchange without changing the emitted instruction sequence or frame layout.
- A named loop invariant needs separate declaration and assignment here. Naming `count - 2` immediately after `row` gives 99.67742%: descriptor and count are correct, but row and last-column argument exchange r26/r27. Declaring `lastColumnArg` before `row`, while leaving its assignment after `ToInt`, reaches 100%. Initializing the invariant earlier changes scheduling instead.

### Unresolved functions and additional checkout blockers

- `MakeMapID`: still 96.46342%. Target map/stage/area/engine are r31/r30/r29/r28; current stage/map/engine/area are r31/r30/r29/r28.
- `SetStageDescTarget`: still 99.710144%. Four instructions exchange the world/rank r30/r29 allocation.
- The handoff counts are not the complete current-checkout inventory. Recompiling the unchanged HEAD sources gives ScriptGame 167/170 strict functions at 100%, including an existing `IncMapClearedNum` miss at 99.49%. The retained source gives 168/170. ScriptWorld starts at 29/37 and ends at 30/37.
- ScriptWorld also has six unpaired target functions, totaling 552 bytes: `SetupScriptWaza__FP5CWazaiRCQ33std3tr122shared_ptr<9BWorldObj>`, `fn_8015ECE0`, `fn_8015ED30`, `dtor_8015ED60`, `dtor_8015EDF0`, and `fn_8015EE70`. No speculative symbol renames were retained. Full unit data, exception-table, and link acceptance still need attention before either promotion.

### Rejected experiments

All experiments below were reverted unless described under retained changes. These results extend the failed attempts in sprint-a13, sprint-r1, sprint-p1, sprint-t1, and sprint-w1o.

- Selectively expand each PopInt into a CValue/Pop/ToInt sequence: all seven nonempty subsets for SetStageDescTarget and all fifteen for MakeMapID. No improvement.
- Assign bitfields immediately after each pop, with uninitialized, empty-initialized, and explicit-zero MapIndex objects. Substantial regressions.
- Inline output-reference pop helpers, both forwarding PopInt and explicitly calling Pop/ToInt; all declaration permutations and nonempty helper-use subsets for SetStageDescTarget. No match. Also tested all declaration permutations with selectively explicit pops and ordinary assignments, without the helper.
- Selective const-int references, const ints, register ints, unsigned ints, and const-s32 references for popped components. No improvement.
- One- and two-level inline pop wrappers, including named return locals and explicit Pop/ToInt bodies, selectively applied to each component. No improvement.
- All 24 permutations of masked shift/OR packing for MakeMapID, as one expression and as compound assignments. No improvement.
- The existing CGameSession MakeMapIndex inline body, with local pop results or direct pop arguments, and helpers that combine packing with PushInt or SetStageDescTarget. No improvement.
- A four-component MapIndex constructor, using member initialization or field assignments. MakeMapID regressed to 89.902435%.
- Temporary MapIndex bitfield base types: unsigned int, long, unsigned long unchanged; signed/unsigned char and short regressed. A union exposing an integer mValue, including explicit whole-word zero initialization, did not help. All header edits reverted.
- Temporary ToInt and PopInt return-type combinations of int/s32/u32: unchanged. Header reverted.
- Component arrays, initialized from pops or assigned in either index direction: unchanged.
- SetScriptOption parameter-order/reference variations and moving the caller loop-index declaration did not solve OpenScriptChoices. A loop helper with the former inner helper retained required another forwarding level and was less clear than the retained single-loop-helper form. A named final title pointer did not fix r18/r20; the final-call inline wrapper did.

### Validation and build limitation

Initial whole-report count of `fuzzy_match_percent == 100`: **13,007**. Final regenerated report: **13,009**. Both source units were independently compiled from unchanged HEAD contents and then from the retained source, using the configured game compiler flags. Comparing the matched-function sets found no losses; the only gains are OpenScriptChoices and SetMapDescEnemyRow.

Used `python tools/re/unitdiff.py <unit> --function <mangled-name>` for initial instruction comparisons, and `--no-build` for comparisons after direct compilation. The final exact compiler commands emitted by `ninja -t commands <object>` were also executed successfully for both units. The ordinary Ninja build repeatedly stalled after compilation, including with `-j1`; only this checkout's stalled Ninja processes were stopped. Direct compiler and sjiswrap commands completed. The cause of Ninja's stall is unverified. `ninja -t restat` did not solve it because stored dependency metadata remained out of date. No successful full Ninja run is claimed.

For fresh artifact verification, deleted `build/wpse-v1/main.dol`, obtained the configured linker inputs through `ninja -t compdb -x link`, generated the normal linker response file, ran the emitted linker command (exit 0), and ran `dtk elf2dol` (exit 0). Removed the response file afterward. The link printed FORCEACTIVE-symbol warnings. Then ran:

```
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
```

Result: **PASS main.dol; PASS: 1 declared artifact**. This verifies the normal fallback build; neither script unit was promoted or linked from the changed source.

Regenerated `build/wpse-v1/report.json` with the emitted `objdiff-cli report generate -o build/wpse-v1/report.json` command (exit 0). It reported exception-table decoding warnings for method_layoutSetString and NPpu::ToUnicode. Final count remains 13,009. `git diff --check` passed. Shared-header contents were restored exactly, and their timestamps were reset to their last Git modification time to avoid rebuilding unchanged dependencies after temporary experiments. No scratch source files were created.


## 2026-10-22: single-function game-unit follow-up

Work stayed in the current rumble-a17 checkout. No Git-writing commands, assembly, target-manifest changes, code comments, or repository scratch files were used. Only MapPiiTable.cpp, configure.py, and this appended record are retained. The broader requested sweep is incomplete: only MapPiiTable gained a perfect function and was promoted.

### Retained result

MapPiiTable::Clear now scores 100%, and the unit is 29/29. A file-local inline SwapValues forwarding function calls vector::swap, while Clear retains ownership of the local empty vector and its original destruction point. The additional inline level exchanges the capacity and size temporaries from r5/r7 to the target's r7/r5. Data remains r6. The final helper uses an anonymous namespace and reference parameters rLeft/rRight. No shared header change is needed.

This extends the unsuccessful swap-operation permutations in sprint-r1: changing the caller's inline depth works without altering vector's storage implementation. One and three forwarding levels matched; two retained the original mismatch. The retained implementation uses one level. Reversing the swap receiver remains worse. Moving the empty vector into the helper changes destruction timing and fails badly; that is not an equivalent source model here.

### Baseline and validation

The starting report was stale (13,292 perfect functions). Ran python configure.py and attempted .venv/Scripts/ninja. Ninja and sjiswrap stalled without compiler progress. Rebuilt all 869 configured source compile commands directly, bypassing sjiswrap; non-ASCII source was temporarily converted from UTF-8 to CP932 and restored byte-for-byte in finally blocks. All 869 compiled successfully. Regenerated report.json over 1,388 units to establish the actual unchanged-source baseline: 13,303 perfect functions. In particular, CUIMenu already contains the earlier goto and integer-argument fixes; its fresh OnInput score is 99.89% and RunMenuList is already 100%.

After the retained edit, MapPiiTable was compiled using its configured Wii/1.1 command. Changed its configure.py entry to Matching, deleted build/wpse-v1/main.dol, and linked the generated input list with the configured linker. Converted the new ELF with dtk elf2dol. Link and conversion exited 0; frozen-manifest verification passed. Repeated after the final helper namespace/parameter-name cleanup. The final ordinary Ninja attempt again stalled and its own process tree was stopped; no successful Ninja exit is claimed. Direct configured compilation, link, and conversion succeeded.

Final verification command:

`python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`

Result: PASS main.dol; PASS: 1 declared artifact. SHA-256: ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da.

The final regenerated report has 13,304 perfect functions, up from the fresh 13,303 baseline. Comparing exact unit/function-name sets finds no lost perfect functions and exactly one gain: Clear__11MapPiiTableFv. Existing exception-decoding warnings for ToUnicode and method_layoutSetString remain. git diff --check passes. Changes are uncommitted.

### Rejected probes and remaining work

All probes below were restored, including their source objects. These supplement, rather than supersede, the prior notes in sprint-a15, sprint-a12, sprint-ida, sprint-r1, sprint-s1, sprint-cx, and sprint-w8cx.

- CGameEnding: one to three inline wrappers around the two list erases leave Calc at 99.75%. Separating declarations from assignments for the line/title iterators and loop index, in all seven nonempty combinations, does not fix the node register; moving title alone worsens it.
- CPiiPersonalData: caching the creation timestamp before various final field assignments and storing the random seed before the cached time does not match. Unsigned 64-bit composition, addition instead of OR, reversed OR operands, and multiplication by 2^32, with both timestamp/seed store orders, do not beat the original 97.08%.
- CBattleRoyalBox: named shared-pointer return locals, copy initialization, and an explicit copy-constructor return all give 85%; default construction followed by assignment is substantially worse. Keep the existing reference-local form at 89.33%.
- BModelObj: computing all four absolute extrema before the max comparisons, across all 24 declaration orders, regresses CalcSize to roughly 87.6%. None is retained.
- CEffect: a CreatePool member template containing both array allocation and pool construction does not solve InitializeEffect. Value and reference count parameters, and every nonempty proper subset of the four pool replacements with an int-reference parameter, remain below the original 99.62%.
- CUIList: moving the class destructor after Load, before Calc, or to the end does not remove the two dead branches from vector<shared_ptr<CLayout>> destruction (95.92%).
- Message: one and three outer inline allocation wrappers change tag slots and registers, worsening InitMsgResList to 99.72%; two levels retain the original swapped tags at 99.99%.
- CPlayerPii: pointer, const-reference, and pointer-reference helpers around the name table do not inline under the unit's current pragma state. A local dont_inline reset/on bracket did not change that result. These probes do not solve the scheduling problem and are not retained.
- CMiniMap: naming the camera-line subtraction temporary leaves CalcView unchanged at 93.75%; separate scalar subtraction from the object-position copy regresses it to 92.65%, with or without the named camera delta.
- CMoneyLifeDisplay: removing const from the constructor's MapDesc pointer, alone or together with the mode helper's pointer, leaves Create at 99.63%.
- GameUtil: expanding any combination of the reverb, delay, and chorus initialization helper bodies directly at their call sites leaves InitBgmFxPresets at 92.23%.

CDividedGround, CDlgMsgWin, CTitleSession, PhotoSave, BUI, CUIMenu, and errmsg were inspected against their current instruction differences and prior failed attempts; no new correction was established. Their previously documented load scheduling, empty-deleter copy scheduling, data ordering, exception-frame layout, induction-variable, saved-register, and prologue scheduling blockers remain. No other requested unit is promoted or claimed complete.
