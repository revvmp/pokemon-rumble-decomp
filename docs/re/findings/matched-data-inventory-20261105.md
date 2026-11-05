# Matched Data inventory, 2026-11-05

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

## Baseline and scope

The initial audit of `build/wpse-v1/report.json` measures 1,245,840 matched data bytes out of 1,788,360, or the report's 69.663826%. All 897 units are complete and source-selected data is 1,788,360 bytes. These are distinct measurements. The [earlier executable acceptance](main-source-completion-20261104.md) explicitly preserves the object-level diagnostic and does not claim it reached 100%.

The audited report has 4,561,010 bytes and SHA-256 `c88be97be32c594219052939c0a613ccc4c82eec59cd4ca6f0942421a29ef54e`. Git status was clean before this investigation. This is an inventory of actual report deficits, not a fresh executable build or full-artifact verification.

## Reproducible inventory

`tools/re/data_match_inventory.py` reads a version 2 objdiff report, validates integer counts, checks unit sums against aggregate counts, and reconciles every unit's lost data credit against its non-perfect data sections. It does not write reports, change configuration, build objects or modify originals. Unclassified sections and accounting discrepancies fail instead of silently changing the denominator.

```powershell
python tools/re/data_match_inventory.py build/wpse-v1/report.json --limit 25
python tools/re/data_match_inventory.py build/wpse-v1/report.json --json
```

The text output ranks units and lists their affected sections. JSON output includes every affected unit regardless of the text limit, aggregate section counts, exact byte totals and the input report digest. The JSON percentage field preserves the report's published floating-point value.

The initial report has 242 units with data deficits, totaling 542,520 bytes:

| Section | Affected sections | Lost data credit |
| --- | ---: | ---: |
| `extab` | 139 | 249396 |
| `.data` | 171 | 155158 |
| `.bss` | 23 | 62420 |
| `extabindex` | 138 | 55680 |
| `.sdata` | 121 | 9790 |
| `.rodata` | 27 | 8240 |
| `.sdata2` | 48 | 1316 |
| `.sbss` | 26 | 520 |

The largest unit deficits are `nw4r/ef/ef_effectsystem` at 49,504 bytes, `game/CDlgPiiSelector` at 20,212, `game/CDlgPiiList` at 18,080, `game/CPpuEnv` at 17,636 and `game/CModel` at 14,460. These figures are whole-section credit losses, not counts of differing bytes.

## Why nearly exact sections lose all credit

