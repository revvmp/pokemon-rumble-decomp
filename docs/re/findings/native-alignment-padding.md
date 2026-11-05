# Native code alignment replaces original padding objects

The first 2026-11-04 five-function alignment pass removes 468 automatic original objects containing only 3,780 zero bytes of code alignment. No instructions become newly matched or source-linked in this change. The code denominator decreases from 3,082,252 to 3,078,472 bytes; matched and source-linked byte numerators remain unchanged. The full native executable is byte-identical to the frozen original. Subsequent Player and NPpuVM source acceptance removes the final two padding objects, totaling twelve bytes, by the same native-link proof. The final code denominator is 3,078,460 bytes, with all 470 ranges retained in the ledger.

## Evidence and implementation

Each removed object has exactly one nonempty allocated section, contains only zero bytes, and has only an automatic `pad_` function entry. The following selected source object has native section alignment that rounds the preceding end address to the original next start. The audit includes `.init`, and searches forward over inputs without a section of the relevant kind. It does not assume that adjacent linker inputs both contain `.text`.

The original addresses, extents, alignment requirements, successor source units and audited compiler-object hashes are retained in [the alignment ledger](../native-alignment.tsv). CModel's hash refers to its first raw final-link fragment, CModelThunks, rather than the separate partial-link analysis object. The ledger is evidence for this checkpoint; later source changes can change an object hash without changing its alignment requirement.

An isolated native final link first omitted the proven padding inputs while retaining every selected source and original fallback object byte unchanged. The linker exited zero, elf2dol exited zero, and the frozen verifier passed with zero differing DOL bytes. A second trial included the newly source-linked selector and likewise reproduced all 4,094,880 bytes.

The production correction uses the existing `skip` section attribute in the pinned dtk 1.8.4 [split parser](https://github.com/encounter/decomp-toolkit/blob/v1.8.4/src/util/config.rs) and [split writer](https://github.com/encounter/decomp-toolkit/blob/v1.8.4/src/util/split.rs). Each skipped range is recorded under the following source owner's ordinary split, immediately before its real code range. dtk emits no original padding bytes and no extra empty unit for that range. The original compiler section alignment supplies the bytes during the native link. No new build mode, output patch, fabricated source array or object transformation is involved.

Normal ninja then exited zero and frozen verification passed. The report has 920 units, including 891 Matching units, and the native final link has 923 inputs because CModel adds three raw fragments. All 923 filename aliases remain byte-identical to their selected inputs. The original-address audit finds five unfinished real functions, no regression and no missing previously exact function. Two automatic code-alignment ranges remain temporarily attached to the Player and NPpuVM original fallbacks: four and eight bytes respectively. They are not removed until their native source alignments are accepted.

## Repeatable byte audit

Run the ordinary build and frozen verifier first, then:

```powershell
python tools/re/audit_alignment.py
```

The real CLI checks every documented range against both DOLs, rejects overlapping or non-alignment ranges, and rejects any nonzero original content. This supplements the full artifact comparison and the recorded compiler alignment evidence; zero bytes alone do not establish source intent. The final accepted ledger reports 470 ranges and 3,792 identical zero bytes. Real CLI rejection runs also rejected an empty ledger, a duplicate range, and a purported alignment range containing original instructions. No unit tests were added.

## Traps

- A progress denominator can include original alignment objects even when the compiled successor already regenerates the same bytes. Prove actual input alignment, omit the padding in a native link, and verify the complete frozen artifact before changing split ownership or accounting.
- The next input may lack the relevant output section. The two SDK `.text` cases around `.init` owners require following the section stream rather than assuming the next object contributes code to the same section.
- A zero-valued data or BSS region is not established padding by this code audit. Unresolved data ownership remains in the separate [data inventory](remaining-data-inventory.md).
