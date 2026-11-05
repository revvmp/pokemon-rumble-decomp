# Independent DSP build-tool review

## Scope and result

The reviewed private packets are `tools/local/tail-20261104/selector/DSP/build_packet/build_dsp.py` and `tools/local/tail-20261104/sort/DSP/tool_packet/`. The first builds a DSP binary, C initializer, and dependency file. The second acquires pinned compiler and assembler sources, builds the host assembler, and verifies an installed copy. This review found no remaining blocking issue in those packets after the corrections below. It does not grant source credit or substitute for the coordinator's production integration and whole-game verification.

The final wrapper SHA-256 is `2412ea532274c68b9ae3c8a23dad101cce81bc789b496726d37c015668e36447`. The reviewed acquisition script SHA-256 is `d846653eee11859d834cf9d43dfd4b3c2e4839f9600b2eb279a3581514fc8e22`; its manifest SHA-256 is `efc47f901f8ab131a2b85f4e9c9ee5d186f89128b1869d9c7d930b973d472593`.

## Dependencies and publication

The wrapper follows the pinned assembler's global `INCDIR` state through nested includes and both assembly passes. Its supported dependency syntax is a literal `INCLUDE` or `INCDIR` path. It rejects missing inputs, recursive includes, unsupported path expressions, input/output aliases, and overlapping outputs before publication. The complete refined program has 29 source inputs; the wrapper and assembler add two tool prerequisites. Depfile targets retain the caller's path spelling, while source prerequisites are resolved paths. The owner's actual Ninja exercise confirms all 31 prerequisites for both generated outputs, including escaped spaces, hashes, and dollar signs, and confirms that an edited nested include rebuilds the image.

The scanner now mirrors the assembler's tab-to-space normalization inside quoted paths. An independent actual CLI exercise includes a source using a tab inside its quoted filename and confirms that both dependency discovery and the assembler select the existing filename containing a space.

All generated artifacts are staged before publication. The review identified a rollback defect in the earlier wrapper: one failed restore could stop recovery and allow later cleanup to delete backups that had not been restored. The final version attempts every restore, preserves any backup whose restore fails, and reports its recovery path. The owner's real Windows locked-depfile exercise rejects publication and restores the preceding binary and initializer. Individual replacement is atomic; a transaction across process termination or filesystem failure is not claimed.

The acquisition helper now stages its executable and installation report before replacing either, retains prior outputs for rollback, and reports the original publication error plus any unsuccessful recovery and backup path. The host driver explicitly flushes and closes its output stream before returning its status, so a buffered output failure is reported through the CLI. These corrections were reviewed in the final packet.

## Inputs and platform

Neither build path reads an original DOL, DSP image, frozen content hash, or original split object. The assembler receives readable source; the wrapper converts the produced bytes into the requested C element width without modifying the binary. The independent comparison scripts read the frozen image only after assembly as verification evidence.

All 22 pinned inputs independently passed their recorded digests and sizes where specified: 17 upstream source/license files, the Zig archive, and four normalized authored host files. The compiler archive is verified before extraction; extracted paths cannot escape the fresh compiler directory, and symbolic-link archive members are rejected. Each build uses fresh source and compiler directories. The reduced `DSPCore.h` transform retains the upstream notice and extracts the required enum declarations from a digest-verified input. Authored host files and Python tools contain no code comments or docstrings; downloaded upstream notices remain in ignored tool staging.

The supported host is Windows x86-64 with the Universal CRT available and Python 3.10 or newer. The package uses pinned Zig 0.13.0 and does not require an installed Visual Studio compiler. Independent PE inspection finds machine `0x8664`, PE32+, timestamp zero, and imports from `KERNEL32.dll` and `api-ms-win-crt-*` API sets. This is an explicit Windows host tool, not a claim of a self-contained executable for every operating system.

The tool owner completed two clean builds in separate paths. I independently compared their published executables: both are 574,464 bytes with SHA-256 `b9afa1a0a47628f6b15464394792707f8a1bc1ba7f27be064b18801d762d47fd`. Independent `--offline` verification of one installed copy exits zero without changing its executable or report. An offline invocation with absent cache/install paths exits one and creates neither path. These results are recorded in `tools/local/tail-20261104/layout_dsp/portable-review/acquisition-review.json`.

