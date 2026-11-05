# Recovered game architecture

The main executable now has matching source units and evidenced interfaces in src/, include/, config/wpse-v1/ and [symbols](SYMBOLS.md). Confirmed library/unit boundaries are recorded in F-0012 and the per-unit findings. A consolidated architectural overview remains separate from this completed byte-matching reconstruction; do not infer unrecorded design intent from descriptive names.

## Recovery sequence

1. Identify executable modules, entry points, imports, exports, and startup flow.
2. Establish compilation units and runtime/library boundaries where evidence supports them.
3. Map call relationships and data ownership into candidate subsystems.
4. Confirm interfaces, lifecycle, resource flow, and cross-module dependencies.
5. Organize source around confirmed boundaries while preserving layout and link requirements for matching.

## Module record

For each module record target, artifact, address or section range, responsibility, entry points, owned data, dependencies, lifecycle, evidence IDs, confidence, unresolved boundaries, and source paths. Label candidate relationships as hypotheses until confirmed.

Add a dependency diagram only after enough relationships are established to make it useful. The [project architecture](../ARCHITECTURE.md) describes development stages, not recovered game facts.

## Executable inventory evidence

F-0003 identifies a main game DOL compressed inside content 1, with entry 0x80004050. Content 6 is the separately identified boot DOL, entry 0x81330000; F-0009 establishes its NAND-loader role. Loader reconstruction remains outside the accepted main.dol scope. Confirmed main-unit boundaries and interfaces are recorded in the per-unit findings and symbols record.
