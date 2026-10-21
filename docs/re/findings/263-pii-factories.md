# Issue 263: Pii factory source and declaration context

## F-263-1: original inventory and cooperative boundary

The original issue owns31 functions/2776 instruction bytes in80152D90ÃƒÂ¢Ã¢â€šÂ¬Ã¢â‚¬Å“80153924. Native52D90/84,52DF0/76 and52E40/4 continue the preceding NewPlayerPii control-block/owner helper chain, whose first ordinary factory527C0 belongs to262. Their implementation, data and source split remain with the first owner262. The new ordinary factory52E50/644 constructs CPii or CDropPii and is followed by its ownership helpers and the two8-byte NewPii/NewDropPii script forwarders. NewEnemyPii53470/504 constructs CEnemyPii or CBossPii and has additional helper functions beyond this issue, including53930 and53A20. These are observed code groups, not accepted complete translation-unit boundaries; a full data/exception/source graph is still required. No split or source selection was published from this investigation.

The script table independently supplies NewPii, NewDropPii and NewEnemyPii. The internal CreatePii spelling is descriptive. A coherent outside producer contains the ordinary CPii/drop factory, both original forwarders and the Enemy factory, using the complete canonical CPii, CDropPii, CEnemyPii, CBossPii, CPpuEngine, CPiiPersonalData and BWorld types. Its use of shared_ptr is supported by the native two-word default stores and reset's raw-owner temporary, swap, destruction and reference-counter release. It does not substitute a move_ptr or an opaque owner.

## F-263-2: native caller contracts

Both factories obtain personal data from CPpuEngine::PopPersonalData, construct the observed derived Pii type, swap the engine register into BWorldObj, initialize the object, register it with the world and push its ID or zero. The generic factory additionally handles an optional parent and script argument. NewEnemyPii selects CBossPii when personal-data FLAG_BOSS is set; the native accessor53670 tails HasFlag with80000000. The flying-species path sets position to(0.0f,0.3f,0.0f). The literals resolve through the actual SDA2 base804C1C60 to804BA2B0 and804BA2C0; guessing the SDA2 base gives unrelated values.

Two missing typed interfaces were used as outside declaration probes only. Native80098750/20 invokes CPii virtual slot80 with false; the descriptive no-argument Initialize overload expresses this lifecycle wrapper. Native800AE560/44 returns GetFlyHeight()!=0; the descriptive species IsFlying query expresses that observed behavior. Neither declaration nor an implementation was added canonically, and neither inferred spelling is claimed recovered. Exact constructor/control-block RTTI and all helper bindings remain to be established before source acceptance.

## F-263-3: bounded identification controls

| Actual ordinary producer | Native | Canonical Wii1.1/O4,p | Wii1.3/O4,p | Genuine2006 MSL | Wii1.1/O4,s | Wii1.1/auto,deferred |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| CreatePii | 644 | 1332 | 1332 | 1332 | 1000 | 1332 |
| NewPii | 8 | 8 | 8 | 8 | 8 | 8 |
| NewDropPii | 8 | 8 | 8 | 8 | 8 | 8 |
| NewEnemyPii | 504 | 1120 | 1120 | 1120 | 796 | 1120 |

The full unadapted2006 MSL oracle uses only the two missing game declarations before the original four include directories. No adapted MSL header precedes them. Wii1.1/1.3 actual ordinary instruction bodies are identical. The two other identification controls are exactly the unresolved hypotheses recorded by F-735-1; neither is adopted. No inline threshold, attribute or custom compiler switch was introduced in those controls. F-263-4 separately records a pure inlining-policy identification; it is not an adopted source or compiler setting.

The native callers retain small engine, personal-data, smart-owner and game-accessor calls that the present public header definitions inline. Combining the two actual factories does not restore that context. The new accepted shared-owner definition contexts and the genuine MSL bundle do not alone recover the native caller extents. The larger baseline candidates are not accepted source. F-263-4 identifies a further whole-producer discriminator requiring complete source-unit proof before any adoption. Matching only the two forwarders would not complete the original issue.

