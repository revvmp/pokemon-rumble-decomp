# Script camera bindings, second block (#232)

Target `wpse-v1`. Issue #232 lists 25 functions in `.text` `0x80131D70`–`0x801325C4`, inside `game/ScriptCamera.cpp` ([F-231-1](231-script-camera-unit.md#f-231-1-unit-boundaries)). `src/game/ScriptCamera.cpp` reconstructs all 25, and objdiff shows each at 100%. The unit stays `NonMatching` until #233 to #236 are written.

## F-232-1: the functions

| Address | Identity | Evidence |
| --- | --- | --- |
| `0x80131D70` | `shared_ptr<CStageCamera>::operator->` | Weak, first used by `NewStageCamera` |
| `0x80131D80` | `shared_ptr<NPpu::CEnvironment>()` | Weak, the empty environment passed to `LoadScript` |
| `0x80131D90` | `NewWorldCamera` | Binding string; `new(0x68)` and `BWorldCamera(BWorldCamera::TYPE)`; it then follows `NewStageCamera` but does not call `LoadScript` |
| `0x80131EB0`–`0x801321B0` | `shared_ptr<BWorldObj>(const shared_ptr<BWorldCamera>&)`, `shared_ptr<BWorldCamera>(BWorldCamera*)`, its `enable_from_this`, `weak_ptr` constructor, `move_ptr`, compressed-pair and `default_delete` constructors, the deleter constructor (vtable `0x803A2CA0`), `release` and `first` | The same TR1 shapes as #231 for `BWorldCamera` |
| `0x801321C0` | `shared_ptr<BWorldCamera>` copy constructor | No defaulted third argument; builds the argument of `SetCurrentCamera` |
| `0x80132210` | `GetCurrentCamera` | Pushes the current camera's id or 0 |
| `0x80132270` | `method_camSetLookAt` | Fetches the argument count, pops the up vector (defaults 0, 1, 0, when fewer than 7, 6 or 5 arguments) and the target, and sets a `POSTURE_LOOKAT` posture |
| `0x80132370` | `NPpu::CEngine::FetchArgCount` | Tail call to `Fetch`: the native-call instruction is followed by the argument count |
| `0x80132380`–`0x801323D0` | `Fetch`, `PopFloat`, `Pop` | Named by F-282 |
| `0x80132400` | `NPpu::CEngine::PushNull` | `PushInt(0)`; every setter binding ends with it |
| `0x80132410` | `nw4r::g3d::Camera::PostureInfo` default constructor | Calls `VEC3()` for the members at 4, 0x10 and 0x1C |
| `0x80132460` | `method_camSetRotate` | Pops the rotation and sets `POSTURE_ROTATE` |
| `0x80132500` | `method_camSetAim` | Pops the twist (default 0 when fewer than 5 arguments) and the target, and sets `POSTURE_AIM` |

The names `FetchArgCount` and `PushNull` are descriptive, with medium confidence. They are inline members added to `include/game/NPpuVM.h`. The binding names are original.

## F-232-2: `BWorld::SetCurrentCamera` takes its pointer by value

All five callers in the executable (`0x800ACB84`, `0x800CDFC4`, `0x800F63FC`, `NewStageCamera`, `NewWorldCamera`) copy a `shared_ptr<BWorldCamera>` before the call and release it afterwards. `NewWorldCamera` does so even though its local already has that type. Only a by-value parameter explains this. The declaration, `src/game/BWorld.cpp` and the symbol are now `SetCurrentCamera__6BWorldFQ33std3tr126shared_ptr<12BWorldCamera>`. `BWorld.cpp` still matches it at 100%, because a by-value parameter makes no difference inside the callee.

## Names outside the block

Renamed for the objdiff pairing: `__rf__` of `shared_ptr<BWorldCamera>` `0x800AD270`, the `move_ptr<BWorldCamera>` destructor `0x800B1990`, and the `nw4r::g3d::Camera` and `ResCommon<CameraData>` copy constructors `0x80098620` and `0x80098650`. Also, from the vtable slots, the `shared_ptr_deleter<BWorldCamera>` vtable `0x803A2CA0`, type info `0x804B6810`, destructor `0x800B1D70`, `get_deleter` `0x800B1580` and `dispose` `0x800B15D0`.

## Method

GNU objdump on the split object and `build/wpse-v1/main.elf`, `tools/re/unitdiff.py`, and `configure.py`, `ninja` and `tools/verify/verify.py`. No decompiler output was used.

## Traps

- **A copy before a `const&` call can mean a by-value parameter.** A guessed `const shared_ptr&` signature matched the callee at 100%, but the callers' copies showed by-value. Check the call sites before you trust a reference parameter in `symbols.txt`.
- **Under `dont_inline` an implicit copy constructor is a call.** The original stores the `nw4r::g3d::Camera` returned by `GetG3dCamera` and then calls the out-of-line `Camera` and `ResCommon` copy constructors. A named local `Camera camera = pCamera->GetG3dCamera();` reproduces this.
