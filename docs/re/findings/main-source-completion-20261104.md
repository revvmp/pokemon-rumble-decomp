# Main executable source-completion acceptance

The later [2026-11-05 data acceptance](matched-data-completion-20261105.md) completes the separate Matched Data metric and repeats both clean builds. Its independent function audit corrects this historical record's real-function count: 13,462 is the raw report count, including one twelve-byte zero-alignment entry; 13,461 are real functions. The earlier measurements and hashes below remain checkpoint evidence.

The matching reconstruction of `wpse-v1/main.dol` is complete under the project's documented D-0015/D-0016 controls and the two explicitly owner-approved D-0017 firmware exceptions. All 13,462 CPU functions are exact, and every selected final-link input comes from reconstructed source. Two full clean builds reproduce the frozen executable and all thirteen compared artifacts.

## Measured completion

| Measure | Accepted result |
| --- | --- |
| Real CPU functions | 13462/13462 exact |
| Matched CPU code | 3078460/3078460 bytes, 100% |
| Source-linked CPU code | 3078460/3078460 bytes, 100% |
| Source-selected data | 1788360/1788360 bytes, 100% |
| Source-selected logical units | 897/897 |
| Raw native linker inputs | 902 source compiler objects; every alias byte-identical |
| Original split objects selected | 0 |
| Previously exact functions missing or regressed | 0 |
| Pre-link object matched-data diagnostic | 1245840/1788360 bytes, 69.663826% |

The final row remains the real objdiff object-level measurement. It was not changed to 100%. The complete linked DOL, including every stored data byte, passes direct comparison and its frozen SHA-256. Source selection, object diagnostics, source fidelity and complete artifact identity are distinct evidence, all retained here.

The denominator excludes the independently documented 470 standalone native alignment entries, totaling 3,792 bytes, and the previously corrected data-padding extents. Those exclusions add no instruction credit and alter no original artifact or expected hash. [The counting rules](../../MATCHING.md#counting-method), [alignment audit](native-alignment-padding.md) and [storage evidence](anonymous-data-20261104.md) preserve that history.

## Source and provenance

The final CPU unit is PiiPropData. All 48 raw bodies, 504 normalized relocations and 26 exception records match. Primary MSL initialization/copy boundaries resolve its last two functions; documented native source fragments preserve text, exception and literal ordering. [Primary-source recovery](piipropdata-primary-msl-tail.md), [emission acceptance](piipropdata-emission-tail.md) and [the fake-match ledger](../FAKEMATCHES.md) distinguish evidence-backed source structure from D-0016 controls.

The 8,192-byte AX DSP program is independently reconstructed from 29 symbolic source files. Its D-0015 original-language evidence is the identified AXWii target plus concrete archived AX assembly inventories; the archive's authenticity and exact source revision remain qualified. [AX acceptance](ax-dsp-production-20261104.md) links the source, compiler and independent review records.

The pre-existing 128-byte OS DSP initialization array and WUD controller patch were discovered separately inside already Matching C units. The owner explicitly approved symbolic assembly for those two payloads after reviewing the uncertain original language and the possibly compiled Thumb helper. [D-0017](../../DECISIONS.md#d-0017-two-owner-approved-embedded-firmware-assembly-exceptions) is narrow and does not establish original source-language recovery. Their programs now generate from named assembly routines, real linked symbols and typed transport records. Neither source generator reads an original executable, original object or expected payload hash. The ordinary OS and WUD compiler objects are byte-identical before and after the change.

The WUD download record is 188 bytes and its install record is 92 bytes: together they contain 194 Thumb instruction bytes, 40 literal bytes, two alignment bytes and 44 protocol bytes. That partition follows actual instruction references and literal loads. The target controller ROM remains an external dependency of the original firmware protocol, not hidden code copied into the reconstructed payload. See [the code audit](embedded-code-audit-20261104.md), [OS proof](os-dsp-init-20261104.md), [native Thumb tool proof](wud-thumb-zig-20261104.md) and [independent source review](embedded-firmware-source-review-20261104.md).

The embedded system-message archive builds from recovered editable JSON and PNG records. These are recovered asset representations, not original authoring files; [its provenance](system-message-archive-20261104.md) remains explicit. The existing dtk workflow still reads orig/wpse-v1/main.dol for analysis, layout and comparison. It supplies no selected original object to the final link. No private probe script, scratch object, generated opcode initializer or running disassembler is required by the normal build.

## Build and verification

`build.bat` is the complete validated entry point: acquire/check pinned tools, install/use ninja, configure, build, and verify. `config/toolchain.json` pins the CPU toolchain; `config/dsp_assembler.json` pins the portable Zig compiler, Dolphin/fmt source receipts, owned host inputs and exact DSP assembler executable. Two fresh host-tool builds reproduce the pinned binary. External sources and licenses remain in ignored local tool storage.

The AX and OS DSP generators validate source/include closure and publish generated binary/includes with dependency records. WUD uses the same pinned Zig installation for Thumb assembly, native ELF linking at the controller's fixed addresses and ordinary section extraction. Its typed records and includes are published only after section/size/input-stability validation. The rules have 31 AX, three OS and fifteen WUD dependencies. Relevant real CLI/Ninja checks cover edited-source rebuilds, no-op builds, malformed input, missing include, wrong digest/size, collisions, concurrent input mutation and locked-output rollback. See [DSP checks](ax-dsp-build-wrapper-20261104.md), [host-tool checks](ax-dsp-host-tool-20261104.md) and [WUD checks](wud-build-wrapper-validation-20261104.md). No unit tests or compiler/object patches were introduced.

The final two clean builds moved the entire previous build/wpse-v1 directory to separate verified workspace backup paths, then invoked build.bat from the repository root. Both entry points exited zero and the frozen verifier passed. Every artifact below is identical between both clean builds and the preceding incremental build:

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| `main.dol` | 4094880 | `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da` |
| `main.elf` | 6036304 | `bd7b247419913824c9f91747c129b9092433cf7ed0cbbea48ce643c232200ee3` |
| `report.json` | 4561010 | `c88be97be32c594219052939c0a613ccc4c82eec59cd4ca6f0942421a29ef54e` |
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

The complete logs and byte-comparison receipts are `tools/local/tail-20261104/firmware-clean-one.log`, `firmware-clean-two.log` and their `.json` receipts. The independent original-address audit confirms zero regressions or missing exact functions, all 902 raw-input aliases remain unchanged, and the final alignment audit retains all 470 original zero ranges. Source/tool packet audits additionally confirm the build consumes the reviewed canonical files.

## Scope

This accepts the main game executable and its embedded source/data generation under the disclosed decisions. It does not claim an exact original-source recovery, a firmware-original-language recovery for D-0017, or a whole release/package reconstruction. Boot-loader content 6, other content assets and WAD packaging remain outside config/targets/wpse-v1.json. No Windows/WebAssembly port is included. Original artifacts and frozen acceptance hashes remain unchanged.
