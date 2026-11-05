# Handoff

## Current position

The matching main.dol reconstruction for `wpse-v1` is complete under D-0015/D-0016/D-0017. All 13,462 real CPU functions, 3,078,460 CPU code bytes and 1,788,360 selected data bytes are complete. All 897 logical units and 902 raw final-link inputs are source-selected; no original split fallback remains. Frozen main.dol is 4,094,880 bytes with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

AX DSP, the OS DSP bootstrap and the Bluetooth controller patch now build from readable symbolic programs. Original language is unproven for the latter two, and the owner explicitly approved those exact assembly exceptions under D-0017. Their provenance limits remain part of acceptance. The semantic system-message archive builds from recovered JSON/PNG assets. The usual dtk workflow still reads the original for analysis/layout/comparison; no selected original object supplies executable output bytes.

Two complete clean `build.bat` runs reproduce the final executable, ELF, report and generated assets/programs exactly. [Final acceptance](re/findings/main-source-completion-20261104.md) and [validation](VALIDATION.md#main-source-completion-checkpoint) record the hashes and audits. Boot-loader content 6, other content assets and WAD packaging are excluded by the frozen manifest. Roadmap Phases 1-5 are complete; Phase 6's broader package scope remains open.

## Main-branch workflow

Work on main by the owner's instruction. Do not create branches or worktrees unless explicitly requested. Parallel agents own separate files and scratch outputs. One coordinator runs configure, ninja, full verification, shared configuration/header changes and checkpoints. Issue claim exclusions remain in force; use the repository skill for issue work, with this main-branch instruction superseding its branch/PR loop.

## Next actions

1. Preserve the accepted source and build. Run `build.bat`, or the configure/ninja/frozen-verifier commands below, after changes. Check ninja's exit status and avoid verifying stale output.
2. Tool setup now also builds the pinned Dolphin DSP assembler with Zig. `config/dsp_assembler.json` pins its sources, licenses, compiler and binary. `tools/setup/acquire_tools.py --offline` verifies the installed tools. `tools/setup/acquire_dsp_assembler.py --rebuild` deliberately rebuilds the host tool from pinned inputs in a fresh directory.
3. DSP generators use complete dependency records; the WUD generator also depends on the verified tool receipt. Generated binaries/includes remain under the chosen build directory and must not become tracked source inputs. Firmware sources are under the AX, OS and WUD SDK subdirectories.
4. Any future source-fidelity cleanup must preserve exact full-artifact proof and maintain the D-0016 ledger. D-0017 authorizes only the two named embedded-firmware payloads. Do not generalize it to other compiled code.
5. The owner previously expressed interest in a native Windows/WebAssembly port after the long tail. That port has not started and is a separate next scope. Boot-loader/package reconstruction is also separate; do not claim the WAD rebuilt.

The final private diagnostic workspace is `tools/local/tail-20261104/`; reusable production tools are tracked in tools/. Older sprint scratch paths may be absent. The accepted source must build without private probe scripts, prebuilt scratch objects or a running IDA session.

## Repository skills

The current Claude skill, `take-issue`, is also exposed to Codex through `.agents/skills/take-issue/SKILL.md`. Invoke it in Codex with `$take-issue`, optionally naming an issue or area label, or ask to pick up an issue. The Codex entry point loads `.claude/skills/take-issue/SKILL.md`, which remains the single authoritative procedure. Edit that shared file for workflow changes and update both descriptions if the trigger changes. No global installation or Windows symlink is required.

Validation and its limits are recorded in [validation](VALIDATION.md#codex-skill-port-validation).

## Build and verification commands

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
