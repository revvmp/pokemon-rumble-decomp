#include <game/ui/CUIManager.h>

CUIManager::CUIManager() {
    mFlags |= 1;
}

CUIManager::~CUIManager() {}

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

namespace {

inline void NotifyFocusChanged(BUI* pUI, BUI* pPrevious) {
    if (pUI->GetFocusCallbacks().get() != nullptr) {
        for (BUI::CallbackList::iterator it = pUI->GetFocusCallbacks().get()->begin(); it != pUI->GetFocusCallbacks().get()->end(); ++it) {
            (**it)(pPrevious);
        }
    }
}

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
        NotifyFocusChanged(pOldUI, pOldUI);
    }
    BUI* pNewUI = pNew.get();
    if (pNew) {
        NotifyFocusChanged(pNewUI, pOld.get());
    }
}

BUI* CUIManager::GetFocusedUI() {
    return mFocusedUI.lock().get();
}

void CUIManager::DispatchFocusedInput(const IUIInput* pInput) {
    ValidateFocusedUI();
    std::tr1::shared_ptr<BUI> pFocused = mFocusedUI.lock();
    if (pFocused && pFocused->IsFocusable()) {
        pFocused->OnInput(pInput);
    }
}

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

