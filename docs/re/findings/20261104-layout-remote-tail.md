# CLayout and CRemoteMemory final-function investigation

## FindAnimation instruction completion

The 2026-11-04 main-branch pass makes all 2,048 bytes of the unrelocated `FindAnimation__7CLayoutFPCc` function identical to the original split object. Every instruction, register, branch displacement, and frame offset agrees. The formatted source is applied in `src/game/CLayout.cpp`, and all 169 previously exact functions retain their scores. `FindPaneMP` remains incomplete. This is an instruction-comparison result; the coordinator still owns native-report and full-source-link acceptance.

The standalone object comparison reports 99.96094% because four loads reference two compiler-local boolean constants rather than the original external labels. Both original and candidate refer to the same values: original `lbl_804B83F2` and candidate `@10434` contain `01`; original `lbl_804B83F3` and candidate `@10435` begin with `00`. The original false symbol also includes four padding bytes. The baseline standalone score was 98.17969%, versus 98.21875% in the configured report, the same 0.03906-point relocation-label difference. No reference symbols, original bytes, or expected hashes were changed.

The retained source directly constructs each map insertion value from its temporary string and animation transform. Local typed adapters reproduce the compact map lookup and insertion through the existing tree implementation. The lookup passes its best node through a caller-local output reference and adds one string-comparison forwarding level. Each insertion keeps a separate caller-local cached predecessor, eliminating the original candidate's two extra predecessor reloads and selecting the required register. Separate locals in the two resource branches are necessary; reusing one local matches the first insertion but leaves the second predecessor in r30 instead of r29.

These adapters are deliberate D-0016 source-shape controls. The three-word storage view corresponds to the compact map's count, root, and first-node compressed-pair fields. The tree operations, key comparisons, insertion results, and resource-loading behavior are unchanged. The artificial output references, comparator forwarding, storage view, and separate predecessor locals are not claimed as recovered original interfaces. There is no shared-header change, assembly, injected machine code, or object patching.

Private evidence is under `tools/local/tail-20261104/game_CLayout/`: `lr_insert2_2` contains the first instruction-identical candidate, and `lr_anim_formatted` contains the applied formatted candidate and full-unit comparison. `layout_remote_animaccept.py` compares the raw function bytes and boolean values. `layout_remote_applyanim.py` retains the narrow source transfer. These diagnostics are not build inputs.

## FindPaneMP investigation

The previously documented 99.579544% private candidate has been reconstructed in the current environment as `game_CLayout/lr_panefinal2_1/CLayout.cpp` with private `lr_panefinal_include/map` and `set` overlays. Its operation sequence, branch destinations, and frame size agree; 78 register and stack operands remain different. It preserves the scores of every previously exact function. The target erases nested descendants inside the ancestor loop and returns directly to the descendant-begin setup. A named restart label reproduces this flow. Iterator storage views, explicit result copies, retained insertion-result construction, underlying-node comparison, and a separately assigned mutable end iterator account for the remaining target stores and address calculations. None of this incomplete candidate is applied.

## CRemoteMemory baseline comparison

Fresh direct compilation of the native source, absolute and relative source paths, and scratch copies with both CRLF and LF endings all produces the same object SHA-256: `048c69ae957c57440e12b26a8fe5d5f7625ad71a2b07f24c5a395053925a07db`. The coordinator's freshly rebuilt native object has the same hash. Direct object comparison reports `FinishWrite` at 98.18627%; the configured report gives 98.65196%. This is comparison context, not stale output or path-dependent compiler behavior. `BeginRead` and `FinishRead` remain at 97.26923% and 97.45731%. No CRemoteMemory source change is applied.

The reproduction is `layout_remote_baselinecheck.py` with per-input comparisons in `game_CRemoteMemory/lr_inputcheck`.

## Rejected approaches

FindAnimation experiments changing lookup forwarding depth, lower-bound declaration order, pointer identities and storage accessors, comparison wrappers, string-size scalar identities, and individual optimization switches did not complete the function. One additional generic lookup level fixes all eight comparison frame slots but changes allocation priority because the root cursor moves below the coloring threshold. Promoting the best-node output into the caller restores the lookup allocation; explicit separate predecessor outputs complete both insertion paths. This distinguishes the successful change from the earlier 99.87695% candidate, which still had stack and register differences.

FindPaneMP experiments separating the two outer iterators, changing find-result initialization, and assigning underlying iterator storage do not improve the reconstructed 99.579544% candidate. The isolated structural correction alone reaches 95.96023% from 93.52651%.

## Traps

- A standalone objdiff percentage can disagree with the configured report even for the same object bytes. Compare hashes first, then inspect external versus local constant relocations. Here four boolean loads account for the FindAnimation discrepancy; changing input paths and line endings does not alter the object.
- Reusing one output-reference local across two inlined insertions does not preserve the same allocation for both definitions. Separate branch-local predecessor outputs resolve the second live range without additional instructions in FindAnimation. This is a local compiler control, not a general register-allocation guarantee.
