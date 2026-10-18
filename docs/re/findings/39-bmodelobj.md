# F-39-1: BModelObj unit, class layout and first reconstruction

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #39 covers the second block of the BController, BModelObj and BWorld range. This finding records the `game/BModelObj.cpp` split, the class hierarchy behind it, and the code-generation rules needed to match the first 29 functions.

## Unit boundaries

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x800066C8`–`0x800078BC` | Entries referenced only by functions in the `.text` range |
| `extabindex` | `0x8004422C`–`0x80044514` | Index entries pointing into the `.text` range |
| `.text` | `0x800528E0`–`0x8005AA98` | Starts right after BController; ends with the `BModelObj` destructor thunk `@72@__dt__9BModelObjFv` at `0x8005AA90` |
| `.data` | `0x80399440`–`0x803999B8` | `__vt__9BModelObj` at `0x80399640`, KD-tree node vtables and string data; BWorld data starts at `0x803999B8` |
| `.sdata` | `0x804B5A40`–`0x804B5AF0` | RTTI records for BModelObj and its bases |
| `.sdata2` | `0x804B9C88`–`0x804B9CA8` | Float constants used by the unit |

The `.text` end is a hypothesis with medium confidence. The helpers at `0x8005AAA0`–`0x8005AC6C` are called from BWorld and from other units, so they are left for the BWorld split.

## Class hierarchy

RTTI at `0x804B5A68`–`0x804B5AA0` names `BModelObj`, `BSoundSource`, `BKDTreeLink`, `IKDTreeLinkRemoved`, `BWorldObj`, `std::tr1::enable_shared_from_this<BWorldObj>` and `ICalc`. `BModelObj` (0xF8 bytes) derives from `BWorldObj` (0x48 bytes, at 0), `BSoundSource` (0x1C bytes, at 0x48) and `BKDTreeLink` (at 0x64). Constructor and vtable evidence are in [symbols](../SYMBOLS.md#game-bmodelobj-39).

## Code-generation rules

- A `VEC3` returned by value from a non-const member reproduces the original word-copy store order. The const form orders the stores differently, so `GetSize` and `GetPosition` are non-const.
- The `neg`/`or`/`srwi` test of a `shared_ptr` comes from a pointer-to-data-member `unspecified_bool_type` conversion. An `operator bool` gives `cmpwi`.
- In-class inline functions are emitted right after the first function that uses them. Out-of-class non-inline template members, as in MSL, are emitted at the end of the unit. This places `tree_imp::destroy`, `allocator::destroy` and the `shared_ptr` destructors late in the original.
- The original `BModelObj` destructor checks `this` four times around the draw-layer set. That pattern is reproduced only by a `DrawLayerSet` class wrapping `std::set`, which wraps `tree`, which wraps `tree_imp`, each with its own destructor.
- `tree_imp::destroy` checks children at the call sites, destroys the value through `this->Allocator::destroy` and frees with `::operator delete`. A temporary allocator and `delete` destroy the value twice.

## Status

29 of 88 functions match in objdiff; the unit stays `NonMatching`. The full DOL verification passes with the original object. Remaining work is the KD-tree insertion inlined into seven virtuals, the `set` insert path (`find_leaf`, `insert_node_at`, rebalance), `shared_ptr` deleters, exception classes, `SetModel`, `ChangeModel`, `CalcSize`, `CalcMtx`, `SetModelScaleDirect`, `OnEnterWorld`, and the data and RTTI order. `GetMotionRate`, `PlayFrom`, `PlayLoopFrom`, `PlayOnceFrom` and `UpdateOutline` match in scratch but need the `CFileData` and `BWorld` layouts before they can be committed. `UpdateOutline` matches only with its switch ordered `case 0`, `default`, `case 2`.

## Traps

- **Const `VEC3` getters change store order.** Symptom: a by-value `VEC3` return differs only in store order. Make the getter non-const.
- **`operator bool` is the wrong `shared_ptr` test.** Symptom: `cmpwi` where the original has `neg`/`or`/`srwi`. Use a pointer-to-member `unspecified_bool_type`.
- **Template emission order depends on definition placement.** Symptom: MSL template functions sit at the end of the original unit. Define those members out of class and non-inline.
- **Repeated `this` checks reveal destructor layering.** Symptom: a destructor with several null checks of the same pointer. Model each check as a separate wrapper class destructor.
- **The `BModelObj` `.text` split stops before shared helpers.** Ending at `0x8005AC6C` also verifies, but the helpers after `0x8005AAA0` serve BWorld and other units; the split ends at `0x8005AA98`.
