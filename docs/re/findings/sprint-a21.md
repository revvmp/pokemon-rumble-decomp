# Sprint a21: closing near-matching units

## Landed

- `game/memory` is Matching. `AllocateCallback` needs a third `throw()` exception region: a file-local `inline void* AllocateAligned(u32 size) throw()` that forwards to `Allocate(size, 32)`, called from the callback, reproduces the target frame (0x60) and the three `__unexpected` handlers. A default argument does not create the region; a separate one-argument inline function does.

## Traps

- Inlined `throw()` functions each add a 0x18-byte exception record and a stack slot. A frame that is 0x20 larger in the target with one more `__unexpected` call means one more nested inline `throw()` wrapper in the original.
- A flipped unit can pass objdiff for every function and still fail the DOL. CGloballyReusableTexBuf: the target emits `__dt__...__vec_split_buffer<WorkBuffer,...>` before `__dt__...__vec_deleter<WorkBuffer,R...>`; the compiled object emits the deleter first. Moving the `__vec_deleter` destructor definition below the split-buffer one in `include/vector` does not change it. Not solved.
- CDisconMsg: the DOL differs in `.data` layout (the target pools strings and places `lbl_8039B36C..8039B394` between the shared_ptr_deleter vtable and the CDisconMsg vtable), so relocations differ from the first function onward. Not solved.

## Open near-misses

- RenderUtilities `NormalizeSignedAngle` 99.56%: the else-branch result is in f2 in the target and f0 here; `value + 360.0f` versus `360.0f + value` and `+=` locals do not move it.
- RenderPrimitive `SetupScreenProjection` 96.33%: target keeps the double conversion constant in f3 and copies the zero argument into f3 last.
- CPiiPersonalData `Import` 95.45% and CUIPiiListItem constructor 87.69%: only load scheduling differs (shared_ptr copy loads are hoisted above the vptr/flags stores here).
- CLayoutManager `Remove`: the target reuses one stack slot (0x8) for the iterator temporary where erase uses two (0x8 and 0xc).
- CMoneyLifeDisplay `Create`: the target loads `pMapDesc` before the vptr store of the inlined constructor.
