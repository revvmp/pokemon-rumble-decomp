# CPlayerPii shadow-name tail

The current baseline is 253/254 exact functions. GetShadowName remains 88.85714% with the same 84-byte instruction inventory. The target stores LR before the constant-table lwzu and loads mpPlayer immediately after that first table load; source moves lwzu before the LR store and mpPlayer after all four table loads.

No candidate from this continuation was retained. Const-casting this, volatile player loads, integer pointer round trips, scalar table structures, external/static tables, register locals, and identity dependencies did not improve the baseline. Scoped common-subexpression, lifetime, scalarization, dead-assignment and pointer-analysis pragmas did not solve it; disabling scheduling changes other emitted weak helpers and worsens this function. Explicit per-element copies and union copies also did not match.

A combined local aggregate containing the four-name array followed by the player pointer gives the target mpPlayer-load position, but retains an extra store of the player pointer at stack +0x18 and still schedules the initial lwzu before the LR store. It is only a diagnostic clue, not an accepted candidate. Scratch scripts and objects are under tools/local/session/player-shadow and player-*.py.

## Traps

A local aggregate can improve one schedule dependency while introducing an observable object store. Compare the complete instruction stream rather than retaining a local scheduling improvement. No shared header, symbol, or main-source changes were made.

## 2026-11-04 initializer and isolated-C continuation

No additional GetShadowName change is retained. Fresh controls include an ABI-identical external C declaration of the known GetPlayerNo symbol, inline aggregate/selection helpers with depth controls, a declaration-only inline-policy overlay, noexcept-style `throw()` and try/catch forms, pointer-array volatile/restrict qualifiers, const/nonconst struct and union array wrappers, empty trailing aggregate members, explicit memcpy/memmove, and player assignments embedded in individual array initializers. The latter preserves most constant initialization but replaces one table load with an explicit string-address materialization, adding an instruction. Explicit element initialization from a static table remains 67.14286% at 88 bytes.

Combining scheduler model 604 with every discovered individual optimization switch does not improve the earlier 90.47619% diagnostic. Numeric-switch compile failures are excluded. Model 750 is explicitly restored after each candidate. The original simple 88.85714% function remains in main.

A private C-shaped wrapper reproduces the same 21-instruction baseline schedule and allows the native C permuter to search this otherwise C++ member. The scratch wrapper uses the already evidenced player-member offset solely as a diagnostic type view and calls the original GetPlayerNo ABI; it is not reconstructed source and must never be substituted for the real class. The private scorer normalizes only the original local name-table label against the generated local constant label. Any zero-score result still requires porting into CPlayerPii::GetShadowName and full-unit objdiff and source-link verification. The active searches cover the normal and 604 scheduler models under `tools/local/session/dp61-GetShadowName*`; drivers and logs use `player61-` and direct compiler controls use `player-61-`.

Further pointer-member const/restrict/reference views and inline/weak/noinline function attributes leave the getter unchanged. A diagnostic compiler-version sweep of the isolated C form found no exact result across GC3.0a3/3.0a5.2 and the installed Wii1.0 through1.7 family; no toolchain setting is changed. Final direct recompilation of the real CPlayerPii source still has only GetShadowName unmatched among real functions. The private array/header overlays have been removed.
