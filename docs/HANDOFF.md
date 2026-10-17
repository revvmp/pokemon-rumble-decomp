# Handoff

## Current position

Target `wpse-v1` (WPSE, USA, TMD title version 1). Roadmap Phases 1 to 3 are complete; Phase 4 is active. Frozen manifests: package `config/targets/wpse-v1-package.json` and executable `config/targets/wpse-v1.json` (`main.dol`). Do not fingerprint either again.

The dtk-template build in `configure.py` and `tools/build/` reproduces `main.dol` exactly from two clean builds. One unit, `Runtime/__init_cpp_exceptions.cpp`, is reconstructed and linked; everything else is original split objects. Per-library compilers are identified in [F-0007](re/FINDINGS.md#f-0007-per-library-compiler-identification). Code progress is 112 of 3,098,940 bytes; see [counting method](MATCHING.md#counting-method).

## Next actions

1. Finish Phase 4: categorize units as libraries are split so per-category progress becomes meaningful, and reconstruct further units. Prefer libraries with matching reference source: RVL_SDK with Wii/1.0 at default alignment; MSL and Runtime with Wii/1.1 plus `-func_align 4`. Reuse requires license review and provenance in the ledger.
2. Replace dtk's per-function auto splits with real translation-unit boundaries in `config/wpse-v1/splits.txt`, starting with the library address regions in F-0007. Then enable the treemap with `--treemap` and add it to the README.
3. For game code, follow the source fidelity standard in root `AGENTS.md`: C++ classes with evidenced names from strings such as `CPauseMenu.cpp`. Record names and layouts in `re/SYMBOLS.md`.
4. After each change, run the build, run verification, regenerate the progress images, and update the ledger.
5. Still open: acquisition provenance of the WAD, the boot loader's exact content-loading path, and whether the original linker was Wii/1.1. Native asset formats remain unconverted.

## Available commands

Prerequisites: tools in `tools/local/` and ninja in `.venv`, as in [setup](SETUP.md#acquire-tools).

```powershell
python configure.py
.venv/Scripts/ninja
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1 --report build/wpse-v1/verification.json
python tools/progress/progress.py render --phases config/progress.json --report build/wpse-v1/report.json --output-dir docs/images
python tools/reference/extract_reference.py --help
```

Follow root `AGENTS.md` and `PROJECT_RULES.md`. All files under `orig/` are eligible for tracking by explicit user instruction. Tool binaries stay in ignored `tools/local/`. Do not add unit tests, code comments, or docstrings.

## Git tracking of originals

The user explicitly requested that all of `orig/` be committable. Final `!/orig/` and `!/orig/**` rules override extension exclusions inside that directory. An earlier `git ls-files --others --exclude-standard -- orig/` check validated the original 13 files; recursive extraction has since added the recorded reference tree. The earlier staging dry run could not create `.git/index.lock`. During the pull integration, approved Git operations resolved conflicts and returned local edits to an unstaged state. Preserve these exceptions after general ignore patterns.
