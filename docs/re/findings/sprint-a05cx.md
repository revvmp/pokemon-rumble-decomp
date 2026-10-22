# Sprint a05cx: PiiSpeciesData unsigned decision tree

## Result

Replaced the switch in `src/game/PiiSpeciesData.cpp::IsValidPiiForm` with thirteen sequential early-return `if (species == constant)` checks, preserving the original case order and form limits. The selector is the existing `u16` parameter, with no cast. The default remains `return form == 0`.

CodeWarrior Wii/1.1 with the existing game flags turns this linear source into the exact balanced decision tree at `0x80124CA0` (572 bytes), including all thirteen `cmplwi` instructions. No compiler flags, headers, symbols, splits, reference artifacts, or target manifests changed. This is a plausible source reconstruction, not proof of the original source spelling.

`game/PiiSpeciesData.cpp` is now `Matching` in `configure.py`.

## Validation

- Baseline: IsValidPiiForm 94.545456%, with only thirteen signed-versus-unsigned comparison differences.
- Final: objdiff-cli through `python tools/re/unitdiff.py game/PiiSpeciesData --no-build` reports 21/21 functions at 100%, 1168/1168 function bytes, and the entire 1280-byte `.text` section at 100%. Exception sections and `.sdata2` also match. The `.data` section remains 25 source bytes versus 32 target bytes including padding; full-link verification below accepts the resulting bytes.
- `python configure.py` exited 0. Repeated `.venv/Scripts/ninja` attempts stalled without producing output and were stopped. Direct compilation using the configured Wii/1.1 compiler and game flags exited 0; each experiment used a freshly compiled object before the no-build diff.
- Deleted `build/wpse-v1/main.dol` before rebuilding. Executed the configured linker directly with the ordered object inputs from generated `build.ninja`, including `build/wpse-v1/src/game/PiiSpeciesData.o`, followed by the configured `dtk elf2dol`, `dtk shasum`, and objdiff report generation commands. All exited 0; shasum reported OK. The linker printed FORCEACTIVE warnings for other units.
- `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exited 0: `PASS main.dol`, one declared artifact.
- Re-ran `python configure.py` to restore the normal generated build file after tooling diagnostics. No persistent build-tool workaround is included. Ninja itself did not complete successfully during this session; the successful validation is the direct configured compile/link/conversion pipeline and exact artifact verification.

Verified main.dol SHA-256: `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

## Traps

- A CodeWarrior decision tree need not originate in a switch. Here, sequential equality checks on the original `u16` parameter lower to the target unsigned tree. Both independent `if` statements and an `else if` chain match; unsigned literal suffixes are unnecessary. Casting the parameter to `u32` in those comparisons instead reproduces the signed tree and the original 94.55% score.
- Tested switch selectors using u16, u32, unsigned long, an enum with a large unsigned enumerator, local variables, and inline helpers still produced the signed tree. Case suffixes and case casts did not fix it. The successful change requires neither a hand-written binary tree nor artificial sentinel cases.
- Ninja stalled in this session even with a separate log directory and redirected output. The cause was not established; do not generalize this into a compiler or project limitation. Direct tool execution succeeded. No original executable bytes were used to replace this source unit.

## 2026-10-22: ScriptEffect integer rounding helper

### Result and evidence

`game/ScriptEffect.cpp` is now `Matching`. Added the local inline `int CeilFrames(f32 frames)`, returning `ceil(frames)`, and used it for `GetWazaRangeEffectParam`'s emission-span assignment. This is a plausible rounding helper, not a claim to have recovered its original name. The conversion to integer inside the inline return changes register allocation: the five range-effect callers now put the enabled result in r0 and the converted frame count in r3, exactly as the target does. All five callers rise from 99.78–99.86% to 100%; the other sixteen functions remain at 100%.

