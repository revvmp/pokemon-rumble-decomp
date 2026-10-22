# Sprint a02cx: pause menu, main, and home button tail

## 2026-10-22 results

Work stayed in the current checkout. Changes are uncommitted. Only this findings file receives documentation updates for this session, as requested. No target manifests, symbol configuration, shared headers, or Matching classifications changed. No scratch files were created.

### Retained source changes

- `src/game/CPauseMenu.cpp`: `ShowMenu` now directly initializes each block-local `UIMenuItem` aggregate for resume, screenshot, remote memory, and controllers. Removed the four separately declared aggregate locals and their copies. This resolves the eight frame-load operand differences in `Run`, including offsets `0x224`, `0x22C`, `0x2C8`, `0x2D0`, `0x300`, `0x308`, `0x370`, and `0x378`. Report score rises from 99.982025% to 100%. Strict unitdiff is 99.93259% (displayed 99.93%): six remaining differences are aggregate/data relocation labels, not instruction or stack allocation differences. Function size remains 1,780 bytes.
- `src/game/homebutton.cpp`: added the inline `LoadHomeButtonIcon(const char*)` helper containing the file load and assignment to `spIcon`; `InitializeHomeButton` constructs the path and calls it. This additional inline level corrects the exception landing-pad stack operands at offsets `0x6C4` and `0x6E4` to `0x68` and `0x80`, respectively. Both the report and strict unitdiff now give 100%, versus 99.995834% before. Function size remains 1,920 bytes.

### Validation

