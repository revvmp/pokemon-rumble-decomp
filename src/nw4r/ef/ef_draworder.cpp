#include <nw4r/ef/ef_draworder.h>
#include <nw4r/ef/ef_effect.h>
#include <nw4r/ef/ef_particlemanager.h>

namespace nw4r {
namespace ef {

ut::List* DrawOrderBase::GetPMList(Effect* pEffect) {
    return &pEffect->mParticleManager;
}

void DrawOrder::Draw(Effect* pEffect, const DrawInfo& rInfo) {
    ut::List* pList = GetPMList(pEffect);

    ParticleManager* pManager = nullptr;

    while ((pManager = static_cast<ParticleManager*>(ut::List_GetNext(pList, pManager))) != nullptr) {
        pManager->BeginDraw();
        pManager->Draw(rInfo);
        pManager->EndDraw();
    }
}

void DrawOrder::Add(Effect* pEffect, ParticleManager* pManager) {
    ut::List* pList = GetPMList(pEffect);
    ParticleManager* pPrev = nullptr;

    while ((pPrev = static_cast<ParticleManager*>(ut::List_GetPrev(pList, pPrev))) != nullptr) {
        if (pPrev->mWeight <= pManager->mWeight) {
            break;
        }
    }

    ut::List_Insert(pList, ut::List_GetNext(pList, pPrev), pManager);
}

void DrawOrder::Remove(Effect* pEffect, ParticleManager* pManager) {
    ut::List_Remove(GetPMList(pEffect), pManager);
}

}
}
