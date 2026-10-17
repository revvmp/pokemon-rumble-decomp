# Agent entry point

Before working, read [project rules](docs/PROJECT_RULES.md), [architecture](docs/ARCHITECTURE.md), [roadmap](docs/ROADMAP.md), [progress](docs/PROGRESS.md), and [handoff](docs/HANDOFF.md). These instructions apply repository-wide. For reverse engineering, also read [the workflow](docs/re/WORKFLOW.md), relevant [findings](docs/re/FINDINGS.md), and [prior art](docs/re/PRIOR_ART.md).

Do not add unit tests, code comments, or docstrings. Keep explanations and project documentation in `docs/`, with all reverse engineering documentation in `docs/re/`. Never change original hashes to make a reconstruction pass. Never claim matching or completion without the evidence required by [matching requirements](docs/MATCHING.md).

Prefer end-to-end validation through actual entry points: run the real tooling workflow or build pipeline and inspect its outputs, exit status, and exact artifact comparisons. End-to-end tests are allowed; unit tests are not. Runtime checks supplement, but never replace, exact binary matching.

Keep this file and the linked living documents current as required by the project rules. Update this entry point when the active phase, target, or validated build entry point changes. Keep detailed evidence and session history in `docs/`.

## Session memory: traps and findings

Read this section before starting work. Before ending every meaningful session, update it with newly discovered traps, failed approaches worth avoiding, and findings that change how future work should proceed. Do not leave reusable knowledge only in chat or a handoff. Each entry must state the trigger or symptom, the verified lesson or workaround, and a link to detailed evidence in `docs/` or `docs/re/` as appropriate.

Keep entries concise and actionable. Distinguish confirmed findings from hypotheses; do not promote a single unexplained failure into a universal rule. Revise or mark superseded guidance when new evidence changes it, preserving an evidence link. If no new trap or finding emerged, say so in the session's progress update rather than inventing an entry.

- **Matching files can still be the wrong reference.** Identical original and rebuilt files fail if both differ from the frozen manifest. Never refresh expected hashes to fix this; identify the input mismatch. This rejection was exercised during [initialization validation](docs/VALIDATION.md).
- **The target template is not a configured release.** Verification rejects its unset identity and hashes. Identify the release and fingerprint a separate target manifest before attempting game verification; see [setup](docs/SETUP.md).
- **Community decomp tooling carries comments.** dtk-template's `configure.py` and `tools/*.py`, used by Petari, are commented, which conflicts with the no-comments rule. Do not vendor them until [D-0007](docs/DECISIONS.md#d-0007-evaluate-the-dtk-template-ecosystem) records a policy. Petari's `NON_MATCHING`, decomp.me, and field-offset comments go in the reconstruction ledger and symbol records instead; see [prior art](docs/re/PRIOR_ART.md#adapting-to-this-projects-rules).
- **Comparable-project facts are not our facts.** Compiler versions, flags, and library lists from Petari are hypotheses for this game. Keep them out of `config/toolchain.json` until our own evidence confirms them; see [roadmap](docs/ROADMAP.md).
- **Progress images are committed outputs.** Changing `config/progress.json` or measured progress without rerunning `tools/progress.py` leaves stale README images. Regenerate and commit them together; never hand-edit the SVGs or pass a fabricated report. See [progress images](docs/PROGRESS.md#progress-images).
- **Byte identity does not prove source reconstruction.** Copying original executable bytes can satisfy comparison. Review source coverage and fallback dependencies separately before claiming decompilation progress; see [acceptance requirements](docs/MATCHING.md).

- **A WAD boot index is not a game-code inventory.** This package declares seven contents and boot index 6. Inspect and validate all decrypted contents before selecting the main executable; see [F-0001](docs/re/FINDINGS.md#f-0001-wad-container-metadata).

- **Content 1 is not directly parseable as DOL.** dtk 1.8.4 panicked on this extracted content; inspect its format before loading. Content 6 parses as the boot DOL. Resolved: content 1 is NLZSS-compressed; use `dtk nlzss decompress` with explicit `-o` to preserve the input. See [F-0003](docs/re/FINDINGS.md#f-0003-main-game-dol-recovered-from-compressed-content-1).

- **Removing `/orig/` alone does not expose its files.** Extension rules still ignore WAD, DOL, and APP files. The user authorized tracking all of `orig/`; final recursive exceptions override these rules. See [tracking guidance](docs/HANDOFF.md#git-tracking-of-originals).

- **Nested .arc.cx files need format inspection.** dtk could list content 5 but could not directly open its Pokemon .arc.cx entry as an archive. Resolved: these files use LZH8 (0x40), which dtk NLZSS rejects. Use the pinned supporting decoder through `tools/extract_reference.py`; see [F-0005](docs/re/FINDINGS.md#f-0005-recursive-reference-extraction-and-repeat-validation).

- **U8 node names can contain backslashes.** On Windows dtk failed to create nested Home Menu sound files until validated parent directories were precreated. Preserve original node names and offsets in the inventory; see [F-0005](docs/re/FINDINGS.md#f-0005-recursive-reference-extraction-and-repeat-validation).

## Current project state

| Current state | Value |
| --- | --- |
| Phase | Roadmap Phase 1: reference extraction validated; executable scope review pending |
| Active target | `wpse-v1` (USA, TMD title version 1) |
| Original hashes | Package SHA-256 frozen in `config/targets/wpse-v1-package.json` |
| Game build entry point | Not implemented |
| Verification entry point | `python tools/verify.py --help` |
| Progress images | `python tools/progress.py render --phases config/progress.json --output-dir docs/images` |
| Extraction entry point | `python tools/extract_reference.py`; see `docs/re/EXTRACTION.md` |
| Next action | Review executable manifests and boot/main coverage; see `docs/HANDOFF.md` |
