# Game-data symbol boundary audit

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

This continues the [selector audit](selector-data-matching-20261105.md) using actual compiler symbols, retained final-ELF identities, original bytes, and relocation targets. It proposes comparison metadata corrections without changing source implementations, section bounds, data denominators, or original artifacts.

## CPpuEnv

The initial report contains 8,040/25,676 matched data bytes. `.data` has 14,664 bytes and `.sdata` has 424 bytes. Every non-padding original object in its initialized data sections has a unique same-address retained compiler symbol of the same size. No symbol-boundary repair is required for this unit; explicit evidence-backed mappings are the appropriate next step. The exception sections remain the separate exception-mapping workstream.

## CModel

Two inferred vtable extents include four trailing zero bytes each:

| Address | Symbol | Original inferred size | Proven compiler size | Relocations |
| --- | --- | ---: | ---: | ---: |
| `0x803BFB98` | `__vt__6CModel` | 64 | 60 | 13 |
| `0x803BFCE0` | `__vt__Q43std3tr16detail72shared_ptr_deleter<Q34nw4r3g3d15AnmObjTexPatRes,PFPQ34nw4r3g3d6G3dObj_v>` | 24 | 20 | 4 |

For each record, the entire meaningful original prefix is byte-identical to the compiler object, all trailing bytes are zero, and every relocation resolves to the same absolute target address with the same type and relative offset. The current final ELF supplies the retained symbol identity and size. The real CModel source fragments and their native input ordering remain unchanged.

Exact old/new symbol lines and proof records are in `tools/local/game-data-20261105/cmodel-symbol-corrections.json`.

## Wider game audit

Applying the same strict read-only check across game objects identifies 167 oversized inferred symbols, containing 639 bytes of trailing zeros. The proposal changes only symbol extents; those bytes remain inside the same section ranges and remain in the measured data denominator.

Each candidate must satisfy all of these independent conditions:

1. The source object contains a unique candidate at the original symbol's final address and section, established through the linked ELF and global identity or local file ownership.
2. The linked compiler symbol size agrees with its source-object size and is smaller than the inferred original extent.
3. Every meaningful original byte equals the compiler symbol's complete body.
4. Every byte after that body in the inferred extent is zero, with no relocation in that tail.
5. All meaningful relocations have identical relative offsets, relocation types, and resolved final addresses.
6. Exactly one configured target symbol exists at the address; every input object and the final ELF remain unchanged during inspection.

The audit rejects 125 larger or merged extents that fail these conditions. Those cannot be truncated to the first compiler symbol: they may contain several real strings, vtables, or metadata objects. Their separate ownership needs reconstruction before proposing a split.

The complete proposal is `tools/local/game-data-20261105/game-symbol-boundaries.json`. Each entry records its unit, address, section, exact old/new line, source symbol and size, raw SHA-256, normalized relocations, and padding-byte count. The rejected inventory is `game-symbol-boundary-rejected.json`. Representative units include CTitleSession with 13 corrections, CMsgRes with seven, CMsgBalloon with six, CLayout and CDlgCollection with five each, and CFileData with four.

These are diagnostic proposals until the coordinator applies them, regenerates actual split objects and reports, and verifies the complete frozen executable. No whole-unit data acceptance or new full-build result is claimed by this audit alone.

## Complete merged metadata partitions

A follow-up audit reconstructs 105 of the previously rejected merged extents as 736 actual compiler objects spanning 27,835 original bytes. Each subobject has independent retained-address and owner evidence, exact raw bytes, and exact normalized relocation tuples. Every uncovered byte is zero and has no relocation. The 5,399 uncovered zero bytes remain in the original section ranges and data denominator. They are not all asserted to be alignment padding; discarded weak metadata can also leave zero-filled storage.

The coordinator received `game-merged-symbol-partitions.json` and integrated the accepted symbol-only changes. After those changes and the earlier extent batch, a fresh audit against the restored frozen-verified build finds zero further corrections under the initial retained-symbol-only rule.

## Physically retained weak strings

The final 18 rejected ranges contain strings whose weak symbol definitions were coalesced elsewhere while their physical bytes remain in the owning pooled data section. Final symbol ownership alone cannot identify those local physical copies.

