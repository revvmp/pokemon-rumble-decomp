#include <game/BWorld.h>
#include <game/CLayout.h>
#include <game/IGameEnding.h>
#include <game/IPauseMenu.h>

#include <list>
#include <memory>
#include <string>

namespace {

class CGameEnding : public IGameEnding {
public:
    inline virtual ~CGameEnding();
    inline virtual void Calc();
    inline virtual void CalcMtx();
    inline virtual void CalcView();
    inline virtual void Draw();
    inline virtual void RunAction();
    virtual const MapDesc& GetMapDesc() const { return *mpMapDesc; }
    virtual BWorld* GetWorld() const { return mpWorld.get(); }
    virtual IPauseMenu* GetPauseMenu() { return mpPauseMenu.get(); }
    inline virtual void ShowCongratulations();
    inline virtual void StopCongratulations();
    inline virtual void ParseStaffRollText(const wchar_t* pText);
    inline virtual void StartStaffRoll();
    inline virtual void HideStaffRoll();
    inline virtual bool IsStaffRollFinished();
    inline virtual void ShowNext();
    inline virtual void StopNext();
    inline virtual bool IsNextVisible();

private:
    enum {
        STATE_IDLE,
        STATE_SCROLLING,
        STATE_SCROLL_DONE
    };

    const MapDesc* mpMapDesc;
    std::tr1::shared_ptr<BWorld> mpWorld;
    std::tr1::shared_ptr<IPauseMenu> mpPauseMenu;
    CLayout mCongratulations;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mpCongratulationsMotion;
    int mState;
    CLayout mStaffRoll;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mpScrollMotion;
    int mLineCount;
    std::list<std::wstring> mLineTexts;
    std::list<std::wstring> mTitleTexts;
    CLayout mNext;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mpNextMotion;
};

inline CGameEnding::~CGameEnding() {}

inline void CGameEnding::ShowCongratulations() {
    mpCongratulationsMotion = mCongratulations.CreateMotion("normal");
    mCongratulations.SetVisible(true);
}

inline void CGameEnding::StopCongratulations() {
    mCongratulations.StopMotion();
}

inline void CGameEnding::ParseStaffRollText(const wchar_t* pText) {
    const wchar_t* pLine = pText;
    const wchar_t* pEnd = pText;
    for (;;) {
        if (*pEnd == L'\n' || *pEnd == 0) {
            std::wstring unused;
            if (*pLine == 0x25C6) {
                mLineTexts.push_back(L"");
                mTitleTexts.push_back(std::wstring(pLine + 1, pEnd));
            } else {
                mLineTexts.push_back(std::wstring(pLine, pEnd));
                mTitleTexts.push_back(L"");
            }
            if (*pEnd == 0) {
                break;
            }
            pLine = ++pEnd;
        } else {
            ++pEnd;
        }
    }
}

inline void CGameEnding::StartStaffRoll() {
    mStaffRoll.SetVisible(true);
    mpScrollMotion = mStaffRoll.CreateMotion("in");
    mStaffRoll.SetPaneVisible("SCROLL", false);
    mState = STATE_SCROLLING;
}

inline void CGameEnding::HideStaffRoll() {
    mStaffRoll.SetVisible(false);
    mState = STATE_IDLE;
}

inline bool CGameEnding::IsStaffRollFinished() {
    return !(mState == STATE_SCROLLING || (mState == STATE_SCROLL_DONE && !mLineTexts.empty()));
}

inline void CGameEnding::ShowNext() {
    mNext.CreateMotion("begin");
    mNext.AppendMotion("loop");
    mpNextMotion = mNext.AppendMotion("end");
    mNext.SetVisible(true);
}

inline void CGameEnding::StopNext() {
    mNext.StopMotion();
}

inline bool CGameEnding::IsNextVisible() {
    return mNext.IsVisible();
}

inline void CGameEnding::Draw() {
    mpWorld->Draw();
    mCongratulations.Draw();
    mStaffRoll.Draw();
    mNext.Draw();
}

inline void CGameEnding::CalcView() {
    mpWorld->CalcView();
    mpPauseMenu->CalcView();
}

inline void CGameEnding::CalcMtx() {
    mpWorld->CalcMtx();
    mpPauseMenu->CalcMtx();
}

inline void CGameEnding::Calc() {
    mpWorld->SetOutlineAlpha(255);
    mpWorld->SetOutlineWidth(1);
    mpWorld->Calc();
    mStaffRoll.Calc();
    mNext.Calc();
    mCongratulations.Calc();
    if ((mState == STATE_SCROLLING || mState == STATE_SCROLL_DONE) && mpScrollMotion->IsEnded()) {
        if (!mLineTexts.empty()) {
            mLineTexts.erase(mLineTexts.begin());
            mTitleTexts.erase(mTitleTexts.begin());
        }
        mStaffRoll.SetPaneVisible("SCROLL", true);
        mpScrollMotion = mStaffRoll.CreateMotion("scroll", "SCROLL", 0.0f, 1.0f);
        std::wstring titles;
        std::wstring lines;
        std::list<std::wstring>::iterator title = mTitleTexts.begin();
        std::list<std::wstring>::iterator line = mLineTexts.begin();
        int i = 0;
        for (; i < mLineCount && line != mLineTexts.end(); ++i, ++line, ++title) {
            if (i > 0) {
                lines += L"\n";
                titles += L"\n";
            }
            lines += *line;
            titles += *title;
        }
        for (; i < mLineCount; ++i) {
            if (i > 0) {
                lines += L"\n";
                titles += L"\n";
            }
        }
        mStaffRoll.SetText("LINES", lines.c_str());
        mStaffRoll.SetText("TITLES", titles.c_str());
        mState = STATE_SCROLL_DONE;
    }
    if (mpNextMotion && mpNextMotion->IsEnded()) {
        mpNextMotion.reset();
        mNext.SetVisible(false);
    }
    if (mpCongratulationsMotion && mpCongratulationsMotion->IsEnded()) {
        mpCongratulationsMotion.reset();
        mCongratulations.SetVisible(false);
    }
}

}
