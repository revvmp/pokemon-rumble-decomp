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
