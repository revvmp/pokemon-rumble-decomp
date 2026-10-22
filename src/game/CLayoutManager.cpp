#include <game/detail/CLayoutManager.h>

std::tr1::shared_ptr<ILayoutManager> ILayoutManager::Create() {
    return std::tr1::shared_ptr<CLayoutManager>(new CLayoutManager);
}

void CLayoutManager::Draw() {
    const std::map<int, std::tr1::shared_ptr<CLayout> >::iterator end = mLayouts.end();
    std::map<int, std::tr1::shared_ptr<CLayout> >::iterator it = mLayouts.begin();
    for (; it != end; ++it) {
        it->second->Draw();
    }
}

void CLayoutManager::Calc() {
    const std::map<int, std::tr1::shared_ptr<CLayout> >::iterator end = mLayouts.end();
    std::map<int, std::tr1::shared_ptr<CLayout> >::iterator it = mLayouts.begin();
    for (; it != end; ++it) {
        it->second->Calc();
    }
}

bool CLayoutManager::Remove(int id) {
    std::map<int, std::tr1::shared_ptr<CLayout> >::iterator it = mLayouts.find(id);
    if (it != mLayouts.end()) {
        mLayouts.erase(it);
        return true;
    }
    return false;
}

std::tr1::shared_ptr<CLayout> CLayoutManager::Get(int id) {
    std::map<int, std::tr1::shared_ptr<CLayout> >::iterator it = mLayouts.find(id);
    if (it != mLayouts.end()) {
        return it->second;
    }
    return std::tr1::shared_ptr<CLayout>();
}

int CLayoutManager::Add(const std::tr1::shared_ptr<CLayout>& rLayout) {
    mLayouts[mNextID] = rLayout;
    return mNextID++;
}
