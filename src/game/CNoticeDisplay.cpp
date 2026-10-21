#include <game/CNoticeDisplay.h>

#include <functional>
#include <game/CFiber.h>
#include <game/CFileData.h>
#include <game/CLayout.h>
#include <game/IGameMap.h>
#include <game/IUIInput.h>
#include <game/LayoutResources.h>
#include <game/MapDesc.h>
#include <game/Message.h>
#include <game/StageID.h>
#include <game/StringFormat.h>
#include <game/SystemMessage.h>
#include <nw4r/lyt/lyt_material.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <string>

IUIInput* GetUIInput();
bool IsStage7EvenMap(MapIndex index);
std::wstring FormatAreaName(MapIndex index);

extern const char* const gAreaPaneNames[9];

void WaitMotion(std::tr1::shared_ptr<CLayout::CMotionProgram> pMotion, bool waitFinish) {
    while (true) {
        IUIInput* pInput = GetUIInput();
        if (pInput != nullptr && pInput->IsConnected() && (pInput->GetTrig() & 0x20)) {
            return;
        }
        if (pMotion && ((waitFinish && pMotion->IsFinished()) || (!waitFinish && pMotion->IsStarted()))) {
            return;
        }
        YieldFiber();
    }
}

void WaitMotionStart(std::tr1::shared_ptr<CLayout::CMotionProgram> pMotion) {
    WaitMotion(pMotion, false);
}

namespace {

class BNoticeDisplayImpl : public ICalcDraw {
public:
    virtual ~BNoticeDisplayImpl() {}
    virtual void Calc();
    virtual void Draw();
    virtual int GetNoticeType() const = 0;
    virtual void Close() = 0;

    bool IsFinished() const { return mFiber.IsFinished(); }

protected:
    CLayout mLayout;
    CFiber mFiber;
};

class BOther : public BNoticeDisplayImpl {
public:
    BOther(int type) : mType(type) {}
    virtual ~BOther() {}
    virtual int GetNoticeType() const { return mType; }

protected:
    int mType;
    CLayout mSubLayout;
    int mEnemyImageCount;
};

class CSimpleText : public BOther {
public:
    CSimpleText(int type);
    virtual ~CSimpleText();
    virtual void Calc();
    virtual void Close();

    bool Init();

private:
    bool LoadLayouts();
    bool SetupLayouts();
    bool StartNotice();
    void Run();

    int mCloseRequested;
    int mAutoCloseFrames;
    int mFrameCount;
    bool mPauseReleased;
};

class CYouWin : public BOther {
public:
    CYouWin(int type, int winner, f32 winnerParam);
    virtual ~CYouWin();
    virtual void Close();

    bool Init();

private:
    void Run();

    int mWinner;
    f32 mWinnerParam;
    CLayout mWinnerLayout;
    CLayout mResultLayout;
};

class CFight : public BNoticeDisplayImpl {
public:
    CFight(int type);
    virtual ~CFight();
    virtual int GetNoticeType() const { return mType; }
    virtual void Close();

    bool Init();

private:
    void Run();

    int mType;
};

class CLose : public BNoticeDisplayImpl {
public:
    CLose(int type);
    virtual ~CLose();
    virtual int GetNoticeType() const { return mType; }
    virtual void Close();

    bool Init();

private:
    void Run();

