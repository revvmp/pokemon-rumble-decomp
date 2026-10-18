# F-344-1: complete narrow printf source unit

Issue344 was legitimately claimed through the collaboration helper after the earlier wide printf draft was open. Its last function at801E8DD0 is the first function of the narrow MSL_C printf unit; the preceding thirteen game-sound functions remain unfinished. The complete narrow unit crosses into owned345. The earlier wide source commits are retained in this combined branch, and partial PR715 is superseded only after actual combined acceptance. No issue344 completion is claimed.

## Independent native reconstruction

The narrow unit has17 actual functions and11,192 instruction bytes, including the8-byte default custom formatter in344. Its remaining16 functions cover11,184 original345 bytes; the already reconstructed wide13 functions cover10,788 bytes, together accounting for all29 original345 rows and21,972 bytes. The combined pair has30 actual functions/21,980 bytes including344's callback exactly once.

Source provenance follows [F-345-1](345-printf.md): pinned Petari CC0 printf reference, extended with independently reconstructed native positional/custom-format behavior. No file-specific notice exists in the reused reference. No decompiler output is used as source. Unused wide-only preparation declarations were removed from the narrow source; no dummy helper, ordering function, assembly, forced register or new compiler flag is used. Wii/1.1 and the existing MSL_C.Printf flag profile reproduce every function extent and non-relocation word.

| Section | Half-open native extent | Source bytes |
| --- | --- | --- |
| extab |80043418–80043478|96|
| extabindex |8005214C–800521DC|144|
| text |801E8DD0–801EB9E8|11288|
| data |803C2D98–803C3194|1020|
| sdata |804B8628–804B8669|65|
| sdata2 |804BA5A8–804BA5B0|8|

The text includes normal function alignment; instruction totals exclude it. The final narrow empty string at804B8668 has one byte, not the previous eight-byte survey extent. The following wide unit supplies its normal eight-byte section alignment; no expected bytes or original hash is changed.

The external audit344-audit-allocated.py independently applies all454 relocations at evidenced section bases and external targets and directly compares all six complete allocated sections to the immutable original. All bytes agree:174 text relocations,255 data jump-table entries,24 exception-index entries and the default callback pointer. The same454-site complete comparison passes against the actual Ninja-produced source object. Both objects are selected from source in the actual Ninja link inputs, with no original-object fallback; all30 final ELF addresses/extents and map names agree, and the full frozen executable matches.

## Real tooling stages

Wide-only baseline on76b5be1b: configure --map, plain Ninja and explicit frozen verification all exit0 (344-wide-baseline logs). The narrow original-only split then passes plain Ninja and explicit frozen verification, both exit0 (344-narrow-split logs). The source was compiled NonMatching and all six actual original/source allocated sections score100% in objdiff. It was then selected Matching: plain Ninja and explicit frozen verification both exit0 (344-narrow-source logs). The combined344-final-accept.py confirms actual source-only inputs, all30 final ELF extents/map names, all narrow allocated bytes after454 relocations, all six wide sections100% and complete immutable DOL identity. The final committed-head repeat and current-main merge loop remain required before publication/merge.

## Traps

- Complete instruction identity can conceal incorrect switch dispatch. A missing uppercase S parse case and the positional prepass ordering of unsigned custom cases, pointer case and character case produced four wrong data targets despite identical instructions. The native table sends S to the same string-conversion block; the source now expresses that case, and the prepass order reproduces all three originally mismatched entries. Audit resolved table targets before accepting a formatter.
- A failed compilation leaves the old object on disk. An early duplicate inline lconv declaration failed compilation after the canonical locale header was added; a diagnostic invoked after that failure saw the previous object. only the subsequent successful compilation and complete six-section audit establish the corrected candidate.

- Locale-default text reads can corrupt UTF-8 punctuation when rewritten explicitly as UTF-8. An unpublished documentation edit was rejected after the diff expanded across unrelated rows. Restoring the exact parent bytes and reapplying bounded edits with explicit UTF-8 and normalized CRLF removed every unrelated row change; final diffs and the absence of doubled carriage returns were checked before publication.