Outside evidence is retained in263-reference:263-enemy.cpp, its sparse complete game declarations,263-compile.py,263-primary.py, bounded compiler-command variants and actual objects.263-compiler-identification.json preserves complete native/candidate ordinary instruction bytes, all symbolic relocation records and complete exception bytes/cleanup references for each control. The original31-function issue inventory remains263-inventory.txt (also retained under347-reference from the initial read-only screen). The direct native Enemy and first-factory disassemblies are reading aids, not source.

## Traps

- A two-word owner plus release() cleanup is a shared owner, even when its separate default constructor resembles an empty allocation guard. Trace reset's allocation and swap before choosing its source type.
- Native direct calls to tiny accessors do not justify an invented no-inline pragma or moving declarations solely for emission. A full ordinary producer and genuine-library/compiler controls must distinguish missing source context first.
- Include BWorld after the sound/Pii headers in this producer. The opposite order exposes an existing RoundUp macro collision with nw4r::ut::RoundUp; no duplicate SDK header or macro workaround is needed.
- Use the actual linked SDA2 anchor for literal identity. The factory's native0.3f literal would be misread as-1.0f with a guessed804C2000 anchor.


## F-263-4: inlining-policy identification, not source acceptance

A single outside policy-identification producer uses the existing CodeWarrior dont_inline pragma after its game includes, with the ordinary game flags unchanged. Its first extents were652/508 rather than644/504. The native calls independently establish two missing mode-zero public wrappers: CPii::LoadScript(name,environment) and CEnemyPii::LoadScript(environment), each12 bytes forwarding to the existing three-/two-argument method. Typed outside definitions recover those calls; explicit parent=null followed by the optional assignment removes the ternary's additional branch. The complete ordinary stream sizes are then644,8,8 and504, and every nonrelocation instruction agrees with the native four functions.

Following each actual relative call through the retained helper graph independently identifies45 emitted functions/3552 instruction bytes in80152E50Ã¢â‚¬â€œ80153D48, all with zero nonrelocation differences and consistent direct-call targets. This includes the observed constructor, weak-owner attachment, compressed-pair and raw-reset helper layers, rather than only the four ordinary bodies. This remains a code diagnostic: complete literal/vtable/RTTI, semantic EH relocation and final source-selected proofs are pending, and the exact containing translation-unit boundary is not yet established with262.

The producer also emits unused local copies of earlier borrowed helpers. In particular, its76-byte personal-data copy constructor resolves to native80089680, the earlier40-byte implementation; these are not newly claimed source. A scoped reset-off policy control inlines owned reset helpers incorrectly while the four ordinary bodies remain native-sized, demonstrating why ordinary caller bytes alone cannot identify the full template context. No policy variant is adopted in production. The outside fixtures263-factories-policy.cpp,263-factories-policy-positive.o,263-policy-graph.py and263-policy-graph.json retain the positive diagnostic and the source-to-native call graph.262's independently reconstructed Player factory is being checked under the same policy before drawing any source-unit conclusion.


## F-263-5: shared whole-source boundary

The first-owner262 investigation independently established Player's native zero and0.3 literals use the identical804BA2B0/804BA2C0 objects. Player's control-block vtable803B71A4 refers to virtual methods155E00,155510 and155560. The three constructors reached from this issue similarly establish Drop vtable803B712C with methods155470/1554C0/155E60, Enemy vtable803B70B0 with1553D0/155420/155EC0 and Boss vtable803B7038 with155330/155380/155F20. Their common retained late helper cluster and the genuinely shared literal pool supersede the tentative factory subdivisions in F-263-1.

