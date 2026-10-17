# Handoff

## Current position

The repository contains architecture, project rules, target and toolchain templates, reverse engineering documentation, and a standard-library Python verifier. There is no configured release, local original, confirmed compiler, or game build command.

The initialization checks passed on Python 3.14.0, including temporary CLI comparisons and failure cases. See [validation](VALIDATION.md). These are tooling checks, not a game match.

Read the traps and findings in root `AGENTS.md` before work and update them with new reusable lessons before finishing. Prefer end-to-end validation through actual tooling and build workflows. End-to-end suites are allowed; unit tests, code comments, and docstrings remain prohibited. The latest session updated these documentation rules only and produced no new reverse engineering findings or tooling traps.

## Next action

Follow `re/REFERENCE.md` to identify a local original release and inventory it. Then follow `SETUP.md` to create and fingerprint `config/targets/<target>.json`. Update the active target in `AGENTS.md`, the inventory in `re/INVENTORY.md`, and milestone evidence in `PROGRESS.md`.

## Prerequisites and commands

Git and Python 3.10 or newer support current tooling. Original artifacts must be supplied locally under `orig/<target>/`. Further tools are to be identified from evidence.

```powershell
git status --short
python tools/verify.py --help
```

No reverse engineering findings were produced during initialization. No original game hash or matching build has been measured. Do not interpret verifier availability as decompilation progress. Keep future explanations in `docs/`, reverse engineering records in `docs/re/`, and source free of comments and unit tests.