The pinned [objdiff v3.8.2 report implementation](https://github.com/encounter/objdiff/blob/v3.8.2/objdiff-cli/src/cmd/report.rs) adds a data or BSS section's full size only when its section percentage equals 100.0. The baseline inventory independently confirms that every lost byte is accounted for by this threshold.

For example, EffectSystem's `.bss` scores 99.82603% over 49,432 bytes and its `.data` scores 85.71429% over 72 bytes. Neither contributes data credit, explaining all 49,504 bytes of this unit's deficit.

The pinned [section comparator](https://github.com/encounter/objdiff/blob/v3.8.2/objdiff-core/src/diff/data.rs) compares BSS symbol address/size sequences and also computes a size-weighted score from matched target symbols. It uses the better result. Data sections begin with the symbol-based score; the byte-stream score may improve it only when every target-side relocation also matches. Therefore naming or mapping independently identified symbols is worth investigating before altering already accepted source emission. It is not valid to invent identities solely to raise a score; each mapping needs address, size, content and relocation evidence as applicable.

## Useful objdiff entry points

The installed `objdiff-cli.exe diff --help` confirms `-p`/`--project`, `-u`/`--unit`, `-1`/`--target`, `-2`/`--base`, `-o`/`--output`, `--format` and repeatable `-c`/`--config`. One-shot JSON output provides actual section and symbol differences:

```powershell
tools/local/objdiff-cli.exe diff -p . -u main/nw4r/ef/ef_effectsystem -o - --format json
```

`report generate --help` confirms project and output selection, JSON/protobuf format selection, configuration overrides and optional global/weak-symbol deduplication. Scratch projects can isolate proven symbol mappings and generate diagnostic reports without modifying the canonical project or baseline report. Deduplication changes accounting and is not a substitute for closing the current denominator.

## CLI validation

The real text entry point exited zero on the initial report and reconciled all 542,520 missing bytes. The JSON entry point was also parsed through PowerShell's `ConvertFrom-Json` and exited zero. By that later observation, the coordinator had regenerated the shared report with the EffectSystem BSS correction: the helper reconciled 1,295,272 matched bytes, 493,088 missing bytes and 242 affected units. That report digest was `0ebf02d5bed0917ee401aa0ed8651d6f0fec201820adb8338cbb73ee86297afb`; this records a later observation rather than silently replacing the initial baseline.

A nonexistent report path returned exit status 1 and an explicit missing-file error. No fixture report or unit test was added. `git diff --check` passed. The helper performs no filesystem writes; shared report regeneration belongs to the coordinator.

## Address-backed data-symbol audit

`tools/re/data_symbol_mappings.py` proposes mappings for same-sized `.data` and `.sdata` objects. Optional `--section` selections also support `.rodata` and `.sdata2`. It uses the ELF reader shared with `map_exception_symbols.py`, without external Python packages.

The audit first validates current `main.dol` against the frozen manifest and original, then compares every requested final ELF data section against that DOL. It resolves local source symbols through the final ELF's `STT_FILE` owner and symbol name; global symbols require a unique retained global name. The resulting linked address, section and size must equal the original target object's recorded address, section and size. Candidate selection never searches for matching byte patterns. Target or source names ambiguous within the object, retained names ambiguous within their ownership scope and merged target ranges are rejected.

Each proposed pair must additionally have identical raw object bytes and identical relocation tuples after resolving every relocation to its final absolute destination, including its addend. Unresolved relocation targets reject the candidate. The evidence stores both object digests, the final ELF and DOL digests, original address, source owner, symbol names, byte digest and normalized relocation tuples. Object mutation during a unit audit and changes to shared configuration, report or final ELF during the sweep cause failure.

The initial real BController audit accepted seven mappings covering 119 bytes: its jump table, compiler string object and five local metadata objects. The jump table's thirteen resolved destinations also matched. This is candidate evidence, not a claim of increased report credit. A subsequent attempted all-unit run correctly failed while the coordinator's `main.dol` was absent during rebuilding. The unknown-unit CLI rejection also returned exit status 1.

```powershell
python tools/re/data_symbol_mappings.py --unit main/game/BController --output tools/local/data-match-20261105/bcontroller-data-mappings.json
```

The optional `--config-output` writes a separate scratch objdiff project containing only requested units, absolute object paths and the proven mappings. Diagnostic outputs are restricted to JSON under `tools/local`; the tool never changes shared configuration or symbols. Run objdiff against that project to measure its real report effect before coordinator integration.

After the coordinator restored the verified DOL, the all-unit `.data`/`.sdata` sweep found 5,301 mappings covering 144,778 bytes. Extending the same address, ownership, size, byte and relocation proof to `.rodata` and `.sdata2` found 6,791 mappings covering 156,025 bytes. The complete evidence is `tools/local/data-match-20261105/data-symbol-mappings.json`; the isolated project is `tools/local/data-match-20261105/data-mapped/objdiff.json`.

The pinned unmodified objdiff CLI generated that isolated project's report successfully. Against the concurrent canonical baseline of 1,573,960 matched data bytes, it measured 1,593,678 bytes (89.11394%), adding 19,718 credit bytes with no unit losing matched data or functions. All 13,462 functions and 3,078,460 code bytes remained matched. This is the scratch-project measurement before production integration. Mapped symbol bytes and newly credited section bytes are different quantities.

```powershell
python tools/re/data_symbol_mappings.py --section .data --section .sdata --section .rodata --section .sdata2 --output tools/local/data-match-20261105/data-symbol-mappings.json --config-output tools/local/data-match-20261105/data-mapped/objdiff.json
tools/local/objdiff-cli.exe report generate -p tools/local/data-match-20261105/data-mapped -o tools/local/data-match-20261105/data-mapped-report.json
```

The isolated report's surviving relocation differences have a separate tool-level explanation: `reloc_eq` in pinned objdiff compares relocation target names directly and does not receive the project's symbol mapping table. Pairing two named symbols does not therefore make references to those symbols compare equal. CPpuEnv demonstrates this with two 20-byte shared-pointer-deleter vtables at 88.88889% and two eight-byte RTTI entries at 66.66667%, despite proven identities. This investigation leaves the comparator and production mappings to their coordinating owners; it does not relax the raw-byte or relocation proof.

### Duplicate target names across sections

Review of six objdiff warnings found a limitation in the first mapper revision: it required source-name uniqueness but had not checked target-name uniqueness across sections. The individual address/body/relocation pairs were proven, but the serialized mapping key could refer to multiple target symbols. The current helper rejects that ambiguity and records any already-configured keys under `ambiguous_existing_mappings`; optional scratch configuration omits them.

| Unit | Ambiguous target key | Original sections |
| --- | --- | --- |
| OSPlayTime | `@470OSPlayTime0` | `.data`, `.sdata`, `.sdata2` |
| NANDCore | `@491NANDCore0` | `.rodata`, `.data` |
| NANDCheck | `@491NANDCheck0` | `.data`, `.sdata` |
| scsystem | `@492scsystem48` | `.data`, `.sdata` |
| scsystem | `@492scsystem60` | `.data`, `.sdata` |
| scsystem | `@492scsystem78` | `.data`, `.sdata` |

The isolated four-unit project `tools/local/data-match-20261105/unique-target-names/` removes only those keys. Its real pinned objdiff report emits none of the six warnings, preserves all 54 functions and preserves every data total: OSPlayTime 160, NANDCore 771, NANDCheck 247 and scsystem 34,038 bytes. No meaningful data credit depended on the ambiguous keys. The earlier 6,791-candidate observation remains historical; subsequent audit receipts use the corrected uniqueness rule.

### Refreshed sweep after boundary corrections

After the coordinator integrated the proven partitions and symbol extents and restored a frozen-verified DOL, the corrected mapper accepted 7,661 mappings covering 180,676 symbol bytes. It found 136 new aliases beyond the current configuration; `tools/local/data-match-20261105/data-symbol-mapping-additions.json` carries those additions in the production schema `{schema_version: 1, units: {unit_name: {target_name: source_name}}}`. The complete regenerated evidence remains `data-symbol-mappings.json` in that directory.

The real isolated report measured 1,627,418 matched bytes (91.00058%), 10,228 above the then-current canonical 1,617,190 bytes. There were no data-unit or function regressions, and all 13,462 functions and 3,078,460 instruction bytes remained matched. The six ambiguous-name warnings were gone. An independent pass through every configured target and source ELF found zero mappings with a missing or multiply defined raw symbol name.

## Independent boundary-helper review

An independent read and real invocation of `tools/re/data_symbol_boundaries.py` checked its candidate and output protections. The helper compares the frozen DOL and final ELF data, checks configured target names/sizes against current split objects, proves each proposed source object's raw bytes and normalized relocation destinations, and accepts uncovered intervals only when every byte is zero and no relocation starts there. It snapshots original/build/configuration inputs and all audited objects and rejects changed inputs before emitting its separate JSON packet. It does not apply corrections or change the denominator.

The independent real CLI run audited 172 game units and produced zero size reductions and 18 complete partitions in `tools/local/data-match-20261105/boundary-independent-review.json`. These partitions include 24 weak exception-message strings whose final addresses are inferred from a shared source-section delta established by at least two retained symbols. Those strings are explicitly marked `pooled_weak_string`; all occur after the last retained anchor. Their full partition byte/relocation proof is valid, but their address evidence is inferred layout, not an individually retained final ELF symbol. Preserve this distinction when reviewing and documenting corrections. The normal native full-link and frozen-verifier gate is still required after integration.

The real CLI was also invoked with `--output build/wpse-v1/report.json`. It rejected the input/output collision with exit status 1, and the actual report's SHA-256 was unchanged. No destructive mutation or fixture was needed to exercise this rejection.

The SDK duplicate-name owners were rechecked after these corrections: OSPlayTime is 160/160, NANDCore 771/771, NANDCheck 247/247 and scsystem 34,038/34,038 matched data bytes, with every reported section at 100%. Renaming their remaining duplicate compiler-generated labels would not recover additional data credit; the strict mapper safely leaves those ambiguous keys unconfigured.

## Traps

- A source-selected data percentage of 100% does not establish 100% Matched Data. Read both exact counters from the real report; this baseline is 100% source-selected and 69.663826% matched.
- A section scoring 99.99% can lose its entire data credit. Rank whole-section deficits, then inspect actual symbols and relocations; do not interpret the lost-credit total as differing-byte count.
- An omitted section percentage in protobuf JSON is a default zero-valued field, not evidence that the section is exact. The inventory treats any value other than 100 as uncredited and verifies the result against the reported counts.
- Objdiff mappings are keyed by name without a section qualifier. A valid individual address match cannot be serialized safely when its target name is repeated in another section of the same unit. Require both target and source name uniqueness; the six rejected SDK keys above produced warnings without changing matched-data credit.
