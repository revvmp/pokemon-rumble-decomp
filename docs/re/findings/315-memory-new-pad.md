# F-315-1: memory allocation units and the following pad boundary

## Scope and status

Issue #315 inventories twenty entries from `0x801B0EF0` through `0x801B15F0`. The first twelve form a game memory unit ending at `0x801B1450`; four global allocation operators follow through `0x801B15A0`. The last four are callbacks belonging to the following pad unit, whose first function is therefore owned by #315, rather than a standalone four-function unit. The initial split retained original objects. F-315-3 below records subsequent source acceptance of the four allocation operators; the memory unit remains NonMatching and the pad unit remains original.

The memory/new filenames are descriptive hypotheses. No original source-file spelling has been recovered. The pinned KooShnoo reference has an empty `src/placeholder/pad.cpp`, so it supplies no implementation.

## Original-only boundaries

| Unit | Section | Native range |
| --- | --- | --- |
| game/memory.cpp | text | 801B0EF0-801B1450 |
| game/memory.cpp | extab | 8003822C-80038488 |
| game/memory.cpp | extabindex | 8005079C-80050820 |
| game/memory.cpp | data | 803BF3A8-803BF3B8 |
| game/memory.cpp | sdata | 804B81F0-804B81F8 |
| game/memory.cpp | sbss | 804B9138-804B9140 |
| game/new.cpp | text | 801B1450-801B15A0 |
| game/new.cpp | extab | 80038488-8003850C |
| game/new.cpp | extabindex | 80050820-80050838 |
| game/new.cpp | data | 803BF3B8-803BF3CC |

The allocator at `0x803BF3A8` is the canonical sixteen-byte `MEMAllocator`, referencing the two-function table at `0x804B81F0`. Its heap and two parameters are initially zero. Two four-byte private heap handles occupy `0x804B9138` and `0x804B913C`. The native exception-filter string at `0x803BF3B8` is `!std::bad_alloc!`; its seventeen bytes are followed by alignment. The pad vtable begins at `0x803BF3D0`.

The two delete functions at `0x801B1580` and `0x801B1590` each retain sixteen instruction bytes. IDA's twelve-byte body estimate omits an unreachable `blr`, but the frozen DOL and ordinary compiled operator bodies both contain `0x4E800020` at `0x801B158C` and `0x801B159C`. These are retained instructions, not zero alignment padding. The new unit ends at the next callback, `0x801B15A0`. An initial twelve-byte extent experiment was rejected after this full-byte comparison.

The late small-data word at `0x804B9BC0` is referenced as the global new-handler function pointer by both allocation operators. Its neighboring four bytes are not yet identified. This patch only names the outside object, preserving main's original eight-byte symbol extent and attributes, and leaves its definition in the original automatic object; attributing it to the early game new unit creates an ordering cycle. Marking the split common converts its definition to ordinary BSS and violates the actual SDA relocation, so that attempt was rejected.

## Native behavior and diagnostic reconstruction

Game Wii/1.1 flags from F-735-1 reproduce eleven of the twelve memory functions exactly in an independent, unselected diagnostic. The evidence pairs every instruction relocation with its actual native address rather than masking relocation words. It also compares 456 bytes of exception records and all corresponding native index entries. The allocator callback at the first address remains unresolved.

| Address | Bytes | Proven behavior |
| --- | --- | --- |
| 801B0EF0 | 144 | MEMAllocator allocation callback, MEM1 then MEM2, fixed alignment 32; remaining exception-region ordering difference |
| 801B0F80 | 116 | MEMAllocator free callback; null check, containing heap lookup and free |
| 801B1000 | 196 | Initialize MEM1 and MEM2 expanded heaps |
| 801B10D0 | 132 | Allocate from MEM1, then MEM2 on failure |
| 801B1160 | 96 | Null-safe free through containing heap |
| 801B11C0 | 84 | Allocate from MEM1 |
| 801B1220 | 100 | MEM1 allocation with explicit null failure branch |
| 801B1290 | 84 | Allocate from MEM2 |
| 801B12F0 | 100 | MEM2 allocation with explicit null failure branch |
| 801B1360 | 12 | Return the allocator address |
| 801B1370 | 120 | Existing `__sys_alloc`, MEM1/MEM2 fallback at alignment 32 |
| 801B13F0 | 96 | Existing `__sys_free`, null-safe containing-heap free |

