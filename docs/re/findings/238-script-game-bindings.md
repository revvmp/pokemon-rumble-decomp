# Game and event script bindings

## F-238-1: Independent Game producer

The retained function `LoadEventData` at `0x80138BA0` is inside original issue #238 and begins an independent Game/event script unit. Its last retained function ends at `0x80140C40`, where the next unit's `AddItem` begins. Original issues #239â€“#244 continue this unit. Issue #238 also contains preceding camera and effect functions; reconstructing the Game unit alone does not complete #238.

The distinction follows source-owned data rather than survey labels. The camera pool at `0x804BA1E8` contains its signed-integer conversion bias at `0x804BA1F8`; the effect conversion factor is at `0x804BA200`. The Game timer pool independently contains `60.0f` at `0x804BA208` and another signed conversion bias at `0x804BA210`. Combining these producers coalesces the duplicate bias and fails a consistent small-data section-base comparison. Separate compilation preserves the Game pool's sixteen bytes.

The first loader uses `5:/event/` at `0x803B66C8`, `/event.brres` at `0x803B66D4`, and `.arc/event.brres` at `0x803B66E4`. These immediately precede the `shared_ptr_deleter<CSimpleGround, default_delete<CSimpleGround>>` vtable at `0x803B66F8`. Its type-name and base information continue through `0x803B67A7`; the next unit's exception string begins at `0x803B67A8`.

The actual `NewSimpleGround` producer reproduces its 840 code bytes, every call target, and 376 exception bytes. It also emits the native tail helpers at `0x80140A70` (76 bytes), `0x80140AC0` (36), `0x80140AF0` (132), `0x80140B80` (116), and `0x80140C00` (64). The 116-byte helper is the destructor of `Metrowerks::move_ptr<CSimpleGround, default_delete<CSimpleGround>>`, not a weak-pointer destructor: the actual typed temporary in the shared-pointer construction and its cleanup at exception-record offset `0xAC` establish the identity.

## F-238-2: Current outside controls

The producer is preserved outside the repository in `C:/MassStorage/GitHub/238-reference/238-ScriptGame.cpp`. Its command uses the canonical Wii/1.1 game flags and bounded header overlays. `238-native-positive.py` resolves complete branch, address, and small-data relocation operands and compares full exception records with typed cleanup targets. Unknown targets are rejected. Code-only diagnostics are separate from strict acceptance.

At this checkpoint, 156 retained functions totaling 26,648 code bytes and 1,936 exception bytes pass those checks. The immutable source, object, headers, compiler arguments, auditors, receipts, and SHA-256 manifest are preserved in `C:/MassStorage/GitHub/238-reference/238-game-156-strict-checkpoint`. This is partial producer evidence, not a source-selected unit, frozen-DOL acceptance, or issue completion. Source order, complete data contributions, all original functions, final linked provenance, and latest-main verification remain required.

The 852-byte event loader and 460-byte placement loader use the native archive paths above. The retained eight-byte forwarder at `0x80138F00` passes zero flags to `CPpuEngine::LoadFile` at `0x800FFAE0`; its descriptive outside name is `LoadEventFile`. The actual callee consumes the flags argument in `r6`, so the current one-argument declaration is incomplete.

The message conversion helper uses a named result and one return. It reproduces the actual message, yes/no, tutorial, and registered-balloon callers while preserving the private 1,024-element wide-character buffer. Invalid input returns the native wide literal `L"???"`. The standard scoped `CEngine::PopWString` reproduces the dialogue title, text, and three-string line bindings without changing the VM implementation.

## F-238-3: Battle display interface correction

The vtable at `0x8039A5A4` identifies three concrete methods:

| Slot | Target | Proven behavior | Inferred descriptive name |
| --- | --- | --- | --- |
| `0x28` | `0x8006B6E0` | Store integer argument at member `0x124` | `SetEnemyCount(int)` |
| `0x2C` | `0x8006B6C0` | Decrement that integer only when positive | `DecEnemyCount()` |
| `0x30` | `0x8006B6B0` | Return that integer | `GetEnemyCount()` |

The original script binding names independently establish enemy-count semantics. The current `SetRound`, `GetRoundIndex`, and `GetRound` names were earlier inferences. The concrete decrement has no return-value production. The three actual script callers reproduce all code and exception bytes with the corrected outside interface. `CPauseMenu` currently calls `GetRound`; any canonical correction must update that consumer and preserve every current source object before landing.

