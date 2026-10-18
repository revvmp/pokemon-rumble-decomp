# F-45-1: BWorld cameras, object registration and MSL emission details

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #45 covers `0x8005D370`–`0x8005F444` in `game/BWorld.cpp`: the camera table, the ground and object registration, and two outline setters.

## Matched functions

`GetCamera(int)` (`0x8005D370`), `AddCamera(shared_ptr<BWorldCamera>, u32)` (`0x8005D400`), `DynamicCast<CShadowCamera>` (`0x8005DC70`) and its `static_pointer_cast` (`0x8005DD90`), `AddCamera(shared_ptr<BWorldCamera>)` (`0x8005DE90`), `AttachCamera` (`0x8005DFB0`), `RemoveCamera` (`0x8005E140`), `ClearCamera(u32)` (`0x8005E160`), `GetShadowCamera` (`0x8005E360`), `SetCurrentCamera` (`0x8005E3D0`), `GetCurrentCamera` (`0x8005E410`), `GetG3dCamera` (`0x8005E480`), `SetOutlineAlpha` (`0x8005E520`), `SetOutlineWidth` (`0x8005E540`), `AddGround` (`0x8005ED90`), `SetGround` (`0x8005EEA0`), `Add` (`0x8005EEE0`), `DynamicCast<BWorldCamera>` (`0x8005F010`) and its `static_pointer_cast` (`0x8005F130`), and the weak `BWorldObj::OnEnterWorld` (`0x8005F440`). The `weak_ptr::lock` instances at the end of the unit (`0x80064EB0`, `0x80064FE0`) match too.

The `set<shared_ptr<BWorldObj>>` destructor (`0x8005BC40`, #44) now matches as well.

## Behavior

- The camera table holds at most 32 `weak_ptr<BWorldCamera>` slots. `SetCamera` clears the slot, grows the vector, stores the camera, writes the slot index into the camera (`BWorldCamera+0x48`), records it as the shadow camera when its type has flag 4, and then calls the camera's first own virtual (vtable `0x80399FC8` slot 0x48) with -1. `ClearCamera` calls the same virtual with the old index.
- `AttachCamera` keeps a camera that already sits at its recorded index, otherwise takes the first empty slot.
- `AddObject` (`0x8005F230`) assigns handles: it tries `mLastObjHandle + i` for i from 1 to the capacity, gives up when bit 31 is set, and takes slot `handle % capacity` when it is empty. It writes the world pointer and handle into the object (`BWorldObj+0x14`, `+0x18`), stores the object in the slot with flags `type | 0x40000`, and links the slot at the head of the list when the type has 0x180 set, otherwise at the tail. Links are 1-based slot indices. It then calls `OnEnterWorld` and, while the world is paused, `BWorldObj::Pause` (`0x8006A4B0`).
- `DynamicCast<T>` returns the object cast to `T` when its type has `T::TYPE` set (`BWorldCamera` 2, `CShadowCamera` 4). The RTTI of `CShadowCamera` lists `BWorldCamera` as its base at offset 0.

`BWorldCamera`, `CShadowCamera`, the cast helpers, `UpdateIndex`, `GetG3dCamera` (`0x800666D0`) and `Pause` are descriptive names. `BWorldCamera.h` and `CShadowCamera.h` declare only what this unit uses; their own units own the rest of the class declarations.

## Header changes

- `<new>` declares the placement `operator delete(void*, void*)`. When a matching placement delete exists, every placement-new of a type whose constructor can throw gets a cleanup region. The original stores `r1` into a stack slot at the start of each one. Without the declaration those stores and their stack slots are missing, and the vector sized constructor and `resize` frames come out 0x20 and 0x50 bytes too small.
- `weak_ptr::lock`, `~shared_ptr`, `~weak_ptr` and the split-buffer destructor are defined outside their classes. The original emits them at the end of the unit, and the `lock` instances are calls, not inlined.
- The MSL `set` has four destructor levels: `set` holds a `tree`, which derives from `red_black_tree`, which derives from `tree_imp`. That reproduces the four `this` checks seen in both `BWorld` and `BModelObj`. `DrawLayerSet` is now a typedef ([F-39-1](39-bmodelobj.md)).

## Status

objdiff in a scratch build of the unit: every function above matches. Remaining in this range:

- `SetCamera` (`0x8005D510`): it inlines `vector<weak_ptr<BWorldCamera>>::resize`. That resize has the same stack-slot order difference as the out-of-line `resize`; only slot offsets and register numbers differ.
- `AddObject` (`0x8005F230`) differs only in register numbers: the original gives the entry base pointer `r29`, above `this` and the object.
- `0x8005E560` (0x608 bytes), `0x8005EB70` and the inline tail call at `0x8005ED70` load a scene animation and its light setting through the file resource system, whose classes are not reconstructed yet.

## Traps

- **A missing placement delete removes `r1` stores.** Symptom: the original stores `r1` to a stack slot in the middle of a placement copy construction and has a larger frame. Declare `operator delete(void*, void*)` in `<new>`.
- **Out-of-line template members are calls only when the original calls them.** Symptom: the original calls `weak_ptr::lock` as a function emitted at the end of the unit. Define the member outside the class.
- **Comparison operand order matters.** Symptom: `cmplw rA, rB` with the operands swapped, or `subf` with the operands reversed, in an equality test. Write the comparison in the original order, for example `camera.get() == lock().get()`.
