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

# F-39-2: Model setup, outline detection and root scale

`SetModel` (`0x80053840`, vtable slot 0x54), `IsOutlineModel` (`0x800536B0`) and `SetModelScaleDirect` (`0x80055980`, slot 0x70) match, with the weak `shared_ptr_deleter_common` destructor (`0x80055BB0`) emitted after the first function that constructs a deleter. The `shared_ptr_deleter<AnmObjChrRootScale, void (*)(G3dObj*)>` members (`0x8005A0E0`, `0x8005A130`, `0x8005A8D0`) and the construction guard destructor (`0x8005A6A0`) match at the end of the unit.

- `SetModel` takes a flag word: bits 0–1 select the model type (1 shadow, 2 outline), bit 2 sizes the model with its model scale. Outline is only set when `IsOutlineModel` accepts the model. A model with draw priority 0 gets translucent priority 50.
- `IsOutlineModel` reads the user data of root node 0. An integer entry `OUTLINED` (`.data` `0x80399440`) equal to 0 rejects the model; otherwise the model needs a `ScnMdl` with at least one material and a valid copied material accessor. The user-data entry layout used is size at 0, data offset at 4, type at 0xC (0 integer). The field at 8 is unnamed in evidence and is recorded as `numData` with low confidence.
- `SetModelScaleDirect` keeps the existing `AnmObjChrRootScale` when the model has one. Otherwise it constructs one from the default allocator (`0x801B1360`, returns `.data` `0x803BF3A8`) and owns it through `shared_ptr` with the deleter function at `0x801C7540`, a tail call to `G3dObj::Destroy`.

## Code-generation rules from F-39-2

- `shared_ptr(Y*, D)` in this MSL protects the pointer with a guard holding the pointer and a reference to the deleter. The guard is cleared after the control block is allocated, which leaves a dead `bctrl` after the branch.
- The control block stores a function-pointer deleter after the pointer. Its constructor takes the deleter by reference and its storage takes it by value; that places the deleter load after the vtable store and before the pointer store.
- `reset(p, d)` written as `shared_ptr(p, d).swap(*this)` reproduces the release of the previous control block without a retain.
- A scale test against 1.0 must read the member just stored (`mModelScale.x`), which produces the `frsp` before each compare. Testing the parameter does not.
- `IsOutlineModel` is not inlined into `SetModel` only when the copied material accessor is a named local.

## Traps from F-39-2

- **A named local can stop inlining.** Symptom: a static helper is inlined into its caller and the caller grows by the helper's size. Returning `CopiedMatAccess(...).IsValid()` directly let the helper inline; a named local kept it out of line and matched both functions.
- **Deleter parameter passing moves one load.** Symptom: the only difference in a control-block construction is where the deleter is loaded. Take it by reference in the control block and by value in the storage.