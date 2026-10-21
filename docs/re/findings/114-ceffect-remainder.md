# Issue 114: CEffect.cpp from CalcMtx to the static initializer

## F-114-1: scope

The block `0x800A1D40`-`0x800A3B08` is the tail of `game/CEffect.cpp`, whose split is owned by [#113](113-cdroppii-ceffect.md). It holds `CEffect::CalcMtx` (`0x800A1D40`, 0xD54 bytes), the `nw4r::ef::MemoryManager` support code (`FreeHeap`, the four pool `Alloc`, `Free` and count accessors, `UpdateObjectID`, `GarbageCollection`, the destructor), `EmitterWithParam::Initialize` and `CreateEmitter`, the `vector<bool>` and `shared_ptr` deleter instantiations used by the effect group IDs, and `__sinit_CEffect_cpp`. All of it is reconstructed in `src/game/CEffect.cpp`; the unit stays `NonMatching`.

Names added to `symbols.txt` from the reconstruction: `CEffect::CalcMtx`, the `MemoryManager` members and `MemoryManagerTmp<T>::UpdateObjectID` instantiations, `EmitterWithParam::CreateEmitter` and `Initialize`, the `__bitvec_deleter<allocator<bool>>` destructor, the `shared_ptr_deleter<vector<bool>, default_delete<vector<bool>>>` members, `sExtendEmissionOffsets` (`0x80352B18`, ten `VEC3` emission offsets) and the two statics `sUsedEffectGroupID` and `sNumUsedEffectGroupID`.

## F-114-2: MSL vector<bool>

`include/MSL_CPP/bitvector.h` adds `vector<bool>` (included from `<vector>`). Evidence from the target:

- The deleter symbol is `__bitvec_deleter<allocator<bool>>`; it owns the word array, the bit count and a `compressed_pair` of the word capacity and the allocator.
- Inlined `delete` of a `vector<bool>` in `shared_ptr_deleter::dispose` has three nested null checks. Only a class chain `vector<bool>` over `__bitvec_base` over `__bitvec_deleter`, each with an explicit destructor, reproduces them: `dispose` and `Metrowerks::move_ptr<vector<bool>>::~move_ptr` went from 95-97% to 100%.
- The constructor fills with `__bit_fill_n`, using `cntlzw` and masks over 32-bit words. This yields the target fill loop in `AllocEffectGroupID`.

Remaining difference in `AllocEffectGroupID` (98.37%): the target keeps the constructor's `false` argument in a callee-saved register and stores it to a stack byte at `8(r31)`, which moves every later stack offset by four. Passing it as `bool`, `const bool&`, `bool&`, a local copy, `bool()` (which creates a static byte in `.sdata`) or with an extra allocator argument did not reproduce that, and the last two made the function worse.

## F-114-3: literal pool order follows source order, not link order

The four rotation `Create` overloads of #113 differed only in `.sdata2` relocation names. The literal pool is filled in source definition order, while `.text` follows declaration order in the class. The original defines `Create(BWorld*, const Param&, BWorldObj*)` (which loads `0.0f` and `360.0f` for `RandReal`) before the position-and-rotation overload that first uses the sine table constant. Moving that definition up in the source, without touching the class, made all five `Create` functions 100% and `.sdata2` 96%.

## F-114-4: current state

objdiff on `main/game/CEffect`: 50 of 58 target functions at 100%. Not yet 100%:

| Function | Match | Cause |
| --- | --- | --- |
| `InitializeEffect` | 99.62% | pool-loop register allocation (see F-113-3) |
| `AllocEffectGroupID` | 98.37% | constructor argument stack byte (F-114-2) |
| `FreeEffectGroupID` | 98.00% | the target branches `blt` to the body and `b` to the end where the source gives a single `bge`; nested, combined and early-return forms all compile to `bge` or worse |
| `CalcMtx` | 98.32% | callee-saved register numbers of `pFollow`, `pModelObj`, `pScnMdl` and `nodeID` (declaration order permutations did not change it) and the order of the emission-offset table loop after `0x0C2C` |
| `FreeHeap` | 95.97% | target holds the previous and next physical blocks in `r5`/`r6` with temporaries `r6`/`r7`; the form with and without temporaries, hoisted declarations and both orders all give `r7`/`r5` |
| `GarbageCollection` | 99.38% | the pool pointer takes the highest register in the generic `CollectClosing` helper, the target the lowest; declaring the particle and its successor in order fixed the first loop |
| `CreateEmitter` | about 96% | stack slots of the scale copy and the zero-vector temporaries (target: scale then temporary, per case, follow case first) |
| `get_deleter` | 99.74% | only the name of the relocation for the `default_delete<vector<bool>>` type info |

The extab and extabindex sections of the object are larger than the target's because the object contains weak instantiations (`ICalc`, `weak_ptr<BWorldObj>`, `exception`) that the link discards; compare with `CDropPii.cpp`.

## Traps

- **`strncmp(...) == 0` and `!strncmp(...)` give `cmpwi`; the original `cntlzw`/`srwi.` came from a small helper returning `bool`.** Wrapping the comparison in an inline `bool` function reproduced it.
- **The literal pool is in definition order.** If constant relocations are right but ordered differently, reorder the definitions in the source; `.text` order still follows the class declaration. See F-114-3.
- **Count the null checks in an inlined `delete`.** The number of `beq` after one `cmpwi` shows how many destructor levels were inlined; adding a class level reproduced three checks.
