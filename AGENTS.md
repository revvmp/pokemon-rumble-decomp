# Agent entry point

Before working, read [project rules](docs/PROJECT_RULES.md), [architecture](docs/ARCHITECTURE.md), [roadmap](docs/ROADMAP.md), [progress](docs/PROGRESS.md), and [handoff](docs/HANDOFF.md). These instructions apply repository-wide. For reverse engineering, also read [the workflow](docs/re/WORKFLOW.md), relevant [findings](docs/re/FINDINGS.md), and [prior art](docs/re/PRIOR_ART.md).

Do not add unit tests, code comments, or docstrings. Keep explanations and project documentation in `docs/`, with all reverse engineering documentation in `docs/re/`. Never change original hashes to make a reconstruction pass. Never claim matching or completion without the evidence required by [matching requirements](docs/MATCHING.md).

Prefer end-to-end validation through actual entry points: run the real tooling workflow or build pipeline and inspect its outputs, exit status, and exact artifact comparisons. End-to-end tests are allowed; unit tests are not. Runtime checks supplement, but never replace, exact binary matching.

Keep this file and the linked living documents current as required by the project rules. Update this entry point when the active phase, target, or validated build entry point changes. Keep detailed evidence and session history in `docs/`.

## Parallel work

Codex discovers the repository skill at [`.agents/skills/take-issue/SKILL.md`](.agents/skills/take-issue/SKILL.md), which loads the shared Claude procedure below. Maintain the workflow in one place; see [skill setup](docs/HANDOFF.md#repository-skills).

If you were given a GitHub issue, or asked to pick one up, follow the `take-issue` procedure in `.claude/skills/take-issue/SKILL.md`; it applies to every agent, not only Claude. It claims through `tools/collab/claim_issue.py`, which never takes an assigned issue. Then read [collaboration](docs/COLLABORATION.md) and the issue before anything else. Work in your own worktree and branch, edit only the files that guide allows for your claim, and report through the pull request. [Workstreams](docs/WORKSTREAMS.md) lists all issues.

## Source fidelity standard

Aim for the readability of the Super Mario Galaxy decompilation (Petari): source that looks like plausible original code, not decompiler output. Matching bytes are necessary but not sufficient.

- Model game code as idiomatic C++ classes with real member functions, constructors, destructors, virtual methods in original vtable order, and inheritance that matches observed vtables and constructors. Use C only where the original unit was C, such as RVL_SDK and MSL.
- Prefer original names. Recover them from embedded evidence such as source-file strings (`CPauseMenu.cpp`), debug and assert strings (`GetPokemonVoiceId`), mangled symbols, and matching library sources. Keep original spelling exactly, including typos.
- When no original name exists, choose a descriptive name that reflects proven behavior, following Petari conventions: `m` members, `p` pointer parameters, `r` reference parameters, `s` unknown statics, `g` unknown globals, `C`-prefixed game classes where evidence shows that convention. Never ship `fn_8012BCB0`, `unk_0x10`, `field_4`, or `lbl_` names in reconstructed source.
- Give every class a header in `include/` with complete, correctly ordered members and exact types. Use original integer types where mangling or code generation requires them (`int` versus `s32`), `nullptr` in C++, `NULL` in C, and `f` suffixes on float literals.
- Petari explains members with comments; this project does not allow comments. Achieve clarity through names, types, small functions, and enums instead, and record field offsets, sizes, evidence, and naming confidence in [symbols](docs/re/SYMBOLS.md).
- Never paste raw decompiler output. Rewrite it into idiomatic source that a person could have written, then prove it matches.

## Session memory: traps and findings

Read this section before starting work. Before ending every meaningful session, update it with newly discovered traps, failed approaches worth avoiding, and findings that change how future work should proceed. Do not leave reusable knowledge only in chat or a handoff. Each entry must state the trigger or symptom, the verified lesson or workaround, and a link to detailed evidence in `docs/` or `docs/re/` as appropriate.

Keep entries concise and actionable. Distinguish confirmed findings from hypotheses; do not promote a single unexplained failure into a universal rule. Revise or mark superseded guidance when new evidence changes it, preserving an evidence link. If no new trap or finding emerged, say so in the session's progress update rather than inventing an entry.

- **Matching files can still be the wrong reference.** Identical original and rebuilt files fail if both differ from the frozen manifest. Never refresh expected hashes to fix this; identify the input mismatch. This rejection was exercised during [initialization validation](docs/VALIDATION.md).
- **The target template is not a configured release.** Verification rejects its unset identity and hashes. Identify the release and fingerprint a separate target manifest before attempting game verification; see [setup](docs/SETUP.md).
- **Community decomp tooling carries comments.** dtk-template's `configure.py` and `tools/*.py`, used by Petari, are commented, which conflicts with the no-comments rule. Do not vendor them until [D-0007](docs/DECISIONS.md#d-0007-evaluate-the-dtk-template-ecosystem) records a policy. Petari's `NON_MATCHING`, decomp.me, and field-offset comments go in the reconstruction ledger and symbol records instead; see [prior art](docs/re/PRIOR_ART.md#adapting-to-this-projects-rules).
- **Comparable-project facts are not our facts.** Compiler versions, flags, and library lists from Petari are hypotheses for this game. Keep them out of `config/toolchain.json` until our own evidence confirms them; see [roadmap](docs/ROADMAP.md).
- **Progress images are committed outputs.** Changing `config/progress.json` or measured progress without rerunning `tools/progress/progress.py` leaves stale README images. CI now renders and commits them on every push to `main` (`DOCS: refresh progress images` by github-actions). Do not commit `docs/images/` from issue branches; for a local preview, render into a scratch directory. Never hand-edit the SVGs or pass a fabricated report. See [progress images](docs/PROGRESS.md#progress-images).
- **dtk rewrites `symbols.txt` and `splits.txt`.** `ninja` regenerated `symbols.txt` and dropped a hand-added `force_active` attribute. Keep linker-visible requirements in source (`#pragma force_active on`) or `config.yml`, and review dtk rewrites before committing. See [F-0010](docs/re/FINDINGS.md#f-0010-split-build-reproduces-maindol).
- **objdiff 100% does not guarantee a link match.** Fuzzy scoring ignores alignment padding. A unit compiled with the Wii default 16-byte function alignment scored 100% but shifted the whole DOL; the runtime needs `-func_align 4`. Mark a unit `Matching` only after full verification passes. See [F-0010](docs/re/FINDINGS.md#f-0010-split-build-reproduces-maindol).
- **The Wii linker dead-strips `.ctors$10` and `.dtors$15`.** Declare the sections with `#pragma section` and wrap the table entries in `#pragma force_active on`. See [F-0010](docs/re/FINDINGS.md#f-0010-split-build-reproduces-maindol).
- **Compilers differ per library.** RVL_SDK matches only Wii/1.0 at default alignment. MSL and Runtime need Wii/1.1 or 1.3 with `-func_align 4`. Do not apply one compiler project-wide. See [F-0007](docs/re/FINDINGS.md#f-0007-per-library-compiler-identification).
- **Library code sits inside the game address range.** Banners and class names show HBM, RevoEX, NCD, CNT, TMC JPEG, FA, MSL printf, and NW4R ef/g3d/snd between 0x80170000 and 0x80292D00, not only after 0x80292000. Check F-0012 before assuming code is game code or choosing a compiler; see [F-0012](docs/re/FINDINGS.md#f-0012-full-executable-layout-survey).
- **The IDA database is not mergeable.** `docs/re/main.elf.i64` is tracked (D-0012), but two people changing it means one loses work. Never commit changes to it from issue branches; record names in `config/wpse-v1/symbols.txt` instead. See [D-0012](docs/DECISIONS.md#d-0012-track-the-shared-ida-database).
- **Worktree junctions can delete shared tools.** Recursive deletion of a worktree may follow `tools\local` and `.venv` junctions. Remove junctions with `cmd /c rmdir` before `git worktree remove`; validated in [collaboration validation](docs/VALIDATION.md#collaboration-workflow-validation).
- **Shared `build/` is single-writer.** Parallel `configure.py` or `ninja` runs clobber `build.ninja` and outputs. Run parallel investigations in separate scratch directories or worktrees.
- **Byte identity does not prove source reconstruction.** Copying original executable bytes can satisfy comparison. Review source coverage and fallback dependencies separately before claiming decompilation progress; see [acceptance requirements](docs/MATCHING.md).

- **A WAD boot index is not a game-code inventory.** This package declares seven contents and boot index 6. Inspect and validate all decrypted contents before selecting the main executable; see [F-0001](docs/re/FINDINGS.md#f-0001-wad-container-metadata).

- **Content 1 is not directly parseable as DOL.** dtk 1.8.4 panicked on this extracted content; inspect its format before loading. Content 6 parses as the boot DOL. Resolved: content 1 is NLZSS-compressed; use `dtk nlzss decompress` with explicit `-o` to preserve the input. See [F-0003](docs/re/FINDINGS.md#f-0003-main-game-dol-recovered-from-compressed-content-1).

- **Removing `/orig/` alone does not expose its files.** Extension rules still ignore WAD, DOL, and APP files. The user authorized tracking all of `orig/`; final recursive exceptions override these rules. See [tracking guidance](docs/HANDOFF.md#git-tracking-of-originals).

- **Nested .arc.cx files need format inspection.** dtk could list content 5 but could not directly open its Pokemon .arc.cx entry as an archive. Resolved: these files use LZH8 (0x40), which dtk NLZSS rejects. Use the pinned supporting decoder through `tools/reference/extract_reference.py`; see [F-0005](docs/re/FINDINGS.md#f-0005-recursive-reference-extraction-and-repeat-validation).

- **U8 node names can contain backslashes.** On Windows dtk failed to create nested Home Menu sound files until validated parent directories were precreated. Preserve original node names and offsets in the inventory; see [F-0005](docs/re/FINDINGS.md#f-0005-recursive-reference-extraction-and-repeat-validation).

- **CNT reference headers can describe another ABI revision.** The public RB3 header declares a three-argument `CNTRead`, but this target's read routine at `0x801783B0` consumes an offset in `r6`. Validate signatures against target accesses before importing a reference interface; see [F-286-1](docs/re/findings/286-cnt.md).
- **CodeWarrior SPR names are not Broadway's.** Wii/1.0 inline asm encodes `HID2` as SPR 1011 and rejects `WPAR` and `HID4`. Write numeric SPRs (`0x398`, `0x399`, `0x3F3`) in SDK asm. See [F-463-2](docs/re/findings/463-ppcarch.md#f-463-2-codewarrior-inline-assembler-spr-names).

- **An incomplete external type can prevent SDA addressing.** BTA_Init used a two-instruction address load for opaque tHCI_CFG, adding an instruction. A proven four-byte unsigned-short storage view restored small-data addressing and 100% function comparison; retain unknown field names as opaque. See [F-510-2](docs/re/findings/510-bte-transport-logging.md#f-510-2-complete-main-unit-split-and-remaining-three-functions).

- **Root licenses can have file-level exceptions.** Petari BTE files retain Broadcom Apache-2.0 notices despite its root CC0 license. Inspect each reused file and preserve its specific notices; see [F-510-1](docs/re/findings/510-bte-transport-logging.md#source-provenance-and-reusable-lesson).
- **An issue boundary can divide SDK static state.** Linking the proposed #484 AI split exposed a callback reference from the preceding #483 function. That function is AIRegisterDMACallback, so #483 owns the AI split. Inspect preceding references; split-only byte identity does not establish a source-unit boundary. See [F-484-1](docs/re/findings/484-ai.md).

- **IPC register declarations affect instruction selection.** A pointer macro produced different instructions from the original indexed loads/stores. Use the evidenced CodeWarrior absolute-address array declaration for ipcMain; see [F-496-1](docs/re/findings/496-esp-ipc-main.md).

- **VI I2C uses a fixed tick conversion.** Petari I2C functions initially differ because its clock macro reads the bus-clock word. This target uses immediate divisor 486; the fixed conversion reproduces all three non-relocation instruction streams. Limit this conclusion to I2C and retain the runtime-helper dependency; see [F-473-1](docs/re/findings/473-vi-i2c.md).

- **PowerShell semicolons do not gate lifecycle steps.** A rejected PR merge still allowed a following release command to execute. Run merge, state confirmation, claim release, and cleanup separately, checking each result; see [the #473 handoff note](docs/re/findings/473-vi-i2c.md#handoff-validation-note).

- **OSAlarm extends past the reference source.** The routine at `0x802B3760` still uses AlarmQueue and cancels reserved-tag alarms by user data; OSAlloc starts at `0x802B3810`. Do not end the alarm split at the reference's last accessor. See [F-464-1](docs/re/findings/464-os-alarm.md).

- **Game SDK copies differ from Petari's function set.** RVL_SDK `nand.c` here links seven functions Petari lacks and omits others. Identify each function by call pattern and take names from other CC0 decompilations (`yomcube/cmwp-decomp`, `pbr-ret/pbr-decomp`), recording provenance. See [F-490-2](docs/re/findings/490-nand.md#f-490-2-nandc-function-set-differs-from-petari).

## Current project state

| Current state | Value |
| --- | --- |
| Phase | Roadmap Phase 4: denominator and first source |
| Active target | `wpse-v1` (USA, TMD title version 1) |
| Original hashes | Package `config/targets/wpse-v1-package.json`; executable `config/targets/wpse-v1.json` |
| Game build entry point | `python configure.py` then `.venv/Scripts/ninja`; see `docs/SETUP.md` |
| Verification entry point | `python tools/verify/verify.py --help` |
| Progress images | `python tools/progress/progress.py render --phases config/progress.json --output-dir docs/images` |
| Extraction entry point | `python tools/reference/extract_reference.py`; see `docs/re/EXTRACTION.md` |
| Next action | Split library translation units and reconstruct sources; see `docs/HANDOFF.md` |
