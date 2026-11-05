# CPlayerPii shadow-name tail

The native candidate now compares all 254 functions exactly. GetShadowName's 84 bytes were completed on 2026-11-04 with the two local source controls documented below. The first source-link trial failed on generated deque helper and exception-record order; the unit remains NonMatching while that separate tail is reconstructed. The previous baseline was 253/254 exact functions, with GetShadowName at 88.85714%. The target stores LR before the constant-table lwzu and loads mpPlayer immediately after that first table load; the previous source moved lwzu before the LR store and mpPlayer after all four table loads.

No candidate from this continuation was retained. Const-casting this, volatile player loads, integer pointer round trips, scalar table structures, external/static tables, register locals, and identity dependencies did not improve the baseline. Scoped common-subexpression, lifetime, scalarization, dead-assignment and pointer-analysis pragmas did not solve it; disabling scheduling changes other emitted weak helpers and worsens this function. Explicit per-element copies and union copies also did not match.

A combined local aggregate containing the four-name array followed by the player pointer gives the target mpPlayer-load position, but retains an extra store of the player pointer at stack +0x18 and still schedules the initial lwzu before the LR store. It is only a diagnostic clue, not an accepted candidate. Scratch scripts and objects are under tools/local/session/player-shadow and player-*.py.

## Traps

A local aggregate can improve one schedule dependency while introducing an observable object store. Compare the complete instruction stream rather than retaining a local scheduling improvement. No shared header, symbol, or main-source changes were made.

## 2026-11-04 initializer and isolated-C continuation

No additional GetShadowName change is retained. Fresh controls include an ABI-identical external C declaration of the known GetPlayerNo symbol, inline aggregate/selection helpers with depth controls, a declaration-only inline-policy overlay, noexcept-style `throw()` and try/catch forms, pointer-array volatile/restrict qualifiers, const/nonconst struct and union array wrappers, empty trailing aggregate members, explicit memcpy/memmove, and player assignments embedded in individual array initializers. The latter preserves most constant initialization but replaces one table load with an explicit string-address materialization, adding an instruction. Explicit element initialization from a static table remains 67.14286% at 88 bytes.

Combining scheduler model 604 with every discovered individual optimization switch does not improve the earlier 90.47619% diagnostic. Numeric-switch compile failures are excluded. Model 750 is explicitly restored after each candidate. The original simple 88.85714% function remains in main.

A private C-shaped wrapper reproduces the same 21-instruction baseline schedule and allows the native C permuter to search this otherwise C++ member. The scratch wrapper uses the already evidenced player-member offset solely as a diagnostic type view and calls the original GetPlayerNo ABI; it is not reconstructed source and must never be substituted for the real class. The private scorer normalizes only the original local name-table label against the generated local constant label. Any zero-score result still requires porting into CPlayerPii::GetShadowName and full-unit objdiff and source-link verification. The active searches cover the normal and 604 scheduler models under `tools/local/session/dp61-GetShadowName*`; drivers and logs use `player61-` and direct compiler controls use `player-61-`.

Further pointer-member const/restrict/reference views and inline/weak/noinline function attributes leave the getter unchanged. A diagnostic compiler-version sweep of the isolated C form found no exact result across GC3.0a3/3.0a5.2 and the installed Wii1.0 through1.7 family; no toolchain setting is changed. Final direct recompilation of the real CPlayerPii source still has only GetShadowName unmatched among real functions. The private array/header overlays have been removed.

## Native named-output continuation, 2026-11-04

After completing the ParticleManager and ArchiveFontBase tails, the next pass reproduced this function's 88.85714% baseline. The original 84-byte function remains in source. None of the following private controls produced an exact candidate.

The coordinator's sixteen array-output helpers initially failed to compile because Wii/1.1 rejects `__declspec(noinline)` on the player-number getter. A declaration-only CPlayer header and late GetPlayerNo definition make those candidates compile, but their array helpers remain out of line while the final dont_inline state is on. Appending dont_inline off at the end changes the deferred inline decisions of this translation unit, including many previously exact weak functions. When the getter is left external under that diagnostic policy, return-by-value array helpers actually inline and reproduce the original 88.85714% schedule; output-table helpers grow to 100 bytes. This is a diagnostic, not a viable whole-unit change.

