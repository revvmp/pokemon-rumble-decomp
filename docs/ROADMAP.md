# Roadmap

This roadmap sequences the work from the current state to a matching executable and then toward full coverage. It adapts the workflow proven by Wii decompilations such as Petari; see [prior art](re/PRIOR_ART.md). Each phase ends at a gate that must be satisfied with evidence before the next phase claims anything. Phases are ordered by dependency, not by calendar. The [completion gates](MATCHING.md#completion-gates) remain the definition of done.

Assumptions marked **hypothesis** are expectations from comparable projects. They are not established facts about this game and must not be written into configuration until confirmed.

## Phase 0: Foundation

Done. Rules, architecture, living documentation, and the exact artifact verifier exist. See [progress](PROGRESS.md).

## Phase 1: Extract and identify

Goal: a frozen manifest for the original package and an extracted, identified main executable.

1. Acquire decomp-toolkit into `tools/local/`, recording version, source URL, and binary SHA-256 in [toolchain](re/TOOLCHAIN.md). Petari pins `v1.8.3`.
2. Run `dtk wad info` and `dtk vfs ls` against the local WAD. [F-0001](re/FINDINGS.md#f-0001-wad-container-metadata) already records title ID `WPSE`, title version 1, seven contents, and boot index 6 from unencrypted metadata. If dtk cannot open a WiiWare WAD, record that as a finding and evaluate another extractor.
3. Extract every content to `orig/<target>/` with a recorded command. Identify which content holds the executable. **Hypothesis:** the boot content is a loader and the main DOL lives in another content, possibly compressed, and the largest content is a U8 data archive.
4. Choose the target identifier from observed identity, for example a form based on `WPSE` and title version 1, and record the reasoning.
5. Create a provenance manifest for the WAD and an acceptance manifest for the extracted main DOL, then fingerprint both following [setup](SETUP.md).
6. Inventory all contents, including archives and nested files, in [inventory](re/INVENTORY.md). Look specifically for any linker map, REL modules, or `selfile.sel`.

Gate: manifests frozen, every content accounted for, and the executable's format and sections recorded.

## Phase 2: Toolchain and library evidence

Goal: known compiler families and versions for at least the runtime and SDK portions.

1. Extract SDK and middleware banner strings from the DOL and record each library name and build stamp.
2. Read DOL section layout. Note whether `extab`/`extabindex` exist, which indicates C++ exception tables.
3. **Hypothesis:** the executable was built with Metrowerks CodeWarrior for Wii and links RVL_SDK, MSL, and the CodeWarrior runtime. Confirm or reject using code-generation signatures and by compiling known library source from existing decompilations with candidate compiler versions and comparing.
4. Record each confirmed compiler version, flag set, and the evidence in [findings](re/FINDINGS.md) and `config/toolchain.json`. Expect different versions per library, as in Petari.
5. Acquire confirmed compilers locally, recording SHA-256, and record whether wibo or wine is needed on non-Windows hosts.

Gate: at least one library translation unit compiles to identical bytes with a recorded compiler, version, and flags.

## Phase 3: Splittable build skeleton

Goal: `python configure.py` then `ninja` reproduces the original DOL byte for byte using only split original objects, with acceptance by `tools/verify.py`.

1. Resolve the open items in [D-0007](DECISIONS.md#d-0007-evaluate-the-dtk-template-ecosystem): vendoring policy for dtk-template tooling and its comments, directory mapping, and the relationship between `build.sha1` and our manifests.
2. Generate initial `symbols.txt` and `splits.txt` with `dtk dol config`, store them under `config/<target>/`, and fix analysis errors with `block_relocations` and `add_relocations`.
3. Set the linker version and `mw_comment_version` from evidence. Handle the exception runtime split, common BSS, and data pooling as described in [prior art](re/PRIOR_ART.md#matching-knowledge-worth-keeping).
4. Wire the verifier after `ninja`. Run a clean build twice and confirm identical outputs.

Gate: a clean build passes `tools/verify.py verify` against the frozen DOL manifest. This is a build milestone with zero source coverage; every object is still an original fallback. Record that explicitly.

## Phase 4: Denominator and first source

Goal: measurable progress and the first reconstructed translation units.

1. Generate objdiff configuration and report. Use its code-byte and function counts as the denominator, and document the counting method in [matching](MATCHING.md). Then pass the report to `tools/progress.py` and add the treemap image to the README.
2. Start with library code that has matching source in other decompilations, recording provenance and license for every reused header or file.
3. Use Ghidra or another disassembler for understanding; keep databases in `docs/re/local/`. Use decomp.me scratches for collaboration and record their links in the ledger, never in source.
4. Mark each object `Matching` only when objdiff shows identity and the full build still passes verification. Record each unit in the [reconstruction ledger](PROGRESS.md#reconstruction-ledger).

Gate: at least one translation unit is source-built and linked, the full DOL still verifies, and progress is reported against the documented denominator.

## Phase 5: Game code reconstruction

Goal: grow source coverage across game code.

1. Recover translation-unit boundaries using assert paths, padding, common BSS inflation, and vtable and RTTI-like data where present.
2. Name symbols with evidence. Without a map, game names are chosen names; record confidence and evidence in [symbols](re/SYMBOLS.md).
3. Establish shared types and layouts in `include/` only after offsets are confirmed, and document them in symbols, not in comments.
4. Extract embedded data tables to editable assets where it improves readability, documenting each asset rule.
5. Keep `NonMatching` work buildable for diffing while the original object remains linked.

Gate: per milestone, progress numbers, ledger, and verification reports agree.

## Phase 6: Full executable and package

1. Eliminate all fallbacks in the claimed scope; list any that remain as blockers.
2. Rebuild the loader content if it is in scope, then any compressed or archived payloads.
3. Investigate WAD packaging: content order, alignment, encryption, hashes, and signature data. Signing and encryption may block exact package reproduction; record that as a blocker rather than weakening the claim.

Gate: the [completion gates](MATCHING.md#completion-gates) for the claimed scope.

## Cross-cutting practices

- **Formatting.** Adopt a `.clang-format` once source exists; Petari's is a reasonable starting point.
- **Progress images.** Keep `config/progress.json` in step with each gate and regenerate the README images as described in [progress images](PROGRESS.md#progress-images).
- **CI.** Once a build exists, a private build container holding originals can run configure, build, verification, objdiff reporting, and progress image rendering on every push. Never put originals in the public repository.
- **Multiple releases.** Other regions or revisions get separate targets and manifests. Use a version define and per-object version matching only after a second release is identified.
- **Session hygiene.** Follow the [project rules](PROJECT_RULES.md) session workflow and update living documents every session.
