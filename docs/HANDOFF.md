# Handoff

## Current position

The repository contains architecture, project rules, target and toolchain templates, reverse engineering documentation, and a standard-library Python verifier. There is no configured release, local original, confirmed compiler, or game build command.

The initialization checks passed on Python 3.14.0, including temporary CLI comparisons and failure cases. See [validation](VALIDATION.md). These are tooling checks, not a game match.

Read the traps and findings in root `AGENTS.md` before work and update them with new reusable lessons before finishing. Prefer end-to-end validation through actual tooling and build workflows. End-to-end suites are allowed; unit tests, code comments, and docstrings remain prohibited. The latest session updated these documentation rules only and produced no new reverse engineering findings or tooling traps.

The [roadmap](ROADMAP.md) sequences all work; the project is in Phase 1. [Prior art](re/PRIOR_ART.md) summarizes the Petari workflow, and [D-0007](DECISIONS.md#d-0007-evaluate-the-dtk-template-ecosystem) proposes adopting it with four open items. A local WAD is present at the repository root; its unencrypted metadata is in [F-0001](re/FINDINGS.md#f-0001-wad-container-metadata). Its acquisition provenance is not yet documented.

Progress images in the README come from `tools/progress.py`; regenerate them whenever `config/progress.json` changes. See [progress images](PROGRESS.md#progress-images).

## Next action

Execute [roadmap Phase 1](ROADMAP.md#phase-1-extract-and-identify): acquire decomp-toolkit into `tools/local/` and record its version and SHA-256 in `re/TOOLCHAIN.md`, run `dtk wad info` on the WAD, extract contents to `orig/<target>/`, and identify the executable. Then follow `re/REFERENCE.md` and `SETUP.md` to choose a target identifier and fingerprint `config/targets/<target>.json`. Update the active target in `AGENTS.md`, the inventory in `re/INVENTORY.md`, and milestone evidence in `PROGRESS.md`. Ask the project owner for provenance details rather than inferring them.

## Prerequisites and commands

Git and Python 3.10 or newer support current tooling. Original artifacts must be supplied locally under `orig/<target>/`. Further tools are to be identified from evidence.

```powershell
git status --short
python tools/verify.py --help
python tools/progress.py render --phases config/progress.json --output-dir docs/images
```

F-0001 is the only reverse engineering finding. No original game hash or matching build has been measured. Do not interpret verifier availability as decompilation progress. Keep future explanations in `docs/`, reverse engineering records in `docs/re/`, and source free of comments and unit tests.
