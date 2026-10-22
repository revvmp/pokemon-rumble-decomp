#include <game/CPpuEngine.h>
#include <game/CPlayerPii.h>
#include <game/CPiiPersonalData.h>
#include <game/PiiSpeciesData.h>
#include <game/CPlayer.h>
#include <game/IGameSession.h>
#include <game/CEnemyPii.h>
#include <game/CBossPii.h>
#include <game/CDropPii.h>
#include <game/BWorld.h>
#include <game/CHPBar.h>
#include <game/CModel.h>
#include <game/CPiiInfoWorldObjStat.h>
#include <game/IGameMap.h>
#include <game/IWorldObjStatDisplay.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <list>
#include <vector>
#include <algorithm>

int GetStrongestBP();

#pragma dont_inline on

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
            if (data->GetSpeciesData()->IsFlying()) {
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

int NewPii(NPpu::CEngine* pEngine) { return CreatePii(static_cast<CPpuEngine*>(pEngine), false); }
int NewDropPii(NPpu::CEngine* pEngine) { return CreatePii(static_cast<CPpuEngine*>(pEngine), true); }

int NewEnemyPii(NPpu::CEngine* pState) {
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

int method_faceHeadForward(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) pPii->FaceHeadForward();
    pBase->PushNull();
    return 0;
}
int method_faceHeadTowardZX(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    float z = pBase->PopFloat();
    float x = pBase->PopFloat();
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) pPii->FaceHeadToward(nw4r::math::VEC3(z, pPii->GetPosition().y, x));
    pBase->PushNull();
    return 0;
}
int method_faceHeadTowardXYZ(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    float z = pBase->PopFloat();
    float y = pBase->PopFloat();
    float x = pBase->PopFloat();
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) pPii->FaceHeadToward(nw4r::math::VEC3(x, y, z));
    pBase->PushNull();
    return 0;
}
int method_faceHeadTowardObj(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    BWorldObj* pTarget = pEngine->PopWorldObj();
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) pPii->FaceHeadToward(pTarget);
    pBase->PushNull();
    return 0;
}
int method_faceHeadTowardObjNode(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    const char* pNode = pBase->PopString();
    BModelObj* pTarget = DynamicCast<BModelObj>(pEngine->PopWorldObj());
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) pPii->FaceHeadToward(pTarget, pNode);
    pBase->PushNull();
    return 0;
}
int method_getPlayerIndex(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    BWorldObj* pObj = pEngine->PopWorldObj();
    int index = -1;
    if (pObj) {
        CPlayerPii* pPii = DynamicCast<CPlayerPii>(pObj);
        if (pPii) index = pPii->GetPlayer()->GetPlayerNo();
    }
    pEngine->PushInt(index);
    return 0;
}

typedef int (*FoundFunc)(NPpu::CEngine*, NPpu::Slice*, int, std::list<BWorldObj*>&);
typedef int (*NoneFunc)(NPpu::CEngine*, NPpu::Slice*, int);

static int PushObjectIds(NPpu::CEngine* pEngine, NPpu::Slice* pSlices, int capacity, std::list<BWorldObj*>& rObjects) {
    int i = 0;
    int count = std::min(capacity - 1, static_cast<int>(rObjects.size()));
    const std::list<BWorldObj*>& rList = rObjects;
    std::list<BWorldObj*>::const_iterator it = rList.begin();
    for (; i < count; ++it, ++i) {
        pSlices[i].SetInt((*it)->GetId());
    }
    if (count >= 0) {
        pSlices[count].SetInt(0);
    }
    pEngine->PushInt(count);
    return 0;
}

static int PushNoObjects(NPpu::CEngine* pEngine, NPpu::Slice* pSlices, int capacity) {
    if (capacity > 0) {
        pSlices[0].SetInt(0);
    }
    pEngine->PushInt(0);
    return 0;
}

static int SearchByGroupNo(NPpu::CEngine* pEngine, FoundFunc pFound, NoneFunc pNone);

int method_findObjByGroupNo(NPpu::CEngine* pEngine) {
    return SearchByGroupNo(pEngine, PushObjectIds, PushNoObjects);
}

