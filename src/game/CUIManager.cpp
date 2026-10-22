#include <game/CUIManager.h>

CUIManager::CUIManager() {
    mFlags |= 1;
}

CUIManager::~CUIManager() {}

void CUIManager::Calc() {
    BUI::Calc();
}

void CUIManager::CalcMtx() {
    BUI::CalcMtx();
}

void CUIManager::CalcView() {
    BUI::CalcView();
}

void CUIManager::Draw() {
    BUI::Draw();
}

CUIManager* CUIManager::GetManager() {
    return this;
}

void CUIManager::ValidateFocusedUI() {
    std::tr1::shared_ptr<BUI> pFocused = mFocusedUI.lock();
    if (pFocused && pFocused->GetManager() == this) {
        return;
    }
    mFocusedUI.reset();
}

BUI* CUIManager::GetFocusedUI() {
    return mFocusedUI.lock().get();
}

void CUIManager::SetFocusedUI(BUI* pUI) {
    ValidateFocusedUI();

    std::tr1::shared_ptr<BUI> pNew = mFocusedUI.lock();
    std::tr1::shared_ptr<BUI> pOld;
    if (pNew) {
        if (pNew.get() == pUI) {
            return;
        }
        pOld = pNew;
        mFocusedUI.reset();
        pNew.reset();
    }

    if (pUI != nullptr) {
        pNew = pUI->shared_from_this();
        mFocusedUI = pNew;
    }

    if (pOld) {
        if (pOld->mFocusCallbacks.get() != nullptr) {
            for (BUI::CallbackList::iterator it = pOld->mFocusCallbacks.get()->begin(); it != pOld->mFocusCallbacks.get()->end(); ++it) {
                (**it)(pOld.get());
            }
        }
    }
    if (pNew) {
        if (pNew->mFocusCallbacks.get() != nullptr) {
            for (BUI::CallbackList::iterator it = pNew->mFocusCallbacks.get()->begin(); it != pNew->mFocusCallbacks.get()->end(); ++it) {
                (**it)(pOld.get());
            }
        }
    }
}

void CUIManager::DispatchFocusedInput(const IUIInput* pInput) {
    ValidateFocusedUI();
    std::tr1::shared_ptr<BUI> pFocused = mFocusedUI.lock();
    if (pFocused && (pFocused->mFlags & 4)) {
        pFocused->OnInput(pInput);
    }
}
