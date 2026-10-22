#include <game/BModelObj.h>
#include <game/BWorld.h>
#include <game/CDividedGround.h>
#include <game/CEnemyPii.h>
#include <game/CPii.h>
#include <game/CPlayerPii.h>
#include <game/CPpuEngine.h>
#include <algorithm>
#include <list>
#include <set>

#pragma dont_inline on

typedef int (*FoundFunc)(NPpu::CEngine*, NPpu::Slice*, int, std::list<BWorldObj*>&);
typedef int (*NoneFunc)(NPpu::CEngine*, NPpu::Slice*, int);

int method_setOutlineType(NPpu::CEngine* pEngine) {
    int type = pEngine->PopInt();
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pModel != nullptr) {
        pModel->SetOutlineType(type);
    }
    pEngine->PushNull();
    return 0;
}

int method_getOutlineType(NPpu::CEngine* pEngine) {
    BModelObj* pModel = DynamicCast<BModelObj>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj());
    if (pModel != nullptr) {
        pEngine->PushInt(pModel->GetOutlineType());
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_isInKeepOutArea(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    bool keepOut = true;
    if (pObject != nullptr) {
        nw4r::math::VEC3 position = pObject->GetPositionRef();
        CDividedGround* pGround = DynamicCast<CDividedGround>(static_cast<CPpuEngine*>(pEngine)->GetWorld()->GetGround());
        if (pGround != nullptr && !pGround->IsOutOfBounds(position)) {
            keepOut = false;
        }
    }
    pEngine->PushInt(keepOut);
    return 0;
}

int method_isCPii(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IsKindOf<CPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj()));
    return 0;
}

int method_isCPlayerPii(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IsKindOf<CPlayerPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj()));
    return 0;
}

int method_isCEnemyPii(NPpu::CEngine* pEngine) {
    pEngine->PushInt(IsKindOf<CEnemyPii>(static_cast<CPpuEngine*>(pEngine)->PopWorldObj()));
    return 0;
}

static int PushObjectIds(NPpu::CEngine* pEngine, NPpu::Slice* pSlices, int capacity, std::list<BWorldObj*>& rObjects) {
    int count = std::min(static_cast<int>(rObjects.size()), capacity - 1);
    std::list<BWorldObj*>::iterator it = rObjects.begin();
    for (int i = 0; i < count; ++i, ++it) {
        pSlices[i].SetInt((*it)->GetId());
    }
    if (count >= 0) {
        pSlices[count].SetInt(0);
    }
    pEngine->PushInt(count);
    return 0;
}

static int MergeObjectIds(NPpu::CEngine* pEngine, NPpu::Slice* pSlices, int capacity, std::list<BWorldObj*>& rObjects) {
    std::set<int> ids;
    for (int i = 0; i < capacity; ++i) {
        int id = pSlices[i].ToInt();
        if (id == 0) {
            break;
        }
        ids.insert(id);
    }
    for (std::list<BWorldObj*>::iterator it = rObjects.begin(); it != rObjects.end(); ++it) {
        ids.insert((*it)->GetId());
    }
    int count = std::min(static_cast<int>(ids.size()), capacity - 1);
    std::set<int>::iterator it = ids.begin();
    for (int i = 0; i < count; ++i, ++it) {
        pSlices[i].SetInt(*it);
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

static int CountMergedIds(NPpu::CEngine* pEngine, NPpu::Slice* pSlices, int capacity) {
    int count = 0;
    while (count < capacity && pSlices[count].ToInt() != 0) {
        ++count;
    }
    if (count == capacity && count > 0) {
        pSlices[count - 1].SetInt(0);
    }
    pEngine->PushInt(count);
    return 0;
}

static int PushObjectCount(NPpu::CEngine* pEngine, NPpu::Slice*, int, std::list<BWorldObj*>& rObjects) {
    pEngine->PushInt(rObjects.size());
    return 0;
}

static int PushZeroCount(NPpu::CEngine* pEngine, NPpu::Slice*, int) {
    pEngine->PushInt(0);
    return 0;
}

template <int Mode> static int SearchArc(NPpu::CEngine* pEngine, FoundFunc pFound, NoneFunc pNone) {
    f32 width = pEngine->PopFloat();
    f32 outerRadius = pEngine->PopFloat();
    f32 innerRadius = pEngine->PopFloat();
    int type = pEngine->PopInt();
    int capacity = 0;
    NPpu::Slice* pSlices = pEngine->PopArray(&capacity);
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        f32 start = pObject->GetRotation() - width * 0.5f;
        f32 end = pObject->GetRotation() + width * 0.5f;
        std::list<BWorldObj*> objects = static_cast<CPpuEngine*>(pEngine)->GetWorld()->FindObjectsInSector(type, pObject->GetPositionRef(), innerRadius, outerRadius, start, end);
        return pFound(pEngine, pSlices, capacity, objects);
    }
    return pNone(pEngine, pSlices, capacity);
}

static int CountArc(NPpu::CEngine* pEngine, FoundFunc pFound, NoneFunc pNone) {
    f32 width = pEngine->PopFloat();
    f32 outerRadius = pEngine->PopFloat();
    f32 innerRadius = pEngine->PopFloat();
    int type = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        f32 start = pObject->GetRotation() - width * 0.5f;
        f32 end = pObject->GetRotation() + width * 0.5f;
        std::list<BWorldObj*> objects = static_cast<CPpuEngine*>(pEngine)->GetWorld()->FindObjectsInSector(type, pObject->GetPositionRef(), innerRadius, outerRadius, start, end);
        return pFound(pEngine, nullptr, 0, objects);
    }
    return pNone(pEngine, nullptr, 0);
}

