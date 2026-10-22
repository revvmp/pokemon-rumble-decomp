

## 2026-10-22: dialog register order and initialization

Work remained in the current checkout, with no commits, target-manifest edits, shared-header edits, or repository scratch files. Modified sources are CDlgCollection.cpp, CDlgInfo.cpp, and CDlgMsgWin.cpp. No complete unit reached 100%, so configure.py was not changed and no Matching promotion or new full-DOL verification is claimed.

### Proven source forms

The Collection Draw/CalcView/CalcMtx family needs two inline boundaries: the dialog calls an implementation forwarding method, which calls the item-model member containing the loop. A direct dialog loop assigned the implementation pointer to the wrong register. Moving only the loop to the model fixed that pointer but swapped the loop index and model pointer. The implementation forwarding methods give the target's index r31, model r30, byte offset r29, and implementation r28 exactly. The same structure fixes Info Draw and Calc. Collection Calc also benefits, though it has additional allocation differences.

Collection Show is now 100% in both unitdiff and report.json. Its initialization goes through a dialog Initialize member into the implementation Init member, restoring the stack-object ordering. GetModelCount on the item model gives the target's implementation/per-row register allocation. Collection totals belong to mList's layout, not mRootLayout; GetLayout()->SetText reproduces the argument scheduling, whereas the BUILayout::SetText forwarding call delays the receiver load until after c_str. The row-building loop evaluates species.begin() independently for dereferencing and erasing. Retaining a named iterator carried the old pointer through push_back and produced the wrong frame stores and reload.

Info Show needs the same initialization boundary and a named shared_ptr<CDlgInfoItem> before push_back. The named item causes the constructor's model argument to be destroyed before the push_back conversion, exactly as in the target. Replacing the initial early return with an enclosing if (!mImpl) also matches the target control flow. All remaining strict unitdiff differences are relocation names.

Info GetPage was behaviorally wrong: it constructed every newly added vector entry. The target resizes the vector, then creates only mPages[page] if that specific entry is empty. The corrected function is report-100%; strict unitdiff differences are only relocation names.

Info Update now measures text width through GetTextWidth(TextBox*), whose local Rect is returned as a float width. Together with std::max(width, widths[j]), this restores the target frame slots and the local maximum-selection sequence. Register allocation and the final column-width floating-point expression still differ.

Collection Calc now lives on the item-model class and is reached through CalcModels on the implementation. GetFormMask(mask, form) expresses the repeated per-form mask operation and gives the two inline return values the target's higher register priority. The model pointer versus strength-reduced index ordering and the texture receiver loads remain unresolved.

MsgWin Calc needed reversed source branch order for yes/no sounds: nonzero cursor plays 0x5a; otherwise 0x9. The target uses that order. Report reaches 100%; unitdiff retains only local string relocation-name differences.

PiiSelector Draw, CalcView, CalcMtx, and Calc were checked individually with unitdiff and already score 100%. No PiiSelector changes were necessary. The report scan found no other unmatched dialog rendering-family instances besides those investigated here and MsgWin Calc.

### Measured results

The initial report contained 12,980 functions with fuzzy_match_percent == 100. The final regenerated report contains 12,989, a gain of nine. Comparing every function score in the three modified units against the initial report found eleven improvements and no regressions.

| Unit/function | Initial report | Final report | Final strict unitdiff |
| --- | ---: | ---: | ---: |
| Collection Draw | 98.265305 | 100 | 100 |
| Collection CalcView | 98.26087 | 100 | 100 |
| Collection CalcMtx | 98.26087 | 100 | 100 |
| Collection Show | 96.998566 | 100 | 100 |
| Collection Calc | 98.89317 | 99.61471 | 99.61471 |
| Collection item constructor | 94.67593 | 94.67593 | 94.67593 |
| Info Draw | 98.289474 | 100 | 100 |
| Info Calc | 99.06593 | 100 | 100 |
| Info Show | 96.69624 | 100 | 99.91467 |
| Info GetPage | 97.3513 | 100 | 99.83271 |
| Info item-line Update | 97.676315 | 98.8421 | 98.61842 |
| MsgWin Calc | 99.96429 | 100 | 99.84694 |

