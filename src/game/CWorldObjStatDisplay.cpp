#include <game/BWorld.h>
#include <game/IWorldObjStat.h>
#include <game/IWorldObjStatDisplay.h>
#include <algorithm>
#include <vector>

class CWorldObjStatDisplay : public IWorldObjStatDisplay {
public:
    CWorldObjStatDisplay() {}
    virtual ~CWorldObjStatDisplay();
    virtual void Calc();
    virtual void Draw();
    virtual void Add(BWorldObj* pObj);
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

static bool CompareType(IWorldObjStat* pLeft, IWorldObjStat* pRight) {
    return pLeft->GetType() < pRight->GetType();
}

void CWorldObjStatDisplay::Draw() {
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
