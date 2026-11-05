#include <string>
#include <game/core/ICalcDraw.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <nw4r/ut/ut_RuntimeTypeInfo.h>

#include <game/world/BWorld.h>
#include <game/core/CFiber.h>
#include <game/ui/CLayout.h>
#include <game/script/CPpuEngine.h>
#include <game/world/CShadowCamera.h>
#include <game/io/FileList.h>
#include <game/session/IGameEnding.h>
#include <game/session/IGameSession.h>
#include <game/ui/IPauseMenu.h>
#include <game/ui/LayoutResources.h>
#include <game/world/MapDesc.h>
#include <game/text/Message.h>
#include <game/ui/SystemMessage.h>

#include <list>
#include <memory>

namespace {

class CGameEnding : public IGameEnding {
public:
    explicit CGameEnding(const MapDesc* pMapDesc) : mpMapDesc(pMapDesc), mState(STATE_IDLE), mLineCount(0) {}
    inline void Init(bool background);
    inline virtual void ShowCongratulations();
    inline virtual void StopCongratulations();
    inline virtual void ParseStaffRollText(const wchar_t* pText);
    inline virtual void StartStaffRoll();
    inline virtual bool IsStaffRollFinished();
    inline virtual void HideStaffRoll();
    inline virtual void ShowNext();
    inline virtual void StopNext();
    inline virtual bool IsNextVisible();
    virtual BWorld* GetWorld() const { return mpWorld.get(); }
    virtual const MapDesc& GetMapDesc() const { return *mpMapDesc; }
    virtual IPauseMenu* GetPauseMenu() { return mpPauseMenu.get(); }
    inline virtual void Calc();
    inline virtual void CalcMtx();
    inline virtual void CalcView();
    inline virtual void Draw();
    inline virtual void RunAction();
    inline void ScrollStaffRoll();

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

inline void CGameEnding::Init(bool background) {
    mpPauseMenu = IPauseMenu::Create(mpMapDesc, true, true);

    BWorld::Desc worldDesc;
    worldDesc.SetDrawType(1);
    worldDesc.SetObjectCapacity(0x400);
    worldDesc.SetOutlineCapacity(worldDesc.GetObjectCapacity());
    worldDesc.SetModelCapacity(worldDesc.GetObjectCapacity() + 0x80);
    worldDesc.SetScnObjCapacity(worldDesc.GetOutlineCapacity() + worldDesc.GetModelCapacity() + 0x200);
    worldDesc.SetEffectCapacity(worldDesc.GetObjectCapacity());
    worldDesc.SetCaptureEnable(true);
    worldDesc.SetSoundListenerEnable(true);
    mpWorld.reset(new BWorld(worldDesc));

    std::tr1::shared_ptr<CShadowCamera> pShadowCamera;
    pShadowCamera.reset(new CShadowCamera(4));
    pShadowCamera->Setup(0x200, 0x200, 1, false);
    mpWorld->AddCamera(pShadowCamera, 1);

    std::tr1::shared_ptr<CLayout::FontMap> pFonts = GetDefaultFontMap();
    void* pEndingArchives;
    void* pEpilogueArchives;
    std::tr1::shared_ptr<FileList> pFiles =
        LoadFileList(&pEndingArchives, "event/ending.arc/event2d.arc", 0, background, nullptr);
    mCongratulations.Load(static_cast<const char* const*>(pEndingArchives), "congratulations", pFonts, background);
    mCongratulations.SetVisible(false);
    mStaffRoll.Load(static_cast<const char* const*>(pEndingArchives), "staff_roll", pFonts, background);
    mStaffRoll.SetVisible(false);

    nw4r::lyt::TextBox* pLines = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(mStaffRoll.FindPane("LINES"));
    mLineCount = 1;
    for (const wchar_t* pChar = pLines->GetStringBuffer(); *pChar != L'\0'; pChar++) {
        if (*pChar == L'\n') {
            mLineCount++;
        }
    }
    for (int i = 0; i < mLineCount; i++) {
        mLineTexts.push_back(std::wstring(L""));
        mTitleTexts.push_back(std::wstring(L""));
    }

    pFiles = LoadFileList(&pEpilogueArchives, "event/epilogue.arc/event2d.arc", 0, background, nullptr);
    mNext.Load(static_cast<const char* const*>(pEpilogueArchives), "next", pFonts, background);
    mNext.SetText("TextBox_C_01", GetMessageString(0x60115, nullptr));
    const wchar_t* pNextText = GetMessageString(0x60114, nullptr);
    mNext.SetText("TextBox", pNextText);
    mNext.SetText("TextBox2", pNextText);
    mNext.SetVisible(false);

    int difficulty = mpMapDesc->mIndex.mMode;
    mNext.SetPaneVisible("HARD", difficulty == 0);
    mNext.SetPaneVisible("VERYHARD", difficulty == 1);
    mNext.SetPaneVisible("THANKYOU", difficulty == 2);
}

inline void CGameEnding::RunAction() {
    IGameSession::GetInstance()->SetFade(0, -1);
    YieldFiberStarted();
    RunScript("CGameEnding.pkc", 1, IGameSession::GetInstance()->GetEnvironment());
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

inline void CGameEnding::ScrollStaffRoll() {
    if (!mLineTexts.empty()) {
        typedef std::wstring StaffText;
        typedef std::__list_deleter<StaffText, std::allocator<StaffText> > TextListBase;
        TextListBase::node* pNode;
        pNode = static_cast<TextListBase::node*>(mLineTexts.begin().base());
        pNode = reinterpret_cast<TextListBase::node*>(reinterpret_cast<unsigned long>(pNode) | 0);
        TextListBase::unlink_nodes(pNode, pNode);
        pNode->data_.~StaffText();
        reinterpret_cast<TextListBase&>(mLineTexts).node_alloc().deallocate(pNode, 1);
        --reinterpret_cast<TextListBase&>(mLineTexts).size_ref();
        pNode = static_cast<TextListBase::node*>(mTitleTexts.begin().base());
        pNode = reinterpret_cast<TextListBase::node*>(reinterpret_cast<unsigned long>(pNode) | 0);
        TextListBase::unlink_nodes(pNode, pNode);
        pNode->data_.~StaffText();
        reinterpret_cast<TextListBase&>(mTitleTexts).node_alloc().deallocate(pNode, 1);
        --reinterpret_cast<TextListBase&>(mTitleTexts).size_ref();
    }
    mStaffRoll.SetPaneVisible("SCROLL", true);
    mpScrollMotion = mStaffRoll.CreateMotion("scroll", "SCROLL", 0.0f, 1.0f);
    std::list<std::wstring>::iterator line = mLineTexts.begin();
    std::list<std::wstring>::iterator title = mTitleTexts.begin();
    std::wstring lines;
    std::wstring titles;
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

inline void CGameEnding::Calc() {
    mpWorld->SetOutlineAlpha(255);
    mpWorld->SetOutlineWidth(1);
    mpWorld->Calc();
    mStaffRoll.Calc();
    mNext.Calc();
    mCongratulations.Calc();
    if ((mState == STATE_SCROLLING || mState == STATE_SCROLL_DONE) && mpScrollMotion->IsEnded())
        ScrollStaffRoll();
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

std::tr1::shared_ptr<IGameEnding> IGameEnding::Create(const MapDesc* pDesc, bool background) {
    std::tr1::shared_ptr<CGameEnding> pGameEnding(new CGameEnding(pDesc));
    pGameEnding->Init(background);
    return pGameEnding;
}
