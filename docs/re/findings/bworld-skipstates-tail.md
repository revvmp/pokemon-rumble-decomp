# BWorld skip-state rematerialization

2026-11-04 direct canonical Wii/1.1 compilation confirms Calc is 1900 bytes versus native 1904 (99.69538%), and CalcMtx is 660 versus native 664 (99.36747%). Both lack the first skip-bit path's `li r0, 0` before `ori r30, r0, 2`; source instead updates r30 directly. Calc additionally keeps the removed-object traversal node in r26 versus native r27. No source or header change is retained.

Earlier ordinary forms and flag controls are recorded in sprint-w10, sprint-r1, sprint-a12, sprint-a15 and sprint-a17. New isolated header controls included 64-bit state storage, selectively narrowed bit updates, by-value and const-reference initial-state parameters, local/aggregate/default zero objects, pointer/reference round trips, unions, volatile/register-qualified locals, bounded one-iteration loops, memory copy/zero operations, identity arithmetic, delayed zero uses, helper return types, and additional optimization-pass controls. None matches both required instructions.

Two diagnostics reproduce the complete native CalcMtx extent and every other instruction: a const-reference default zero gives an SDA load of zero instead of the required immediate load; computing a named flags value XOR the same member gives `xor r0, r4, r4` instead. Both reach 99.63856%, but a single wrong instruction is still nonmatching. The 64-bit-state variant instead produces `li r0, 2; or r30, r30, r0`, confirming that extra size alone is not enough. Direct constant-zero expressions fold to a single immediate assignment, while volatile zeros require real stack traffic.

Scoped `opt_propagation`, `opt_common_subs`, `opt_dead_assignments`, and `opt_scalarize` controls combined with several zero spellings produce substantial regressions. Additional individual front-end passes did not close the function; an isolated prelinearization-disabled compiler control triggered an internal compiler error in IroLinearForm.c. No compiler flag is changed in the repository.

Reproducers and complete comparisons are under ignored `tools/local/session/bworld-61-compile` and `bworld-61-*.py`. All probes use a private BWorld.h overlay; the main header is untouched.

## Traps

- Matching the missing instruction count does not establish the intended zero materialization. The SDA-load and self-XOR variants have the correct 664-byte size but fail the exact instruction comparison.
