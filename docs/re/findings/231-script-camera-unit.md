# Script camera binding unit (#231)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #231 lists 21 functions in `.text` `0x80131780`–`0x80131D64`. They are the first functions of one source unit that runs to `0x801352EC` and so spans #231 to #236.

## F-231-1: unit boundaries

| Section | Range | Evidence |
| --- | --- | --- |
| `.text` | `0x80131780`–`0x801352EC` | Ends with the static initializer `0x801352A0` (0x4C bytes), listed in `.ctors` at `0x803529F0`; `0x801352F0` (`method_BWorldCamera_play`) starts the next unit |
| `extab` | `0x8002DE28`–`0x8002E278` | Entries of the 99 `extabindex` records whose functions lie in the `.text` range; the next record (`0x801352F0`) points to `0x8002E278` |
| `extabindex` | `0x8004D07C`–`0x8004D520` | First record names `0x80131780`; the record before it names `0x80131740` |
| `.ctors` | `0x803529F0`–`0x803529F4` | Added by dtk |
| `.data` | `0x803B65A0`–`0x803B66C8` | Jump table `0x803B65A0` (targets `0x801333C0`–`0x80133484`), static object `0x803B65C0` (0x58 bytes, filled by the static initializer), `shared_ptr_deleter<CStageCamera, default_delete<CStageCamera>>` vtable `0x803B6618`, its type names and base list; `0x803B66C8` (`"5:/event/"`) belongs to the next unit |
| `.sdata` | `0x804B79A8`–`0x804B79B8` | Type info for the deleter and for `default_delete<CStageCamera>`; `0x804B79B8` (`".arc"`) belongs to the next unit |

The start follows the end of `main.cpp`. Its last string, `"exception"`, is at `0x803B6590`, directly before this unit's jump table. No unit references `.sdata2`, `.bss` or `.sbss`. The unit is split and registered `NonMatching`; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with the original object.