int method_findObjInArcZX(NPpu::CEngine* pEngine) {
    return SearchArc<0>(pEngine, PushObjectIds, PushNoObjects);
}

int method_mergeFindObjInArcZX(NPpu::CEngine* pEngine) {
    return SearchArc<1>(pEngine, MergeObjectIds, CountMergedIds);
}

int method_countObjInArcZX(NPpu::CEngine* pEngine) {
    return CountArc(pEngine, PushObjectCount, PushZeroCount);
}

int method_getGenerator(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        BWorldObj* pGenerator = pObject->GetParent();
        if (pGenerator != nullptr) {
            pEngine->PushInt(pGenerator->GetId());
            return 0;
        }
    }
    pEngine->PushInt(0);
    return 0;
}

int method_setGenerator(NPpu::CEngine* pEngine) {
    BWorldObj* pGenerator = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        if (pGenerator != nullptr) {
            pObject->SetParent(pGenerator->shared_from_this());
        } else {
            pObject->SetParent(std::tr1::shared_ptr<BWorldObj>(0));
        }
    }
    pEngine->PushNull();
    return 0;
}

int method_getRegister(NPpu::CEngine* pEngine) {
    u32 index = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pEngine->Push(pObject->GetRegister(index));
    } else {
        pEngine->PushInt(0);
    }
    return 0;
}

int method_setRegister(NPpu::CEngine* pEngine) {
    NPpu::Slice value;
    pEngine->Pop(value);
    u32 index = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->SetRegister(index, value);
    }
    pEngine->PushNull();
    return 0;
}

int GetNewObjRegister(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    u32 index = pEngine->PopInt();
    pEngine->Push(pEngine->GetRegister()->GetValue(index));
    return 0;
}

int SetNewObjRegister(NPpu::CEngine* pBaseEngine) {
    CPpuEngine* pEngine = static_cast<CPpuEngine*>(pBaseEngine);
    NPpu::Slice value;
    pEngine->Pop(value);
    u32 index = pEngine->PopInt();
    pEngine->GetRegister()->Set(index, value);
    pEngine->PushNull();
    return 0;
}

int method_isShown(NPpu::CEngine* pEngine) {
    bool shown = false;
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        shown = pObject->IsVisible();
    }
    pEngine->PushInt(shown);
    return 0;
}

int method_showInMiniMap(NPpu::CEngine* pEngine) {
    int show = pEngine->PopInt();
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->SetEffectEnable(show != 0);
    }
    pEngine->PushNull();
    return 0;
}

int method_enablePauseForUI(NPpu::CEngine* pEngine) {
    bool enable = pEngine->PopInt() != 0;
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->EnablePauseForUI(enable);
    }
    pEngine->PushNull();
    return 0;
}

int method_isPauseForUIEnabled(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushInt(pObject != nullptr ? pObject->IsPauseForUIEnabled() : true);
    pEngine->PushNull();
    return 0;
}

int method_enableHitstop(NPpu::CEngine* pEngine) {
    bool enable = pEngine->PopInt() != 0;
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->EnableHitstop(enable);
    }
    pEngine->PushNull();
    return 0;
}

int method_isHitstopEnabled(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushInt(pObject != nullptr ? pObject->IsHitstopEnabled() : true);
    pEngine->PushNull();
    return 0;
}

int method_setIgnoreEnterEvent(NPpu::CEngine* pEngine) {
    bool ignore = pEngine->PopInt() != 0;
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    if (pObject != nullptr) {
        pObject->SetIgnoreEnterEvent(ignore);
    }
    pEngine->PushNull();
    return 0;
}

int method_isIgnoreEnterEvent(NPpu::CEngine* pEngine) {
    BWorldObj* pObject = static_cast<CPpuEngine*>(pEngine)->PopWorldObj();
    pEngine->PushInt(pObject != nullptr ? pObject->IsIgnoreEnterEvent() : true);
    pEngine->PushNull();
    return 0;
}

int CountObj(NPpu::CEngine* pEngine) {
    u32 type = pEngine->PopInt();
    pEngine->PushInt(static_cast<CPpuEngine*>(pEngine)->GetWorld()->CountObjects(type));
    return 0;
}

int GetCIStr(NPpu::CEngine* pEngine) {
    pEngine->Drop(1);
    pEngine->PushNull();
    return 0;
}