Initialization reserves 32 bytes when MEM1 arena high is exactly `0x81800000`. It measures high minus low, allocates that extent through **OSAllocFromMEM1ArenaHi**, and creates an expanded heap with flags 7. It rounds MEM2 high down and low up to 64-byte boundaries, creates the second expanded heap with flags 7, and clears both MEM2 arena pointers. Pairing relocations distinguished the high allocator at `0x802B3B40` from the low allocator at `0x802B3B00`; both have the same C signature.

Natural empty exception specifications account for the observed unexpected-handler regions in the eleven accepted diagnostic bodies. Inlining two independently restricted per-heap functions into the allocation callback currently produces 148 bytes with an interleaved first handler, whereas native has 144 bytes with its three handlers after the allocation branches. Calling the general restricted allocator produces 132 bytes and only two handlers. Equivalent if-initializer and assignment/conditional-expression variants do not resolve that difference. Wii/1.3 reproduces the same diagnostic results; no compiler flag change was accepted.

The four following operator bodies have the standard new/new-array/delete/delete-array behavior. New treats size zero as four, calls malloc, invokes the global handler on failure, and returns null when no handler exists. All four ordinary candidate bodies now reproduce every native instruction byte, all call/data targets, 132 exception bytes and both indices: new136, new-array152, delete16, delete-array16. A complete standard bad_alloc declaration is required even when it is only used by the exception specification. The current project new header does not yet expose that type or new_handler.

The native exception region for new starts after the new-handler call, rather than the earlier malloc call. Declaring malloc with CodeWarrior's `__attribute__((nothrow))` reproduces the native region start; free needs the same ordinary attribute to suppress an otherwise emitted delete handler. Empty dynamic exception specifications alone do not express this compiler guarantee. The accepted diagnostic uses a bounded, independently authored stdlib declaration overlay with these two attributes, plus a declaration-only standard bad_alloc interface deriving from exception. Existing consumers must be checked before publishing canonical header changes. No pragma or compiler flag changed. Wii/1.3 does not fix the missing declaration attributes.

The callbacks at `0x801B15A0` and `0x801B15B0` allocate from MEM2 with alignment 32 and free memory with a success result of one. The two empty callbacks at `0x801B15E0` and `0x801B15F0`, together with their consumers in the subsequent pad initialization, establish the following unit boundary. Its full source and state remain to be reconstructed before #315 can be complete.

## Reproduction and validation

The independent scripts and generated objects are outside the checkout at `C:/MassStorage/GitHub/315-*`: `315-native-functions.json`, `315-memory-probe.cpp`, `315-compile-memory.py`, `315-audit-memory.py`, and `315-memory-native-proof.json`. The compile driver uses only canonical project headers and the actual game flags. A separate 1.3 driver records the alternate compiler experiment. These artifacts document diagnostic results, not source acceptance.

The original-only split was verified in the isolated `rumble-315` worktree:

- `python configure.py --map`: 0.
- `.venv/Scripts/ninja`: 0, `315-original-split-ninja6.log`.
- `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`: 0, frozen main.dol.

No expected hash changed. No original bytes were copied into reconstructed source. At that initial split checkpoint no unit was marked Matching. The later source checkpoint is recorded below.

## Traps

- IDA can exclude retained unreachable instructions from a function estimate. The two delete operators retain a final BLR; preserve their sixteen-byte native/compiler extents and the `0x801B15A0` split end.
- A late SDA variable cannot be moved to an early unit by declaring its split common: common storage changes the output section and causes a small-data relocation error. Keep the original definition until ownership and linker ordering are established.
- The bad_alloc string ends at an unaligned address. dtk rejects the automatic data fragment beginning at `0x803BF3C9`; the original-only split includes its observed four-byte alignment to `0x803BF3CC` instead.
- Masking relocation words conceals semantically wrong callees. The first initialization candidate called the low arena allocator; actual target pairing proved the native call uses the high allocator.
- A failed compile leaves an old diagnostic object on disk. Only audit after the compiler exits zero; rejected declaration experiments do not supply new matching evidence.

## F-315-3: source-linked global allocation operators

### Accepted scope

The actual game pipeline now selects `src/game/new.cpp` as Matching under Wii/1.1 and `cflags_game`. Its four functions contain 320 native instruction bytes: scalar new at `0x801B1450` (136), array new at `0x801B14E0` (152), scalar delete at `0x801B1580` (16), and array delete at `0x801B1590` (16). Both retained terminal BLRs are included. The seventeen-byte exception-filter string and ordinary linker alignment reproduce the native twenty-byte data extent without fabricated padding in source.

