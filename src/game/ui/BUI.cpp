#include <game/ui/CUIManager.h>
#include <game/ui/BUICallbackList.h>

BUI::BUI() :
    mFlags(6),
    mpParent(nullptr) {}

BUI::~BUI() {
    while (!mChildren.empty()) {
        Child child = mChildren.front();
        mChildren.pop_front();
        child->mpParent = nullptr;
        child->mFlags &= ~1;
        child->OnDetached(this);
    }
}

CUIManager* BUI::GetManager() {
    if (mpParent != nullptr) {
        return mpParent->GetManager();
    }
    return nullptr;
}

void BUI::SetVisible(bool visible) {
    if (visible != ((mFlags >> 1) & 1)) {
        if (visible) {
            mFlags |= 2;
        } else {
            mFlags &= ~2;
        }
        if (mVisibilityCallbacks.get() != nullptr) {
            for (CallbackList::iterator it = mVisibilityCallbacks.get()->begin(); it != mVisibilityCallbacks.get()->end(); ++it) {
                (**it)(this);
            }
        }
    }
}

bool BUI::IsFocused() {
    CUIManager* pManager = GetManager();
    if (pManager != nullptr) {
        return pManager->GetFocusedUI() == this;
    }
    return false;
}

bool BUI::AddChild(Child child) {
    if (!child || (child->mFlags & 1)) {
        return false;
    }
    mChildren.push_back(child);
    child->mpParent = this;
    child->mFlags |= 1;
    child->OnAttached();
    return true;
}

bool BUI::RemoveChild(Child child) {
    if (!child || child->mpParent != this) {
        return false;
    }
    for (std::list<Child>::iterator it = mChildren.begin(); it != mChildren.end(); ++it) {
        if ((*it).get() == child.get()) {
            mChildren.erase(it);
            child->mpParent = nullptr;
            child->mFlags &= ~1;
            child->OnDetached(this);
            CUIManager* pManager = GetManager();
            if (pManager != nullptr) {
                pManager->ValidateFocusedUI();
            }
            return true;
        }
    }
    return false;
}

void BUI::OnAttached() {}

void BUI::OnDetached(BUI* pParent) {}

BUI* BUI::NavigateTo(int direction) {
    mFlags |= 8;
    BUI* pResult = nullptr;
    BUI* pNeighbor = mNeighbors[direction].lock().get();
    if (pNeighbor != nullptr) {
        pResult = pNeighbor->FindFocusableFrom(direction);
    }
    mFlags &= ~8;
    return pResult;
}

BUI* BUI::FindFocusableFrom(int direction) {
    if (mFlags & 8) {
        return nullptr;
    }
    if (mFlags & 4) {
        return this;
    }
    mFlags |= 8;
    BUI* pResult = nullptr;
    for (int i = 0; i < 4; i++) {
        int next = (i + direction) % 4;
        BUI* pNeighbor = mNeighbors[next].lock().get();
        if (pNeighbor != nullptr) {
            pResult = pNeighbor->FindFocusableFrom(next);
            if (pResult != nullptr) {
                break;
            }
        }
    }
    mFlags &= ~8;
    return pResult;
}

#pragma opt_strength_reduction off

void BUI::OnInput(const IUIInput* pInput) {
    int i = 0;
    for (; i < 4; i++) {
        if (pInput->GetTrig() & (1 << i)) {
            std::tr1::weak_ptr<BUI>& rNeighbor = mNeighbors[i];
            BUI* pFirst = rNeighbor.lock().get();
            if (pFirst != nullptr) {
                BUI* pTarget = NavigateTo(i);
                if (pTarget != nullptr) {
                    GetManager()->SetFocusedUI(pTarget);
                }
                return;
            }
        }
    }
    if (mpParent != nullptr) {
        mpParent->OnInput(pInput);
    }
}

#pragma opt_strength_reduction reset

void BUI::Calc() {
    for (std::list<Child>::iterator it = mChildren.begin(); it != mChildren.end(); ++it) {
        (*it)->Calc();
    }
}

void BUI::CalcMtx() {
    for (std::list<Child>::iterator it = mChildren.begin(); it != mChildren.end(); ++it) {
        (*it)->CalcMtx();
    }
}

void BUI::CalcView() {
    if (mFlags & 2) {
        for (std::list<Child>::iterator it = mChildren.begin(); it != mChildren.end(); ++it) {
            (*it)->CalcView();
        }
    }
}

void BUI::Draw() {
    if (mFlags & 2) {
        for (std::list<Child>::iterator it = mChildren.begin(); it != mChildren.end(); ++it) {
            (*it)->Draw();
        }
    }
}
