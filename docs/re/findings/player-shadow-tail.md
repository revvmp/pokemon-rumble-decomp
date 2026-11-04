# CPlayerPii shadow-name tail

The current baseline is 253/254 exact functions. GetShadowName remains 88.85714% with the same 84-byte instruction inventory. The target stores LR before the constant-table lwzu and loads mpPlayer immediately after that first table load; source moves lwzu before the LR store and mpPlayer after all four table loads.

No candidate from this continuation was retained. Const-casting this, volatile player loads, integer pointer round trips, scalar table structures, external/static tables, register locals, and identity dependencies did not improve the baseline. Scoped common-subexpression, lifetime, scalarization, dead-assignment and pointer-analysis pragmas did not solve it; disabling scheduling changes other emitted weak helpers and worsens this function. Explicit per-element copies and union copies also did not match.

A combined local aggregate containing the four-name array followed by the player pointer gives the target mpPlayer-load position, but retains an extra store of the player pointer at stack +0x18 and still schedules the initial lwzu before the LR store. It is only a diagnostic clue, not an accepted candidate. Scratch scripts and objects are under tools/local/session/player-shadow and player-*.py.

## Traps

A local aggregate can improve one schedule dependency while introducing an observable object store. Compare the complete instruction stream rather than retaining a local scheduling improvement. No shared header, symbol, or main-source changes were made.
