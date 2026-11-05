# Native data alignment audit, 2026-11-05

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

## Scope and acceptance

Seven verified zero-padding ranges total 37 physical bytes. Seventeen of those bytes were still included in the preceding objdiff data denominator; the other twenty already lay outside counted unit extents. The accepted correction excludes only those seventeen counted alignment bytes. It does not remove bytes from the executable, add source padding, alter original files or change an expected artifact hash.

Their ordinary native section alignment explains every range. After applying the configuration packets, the coordinator's normal Ninja build exited zero and the frozen verifier passed. The independently inspected receipt records `2026-11-05T08:45:19.310939+00:00`, the unchanged 4,094,880-byte DOL and a null first-difference offset. No source code or original input changed for this correction.

The observed accepted report has 1,788,343 total data bytes, exactly seventeen fewer than the previous 1,788,360. All five affected units now have 100% matched data: CPiiCollectionBox 1,467/1,467, CUIMenuSoundCallback 1,552/1,552, DCIM 88/88, game/new 173/173 and dvderror 663/663 bytes. All 13,462 functions and 3,078,460 code bytes remain matched and source-linked. The project-wide data measure at this checkpoint is 1,787,651/1,788,343 (99.9613%); the remaining 692 bytes belong to other work and are not concealed by this correction.

## Measured ranges

| Section | Native gap start | Next retained object | Physical gap | Previously counted | Successor source |
| --- | --- | --- | ---: | ---: | --- |
| `.data` | `0x803A8B6B` | `0x803A8B70` | 5 | 1 | `game/CPiiInfoWorldObjStat.cpp` |
| `.data` | `0x803D6C7B` | `0x803D6C80` | 5 | 1 | `RVL_SDK/dvd/dvdFatal.c` |
| `.data` | `0x803B45B2` | `0x803B45B8` | 6 | 2 | `game/l10n_files.cpp` |
| `.sdata` | `0x804B783A` | `0x804B7840` | 6 | 2 | `game/l10n_files.cpp` |
| `.data` | `0x803BF3C9` | `0x803BF3D0` | 7 | 3 | `game/pad.cpp` |
| `.data` | `0x803AEF4C` | `0x803AEF50` | 4 | 4 | `game/CUIMenuSoundCallback.cpp` |
| `.sdata` | `0x804B7574` | `0x804B7578` | 4 | 4 | `game/CUIMenuSoundCallback.cpp` |

Every successor compiler section has `sh_addralign = 8`, and rounding each listed start up to eight gives its listed next object. Every range contains only zero bytes in both original and rebuilt DOLs and contains no retained final ELF data symbol. The [machine-readable ledger](../native-data-alignment-20261105.tsv) records section, range, native alignment, successor source and actual compiler-object SHA-256. This is separate from the established code-alignment ledger.

### CPiiCollectionBox and DVD error literals

CPiiCollectionBox's final retained `@3556` object begins at `0x803A8B3C` and contains the 47-byte null-terminated `std::detail::default_delete<CPiiCollectionBox>` type name. It ends at `0x803A8B6B`; the next ordinary source object begins at `0x803A8B70`. An explicit one-byte gap object at the former address had made `.data` lose all 260 bytes of credit even though the native full output was exact. The source object contains no corresponding one-byte C++ object.

`dvderror.c` has exactly 43 compiler `.data` bytes: a 28-byte DVD error file path and the 15-byte `@703` directory path at `0x803D6C6C`, including its null terminator. The target annotation had assigned sixteen bytes to the latter literal and a 44-byte unit extent. The real string ends at `0x803D6C7B`; eight-byte alignment places `dvdFatal.c` at `0x803D6C80`. The size correction is 16 to 15 and the owned extent correction is 44 to 43.

The scratch receipt is `tools/local/data-match-20261105/two-data-tails-evidence.json`. It records final owned symbol identity, exact literal bytes, source and successor object hashes, full zero gaps and the unchanged frozen DOL hash. `two-data-tails.patch` records the endpoint/skip and annotation edits; no compiler source changes are involved. Its preliminary status predates the full-link acceptance recorded above.

### DCIM, allocation exception name and menu callback

DCIM's `.data` literal at `0x803B45A8` has ten compiler bytes, and its last `.sdata` literal at `0x804B7834` has six. Their two explicit two-byte target padding objects cover only part of the six-byte gaps before the corresponding `l10n_files.cpp` sections. The `game/new.cpp` exception-name object begins at `0x803BF3B8`, has seventeen bytes and ends at `0x803BF3C9`; its explicit three-byte pad also covers only part of the seven-byte gap before `game/pad.cpp`.

CUIMenuSoundCallback's current splits start four bytes before their first retained `.data` and `.sdata` objects. Both compiler sections demand eight-byte alignment. Moving the owned starts to `0x803AEF50` and `0x804B7578` and recording the preceding ranges as native skips preserves the actual data objects. Independent menu-callback evidence is in `tools/local/data-match-20261105/game-bte-lane/menu-callback-padding.json`; the consolidated independent receipt is `additional-data-alignment-evidence.json` in the parent directory.

`additional-data-alignment-corrections.json` supplies the integrated old/new split strings and three explicit padding-symbol lines removed. Its matching patch is written with LF line endings. These five ranges account for fifteen of the seventeen-byte correction.

## Validation and integration rule

The existing real byte auditor accepts all seven ranges:

```powershell
python tools/re/audit_alignment.py --ledger docs/re/native-data-alignment-20261105.tsv
```

The observed exit status was zero both before the correction and independently after full-link acceptance, with `ranges: 7`, `bytes: 37`, `original_zero: true` and `rebuilt_identical: true`. The exact current DOL remained SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

Each split skip covers the full gap and uses `align:1 skip` so odd starting addresses remain exact in dtk's model. The following source object's native eight-byte alignment supplies the output padding. Existing literal objects retain their real bytes and sizes. The build's source inputs must remain unchanged and frozen full-file verification must pass after integration. A zero-filled range alone would not justify this exclusion; native alignment, real symbol extents and complete artifact proof are all required.

## Traps

- A final literal's inferred target size can include a byte from the following native alignment. Correct both the symbol extent and unit endpoint, and explicitly skip the full native gap; creating a synthetic source byte only obscures ownership.
- An explicit padding object can describe only part of a gap. Record the entire alignment range but subtract only the bytes previously included in the denominator. Here the seven ranges cover 37 bytes while the proposed denominator correction is seventeen.
- Odd native-gap starts need `align:1` in split skips. Otherwise dtk can round the analysis extent and generate a gap symbol beyond the actual object boundary.