Final unit report percentages: Collection 98.54455, Info 96.376724, MsgWin 99.72457. These are fuzzy object scores, not full-unit binary identity.

Validation used the pinned Wii/1.1 compiler with the corresponding build.ninja game flags, followed by python tools/re/unitdiff.py game/<unit> --no-build --function <full-mangled-name>. All three final source compiles succeeded. The normal unitdiff build invocation stalled in the sjiswrap path and was interrupted; direct compiler invocation was used for subsequent experiments. MsgWin contains Japanese strings, so its source bytes were temporarily converted from UTF-8 to CP932 for direct compilation and restored in a finally block. No encoded scratch source was left in the repository. Ninja still considers the directly compiled objects' stored dependency metadata out of date; this is not a completed Ninja build.

The final report was generated with tools/local/objdiff-cli.exe report generate -p . -o build/wpse-v1/report.json, exit 0. It covered 1,475 units. It warned about exception-table decoding in the unrelated NPpu ToUnicode and method_layoutSetString functions. git diff --check passed. The existing main.dol was not used as evidence for these NonMatching sources.

### Traps and remaining work

- One item-model helper alone is insufficient for this register family. Declaration reordering, unsigned-long loop indices, const model methods, reference receivers, and a static reference helper did not fix the last index/model swap. The implementation forwarding boundary is the distinguishing successful form.
- Flattening Init into Show exhausted the compiler's inline budget and severely reduced matching; do not confuse that result with a simple frame-slot permutation.
- Collection Show's named set iterator is a semantic access-pattern difference, not just a stack-slot swap: the target reloads begin after push_back.
- Collection's item constructor differs inside the vector<unsigned short> copy path. The target retains first and last across allocation and recomputes their difference; the current header reuses the earlier length. A range constructor is not currently supported by the unsigned-short __vec_constructor specialization. No shared-header experiment was retained.
- Further splitting Collection's model initialization into a helper regressed Calc. Adding another whole-loop forwarding layer also regressed it. A named model accessor, reference local, and &*shared_ptr access did not resolve the remaining model/index swap.
- Info Update: direct rectangle-width subtraction, direct HasText expansion, reversing min arguments, and moving the gaps declaration did not improve the retained form.
- Remaining requested work is Collection Calc and its item constructor, and Info item-line Update. The complete units also contain other unmatched library/template functions and data/exception-section differences. Do not promote them based on the newly matching individual methods.


## 2026-10-22: equal-constant calls use an integer loading flag

### Result

`game/ScriptLayout` now has 15/15 functions at 100% and is enabled as Matching. The final `main.dol` passes the frozen `config/targets/wpse-v1.json` verifier: 4,094,880 bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The report-wide count of `fuzzy_match_percent == 100` increased from 12,999 to 13,003. Work stayed in this checkout; changes are uncommitted. No manifests, original artifacts, code comments, assembly, or repository scratch files were added or changed. Session notes are confined to this file.

### Matching source and interface evidence

The loading flag is represented as `int`, not `bool`, through the resource-loading call chain. `NewLayout` itself retains `LoadFileList(&pEntry, pArchive, 0, false, nullptr)`: parameter conversion now yields a word-sized constant. At `NewLayout` (0x801434B0), offsets 0x114, 0x118 and 0x11C are respectively `li r6,0`, `li r7,0`, and `li r8,0`, followed by the same callee at 0x120. Its full 528-byte function compares at 100%.

Changing only the LoadFileList declaration is insufficient. An isolated int parameter gives NewLayout the exact instruction sequence but introduces `neg/or/srwi` boolean normalization inside the callee when forwarding to still-bool APIs. Propagating the integer type through LoadFiles, LoadLocalizedFiles, LoadFile, LoadResourceFile, LoadLocalizedFile, CreateFileData and WaitFileData removes those spurious conversions. The corresponding resource-list inline wrapper also needs the new signature. WaitFileData tests the argument against zero and already uses a full-register comparison. The exact original typedef spelling (for example, int versus a typedef of int) is not recovered; the retained int interface is supported by caller, callee and full-link evidence.