`src/game/memory.cpp` is registered NonMatching. Eleven of its twelve functions reproduce all 1136 instruction bytes, paired call/data targets, 456 exception bytes, and their native index entries. The allocation callback at `0x801B0EF0` remains 132 bytes against native 144, with two exception regions against three. Its complete 1280-byte native unit remains selected from the original split object, and earns no reconstructed-source credit. The four following pad callbacks also remain original. Issue #315 is partial.

The source uses descriptive `GameMemory` namespace and function names because no original namespace or filename spelling was recovered. The canonical sixteen-byte `MEMAllocator` and eight-byte callback table describe the actual private objects; two private four-byte heap handles occupy the observed SDA slots. The complete public interface is in `include/game/Memory.h`. Existing `__sys_alloc` and `__sys_free` spellings are retained.

### Actual object and exception proof

The actual Ninja-produced `build/wpse-v1/src/game/new.o` reproduces every instruction, all relocation targets, all 132 exception bytes, and both exception indices against the frozen native addresses. The standalone canonical-header proof is supplemented by `315-audit-actual-new.py` and `315-audit-actual-memory.py` outside the repository. These scripts audit objects produced by the real build rather than substitute diagnostic objects.

Objdiff reports all four new/delete functions and its text, data, and exception indices at 100%. Its exception-section fuzzy score is 87.878784%, because the native semantic filter symbol `stdBadAllocExceptionName` and the compiler's numeric local label have different names. Explicit relocation pairing resolves both to the same native address and proves all 132 exception bytes. No numeric compiler label was forced into metadata to improve the fuzzy score. This limitation is separate from the complete source-linked DOL comparison, which passes.

The declaration prerequisite is F-315-2, published separately in PR905: standard `bad_alloc` and `new_handler` declarations, plus the native compiler nothrow attributes on malloc/free. This source branch temporarily carries that exact bounded prerequisite until it merges; it introduces no second interface.

### End-to-end validation

The isolated managed source checkout is `C:/Users/brand/.codex/worktrees/315-source/pokemon-rumble-decomp`. After rebasing the original-only split onto `af72e095`, a fresh original-only configure, plain Ninja, and explicit frozen verifier all exited 0. Both units were then registered NonMatching; `ninja all_source`, plain Ninja, the actual object audits, and the frozen verifier exited 0. Finally only the new unit was selected Matching, and plain Ninja and the actual frozen verifier exited 0 again.

Ninja's actual input graph for `main.elf` contains `src/game/new.o` and `obj/game/memory.o`. Thus the accepted operators are source-linked while the incomplete memory unit retains original fallback. Logs outside the repository are `315-managed-original-split.log`, `315-managed-nonmatching-full.log`, and `315-managed-new-matching.log`. The committed-head repeat is recorded in the pull request.

No original hash changed. The four accepted functions earn 320 code bytes; the eleven diagnostic memory matches, linker alignment, exception records, external new-handler storage, and original pad functions earn no additional code credit.

### Traps

- An exception section can have a lower objdiff fuzzy score solely because an equivalent relocation uses a compiler-generated local name. Preserve meaningful metadata and prove the actual target addresses and complete linked artifact rather than renaming symbols to numeric compiler labels.
- A source registration is not source selection. Inspect the real Ninja input graph to distinguish a NonMatching compiled diagnostic from the original object used in the link.


## Post-905 preparation

The source branch rebases onto main after confirmed PR905, dropping its duplicate temporary allocation-header prerequisite and preserving the complete merged header findings. The entry-wise metadata merge retains the newly added BModelObj fallback unit. Review restores the outside new-handler symbol's original eight-byte extent; only its name changes, while the source consumes its demonstrated four-byte function-pointer word. The owned memory/new object lines follow the existing game units in native address order. Earlier source-selected verification is historical and is not a current-main gate. The updated ccfd461c skill requires retaining the incremental build, deleting only its old main.dol before Ninja and checking the rebuilt artifact; it supersedes the earlier clean-output instruction.

### Canonical allocator getter after the current-main rebase

Main independently published the descriptive global `GetDefaultAllocator()` name and declaration for the owned 12-byte getter at `0x801B1360`. The rebase preserves that declaration, its existing `revolution/mem.h` include and its symbol spelling. The owned source now defines the global getter returning the same typed `GameMemory` allocator storage. The earlier unpublished `GameMemory::GetAllocator()` name is removed; there is no alias, wrapper or extra implementation. The other compatible namespace declarations are additive. Actual-object/native verification of this adaptation is pending the queued source gate; previous 11-of-12 memory results remain historical until repeated.
