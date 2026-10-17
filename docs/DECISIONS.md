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

## Future decisions

Use stable identifiers and record status, problem, approach, evidence, consequences, and superseded decisions. Put reverse engineering evidence in `re/` and link it here rather than duplicating it.
