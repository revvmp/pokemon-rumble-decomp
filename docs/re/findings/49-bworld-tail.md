# F-49-1: BWorld.cpp tail, weak template instances 0x80065870-0x80066454

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Question from issue #49: which source unit owns the 24 functions in `0x80065870`-`0x80066454`, and what are they.

## Result

All 24 functions are the last weak template instances of `game/BWorld.cpp`, confirmed against [F-46-1](46-bworld-unit-boundaries.md): the unit ends at `0x80066454`, and the next object, `BWorldCamera::BWorldCamera(unsigned long)` at `0x80066460`, stores the `BWorldCamera` vtable. The seven `shared_ptr_deleter` vtables in `.data` list `0x80065E80`-`0x80066000` as their destructor slots, so these small functions belong to the same file as the `get_deleter` and `dispose` bodies at `0x80064840`-`0x80064DA0`. No split, object line or header change is needed; #43 owns them.

The compiled `src/game/BWorld.cpp` already emits every one of these instances except two, so the work was identifying them from the compiled object and naming them in `config/wpse-v1/symbols.txt`. Identification compared instruction streams of the split target object with the compiled object, then used vtable relocations in `.data` and cleanup-action relocations in `extab` to separate same-shaped functions.

| Address | Identity | State (`tools/re/unitdiff.py`) |
| --- | --- | --- |
| `0x80065870` | `shared_ptr<BWorldObj>` destructor | 100% (named earlier) |
| `0x80065900` | `Metrowerks::move_ptr<CEffect, default_delete<CEffect>>` destructor | 100% |
| `0x80065980` | `shared_ptr_holder<AnmScnRes, void (*)(G3dObj*)>` destructor | not emitted (needs the #45 loader) |
| `0x800659F0` | `move_ptr<CKDTree<BModelObj>, default_delete>` destructor | 100% |
| `0x80065AA0` | `shared_ptr_holder<ScnRoot, void (*)(G3dObj*)>` destructor | 100% |
| `0x80065B10` | `shared_ptr_holder<ScnGroup, void (*)(G3dObj*)>` destructor | 100% |
| `0x80065B80` | `move_ptr<BWorld::EffectData, default_delete>` destructor | differs in register allocation of the inlined `vector<shared_ptr<CEffect>>` destructor |
| `0x80065C80` | `move_ptr<BWorld::OutlineData, default_delete>` destructor | 100% |
| `0x80065D00` | `allocator<shared_ptr<BWorldObj>>::destroy` | 100% (named earlier) |
| `0x80065D60` | `alloc_ptr` destructor, tree node of `tree_imp<shared_ptr<BWorldObj>>` | 100%; which of the three `alloc_ptr` instances this is rests on neighbouring order only |
| `0x80065DC0`, `0x80065E20` | both `find_leaf` instances | 100% (named earlier) |
| `0x80065E80`-`0x80066000` | seven `shared_ptr_deleter` destructors: `OutlineData`, `EffectData`, `ScnGroup`, `ScnRoot`, `CKDTree<BModelObj>`, `AnmScnRes`, `CEffect` | 100% except `AnmScnRes`, which is not emitted |
| `0x80066040` | `__vec_split_buffer<void*, allocator<void*>>` destructor | 100% in the source; left as `dtor_80066040` (see Traps) |
| `0x800660C0` | `__vec_split_buffer<shared_ptr<CEffect>>` destructor | register allocation of the inlined base destructor differs |
| `0x800661E0` | `__vec_split_buffer<weak_ptr<BWorldCamera>>` destructor | same difference |
| `0x800662E0` | `__vec_deleter<weak_ptr<BWorldCamera>, allocator&>` destructor | 100% |
| `0x80066390` | `__vec_deleter<shared_ptr<CEffect>, allocator&>` destructor | 100% |

The deleter destructors appear in the opposite order to the deleter vtables in `.data` (`CEffect` first in data, last in text).

## Remaining differences

- `0x800660C0`, `0x800661E0`, `0x80065B80`: after the split buffer's own loop, the inlined `__vec_deleter` destructor keeps the data pointer in `r31` and the element counter in `r30` in the original; the rebuild swaps them. Changing `clear()` to pass `size_` directly, reordering the statements of `erase_at_end`, `destroy(--p)` and a `while` form of the split buffer destructor did not fix them; the `clear()` change also lowered `CDividedGround`, `CPpuEngine` and `Message` and was dropped.
- The source emits a second variant of every vec_deleter that the original has once: `__vec_deleter<weak_ptr<BWorldCamera>, allocator>` and `<shared_ptr<CEffect>, allocator>` (by value) and `<void*, allocator&>`. The original contains only the reference variants for the first two and only the by-value variant for `void*`. Nothing in this range depends on them, but the whole unit cannot be `Matching` until they stop being emitted.
- The source also emits `shared_ptr<char>`, `shared_ptr_holder<char, default_delete<char[]>>` and `shared_ptr_deleter<char, ...>` instances that the original lacks.
- `0x80065980` and `0x80065FC0` need `shared_ptr<AnmScnRes>`, which only the scene-animation loader at `0x8005EB70` (#45, unclaimed) instantiates.
- Weak template instances in the compiled object are emitted at other offsets than in the original (the holders near `0x80000B90`, the vec_deleters near `0x0000AE80`); the order is fixed by deferred inlining and instantiation order for the whole unit.

## Traps (F-49-1)

- **Renaming a weak function to a name that another unit also defines changes `main.dol`.** With `scope:weak` on all renamed lines the DOL failed at header byte `0x1F`, because the linker merges same-named weak functions across units. Without the attribute only `__vec_split_buffer<void*>` collided (`multiply-defined` against CSaveManager.o). The symbol at `0x8010BF40` carries the same mangled name but is `0x108` bytes, against `0x74` here, so one of the two names is a wrong imported library name (D-0014); `0x80066040` therefore keeps its placeholder until the other is resolved.
- **Same-sized destructors cannot be told apart by code.** `alloc_ptr` and `weak_ptr` destructors, and the `shared_ptr_deleter` destructors, have identical instruction streams. Use `.data` vtable relocations and `extab` cleanup relocations (`objdump -r` on the split object) to identify them.
- **Changing `include/vector` moves other units.** See [F-47-1](47-bworld-draw.md); compare `report.json` of every unit before keeping such a change.
