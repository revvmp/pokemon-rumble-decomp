# Project architecture

## Design boundary

This document defines the development and reconstruction architecture. It does not assert an unobserved game architecture. Recovered game subsystems and their dependencies belong in [the reverse engineering architecture record](re/ARCHITECTURE.md), supported by addressable evidence.

The project has five boundaries: immutable originals, investigation evidence, reconstructed source, reproducible builds, and acceptance verification. Outputs must not flow backward into the reference inputs.

## Repository ownership

| Path | Responsibility | Versioned content |
| --- | --- | --- |
| `docs/` | Project rules, setup, architecture, acceptance, progress, handoff | Human-readable Markdown |
| `docs/re/` | Reverse engineering workflow, inventories, symbols, toolchain evidence, recovered architecture | Independently written evidence and observations |
| `docs/re/local/` | Local analysis databases and bulky raw outputs | None; ignored |
| `config/targets/` | Release identity, comparison scope, reference size and SHA-256 | JSON manifests |
| `config/toolchain.json` | Pinned build tools, flags, environment, evidence links | Configuration; unknowns stay null |
| `config/<target>/` | Future section layouts, linker scripts, compilation-unit ordering, asset rules | Only evidence-backed build configuration |
| `src/` | Reconstructed implementation grouped by confirmed module or subsystem | Source added as reconstruction begins |
| `include/` | Shared types and interfaces with confirmed ABI | Headers added when established |
| `tools/` | Extraction orchestration, build support, comparison, progress tooling | Original source; currently only artifact verification |
| `tools/local/` | Locally acquired compiler and analysis tool binaries | None; ignored |
| `orig/<target>/` | Untouched reference artifacts and separately identified extraction inputs | None; ignored |
| `build/<target>/` | Objects, linked outputs, packaged artifacts, reports | None; ignored |

Directories for future source and build stages need not exist until their first real files are added. Do not create speculative subsystems. The root README and `AGENTS.md` remain small entry points; `LICENSE` remains at the repository root.

## Reconstruction stages

| Stage | Inputs | Outputs | Gate |
| --- | --- | --- | --- |
| Identify | Local original and observable release metadata | Target identity and provenance | No guessed region or revision |
| Inventory | Original container and recorded extraction procedure | Artifact list, digests, sections, address mapping | Complete scope and reproducible extraction |
| Analyze | Identified artifact, disassembler, recorded tool versions | Findings, symbols, types, dependency map | Facts separated from hypotheses |
| Reconstruct | Confirmed interfaces and behavior | Readable source and declarations | No undisclosed original-code substitution |
| Compile | Source, pinned compiler and flags | Target objects and diagnostics | Per-unit code generation inspected |
| Link | Objects, section layout, linker settings, data | Executables and modules | Full artifact identity verified |
| Package | Matching payloads and observed container rules | Release package when in scope | Exact package bytes, including metadata |
| Accept | Frozen manifest, originals, independently built outputs | Verification report and progress evidence | All acceptance gates satisfied |

Only manifest fingerprinting and artifact acceptance tooling exist today. Extraction, compilation, linking, and packaging commands must be implemented after their requirements are established. The actual build driver is deliberately undecided.

## Source and interface design

Use confirmed module boundaries first, then subdivide into subsystems when call graphs and data ownership support them. Record the original compilation-unit and link order separately from the logical organization. Do not reorganize matching code casually if ordering changes output bytes.

Keep target-specific addresses, layout, and flags in target configuration. Shared headers express established ABI, type sizes, alignment, and declarations; uncertain interpretations stay in findings until validated. Isolate independently reconstructed runtime or library code from game-specific code once that distinction is proven. Do not assume a particular SDK or library version.

Keep executable reconstruction separate from asset processing. Asset rules must document whether each output is generated from an editable representation or requires local original data. A byte-preserving data dependency is disclosed; an original executable dependency blocks a full source reconstruction claim.

## Reproducibility contract

Pin tool identity with version, acquisition source, and binary SHA-256. Record host requirements, exact flags, environment variables, locale, working directory assumptions, link order, section alignment, padding, and packaging metadata as they become known. Do not use current timestamps, random ordering, or absolute machine paths in generated artifacts without an explicit reproducibility strategy.

A future build entry point must keep outputs within `build/<target>/`, fail when required inputs or tools are missing, avoid editing references, and expose the actual commands needed to reproduce a result. Acceptance runs independently after the build. A clean rebuild is required at milestone acceptance and after toolchain or determinism changes.

## Evidence and state

`config/targets/` is authoritative for recorded expected bytes. `docs/re/INVENTORY.md` is authoritative for the discovered content inventory and coverage gaps. `docs/re/FINDINGS.md` records evidence; `docs/re/ARCHITECTURE.md` summarizes confirmed relationships. `docs/PROGRESS.md` reports achieved milestones and source coverage. `docs/HANDOFF.md` describes the next executable action.

These documents are manually maintained by every contributor, including LLMs, under the living documentation contract. No background process automatically updates them. Generated reports are local evidence, not a replacement for reviewing scope and source coverage.
