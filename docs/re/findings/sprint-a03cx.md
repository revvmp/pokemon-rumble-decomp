

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
