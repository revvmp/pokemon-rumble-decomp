# F-262-1: Input bindings and player factory checkpoint

## Scope and status

Original issue #262 owns 27 functions, 7,052 instruction bytes, from
80151130 through 80152D84. It is not complete. No source split, Matching
registration, ledger claim, or canonical header change is published by this
checkpoint. The survey's bind_6 file start is not a proven translation-unit
boundary.

The ordinary typed producer is retained outside the checkout at
`C:/MassStorage/GitHub/262-reference/ScriptInput.cpp`. Its reproducible generator
is `C:/MassStorage/GitHub/262-prepare-probe.py`; it creates sparse proposed
headers before invoking the standard game compiler. The final rebase control
uses main 32d30869ef5c52995465c5960f071bbec9d9f6c0.

## Complete original inventory

| Address | Bytes | Identity | Checkpoint result |
| --- | --- | --- | --- |
| 80151130 | 116 | method_mpResume | Strict code, targets and EH exact |
| 801511B0 | 116 | method_mpIsPausing | Strict code, targets and EH exact |
| 80151230 | 320 | GetPadDirValue | Strict code, targets and EH exact |
| 80151370 | 436 | GetPadDirDeg | Strict code, targets and EH exact |
| 80151530 | 376 | GetPadTrig | Strict code, targets and EH exact |
| 801516B0 | 364 | GetPadHold | Strict code, targets and EH exact |
| 80151820 | 376 | GetPadRelease | Strict code, targets and EH exact |
| 801519A0 | 204 | IsControllerNunchaku | Strict code, targets and EH exact |
| 80151A70 | 308 | RTGetPadTrig | Strict code, targets and EH exact |
| 80151BB0 | 308 | RTGetPadHold | Strict code, targets and EH exact |
| 80151CF0 | 308 | RTGetPadRelease | Strict code, targets and EH exact |
| 80151E30 | 308 | RTGetPadRepeat | Strict code, targets and EH exact |
| 80151F70 | 372 | StartRumble | Strict code, targets and EH exact |
| 801520F0 | 296 | StopRumble | Strict code, targets and EH exact |
| 80152220 | 628 | IsPadLowBattery | Strict code, targets and EH exact |
| 801524A0 | 412 | CheckRemoteMemoryFatalError | Strict code, targets and EH exact |
| 80152640 | 372 | GetButtonSymbol | Strict code, targets and EH exact |
| 801527C0 | 920 | NewPlayerPii | Typed source emits 1,428 bytes |
| 80152B60 | 20 | CPlayerPii::Initialize() | Strict code and targets exact |
| 80152B80 | 12 | CPlayerPii::LoadScript(environment) | Strict code and targets exact |
| 80152B90 | 168 | shared_ptr<CPlayerPii> raw-pointer constructor | Native helper; currently inlined |
| 80152C40 | 88 | enable_from_this specialization | Native helper; currently inlined |
| 80152CA0 | 60 | weak_ptr raw-pointer/control-block constructor | Native helper; currently inlined |
| 80152CE0 | 48 | move_ptr constructor | Native helper; currently inlined |
| 80152D10 | 48 | compressed-pair constructor layer | Native helper; currently inlined |
| 80152D40 | 64 | compressed-pair storage constructor | Native helper; currently inlined |
| 80152D80 | 4 | default deleter constructor | Native helper; currently inlined |

The first 17 functions total 5,620 code bytes and 136 EH bytes. The two
overloads add 32 exact instruction bytes. Remaining original scope is eight
functions and 1,400 instruction bytes; this includes the factory and seven
memory helpers. Actual linked data, EH indexing, whole-unit ownership, and
source-only provenance have not been accepted.

## Native contracts

Controller index 4 selects GetUIInput at 80124A40. Nonnegative other indices
select the player's assigned controller. Both paths require IsConnected.
The UI input is consumed through IController virtual slots, not an invented
private storage view.

Collected Hold reads CPlayer's accumulated mask at 6C. Collected Trig is
accumulated AND (last XOR accumulated); collected Release is last AND
(last XOR accumulated), with last at 70. The existing collection flag is
bit zero. The sparse header exposes read-only accessors to these real fields.
The RT functions always query the connected controller directly.

MotorPattern has the existing canonical 12-byte index/Wii/GC structure.
StartRumble addresses rows of the native 803550B8 table. Its external array
declaration is unsized and makes no claim to change the existing symbol extent.