Ran `python tools/re/unitdiff.py <unit> --function <mangled-name>` for the requested functions. The initial Ninja-driven invocation failed to return promptly; its Ninja process was stopped. Subsequent compiles used the exact compiler command emitted by `.venv/Scripts/ninja -t commands build/wpse-v1/src/game/<unit>.o`, followed by unitdiff with `--no-build`. All retained source compiles exited zero. This follows the previously documented workaround in [sprint-r1](sprint-r1.md#build-validation-and-limitations); this session does not establish a general cause for the Ninja symptom.

Regenerated `build/wpse-v1/report.json` with `tools/local/objdiff-cli.exe report generate -o build/wpse-v1/report.json`. The report present at entry counted 13,236 functions with `fuzzy_match_percent == 100`; the report generated at this validation step also counts 13,236, so the requested count does not drop. To establish a source-controlled baseline rather than rely on existing build artifacts, temporarily compiled the HEAD versions of the two changed sources, generated a full report to stdout, restored the retained source bytes, recompiled, and generated another full report to stdout. Those fresh baseline/final reports count 13,234 and 13,236 perfect functions, respectively. Across all 1,388 units, exactly the two functions above improve, no function regresses, and no previously perfect function loses 100%. All experiment sources were restored before this comparison.

The report generator emits existing exception-table decoding warnings for `method_layoutSetString` and `NPpu::ToUnicode`. These do not prevent report generation; no changes to those units were made.

No whole unit reaches 100%, including its data and exception sections. Therefore no unit was promoted in `configure.py`, and no fresh full-DOL verification is claimed. The two function-level improvements do not establish whole-unit source linking or exact executable reconstruction.

### Remaining functions (final report)

| Unit | Function | Fuzzy match percent |
| --- | --- | --- |
| CPauseMenu | LeaveStage | 98.72596 |
| CPauseMenu | TakeScreenshot | 99.83075 |
| main | HandleFileError | 96.6129 |
| main | main | 88.82936 |
| homebutton | InitializeHomeButtonInfo | 98.73276 |
| homebutton | UpdateHomeButton | 96.668175 |

### Rejected experiments and traps

- `LeaveStage`: by-value `GetHubIndex` worsened strict score to 98.23077%; an explicit two-integer helper, separate area/mode locals, and deferred field assignment did not fix evaluation/register ordering. Restored original helper.
- `TakeScreenshot`: predeclaring EFB dimensions, the render-mode pointer, or picture pointer did not correct register allocation. Removing explicit inline from `CaptureScreenshot` left an out-of-line call and reduced the strict score to 2.6149507%. Restored all these trials.
- `HandleFileError`: combined range guards, logical expressions, nested conditions, range switches, a separate inline range predicate, and a const-reference parameter did not match. In particular, combining the first two guards lets the compiler eliminate comparisons known from the caller's outer switch, whereas the target retains them. Best retained source remains the original separate guards; all trials reverted.
- `main`: setter assignment (`mErrorHandler = rHandler`) leaves main at 88.82936%; temporary-and-swap does not fix main and regresses the setter; `std::swap` substantially regresses both. Restored the original setter. No dead statements or forced compiler controls were introduced to suppress inlining.
- `InitializeHomeButton`: naming the loaded shared pointer, calling `OpenFileData` directly, and swapping the returned pointer into `spIcon` failed to reproduce the landing-pad layout. The narrow inline assignment helper succeeds without changing the string lifetime or assignment semantics.
- `InitializeHomeButtonInfo`: wrapping its full body in an explicit inline helper, with all six parameter orders, gives only 24.275862% strict score. Moving this large body across an inline boundary is not a register-only change. All six variants reverted.
- `UpdateHomeButton`: separate clamped expressions, a Vec2 movement local, an explicit scale local, and a predeclared Y component did not improve the function; all reverted. Its diff also contains control-flow and accessor/callee differences beyond the initial float-register mismatch.
- For `Run`, direct block-local aggregate initialization matters: the removed outer aggregates created redundant constant copies and reversed the loaded slots. Do not chase the remaining data relocation labels as stack mismatches; compare both strict unitdiff and report scores.

### Final report observation

A subsequent read of `build/wpse-v1/report.json` counted 13,238 perfect functions, without another report-generation command or source change by this session. The cause of that report update was not established. The three investigated units retain the scores listed above. Credit this session only with the two improvements established by the controlled recompile comparison, not the additional report-count movement. The latest observed count still exceeds the entry count of 13,236. `git diff --check` passes, and the only worktree changes are the two source files and this findings file.


## 2026-10-22: CPiiListFilterSort follow-up

### Retained changes and evidence

- `CompareSpeciesOrder` reaches 100% in both strict unitdiff and the report, from 96.268654%. The existing personal-data BP-chain helper now returns `bool`, matching its actual predicate result. An inline shared-pointer adapter (`CompareItemBP`) calls it, and the species-order comparator uses that adapter for its tie-break. Both the boolean result type and the additional inline level matter. The other comparators retain their baseline scores.
- `ShowTypeFilterMenu` was missing its cancel-path `onChanged(this)` invocation. The target contains this invocation at function offsets `0x838` through `0x88C`. Restoring it reproduces the target's 0x260-byte frame and fixes the stack offsets; report score improves from 96.55621% to 99.94083%. Strict unitdiff is 99.93343%, with six shift-register operand differences and the existing placeholder relocation-name difference remaining. This was a missing behavior, not simply a frame-allocation problem.
- No shared header, target manifest, symbol configuration, or Matching classification changed. The unit still has nine report-level non-perfect functions. `ShowOriginalTrainerFilterMenu` and `ShowSortTypeMenu` were already report-perfect at this session's baseline; their strict mismatch is the placeholder symbol at `0x804B9028`.

### Validation and limitations

The first command was `git reset --hard main`. It failed because the linked worktree's index is under the main repository outside the writable sandbox. Read-only checks established a clean checkout with both HEAD and main at `36186eb9eb4f307679bab4cd6f8023ca9a755747`, so the requested starting contents already held. No commit was attempted.

`python configure.py` exited zero. A normal `.venv/Scripts/ninja` invocation and a later `-j 2` retry produced no output before being interrupted; neither establishes a successful full build. Used the exact compilation command from `.venv/Scripts/ninja -t commands build/wpse-v1/src/game/CPiiListFilterSort.o`, as in the earlier session's workaround. The source compiles exited zero, and comparisons used objdiff-cli and `tools/re/unitdiff.py --no-build`.

For a controlled baseline, temporarily compiled the HEAD source, generated a full report to stdout, restored the retained source bytes, compiled again, and generated a second full report. Counts across 1,388 units are **13,284 to 13,285** functions with `fuzzy_match_percent == 100`; no previously perfect function loses 100%, and exactly the two functions listed above change score. The report present before this comparison counted 13,286 and was stale relative to freshly generated reports; do not use it as the source baseline. The retained report is regenerated at `build/wpse-v1/report.json`. The existing exception-table warnings for NPpu ToUnicode and method_layoutSetString persist.

The unit remains NonMatching, with data and exception sections also unresolved. No fresh full-DOL verification or whole-unit completion is claimed. All experiments used in-memory source backups and restored rejected edits; no scratch files were created. Only this findings file receives appended notes.

### Rejected experiments and reusable findings

- Shared-pointer `.get()`, dereference syntax, and dereferencing the personal-data shared pointer do not change the BP comparator registers. Replacing the BP scalar comparison with the reference comparison does not fix those comparators and regresses CompareFavoriteFirst; reverted.
- Wrapping the full BP comparators in raw-item or raw-personal-data helper functions worsens the standalone comparators. The personal-data helper nevertheless revealed the matching SpeciesOrder inline structure. Isolating that structure to the species-order path avoids those regressions.
- Calling the BP-chain helper with its old `int` return type scores only 93.0597% directly and 93.731346% through one adapter. With `bool`, a direct call scores 99.32836%, and one shared-pointer adapter reaches 100%. Additional int-returning adapter levels do not help. Calling the chain directly from CompareSpeciesName lowers its score to 98.776596%; reverted.
- Sort: named begin/end iterators, a named comparator, individual begin/end locals, a const comparator table, an unsigned index, explicit comparator construction, and pointer-arithmetic table access do not reproduce the post-begin scheduling. The const table substantially worsens the function; all reverted.
- ShowPrefixFilterMenu: an unsigned upper-bound comparison preserves two range branches but changes the upper compare to `cmplwi` (99.69768% strict). Early negative/upper rejection with duplicated callback bodies scores 92.95349%; a switch wrapper still folds the range. Reverted all variants; baseline remains 98.76744% in the report.
- ShowTypeFilterMenu: compound shift assignment, const-reference/int/unsigned/long kind parameters, a mask argument, a separate mask-return helper, an item reference, and a separate item-kind accessor all retain the same swapped shift registers. Passing the whole item by value or using a short kind regresses the function. Reverted these changes. The six remaining instruction differences are `li`, `lwz`, and `slw` at offsets `0x188/0x190/0x1A0` and `0x210/0x218/0x228`.
- The UIMenuItem sorts and shared-pointer comparator-reference sort retain the register-allocation problems documented in sprint-r1 and sprint-cx. No shared algorithm/header change is retained or credited.

Final recheck: after the second Ninja interruption, the generated total changed again. Repeating the controlled HEAD-source versus retained-source compilation and report comparison gives 13284 to 13285 perfect functions, with 0 perfect-function losses and only the same two CPiiListFilterSort score changes. Credit this work with one new perfect function. Changes to other pre-existing object artifacts between report generations are not credited; their cause was not established. Final git diff --check passes.


## 2026-10-22: library straggler continuation

No new function reached 100%. All source experiments were reverted byte-for-byte; no Matching classification, header, symbol, split, or target-manifest change is retained. Work stayed in the current directory, used no Git write commands and created no scratch files. This entry is the only retained repository edit. The requested library completion remains unfinished.

### Rejected controls

- TMCJPEGSetupRGB565: replacing height alignment with a named signed height quotient followed by compound addition of the remainder predicate changed register allocation and reduced 99.830505% to 89.41%. Reverted. No claim is made for the unchanged RGBA8 setup or converters.
- ArchiveFontBase ConstructOpAnalyzeGLGR: deferring adjustment-table pointer construction until after the remaining-space rejection changed the block-end and sheet-count registers and reduced 99.291664% to 94.77%. Reverted.
- TextWriterBase PrintToDisplayListImpl: moving the z-coordinate read after GetBaselinePos, with either declaration at that point or an earlier uninitialized declaration, changed the constant-register allocation and reduced the char/wchar scores to 97.57914%/97.6177%. Reverted both forms; restored scores are 98.48921%/98.513275%. This confirms the existing declaration-order limitation rather than solving it.
- Both baked animation executors: lifting the fitted lifetime temporary declaration into the outer scope left 99.739334% unchanged. Hoisting its initialization as well reduced both to 96.72986%. Reverted. The eleven remaining register-operand differences are still present.
- Read the prior flag sweeps, converter permutations, particle-manager frame issues and draw-strategy switch findings. No new source candidate was retained for the remaining requested routines.

### Validation and tooling

`python configure.py` succeeded. Normal Ninja and a verbose single-job retry stalled without output and were interrupted; neither is a passing build. Running its configured `dtk dol split` command directly succeeded. Individual ASCII source controls used generated compiler commands and live unitdiff, and the original sources were recompiled after restoration.

The report initially on disk counted 13,291 perfect functions. Regenerating it after restoration and a fresh split counted 13,286 while every requested function retained its initial score and Git showed no source differences. These cached report totals are not a controlled source-change comparison. A full configured-source rebuild was started to resolve the stale-object discrepancy.

### Final validation

All 869 configured source compilation commands completed with zero failures when run with inherited output, retaining sjiswrap and the configured compiler flags. The earlier captured-output attempt stalled and was interrupted. This is an observed invocation difference, not a proven general cause of Ninja stalls.

After deleting build/wpse-v1/main.dol, the generated linker command and elf2dol command both exited zero. The linker response file was generated from Ninja compdb's expanded link inputs in its normal configured build location. Frozen verification passed with:

```text
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
```

The final refreshed report counts **13,303** functions with fuzzy_match_percent equal to 100, compared with the initial cached 13,291. This increase is from rebuilding existing source artifacts and is not credited as reconstruction work. No requested function gained 100%, and no unit was promoted. Only this findings file is modified. The normal Ninja entry point remains unvalidated in this session; the successful validation used its generated compilation and link commands directly.


## 2026-10-22 CLayout remaining-functions follow-up

### Retained result

`CLayout::CLayout()` (`__ct__7CLayoutFv`, 852 target bytes) improves from 96.938965% to 100% in both report.json and strict unitdiff. `GetDefaultPixelScale()` now returns `const nw4r::math::_VEC2&` to the cached POD scale. The NW4R `VEC2(const _VEC2&)` conversion constructor copies its two float members. Together these produce the assignment temporary directly, removing the extra eight-byte temporary and six copy/reload instructions. Returning the cache by reference without the conversion constructor was not the earlier attempted by-value VEC2 return: the return type and conversion point both matter.

Only `src/game/CLayout.cpp`, `include/nw4r/math/math_types.h`, and this appended note are retained. No comments, assembly, optimizer pragmas, target hashes, symbols, or Matching classifications were added or changed. Changes remain uncommitted. CLayout remains NonMatching, with 159 of 171 report functions at 100%; the remaining requested routines are not solved.

### Controlled validation

- Read CODEGEN_RULES, sprint-ida, and prior CLayout notes, including sprint-a02, sprint-a03, sprint-a05 and the relevant t1/other sprint references.
- `python configure.py` exited zero. Normal Ninja, verbose/TTY retries, and an explicit job-limit retry stalled without build output and were interrupted. These are not successful Ninja builds. As already recorded above, used the exact configured commands from `ninja -t commands` instead, preserving sjiswrap and compiler flags.
- Recompiled all 239 objects whose Ninja dependency records contain `math_types.h`, with four concurrent independent compiler processes. Repeated this entire affected-object sweep with the original header/source and then the retained header/source. Every compilation exited zero. No previously perfect function was lost; the only new perfect function in the controlled comparison is the CLayout constructor.
- The report at entry counted 13,311 perfect entries and included stale objects. After rebuilding, a final original-source control counts **13,320**, versus **13,321** for the retained candidate. Do not credit the stale-object refresh as reconstruction progress. An intermediate comparison keyed by `(unit, function name)` printed 13,318 versus 13,319 because duplicate names collapse two entries; the final counts above count every report function entry, consistently with the task.
- Deleted `build/wpse-v1/main.dol`, generated the normal linker response file from Ninja's expanded `compdb -x link` inputs, and ran the configured linker and elf2dol commands. Both exited zero. `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exited zero with `PASS main.dol`. This verifies the configured source/fallback selection; CLayout itself still uses its original fallback object for linking.
- Regenerated `build/wpse-v1/report.json` after restoring the retained sources and recompiling CLayout. Final raw perfect-entry count: 13,321. Strict unitdiff gives the constructor 100%. `git diff --check` passes. No scratch files were created; compiler outputs and the linker response file are normal build artifacts.

### Rejected experiments and remaining evidence

All experiments below were restored; no partial-score source changes were retained.

- Constructor: a named VEC2 return object, const-qualified VEC2 return, and const named return do not remove the copy. Returning `_VEC2` by value with the conversion constructor scores 97.92488%. Both const and non-const `_VEC2&` returns score 100%; retained the const reference because callers only read the cache.
- Archive-array helper: extracting the append loop into an inline `AppendArchives` helper gives 94.27778% versus 94.35185%. Per-function scheduling-off, propagation-off, common-subexpression-off, lifetime-off, dead-assignment-off and pointer-analysis-off trials do not reach 100%; scheduling-off falls to 65.4815%. The array exception constructor's remaining diff is actually its final shared_ptr copy: the target loads/stores the pointer before loading the control block, whereas ours hoists the control-block load and null comparison.
- Pointer-set insert: returning the native pair through the converting pair constructor, with an implicit tree-iterator conversion, raises Attach/AttachLayout only to 99.34217%/99.363%; FindPaneMP falls to 92.76515%. Passing the converting iterator by value, including its const-iterator conversion, does not help. Explicit `tr1::get<0/1>` access raises FindPaneMP to 94.867424%, but Attach/AttachLayout remain 99.3253%/99.34426%. Const pair results and named references do not solve the tag/iterator slot arrangement. These are partial structural clues, not accepted changes.
- DetachAttachedLayouts: making the copied children set const gives 97.721%; caching end() gives 96.966%, both below 98.37267%.
- RebindAnimations: moving the map iterator into the for initializer lowers ReplaceMotions/AdvancePaneMotions to 95.075%/93.220%; moving the child iterator into its for initializer gives 99.092%/95.130%. Both are worse than 99.47369%/95.41799%.
- CalculateAttachedMtx: the target really contains `cmplw r31,r31`, and reloads the attach-pane visibility. A local common-subexpression-off probe raises 94.93535% to 97.72845%; propagation-off gives 95.409485%, both together 94.905174%, and adding dead-assignment-off does not improve it. Rebuild-conditionals-off loses; control-flow, tree-transformation and scalarization-off do not help. No optimizer pragma is retained. A source-level explanation for the target's redundant compare remains unresolved.
- StopMotion still lacks the kept map-result iterator and begin temporaries described in a05. FindAnimation still differs in frame/register allocation and two reloads. This session does not establish a new solution to those previously documented gaps.
