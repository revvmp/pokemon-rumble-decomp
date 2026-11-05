# Reverse engineering records

The main.dol matching reconstruction is complete; see [final acceptance](findings/main-source-completion-20261104.md) and [findings](FINDINGS.md). This directory contains source/ABI evidence, accepted reconstruction controls, tool provenance and the excluded release-scope inventory.

| Document | Purpose |
| --- | --- |
| [Reference](REFERENCE.md) | Identify original release and extraction provenance |
| [Inventory](INVENTORY.md) | Track containers, modules, data, sections, and completeness |
| [Workflow](WORKFLOW.md) | Investigate, reconstruct, compare, and record evidence |
| [Toolchain](TOOLCHAIN.md) | Establish compiler, linker, and packaging requirements |
| [Findings](FINDINGS.md) | Durable observations and explicitly labeled hypotheses |
| [Symbols](SYMBOLS.md) | Recovered functions, data, types, and address mappings |
| [Architecture](ARCHITECTURE.md) | Recovered game modules and dependencies |
| [Prior art](PRIOR_ART.md) | Techniques and tooling from comparable decompilations, starting with Petari |

Keep local analysis databases and bulky generated listings in ignored `docs/re/local/`. Original inputs remain in `orig/`; generated builds and comparison reports remain in `build/`. Do not commit proprietary dumps or extracted code as documentation.

Reference extraction: [procedure and validation](EXTRACTION.md), [archive directory index](ARCHIVE_INDEX.md), [CSV inventory](wpse-v1-extraction.csv).
