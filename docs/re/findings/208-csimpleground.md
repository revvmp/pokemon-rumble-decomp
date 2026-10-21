# Issue 208: SelectLight and CSimpleGround

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #208 starts at `0x8010D7D0`, inside the `game/CShadowCamera.cpp` unit registered by [F-207-2](207-cshadowcamera.md#f-207-2-gamecshadowcameracpp-unit-boundaries).

## F-208-1: SelectLight and CSimpleGround

`0x8010D7D0`-`0x8010DDD8` are nine functions of the `CShadowCamera.cpp` unit. They are reconstructed in `src/game/CShadowCamera.cpp` (after `SetupShadowModel`, in address order) with the class declared in `include/game/CSimpleGround.h`.

| Address | Function | Match |
| --- | --- | --- |
| `0x8010D7D0` | `CShadowCamera::SelectLight()` | 100% |
| `0x8010D8E0` | `CSimpleGround::CSimpleGround(u32)` | 100% |
| `0x8010D940` | `CSimpleGround::~CSimpleGround()` | 100% |
| `0x8010D9E0` | `CSimpleGround::Initialize(bool)` (empty) | 100% |
| `0x8010D9F0` | `CSimpleGround::LoadScript(const char*, int, const shared_ptr<NPpu::CEnvironment>&)` | 100% |
| `0x8010DB40` | `CSimpleGround::LoadModel(const char*, bool)` | 99.6% |
| `0x8010DD20` | `CSimpleGround::Calc()` | 100% |
| `0x8010DD70` | `CSimpleGround::OnLeaveWorld()` | 100% |
| `0x8010DDC0` | `CSimpleGround::GetPpuEngine() const` (inline, emitted last) | 100% |
| `0x8010DDD0` | `@72@__dt__13CSimpleGroundFv` (BSoundSource-base destructor thunk) | 100% |

Behaviour:
- `SelectLight` walks the light sets of the world's `ScnRoot` light setting and takes the first light that is neither a spot nor a specular light (`IsDiffuseLight`). It stores its set and light indexes and sets the automatic-selection flag; with no world or no such light it stores 0 and 0.
- `CSimpleGround` is a `BModelObj` with type bit `1 << 14` (the `IsKindOf` test in `ScriptBinding_method_CSimpleGround_loadModel` at `0x801409A0`), a reserved word at `0xF8` (zeroed, never read) and the script engine `shared_ptr<CPpuEngine>` at `0xFC`, the same shape as `CItem`. Its vtable `0x803ADC90` overrides the destructor, `Calc`, `GetPpuEngine`, `OnLeaveWorld`, and adds one empty virtual after `BModelObj`'s last slot, named `Initialize(bool)` after `CItem`'s slot (descriptive).
- `LoadScript` creates the engine and loads a script for the object. `LoadModel` is called by the script binding with a path and `background = false`; it loads the file list, loads the model without a model name through the shared `CModel::LoadParams` constant `{nullptr, false, 0xD15, 1, nullptr}` at `0x80354E20`, terminates the fiber with 1 on failure, otherwise draws it in the translucent priority 1, installs it with `ChangeModel(model, 1)` and sets `gFrameTimingReset`.
- `LoadModel` differs from the original only in the names of two relocation targets (the `CModel` deleter vtable `0x8039BA78` and the parameter constant `0x80354E20`), which `symbols.txt` keeps under placeholder labels.

Symbols renamed in `symbols.txt`: the nine functions, `__vt__13CSimpleGround` (`0x803ADC90`) and `__RTTI__13CSimpleGround` (`0x804B7360`).

## Remaining work in #208

`0x8010DDE0`-`0x80110134` are the first part of a unit that runs to `0x801119A4` ([F-209-4](209-cstagecamera.md#f-209-4-the-other-functions-of-209)). It is a set of non-inline container functions: tree destructors unrolled four levels, node insertion and `vector<NPpu::Slice>` helpers called from the `CGameSession` and script-binding code. It is not reconstructed.

## Method

GNU objdump 2.42 with `-M gekko`, call-site scans of `build/wpse-v1/main.elf`, and `tools/re/unitdiff.py`. No decompiler output was used.

## Traps

- **Pass-through virtuals after `BModelObj` are visible only in the second vtable group.** `BModelObj`'s new virtuals sit after the `BSoundSource` destructor thunk at vtable offset `0x48`, so a class that adds one more virtual shows it as the entry after `AddDrawLayer` (`0x800570D0`), not in the first sixteen slots.
