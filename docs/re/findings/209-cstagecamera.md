# CStageCamera unit and the rest of #209

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #209 lists 34 functions in `.text` `0x80110140`–`0x80112EE8`. The last nine form the complete unit `game/CStageCamera.cpp`; the others belong to two earlier units (F-209-4).

## F-209-1: CStageCamera unit boundaries

The unit is registered as `game/CStageCamera.cpp` (descriptive file name; no source-file string is embedded, and `"CStageCamera.pkc"` is the script it loads).

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80029534`–`0x800295E8` | Entries of the eight `extabindex` records below; `0x800295E8` is the entry of `0x80112EF0` |
| `extabindex` | `0x8004BEAC`–`0x8004BF0C` | First record names `0x801128C0`; the record before it names `0x80112880`, the next names `0x80112EF0` |
| `.text` | `0x801128C0`–`0x80112EE8` | The previous function `0x80112880` is slot 0 of the `shared_ptr_deleter<vector<NPpu::Slice> >` vtable `0x803ADE0C` and belongs to the preceding unit; `0x80112EF0` uses the `CStatistic` unit's `"vector length error"` at `0x803ADFC8` |
| `.data` | `0x803ADF30`–`0x803ADFC8` | `"CStageCamera.pkc"` (used by `0x801129B0`), the vtable `0x803ADF44`, the RTTI name `"CStageCamera"` and its base list; `0x803ADFC8` starts the `CStatistic` unit's exception-string cluster |
| `.sdata` | `0x804B7380`–`0x804B7388` | `CStageCamera` type info; `0x804B7378` is the `default_delete<vector<NPpu::Slice> >` type info used by `0x801126B8`, `0x804B7388` is used by `0x80113910` |
| `.sdata2` | `0x804BA108`–`0x804BA128` | Eight floats used only here (`0`, `256/360`, `90`, `360/256`, `-45`, `20.2561665`, `0.01`, `1000`); `0x804BA100` is a double of `0x8010CD94`, `0x804BA128` is used by `0x80118F3C` |

The unit has no `.ctors`, `.bss` or `.sbss` contribution. The whole unit lies inside #209.

## F-209-2: CStageCamera layout and virtual functions

The class is 0x74 bytes: `ScriptBinding_NewStageCamera` allocates `new(0x74)` and calls the constructor with type 8. RTTI `0x804B7380` with base list `0x803ADFA0` names `BWorldCamera` at 0 (which derives from `BWorldObj`, `ICalc` and `enable_shared_from_this<BWorldObj>` at 4).

| Offset | Member | Type | Evidence |
| --- | --- | --- | --- |
| `0x00`–`0x68` | base | `BWorldCamera` | Constructor `0x801128C0` calls `BWorldCamera(type | 8)` at `0x80066460`; destructor ends with `~BWorldCamera` `0x800665B0` |
| `0x68` | `mReserved` | `u32` | Zeroed by the constructor and never read in the unit, by the script camera bindings or by any reader found in a scan of game code; name is a placeholder for an unknown purpose (low) |
| `0x6C` | `mEngine` | `std::tr1::shared_ptr<CPpuEngine>` | `0x801129B0` creates a 0xC4-byte `CPpuEngine` and resets the pointer; `Calc` and `OnLeaveWorld` use it; the destructor releases it |

`CStageCamera::TYPE` is 8, which `IsKindOf<CStageCamera>` (`0x801317F0`) tests.

The vtable `0x803ADF44` keeps the `BWorldObj` order of [SYMBOLS](../SYMBOLS.md) plus `BWorldCamera::UpdateIndex`. `CStageCamera` overrides the destructor (`0x80112910`), `Calc` (`0x80112D90`), slot 0x2C (`0x80112EE0`), `OnLeaveWorld` (`0x80112E90`) and `UpdateIndex` (`0x80112DF0`). The other slots point to the `BWorldCamera` overrides `SetPosition` `0x80067CD0` and `OnEnterWorld` `0x80067DB0`, and to the `BWorldObj` functions `CalcMtx` `0x80062C70`, `SetScale` `0x80067EE0`, `SetRotation` `0x80067ED0`, `GetSize` `0x800605C0`, `GetPosition` `0x80057D00`, `SetVisible` `0x80067EA0`, `GetEffect` `0x80057CE0`, `SetEffectEnable` `0x80057CD0`, `GetDrawLayers` `0x80067E90` and `UpdateVisible` `0x80057CC0`. The `BWorldObj` slots are the targets of the same entries in its vtable `0x8039A2B8`.

Slot 0x2C returns a `CPpuEngine*`, not a `BWorldObjOwner*`: `ScriptBinding_SendTo` passes its result as `this` to the `CPpuEngine` member `0x80101CA0`, and both `CStageCamera` and `CSimpleGround` (`0x8010DDC0`) return their engine pointer unadjusted. PR #991 renamed the slot to `GetPpuEngine` (descriptive) in `BWorldObj.h` and `CDividedGround.h`.

`BWorldCamera` (0x68 bytes, also from PR #991): `mIndex` 0x48 (-1 initially), `mNextMotionID` 0x4C (starts at 1; `0x800677C0` hands it to a new motion program and skips 0 on wrap), `mAnmFile` 0x50 (`shared_ptr<CFileData>`, assigned by the camera animation loader `0x80066720`), `mMotionPrograms` 0x58 (`std::list<shared_ptr<CMotionProgram> >`, whose deleter type names `BWorldCamera::CMotionProgram`) and `mMotionFrame` 0x64 (`f32`, advanced by `0x80067550` and shared with every motion program). Member names are descriptive (medium).

## F-209-3: the functions

objdiff with `src/game/CStageCamera.cpp`: 9/9 functions and 1,520/1,520 code bytes at 100%; the unit links as `Matching` and the full DOL verifies.

| Address | Function | Notes |
| --- | --- | --- |
| `0x801128C0` | `CStageCamera(u32 type)` | |
| `0x80112910` | `~CStageCamera()` | |
| `0x801129B0` | `LoadScript(int mode, const shared_ptr<NPpu::CEnvironment>&)` | `mEngine.reset(new CPpuEngine)`, then `CPpuEngine::Load(this, "CStageCamera.pkc", mode, env)`. The inline `LoadScript(const shared_ptr<NPpu::CEnvironment>&)` forwards mode 0; its weak copy is `0x801319A0` in `ScriptCamera.cpp` |
| `0x80112B00` | `GetStickWorldAngle(const IUIInput*)` | Called by `ScriptBinding_GetPadDirDeg` (`0x8015149C`) on world camera 0 when it is a `CStageCamera`. Returns 0 unless the input is connected with positive stick length; otherwise rotates the stick direction (stick angle + 90 degrees) through the inverse camera matrix, measures it from the transformed origin with `Atan2Deg(x, z)` and wraps the result with `NormalizeSignedAngle` (`0x801B7280`) |
| `0x80112CA0` | `UpdateLightPosition()` | For every light set of the world's `ScnRoot` light setting, moves each spot light to the camera's x and z, keeping its height |
| `0x80112D90` | `Calc()` | `BWorldCamera::Calc`, `UpdateLightPosition`, then the engine's `Calc` |
| `0x80112DF0` | `UpdateIndex(int)` | When the camera has a slot, sets the g3d camera position, a rotation posture of (-45, 0, 0) and a perspective of 20.2561665 degrees, `GetDisplayAspectRatio()`, 0.01 and 1000 |
| `0x80112E90` | `OnLeaveWorld()` | `BWorldCamera::OnLeaveWorld`, then `NPpu::CEngine::Reset` on the engine |
| `0x80112EE0` | `GetPpuEngine() const` | Inline in the class; emitted last |

`CStageCamera`, `CPpuEngine` and the `NPpu` names are RTTI or existing names. `LoadScript`, `GetStickWorldAngle`, `UpdateLightPosition`, `GetPpuEngine`, `mReserved` and `mEngine` are descriptive. The file-local helper `Transform` (by-value matrix-vector product) is inlined and leaves no symbol.

## F-209-4: the other functions of #209

`0x80110140`–`0x801128BC` belong to two earlier units, separated by duplicate exception-string clusters (with `-str reuse` one unit pools identical literals):

- **`0x80110140`–`0x801119A4`** continue a unit that starts in #208. Its functions use the first `"vector length error"`/`"tree::insert length error"` cluster at `0x803ADD58`–`0x803ADDBE` (`0x8010F150` in #208, `0x80110990` and `0x80110F90` here) and the `.sdata` bytes `0x804B7368`–`0x804B736B` (`0x801104AC`, `0x801108A0`). The split belongs to #208.
- **`0x801119B0`–`0x801128BC`** are a separate unit, entirely inside #209. Its data is the second cluster `0x803ADDC0`, the `shared_ptr_deleter<vector<NPpu::Slice> >` vtable `0x803ADE0C` with its type names, through `0x803ADF30`; `.sdata` `0x804B7370`–`0x804B7380`. `0x80111A50` and `0x80111D80` use that cluster and the deleter vtable. The unit starts with the out-of-line constructor `0x801119B0` and destructor `0x801119C0` (with a deleting flag) of an 8-byte class holding one `shared_ptr`; `BWorldObj`'s constructor and destructor, `CPpuEngine` (member 0x88) and other game classes call them, so they are not weak template copies of an earlier unit. Its script users are the register and parameter bindings (`SetMapDescParams`, `SetStageDescParams`, `GetNewObjRegister`, `method_ppdGetRegister`, `method_ppdSetRegister`), and `BWorldObj` `0x8006A480`, `0x8006A490` and `0x8006A4A0` call `0x80111D20`, `0x80111D80` and `0x80112590`. Confidence in the `0x801119B0` start is medium: `0x801119B0` and `0x801119C0` could also close the previous unit. The file name is not evidenced, so the unit is not split yet.

## Method

GNU objdump 2.42 and Python scans of `build/wpse-v1/main.elf` (vtables, RTTI base lists, `extabindex`, absolute, small-data and call references), the dtk-generated assembly, and `tools/re/unitdiff.py`. No decompiler output was used.

## Traps

- **A shared header can be overwritten when moving changes between branches.** Checking out the stash's headers onto a fresh branch replaced `NPpuVM.h` with the stale pre-rename copy, and the build failed on the missing `NPpu::CEngine`. Check `git diff <base> origin/main` for every file before carrying it across.
- **A misspelled RTTI symbol keeps a weak duplicate alive.** `symbols.txt` named `0x804B5A90` `__RTTI__Q33std3tr139enable_shared_from_this<9BWorldObj>` (length 39 instead of 35). The compiled object's weak type info did not merge with it, so the type info and its 45-byte name string were linked a second time and `.data` grew by 0x40. Correct the mangled name rather than adding padding.
- **Weak vtables and their functions need the original names.** The unit links only when the `CPpuEngine` deleter vtable, its type info, its three functions and the `move_ptr` destructor carry their mangled names, so the linker drops the object's weak copies.
- **Constant order follows the source operand order.** `SinCosDeg(x)` expands to `x * (256/360)` and emitted `90` before `256/360` in `.sdata2`; the original has `(256.0f / 360.0f) * (90.0f + angle)`.
- **A by-value helper explains extra stack copies.** The original transforms into a temporary and copies it back, with the subtraction temporary above the transform temporary; a file-local `Transform` returning `VEC3` reproduces the frame.
- **The light test is the spot flag.** The original tests bit 0 of the light flags, which is `IsSpotLight`, not `IsEnable` (bit 2).

## F-209-5: Recover the register value's original Slice identity

The register ownership RTTI names `std::vector<NPpu::Slice>` in the original image. `CValue` was a descriptive reconstruction name, not a recovered spelling. The register's Set copies both words directly into an eight-byte vector element, and GetValue returns the same two-word payload. Reserve's native exception record names the vector storage cleanup at `0x80110920`; Set's records name the split-buffer cleanup at `0x80111930`. These are also emitted by MapPiiTable's value vector. This links the register and table value types without introducing inheritance or conversion storage.

The complete existing two-int value declaration is therefore named `NPpu::Slice`, retaining `NPpu::CValue` as a compatibility typedef. Its methods, layout, and public API spelling remain intact. CPpuRegister's forward declaration uses the same alias; MapDesc's redundant forward declaration is removed with the MapPiiTable owner's agreement. Thirty-eight configured symbol spellings follow the actual compiler's type mangling. No split or source acceptance changes accompany this prerequisite.

On baseline `f62a0a0cbf44744e01f099f036e7ee17d14ef177`, all 759 compiled source objects retain identical function bytes and relocation identities, and identical allocated section metadata, payloads, and relocation identities after the explicit type-name normalization. The full build exits 0 and the frozen executable verification passes. Independent MapPiiTable owner control retains all 55 emitted function bodies and every allocated section and relocation.

The actual outside CPpuRegister producer now reproduces all twelve functions, 3,812 code bytes with 118 applied relocations, plus eleven exception records totaling 600 bytes. These producer results support the type identity but do not mark CPpuRegister source-selected yet. Original #209 remains incomplete: its MapPiiTable continuation is coordinated with #208.

Reproducers and raw controls are preserved outside the repository under `209-reference/`: `209-register-strict-audit.py`, `209-register-eh-audit.py`, and `209-alias-control/209-audit-consumers.py`. Independent #208 receipt is `208-reference/208-alias-control/report.json`.

### Traps

Matching two-word layouts alone does not establish a base class. Native ownership RTTI, typed producer payloads, and shared exception cleanup identities support one value type here; adding an inheritance layer would introduce an unsupported source contract.

## F-209-6: CPpuRegister producer and primary vector contracts

The complete CPpuRegister unit is independently split at text 0x801119B0–0x801128C0, extab 0x800292DC–0x80029534, extabindex 0x8004BE28–0x8004BEAC, data 0x803ADDC0–0x803ADF30, and sdata 0x804B7370–0x804B7380. The last instruction is at 0x801128BC; the exclusive text end is 0x801128C0. Duplicate exception-string clusters and the register-specific ownership RTTI separate this unit from MapPiiTable and CStageCamera. CPpuRegister is an evidenced class name; the filename is descriptive. The split-only full executable verifies against the frozen manifest.

The actual seven-method producer naturally emits its five owned ownership helpers. Reserve lazily allocates the value vector and reserves capacity; Set grows the vector, initializes intervening elements through Slice::SetInt(0), then copies the requested value. Reset releases the vector. Swap copies the other register owner, assigns this owner to the other register, and assigns the saved owner to this register.

The original 2006 MSL vector header supplies four missing source contracts: the public reserve wrapper (lines 2425–2433), distinct move_construct_to_end using a strict pointer comparison (936–943), append_realloc clearing the moved-from owner before direct std::swap operations (1278–1289), and resize reading the size member directly (1985–1993). The ordinary copy loop retains its != comparison. The primary header is pinned by the 2006 manifest recorded in #198. The outside candidate preserves existing POD dispatch semantics and introduces no compiler flags or retention attributes.

These contracts make all twelve native instruction streams exact: constructor16, destructor136, Reserve720, GetValue92, Set1960, Reset96, Swap272, deleter-query76, dispose96, shared-owner destructor132, move-owner destructor152, and deleter destructor64. Strict audit applies all118 code relocations; all600 owned exception bytes and17 cleanup relocations also match. The compiler emits extra duplicate library definitions normally; source-only full-link acceptance remains necessary to show they are discarded correctly.

Independent existing producer controls prove the renamed public reserve chain from CGameSession::SaveToSlot and the destructive move chain from ScriptCamera: seven bodies3372bytes with fully bound targets match native. ScriptCamera reallocate and its two move helpers have exact24/8/8-byte exception records. SaveToSlot has an existing exception-record mismatch; baseline and candidate records are identical200bytes, so this is not a regression or a new matching claim. No whole CGameSession source acceptance is asserted.

### Traps

A public forwarding method can change register allocation under file IPA. Replacing the public reserve wrapper with a using declaration preserved behavior but changed39 words in the register producer. Similarly, caching the size member in resize changed32 registers in Set; the primary direct-member form restores the native stream. Preserve the primary source structure rather than adjusting flags or manually selecting registers.
