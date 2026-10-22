#ifndef GAME_DETAIL_CMONEYLIFEDISPLAY_H
#define GAME_DETAIL_CMONEYLIFEDISPLAY_H

#include <game/IMoneyLifeDisplay.h>
#include <game/CDisplayPhaseCtrl.h>
#include <game/CUIManager.h>
#include <game/BUILayout.h>
#include <game/MapDesc.h>
#include <game/IGameSession.h>
#include <game/IGameStage.h>
#include <game/LayoutResources.h>
#include <game/StringFormat.h>

namespace {

enum MoneyLifeDisplayMode {
    DISPLAY_NONE,
    DISPLAY_MONEY,
    DISPLAY_LIFE,
    DISPLAY_MONEY_AND_LIFE
};

inline MoneyLifeDisplayMode GetMoneyLifeDisplayMode(const MapDesc* pMapDesc) {
    if (pMapDesc->mIndex.mStage == 7) {
        if (pMapDesc->mIndex.mMap == 1) return DISPLAY_NONE;
        return DISPLAY_LIFE;
    }
    if (pMapDesc->mIndex.mStage == 0) return DISPLAY_MONEY;
    return DISPLAY_MONEY_AND_LIFE;
}

class CMoneyLifeDisplay : public IMoneyLifeDisplay {
public:
    explicit CMoneyLifeDisplay(const MapDesc* pMapDesc);
    virtual void Calc() {
        if (mMode == 0) return;
        mPhase.Calc();
        mManager->Calc();
        if (mMode & 2) {
            if (mMotion && mMotion->IsEnded()) mMotion.reset();
            int life = GetGameStage()->GetLife();
            if (!mMotion) {
                if (mShownLife < life) {
                    mMotion = mLifeLayouts[mShownLife]->GetLayout()->CreateMotion("begin");
                    mLifeLayouts[mShownLife]->GetLayout()->QueueMotion("normal");
                    ++mShownLife;
                } else if (life < mShownLife) {
                    --mShownLife;
                    mMotion = mLifeLayouts[mShownLife]->GetLayout()->CreateMotion("end");
                }
            }
        }
        if (mMode & 1) {
            if (mShownMoney != IGameSession::GetInstance()->GetMoney()) {
                mMoneyLayout->GetLayout()->SetText("MONEY", FormatString(L"%d", IGameSession::GetInstance()->GetMoney()).c_str());
                mMoneyLayout->GetLayout()->CreateMotion("get", "MONEY", 0.0f, 1.0f);
                mShownMoney = IGameSession::GetInstance()->GetMoney();
            }
        }
    }
    virtual void Draw() {
        if (mMode != 0 && mPhase.IsVisible()) mManager->Draw();
    }
    virtual void SetForceVisible(bool visible) {
        if (mMode != 0) mPhase.SetForceVisible(visible);
    }
    virtual void Show() {
        if (mMode != 0) mPhase.Show();
    }
    virtual void Hide() {
        if (mMode != 0) mPhase.Hide();
    }

    bool Initialize(bool background) {
        if (mMode == 0) return true;
        std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
        const char* const* pArchives = gSharedLayoutArchives;
        mManager.reset(new CUIManager());
        mMoneyLayout.reset(new BUILayout());
        if (mMode & 2) {
            for (int i = 0; i < 3; ++i) mLifeLayouts[i].reset(new BUILayout());
        }
        const char* pRoot;
        switch (mMode) {
        default: pRoot = "money_root"; break;
        case 2: pRoot = "life_root"; break;
        case 3: pRoot = "moneylife_root"; break;
        }
        mMoneyLayout->Load(pArchives, pRoot, fonts, background);
        mManager->AddChild(mMoneyLayout);
        if (mMode & 2) {
            for (int i = 0; i < 3; ++i) {
                mLifeLayouts[i].reset(new BUILayout());
                mLifeLayouts[i]->Load(pArchives, "life", fonts, background);
            }
        }
        mManager->AddChild(mMoneyLayout);
        if (mMode & 2) {
            for (int i = 0; i < 3; ++i) mMoneyLayout->AttachChild(FormatString("LIFE%d", i).c_str(), mLifeLayouts[i]);
        }
        mPhase.SetTarget(mMoneyLayout->GetLayout(), static_cast<nw4r::lyt::Pane*>(nullptr));
        mShownLife = GetGameStage()->GetLife();
        mShownMoney = IGameSession::GetInstance()->GetMoney();
        if (mMode & 1) mMoneyLayout->GetLayout()->SetText("MONEY", FormatString(L"%d", mShownMoney).c_str());
        if (mMode & 2) {
            for (int i = 0; i < 3; ++i) {
                CLayout* pLayout = mLifeLayouts[i]->GetLayout();
                const char* pMotion = i < mShownLife ? "normal" : "end";
                u32 frame = pLayout->GetMotionLength(pMotion);
                pLayout->CreateMotion(pMotion, spDefaultMotionPane, frame, 1.0f);
                pLayout->StopMotion();
            }
        }
        return true;
    }

private:
    MoneyLifeDisplayMode mMode;
    CDisplayPhaseCtrl mPhase;
    int mShownMoney;
    int mShownLife;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotion;
    std::tr1::shared_ptr<CUIManager> mManager;
    std::tr1::shared_ptr<BUILayout> mMoneyLayout;
    std::tr1::shared_ptr<BUILayout> mLifeLayouts[3];
};

}

#endif
