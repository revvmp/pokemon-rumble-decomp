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

    BUI* pOldUI = pOld.get();
    if (pOld) {
        if (pOldUI->mFocusCallbacks.get() != nullptr) {
            for (BUI::CallbackList::iterator it = pOldUI->mFocusCallbacks.get()->begin(); it != pOldUI->mFocusCallbacks.get()->end(); ++it) {
                (**it)(pOldUI);
            }
        }
    }
    BUI* pNewUI = pNew.get();
    if (pNew) {
        if (pNewUI->mFocusCallbacks.get() != nullptr) {
            for (BUI::CallbackList::iterator it = pNewUI->mFocusCallbacks.get()->begin(); it != pNewUI->mFocusCallbacks.get()->end(); ++it) {
                (**it)(pOldUI);
            }
        }
    }
}

void CUIManager::DispatchFocusedInput(const IUIInput* pInput) {
    ValidateFocusedUI();
    std::tr1::shared_ptr<BUI> pFocused = mFocusedUI.lock();
    if (pFocused && pFocused->IsFocusable()) {
        pFocused->OnInput(pInput);
    }
}
