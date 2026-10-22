# Sprint a03: CDividedGround and CEffect

Issues 61-65 (`game/CDividedGround.cpp`) and 113-114 (`game/CEffect.cpp`).

## State

- `CDividedGround.cpp`: every function of the unit is reconstructed. All but `LoadAttr`, the `list::insert<iterator>` instantiation and cosmetic relocation-name differences (`LoadPkc`, `BuildScene`) are exact. The unit stays `NonMatching`.
- `CEffect.cpp`: `FreeHeap` and `GarbageCollection` became exact. `InitializeEffect`, `AllocEffectGroupID`, `FreeEffectGroupID`, `CalcMtx` and `CreateEmitter` remain below 100%.

## Traps

- **Search loops that fall through to a fallback need a `goto`.** `for (...) { if (found) break; } if (it == end) {...}` re-evaluates `!=` through an out-of-line call and adds a stack temporary. The target jumps from the loop body straight past the fallback; `goto found` reproduces `Load` (100%) and `BuildScene`.
- **Empty-class tag temporaries decide stack slots in string code.** `random_access_iterator_tag()` passed by value creates two one-byte slots; their order depends on whether the tag is a named local. A named local in `replace(pos, count, const string&)`, `replace(..., ForwardIterator)` and a separately named `replace_tagged` for `append(const char*)` reproduced `LoadPkc`, `LoadAttr`, `BWorldLightSet::CreateMotion` together. `operator=` and `assign(const string&)` need different bodies: the out-of-line `assign` (symbol `assign__...`) uses `replace(0, n, p, p + size, forward_iterator_tag())`, the inlined `operator=` uses the tagged `replace`.
- **`OnLeaveWorld` takes the world.** The target passes `r4` through to `BModelObj::OnLeaveWorld`; `BWorld` calls it with `this`. All overrides were changed to `OnLeaveWorld(BWorld*)` and their symbols renamed (`...FP6BWorld`).
- **A reference symbol can carry the wrong signature.** `CModel::Load` was imported as `Q36CModel10LoadParams`; the compiler produces `Q26CModel10LoadParams`. The string range constructor needs a third pointer argument (`li r6, 0`) in the caller; modelled as `const Allocator* = 0`.
- **Tiny identical functions cannot be named by fingerprint.** `lwz r3,0(r3); blr` and `blr` exist many times (`get`, `operator*`, `GetModel`). Name them from the call sites of a nearly matching caller instead (`bl` pairs at equal instruction indexes after sequence alignment).
- **`GetModel` has a const and a non-const overload.** `BuildScene` calls the non-const one for `SetPriorityDrawXlu` and `SetMtx` and the const one (reached through a const reference) for `PushBack`.
- **`std::list::insert(pos, first, last)` temporary.** The target has two more zero-byte stack stores before the copy loop than `list temp(first, last)` produces; `list(first, last) : base(first, last, a)` gives one. Not solved.
- **Commutative float multiplication order is not controllable from source.** `x * 0.5f` versus `0.5f * x`, and `a * (b / c)` versus `b / c * a`, compile identically; the `fmuls` operand swap in `CalcMtx` and `CreateEmitter` cannot be changed this way.
- **Compiler version and flags do not matter for the remaining CEffect functions.** Wii/1.0 to 1.5 give identical output; `-O4,s`, `-inline deferred`, `-opt nopeephole` and `-opt noschedule` make it worse.
