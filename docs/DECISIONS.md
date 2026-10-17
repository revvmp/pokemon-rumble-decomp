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

Proposed. Problem: the build driver, splitting, and diffing approach are undecided, and building them from scratch would duplicate mature community tooling. Approach: adopt the decomp-toolkit, objdiff, ninja, and `configure.py` workflow used by Petari and most GameCube and Wii decompilations, as described in [prior art](re/PRIOR_ART.md) and sequenced in the [roadmap](ROADMAP.md).

Acceptance gate: evidence that the extracted executable is a PowerPC DOL that dtk can analyze, recorded in findings. Until then this is a direction, not a commitment, and nothing may be written to `config/toolchain.json` on its basis.

Open items to settle before vendoring: (1) dtk-template's `configure.py` and `tools/*.py` contain comments, which conflict with D-0002; either strip them or record a narrow exception for unmodified third-party files. (2) Map dtk-template's `config/<GAMEID>/`, `orig/<GAMEID>/`, and `build/` onto our `config/<target>/`, `orig/<target>/`, and `build/<target>/`. (3) dtk's `build.sha1` is a build-time check only; acceptance remains the SHA-256 manifest verified by `tools/verify.py`. (4) Non-matching status and decomp.me links go in the reconstruction ledger rather than source comments.

## D-0008: Generate progress images in repository

Accepted at the project owner's request. The project owner reports that decomp.dev, the hosted progress service used by Petari, is being sunset; this was not independently verified. Progress images are therefore rendered locally by `tools/progress.py` from `config/progress.json` and, once a denominator exists, an objdiff `report.json`, then committed under `docs/images/`. Consequences: no external service, API key, or CI upload is needed; images must be regenerated manually or by future CI when state changes; and code percentages appear only from a real objdiff report, preserving the rule against percentages without a denominator. See [progress images](PROGRESS.md#progress-images).

## Future decisions

Use stable identifiers and record status, problem, approach, evidence, consequences, and superseded decisions. Put reverse engineering evidence in `re/` and link it here rather than duplicating it.
