# Decisions

## D-0001: Separate byte identity from source coverage

Accepted during initialization. Completion requires both exact artifacts and reconstructed source. A copied original may match perfectly without any source reconstruction. Comparison scope must be reviewed against a complete release inventory.

## D-0002: Documentation contains explanations

Accepted from the project requirements. No unit tests, code comments, or docstrings. Use readable code and Markdown explanations. Preserve the original `.gitignore` content and section headings at the user's explicit request; organized `.gitignore` headings are the exception to the configuration-comment restriction.

## D-0003: Keep all substantive documentation under docs

Accepted from the project requirements. Use `docs/` for project documentation and `docs/re/` for anything reverse engineering related. Root `AGENTS.md` and README are short discovery entry points. Executable code and machine-consumed build configuration retain their conventional separate directories.

## D-0004: Establish original build requirements through evidence

Accepted during initialization. Compiler, linker, flags, release identifiers, and original hashes remain unknown until investigated. No placeholder build or speculative game implementation will be presented as working.

## D-0005: Freeze expected fingerprints

Accepted during initialization. Store reference size and SHA-256 in versioned manifests. Fingerprinting is explicit and refuses previously sealed manifests. Verification never changes expectations and additionally performs a direct byte comparison.

## D-0006: Preserve session lessons and prefer end-to-end validation

Accepted from the project requirements. Every meaningful session must keep the traps and findings section of root `AGENTS.md` current, with concise actionable lessons linked to detailed evidence. This makes recurring mistakes visible before future work starts. Record when no new lesson emerged rather than inventing one. Prefer end-to-end tests through real tooling and build entry points. End-to-end suites are allowed; unit tests, code comments, and docstrings remain prohibited.

## D-0007: Evaluate the dtk-template ecosystem

Accepted. The extracted executable is a PowerPC DOL that dtk analyzes (F-0003), and a dtk-template build reproduces it exactly (F-0010). Resolutions of the former open items:

