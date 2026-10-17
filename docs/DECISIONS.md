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

## Future decisions

Use stable identifiers and record status, problem, approach, evidence, consequences, and superseded decisions. Put reverse engineering evidence in `re/` and link it here rather than duplicating it.