An additional discriminator is CPpuEngine::LoadScriptFile, which forwards an unsigned-long flags parameter to the localized resource loader. Removing the invented bool boundary removes its unwanted normalization and brings that function to report-100%.

The same correction applies to the last parameter of IPauseMenu::Create; its hideBackground parameter remains bool. Its implementation forwards the integer through CLayout and BUILayout loading methods. CUIList and CUIPiiSpec must retain matching virtual override signatures, and CUIPiiSpec forwards the value to CUIType::Initialize. All of these loading interfaces were updated consistently. The public and implementation signatures, forwarding declarations in BWorld, BWorldCamera, main and homebutton, and 21 corresponding names in symbols.txt were updated. Symbol addresses, sizes and other attributes were preserved. The unused RequestFile declaration was left unchanged.

Report gains are NewLayout, CPpuEngine::LoadScriptFile, IGameEnding::Create and IMultiRanking::Create. IGameEnding::Create and IMultiRanking::Create now score 100%. IGameResult::Create has the corrected pause-menu constant setup but remains 99.083015% due to other differences. CNoticeDisplay uses the corrected loading signatures but still has other differences; its perfect-function count remains 98. IPauseMenu::Create retains report-100%. All eight CModel::CreateAnimObj template instances remain 100%; their genuine bool arguments still use the matching merge.

### Full-link discoveries in ScriptLayout

The initial 15/15 promotion failed exact verification: the DOL was 32 bytes shorter, with the first difference at file offset 0x23. The discrepancy came from method_layoutSetString (0x801437A0), not NewLayout. Its original exception-table entry is 0x54 bytes; the __alloca reconstruction emitted only 0x38 bytes. Replacing the pointer allocated with __alloca by `wchar_t buffer[size + 1]` and enabling this unit's `-dialect c99` reproduces the dynamic-stack cleanup metadata without changing its instructions. Default C++ mode rejects the variable-length array; `-gcc_extensions on` alone also rejects it. `-dialect c99` and `-flag c99` both compile it and preserve all 15 perfect functions; configure.py retains the explicit dialect option.

After that change only three bytes differed, at DOL file offsets 0x3E4659, 0x3E465B and 0x3E465D (runtime 0x804B7A19, 0x804B7A1B and 0x804B7A1D). The target's wide fallback string is `L"???"`, whereas the reconstruction used `L""`. Correcting that fallback produces exact full-DOL identity. configure.py retains `Object(Matching, "game/ScriptLayout.cpp", extra_cflags=["-dialect c99"])`.

### Rejected experiments and traps

- ScriptLayout and CModel originally used the same Wii/1.1 compiler and game flags; no existing per-file peephole difference explained the discrepancy.
- Const or volatile qualification of the bool parameter, a same-type local function pointer, inline bool/int constant-return helpers, and a const-bool reference all retain NewLayout's 95.98485% result. A bool aggregate changes other scheduling but does not match.
- An unsigned-char flag still merges. Signed char and short introduce sign-extension instructions. An int flag gives the exact instructions, with only the expected signature relocation mismatch until symbols and the forwarding chain are updated.
- `-opt nopeep` regresses ScriptLayout to 4/15, `-O3` to 12/15, and `-ipa off` to 9/15. `-O4` without the scheduling suffix and deferred inlining retain 14/15. None is retained.
- Widening BUILayout's virtual Load parameters without updating CUIList and CUIPiiSpec stops those methods from overriding their base slots and regresses callers. Update the complete observed override chain. Widening CUIPiiSpec without CUIType::Initialize also inserts unwanted normalization. Those temporary regressions were corrected before final validation.
- Instruction equality does not establish exception-table or literal-data equality. The variable-length-array cleanup and three question marks were required for the Matching promotion. dtk/objdiff still warn about exception action 30 in method_layoutSetString, now identically present in target and rebuilt code; do not remove that metadata to silence the warning.