The containing ScriptPii.cpp source begins with NewPlayerPii at801527C0 in262 and ends80155FD8, before method_CPii_play at80155FE0.262 owns its source/split/header integration;263 supplies its factory bodies and typed native helper evidence, while the subsequent original block owners supply their ordinary Pii methods. There is no263 factory split. The clean fragment263-factories-fragment.cpp contains the ordinary source without any inlining-policy directive; the directive remains confined to the explicitly diagnostic producer until the complete containing unit passes its acceptance gates.


## Checkpoint and release

New work stopped at the user's wrap instruction. No canonical source, header, split or source-selection change is published. The original263 issue remains incomplete: its28 factory/helper functions have strict unlinked producer evidence, and its first three Player helpers belong to the first-owner262 integration. The whole ScriptPii translation unit still requires the remaining ordinary methods and actual final ELF/MAP/DOL/source-selection acceptance. The claim is released normally because that remaining work belongs to the containing source-unit effort; this checkpoint does not label it a missing-reference blocker.

The strict producer resolves all45 instructions streams/3552 bytes with every relocation, float payload and reachable control-block RTTI/vtable object. All31 exception records/684 bytes and cleanup pointers agree. Within the original263 inventory this is28 functions/2612 bytes and20 exception records/580 bytes. Seventeen naturally emitted264 helpers contribute940 additional code bytes and104 exception bytes. These are diagnostic counts, not new decompiled/source-selected coverage. There is no compiler-policy adoption: the directive appears only in the outside identification fixture. The clean source below intentionally has no such directive.

### Clean source fragment for the containing unit

The following ordinary source is retained for integration by262. It requires the additive declarations shown below and must be revalidated in the whole actual ScriptPii source context. Its public callback signature follows NPpu::NativeFunc; the game-engine downcast preserves the actual zero-offset base relation.

```cpp
#include <revolution/types.h>
#include <game/CPpuEngine.h>
#include <game/CPiiPersonalData.h>
#include <game/PiiSpeciesData.h>
#include <game/CEnemyPii.h>
#include <game/CBossPii.h>
#include <game/CDropPii.h>
#include <game/BWorld.h>

int CreatePii(CPpuEngine* pEngine, bool drop) {
    int count = pEngine->FetchArgCount();
    BWorldObj* pParent = nullptr;
    if (count >= 3) {
        pParent = pEngine->PopWorldObj();
    }
    const char* pScript = nullptr;
    if (count >= 2) {
        NPpu::CValue argument;
        pEngine->Pop(argument);
        if (argument.IsString()) {
            pScript = pEngine->ToString(argument);
        }
    }
    std::tr1::shared_ptr<CPiiPersonalData> personalData = pEngine->PopPersonalData();
    std::tr1::shared_ptr<CPii> pii;
    if (drop) {
        pii.reset(new CDropPii(personalData, CDropPii::TYPE));
    } else {
        pii.reset(new CPii(personalData, CPii::TYPE));
    }
    pii->SwapRegister(pEngine->GetRegister());
    pii->Initialize();
    if (pParent != nullptr) {
        pii->SetParent(pParent->shared_from_this());
    }
    if (pEngine->GetWorld()->Add(pii)) {
        if (personalData->GetSpeciesData()->IsFlying()) {
            pii->SetPosition(nw4r::math::VEC3(0.0f, 0.3f, 0.0f));
        }
        if (pScript != nullptr) {
            pii->LoadScript(pScript, std::tr1::shared_ptr<NPpu::CEnvironment>());
        }
        pEngine->PushInt(pii->GetId());
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int ScriptBinding_NewPii(NPpu::CEngine* pEngine) { return CreatePii(static_cast<CPpuEngine*>(pEngine), false); }
int ScriptBinding_NewDropPii(NPpu::CEngine* pEngine) { return CreatePii(static_cast<CPpuEngine*>(pEngine), true); }

int ScriptBinding_NewEnemyPii(NPpu::CEngine* pState) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pState);
    std::tr1::shared_ptr<CPiiPersonalData> personalData = pEngine->PopPersonalData();
    std::tr1::shared_ptr<CEnemyPii> pii;
    if (personalData->IsBoss()) {
        pii.reset(new CBossPii(personalData, CBossPii::TYPE));
    } else {
        pii.reset(new CEnemyPii(personalData, CEnemyPii::TYPE));
    }
    pii->SwapRegister(pEngine->GetRegister());
    pii->CPii::Initialize();
    if (pEngine->GetWorld()->Add(pii)) {
        if (personalData->GetSpeciesData()->IsFlying()) {
            pii->SetPosition(nw4r::math::VEC3(0.0f, 0.3f, 0.0f));
        }
        pii->LoadScript(std::tr1::shared_ptr<NPpu::CEnvironment>());
        pEngine->PushInt(pii->GetId());
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}
```