1. Vendored dtk-template tooling is copied from upstream commit `95a941f755919ebe50c1725a4ce73524470e7a02` (CC0) by `tools/vendor/vendor_strip.py`, which removes comments and docstrings and verifies the result has the same syntax tree as upstream. The only behavioral adaptation is `tools/build/decompctx.py` resolving the repository root two directories up, because it lives in `tools/build/`. To update, rerun the stripper on a newer upstream checkout and reapply that adaptation.
2. dtk-template's `config/<GAMEID>/`, `orig/<GAMEID>/`, and `build/<GAMEID>/` map directly onto `config/<target>/`, `orig/<target>/`, and `build/<target>/` with target `wpse-v1`. `configure.py` sets `config.tools_dir` to `tools/build`.
3. `config/<target>/build.sha1` is dtk's build-time check. Acceptance remains `tools/verify/verify.py` against `config/targets/wpse-v1.json`.
4. Non-matching status and decomp.me links go in the [reconstruction ledger](PROGRESS.md#reconstruction-ledger), not source comments.

## D-0008: Generate progress images in repository

Accepted at the project owner's request. The project owner reports that decomp.dev, the hosted progress service used by Petari, is being sunset; this was not independently verified. Progress images are therefore rendered locally by `tools/progress/progress.py` from `config/progress.json` and, once a denominator exists, an objdiff `report.json`, then committed under `docs/images/`. Consequences: no external service, API key, or CI upload is needed; images must be regenerated manually or by future CI when state changes; and code percentages appear only from a real objdiff report, preserving the rule against percentages without a denominator. See [progress images](PROGRESS.md#progress-images).

## D-0009: Organize tools by category

Accepted at the project owner's request. `tools/build/` holds the vendored build system, `tools/reference/` reference extraction, `tools/verify/` acceptance verification, `tools/progress/` progress images, `tools/vendor/` the vendoring stripper, `tools/collab/` issue claiming and worktrees, and ignored `tools/local/` acquired binaries. Root `configure.py` remains the build entry point.

## D-0010: Match the original source language and aim for Petari-level fidelity

Accepted at the project owner's request. Each unit is written in the language of its original: C++ for game code, NW4R, and the CodeWarrior runtime, as shown by C++ exception tables and `.cpp` source names in the executable; C for RVL_SDK and MSL. Source must read like clean original code with properly named classes, following the standard in root `AGENTS.md`, while respecting the no-comments rule.

## D-0011: Multi-contributor workflow

Accepted at the project owner's request. Work is divided into GitHub issues by library or workstream, with file-level claims inside large issues. Each contributor or agent works in its own branch and worktree. To prevent conflicts:
- the reconstruction ledger is split into per-area files under `docs/ledger/`
- new findings are separate files under `docs/re/findings/`, with IDs derived from issue numbers
- shared summaries, phase state, and progress images are maintainer-only and updated after merge

Session-by-session updates to `PROGRESS.md` and `HANDOFF.md` therefore move to pull request descriptions for contributors. See [collaboration](COLLABORATION.md).

## D-0012: Track the shared IDA database

Accepted at the project owner's request. `docs/re/main.elf.i64` is an IDA Pro database of `build/wpse-v1/main.elf` (49,316,513 bytes when first committed), and `*.i64` is no longer ignored. Consequences:
- Git cannot merge IDA databases. Concurrent edits mean one contributor's work is discarded, so only one person updates the database at a time, coordinated through its owner.
- Every committed save adds the full database to history.
- `config/wpse-v1/symbols.txt` remains the authoritative, mergeable source of names. Names recovered in IDA must also be synced there.

## D-0013: Shared RVL_SDK headers come from Petari

Accepted to stop parallel contributors from writing duplicate and conflicting SDK headers.
- `include/revolution/` and `include/private/` hold the RVL_SDK headers, laid out as in Petari.
- Headers that contributors had already adapted and matched against were kept unchanged.
- Every other Petari RVL_SDK header was imported from Petari commit `e0ab141` (CC0) with comments stripped: every header not already present, about 135 files. The VF headers were left out because this game has no VF library.
- `tools/vendor/vendor_strip.py` gained C/C++ support for the stripping. It refuses output whose code differs from the original apart from comments and whitespace.

Consequences:
- Contributors use and extend these headers in place instead of writing new ones.
- `include/RVL_SDK/` is deprecated. Its headers moved to `include/revolution/`, leaving one-line forwarding headers so open pull requests keep building. `include/RVL_SDK/gx/GXFifo.h` still differs from `include/revolution/gx/GXFifo.h` and is kept until its unit's owner reconciles them.
- Remove the forwarders once no open pull request uses the old paths.
- An imported header may still need adjusting for this game's SDK version; adjust it in place.

## D-0014: Import library names from reference symbol maps

Accepted to stop contributors from naming the same outside callees one by one and conflicting over the renames. `tools/re/import_reference_symbols.py` aligns ordered function and data sizes in each library range against pinned public symbol maps and renames only `fn_`, `lbl_` and `dtor_` placeholders, never a real name, and never to a name already in use. Its method and measured accuracy are [F-0013](re/FINDINGS.md#f-0013-reference-symbol-maps-name-library-functions-by-size-alignment); its sources and licenses are in [reference symbol map sources](re/licenses/reference-symbol-maps.md).

- High- and medium-confidence names are applied; low-confidence names are reported only.
- Every applied name is recorded with its tier, method and sources in `docs/re/reference-symbols.tsv`, and derived file spans in `docs/re/reference-files.tsv`.
- An imported name is evidence of medium strength. A contributor whose matched source or code evidence shows a different name or signature renames the symbol and notes it in their findings file.
- Maintainers rerun the tool after new references or revisions appear. It only touches placeholders, so a rerun does not undo contributor names.

## D-0015: Assembly source only where the original was assembly

Accepted with the repository owner's approval so that MetroTRK's hand-written exception vector table can be built from source. `configure.py` builds a `.s` unit with the pinned binutils `powerpc-eabi-as` (`config.asflags`, then `dtk elf fixup`), the same path dtk-template uses for its generated assembly; no new tool is needed.

- An assembly-source unit is allowed only where evidence shows the original unit was assembly, such as `__exception.s` (`gTRKInterruptVectorTable` to `gTRKInterruptVectorTableEnd`, a fixed-offset vector table no compiler emits). Compiled code must never be recovered as assembly.
- The source must read as hand-written assembly: real symbol references with relocations instead of absolute addresses, named macros and constants for repeated vector bodies, `.org` for vector offsets, and no comments.
- The unit follows the same acceptance rule as any other: 100% in objdiff and a full build plus `verify.py` PASS before it is marked `Matching`.
- The first such unit is `src/MetroTRK/debugger/embedded/MetroTRK/Processor/ppc/Generic/__exception.s`. Its symbol is a function spanning the whole table so objdiff can compare it; dtk keeps the original label beside it.

### 2026-11-04 application to AX DSP

The independently reconstructed AX DSP program meets this decision's original-language boundary. The target is identified as RVL AXWii by its matched host configuration and command protocol, corroborated by Dolphin's image identity. Pinned archived SDK inventories explicitly name the AX DSP assembly inputs, includes, assembler and linker. That concrete family-level language evidence, together with the identified target, supports using symbolic assembly here. It does not authenticate the archive as an official publication or recover the exact original source revision. The [provenance record](re/findings/ax-dsp-identities-20261104.md#original-language-provenance) preserves these limits.

The 29-file source tree uses actual routine labels, semantic state and parameter-block names, symbolic dispatch tables and shared processing kernels. An independent [source review](re/findings/ax-dsp-source-review-20261104.md) and [staging inventory](re/findings/ax-dsp-staging-20261104.md) establish readability and exact full-image assembly separately from the language evidence. This application does not authorize assembly recovery of compiled C/C++ or automatically cover other embedded firmware arrays.

The PowerPC assembler cannot encode this processor. The normal build therefore uses a [separately pinned Dolphin DSP assembler](re/findings/ax-dsp-host-tool-20261104.md), built from pinned source with portable Zig. `tools/dsp/build_dsp.py` assembles source and emits a generated big-endian word initializer; the ordinary Wii compiler owns the host array in AXDSPCode.c. No original executable is an input to this rule. Native final-link acceptance remains mandatory; generated instruction bytes are outputs, never reconstruction inputs.

## D-0016: Documented fake matches for the remaining long tail

Accepted by the repository owner on 2026-10-22 to finish the last functions. Honest reconstruction stays the default; a fake match is a last resort for a function whose register allocation, scheduling, or emission order no plausible source has reproduced.

- Allowed only after at least one documented honest attempt, recorded in a findings file, and preferably after the recovered rules in [CODEGEN_RULES](re/CODEGEN_RULES.md) have been applied.
- A fake match is source that compiles to the original bytes but is probably not what the developers wrote, such as an extra temporary, a reordered or redundant statement, a cast, a probe declaration, an explicit instantiation, `volatile`, or a per-function `#pragma`. It must still be C or C++ source; compiled code is never recovered as assembly (D-0015).
- Source still carries no comments. Every fake match is listed in [the fake-match ledger](re/FAKEMATCHES.md) with its unit, function, the fake construct, and a link to the honest attempts, so it can be replaced with plausible source later.
- The acceptance rule is unchanged: 100% in objdiff and a full build plus `verify.py` PASS before a unit is marked `Matching`. Progress reports count fake-matched functions as matched; the ledger is the record of which ones they are.

## D-0017: Two owner-approved embedded-firmware assembly exceptions

Accepted by the repository owner on 2026-11-04 with the explicit reply, "Approve these two assembly exceptions." The review disclosed that their original authoring language is unproven and that the Bluetooth reset helper may have been compiled from C. The concrete readable sources, byte comparisons and integration evidence were available before approval.

This is a narrow exception to D-0015's original-language requirement for exactly two existing embedded payloads:

- OSAudioSystem's 128-byte `DSPInitCode` bootstrap at PowerPC address `0x803C7770`, including its vector entries and reserved tail.
- WUD's controller patch for `BCM2045B3_002.003.014.0167`: the 180-byte payload loaded at controller address `0x89970` and seven eight-byte replacement blocks. The host-side 188-byte download record and 92-byte install record include typed protocol fields as well as the generated instructions.

The source must remain readable symbolic assembly with named routines, state, hardware fields and external firmware targets. Protocol metadata and literal pools remain explicitly typed or named. Machine-code arrays, original object imports, instruction patching and changed expected hashes are not accepted as reconstruction inputs. Pinned assembly/link tools, unchanged native compiler outputs, complete artifact verification and repeated clean builds remain required.

The exception does not claim that these sources reproduce the original language or exact original source text. It does not apply to any other compiled CPU or firmware code. D-0015 and D-0016 remain unchanged outside these two listed payloads. [The OS evidence](re/findings/os-dsp-init-20261104.md) and [embedded-code audit](re/findings/embedded-code-audit-20261104.md) retain the provenance limits, source partition and exact comparisons.

## Future decisions

Use stable identifiers and record status, problem, approach, evidence, consequences, and superseded decisions. Put reverse engineering evidence in `re/` and link it here rather than duplicating it.
