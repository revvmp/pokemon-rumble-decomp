# Issue 261: generic motion dispatch

## F-261-1: Complete source-selected dispatch unit

Original script names identify twenty-six ordinary motion dispatchers at `0x801504C0` through exclusive `0x80151224`. Each peeks at the actual argument's world-object type and delegates to the original typed binding. Play, request, extended forms, and length lookup check CPii first, then world camera, null object, and light set, with model object as the default. Length-based requests return zero directly for unsupported camera, null-object and light-set types. Pause and motion-state operations use the latter four-way dispatch. Existing canonical TYPE enums independently supply the exact native masks; no new flag or numeric cast is invented. The peek index is zero, one or three according to the independently observed wrapper.

The ordinary producer uses only complete canonical game declarations and unchanged Wii/1.1 game flags. All twenty-six bodies match all 3,180 instruction bytes and all 131 resolved direct-call operands. The complete 3,428-byte text section, including 248 native alignment bytes, also matches after applying relocations. All twenty-six eight-byte exception records and 312 exception-index bytes match, including exact function pointers and extents. The object emits no data, small data, BSS, constructor or other allocated contribution.

The actual unit ranges are `.text 0x801504C0–0x80151224`, `extab 0x80030D18–0x80030DE8`, and `extabindex 0x8004EBE8–0x8004ED20`. The previous WorldObj template helper at `0x801504B0` belongs to the preceding unit. The next ordinary binding at `0x80151230` is GetPadDirValue and starts the independent controller group. The adjacent type-specialized groups and this common dispatch module have distinct implementations; the survey bind_6 label did not establish source ownership. The first function is inside #261, so #261 owns this complete unit's split and object line. Its final two functions are in original #262; no competing source or header claim was taken.

The fallback split check passes the full frozen DOL verification. The reconstructed object then passes unitdiff at 100% for every function and all three allocated sections. Marking the complete unit Matching and running the actual configure, Ninja, and frozen verifier succeeds. A newly generated MAP and the final ELF place every definition at its original address and size in ScriptMotion.o. The direct Ninja link graph selects `src/game/ScriptMotion.o` and excludes the corresponding original object. The source auditor checks all26 definitions and their native bytes; this is source-selected reconstruction, not fallback identity. The official landing wrapper repeats fresh-MAP build, frozen verification, strict allocated code/relocation, exception/index and source-graph audits after each rebase.

Original binding-table names are preserved exactly. The module filename ScriptMotion.cpp is descriptive because no original file-name string survives in this fragment. Outside symbol corrections name only existing script callees at their unchanged addresses, sizes and types so source calls resolve; they do not take their source files or splits. No shared header or class layout changed.

## Original issue inventory coverage

| Original issue | Original functions | Source-selected here | Remaining |
| --- | --- | --- | --- |
| #261 | 25 | 24 /2,948 code bytes | Eight-byte preceding WorldObj helper at `0x801504B0` |
| #262 | 27 | 2 /232 code bytes | Twenty-five original controller/player/template functions |

Neither original issue is complete. The twenty-six dispatchers form a complete independently verified source unit, but this PR must use Part of #261. The preceding helper must not be moved into this unit merely to change the count.

## Reproduction and artifacts

The immutable source/control artifacts are retained in `261-reference`: ordinary producer and compiler command, original native descriptors, strict whole-text/target proof, strict exception/index proof, and source-selected MAP/ELF graph proof. `261-audit.py` compares the actual configured source object; `261-eh-audit.py` checks all native exception entries; `261-audit-link.py` checks current linked ownership, MAP columns, function addresses/sizes and original-inventory coverage. `261-land.py` runs these audits inside the official local landing verifier after a fresh source-selected map build. All expected bytes remain the original frozen release.

## Traps

A survey block may include one helper from a preceding source unit and omit two final functions from the new one. Whole-unit source acceptance therefore does not automatically close the original block issue. Track both inventories and inspect the actual final source graph.

The dispatcher type-test order matters for derived objects carrying several type bits. CPii handling precedes its model fallback where the original provides it. Unsupported length-based variants return directly; adding argument pops or a pushed result would change native behavior.
