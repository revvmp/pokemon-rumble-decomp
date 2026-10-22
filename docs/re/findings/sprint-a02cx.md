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