### Validation and build-tool limitation

Normal Ninja execution, including the build step inside unitdiff, stalled before producing a compiler command. Interrupted those attempts. Ninja's read-only `-t commands`, `-t compdb` and `-t deps` queries worked. Used the actual generated commands directly, including sjiswrap, with dependencies selected from the changed headers and sources. The final pass rebuilt 77 affected objects with four compiler processes; all exited zero. No source encoding conversion or alternate compiler flags were used beyond the retained per-unit C99 extension.

Ran dtk dol split and python configure.py successfully after symbol changes. Generated the link response file from the actual build.ninja link inputs, deleted build/wpse-v1/main.dol, ran the configured Wii/1.1 linker and dtk elf2dol, and checked each exit status before verification. Both exited zero. objdiff-cli report generation exited zero and covered 1,475 units. This is a completed build through the generated compile/link pipeline, not a successful Ninja invocation; Ninja's stored dependency timestamps may still require a rebuild on the next normal run.

Final function inspection used `python tools/re/unitdiff.py game/ScriptLayout --no-build --function NewLayout__FPQ24NPpu7CEngine` against the freshly compiled object, plus whole-unit inspection. Final acceptance used `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`, which reported PASS. No unrelated unit was promoted. GameResult and CNoticeDisplay remain NonMatching; their other mismatches are outside this correction.


## 2026-10-22: follow-up layout callers and Format register allocation

### Retained changes and verification

Worked only in the current checkout, without commits or repository scratch files. Notes are confined to this file. Frozen targets and originals were not edited. Initial git status was clean. Initial report had 13,047 functions with fuzzy_match_percent == 100; final regenerated report has 13,050, a gain of three. The final linker and elf2dol both exited zero after deleting build/wpse-v1/main.dol. The requested frozen-manifest verification passed: 4,094,880 bytes, SHA-256 ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da.

- NPpuLibrary: both Format overloads now compare at 100% with unitdiff. The wide overload declares pSpec, size and type in the outer parsing scope, with pIn after pSpec and size. The narrow overload moves type to the outer scope and declares pIn after arg, pOut and pLimit. These declaration/scope changes reproduce register allocation and scheduling without changing parsing behavior. All 27 report functions are perfect. configure.py now marks game/NPpuLibrary.cpp Matching; its full-link promotion passed exact verification.
- NIntl.h: str() uses mResult += mArgs[index - 1] and mResult += ch. The additional existing string operator forwarding layer produces the target iterator-tag temporary order. FormatAreaName is now report-100%; CYouWin::Init loses its extra stack-frame space and most register differences. Only CNoticeDisplay includes this header, confirmed with rg and the Ninja dependency database; no other source unit needs this header rebuilt.
- CNoticeDisplay: declare gAreaPaneNames before use and define it at the end of the translation unit. This prevents its accesses from being folded into the unit's pooled-data base and reproduces the separate lis/addi address setup in CSimpleText::Init. Declare eligible before mode while keeping its initialization at the original logical point; this reproduces CYouWin's mode/area/newRecord/eligible register assignment. The unit remains NonMatching, at 99/102 report-perfect functions.

Final report detail:

| Unit/function | Result |
| --- | --- |
| NPpuLibrary | 27/27 perfect; Matching; full DOL PASS |
| CNoticeDisplay | 99/102 perfect; NonMatching |
| CSimpleText::Init | 99.98441% |
| CFight::Init | 99.97561% |
| CYouWin::Init | 99.973305% |
| CDlgMsgWin | 217/218 perfect; NonMatching |
| shared_ptr_deleter<char, default_delete<char[]> > constructor | 62.962963%; scheduling differs |
| CTutorialMsg | 114/114 report-perfect; promotion rejected by exact verification |
| CMoneyLifeDisplay | 12/13 perfect; Create remains 99.2953% |