The narrow additional proof requires at least two retained objects in the same compiler section, each independently establishing the same final-address minus source-offset displacement. Only a unique weak `@STRING@` symbol within that established extent is considered. Its body must be a printable ASCII NUL-terminated literal with no relocations. The complete original subrange must equal its entire source body, and the complete proposed partition must still cover every nonzero byte and every relocation. The proof explicitly labels such placement `pooled_weak_string`, separately from `retained_final_symbol`; it does not claim that the coalesced symbol itself has that final address.

For CDlgPiiList, the final original range at `0x8039F448` contains these four real strings:

| Address | Compiler identity | Bytes | Placement evidence |
| --- | --- | ---: | --- |
| `0x8039F448` | `@21352` | 72 | Retained final symbol |
| `0x8039F490` | `@21353` | 36 | Retained final symbol |
| `0x8039F4B4` | `@STRING@what__Q33std3tr112bad_weak_ptrCFv` | 13 | Physically retained pooled string |
| `0x8039F4C4` | `@STRING@what__Q23std9exceptionCFv` | 10 | Physically retained pooled string |

The first two literals name the CDlgPiiListImpl and char-array default deleters. Their retained addresses independently anchor the final `bad_weak_ptr` and `exception` bytes. The remaining five zero bytes stay in the section. This resolves a real symbol-boundary explanation for the `.data` diagnostic of 99.982216%, without adding source padding or changing the 5,624-byte section.

