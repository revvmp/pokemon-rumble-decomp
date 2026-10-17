# Project rules

These rules apply to humans and LLMs throughout the repository.

## Objective and constraints

- Reconstruct readable source that produces exactly the bytes of a specifically identified original release. Never claim a match without a passing verification against a frozen reference manifest.
- Never claim a full decompilation solely because binary hashes match. Source coverage and artifact identity are separate requirements.
- Do not create unit tests, unit-test suites, or unit-test infrastructure. Prefer end-to-end tests through real entry points and actual outputs; end-to-end suites are allowed when they validate meaningful workflows.
- Do not add comments to code, scripts, configuration, assembly, or build files. Do not use docstrings as substitute comments. Put explanations in Markdown. Preprocessor directives and literal text required by a file format are not comments.
- Preserve original `.gitignore` content and use organized section headings there, as explicitly requested. This narrow exception does not permit comments in source or other configuration files.
- Keep code readable through descriptive names, small functions, explicit data structures, and consistent formatting. Preserve verified ABI and matching constraints; explain any resulting readability tradeoffs in documentation.
- Do not invent release identifiers, original hashes, symbols, types, compiler versions, flags, addresses, or progress measurements. Record unknowns explicitly until evidence resolves them.
- By explicit user instruction, all files under `orig/` may be tracked and committed. Keep original files immutable. Outside `orig/`, keep original files, extracted proprietary data, keys, analysis databases, and tool binaries in ignored local paths; do not commit them. The one exception is the shared IDA database `docs/re/main.elf.i64`, tracked by explicit user decision ([D-0012](DECISIONS.md#d-0012-track-the-shared-ida-database)).
- Never modify reference artifacts or expected hashes to make a reconstruction pass. Different releases get separate target manifests.
- Do not count copied original executable bytes, embedded machine code, original objects, or assembly fallbacks as decompiled source.
- Inspect repository status and relevant instructions before editing. Preserve unrelated work.
- Do not add speculative SDK declarations, guessed build flags, fake build commands, or dummy game implementations to populate the layout.

## Session workflow

1. Read `AGENTS.md`, this file, architecture, progress, handoff, and relevant findings and decisions. Inspect `git status --short`.
2. Select a bounded task with a clear outcome and affected target.
3. Read existing code and evidence. Separate facts from hypotheses. Preserve addresses, offsets, and tool versions in reverse engineering findings.
4. Make the smallest coherent change and document reasoning outside code.
5. Prefer end-to-end validation: invoke real CLI workflows for tooling and real build-to-verification pipelines for reconstructed code, checking exit status, outputs, and exact binary identity. Use syntax checks and runtime observations as appropriate supplements. Exercise relevant failure paths. Do not introduce unit tests, replace the real workflow with mocked internal calls, or report checks that did not run.
6. Update the living documentation before finishing. State what changed, what was verified, and what remains blocked.

## Multi-contributor work

When several contributors work in parallel, [collaboration](COLLABORATION.md) governs which files each contributor edits. Contributors record session results in their pull request description, area ledger rows, symbol records, and per-issue findings files. Maintainers update the shared summaries, handoff, progress images, and phase state after merging. The living documentation duties below fall to maintainers for shared files and to contributors for their area files.

## Living documentation contract

Update `docs/PROGRESS.md` and `docs/HANDOFF.md` after every meaningful work session. The handoff must let another human or LLM resume without prior chat history: include exact next actions, commands, prerequisites, unresolved issues, and current evidence. Do not include secrets or private machine-specific paths.

Update `docs/re/FINDINGS.md` and relevant inventory or architecture records when an investigation changes knowledge. State explicitly when a session produces no new reverse engineering findings. Record durable decisions in `docs/DECISIONS.md`. Keep setup and matching commands synchronized with implementation.

Update the root `AGENTS.md` when the active phase, target, build entry point, or navigation changes. Update these rules when established conventions change. Do not turn `AGENTS.md` into a chronological log or silently relax the no-comments, no-unit-tests, or exact-match requirements.

Before ending each meaningful session, update the traps and findings section in `AGENTS.md` with reusable lessons, failed approaches worth avoiding, and findings that affect future work. Include the trigger or symptom, verified lesson or workaround, and an evidence link. Keep detailed project-tooling evidence in `docs/` and reverse engineering evidence in `docs/re/`. Mark uncertainty explicitly and supersede outdated guidance when evidence changes. If nothing new emerged, record that in progress; do not invent a lesson. Future sessions must read these entries before repeating an investigation.

Keep all project documentation under `docs/` and all reverse engineering documentation under `docs/re/`. Link detailed findings rather than duplicating them in several summaries. Human-authored updates follow the same requirements as LLM-authored updates.
