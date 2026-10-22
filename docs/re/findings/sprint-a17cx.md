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