| Unit | Before | After | Configuration |
| --- | --- | --- | --- |
| game/ScriptEffect | 16/21 | 21/21 | Matching |
| game/ScriptCameraMotion | 26/30 | 26/30 | NonMatching |
| game/ScriptLightMotion | 19/24 | 19/24 | NonMatching |
| game/ScriptLayout | 14/15 | 14/15 | NonMatching |

The regenerated `build/wpse-v1/report.json` contains 12,789 function entries with `fuzzy_match_percent == 100`, versus 12,784 in the saved starting report. Comparing function scores by unit and symbol found only the five ScriptEffect improvements and no decreases. Raw entry counts are used here; collapsing duplicate unit/symbol keys gives 12,780 before and 12,785 after.

`python configure.py` exited 0. Ninja again stalled without output and was stopped. Experiments used direct execution of the configured sjiswrap/Wii 1.1 compiler command with unchanged game flags, followed by objdiff. After setting ScriptEffect to Matching, deleted `build/wpse-v1/main.dol`, ran the configured linker with the ordered inputs from generated `build.ninja`, and ran `dtk elf2dol`; both exited 0. The linker emitted FORCEACTIVE warnings as in the earlier session. The frozen-manifest command exited 0 with `PASS main.dol`:

`python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`

Verified SHA-256: `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This verifies only the manifest's main-executable scope. No target manifests, compiler flags, shared headers, symbols, or splits were changed. All unsuccessful source experiments were restored. Changes remain uncommitted.

### Traps and remaining mismatches

- **Move the conversion into the inline return, not merely the surrounding bool.** `CeilFrames` returning `int`, `short`, or `unsigned short` makes all 21 ScriptEffect functions match; an unsigned-int return changes conversion code and regresses the range callers to roughly 94–96%. The retained int form avoids unsigned conversion semantics. One through four extra bool forwarding layers and a bool output-reference helper leave the original five register differences unchanged. This extends sprint-r1's inline-return allocation observation to integer conversion results.
- **Camera value-copy forms do not fix the early frame-address hoist.** Const-reference ToString forwarding helpers, reversed helper argument order, and one through four forwarding layers preserve the baseline mismatches. A by-value conversion helper regresses all seven string-pop callers. Scratch-header experiments with a pointer cursor, explicit field copies, a post-decrement index, an empty Slice constructor, explicit assignment, and explicit copy construction do not improve the four unmatched functions. Separating the stack decrement or making an intermediate CValue copy regresses other functions. No header change was retained.
- **Layout wrappers can improve a score by leaving a call uninlined.** LoadFileList wrappers scored 97.69–98.45%, but inspection showed a call to the wrapper and missing constant setup, not a solution. False-returning inline helpers at depths one through four, direct shared_ptr construction, and binding the result to a const reference retain NewLayout's 95.98% mismatch. Default construction followed by assignment regresses it to 74.52%. The original constant-copy peephole issue remains unresolved.
- **LightMotion has more than the length-query mismatch.** Its play/request/requestNext functions have the same early-address problem as CameraMotion, and one 340-byte target helper remains unpaired with the source's PopLightMotion symbol. It remains 19/24, rather than being one function from completion.

## 2026-10-22: Near-miss register and stack-slot sweep

### Retained changes

No new unit was promoted to Matching. Five function scores improved, including two new 100% functions in the project-wide report. All edits remain uncommitted.

| Function | Starting report | Final report |
| --- | --- | --- |
| uhf_msc_blk_pread | 98.809525% | 99.404760% |
| uhf_msc_blk_pwrite | 99.146340% | 99.349594% |
| CTextWorldObjStat::CalcView | 98.817210% | 98.870964% |
| FormatDate | 99.989670% | 100% |
| FormatTime | 99.913700% | 100% |

- `src/RVL_SDK/fa/puh_msc_blk.c`: declare the block count before the other transfer locals, order the remaining locals by their target registers, and name the read-side byte count used by both memcpy and the buffer advance. Both functions now differ only in the interchange of r26 and r27 for the unit pointer and block count. No helper or compiler change is retained.
- `src/game/CTextWorldObjStat.cpp`: split the x scaling into two compound multiplications. This corrects a multiply operand order; the floating-point register allocation remains unresolved.
- `src/game/Message.cpp`: use string assignment instead of explicit assign for the four concatenation results, and declare pMeridiem before hourCycle. Assignment corrects the string iterator-tag stack slots; the earlier declaration gives pMeridiem its target register. InitMsgResList remains 99.987340%, with its two exception-related bool stores at interchanged stack offsets.

The current checkout already had all 45 pf_file functions at 100% and the unit marked Matching. Fresh compilation confirmed PFFILE_GetSFD at 100%; neither its source nor its configuration was changed. CUIPiiSpec::SetPii remains 96.655520% in the project report (96.404686% in isolated unitdiff), and the SeqSound constructor remains 94.871796%.

### Validation

Saved the starting report outside the repository before experiments. Raw function-entry counts with fuzzy_match_percent equal to 100 rose from **12,803 to 12,805**. Comparing every unit/symbol score found only the five improvements listed above and no regressions or lost perfect functions. Deduplicating unit/symbol keys yields 12,799 before and 12,801 after; those are not the raw entry counts requested for this sprint.

`python configure.py` exited 0. Ninja stalled without output and was interrupted. Every experimental object was compiled through its exact configured command from `ninja -t commands`, followed by objdiff; rejected experiments were restored and recompiled. Final compilation covered all six requested units. No shared-header, flag, symbol, split, or target-manifest change remains.

Deleted `build/wpse-v1/main.dol`, then executed the configured Wii/1.1 linker using the ordered explicit object inputs in build.ninja and executed `dtk elf2dol`. Both exited 0. The linker emitted FORCEACTIVE warnings consistent with the earlier sessions above. Generated a real objdiff report outside the repository, checked the regression invariant, and copied that generated report to build/wpse-v1/report.json. This is the normal project report, not a fabricated or edited measurement.

The requested command exited 0:

`python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`

Result: `PASS main.dol`, one declared artifact. SHA-256 remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The improved units remain NonMatching and therefore still use their original-object fallbacks in the link. This verification does not claim that those entire units can be linked from source. `git diff --check` passed.

### Traps and rejected approaches

- Isolated unitdiff and the project report do not give identical scores for these game units. FormatDate and FormatTime retain relocation-related differences in isolated unitdiff (99.879520% and 99.887430%), while the project report resolves them to 100%. Do not mistake those isolated relocation rows for remaining instruction or stack-slot differences, or promote Message while InitMsgResList remains unmatched.
- Returning the transfer count from a small helper does not solve the r26/r27 interchange. Expanding the unit search in the callers instead introduces the r4/r5 index/cursor swap recorded in sprint-r1. Output-pointer search helpers, definition order, and a register-qualified block count did not solve the pair. The retained declaration order and named byte count produce the gains without these helpers.
- SeqSound: all 63 nonempty subsets of the six scalar initializers moved into the constructor body failed to beat the original constructor. Moving the SeqLoadTask definition earlier, assigning its fields in its body, explicitly value-initializing SeqPlayer/SeqLoadTask, and removing both volatile qualifiers also failed. Both qualifiers and the original constructor remain intact.
- CalcView: local coordinate copies, inline bounds/identity helpers, all sixteen comparison-operand permutations with the split scaling, separate y negation, and y-first scaling failed to produce a match. The retained split x scaling is the only improvement from these experiments.
- CUIPiiSpec: scoped named FormatString results and local aliases for this/pii did not solve the register allocation; restored all changes.
- InitMsgResList: a named raw pointer, shared_ptr assignment, temporary/named shared_ptr swaps, and an allocation helper failed to beat the original. The vector interface in this checkout does not support the tested explicit allocator/value constructor forms. Restored the original initialization.

Scratch scripts, saved reports, and the link response file were kept in the system temporary directory; no scratch directory or experimental artifact was added to the repository. Documentation changes are restricted to this appended section.
