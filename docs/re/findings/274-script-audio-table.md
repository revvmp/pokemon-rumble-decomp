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


# F-274-2: Complete audio binding module

The first nine original functions form the self-contained descriptive module `src/game/ScriptAudio.cpp`. No recovered filename is asserted. As with F-272-2, the survey bind_7 grouping is not an original source-boundary claim. This module owns native text `0x8015CA00–0x8015D144`, extab `0x80032790–0x800327D8`, extabindex `0x8004F86C–0x8004F8D8`, and the three float constants `0x804BA2E8–0x804BA2F4`. Its nine instruction bodies total1,804 bytes; text also contains56 native alignment bytes. There are no owned constructors, static initializers or hidden static state. Emitted weak class RTTI resolves to the earlier canonical definitions and is discarded from this module.

The source uses the original binding-table names and canonical complete headers. PopSoundID preserves the native int/string/reference branches and the observed sound-player lookup before popping a label. Dynamic casts obtain the existing BSoundSource mixin; absent owners preserve the native default pitch or no-op setter behavior. The single-argument ContinueGameBGM callee name and four BSoundSource method names are descriptive native API names, not recovered debug symbols. All outside symbols retain their original ranges and sizes.

Initial NonMatching split validation passed the full frozen executable. The actual registered source object has9/9 functions,1,804/1,804 code bytes,72EH bytes,108 exception-index bytes and12constant bytes at100% in unitdiff. Strict auditors apply every code relocation, verify literal payloads, and compare every exception record without masking. Matching source selection also passes the full frozen executable. A fresh mapped link independently proves every function's unique native address, exact size and `ScriptAudio.o` source column. The linker input query selects the source object and excludes its original fallback. All four retained allocated ranges, including padding and exception-index relocations, equal native bytes.

The original inferred eight-byte float symbol at804BA2F0 was refined to its evidenced four-byte payload; the following four bytes remain native alignment outside this module. No source padding array, executable-byte storage, flags change, retention directive or explicit instantiation is used.

Reproducers: `274-source-audio-audit.py`, `274-source-audio-eh.py`, and `274-source-audio-acceptance.py` under the retained outside `274-reference` directory. Official source landing repeats all_source, strict audits, a fresh mapped source-only link, and frozen verification after rebasing. The accepted artifact archive contains the source object, native receipts, ELF, DOL, MAP, source config, Ninja graph and hash manifest.

## Original issue remains partial

This accepts9/28 original functions and1,804/5,644 code bytes. The remaining19functions/3,840bytes belong to the table/callback/world module. Four of those bodies still have code differences; the other15 are strict outside evidence only. The issue must remain open and receives no whole-issue completion credit.

## Additional traps

- A source object's weak RTTI sections are not necessarily retained allocations. Check the actual mapped link before assigning ownership; this module's class RTTI is supplied by earlier canonical definitions.
- Wii/1.3 gives the same complete28-function instruction streams as Wii/1.1 for this producer. It does not resolve the four remaining table-binding gaps. Direct reverse-index reassociation and direct record-member construction were bounded negative controls; they are not adopted as register-retention workarounds.
