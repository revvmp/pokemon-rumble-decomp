# Sprint e2: the deferred emission machine

Measured in sandbox objects with the game flags (`Wii/1.1`, `-O4,p -inline auto -ipa file`, plus `-inline off` where noted) and checked against the target order of CTutorialMsg (an `-inline off` unit, so every call is visible), CPauseMenu, CDlgCollection, CGameSession and CPiiCollectionBox. Sandbox sources and the `sb.sh` runner are in the session scratchpad (`e2/`).

## Model

With `-ipa file` the whole file is parsed first, then code is generated in this order:

1. **Strong functions in source order.** After each one, the inline functions it calls that have not been generated yet are generated depth first, in call order (pre-order). This is the "during parsing" phase of sprint-e1 rule 1.
2. **Vtable-only inline functions** (sprint-e1 rule 6).
3. **Rounds**, repeated until nothing is pending. Each round has four stages:
   1. **Class-template walk.** Class templates are visited in reverse order of their first declaration (rule 4a); within a template, newest specialization first; within a specialization, members in declaration order (an overload set at its first declaration). A member is generated if it is pending. A request made during the walk for a member that the walk has not passed yet (a later member of the same class, or any member of a later group) is generated in this round. A request for a member the walk has already passed waits: an **out-of-class** member for the next round's walk, an **in-class** member for this round's drain (stage 3).
   2. **Function-template loop.** Pending non-member function templates (`std::move`, `std::swap`, `std::max`) are generated in their own walk (reverse first declaration, newest specialization first), repeated until none are pending. `move<T>`, `swap<T*>`, `move<T*>` in that order means the move group is walked before the swap group and `move<T*>` was requested by `swap`.
   3. **Drain.** In-class members that were requested behind the walk are generated in reverse order of the functions that requested them, each requester's list in call order, depth first. In CTutorialMsg the drain after round 1 is `grow_by` (requested by `push_back_realloc`, the later requester) and then `__vec_deleter(A)`, `compressed_pair(n, A)`, `CompressedPairStorage(n, A)`, `allocator(const allocator&)`, `allocate(n)`, `begin` (requested by `reallocate`, in call order, with the constructor chain depth first).
   4. **Implicit stage.** Implicit destructors (`~CTutorialMsg`, `~shared_ptr_deleter<T>`, `~CPauseMenu`) are generated here, after the first round's drain. A sandbox class with an implicit virtual destructor lands between round 1 and round 2.
4. **Requests from template instantiation chains wait one round.** A destructor that is only referenced from an exception table (a local `__vec_split_buffer` in an inlined reallocation) is generated in round 1 if the inline body that holds the local is reached directly from a strong function, but in round 2 if it is reached through an out-of-class template member (`push_back_realloc`). The target puts every such `~__vec_split_buffer` right after the implicit stage (CPauseMenu, CDlgCollection, CGameSession), so in the original these locals sit behind an out-of-class template member.

The walk and the drain explain the "chains" in the target (`max_size`, `alloc() const`, `second() const`, `allocator::max_size`) as depth-first drain items, and the "interleaving" of `__vec_deleter` and `__vec_constructor` members as one walk group.

## Consequences for the vector header

- **The `__vec_constructor` / `__vec_deleter` split of the algorithms is invented.** The reconstructed names come from no symbol map (F-245, `245-vector-growth.md`). In CTutorialMsg's round 1 the target walks `~__vec_deleter`, `reallocate`, `erase_tail`, `push_back_realloc`, `move_construct_to_end`, `clear_moved` as one group. Two class templates can never interleave in a walk, so these members belong to one class, as in the archived 2006 MSL header where `__vec_deleter` owns the growth algorithms. Fixing the vector order needs that ownership change and a matching rename in `symbols.txt`.
- **The CTutorialMsg target order fixes the `__vec_deleter` declaration order:** in-class `__vec_deleter(A)`, `begin`, `end`, `cap`, `alloc`, `max_size`, `allocate(n)`, `grow_by` before `reallocate` (they are drained); out-of-class `~__vec_deleter` before `clear`, then `allocate(n, tag)`, `erase_at_end`, `erase_tail`; `reallocate` before `erase_tail`; `recommend` and `append_realloc` out of class, declared before `push_back_realloc(x, tag)`; `move_construct_to_end` and `clear_moved` after `reallocate`. `allocate(min, max)` belongs to `__vec_deleter<T, A&>`, not to the split buffer (it is drained, while the split buffer's own members are walked).

## Landed

- `vector::push_back` always calls the base `push_back`; the in-class `push_back_pod` copy is gone. With it the POD reallocation reached the split buffer directly from the strong caller, so `~__vec_split_buffer<UIMenuItem>` was generated in round 1. Through the base path it waits for round 2 as in the target: CPauseMenu's `.text` order is now exact, and `push_back<UIMenuItem>` (94% to 100%) and one PiiPropData function reached 100% (13,064 to 13,066, no losses, DOL verifies). CPauseMenu still has code differences in `Run` (one stack offset), `LeaveStage` and `TakeScreenshot` (register choice).

## Method

- Order of a unit: `python scratchpad/e1/var.py game/<Unit> --obj build/wpse-v1/src/game/<Unit>.o .text` (or without `--obj` to compile; sources that need Shift-JIS must use the ninja object).
- Stage boundaries in a sandbox: add a class template whose out-of-class member is requested from a strong function (round 1 marker) and which requests an earlier-declared member (round 2 marker), and make the members recursive so they are not inlined.
- Instantiation point: `typedef typename T::probe_x probe;` in the body; the message names the point of instantiation and the chain of templates being instantiated.