static int SearchByGroupNo(NPpu::CEngine* pBase, FoundFunc pFound, NoneFunc pNone) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    int capacity = 0;
    NPpu::Slice* pSlices = pBase->PopArray(&capacity);
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) {
        std::vector<BWorldObj*> objects = pEngine->GetWorld()->GetObjects(0x100);
        std::list<BWorldObj*> found;
        int groupNo = pPii->GetPersonalData()->GetGroupNo();
        for (std::vector<BWorldObj*>::iterator it = objects.begin(); it != objects.end(); ++it) {
            if (DynamicCast<CPii>(*it)->GetPersonalData()->GetGroupNo() == groupNo) {
                found.push_back(*it);
            }
        }
        return pFound(pEngine, pSlices, capacity, found);
    }
    return pNone(pEngine, pSlices, capacity);
}

int FindEnemyByGroupNo(NPpu::CEngine* pBase) {
    int groupNo = pBase->PopInt();
    int capacity = 0;
    NPpu::Slice* pSlices = pBase->PopArray(&capacity);
    std::vector<BWorldObj*> objects = static_cast<CPpuEngine*>(pBase)->GetWorld()->GetObjects(0x100);
    std::list<BWorldObj*> found;
    for (std::vector<BWorldObj*>::iterator it = objects.begin(); it != objects.end(); ++it) {
        if (DynamicCast<CPii>(*it)->GetPersonalData()->GetGroupNo() == groupNo) {
            found.push_back(*it);
        }
    }
    return PushObjectIds(pBase, pSlices, capacity, found);
}

int CountEnemyByGroupNo(NPpu::CEngine* pBase) {
    int groupNo = pBase->PopInt();
    std::vector<BWorldObj*> objects = static_cast<CPpuEngine*>(pBase)->GetWorld()->GetObjects(0x100);
    int count = 0;
    for (std::vector<BWorldObj*>::iterator it = objects.begin(); it != objects.end(); ++it) {
        if (DynamicCast<CPii>(*it)->GetPersonalData()->GetGroupNo() == groupNo) {
            count++;
        }
    }
    pBase->PushInt(count);
    return 0;
}

int method_showPiiInfoWorldObjStat(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    if (pPii) {
        std::tr1::shared_ptr<CPiiInfoWorldObjStat> stat(new CPiiInfoWorldObjStat);
        stat->SetPersonalData(pPii->GetPersonalData());
        pPii->AddDrawLayer(stat);
        GetCurrentGameMap()->GetWorldObjStatDisplay()->Add(pPii);
    }
    pBase->PushNull();
    return 0;
}

int method_getNodeY(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    const char* pName = pEngine->PopString();
    CPii* pPii = DynamicCast<CPii>(pEngine->PopWorldObj());
    f32 y = 0.0f;
    if (pPii && pPii->GetModel()) {
        nw4r::g3d::ScnMdlSimple* pScnMdl = static_cast<nw4r::g3d::ScnMdlSimple*>(pPii->GetModel()->GetScnObj());
        if (pScnMdl != nullptr) {
            nw4r::g3d::ResNode node = pScnMdl->GetResMdl().GetResNode(pName);
            if (node.IsValid()) {
                nw4r::math::MTX34 mtx;
                pScnMdl->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, node.GetID());
                y = mtx._13;
            }
        }
    }
    pEngine->PushFloat(y);
    return 0;
}

int method_showHPBar(NPpu::CEngine* pBase) {
    bool show = pBase->PopInt() != 0;
    CPii* pPii = DynamicCast<CPii>(static_cast<CPpuEngine*>(pBase)->PopWorldObj());
    if (pPii) {
        std::tr1::shared_ptr<CHPBar> bar = pPii->GetHPBar();
        if (bar) {
            bar->SetEnabled(show);
        }
    }
    pBase->PushNull();
    return 0;
}

int IsNewMonsno(NPpu::CEngine* pEngine) {
    u16 species = pEngine->PopInt();
    pEngine->PushInt(IGameSession::GetInstance()->GetCollectionType(species) != 3);
    return 0;
}

int GetMaxBP(NPpu::CEngine* pEngine) {
    pEngine->PushInt(GetStrongestBP());
    return 0;
}

int method_getDesignPiiFlyHeight(NPpu::CEngine* pBase) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBase);
    std::tr1::shared_ptr<CPiiPersonalData> data = pEngine->PopPersonalData();
    f32 height = 0.0f;
    if (data) {
        if (data->GetSpeciesData()->IsFlying()) {
            height = 0.3f;
        }
    }
    pEngine->PushFloat(height);
    return 0;
}