CheckRemoteMemoryFatalError proves IGameSession's slot AC takes an
IController pointer. Its native 800BABD0/108 implementation searches the four
player controller pointers. The canonical IsRegisteredID(u32) declaration is
an earlier inference, superseded by this caller and implementation evidence.
The proposed name is IsControllerAssigned(const IController*). The corresponding
CGameSession declaration must change together in any bounded prerequisite.

The remote-memory flag read clears bit 80 and returns its previous state.
The complete sparse CRemoteMemory layout is inherited from the earlier #217
producer, but its constructor and member at offset 4 are corrected to COyaInfo*,
as proven by canonical CGameSession's actual constructor call. It is not CPiiBox*.
No canonical layout was changed here.

The factory consumes pending personal data, resets pending state when present,
otherwise selects ordinary personal data, and rejects data for an unjoined
player. An old Pii with the same personal-data owner supplies the existing ID.
Otherwise it constructs CPlayerPii, exchanges script registers, initializes it,
inherits old position/rotation or positions joined players with 0.7 spacing,
uses 180 degrees without an old Pii, and raises fliers to 0.3. Adding it to the
world precedes loading its script. IsPiiSpeciesFlier is a descriptive outside
name: native 800AE560 calls GetFlyHeight at 80124F40 and compares against zero.
The two retained overloads delegate to Initialize(false) and LoadScript(0, env).

## Reproduction and receipts

Run the outside scripts in order: `262-prepare-probe.py`, `262-audit.py`,
`262-prepare-audit.py`, `262-native-positive.py`, and `262-audit-overloads.py`.
The opcode report is diagnostic only. The positive script applies actual
relocation target values and compares complete instructions and EH records
against the frozen executable; it does not mask relocation words. Reports are
`262-reference/native-proof.json` and `262-reference/overload-proof.json`.
`native-disasm.txt`, `helper-disasm.txt`, `command.json`, and sparse headers
retain the inputs. The factory reading aid is `262-new-player-decompile.json`;
the producer was rewritten as ordinary class-based C++.

## Traps

- Extracting a bool-returning battery predicate emits 656 rather than native
  628 bytes. Updating the final result directly in each native branch restores
  the complete stream. Both WPADInfo objects have independent source scopes.
- GetButtonSymbol's button is u32, matching the existing IController API.
  Declaring it int leaves 13 register-allocation word differences; u32 removes
  every difference without changing the algorithm.
- The stage-camera helper performs a checked DynamicCast and rejects a null
  converted result before returning. Collapsing it to a conditional expression
  removes eight native bytes. The explicit ordinary checks match all 436 bytes.
- Keeping the remote-memory call nested as GetRemoteMemory(controller->GetChannel())
  preserves the actual pointer-call context; materializing an extra channel
  local leaves two register differences.
- The remaining factory natively calls tiny GetId, GetRegister, VEC3 default,
  shared_ptr dereference and construction layers. Current headers inline these
  into the 1,428-byte body. Wii/1.3 gives the same factory size and first-stop
  differences as Wii/1.1. Original outlining/declaration context is missing.
  The inherited CPlayer source pragma is not independent evidence for a script
  pragma; no no-inline pragma or compiler-flag workaround was adopted.


## F-262-2: Current-header checkpoint and player factory identification

The user ended the session before whole-source integration. Issue #262 remains
incomplete. No new production source, split, Matching registration, or canonical
header change is made by this checkpoint. PR #1240 subsequently reconstructed
the first two motion functions in the original inventory.

Current canonical headers retain strict native equality for all first seventeen
functions, 5,620 instruction bytes and 136 exception bytes, including actual
call targets. Reproduction is `262-current-primary-control.py` followed by
`262-current-audit-control.py`; receipt is
`262-reference/current-primary/native-proof.json`. The corrected remote contract
also passes `262-remote-contract-pair.py` independently.

The factory-only standard header control and full original 2006 MSL control both
emit 1,428 bytes. The bounded size-optimization diagnostic emits 1,176 bytes;
none establishes a production flag change. A diagnostic `dont_inline` source
policy after includes retains the native helper calls. Nested comparison scope,
explicit assignments in both vertical-position branches, and centering written
as division produce the native 920-byte instruction layout. Reproduction is
`262-centering-division-control.py`; `opcode-diagnostic.json` records zero
non-relocation instruction differences. This is not full target/EH/data/link
acceptance: `262-player-strict.py` still stops on an unresolved later owned
move_ptr destructor. No masked comparison is represented as a strict pass.