### Outside header contract proposals

These are additive declaration/ordinary inline-wrapper proposals, not canonical edits. All existing class storage and virtual slots remain those of the current complete headers. Initialize and IsFlying are descriptive inferred names. The two mode-zero wrappers have direct native caller/callee evidence; their bodies produce the respective12-byte native forwarding entries.

```diff
--- include/game/CPii.h
+++ outside/include/game/CPii.h
@@ -52,6 +52,7 @@
                               const nw4r::math::VEC3& rVelocity);
     virtual void SetBaseMtx(const nw4r::math::MTX34& rMtx);
 
+    void Initialize();
     virtual void Initialize(bool background);
     virtual const char* GetShadowName() const;
     virtual bool IsVisible() const;
@@ -63,6 +64,7 @@
     }
     void SetHPBar(std::tr1::shared_ptr<CHPBar> hpBar);
     void SetMiniMapMarker(std::tr1::shared_ptr<CMiniMapMarker> marker);
+    bool LoadScript(const char* pName, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment);
     bool LoadScript(const char* pName, int mode,
                     const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment);
 
@@ -81,4 +83,6 @@
     std::vector<std::tr1::shared_ptr<CFileData> > mShadowFiles;
 };
 
+inline bool CPii::LoadScript(const char* pName, const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment) { return LoadScript(pName, 0, rEnvironment); }
+
 #endif
```

```diff
--- include/game/CEnemyPii.h
+++ outside/include/game/CEnemyPii.h
@@ -12,8 +12,11 @@
 
     virtual void Initialize(bool background);
 
+    bool LoadScript(const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment);
     bool LoadScript(int mode,
                     const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment);
 };
 
+inline bool CEnemyPii::LoadScript(const std::tr1::shared_ptr<NPpu::CEnvironment>& rEnvironment) { return LoadScript(0, rEnvironment); }
+
 #endif
```

```diff
--- include/game/PiiSpeciesData.h
+++ outside/include/game/PiiSpeciesData.h
@@ -11,6 +11,7 @@
     int GetFormNumber() const;
     int GetBoneType() const;
     int GetFlyHeight() const;
+    bool IsFlying() const;
     int GetASize() const;
     u32 GetGenderAndTypeFlags() const;
     f32 GetWalkSpeedCoeff() const;
```

### Reproduction and immutable receipts

The outside archive263-reference/checkpoint contains the clean and policy-identification sources, sparse header adapters, exact compiler commands, actual objects, original issue inventory, code/target/data/exception receipts and a SHA-256 manifest. The canonical input head and all include hashes are retained in263-native-signature-positive/input-manifest.json. Native reference bytes remain the frozen DOL; no reference hash was changed.

Run263-compile-policy.py against the captured fixture, then263-native-signature-graph.py and263-strict.py for the positive unlinked context.263-compiler-identification.json retains the earlier failed library/compiler controls;263-original-coverage.json separates the original issue from the subsequent helper continuation. No guessed primitive padding, copied decompiler source, assembly, byte forcing or synthetic emission driver is present.
