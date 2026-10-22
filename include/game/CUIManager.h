#ifndef GAME_CUIMANAGER_H
#define GAME_CUIMANAGER_H

#include <game/BUI.h>
#include <memory>

class CUIManager : public BUI {
public:
    CUIManager();
    virtual ~CUIManager();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual CUIManager* GetManager();

    void SetFocusedUI(BUI* pUI);
    void DispatchFocusedInput(const IUIInput* pInput);
    BUI* GetFocusedUI();
    void ValidateFocusedUI();

private:
    std::tr1::weak_ptr<BUI> mFocusedUI;
};

#endif
