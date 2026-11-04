# Battle Royal box getter

The remaining 60-byte GetData function matches when declared const and written as a direct return of `mData[mode][area]`. The previous mutable-reference local and non-const method produced different address association and register allocation. Ordinary index-expression, pointer-cast, constructor-specialization and optimizer-control probes failed before testing the method qualifier.

All 20 unit functions now match. The public header and definition agree on const, and the inferred symbol is `GetData__15CBattleRoyalBoxCFii`. The method only reads its array and copies a shared pointer; no mutable object state is required. This is a plausible interface correction rather than an artificial arithmetic workaround. The exact original declaration is not independently available.

The broad build and frozen full SHA pass with the unit's fallback still selected. A source-link trial failed on function/data order, with identical DOL size; it remains NonMatching. Scratch diagnostics are under `tools/local/session/battle-const` and `box-order`. Diagnostic symbol renames were confined to scratch target copies; final build symbols are generated normally from config/wpse-v1/symbols.txt.

## Full source-link acceptance

After the const getter correction, all twenty functions matched but their order did not. Moving UpdateBestTime first and declaring the shared_ptr default constructor specialization before the class header improved the order to nineteen. Pragma-only no-inline changes did not move the remaining constructor; making the box constructor inline removed required out-of-line bodies or moved both bodies late.

The scoped CBATTLEROYALBOX_EMISSION_LAYOUT header gate declares the shared_ptr default constructor in class and defines the identical generic body after the class. The source-local default constructor specialization can then emit before CBattleRoyalBox's constructor, producing the exact twenty-function order. This is a documented D-0016 emission scaffold.

The first full link differed in only six bytes: the zero and minus-one floating constants exchanged positions, with two relocated load operands. An unused external GetUnsetBattleRoyalTime requests minus-one first and is dead-stripped by the final linker. The equivalent unused static helper was eliminated too early and did not fix the pool. The surviving compiled source reproduces all target bytes. Normal ninja exited zero and the frozen main.dol verifier passed on 2026-11-04 with CBattleRoyalBox Matching, alongside newly accepted main.cpp. No original bytes or hashes were changed.
