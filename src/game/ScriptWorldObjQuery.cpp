#include <game/BModelObj.h>
#include <game/BWorld.h>
#include <game/CDividedGround.h>
#include <game/CEnemyPii.h>
#include <game/CPii.h>
#include <game/CPlayerPii.h>
#include <game/CPpuEngine.h>

#pragma dont_inline on

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
