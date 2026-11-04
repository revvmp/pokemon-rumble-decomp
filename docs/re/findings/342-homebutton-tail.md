# F-342-1: Home Button final register-allocation attempts

2026-11-04, source at `d14a20ca1`, Wii/1.1 and the configured game flags. No source change retained and no new matching claim.

`UpdateHomeButton` remains 99.64555% in objdiff. Its 26 differing instruction rows concern the stick divisor and X component registers, the saved waiting-state register and registers reused around it, and a merged zero used by controller reset and the `InitializeHomeButtonInfo` argument. All other instruction shapes match. `InitializeHomeButtonInfo` has only relocation-label differences in strict unitdiff; its prior report match is recorded in sprint-ra.

Direct compiler experiments used isolated input/output files under ignored `tools/local/session/342-candidates/`, the original split object, and objdiff-cli `diff -1 <original> -2 <candidate>`. They did not alter main build outputs. Earlier experiments used the isolated issue worktree; the owner subsequently requested work on main, and that worktree was left clean.

Rejected variants include per-function common-subexpression, propagation, lifetime, dead-assignment, scheduling, peephole and strength-reduction pragmas; integer and Boolean waiting-state types; nested waiting-state accessors; predeclared waiting locals; exception specifications and rethrow regions; separate scalar loads/divisions in every valid order; vector temporaries; one- and two-component division helpers with parameter-order permutations; and controller-reset helpers with integer/Boolean arguments and exception specifications. None produced an exact function match. The best scalar helper score was 99.65309%, only a marginal change, and was not retained.

## Traps

The allocator trace shows the divisor as temporary f67 and the later X load as f68; descending allocation assigns f68 to f0 and f67 to f1, opposite the target. The named waiting variable is eliminated: its loaded value lives in a generated high-degree node, so changing its source declaration position or integer type alone does not control the surviving register. These are observations for this function, not general restrictions on source transformations.

The remaining code and data/link work is still open. No full build or artifact verification was performed for these rejected candidates.
