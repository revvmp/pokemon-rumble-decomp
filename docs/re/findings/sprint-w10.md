# Sprint w10: BWorld and CEffect

BWorld rose from 183 to 187 of 196 functions in this shift's first pass and to 188 after LoadLightScene; CEffect from 55 to 56 of 58. Neither unit can flip yet (BWorld's `find_leaf` order is parked; CEffect still has `InitializeEffect` and `AllocEffectGroupID`).

## Fixes

| Function | Change |
| --- | --- |
| CEffect `CreateEmitter` | The "sub" branch halves a copy of the height (`f32 halfHeight = height; halfHeight *= 0.5f;`), giving `fmuls fD, fHeight, fHalf` with the result in the constant's register. |
| BWorld `SetCamera` | The shadow camera is declared in the `if` condition, so it is destroyed before `UpdateIndex(-1)`. |
| BWorld `RemoveOldestEffect`, `~map<u32, CEffect*>` | `std::map<unsigned long, T*>` stores its values in the `pair<const unsigned long, unsigned long>` tree held through a `map_base` (see below). |
| BWorld `FindObjectsInSector` | Normal set as x, z, y; `startPlane.mNormal.Dot(center)`; explicit stop mask (r1 rule 17); halved range through a compound assignment; plane distance takes the sphere and is built from three separate assignments; result added with `objs.insert(objs.end(), pObj)`. 95.06% to 99.86%. |
| BWorld `FindObjectsInRect` | Matrix built in an inline helper that receives `-center` by `const&`; the four expanded bounds are named locals declared `maxZ`, `minZ`, `maxX`, `minX` order; the position is transformed through `pEntry->mObj->mPosition`. 100%. |
| BWorld `LoadLightScene` | The scene is applied in an inline helper taking `ResAnmScn` by value and the `shared_ptr<ScnRoot>` by `const&`; the caller names the scene first. 100%. |
| BWorld `DrawShadow` | The zero clear colour comes from an inline `GetClearColor()`; all slots now match (98.35%, one `mr r5` schedule left). |

## Traps

- **Misnamed weak template copies hide a header difference.** BWorld's `insert_node_at` and `destroy` at 0x80063DE0 were named for `map<Ul,Ul>` and CWaza (Matching) calls BWorld's copy. That is evidence that `map<u32, CEffect*>` used the `unsigned long` tree, not a naming accident. The map dtor's inlined null checks (four `beq` after `addic.`) gave the wrapper depth: the specialization holds a `map_base<unsigned long, unsigned long>` and inserts through `tree_.insert_one` directly (a `friend class std::map` in `map_base`), so exactly one `pair<iterator, bool>` result is materialized. Holding the tree directly drops one null check; going through `map_base::insert` adds a pair copy.
- **Extra or misplaced stack objects often mean an inline helper with by-value parameters.** LoadLightScene's two dead `ResAnmScn` copies and its scene slot at the bottom of the bucket, FindObjectsInRect's matrix and `sin`/`cos` slots, and DrawShadow's clear-colour temporaries below `SetupFlatColor`'s objects were all fixed by moving code into an inline helper (t1 rule 3), not by reordering declarations.
- **A `const&` bound to a temporary pointer keeps the pointer in memory only while its address is still in use.** FindObjectsInRect reloads `pObj` from its slot after `PSMTXMultVec` only when the transform reads `pEntry->mObj->mPosition` instead of `pObj->mPosition`.
- **Name compiler-generated data with the build's `@` numbers.** RemoveOldestEffect, both `insert_node_at` and CEffect's `get_deleter` were 99.x% only because of `lbl_` data names. The `@` numbers change when the unit's source changes, so recheck them before a flip.

## Open

- BWorld `FindObjectsInSector`/`FindObjectsInRing`: radius in f1 instead of f0 (`f32 radius = pObj->GetSize().z`). Computing `innerRadius - radius` before the first test fixes the colouring but schedules the subtraction early. Ring's `distSq` also ranks below the two radius parameters (target f31 above them).
- BWorld `AddObject`, `RemoveObject`: `pEntries` ranks below `this`/`obj` (AddObject); first computed neighbour takes r5 (RemoveObject). Inline helpers for linking did not move them.
- BWorld `Calc`/`CalcMtx` skip mask (`li r0,0; ori r30,r0,2`): a sandbox (`GetSkipStates` with `|=`, `+=`, `^=`, ternaries, bit structs, reference helpers, narrower types) never reproduced the rematerialized zero; `+=` folds to `li 2`.
- BWorld `vector<ObjEntry>(n)`: an inline `init(n)` level fixes the 16-byte slots but reverses the byte flags (t1 open case, header-wide).
- BWorld `DrawShadow`: `mr r5, r25` is scheduled before `addi r3` in the `clearColor` calls only.
- CEffect `InitializeEffect`: the target colours `numEffects` and the later pool pointers into r23 (ours r24/r27); declaration order has no effect, and a template `CreatePool<T>` helper reverses the per-block EH slots.
- CEffect `AllocEffectGroupID`: unchanged (r1 open case); a `const bool&` constructor makes the `false` an `.sdata` constant, and by-value forms never keep the store.