No original file name is embedded. The name `game/ScriptCamera.cpp` is descriptive (the survey label was `bind_camera`). The script bindings it defines are `NewStageCamera` (#231), `NewWorldCamera`, `GetCurrentCamera`, the `method_cam*` setters and getters, `IsTV16x9`, the `method_camIsObjInSafeFrame*` group and `method_camAdjustPos` ([F-29-1](29-script-bindings.md)).

## F-231-2: the functions of #231

The unit is compiled without inlining: `BWorldObj::GetType` (`0x80131840`, one load) and every TR1 helper are called out of line, as in `CDlgPiiSelector.cpp` ([F-92-1](92-cdlgpiiselector-construction.md)). Out-of-line copies follow their first referencing function.

| Address | Identity | Evidence |
| --- | --- | --- |
| `0x80131780` | File-local helper: pops an object from the script stack and casts it to `CStageCamera*` | Calls `0x80100DB0` (CPpuEngine, pops an id and looks up the world object), then `0x801317B0`; called by the 20 `method_cam*` bindings |
| `0x801317B0` | Pointer `DynamicCast<CStageCamera>` | Returns its argument if `0x801317F0` is true, else null |
| `0x801317F0` | Pointer `IsKindOf<CStageCamera>` | Non-null and `GetType() & 8` |
| `0x80131840` | `BWorldObj::GetType` | Loads offset 0x10; also called from six other units |
| `0x80131850` | `NewStageCamera` | `new(0x74)` and `0x801128C0(this, 8)`: `CStageCamera(u32)`, which ORs 8 into the type and calls the `BWorldCamera` constructor `0x80066460`; so `CStageCamera::TYPE` is 8 |
| `0x80131990` | `CPpuEngine` accessor returning `&member` at 0x88 | Passed to `0x8006A4A0`, which assigns it to `BWorldObj` offset 0x24 (`mOwnerRef`, `shared_ptr<BWorldObjOwner>`) |
| `0x801319A0` | Inline `CStageCamera` member forwarding `(0, r)` to `0x801129B0` | `0x801129B0` creates a 0xC4-byte `CPpuEngine` and stores it at `CStageCamera` 0x6C/0x70 |
| `0x801319B0` | Pushes an `int` as an `NPpu::CValue` | Builds a value with `0x800C98C0` (type 0, payload) and calls `0x801319F0` |
| `0x801319F0` | Pushes a `CValue` | `mStack[mSP++] = value` through the copy assignment `0x801100D0`; also called from other units |
| `0x80131A10`, `0x80131A60` | `shared_ptr<BWorldCamera>` and `shared_ptr<BWorldObj>` from `const shared_ptr<CStageCamera>&` | Converting constructors with the defaulted third argument (`li r5, 0`) |
| `0x80131AB0`–`0x80131D60` | `shared_ptr<CStageCamera>(CStageCamera*)` and its `move_ptr`, compressed-pair, `default_delete`, deleter constructor, `release` and `first()` helpers | Same shapes as F-92-2 |

`NewStageCamera` makes the camera, sets its owner from the engine, adds it to the engine's world (`0x800FFA20`, then `BWorld::Add`), and on success makes it current (`BWorld::SetCurrentCamera`), calls `0x801319A0` with an empty `shared_ptr`, and pushes the camera's id (`0x800C98B0`, `BWorldObj` offset 0x18); otherwise it pushes 0. It returns 0.

The `CPpuEngine` RTTI base list at `0x803A9760` names its bases `NPpu::CEngine` at 0, a base at 0x64 and `enable_shared_from_this<CPpuEngine>` at 0x68. The class `include/game/NPpuVM.h` calls `NPpu::CVM` is therefore `NPpu::CEngine` in the original.

## F-231-3: what blocks the source

Every #231 function except `0x80131780` and `NewStageCamera` is a weak copy that is emitted only through `NewStageCamera` or the `method_cam*` bindings. The binding needs declarations that have no header and belong to other issues:
- `CStageCamera` (#209): the constructor, the 0x74-byte layout and the member at `0x801129B0`, whose second parameter type and name are not evidenced.
- `CPpuEngine` (#195): the layout up to the member at 0x88 (constructor `0x800FE5F0`), the world getter `0x800FFA20` and the object lookup `0x80100DB0`.
- `NPpu::CVM` push and the `CValue` int constructor, in the NPpu header that #281 to #283 maintain.

The source therefore stays uncommitted, and the unit links the original object.

Superseded: PRs #986, #988, #991 and #992 supplied these declarations, and F-231-4 records the source.

## F-231-4: the source of #231

`src/game/ScriptCamera.cpp` reconstructs all 21 functions; objdiff shows each at 100%. The file uses `#pragma dont_inline on`, like `CDlgPiiSelector.cpp`.

- **Bindings.** Each binding is a global `int f(NPpu::CEngine*)` named by its script string, so `NewStageCamera` is `NewStageCamera__FPQ24NPpu7CEngine`. The binding table is in another unit, so the functions are not static. They reach `CPpuEngine` members through `static_cast<CPpuEngine*>`, which has offset 0 and no null test.
- **Helper.** `0x80131780` is the file-local `static CStageCamera* PopStageCamera(CPpuEngine*)`, which returns `DynamicCast<CStageCamera>(pEngine->PopWorldObj())`. It is emitted only when a `method_cam*` binding (#232) calls it. Its symbol is `scope:local`.
- **Pointer casts.** `BWorldObj.h` gains pointer overloads of `IsKindOf<T>(const U*)` and `DynamicCast<T>(U*)`. They sit beside the existing `shared_ptr` overloads and reproduce `0x801317F0` and `0x801317B0`.
- **Owner.** `0x8006A4A0` is declared `BWorldObj::SetOwner(const shared_ptr<BWorldObjOwner>&)`. The name is descriptive, low confidence.
- **Constant.** `NewStageCamera` passes `CStageCamera::TYPE` (8) to the constructor. It starts the script with the inline `LoadScript(const shared_ptr<NPpu::CEnvironment>&)` and an empty environment, and pushes `GetId()` or 0.
- **`.sdata2`.** The unit also owns `.sdata2` `0x804BA1B8`–`0x804BA1E8`: 0.0f, 1.0f, 0x804BA1C0–0x804BA1CC, two int-to-float doubles and -1.0f, which its bindings read 42 times. F-231-1 missed this range. The split now includes it; the previous unit's constants end at `0x804BA1B8`, and `0x804BA1E8` starts the next unit.

Renamed in `symbols.txt` for the objdiff pairing and a later link: the 21 functions and every weak callee and destructor the source names, including `0x80065110`, `0x80065630`, `0x80065870`, `0x8008F540`, `0x8009D2C0`, `0x800C98B0`, `0x80134FE0` and `0x80135070`. Also `CPpuEngine::GetWorld` `0x800FFA20` and `PopWorldObj` `0x80100DB0`, from the names in the #195 header, and the `shared_ptr_deleter<CStageCamera>` vtable `0x803B6618` and type info `0x804B79A8` and `0x804B79B0`. Each name is what the compiler emits for the call, so the linker resolves the original call to the same symbol.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf` and the split object, and a Python scan of `extabindex` records and absolute and small-data references of the `.text` range. No decompiler output was used.

## Traps

- **A binding file's first function can be a file-local helper.** `0x80131780` precedes the first binding and is followed by the weak functions it calls, so the unit starts before `NewStageCamera`.
- **The `NPpu::CVM` name is descriptive.** RTTI of `CPpuEngine` names its base `NPpu::CEngine`.
- **An unused static helper is not emitted.** `PopStageCamera` produced no code until a binding called it. Reconstruct its first caller before judging the helper's match.
- **A split can miss `.sdata2`.** The unit's binding constants sat in an automatic `.sdata2` split. objdiff then compared relocations against labels outside the object. Check `R_PPC_EMB_SDA21` targets of the original object when you write the split.