Accepted always_inline function-attribute spellings did not override the final dont_inline state for these helpers. Moving the caller declaration under dont_inline off, or making the late getter definition strong, did not resolve this. These results are specific to this deferred-generation case; they do not establish that the compiler ignores those attributes generally.

Additional bounded source probes found no improvement:

- 64 typed enum/integer/pointer output forms with by-value or reference player inputs.
- 48 combined array/player aggregates with typed alias analysis and enum index views.
- 96 array-entry type, qualifier and aggregate representations.
- Forty explicit copies from an early or late global table, with the player read at different copy positions.
- Twelve equivalent member-call and member-pointer expressions.
- All 164 combinations of the discovered 82 optimizer option names with on/off at the end of the translation unit; unsupported numeric-value forms fail compilation and are not retained.
- Six private enum-return views of GetPlayerNo, with the ordinary array and aggregate diagnostic. The other real function comparisons remain unchanged for those six views, but the target function does not improve.

The source baseline and full diff are under `tools/local/tail-20261104/game_CPlayerPii/pf_player_baseline/`. New drivers use `tools/local/tail-20261104/pf_player_*.py`. The corrected output-helper results and final-option results are recorded in `pf_player_output_results.json` and `pf_player_finalflag_results.json`. These families did not produce a retained change; the later exact source below supersedes that result.

The remaining unsuccessful controls were 24 alignment forms, twelve inline-policy command combinations, 32 aggregate-copy forms, 32 reference-member aggregates, twelve union-lifetime forms, 36 global-struct table copies, 24 table-derived identity expressions, and 24 constructor-output variants. A reference-member aggregate reached 92.38095% at 88 bytes, but its extra stack store and early player load make it unsuitable. That score is not a retained improvement.

## Exact local alias controls, 2026-11-04

The 84-byte function is exact with two local types:

- `ShadowNames` contains the four string pointers and declares an unused copy-assignment operator. This makes the class non-POD without emitting a helper or adding storage.
- A local union contains `const CPlayerPii*` and a full-range unsigned enum. The function assigns `this` to the pointer member, converts that pointer to the enum member, and obtains `mpPlayer` through the converted address. Every union read uses the member most recently written; the conversion adds no machine instructions.

The same native trace that captured colouring now records scheduler edges using the existing `sort_sched_hook.js`. In the old source, the LR stack store has no memory-alias dependency on the table loads or player load. The non-POD table introduces edges from that store to all four table loads. The enum union also introduces the edge from the LR store to the player load. The resulting final schedule is exactly `lis`, LR store, first table `lwzu`, player `lwz`, and the remaining table loads, followed by all four stack stores. Registers, branches, frame size, and function size match.

Neither control alone is sufficient. The non-POD table by itself remains 79.85714% at 84 bytes; 56 direct alias-load forms with the original array did not beat the old baseline. Combining the controls produced an exact result on candidate 24 of the bounded table/alias family. The enum union is a source-only D-0016 control, and the copy-assignment declaration is deliberately unused. There is no assembly, object rewriting, shared-header change, or compiler-flag change.

The first exact candidate had `c9x_alias_by_type on` left active after the function and regressed three deferred deque helpers. The alias pragma is unnecessary: removing it entirely keeps GetShadowName exact and restores all 253 other functions. The retained source has no new pragma. This is a measured deferred-generation trap for this translation unit, not evidence that every scoped optimizer setting behaves the same way.

The readable candidate is `tools/local/tail-20261104/game_CPlayerPii/pf_player_ready/CPlayerPii.cpp`, installed in the owned source file. Its native `diff.json` compares all 254 real functions at 100%, and `scheduler.txt` records the exact dependencies and schedule. `data-audit.json` compares the candidate with the saved pre-change native object:

- Every emitted function identity, address, and size is unchanged.
- `.rodata` (16 bytes), `.data` (1354 bytes), `.sdata` (160 bytes), `extab` (1912 bytes), and `extabindex` (2088 bytes) retain identical bytes and relocation destinations.
- The four name-table relocations still resolve to `.data` offsets 32, 56, 80, and 104, containing the original p1 through p4 resource names.
- The sole text-relocation change moves the table's LO16 relocation four bytes later, with the now-correct `lwzu` instruction. Its destination is unchanged.

