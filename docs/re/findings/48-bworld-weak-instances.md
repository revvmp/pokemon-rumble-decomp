# F-48-1: BWorld deleter, weak_ptr and shared_ptr instances in 0x80064BB0-0x80065864

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Question from issue #48: what are the 25 functions in `0x80064BB0`-`0x80065864`, and do they belong to `game/BWorld.cpp`.

## Result

All 25 functions lie inside `game/BWorld.cpp` (F-46-1: `.text` `0x8005AAA0`-`0x80066454`). The file does not end in this range and no other file begins in it: `BWorldCamera.cpp` still starts at `0x80066460`. The block is the weak template instances the compiler emits after the unit's own functions, in the order `get_deleter`/`dispose` pairs, `weak_ptr` destructors and `lock`, then `shared_ptr` destructors. The source of `src/game/BWorld.cpp` already emitted every one of them; they were reported as missing only because the target symbols were still `fn_`/`dtor_` placeholders. Renaming them in `config/wpse-v1/symbols.txt` pairs them with the compiled functions, and no source change was needed.

| Address | Identity | State (`tools/re/unitdiff.py`) |
| --- | --- | --- |
| `0x80064BB0`, `0x80064C00` | `shared_ptr_deleter<ScnGroup, void (*)(G3dObj*)>` `get_deleter`, `dispose` | 100% |
| `0x80064C20`, `0x80064C70` | `shared_ptr_deleter<BWorld::EffectData, default_delete<EffectData>>` `get_deleter`, `dispose` | 100%, 98.96% |
| `0x80064D50`, `0x80064DA0` | `shared_ptr_deleter<BWorld::OutlineData, default_delete<OutlineData>>` `get_deleter`, `dispose` | 100% |
| `0x80064DF0` | `weak_ptr<CEffect>::~weak_ptr` | 100% |
| `0x80064E50` | `weak_ptr<CShadowCamera>::~weak_ptr` | 100% |
| `0x80064F80` | `weak_ptr<BWorldCamera>::~weak_ptr` | 100% |
| `0x800650B0` | `weak_ptr<BWorld>::~weak_ptr` | 100% |
| `0x80065240` | `shared_ptr<AnmScnRes>::~shared_ptr` | not emitted: needs the scene-animation loader `0x8005EB70` (#45) |
| `0x80065360` | `shared_ptr<CEffect>::~shared_ptr` | 100% |
| `0x800653F0` | `shared_ptr<BWorld::OutlineData>::~shared_ptr` | 100% |
| `0x80065480` | `shared_ptr<BWorld::EffectData>::~shared_ptr` | 100% |
| `0x80065510` | `shared_ptr<CKDTree<BModelObj>>::~shared_ptr` | 100% |
| `0x800655A0` | `shared_ptr<CShadowCamera>::~shared_ptr` | 100% |
| `0x80065630`, `0x800656C0`, `0x800657E0` | `shared_ptr<BWorldCamera>`, `shared_ptr<ScnGroup>`, `shared_ptr<unsigned char>` destructors (named before, already 100%) | 100% |
| `0x80065750` | `shared_ptr<ScnRoot>::~shared_ptr` | 100% |

`0x80064EB0`, `0x80064FE0`, `0x80065110` and `0x80065170` (the `lock` instances and `weak_ptr<BWorldObj>` destructor) were named in F-47-1 and are 100%.

## F-48-2: how the instances were identified

The destructors are identical machine code, so the types come from where the original object references them. In `__ct__6BWorldFRCQ26BWorld4Desc` the exception cleanup chain lists the members' destructors latest constructed first: `0x800653F0`, `0x80065480`, `0x80065510` are the actions for `mOutlineData`, `mEffectData`, `mModelTree`, `0x80064E50` and `0x80064F80` those for `mShadowCamera` and `mCurrentCamera`, and `0x80065750` the one for `mScnRoot`. `CreateEffect` references `0x80065360` and the `DynamicCast<CShadowCamera, BWorldCamera>` helpers `0x800655A0`. The weak destructors with no cleanup entry in this unit, `0x80064DF0` and `0x800650B0`, are `CEffect` and `BWorld` by emission order: the compiled object lists the weak destructors as `CShadowCamera`, `BWorldCamera`, `CEffect`, `BWorld`, `BWorldObj`, and the target has `0x80064DF0` before the `CShadowCamera` pair while `0x800650B0` follows `BWorldCamera`. This assignment is inferred, not proven, for these two (medium confidence); they are byte-identical to each other and to the other `weak_ptr` destructors, so the choice affects only the name.

## F-48-3: remaining differences

- `dispose` of the `EffectData` deleter (`0x80064C70`): the code is the same, but the original keeps the element count of the inlined `vector<shared_ptr<CEffect>>` destructor loop in `r28` and the end pointer in `r29`, and the rebuild swaps them (12 rows). The standalone vector destructor is 100%, so the difference depends on the inlined context.
- `0x80065240` and the `AnmScnRes` deleters `0x80064A00`, `0x80064A50` and holder `0x80065980`: need the loader at `0x8005EB70` (#45).
- The emission order of the whole tail (the compiled object lists the `weak_ptr` and `shared_ptr` destructors in a different order from the target) is not fixed yet, and decides the unit's final layout.

## Traps

- **Missing is not unwritten.** `unitdiff.py --all` lists a target function as missing when its symbol still has an `fn_` or `dtor_` name, even if the source already emits it. Compare `powerpc-eabi-nm` of `build/wpse-v1/src/game/BWorld.o` with the placeholder list before writing source.
- **Template instances with identical code are named from their callers.** Use the exception cleanup chain of the constructor (latest member first) and the extab references to assign types; the extab words that point at a destructor sit in `.extab` `0x800078BC`-`0x80009DDC`.
