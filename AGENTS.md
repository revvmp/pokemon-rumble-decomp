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

Contributors working a GitHub issue do not edit this list. They record traps in their issue's findings file in `docs/re/findings/`, under a `Traps` heading. Maintainers review those and promote the ones that apply project-wide, so this list stays short and pull requests do not conflict on it.

Keep entries concise and actionable. Distinguish confirmed findings from hypotheses; do not promote a single unexplained failure into a universal rule. Revise or mark superseded guidance when new evidence changes it, preserving an evidence link. If no new trap or finding emerged, say so in the session's progress update rather than inventing an entry.

- **Matching files can still be the wrong reference.** Identical original and rebuilt files fail if both differ from the frozen manifest. Never refresh expected hashes to fix this; identify the input mismatch. This rejection was exercised during [initialization validation](docs/VALIDATION.md).
- **The target template is not a configured release.** Verification rejects its unset identity and hashes. Identify the release and fingerprint a separate target manifest before attempting game verification; see [setup](docs/SETUP.md).
- **Community decomp tooling carries comments.** dtk-template's `configure.py` and `tools/*.py`, used by Petari, are commented, which conflicts with the no-comments rule. Do not vendor them until [D-0007](docs/DECISIONS.md#d-0007-evaluate-the-dtk-template-ecosystem) records a policy. Petari's `NON_MATCHING`, decomp.me, and field-offset comments go in the reconstruction ledger and symbol records instead; see [prior art](docs/re/PRIOR_ART.md#adapting-to-this-projects-rules).
- **Comparable-project facts are not our facts.** Compiler versions, flags, and library lists from Petari are hypotheses for this game. Keep them out of `config/toolchain.json` until our own evidence confirms them; see [roadmap](docs/ROADMAP.md).
- **Progress images are committed outputs.** Changing `config/progress.json` or measured progress without rerunning `tools/progress/progress.py` leaves stale README images. The merging agent refreshes them with `tools/progress/refresh_images.py` right after each merge, from its verified build; it pushes only if `main` still matches that build and the last image commit is at least 60 minutes old, so image commits stop restarting other agents' merge loops. Do not commit `docs/images/` from issue branches; for a local preview, render into a scratch directory. Never hand-edit the SVGs or pass a fabricated report. See [progress images](docs/PROGRESS.md#progress-images).
- **dtk rewrites `symbols.txt` and `splits.txt`.** `ninja` regenerated `symbols.txt` and dropped a hand-added `force_active` attribute. Keep linker-visible requirements in source (`#pragma force_active on`) or `config.yml`, and review dtk rewrites before committing. See [F-0010](docs/re/FINDINGS.md#f-0010-split-build-reproduces-maindol).
- **objdiff 100% does not guarantee a link match.** Fuzzy scoring ignores alignment padding. A unit compiled with the Wii default 16-byte function alignment scored 100% but shifted the whole DOL; the runtime needs `-func_align 4`. Mark a unit `Matching` only after full verification passes. See [F-0010](docs/re/FINDINGS.md#f-0010-split-build-reproduces-maindol).
- **The Wii linker dead-strips `.ctors$10` and `.dtors$15`.** Declare the sections with `#pragma section` and wrap the table entries in `#pragma force_active on`. See [F-0010](docs/re/FINDINGS.md#f-0010-split-build-reproduces-maindol).
- **Compilers differ per library.** RVL_SDK matches only Wii/1.0 at default alignment. MSL and Runtime need Wii/1.1 or 1.3 with `-func_align 4`. Do not apply one compiler project-wide. See [F-0007](docs/re/FINDINGS.md#f-0007-per-library-compiler-identification).
- **Library code sits inside the game address range.** Banners and class names show HBM, RevoEX, NCD, CNT, TMC JPEG, FA, MSL printf, and NW4R ef/g3d/snd between 0x80170000 and 0x80292D00, not only after 0x80292000. Check F-0012 before assuming code is game code or choosing a compiler; see [F-0012](docs/re/FINDINGS.md#f-0012-full-executable-layout-survey).
- **Do not write RVL_SDK headers from scratch.** Parallel contributors created duplicate headers in two trees, `include/revolution/` and `include/RVL_SDK/`, which caused conflicts and divergent declarations. Use and extend the imported Petari headers in `include/revolution/` and `include/private/`; see [D-0013](docs/DECISIONS.md#d-0013-shared-rvl_sdk-headers-come-from-petari).
- **The IDA database is not mergeable.** `docs/re/main.elf.i64` is tracked (D-0012), but two people changing it means one loses work. Never commit changes to it from issue branches; record names in `config/wpse-v1/symbols.txt` instead. See [D-0012](docs/DECISIONS.md#d-0012-track-the-shared-ida-database).
- **Worktree junctions can delete shared tools.** Recursive deletion of a worktree may follow `tools\local` and `.venv` junctions. Remove junctions with `cmd /c rmdir` before `git worktree remove`; validated in [collaboration validation](docs/VALIDATION.md#collaboration-workflow-validation).
- **Shared `build/` is single-writer.** Parallel `configure.py` or `ninja` runs clobber `build.ninja` and outputs. Run parallel investigations in separate scratch directories or worktrees.
- **Relative paths can land in the main checkout.** Two agents edited `splits.txt`, `include/memory`, and a new tool in the main checkout because a relative path resolved against the session's starting directory, not their worktree. Use absolute `..\rumble-<n>\...` paths for every edit and check `git status` in the main checkout before finishing; see [F-82-3](docs/re/findings/82-cdlgpiilist.md).
- **Copied build directories keep the donor's header paths.** `.ninja_deps` stores absolute header paths, so a plain copy of another worktree's `build/` tracks the donor's headers and misses edits to your own. Seed worktrees only with `tools/collab/build_seed.py`, which rewrites those paths and backdates unchanged files; see [validation](docs/VALIDATION.md#incremental-and-seeded-build-validation).
- **Wiping `build/` after a config merge only costs time.** An incremental build across twelve commits changing `configure.py`, `splits.txt`, and `symbols.txt` produced the same `main.dol`, `main.elf`, and `report.json` as a clean build. Delete only `build/wpse-v1/main.dol` before `ninja`; `tools/collab/land.py` does this. See [validation](docs/VALIDATION.md#incremental-and-seeded-build-validation).
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
- **Replacing a unit can remove its trailing alignment padding.** GXTev scored 100% but its source object omitted twelve trailing padding bytes; the next fallback unit began early at four-byte alignment. Preserve the observed next entry alignment (`GXSetFog`, 16 bytes) in symbols before accepting the link. See [F-479-2](docs/re/findings/479-gx.md).

- **Packed 4-byte function starts inside the RVL_SDK range mean another compiler.** NdevExi2A (`0x8033EEB4`-`0x8033F974`) matches only GameCube GC/3.0a3 to 3.0a5, not Wii/1.0. Check function alignment before assuming F-0007's per-region compiler. See [F-535-2](docs/re/findings/535-gdtexture-ndevexi2a.md#f-535-2-ndevexi2a-is-built-with-a-gamecube-compiler).

- **ENC Latin1 includes Windows-1252 mappings.** Replacing its C1 entries with a standards-only ISO-8859-1 table changes target data; preserve the verified source maps shared by both forward conversions. See [F-494-1](docs/re/findings/494-enc-latin.md#reusable-lesson).

- **Source units can lose trailing alignment before a fallback unit.** When objdiff is 100% but the next function shifts by four bytes, inspect its inferred alignment. ENC Latin needed `align:16` on the following owned symbol to preserve the original boundary; see [F-494-2](docs/re/findings/494-enc-latin.md#f-494-2-alignment-of-the-next-fallback-unit).

- **Negated closing keywords still close issues.** A partial-work pull request automatically closed its unfinished issue because its description contained a negated closing-keyword phrase. Use `Part of` and avoid closing-keyword issue references anywhere in partial-work descriptions; check issue state after merging. See [F-514-2](docs/re/findings/514-bta-dm-api.md#f-514-2-partial-work-issue-closure-keywords).

- **End a `.text` split at the last function's end, not the next alignment boundary.** Ending `arc.c` at the padded 0x802F08E0 made the compiled object 8 bytes short of its split, and `.ctors` moved to 0x803529B8. Ending it at 0x802F08D8 leaves the padding to the next object and matches. See [F-495-1](docs/re/findings/495-arc-unit.md).

- **verify.py checks whatever `main.dol` is on disk.** After a failed link it passed against the previous build's output. Delete `build/wpse-v1/main.dol` or confirm `ninja` exited 0 before trusting a verification result; observed in [F-495-1](docs/re/findings/495-arc-unit.md).

- **A whole `.bss` extent can include the next unit's padding.** Splitting `mem_heapCommon.c` `.bss` to the next symbol shifted `.sdata` by `-0x20` and failed verification at DOL header byte `0x7F`. End the split at the unit's last object and leave the padding to the following automatic object. See [F-488-1](docs/re/findings/488-axfx-mem-units.md#f-488-1-unit-boundaries-in-0x802df1900x802e18f0).

- **A failed link leaves the old `main.dol` for verification.** `verify.py` printed `PASS` after `ninja` failed to link, because it checked the previous build's DOL. Check the `ninja` exit status first; linking a `Matching` unit also needs every callee renamed from `fn_` in `symbols.txt`. See [F-472-4](docs/re/findings/472-exi-si-db-units.md#f-472-4-verification-can-pass-on-a-stale-maindol).

- **Imported library names can carry the wrong signature.** About 1,260 library placeholders were renamed from public symbol maps (D-0014); their mangled names come from the reference revision, so a matched unit can fail to link on a signature such as `snd::PanMode` versus this target's `snd::detail::PanMode`. Rename the symbol to your source's name when code evidence disagrees, and check `docs/re/reference-symbols.tsv` for the tier and sources. See [F-0013](docs/re/FINDINGS.md#f-0013-reference-symbol-maps-name-library-functions-by-size-alignment).
- **The inline budget is all-or-nothing.** A function whose direct inline callees exceed about 10000 units inlines none of them, so adding or deleting one statement can flip inlining across the whole function. Move heavy bodies into an inline helper; flags and pragmas do not change it. See [CODEGEN_RULES](docs/re/CODEGEN_RULES.md#inlining).
- **Stack slots depend on inline-depth parity.** Frame objects sort into power-of-two size buckets, first-created highest, and each inline level reverses its callee's objects. A swapped pair of slots usually means one inline level too few or too many, not a declaration-order problem. See [CODEGEN_RULES](docs/re/CODEGEN_RULES.md#stack-frame-slots).
- **A register swap can hide a source difference.** A node with degree of 29 or more is coloured first, so one missing live value moved homebutton's parameters from r31 to the bottom. Run `tools/re/regscan.py` and fix instruction differences first, then read the colouring order with `tools/re/ra_trace.py`. See [sprint-ra](docs/re/findings/sprint-ra.md#rules).
- **Template emission is newest first, and header inline bodies create instances at include time.** A weak-function or vtable order flip is fixed by include order and first-declaration position, not by moving definitions. See [CODEGEN_RULES](docs/re/CODEGEN_RULES.md#emission-order-of-weak-functions-and-data).
- **`const&` parameters cache loads across stores.** A target that reloads after each store means a non-const reference or by-value parameter, and the mangled name changes with it. See [CODEGEN_RULES](docs/re/CODEGEN_RULES.md#loads-and-constants).
- **`name_callees.py` and `permute_names.py` renames outside your unit can break the DOL.** Apply only `symbols.txt` changes inside the unit's address range and confirm `ninja` exits 0. See [CODEGEN_RULES](docs/re/CODEGEN_RULES.md#tooling-traps).
- **Codex cannot commit from a linked worktree.** Its sandbox write root is the worktree, but the worktree's index lives under the main checkout's `.git/worktrees/`, so `git commit` fails on `index.lock`. Launch Codex from the worktree directory, have it leave changes uncommitted, and commit and land them yourself; see [collaboration](docs/COLLABORATION.md).

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
| Next action | Remaining work is long-tail near-misses and emission-order flips; see `docs/re/CODEGEN_RULES.md` and `docs/HANDOFF.md` |