The reference [mweppc alias implementation](https://github.com/doldecomp/mweppc/blob/919828d97d33d236da675824d43c5d57332afe74/src/compiler/optimizer/Alias.c) treats const non-POD class objects differently from const pointer arrays. That is useful context for the source-control hypothesis; the acceptance evidence here is the shipped Wii/1.1 compiler's measured graph and native object comparisons.

## Native source-link tail and helper identity audit

The coordinator's `player-shadow-complete` isolated native link exited zero and produced the original DOL size, but the frozen comparison rejected 12,523 differing bytes. The coordinator independently verified that the executable's complete `.rodata`, `.data`, `.sdata`, and `.sdata2` sections are already byte-identical. The differences are in `.text` (12,080 bytes), `extab` (173), and `extabindex` (270). Retained function sizes match; 191 generated function addresses move because the reconstructed deque header emits its helpers in a different order. The exact GetShadowName implementation is retained. No alternate destructor identity has been inferred from these order differences.

Private source-only declaration experiments use the preserved original split object `tools/local/tail-20261104/pf_originals/game_CPlayerPii.o`, SHA256 `F1D6CD710F4EF11DB0D560DF4BA09DCD51DE2C25AC9357B72075509898E70714`. The comparator requires the frozen set of 254 function identities, so a missing function cannot disappear silently from its count. It records ordered-prefix length and SequenceMatcher edit distance as diagnostic order measures, not acceptance scores.

Moving the complete `__deque_ops` buf/alloc accessor groups after its destructor declaration, moving back_capacity after push_back, and moving destroy_all after pop_front restores the first generated ops wave. The buffer's push_back, pop_front, clear, and cap/alloc declaration placement restores the following buffer wave. The best current private header additionally places the three non-template copy/copy_backward definitions inside `__cdeque_buf`; this changes generation phase without changing their machine instructions. The first 92 retained functions are ordered correctly and the diagnostic order distance falls from the original 139 to 74. This is incomplete and has not been installed in the shared header.

The ordinary reconstructed `std::__throw_deque_length_error(const char*)` name at `0x800FA240` has no established original mangled spelling. A private D-0016 replacement uses `std::__deque_common<true>::throw_length_error(const char*)`, with its template first declared immediately before `__deque_iterator`. This gives the observed generation wave. Its 64 instruction bytes, six relocation destinations including the exception-type literal, eight exception bytes, and exception-index length are identical to the original after resolving ordinary function and literal identities. The exception bytes are `0008000000000000`.

A scan of all original DOL text branch instructions finds exactly two direct callers of this helper:

- `__deque_ops<VEC3>::allocate_back` calls from `0x800F9F68`, offset `0xF8` in the function at `0x800F9E70`.
- CSaveManager's `__deque_imp<WorkType>::allocate_back` calls from `0x8010A418`, offset `0xE8` in the function at `0x8010A330`.

Both caller bodies, normalized relocations, exception bytes, and exception-index lengths remain identical under the proposed helper identity. A helper-only private CSaveManager header retains all 243 original functions in the same order; the sole non-100 comparison is its renamed call relocation, which the byte and destination audit resolves. Any accepted shared rename must update both call sites together, because keeping the old helper definition elsewhere can emit an unwanted second copy. The audit is `game_CPlayerPii/pf_player_order_capalloc0/helper-identity.json`; the CSaveManager control is `game_CSaveManager/pf_player_helper_identity/` under the same ignored experiment root.

The current first order mismatch is the four-byte buffer swap wrapper: it emits after clear in the first buffer wave, but the original places it after the later cap/alloc wave at `0x800FC0E0`. Moving its body outside the class or to the source end, reordering its pointer-wrapper declaration, dependent allocator-rebind bases, early template declarations, receiver casts, select-based dependent receivers, and local noinline controls do not produce the required delay. Moving dont_inline after the includes also leaves the order unchanged. A member-pointer call changes instructions and is rejected. Early unused iterator construction moves cap and iterator helpers too early; it is not retained. The private drivers and exact outcomes are saved as `pf_player_order_*.py`, with per-candidate native objects and `order.json` files. No compiler or linker setting is changed by these experiments.
