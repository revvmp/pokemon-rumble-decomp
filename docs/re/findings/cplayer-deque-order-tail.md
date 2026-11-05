# CPlayerPii deque helper emission tail

## Scope and evidence

This investigation continued the CPlayerPii source-link work in [the unit findings](184-cplayerpii.md). The initial late-helper packet was `pf_player_order_externaliter1`, with the original object preserved at `tools/local/tail-20261104/pf_originals/game_CPlayerPii.o`. The iterator class identities in that packet are inferred names. Every comparison uses its recorded old-to-new symbol mapping; names alone are not evidence of equivalence.

The sort-tail lane owned the const-copy helpers at target indices 211?222 and the allocator-reference buffer helpers at indices 236?252. The particle/font lane owned the earlier generation passes and the final combined packet. All experiments used direct compiler outputs and private include overlays. No original object, instruction stream, or relocation was rewritten.

## Const-copy generation

The const-copy group initially emitted the iterator subtraction, addition, and copy constructor before the copy routines. Moving their bodies outside the iterator class could move the group, but displaced it to a later generation pass. Moving only the pointer-copy definition also failed.

Two named, unused function-pointer locals request the existing overloads before the iterator operations:

- At the start of the generic tagged `__cdeque_buf::construct_at_end`, a `T* (*)(const_iterator, const_iterator, T*)` local is initialized with `&copy`.
- At the start of the const-iterator `copy`, a `T* (*)(const T*, const T*, T*)` local is initialized with `&copy`.

The compiler eliminates both locals from executable instructions but retains their template generation requests. `sortplayer_request_31` reproduces the required twelve-function order and all 254 raw function bodies. A discarded cast to a function pointer had no effect in this experiment. Calling through the local pointer added instructions and was rejected. These unused locals are D-0016 source controls, not recovered original source.

## Allocator-reference buffer tail

The combined earlier work left this tail order, using target indices:

`236, 239, 237, 241, 240, 242, 243, 244, 245, 246, 247, 248, 238, 251, 252, 249, 250`.

Indices 236?241 identify the allocator-reference buffer destructor, tagged allocation, maximum size, clear, tagged construction at the front, and tagged construction at the end. Indices 249?252 identify the two pop-back helpers, the const allocator accessor, and its compressed-pair accessor.

A named, unused member-function pointer at the beginning of the three-argument allocation forwarding overload requests `max_size`:

```cpp
typedef size_type (__cdeque_buf::*MaximumProc)() const;
MaximumProc pMaximum = &__cdeque_buf::max_size;
```

This moves `max_size` into the preceding generation pass and puts the const allocator accessors after the pop-back helpers. Placing the same request in the generic destructor also moved the ordinary allocator buffer helpers and regressed the earlier order. The three-argument forwarding overload supplies the narrow request site. The control is recorded as D-0016; it introduces no executable instructions.

With the particle/font lane's complete earlier group, `sortplayer_late4` has all 254 raw bodies exact and only this ordering difference:

`236, 238, 239, 237, 241, 240`, followed by every function from 242 through 253 in the required order.

The remaining allocation displacement is an ordinary declaration-order correction: move all four `allocate` overload declarations together, immediately after the buffer destructor declaration. Moving only the tagged overload, moving out-of-class definitions, and inserting further pointer requests did not fix it. The first declaration of the overloaded name controls this ordering. The `sortplayer_reffamily_0` packet has the allocation group in exact order and only the tagged front/end pair reversed.

The particle/font lane independently confirmed that moving the entire `construct_at_front` declaration family before the entire `construct_at_end` family fixes the final pair. Moving only the tagged front declaration had failed because the first front declaration still followed the first end declaration.

## Validation

`pf_player_semantic_audit.py sortplayer_reffamily_0` reports:

- 254 required functions.
- 254 exact raw function bodies and normalized relocations, including relocation-backed literal values.
- 254 exact per-function exception records.
- No function or exception differences.

The original-object SHA-256 is `f1d6cd710f4ef11db0d560df4ba09dcd51de2c25ac9357b72075509898e70714`. The candidate-object SHA-256 is `e5c71088818d9ac66034999f0982533fdc1b181f3b74f2393c8e88622b17699a`. These whole-object hashes differ because the packet still reverses the final front/end pair and carries the inferred iterator identities.

The final combined packet `pf_player_order_complete` reports all 254 functions present, prefix 254, and ordering distance zero. Its raw-body and semantic audits report 254 exact function bodies, normalized relocation/literal sets, and per-function exception records, with no differences. Its object SHA-256 is `ec6c0360cbb0edb62c8318acb98205ce9ae44b80d33cc32ca310f0e3524c16f1`.

The particle/font lane owns the global identity audit, source/header handoff, and final source-link evidence. The coordinator subsequently accepted the native full source link with the compatible CSaveManager caller, as recorded below.

## Full native source-link acceptance

The coordinator's first complete Player trial, `links/player-natural-order`, linked but produced a 4,097,408-byte executable, 2,528 bytes too large. Its final ELF retained 25 helpers under both the old nested-iterator names and the new external-iterator names. All 25 old-name copies came from `unit_0066.o`, the selected CSaveManager source object.

Those copies contain 2,104 instruction bytes. With their observed 16-byte function alignment they occupy 2,288 bytes, exactly the increase in the linked `.text` section. Their ten retained exception records add 80 bytes to `extab` and 120 bytes to `extabindex`; section alignment accounts for the remaining file-size effect. The copied old definitions were the cause, not a missing Player function or bad reconstructed instruction stream. The private `sortplayer_duplicate_owners.json` records every duplicate, address, size, selected owner, and exception record.

The particle/font lane independently compiled the deque include owners with compatible global iterator and common-helper identities. Its CSaveManager packet preserves all 469 function bodies, normalized relocations, and exception records under the documented name mapping. The coordinator substituted this source object together with Player in `links/player-compatible-save`.

That native link passed the frozen executable manifest on 2026-11-05 at 02:53:26 UTC: all 4,094,880 bytes are identical, with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The evidence is `tools/local/tail-20261104/links/player-compatible-save/verification.json`. Production header/configuration integration remains coordinator-owned.

## Traps

- Moving one overload can leave emission unchanged. In this compiler, the first declaration of the overloaded name controls the demonstrated buffer-helper family order. Move and inspect the entire overload family; do not infer that declaration order is ineffective from a single tagged-overload move.
- A lower objdiff percentage can reflect changed relocation identities or positions while all raw instruction bytes remain exact. The inline-allocation experiments retained all 254 raw bodies despite lower fuzzy scores. Use normalized relocation and exception comparisons before attributing such changes to instruction generation.
- Generation requests have different scopes. A generic destructor request moved helpers for both allocator-value and allocator-reference buffers. The allocation forwarding overload supplied a narrow site without changing earlier helpers.
- Completing a delayed friend from a destructor can bind its body too late to emit it. The `sortplayer_refdelayed_00` and `_01` experiments omitted the tagged allocation body and were rejected; a lower ordering distance with a missing symbol is not progress.
