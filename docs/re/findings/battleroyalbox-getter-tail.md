# Battle Royal box getter

The remaining 60-byte GetData function matches when declared const and written as a direct return of `mData[mode][area]`. The previous mutable-reference local and non-const method produced different address association and register allocation. Ordinary index-expression, pointer-cast, constructor-specialization and optimizer-control probes failed before testing the method qualifier.

All 20 unit functions now match. The public header and definition agree on const, and the inferred symbol is `GetData__15CBattleRoyalBoxCFii`. The method only reads its array and copies a shared pointer; no mutable object state is required. This is a plausible interface correction rather than an artificial arithmetic workaround. The exact original declaration is not independently available.

The broad build and frozen full SHA pass with the unit's fallback still selected. A source-link trial failed on function/data order, with identical DOL size; it remains NonMatching. Scratch diagnostics are under `tools/local/session/battle-const` and `box-order`. Diagnostic symbol renames were confined to scratch target copies; final build symbols are generated normally from config/wpse-v1/symbols.txt.
