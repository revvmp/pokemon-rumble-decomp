# CPii missing exception methods and personal-data vector cleanup

2026-11-04, `wpse-v1`, Wii/1.1. This follows [sprint-tZ](sprint-tZ.md) and [sprint-a12](sprint-a12.md). Neither CPii nor CGameSession is accepted as a fully source-linked unit by this investigation yet.

## Missing bad_function_call virtuals

The original CPii unit retains the twelve-byte `std::tr1::bad_function_call::what` at `0x800E1810` and its sixty-four-byte destructor at `0x800E1A50`. The current reconstructed callers no longer instantiate their original discarded throw site. An unused ordinary factory returning `new std::tr1::bad_function_call` recreates the two virtual definitions without changing any existing caller. The factory is expected to be discarded when the complete unit is linked; full acceptance must verify that.

Unused inline throw helpers and explicit call-operator instantiations emit neither method. A direct throw helper or a discarded stack object emits the destructor too early. Constructing a heap object requests the vtable without explicitly requesting destruction, preserving the two different virtual emission phases. The retained factory is D-0016 scaffolding, not recovered original source. The coordinating agent's broad report confirms both methods at 100%.

## Personal-data vector destructor depth

A targeted specialization of `__vec_constructor<SD_PiiPersonalData::Data, Allocator, false>` derives from the unchanged `true` primary-template form. The primary implementation does not branch on that boolean template argument; the inherited storage, allocation behavior, and public vector API stay unchanged. The specialization forwards construction and adds one empty destructor level. This reproduces the extra original null branch and is recorded as D-0016 source shaping.

Repository searches found instantiated consumers of `vector<SD_PiiPersonalData::Data>` only in CPii and CGameSession. Direct compilation of both retains every previously exact function and yields:

| Function | Before | After |
| --- | --- | --- |
| CGameSession `vector<Data>` destructor | 96.77419% | 100% |
| CGameSession `SessionSnapshot` destructor | 97.72727% | 98.86364% |
| CPii `swap<vector<Data>>` | 97.698555% | 99.16514% |

The remaining swap differences include tag slots and register allocation. The snapshot destructor initially still lacked one redundant branch for its battle-royal vector member at offset `0x28`. A shared specialization for that element type regressed the already-exact CBattleRoyalBox exporter and destructor, so it was rejected. Instead, the local empty `BattleRoyalSnapshotData` subclass supplies precisely that destructor level for the snapshot alone. The complete broad build now reports the snapshot destructor at 100%, CGameSession at 218/219 functions, and CPii at 71/72. The coordinator verified the frozen full DOL hash with both units still using their original link fallbacks; these counts establish function gains, not full source-link acceptance of either unit. No generic vector implementation change was retained. Experiments with a second storage selector, hand-expanded copy/assignment bodies, and explicit generic destructors did not improve the targeted specialization.

## CGameSession GetSnapshot const qualification

The final CGameSession function was resolved independently of the vector shaping. `GetSnapshot` must be const and write its three scalar results directly into the destination. Const qualification permits the target's load caching across the stores; removing the temporary scalar locals reproduces the original 64-bit expression temporary allocation. Const alone reached 93.21429%, while direct stores alone reached 82.42857%; together they reproduce all 168 bytes exactly. The earlier 88.2381% form used a non-const method with local caches.

The source and inferred symbol are corrected to `GetSnapshot__Q226@unnamed@CGameSession_cpp@12CGameSessionCFPQ226@unnamed@CGameSession_cpp@15SessionSnapshot`. Isolated comparison used only a diagnostic symbol-name remap of the compiled scratch object; no modified object is supplied to the real build. The integrating agent performs the complete source link using the corrected C++ signature and regenerated target names. Full CGameSession source-link acceptance remains pending.

## Traps

An added destructor layer also changes constructor inlining depth when constructors forward through it. This can improve cleanup branches while moving tag slots or register allocation in generic copy/swap operations. Check the complete consumers, not only the destructor that motivated the change.
