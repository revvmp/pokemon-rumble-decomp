#include <game/BWorld.h>
#include <game/BWorldCamera.h>
#include <game/BWorldObj.h>
#include <game/IWorldObjStat.h>
#include <game/IWorldObjStatDisplay.h>
#include <algorithm>
#include <vector>

class CWorldObjStatDisplay : public IWorldObjStatDisplay {
public:
    CWorldObjStatDisplay() {}
    virtual void Add(BWorldObj* pObj);
    virtual void Calc();
    virtual void Draw();
    void SetWorld(BWorld* pWorld) { mWorld = pWorld->shared_from_this(); }

private:
    std::tr1::weak_ptr<BWorld> mWorld;
    DrawLayerSet mHidden;
    DrawLayerSet mVisible;
};

std::tr1::shared_ptr<IWorldObjStatDisplay> IWorldObjStatDisplay::Create(BWorld* pWorld) {
    std::tr1::shared_ptr<CWorldObjStatDisplay> pDisplay(new CWorldObjStatDisplay);
    pDisplay->SetWorld(pWorld);
    return pDisplay;
}

inline void CWorldObjStatDisplay::Calc() {
    std::tr1::shared_ptr<BWorld> world = mWorld.lock();
    if (!world) {
        return;
    }
    BWorldCamera* pCamera = world->GetCurrentCamera();
    if (pCamera == nullptr) {
        return;
    }
    DrawLayerSet stats(mVisible);
    std::vector<BWorldObj*> objects = world->GetObjects(1);
    for (std::vector<BWorldObj*>::iterator it = objects.begin(); it != objects.end(); ++it) {
        const DrawLayerSet* pLayers = (*it)->GetDrawLayers();
        if (pLayers != nullptr) {
            for (DrawLayerSet::iterator itStat = pLayers->begin(); itStat != pLayers->end(); ++itStat) {
                (*itStat)->Update(*it);
                stats.insert(*itStat);
            }
        }
    }
    nw4r::math::MTX34 view;
    nw4r::math::MTX44 projection;
    pCamera->GetG3dCamera().GetCameraMtx(&view);
    pCamera->GetG3dCamera().GetProjectionMtx(&projection);
    mVisible.clear();
    mHidden.clear();
    for (DrawLayerSet::iterator it = stats.begin(); it != stats.end(); ++it) {
        (*it)->CalcView(view, projection);
        if ((*it)->IsVisible()) {
            mVisible.insert(*it);
        } else {
            mHidden.insert(*it);
        }
    }
}

inline void CWorldObjStatDisplay::Add(BWorldObj* pObj) {
    if (pObj == nullptr) {
        return;
    }
    bool hasCamera = false;
    nw4r::math::MTX34 view;
    nw4r::math::MTX44 projection;
    {
        std::tr1::shared_ptr<BWorld> world = mWorld.lock();
        if (world) {
            BWorldCamera* pCamera = world->GetCurrentCamera();
            if (pCamera != nullptr) {
                pCamera->GetG3dCamera().GetCameraMtx(&view);
                pCamera->GetG3dCamera().GetProjectionMtx(&projection);
                hasCamera = true;
            }
        }
    }
    DrawLayerSet* pLayers = pObj->GetDrawLayers();
    if (pLayers != nullptr) {
        for (DrawLayerSet::iterator it = pLayers->begin(); it != pLayers->end(); ++it) {
            (*it)->Update(pObj);
            if (hasCamera) {
                (*it)->CalcView(view, projection);
            }
            mVisible.insert(*it);
        }
    }
}

static inline bool CompareType(IWorldObjStat* pLeft, IWorldObjStat* pRight) {
    return pLeft->GetType() < pRight->GetType();
}

inline void CWorldObjStatDisplay::Draw() {
    std::vector<IWorldObjStat*> objects;
    objects.reserve(mHidden.size() + mVisible.size());
    for (DrawLayerSet::iterator it = mHidden.begin(); it != mHidden.end(); ++it) {
        objects.push_back(it->get());
    }
    for (DrawLayerSet::iterator it = mVisible.begin(); it != mVisible.end(); ++it) {
        objects.push_back(it->get());
    }
    std::sort(objects.begin(), objects.end(), CompareType);
    for (std::vector<IWorldObjStat*>::iterator it = objects.begin(); it != objects.end(); ++it) {
        (*it)->Draw();
    }
}