### Pause-menu callers

rg identifies CGameEnding, CGameResult and CMultiRanking as the requested three true/true-equivalent callers; CMultiRanking already spells its final argument as 1. CGameMap is a fourth caller using false/1. The previous correction is already present: IPauseMenu::Create(const MapDesc*, bool hideBackground, int background). The loading flag is int; there is no new evidence to widen hideBackground. Freshly compiled reports retain IGameEnding::Create and IMultiRanking::Create at 100%; IGameResult::Create is 99.083015% with the equal-constant setup corrected and other differences remaining. No additional interface or caller edit was needed.

### Unfinished work and rejected experiments

CTutorialMsg was actually promoted, configured, linked after removing main.dol, and run through the requested verifier. It failed at DOL offset 0x3A8F3: both artifacts are 4,094,880 bytes, with 6,559 differing bytes. The promotion was reverted. Its matched functions still have deferred emission-order differences: reallocate/erase_tail swap at rows 53/54, followed by move_construct_to_end versus append_realloc and later vector/allocator groups. String/data ordering also remains unresolved. Report-100% is insufficient to promote this unit.

CDlgMsgWin's remaining deleter constructor has the same instructions but different scheduling around the vtable store, the deleter byte copy and compressed_pair constructor arguments. Explicitly copying D in the member initializer, explicitly initializing the base, and defining the constructor outside its template class (with and without inline) did not change its 62.962963% result. All shared-pointer header experiments were reverted.

CMoneyLifeDisplay: passing MapIndex by value loads the descriptor before the vtable store but adds stack copies and changes frame size; passing a const MapIndex reference leaves the late load. Passing the computed display mode moves the entire mode calculation before the vtable store, which also differs. Computing the mode in the constructor body moves it past other member initialization. None is retained. The original pointer-taking constructor remains unchanged.

CNoticeDisplay still needs data/weak-function ordering. Direct diffs for FormatAreaName, CSimpleText::Init and CFight::Init now show relocation differences; the tiny non-perfect report scores on the latter two remain. CYouWin::Init additionally uses lbz r0 / extsb r25,r0 where the target uses lbz r25 / extsb r25,r25. Making area s8 adds an unwanted sign extension at GetData; splitting the unsigned load and cast changes allocation; inline signed-area accessors do not improve the retained result. Chaining the two FormatAreaName format-spec expressions regresses it. These experiments were reverted. The earlier claim that declaration permutations do not help CYouWin is conditional on the old formatter expansion: after the += correction, moving eligible's declaration fixes most of its register allocation.

### Commands and build limitation

Function inspection used python tools/re/unitdiff.py <unit> --function <mangled-name>, with --no-build when inspecting an object just compiled by its generated command. Freshly compiled all five requested units and the three pause-menu caller units. Ninja again stalled when asked to rebuild a changed object; that attempt was interrupted. Read-only ninja -t commands and -t deps worked. Used the exact generated sjiswrap/compiler commands directly, with checked return codes, and generated the link response file from build.ninja's actual link inputs. Then ran the configured linker and dtk elf2dol sequentially. This is successful compilation/linking through the generated build pipeline, not a successful normal Ninja invocation. No compiler options were changed.

Final acceptance command: python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1. Final report command: tools/local/objdiff-cli.exe report generate -o build/wpse-v1/report.json. Existing exception-action-30 warnings remain. Only NPpuLibrary was newly enabled as Matching; the other requested units are explicitly unfinished.


## 2026-10-22 NPpuVM and CModel follow-up

### Retained result and validation

CModel::AddResourceFile now scores 100% in both unitdiff and report.json, up from 96.99235%. Use a direct vector-iterator loop, evaluating rFiles.end() at the loop test, returning immediately when *it == file, then push_back after the loop. The generated target loop keeps the cursor in a register; the previous std::find call spilled its by-value iterator. Caching end in a separate local instead gives 98.591835%; a goto past push_back also gives 100%, but the early return is clearer. Using raw begin/end pointers with std::find gives 97.875% and does not reproduce the iterator temporaries.

