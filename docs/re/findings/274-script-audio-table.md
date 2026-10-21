# F-274-1: Script audio and descriptor contracts

Issue #274 contains 28 original bindings, 5,644 instruction bytes, at `0x8015CA00–0x8015E0D4`. This checkpoint accepts shared declarations only. It does not select any of those bindings as source or complete the issue.

## Native contracts

The script table supplies the original binding names. Nine audio bindings have strictly paired native instructions and every relocation (1,804 bytes), with all nine exception records (72 bytes). Their literal defaults are independently checked: StopBGM uses `0.1f`, StopSE uses `0.0f`, and GetPitch uses `1.0f`.

`BSoundSource` has nonvirtual StartSE(u32), StopSE(f32), SetPitch(f32), and GetPitch() const methods at `0x801E88B0`, `0x801E89F0`, `0x801E8A80`, and `0x801E8BD0`. Native callees consume the sound identifier in r4 or the float in f1. StopSE forwards to StopSound; SetPitch stores actor+76; GetPitch reads that field or returns 1.0f. The setter callees establish no scalar return value. These additions preserve the complete existing layout and virtual order.

The four-byte CPpuEngine target at +0x90 is a MapIndex. Current-map adapters copy the entire MapDesc index, and descriptor calls pass that word to IGameSession's existing MapIndex APIs. The former StageID interpretation required reinterpret_casts in CGameMap; replacing those with direct MapIndex copies preserves all instructions. StageID remains a separate type for its existing consumers. The target getter returns a value: write operations pass its snapshot by const reference to CreateMapDesc, whereas read operations pass it by value to GetMapDesc. This naturally reproduces the native local copies. Both target setters strictly match all 24 native bytes and their data reference.

StageDesc's eight-byte parameter owner at +4 is CPpuRegister. Native parameter operations call its Reserve, Set and GetValue members on StageDesc+4. Independent actual CGameSession producers for StageDesc's constructor at `0x800B2490` and destructor at `0x800B25B0` strictly match all 296 native instruction bytes and every target after replacing the former hypothetical DescParamTable constructor/destructor calls. The descriptive DescParamTable name remains an alias, preserving its public spelling.

CPpuEngine's InsertCallback accepts three ints, EraseCallback two ints, and both return an int. The former callee checks its third parameter against 0 and -1 before choosing list insertion positions. BWorld's IsCalcEnabled reads the inverse of its existing FLAG_CALC_DISABLED bit; this reproduces the whole IsCalcWorldEnabled binding, including its observed PushInt followed by PushNull behavior.

## Verification

Baseline main: `9ed64200c59b7e8e2eb6df98da84fe9356f5a9a9`.

- `python configure.py --map`: exit 0.
- `.venv/Scripts/ninja.exe all_source`: exit 0, all 763 current source objects compiled or current.
- Current-consumer audit: 763 objects; 27 raw object changes; every allocated section, alignment, payload and relocation identity unchanged after four explicitly evidenced name mappings. Two mappings rename the target setter parameter type; two replace hypothetical DescParamTable constructor/destructor identities with the strictly paired CPpuRegister calls. No other normalization is allowed.
- Native StageDesc producer audit: 2/2 functions, 296 bytes, all targets exact.
- Native target-setter audit: 2/2 functions, 24 bytes, all targets exact.
- Delete only the old main.dol, run `.venv/Scripts/ninja.exe`: exit 0.
- `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`: exit 0, frozen main.dol exact.

Reproducers and immutable raw controls are retained outside the checkout under `C:/MassStorage/GitHub/274-reference/`: `274-header-consumer-audit.py`, `274-header-controls/`, `274-stage-register-audit.py`, `274-stage-register-proof.json`, `274-target-setter-audit.py`, `274-target-setter-proof.json`, `274-audio-nine-strict.py`, and `274-audio-nine-eh.py`. No frozen hashes were changed.

## Remaining original scope

The actual complete producer currently matches 24/28 original instruction streams and all their relocations. Remaining differences are SetStageDescTarget (four register words), SetStageDescParams and SetMapDescParams (two arithmetic words each), and SetMapDescEnemyRow (register/lifetime context). This is outside evidence, not accepted source coverage. No original issue completion or additional source bytes are claimed.

## Traps

- A const-reference target getter removed the native local snapshot before CreateMapDesc. Returning the four-byte record by value restores the native dataflow without manual storage or retention mechanisms.
- Reusing an old outside header after another contributor's merge silently loses newly accepted declarations. Rebuild sparse overlays from current main and apply only the owned additive changes; this control retains the incoming CPiiPersonalData APIs.
- Literal-address binding must verify payload as well as the relocated instruction. The audio audit explicitly verifies StopBGM's 0.1f native bytes, rather than binding any source float to the expected address.