Across game units, the extended audit proves 18 partitions containing 153 objects across 5,346 original bytes, including 24 physically retained weak strings. All 657 uncovered zero bytes remain. Evidence is `tools/local/game-data-20261105/current-pooled-audit.json` with frozen DOL SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da` and contemporaneous ELF SHA-256 `f5abbda3a563a738c116c91d533c88edee5f30b304e9e6084a239e214079b9bc`.

## Durable audit entry point and validation

`tools/re/data_symbol_boundaries.py` performs these checks and emits proposals only. It never edits symbols, splits, source, originals, or measured reports. It verifies the frozen DOL and all four initialized final-ELF data sections, then checks every input snapshot again before publishing. The snapshots include originals, the manifest, final artifacts, reports, configuration, and input objects. Output must be JSON under `build/` or `tools/local/`; direct and hardlink aliases of inputs are rejected.

The ordinary CLI `python tools/re/data_symbol_boundaries.py --output tools/local/game-data-20261105/current-pooled-audit.json` exits zero and audits 172 current game units. A deliberately missing `main.dol` during the coordinator's rebuild caused a correct exit-one rejection without output. The real collision path, requesting `build/wpse-v1/report.json` as output, also exits one and leaves that report's SHA-256 unchanged. No unit tests were introduced. An independent tooling review found no content-proof bypass and confirmed the distinction between retained symbol ownership and inferred physical pool placement.

The coordinator owns integration and complete artifact acceptance after the final 18 proposals. The helper's diagnostic success alone is not full source-link acceptance.

## CPpuEnv destructor identities

The original `shared_ptr_deleter<CPpuEnv>` vtable's destructor slot targets `0x80106130`. The accepted source and final ELF also identify that address as the CPpuEnv deleter destructor, but the inferred original symbol map names it as the CPpuCode deleter destructor. The reciprocal error occurs at `0x801060F0`: the original CPpuCode vtable and final source ELF identify CPpuCode, while the target map says CPpuEnv.

Both destructor bodies are identical 64-byte functions, so function scoring did not expose the swapped identities. Their correctly typed vtable consumers do. `cppuenvironment-destructor-identities.json` contains the exact two-line name swap; source code and function placement remain unchanged. This repeats the established identical-destructor naming trap rather than introducing a new emission workaround.

After correcting those function names, the existing exception-record mappings still reflected the old identities. The original records at `@eti_8004B4EC` and `@etb_800284A8` belong to CPpuCode and map to compiler `@16973` and `@16972`; `@eti_8004B4F8` and `@etb_800284B0` belong to CPpuEnv and map to `@16978` and `@16977`.

Both 12-byte exception-index records and both eight-byte exception tables are raw-byte-identical. After the mapping correction, every relocation matches exactly as a tuple of relative offset, mapped symbol identity, relocation type, and addend. The actual production objdiff CLI, run with an isolated corrected configuration, exits zero and reports CPpuEnv at **25,676/25,676 matched data bytes, 100%**, with all six sections and all 33 functions exact. The packet and report are under `tools/local/game-data-20261105/cppuenvironment-eh-fix/`. The coordinator owns production integration and complete artifact verification.

## CMsgRes packed collation data

The remaining 1,856-byte `.rodata` section discrepancy is confined to `sKanaCollation`, a 90-element `u32` array at `0x80367660–0x803677C8`. Every one of its 360 compiled bytes equals the original DOL. The compiler emits no relocations for the array. However, the original split contains 23 inferred function-address relocations, which replace integer words with zero relocation slots before comparison.

Examples are the collation keys `0x80202528` and `0x80202568`, incorrectly interpreted as references into `PFDIR_p_chmod`, and `0x80204528`, incorrectly interpreted as a reference into `PFDIR_ChmodEntry`. `SetCharacterKey` indexes this array for hiragana and katakana and assigns each value directly into `CollationKey::mValue`; these are packed ordering fields, not pointers.

`tools/local/game-data-20261105/cmsgres-packed-collation.json` records all 23 exact relocation source sites, original integer words, and false target names. The proposed fix blocks only those demonstrated false relocations. It changes neither table contents nor acceptance hashes. This is the same packed-data ambiguity already established for species metadata.

## Explicit final padding symbols

DCIM's `.data` contains the complete ten-byte `%03dNIN01` format string plus two zero alignment bytes. Its `.sdata` contains exact `/DCIM`, `*`, and `NIN01` literals, ending at offset 18, followed by two zeros. The inferred target symbols explicitly name those final two-byte ranges as objects, unlike the neighboring dtk-generated hidden gap symbols. That makes them affect symbol matching even though the compiler emits no separate padding objects.

The game `new` unit has the same issue: its exact 17-byte `!std::bad_alloc!` exception name is followed by an explicitly named three-byte padding object. An initial diagnostic proposal to hide these explicit padding symbols for comparison was rejected by the coordinator and is not an accepted fix. Hidden visibility is not a substitute for independently demonstrating native padding ownership and an honest denominator correction under the established alignment policy.

The original named ranges are `0x803B45B2–0x803B45B4`, `0x804B783A–0x804B783C`, and `0x803BF3C9–0x803BF3CC`. All contain only zeros and no relocations, with no compiler-defined data object at those ranges. DCIM's compiler `.data` has size 10 and `.sdata` size 18; `new`'s compiler `.data` has size 17. Each compiler section has ELF alignment 8. The next retained objects are l10n_files literals at `0x803B45B8` and `0x804B7840`, and the pad.cpp vtable at `0x803BF3D0`. The named pads therefore cover only part of the actual native inter-unit alignment gaps. The inventory workstream owns their consolidated native-alignment ledger and complete link proof before any exclusion is accepted.

CFiber's leading range is a different case: it contains a real terminated-exception type string despite its inferred `pad_` name. The SDK workstream owns that correction; it must be named and compared, not classified as padding.

## Traps

- A correct compiler vtable size can be smaller than the inferred target symbol while every final byte already matches. Preserve its adjacent zero bytes in the section and correct only the evidenced object extent.
- A merged metadata range is not padding merely because its first source symbol is shorter. Require the complete remainder to be zero and relocation-free before shortening; otherwise reconstruct every owned object separately.
- A coalesced weak string can retain its local physical bytes without a local final symbol. Prove its pool placement through independent neighboring retained anchors and exact literal bytes, and disclose that inference separately from final symbol ownership.
- A `pad_` name alone proves nothing. CFiber's first such range contains a real exception type string; classify padding only after checking bytes, source objects, and consumers.
- Correcting two identical destructor identities also changes which exception records belong to each name. Re-derive their index/table mappings from function relocations; otherwise stale mappings can keep exception-index data below 100% after the vtables are fixed.