Only src/game/CModel.cpp is changed. No shared-header experiment is retained. CModel is 152/158 functions at 100%; NPpuVM remains 56/63. Neither unit was promoted to Matching, and configure.py and target manifests are unchanged. The remaining six CModel and seven NPpuVM functions are unfinished.

The requested first command, git reset --hard main, failed because the linked worktree index is under the main checkout outside the writable root. Read-only checks established a clean checkout with HEAD and main both at 36186eb9eb4f307679bab4cd6f8023ca9a755747, so the requested starting contents were already present. No commit was attempted.

python configure.py exited 0. Normal Ninja and serial Ninja both stalled without output and were interrupted; neither has a successful build exit to report. The exact generated compiler commands obtained through ninja -t commands compiled NPpuVM and CModel successfully. Inspection used tools/re/unitdiff.py with --no-build and objdiff-cli report generate. git diff --check passed. No full-link verification or whole-unit source match is claimed.

The initial on-disk report counted 13,280 perfect functions, but was stale relative to the generated configuration (including CSaveManager iterator names). For a comparable baseline, the unchanged HEAD version of CModel.cpp was compiled and a fresh whole-project report generated, then the retained source was restored, compiled, and reported again. Both reports used the same configuration and other objects. Counts were 13,260 before and 13,261 after. AddResourceFile was the only function record that changed; no perfect function regressed. The final report is build/wpse-v1/report.json. All temporary source variants were held in process memory; no scratch files were created.

### Rejected experiments and traps

- NPpuVM CCode::Load: a restricted local header pointer and const-qualified SHeader fields each leave 88.051285% unchanged. Both were reverted.
- NPpuVM numeric operations: separate right and left CValue locals instead of the two-element array fix the initial ConvertToNumber region but leave later type-test, cached-load and register differences. The best tested right-then-left declaration form gives BinaryOp 95.09557%, BitwiseOp 98.15862%, CompareOp 98.26408%; none reaches 100%, so the array implementation is retained. Expanding the wide-string predicate directly in ConvertToNumber or adding a non-const IsWString overload regresses these scores. IsInt negation/explicit-result variants also regress existing perfect functions and were reverted.
- NPpuVM Execute: continue, goto next, or an empty inlined callback under the argument-sign check all eliminate the redundant comparison, just as the existing break does. Execute stays 99.77628%; the target retains cmpwi/blt followed by an unconditional branch to the same loop head. No artificial side effect was introduced.
- NPpuVM pointer-set dereference: returning a const reference to the stored pointer instead of a value raises CollectGarbage only to 90.042854% and does not fix the engine destructor. Reverted; this is not evidence for changing the shared container interface.
- CModel IsIdle: explicit nested result variables, changing IsIdle and/or IsReplaceable return types from bool to int, and double-negating the IsIdle result do not resolve the temporary-register order. Explicit result-variable forms worsen it; type and double-negation forms are unchanged.
- CModel Update: one, two or three inline wrappers around pop_front, changing advanced to int, and true-setting compound/boolean expressions do not change either Update score. Moving the initial advanced declaration before IsReplaceable worsens both. A conditional assignment guarded by !advanced also worsens both. All reverted.
- CModel Load/destructor: extending the previously documented vector-clear helper experiment to one through three helper levels and zero through two independent model-reset wrappers still leaves the exception-cleanup slots reversed. Odd vector-helper depth gives destructor 99.994804% and Load 99.99005%; even depth returns the baseline scores. A temporary-vector receiver or empty.swap(mFiles) worsens both; vector assignment changes the operation and is much worse. No helper scaffolding is retained.
- CModel AddResourceFile: removing vector iterator's user-written assignment operator does not eliminate the std::find spill. Combining it with the documented break-based find or an early-return find also fails. The successful fix is local to AddResourceFile, with no shared algorithm or iterator changes.
