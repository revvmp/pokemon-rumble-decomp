# Complete Matched Data acceptance, 2026-11-05

The ordinary production report now measures **1,788,343/1,788,343 matched data bytes (100%)**. All categories and all 897 logical units are complete. Two complete clean `build.bat` runs exit zero and reproduce all thirteen compared artifacts byte for byte, including the unchanged frozen main.dol. This closes the owner's Matched Data request; it does not expand the accepted executable scope to the boot loader, other contents or WAD packaging.

## Measures and source ownership

| Measure | Accepted result |
| --- | ---: |
| Matched data | 1,788,343/1,788,343 bytes |
| Source-selected data | 1,788,343/1,788,343 bytes |
| Game data | 578,704/578,704 bytes |
| RVL_SDK data | 962,379/962,379 bytes |
| NW4R data | 225,555/225,555 bytes |
| Runtime/MSL data | 21,705/21,705 bytes |
| Reported matched and linked code | 3,078,460/3,078,460 bytes |
| Raw exact function entries | 13,462/13,462 |
| Real exact functions | 13,461/13,461 |
| Complete logical units | 897/897 |
| Raw source linker inputs | 902/902 |
| Original split inputs selected | 0 |
| Units with lost data credit | 0 |

The real-function audit identifies exactly one zero-filled twelve-byte report entry, `pad_03_802347B4_text`. Thus the report's code total contains twelve alignment bytes; real instruction bytes are 3,078,448. This corrects earlier documents that called all 13,462 report entries real functions. No new exclusion or score adjustment was made for this embedded padding entry. The image renderer filters it. Broadcom `gap_conn_*` names are real functions and must not be classified as padding by a loose prefix match.

Independent inspection of `main_link.json` and its generated response file confirms 897 logical groups and 902 raw source objects. All 902 linker aliases are byte-identical to their compiler inputs, and no selected input comes from the original split-object directory. The earlier [source acceptance](main-source-completion-20261104.md), D-0015/D-0016 controls and two explicitly owner-approved D-0017 firmware exceptions continue to apply. The WUD retained-diagnostic declaration control is disclosed in [the fake-match ledger](../FAKEMATCHES.md).

## Why the data metric changed

The initial raw report measured 1,245,840/1,788,360 bytes (69.663826%), despite complete source selection and an exact linked executable. The completed corrections establish original object identities and boundaries using final addresses, source ownership, original bytes, literal terminators, RTTI/vtables, exception consumers and normalized relocations. No result JSON is rewritten.

- [Game boundaries](game-data-boundaries-20261105.md), [identities](game-data-identities-20261105.md), [BSS](game-bss-data-20261105.md), [small data](game-sdata-20261105.md) and [Selector](selector-data-matching-20261105.md) document original records and compiler correspondences.
- [SDK](sdk-data-matching-20261105.md), [BTE/HBM](bte-data-matching-20261105.md), [NW4R](nw4r-data-boundaries-20261105.md), [runtime](runtime-data-matching-20261105.md), [literal tails](retained-literal-tails-20261105.md) and [exception corrections](exception-data-matching-20261105.md) record the remaining extents, genuine pointer relocations and packed values previously misclassified as pointers.
- The only reconstructed implementation edit replaces WUD's unreferenced pointer table with two explicitly retained diagnostic arrays. Their original string bytes are unchanged. The declaration spelling remains unproven and is disclosed under D-0016.
- [Native data alignment](native-data-alignment-20261105.md) proves seven physical gaps totaling 37 zero bytes. Twenty were already outside the old denominator; exactly seventeen newly leave it. Native source alignment regenerates every byte. No genuine object is excluded, and the frozen artifact and expected hashes are unchanged.

The checked-in `config/wpse-v1/objdiff-mappings.json` is authoritative for ordinary configure output. The build rejects malformed mappings, duplicate keys, duplicate destinations and unknown units. Mapping generators require unique names on both sides; anonymous compiler names duplicated across sections cannot safely identify an object by name alone. Data and exception auditors produce diagnostics only and cannot replace build objects or reports.

## Comparator correction

Upstream objdiff 3.8.2 pairs explicitly mapped symbols but does not honor that identity when comparing data relocation destinations. Discarded weak metadata can move the correct compiler object within its pre-link section. The pinned `3.8.2-rumble-data-mappings-2` comparator recognizes the verified identity while preserving payload, relocation type, target and addend checks. A narrowly checked section-pool form requires a uniquely mapped, equally sized nonempty object in the same section and exactly equal effective destination addresses. The unmapped comparison path is unchanged.