## Traps

- A survey issue boundary is not necessarily a translation-unit boundary. Separate literal pools with identical conversion biases expose a false combined producer even when many functions match independently. Preserve the shared original-inventory evidence in F-237-5 and check each section before publishing a split.
- The final 116-byte cleanup is a typed move-pointer destructor, despite its resemblance to other pointer cleanups. Establish cleanup identity from the actual source temporary and exception action, not body size or identical instructions.
- `OpenNumericInputScreen` compares the fetched argument count with signed `cmpwi`. Declaring its local count unsigned changes that instruction; the native 292-byte body matches with `int`.
- The accepted matrix deserializer in `ScriptUtilities` does not prove the source form of the inverse matrix serializer here. `GetEventPlacementMtx` remains four bytes short; no inference from that inverse consumer is accepted.
- Naming bytes inside an existing reserved array requires checking pointer arithmetic into that array. The first status-field control changed `CTitleSession` because its existing `mReserved + 4` points to the wireless MAC address. Replacing that expression with `mWirelessMacAddress` preserves the original address; the same bytes also seed the lucky-number binding. A frozen-DOL pass alone did not detect this NonMatching source-consumer regression.

## F-238-4: Complete statistic storage contract

The actual factory at `0x80117020` allocates 92 bytes and calls the constructor at `0x80113540`. Three saved-data vector imports target members at 0, 24, and 48. The export at `0x80116260` assigns those vectors to the existing `vector<SD_StatisticValue::Data>` output parameters. The snapshot method at `0x80114210` copies 0 to 12, 24 to 36, and 48 to 60, then copies the saved counters at offsets 4824 and 4828 into members 72 and 76. A `map<int, int>` begins at 80; the counter-stop setter, lookup, node key/value accesses, and default constructor establish its identity.

The complete outside class therefore contains six vectors of four-byte statistic records, two `u32` snapshot counters, and the twelve-byte integer map. A natural empty `CStatistic::CStatistic()` body reproduces every one of the native constructor's 96 bytes with no relocations. `238-statistic-contract-proof.json` preserves the allocation, member, and constructor controls. No unknown storage field was added. Seven actual Game statistic callers also match strictly with this contract. Names of the storage members and formerly unnamed methods remain descriptive inferences.

The existing `SlotData::Data` contains `s64`, so the complete `SlotData` places `mData` at offset eight. This explains the native signed-byte maximum-grade/difficulty accesses at 20 and 21 without changing its header size. The existing twenty-byte status union contains registry and elapsed counters at 4840 and 4844, ticket species and form at 4848 and 4850, one unresolved byte at 4851, six wireless MAC address bytes at 4852, and signed extra-battle-round maximum and current values at 4858 and 4859. All eight paired grade/difficulty/round getters and setters and three ticket bindings match strictly with the unchanged extent.

The actual lucky-number binding uses unsigned 64-bit division and remainder helpers. Its call to `GetCurrentSlot` consumes the full signed return word without character truncation. Correcting the outside return declaration from `u8` to `int` reproduces all 488 code bytes, fourteen calls, and eight exception bytes. The canonical definition must be corrected with the declaration; this pending contract has been coordinated with the owner of issue #277.

## F-238-5: Typed species notice consumer

The 200-byte `AddEnemyToNoticeDisplay` binding calls `CPpuEngine::PopSpeciesData` at `0x80100F10`, then dispatches notice-interface slot `0x28` with the resulting `const PiiSpeciesData*` and an integer argument. The species identity is independently established by the property bindings: their consumers pass the same popped pointer to `PiiSpeciesData::GetSpecies` at `0x80124F10`. The notice target at `0x800D0420` is empty, so its body alone cannot establish the argument types.

The actual binding matches every code byte, all three resolved calls, and eight exception bytes with `AddEnemy(const PiiSpeciesData*, int)`. That method name is inferred from the original script binding. The earlier notice-interface name `Clear()` was an inference and is inconsistent with the typed caller. Issue #270 owns the additive species-data and `CPpuEngine::PopSpeciesData` prerequisite; this independent consumer is supplied as a control rather than duplicated publication.