## Independent refined-source assembly

`tools/local/tail-20261104/layout_dsp/review_portable_tool.py` invokes the final installed portable assembler and wrapper through their actual CLIs. Its master is `tools/local/tail-20261104/smallgame/DSP/complete_source_refined/ax_dsp.ds`, which includes the small naming improvements from the [source review](ax-dsp-source-review-20261104.md). The evidence is `layout_dsp/portable-review/review.json`, including all source hashes, commands, exit codes, and PE imports.

The direct assembler and both wrapper formats reproduce every one of the 8,192 frozen DSP bytes, SHA-256 `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`. The 16-bit initializer has 4,096 big-endian words; the 8-bit initializer has 8,192 byte values. Each reconstructs the identical binary. A rejected expected-size check preserves all three accepted outputs and leaves no staged files for either format. No refined source changed during the check.

The two instruction-memory table ranges also match separately: 370 bytes at word addresses `0x0D02` through `0x0DBA` and eight bytes at `0x0F95` through `0x0F98`. Their combined coverage is 189 words, or 378 bytes. This independently confirms that the complete table inventory survived the refined source and new host-tool build.

The owner's broader evidence remains separate: the wrapper's 20 real CLI/Ninja cases are in `selector/DSP/build_packet/build_wrapper_evidence.json`; host acquisition/build checks are in `sort/DSP/tool_validation/validation.json`. This review confirms the source-to-DSP encoding and tool behavior. Original-language provenance, production dependency closure, and the final native executable remain the coordinator's acceptance gates.

## Canonical production acceptance

After the coordinator accepted the production integration and completed two successful clean `build.bat` runs, I audited the final stable build independently. All 29 files under `src/RVL_SDK/ax/dsp/` are byte-identical to the frozen production packet and its recorded inventory. The seven canonical host/configuration/wrapper files are byte-identical to their reviewed packets, and the installed assembler retains its pinned digest. The ordinary `AXDSPCode.c` owner declares the aligned 4,096-element `u16` array and includes the generated initializer from `build/`; it contains no copied numeric opcode initializer. No raw opcode-word directive, binary inclusion directive, or public generated DSP binary/initializer appears in the audited inputs.

The canonical wrapper and installed tool also assemble the canonical nested source tree successfully into a separate private output. The production binary remains exactly 8,192 bytes with the frozen DSP SHA-256 above, and its 4,096 generated words reconstruct those bytes. Both production Ninja outputs have 31 valid dependencies: every one of the 29 canonical source files, the wrapper, and the installed assembler.

The final-link receipt contains **897 source-selected units and 902 raw input objects**. Every raw object is under `build/wpse-v1/src/`; every `main_link/unit_*.o` alias is byte-identical to its corresponding compiled source object. The final response file contains exactly those 902 aliases, with no original split-object input. Independently hashing the resulting 4,094,880-byte `main.dol` gives the frozen `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The reproducible read-only audit is `tools/local/tail-20261104/layout_dsp/audit_production.py --ninja-deps`. Its private receipt, `layout_dsp/production-audit/source-coverage.json`, records all canonical source/tool hashes and all 902 source-to-alias comparisons; receipt SHA-256 is `79caf35c39e7ff492f5420725370f1dd796a7e76988c6ffa78a45d4ef44e1a2a`. This establishes the production DSP dependency closure and source-selected main-executable link. The frozen target's scope still excludes the separate boot loader, data contents, and WAD packaging; the original-language provenance qualifications remain unchanged.

## Traps

- A failed publication can be followed by a failed rollback. Continue recovery for every previously replaced output and preserve any original backup that could not be restored; deleting it during cleanup loses the accepted result.
- A successful buffered `write` does not establish successful file closure. Flush and close the host assembler output before returning its status.
- A portable compiler package removes a local compiler installation requirement, but the produced executable can still depend on operating-system runtime libraries. Inspect PE imports before describing its host requirements.