    int mState;
    int mType;
};

class CNoticeDisplay : public INoticeDisplay {
public:
    CNoticeDisplay() : mWinner(0), mWinnerParam(0.0f) {}
    virtual ~CNoticeDisplay();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual bool IsShown() const;
    virtual void Show(int type);
    virtual void Close();
    virtual void Clear() {}
    virtual void SetWinner(int winner) { mWinner = winner; }
    virtual void SetWinnerParam(f32 param) { mWinnerParam = param; }
    virtual int GetType() const;

private:
    std::tr1::shared_ptr<BNoticeDisplayImpl> mpImpl;
    int mWinner;
    f32 mWinnerParam;
};

CNoticeDisplay::~CNoticeDisplay() {}

void BNoticeDisplayImpl::Calc() {
    mFiber.Resume();
    mLayout.Calc();
}

void BNoticeDisplayImpl::Draw() {
    mLayout.Draw();
}

void CNoticeDisplay::Calc() {
    if (mpImpl) {
        mpImpl->Calc();
        if (mpImpl->IsFinished()) {
            mpImpl.reset();
        }
    }
}

void CNoticeDisplay::CalcMtx() {
    if (mpImpl) {
        mpImpl->CalcMtx();
    }
}

void CNoticeDisplay::CalcView() {
    if (mpImpl) {
        mpImpl->CalcView();
    }
}

void CNoticeDisplay::Draw() {
    if (mpImpl) {
        mpImpl->Draw();
    }
}

int CNoticeDisplay::GetType() const {
    if (mpImpl) {
        return mpImpl->GetNoticeType();
    }
    return 10;
}

void CNoticeDisplay::Close() {
    if (mpImpl) {
        mpImpl->Close();
    }
}

#pragma dont_inline on
void CNoticeDisplay::Show(int type) {
    const MapDesc& rDesc = GetCurrentGameMap()->GetMapDesc();
    if (type == 0) {
        MapIndex index = rDesc.mIndex;
        switch (index.mStage) {
        case 0:
            type = 10;
            break;
        case 7:
            if (IsStage7EvenMap(index)) {
                type = 4;
            } else {
                type = 10;
            }
            break;
        default:
            type = 2;
            if (index.mMap <= 1) {
                type = 1;
            }
            break;
        }
    }

    if (type == 10) {
        GetCurrentGameMap()->SetPausing(false);
        Close();
        return;
    }

    if (mpImpl) {
        if (type == mpImpl->GetNoticeType()) {
            return;
        }
        mpImpl.reset();
    }

    GetCurrentGameMap()->SetPausing(true);

    switch (type) {
    case 1:
    case 2:
    case 3: {
        std::tr1::shared_ptr<CSimpleText> pNotice(new CSimpleText(type));
        if (pNotice->Init()) {
            mpImpl = pNotice;
        }
        break;
    }
    case 4: {
        std::tr1::shared_ptr<CFight> pNotice(new CFight(type));
        if (pNotice->Init()) {
            mpImpl = pNotice;
        }
        break;
    }
    case 9:
    case 5: {
        std::tr1::shared_ptr<CLose> pNotice(new CLose(type));
        if (pNotice->Init()) {
            mpImpl = pNotice;
        }
        break;
    }
    case 6:
    case 7:
    case 8: {
        std::tr1::shared_ptr<CYouWin> pNotice(new CYouWin(type, mWinner, mWinnerParam));
        if (pNotice->Init()) {
            mpImpl = pNotice;
        }
        break;
    }
    }

    if (!mpImpl) {
        GetCurrentGameMap()->SetPausing(false);
    }
}

#pragma dont_inline reset

CSimpleText::CSimpleText(int type)
    : BOther(type), mCloseRequested(0), mAutoCloseFrames(-1), mFrameCount(0), mPauseReleased(false) {}

inline bool CSimpleText::LoadLayouts() {
    const MapDesc& rDesc = GetCurrentGameMap()->GetMapDesc();
    std::string archive = FormatString("5:/stage/%s/common.arc/stage2d.arc", gStageWorlds[rDesc.mIndex.mStage].mName);
    std::tr1::shared_ptr<CFileData> pFile = LoadResourceFile((archive + "/arc/blyt/notice.brlyt").c_str(), 0, false);
    if (!pFile || !pFile->IsLoaded()) {
        return false;
    }

    std::tr1::shared_ptr<CLayout::FontMap> pFonts = GetDefaultFontMap();
    mLayout.Load(gSharedLayoutArchives, "notice", pFonts, false);
    mSubLayout.Load(archive.c_str(), "notice", pFonts, false);
    mLayout.SetPaneVisible("AREA", false);
    mLayout.SetPaneVisible("STAGE", false);
    mLayout.SetPaneVisible("ENEMY", false);
    mLayout.SetPaneVisible("START", false);
    mLayout.SetPaneVisible("CLEAR", false);
    mLayout.SetPaneVisible("YOUWIN", false);
    mLayout.SetText("AREA", GetMessageText(gStageWorlds[rDesc.mIndex.mStage].mMessageID, gEmptyText));
    mLayout.SetText("STAGE", FormatAreaName(rDesc.mIndex).c_str());

    const nw4r::lyt::Material* pSrcMaterial = mSubLayout.FindPane("BAR_AREA")->GetMaterial();
    const nw4r::lyt::TexMap* pSrcMap = pSrcMaterial->GetTexMapAry();
    nw4r::lyt::TexMap* pDstMap = mLayout.FindPane("BAR_AREA")->GetMaterial()->GetTexMapAry();
    *pDstMap = *pSrcMap;

    for (int i = 0; i < 9; i++) {
        nw4r::lyt::Pane* pSrcPane = mSubLayout.FindPane(gAreaPaneNames[i]);
        nw4r::lyt::Pane* pDstPane = mLayout.FindPane(gAreaPaneNames[i]);
        if (pSrcPane != nullptr && pDstPane != nullptr) {
            nw4r::lyt::TextBox* pSrcText = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(pSrcPane);
            nw4r::lyt::TextBox* pDstText = nw4r::ut::DynamicCast<nw4r::lyt::TextBox*>(pDstPane);
            if (pSrcText != nullptr && pDstText != nullptr) {
                pDstText->SetTextColor(0, pSrcText->GetTextColor(0));
                pDstText->SetTextColor(1, pSrcText->GetTextColor(1));
            }
        }
    }

    mEnemyImageCount = 0;
    nw4r::lyt::Pane* pPane;
    do {
        pPane = mLayout.FindPane(FormatString("ENEMY_IMG%d", mEnemyImageCount).c_str());
        if (pPane == nullptr) {
            break;
        }
        mLayout.SetVisible(pPane, false);
        mEnemyImageCount++;
    } while (pPane != nullptr);
    return true;
}

inline bool CSimpleText::SetupLayouts() {
    if (!LoadLayouts()) {
        return false;
    }

    switch (mType) {
    case 3:
        mLayout.SetPaneVisible("AREA", true);
        mLayout.SetPaneVisible("STAGE", true);
        mLayout.SetPaneVisible("CLEAR", true);
        break;
    case 1:
        mLayout.SetPaneVisible("AREA", true);
        mLayout.SetPaneVisible("STAGE", true);
    case 2:
        mAutoCloseFrames = 93;
        break;
    }
    return true;
}

inline bool CSimpleText::StartNotice() {
    if (!SetupLayouts()) {
        return false;
    }

    mFiber.Start(std::tr1::bind(&CSimpleText::Run, this), 0x8000);
    return true;
}

bool CSimpleText::Init() {
    return StartNotice();
}

}

std::tr1::shared_ptr<INoticeDisplay> CreateNoticeDisplay(int) {
    std::tr1::shared_ptr<CNoticeDisplay> pDisplay(new CNoticeDisplay);
    return pDisplay;
}