Shared float data and control-block vtables prove the larger ScriptPii producer
starts at 801527C0 and extends through 80155FD8, rather than separate factory
units. The final tail belongs to original issue #268 as well as #263–#267.
Cooperating #263 and #264 producers retain independent strict code/target/EH
receipts. They must be combined into one real producer before source selection
or original issue closure. No shared source was integrated before wrapping.

The immutable local archive is `C:/MassStorage/GitHub/262-wrap-checkpoint-20261021.zip`.
Its SHA-256 is `61419dd224bebd84373931a8c59ce384b02f3533be52560331fe5592efe20666`. It includes all current and prior reference inputs,
objects, receipts, sparse header adapters, and issue-prefixed reproduction
scripts. Earlier checkpoint branch `codex/262-prior-cc5f36a8` retains the original
committed findings; its old reference proof was restored and checked against
the original archive manifest after a current-header replay.

### Additional traps

- A compound old-Pii and temporary shared-owner comparison extends temporary
  lifetimes across the outer condition, producing extra construction flags.
  Nested ordinary if statements reproduce the native lifetime structure.
- Multiplication by 0.5 gives the same mathematical center but a different
  floating multiply operand order here. The ordinary division expression below
  reproduces the native compiler transformation; this finding is local to this
  factory and does not establish a global arithmetic rewrite rule.
- Source symbol-table order is not physical order. Inspect offsets when checking
  emitted helper placement, and require the complete linked source unit.

### Preserved diagnostic producer

This is a candidate source context, not accepted reconstructed source. The
diagnostic source pragma still requires whole-unit native evidence.

```cpp
#include <game/CPpuEngine.h>
#include <game/CPlayerPii.h>
#include <game/CPiiPersonalData.h>
#include <game/CPlayer.h>
#include <game/CRemoteMemory.h>
#include <game/IGameSession.h>
#include <game/CStageCamera.h>
#include <game/BWorld.h>
#include <game/BWorldLightSet.h>
#include <game/CNullObj.h>
#include <game/ControllerTable.h>
#include <game/MotorPattern.h>
#include <revolution/wpad.h>

IController* GetUIInput();
extern const MotorPattern gMotorPatterns[];

#pragma dont_inline on

bool IsPiiSpeciesFlier(const PiiSpeciesData* pSpecies);

int NewPlayerPii(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    int player = pEngine->PopInt();
    u32 id = 0;
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
    std::tr1::shared_ptr<CPiiPersonalData> data = pPlayer->GetPendingPersonalData();
    if (data) {
        pPlayer->ResetPendingPersonalData();
    } else {
        data = pPlayer->GetPersonalData();
    }
    if (!pPlayer->IsJoined()) {
        data.reset();
    }
    if (data) {
        CPlayerPii* pOldPii = pPlayer->GetPii();
        if (pOldPii) {
            if (pOldPii->GetPersonalData() == data) {
                id = pOldPii->GetId();
            }
        }
        if (!id) {
            std::tr1::shared_ptr<CPlayerPii> pii(new CPlayerPii(pPlayer, data, CPlayerPii::TYPE));
            pii->SwapRegister(pEngine->GetRegister());
            pii->Initialize();
            nw4r::math::VEC3 position;
            float rotation;
            if (pOldPii) {
                position = pOldPii->GetPositionRef();
                rotation = pOldPii->GetRotation();
            } else {
                position = nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
                rotation = 180.0f;
                int count = 0;
                int ordinal = 0;
                for (int i = 0; i < 4; ++i) {
                    if (IGameSession::GetInstance()->GetPlayer(i)->IsJoined()) {
                        if (i == player) {
                            ordinal = count;
                        }
                        ++count;
                    }
                }
                position.x += 0.7f * (-(count - 1) / 2.0f + ordinal);
            }
            if (IsPiiSpeciesFlier(data->GetSpeciesData())) {
                position.y = 0.3f;
            } else {
                position.y = 0.0f;
            }
            pii->SetPosition(position);
            pii->SetRotation(rotation);
            if (pEngine->GetWorld()->Add(pii)) {
                pii->LoadScript(std::tr1::shared_ptr<NPpu::CEnvironment>());
                id = pii->GetId();
            }
        }
    }
    pEngine->PushInt(id);
    return 0;
}

void CPlayerPii::Initialize() {
    Initialize(false);
}

bool CPlayerPii::LoadScript(const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment) {
    return LoadScript(0, rEnvironment);
}
```
