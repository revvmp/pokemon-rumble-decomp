# Project documentation

The objective is a readable, reproducible source reconstruction of Pokémon Rumble with exact original artifact hashes and direct byte equality. This repository currently provides the development foundation, not a decompiled or matching game.

## Reading order

1. [Project rules](PROJECT_RULES.md): requirements for humans and LLMs.
2. [Architecture](ARCHITECTURE.md): repository boundaries, build stages, and evidence flow.
3. [Setup](SETUP.md): release identification, local originals, and frozen manifests.
4. [Reverse engineering index](re/README.md): investigation and reconstruction.
5. [Matching](MATCHING.md): exact verification and completion gates.
6. [Progress](PROGRESS.md) and [handoff](HANDOFF.md): current state and next actions.
7. [Roadmap](ROADMAP.md): phased plan and gates from extraction to a full match.
8. [Collaboration](COLLABORATION.md), [workstreams](WORKSTREAMS.md), and the [ledger](ledger/README.md): parallel work, issue map, and per-unit status.
9. [Decisions](DECISIONS.md): durable project choices and rationale.
10. [Validation](VALIDATION.md): executed checks and their limits.

All narrative documentation belongs here. Reverse engineering methods, observations, artifact inventories, symbol maps, toolchain investigations, and recovered game architecture belong in `docs/re/`. The root README and `AGENTS.md` are discovery entry points only. Executable tooling, build configuration, and reconstructed source live in their designated repository directories rather than among documentation.

The [MIT license](../LICENSE) covers this project's original contributions. Original game files, extracted proprietary data, private keys, and proprietary tool binaries remain local and are not included in version control.