`config/objdiff.json`, `config/toolchain.json`, the digest-checked LF patch recipe and `tools/setup/acquire_objdiff.py` reproduce the comparator from pinned upstream sources. The final executable is 9,139,712 bytes, SHA-256 `ee60619f592086703c25c122e9445a898e418da154e8f5300d4d5b4ff98b9c11`. Its recipe SHA-256 is `3d19aa223c021ef8d04893b7f1db4e3e884b9b3ba8364c879bedeef5c4680e93`. Two fresh builds and a fresh offline acquisition rebuild produce identical executables. Source, Cargo and generated-source paths are remapped before compilation; there is no executable post-processing.

Independent real-CLI review reruns seven exception controls and twelve pooled-pointer cases. Wrong data bytes, destination identities, relocation types, addends, sizes, symbol kinds, ambiguous names and incompatible sections remain nonmatching or are rejected. An entirely unmapped 897-unit report is byte-identical to upstream output, and function arrays remain unchanged. [Comparator acquisition](exception-symbol-mapping-20261105.md) and [independent controls](data-metric-tooling-20261105.md) preserve the detailed evidence and historical revision-1 limitation, which revision 2 resolves.

## Clean end-to-end acceptance

The coordinator preserved the verified incremental build, moved the complete build output aside, and ran the actual `build.bat` entry point from an absent `build/wpse-v1`. After the first run exited zero, its output was preserved and the entire procedure was repeated. Each clean run performed 923 Ninja steps and passed the frozen direct verifier. The second verification receipt records `2026-11-05T08:58:46Z`. The original and rebuilt DOL are each 4,094,880 bytes and match the frozen SHA-256 exactly.

Direct byte comparisons establish equality across the incremental output and both clean outputs for every artifact below. The report is generated by the pinned comparator through the normal build, not imported from an audit workspace.

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| `main.dol` | 4094880 | `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da` |
| `main.elf` | 6036352 | `f5abbda3a563a738c116c91d533c88edee5f30b304e9e6084a239e214079b9bc` |
| `report.json` | 4570494 | `a682eb83660a1901e36056c36a51ad3e2814057052d36d3164494830602ad19a` |
| `assets/system_message.arc` | 11488 | `e27d380bbad3aff2ef2d41cfdc5b77590390a68937fef6edc87502f2d1813161` |
| `include/system_message.inc` | 68928 | `fd3a483529c348ffc3eb9b27ee3d59f43268b17e8ae9417fa16f827bacbe76d5` |
| `dsp/ax_dsp.bin` | 8192 | `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed` |
| `include/ax_dsp.inc` | 34816 | `d3c3f8e6f3f8cc6d4fe16899eb479ab8dcde0ee2c3e2568644c12f108c122fdd` |
| `dsp/os_dsp_init.bin` | 128 | `5b873ee91f33b0884574b814d7f46d81e283394de91b8b2ea9073ac218dee350` |
| `include/OSDspInit.inc` | 544 | `32f75890290310373df7d5b37b18d606fc48f86853076136fdfa49dfd8e733f4` |
| `firmware/wud_patch.bin` | 188 | `fde41feb3ac0bab076e24e613fd40768e3c099bcd7f34c1f746129188990ebae` |
| `firmware/wud_patch_commands.bin` | 92 | `3a5f6a9f6a120cb10a4224a4bcc35db8e75459d62ebaf5b9879180b9e8de5f2c` |
| `include/wud_patch_data.inc` | 1176 | `076270b14cbba7e4b7b39b3f930f494217446479395abbb7be5e8a4b35f3f0a8` |
| `include/wud_patch_commands.inc` | 663 | `d1fb5dc01a21ff94873e35effa4f0e5b6c27a93366287d0b9980befd90dada70` |

The actual `data_match_inventory.py` CLI reconciles all section credit and reports zero unmatched bytes across zero units. Both native-alignment CLI audits pass: 470 established code ranges/3,792 bytes and seven data ranges/37 bytes, all zero in the original and identical in the rebuild. The final tool/source review found no material blocker. No unit tests, source comments or docstrings were added. Progress images were regenerated from this final report. Original files and frozen manifests have no changes.

Private repeat receipts are `tools/local/data-match-20261105/clean-one-artifacts.json` and `clean-two-artifacts.json`; full logs are `clean-one.log` and `clean-two.log`. These scratch records are supplementary: the table, tracked configuration, tooling recipes and source provide durable acceptance evidence.

## Traps

- Explicit symbol pairing and relocation identity comparison are separate operations. A mapped symbol can still lose credit when its pointer is compared by an obsolete pre-link offset. Preserve all byte/type/offset checks and prove any comparator correction with corrupted-input controls and the frozen full artifact.
- A deterministic host-link flag alone does not remove generated-source paths. Remap the Cargo target directory as well as source and registry paths before expecting independent Rust builds to hash identically.
- A report function count can include an alignment entry. Check full numeric-address padding names and actual bytes; ordinary `gap_conn_*` library names are not evidence of padding.
