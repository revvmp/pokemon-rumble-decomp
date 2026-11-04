#define CNOTICE_EMISSION_LAYOUT 1

#include <string>
#include <game/CNoticeDisplay.h>

#include <functional>
#include <game/CFiber.h>
#include <game/CBattleRoyalBox.h>
#include <game/CFileData.h>
#include <game/CLayout.h>
#include <game/FileList.h>
#include <game/IGameSession.h>
#include <game/IGameMap.h>
#include <game/IUIInput.h>
#include <game/LayoutResources.h>
#include <game/MapDesc.h>
#include <game/NIntl.h>
#include <game/Message.h>
#include <game/SaveData.h>
#include <game/StageID.h>
#include <game/StringFormat.h>
#include <game/Sound.h>
#include <game/SystemMessage.h>
#include <math.h>
#include <nw4r/lyt/lyt_material.h>
#include <nw4r/lyt/lyt_textBox.h>

namespace {
class BNoticeDisplayImpl;
}
namespace std { namespace tr1 {
template<> BNoticeDisplayImpl* shared_ptr<BNoticeDisplayImpl>::operator->() const;
}}

namespace std { namespace tr1 {
template<typename T> inline T* shared_ptr<T>::operator->() const {
    return ptr_;
}
}}
namespace NoticeMath {
extern "C" __attribute__((nothrow)) double ceil(double);
}

IUIInput* GetUIInput();
bool IsStage7EvenMap(MapIndex index);
bool IsBattleRoyalFirstPart(MapIndex index);

std::wstring FormatAreaName(MapIndex index) {
    std::wstring name;
    if (index.mMode == 2) {
        if (IsStage7EvenMap(index)) {
            typedef NIntl::basic_formatter<wchar_t> Formatter;
            Formatter formatter(GetMessageString(0x600E7, nullptr));
            int area = GetSaveData(-1)->mData.mExtraBRCurrentRound;
            int part = IsBattleRoyalFirstPart(index) ? 1 : 2;
            formatter % Formatter::format_spec(L"%d") % area;
            formatter % Formatter::format_spec(L"%d") % part;
            name = formatter.str().c_str();
        } else {
            name = GetMessageString(0x60002, nullptr);
        }
    } else {
        name = GetMessageString(gStageRanks[index.mArea].mMessageID, nullptr);
    }
    return name;
}

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

extern "C" const char* gAreaPaneNames[9] = {
    "AREA_nw", "AREA_n", "AREA_ne", "AREA_w", "AREA_c", "AREA_e", "AREA_sw", "AREA_s", "AREA_se",
};

namespace AreaPaneSymbols { extern "C" const char* gAreaPaneNames[]; }
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

class CLose : public BNoticeDisplayImpl {
public:
    inline CLose(int type);
    virtual int GetNoticeType() const { return mType; }
    virtual void Close() { mState = 1; }

    inline bool Init();

private:
    inline bool StartNotice();
    inline void Run();

    int mState;
    int mType;
};

class CFight : public BNoticeDisplayImpl {
public:
    inline CFight(int type);
    virtual int GetNoticeType() const { return mType; }
    virtual void Close() {}

    inline bool Init();

private:
    inline bool StartNotice();
    inline void Run();

    int mType;
};

class BOther : public BNoticeDisplayImpl {
public:
    BOther(int type) : mType(type) {}
    virtual ~BOther() {}
    virtual int GetNoticeType() const { return mType; }

protected:
    inline bool LoadLayouts();

    int mType;
    CLayout mSubLayout;
    int mEnemyImageCount;
};

class CSimpleText : public BOther {
public:
    inline CSimpleText(int type);
    virtual void Close() { mCloseRequested = 1; }
    virtual void Calc() {
        BNoticeDisplayImpl::Calc();
        mFrameCount++;
        if (mAutoCloseFrames > 0 && mAutoCloseFrames == mFrameCount && !mPauseReleased) {
            GetCurrentGameMap()->SetPausing(false);
            mPauseReleased = true;
        }
    }


    inline bool Init();

private:
    bool SetupLayouts();
    inline bool StartNotice();
    inline void Run();

    int mCloseRequested;
    int mAutoCloseFrames;
    int mFrameCount;
    bool mPauseReleased;
};

class CYouWin : public BOther {
public:
    inline CYouWin(int type, int winner, f32 winnerParam);
    virtual void Close() {}
    virtual void Calc() {
        BNoticeDisplayImpl::Calc();
        mResultLayout.Calc();
        mWinnerLayout.Calc();
    }
    virtual void Draw() {
        BNoticeDisplayImpl::Draw();
        mResultLayout.Draw();
        mWinnerLayout.Draw();
    }



    inline bool Init();

private:
    inline bool StartNotice();
    inline void Run();

    int mWinner;
    f32 mWinnerParam;
    CLayout mWinnerLayout;
    CLayout mResultLayout;
};

class CNoticeDisplay : public INoticeDisplay {
public:
    CNoticeDisplay() : mWinner(0), mWinnerParam(0.0f) {}
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual bool IsShown() const { return mpImpl.get() != nullptr; }
    virtual inline void Show(int type);
    virtual void Close();
    virtual void AddEnemy(const PiiSpeciesData*, int);
    virtual void SetWinner(int winner);
    virtual void SetWinnerParam(f32 param);
    virtual int GetType() const;

private:
    std::tr1::shared_ptr<BNoticeDisplayImpl> mpImpl;
    int mWinner;
    f32 mWinnerParam;
};

}


std::tr1::shared_ptr<INoticeDisplay> CreateNoticeDisplay(int) {
    std::tr1::shared_ptr<CNoticeDisplay> pDisplay(new CNoticeDisplay);
    return pDisplay;
}

namespace {

void CNoticeDisplay::Draw() {
    if (mpImpl) {
        mpImpl->Draw();
    }
}

void BNoticeDisplayImpl::Draw() {
    mLayout.Draw();
}
}
namespace std { namespace tr1 {
template<> BNoticeDisplayImpl* shared_ptr<BNoticeDisplayImpl>::operator->() const {
    return ptr_;
}
}}
namespace {



void CNoticeDisplay::CalcView() {
    if (mpImpl) {
        mpImpl->CalcView();
    }
}


void CNoticeDisplay::CalcMtx() {
    if (mpImpl) {
        mpImpl->CalcMtx();
    }
}


void CNoticeDisplay::Calc() {
    if (mpImpl) {
        mpImpl->Calc();
        if (mpImpl->IsFinished()) {
            mpImpl.reset();
        }
    }
}

void BNoticeDisplayImpl::Calc() {
    mFiber.Resume();
    mLayout.Calc();
}


int CNoticeDisplay::GetType() const {
    if (mpImpl) {
        return mpImpl->GetNoticeType();
    }
    return 10;
}


void CNoticeDisplay::SetWinnerParam(f32 param) {
    mWinnerParam = param;
}

void CNoticeDisplay::SetWinner(int winner) {
    mWinner = winner;
}

void CNoticeDisplay::AddEnemy(const PiiSpeciesData*, int) {}

void CNoticeDisplay::Close() {
    if (mpImpl) {
        mpImpl->Close();
    }
}


#pragma dont_inline on
inline void CNoticeDisplay::Show(int type) {
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

inline CSimpleText::CSimpleText(int type)
    : BOther(type), mCloseRequested(0), mAutoCloseFrames(-1), mFrameCount(0), mPauseReleased(false) {}

inline bool BOther::LoadLayouts() {
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
    mLayout.SetText("AREA", GetMessageString(gStageWorlds[rDesc.mIndex.mStage].mMessageID, nullptr));
    mLayout.SetText("STAGE", FormatAreaName(rDesc.mIndex).c_str());

    const nw4r::lyt::Material* pSrcMaterial = mSubLayout.FindPane("BAR_AREA")->GetMaterial();
    const nw4r::lyt::TexMap* pSrcMap = pSrcMaterial->GetTexMapAry();
    nw4r::lyt::TexMap* pDstMap = mLayout.FindPane("BAR_AREA")->GetMaterial()->GetTexMapAry();
    *pDstMap = *pSrcMap;

    for (int i = 0; i < 9; i++) {
        nw4r::lyt::Pane* pSrcPane = mSubLayout.FindPane(AreaPaneSymbols::gAreaPaneNames[i]);
        nw4r::lyt::Pane* pDstPane = mLayout.FindPane(AreaPaneSymbols::gAreaPaneNames[i]);
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



inline bool CSimpleText::Init() {
    return StartNotice();
}

inline void CSimpleText::Run() {
    mLayout.CreateMotion("begin");
    mLayout.QueueMotion("normal");
    std::tr1::shared_ptr<CLayout::CMotionProgram> pMotion = mLayout.AppendMotion("end");
    YieldFiberStarted();
    if (mType == 3) {
        PlaySound(0x5C);
    }

    while (true) {
        if (pMotion->IsEnded()) {
            break;
        }
        if (mPauseReleased) {
            IUIInput* pInput = GetUIInput();
            if (pInput != nullptr && pInput->IsConnected() && pInput->GetTrig()) {
                mCloseRequested = 1;
            }
        }
        if (mCloseRequested == 1 && !pMotion->IsStarted()) {
            pMotion = mLayout.CreateMotion("end");
        }
        mCloseRequested = 0;
        YieldFiber();
    }

    if (mAutoCloseFrames > 0 && !mPauseReleased) {
        GetCurrentGameMap()->SetPausing(false);
        mPauseReleased = true;
    }
}

inline CFight::CFight(int type) : mType(type) {}

inline bool CFight::Init() {
    return StartNotice();
}



inline void CFight::Run() {
    std::tr1::shared_ptr<CLayout::CMotionProgram> pBegin = mLayout.CreateMotion("begin");
    std::tr1::shared_ptr<CLayout::CMotionProgram> pThree = mLayout.AppendMotion("3");
    std::tr1::shared_ptr<CLayout::CMotionProgram> pTwo = mLayout.AppendMotion("2");
    std::tr1::shared_ptr<CLayout::CMotionProgram> pOne = mLayout.AppendMotion("1");
    std::tr1::shared_ptr<CLayout::CMotionProgram> pEnd = mLayout.AppendMotion("end");
    YieldFiberStarted();
    pBegin->Wait();
    PlaySound(0x5B);
    pThree->Wait();
    PlaySound(0x5B);
    pTwo->Wait();
    PlaySound(0x5B);
    pOne->Wait();
    PlaySound(0x32E);
    GetCurrentGameMap()->SetPausing(false);
    pEnd->Wait();
}

inline CLose::CLose(int type) : mState(0), mType(type) {}

inline bool CLose::Init() {
    return StartNotice();
}



inline void CLose::Run() {
    mLayout.CreateMotion("begin");
    mLayout.QueueMotion("normal");
    std::tr1::shared_ptr<CLayout::CMotionProgram> pMotion = mLayout.AppendMotion("end");
    YieldFiberStarted();
    PlaySound(0x5C);

    while (true) {
        if (pMotion->IsEnded()) {
            break;
        }
        if (mState == 1 && !pMotion->IsStarted()) {
            pMotion = mLayout.CreateMotion("end");
        }
        mState = 0;
        YieldFiber();
    }

    IGameSession::GetInstance()->SetFade(1, 0);
}

inline CYouWin::CYouWin(int type, int winner, f32 winnerParam) : BOther(type), mWinner(winner), mWinnerParam(winnerParam) {}

inline bool CYouWin::Init() {
    return StartNotice();
}



inline void CYouWin::Run() {
    mLayout.CreateMotion("begin", "MISC", 0.0f, 1.0f);
    mLayout.CreateMotion("begin", "TEXT", 0.0f, 1.0f);
    mLayout.QueueMotion("normal", "MISC", 0.0f, 1.0f);
    mLayout.QueueMotion("normal", "TEXT", 0.0f, 1.0f);
    std::tr1::shared_ptr<CLayout::CMotionProgram> pMotion = mLayout.AppendMotion("end", "TEXT", 0.0f, 1.0f);
    YieldFiberStarted();
    PlaySound(0x5C);
    WaitMotionStart(pMotion);
    if (!pMotion->IsStarted()) {
        pMotion = mLayout.CreateMotion("end", "TEXT", 0.0f, 1.0f);
    }
    pMotion->Wait();

    if (mType == 7 || mType == 8) {
        YieldFiber();
        if (mType == 8) {
            PlaySound(0x5C);
            mWinnerLayout.SetVisible(true);
            mWinnerLayout.CreateMotion("begin");
            mWinnerLayout.QueueMotion("normal");
            pMotion = mWinnerLayout.AppendMotion("end");
            WaitMotionStart(pMotion);
            if (!pMotion->IsStarted()) {
                pMotion = mWinnerLayout.CreateMotion("end");
            }
            pMotion->Wait();
            mWinnerLayout.SetVisible(false);
        }

        PlaySound(0x5C);
        mResultLayout.SetVisible(true);
        mResultLayout.CreateMotion("begin")->Wait();
        mResultLayout.QueueMotion("normal");
        nw4r::lyt::TextBox* pMoney = mResultLayout.SetText("MONEY", FormatString(L"%d", mWinner).c_str());
        nw4r::ut::Rect rect = pMoney->GetTextDrawRect();
        nw4r::lyt::Pane* pAdjust = mResultLayout.FindPane("MONEY_X_ADJUST");
        pAdjust->SetTranslate(nw4r::math::VEC3((rect.right - rect.left) / 2.0f, 0.0f, 0.0f));

        int shown = 0;
        int place = 1;
        while (shown < mWinner) {
            int next = place * 10;
            int limit = mWinner % next;
            if (limit / place == 0 && limit + next <= mWinner) {
                limit += next;
            }
            while (shown < limit) {
                shown += place;
                std::wstring text = FormatString(L"%d", shown);
                mResultLayout.SetText("MONEY", text.c_str());
                mResultLayout.SetText("MONEY_SHADOW", text.c_str());
                YieldFiber();
                YieldFiber();
                YieldFiber();
            }
            place *= 10;
        }

        std::wstring total = FormatString(L"%d", mWinner);
        mResultLayout.SetText("MONEY", total.c_str());
        mResultLayout.SetText("MONEY_SHADOW", total.c_str());
        pMotion = mResultLayout.AppendMotion("end");
        WaitMotionStart(pMotion);
        if (!pMotion->IsStarted()) {
            mResultLayout.CreateMotion("end");
        }
    }

    IGameSession::GetInstance()->SetFade(1, -1);
    mLayout.CreateMotion("end", "MISC", 0.0f, 1.0f)->Wait();
    while (IGameSession::GetInstance()->GetFadeFrames()) {
        YieldFiber();
    }
}

}


namespace std { namespace tr1 {
template<typename T> template<typename Y>
shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr<Y>& r) {
    shared_ptr(r).swap(*this);
    return *this;
}
}}

namespace {
inline bool CLose::StartNotice() {
    std::tr1::shared_ptr<CLayout::FontMap> pFonts = GetDefaultFontMap();
    mLayout.Load(gSharedLayoutArchives, mType == 9 ? "timeup" : "lose", pFonts, false);
    mFiber.Start(std::tr1::bind(&CLose::Run, this), 0x8000);
    return true;
}
inline bool CFight::StartNotice() {
    void* pEntry;
    std::tr1::shared_ptr<FileList> pFiles = LoadFileList(&pEntry, "event/br_count.arc/event2d.arc", 0, false, nullptr);
    const MapDesc& rDesc = GetCurrentGameMap()->GetMapDesc();
    std::tr1::shared_ptr<CLayout::FontMap> pFonts = GetDefaultFontMap();
    mLayout.Load(static_cast<const char* const*>(pEntry), "beginning_count", pFonts, false);
    mLayout.SetText("AREA", GetMessageString(gStageWorlds[rDesc.mIndex.mStage].mMessageID, nullptr));
    mLayout.SetText("GRADE", FormatAreaName(rDesc.mIndex).c_str());
    mFiber.Start(std::tr1::bind(&CFight::Run, this), 0x8000);
    return true;
}
inline bool CSimpleText::StartNotice() {
    if (!SetupLayouts()) {
        return false;
    }

    mFiber.Start(std::tr1::bind(&CSimpleText::Run, this), 0x8000);
    return true;
}
inline bool CYouWin::StartNotice() {
    void* pEntry;
    int minutes;
    int seconds;
    int hundredths;
    if (!LoadLayouts()) {
        return false;
    }

    std::tr1::shared_ptr<CLayout::FontMap> pFonts = GetDefaultFontMap();
    std::tr1::shared_ptr<FileList> pFiles = LoadFileList(&pEntry, "event/br_end.arc/event2d.arc", 0, false, nullptr);
    mWinnerLayout.Load(static_cast<const char* const*>(pEntry), "clear_time", pFonts, false);

    std::wstring clearTime;
    int total = static_cast<int>(static_cast<float>(NoticeMath::ceil(mWinnerParam * 100.0f)));
    minutes = total / 100 / 60;
    seconds = total / 100 % 60;
    hundredths = total % 100;
    {
        typedef NIntl::basic_formatter<wchar_t> Formatter;
        Formatter formatter(GetMessageString(0x600E4, nullptr));
        formatter % Formatter::format_spec(L"%d") % minutes;
        formatter % Formatter::format_spec(L"%02d") % seconds;
        formatter % Formatter::format_spec(L"%02d") % hundredths;
        clearTime = formatter.str().c_str();
    }

    mWinnerLayout.SetText("CLEARTIME", GetMessageString(0x600E3, nullptr));
    mWinnerLayout.SetText("TIME", clearTime.c_str());
    mWinnerLayout.SetText("TIME_SHADOW", clearTime.c_str());
    mWinnerLayout.SetText("NEWRECORD", GetMessageString(0x600E5, nullptr));

    const MapDesc& rDesc = GetCurrentGameMap()->GetMapDesc();
    bool eligible;
    int mode = rDesc.mIndex.mMode;
    int area = rDesc.mIndex.mArea;
    if (mode == 2) {
        area = GetSaveData(-1)->mData.mExtraBRCurrentRound;
    }
    bool newRecord = false;
    eligible = true;
    if (mode == 2) {
        MapIndex index = rDesc.mIndex;
        if (IsBattleRoyalFirstPart(index)) {
            eligible = false;
        }
    }
    if (eligible) {
        std::tr1::shared_ptr<CBattleRoyalData> pData = IGameSession::GetInstance()->GetBattleRoyalBox()->GetData(mode, area);
        newRecord = pData->UpdateBestTime(mWinnerParam);
    }
    mWinnerLayout.SetPaneVisible("NEWRECORD", newRecord);

    mResultLayout.Load(static_cast<const char* const*>(pEntry), "prize", pFonts, false);
    mResultLayout.SetText("PRIZE", GetMessageString(0x600E6, nullptr));
    mResultLayout.SetText("MONEY", L"");
    mResultLayout.SetText("MONEY_SHADOW", L"");

    mLayout.SetPaneVisible("AREA", true);
    mLayout.SetPaneVisible("STAGE", true);
    mLayout.SetPaneVisible("YOUWIN", true);
    mWinnerLayout.SetVisible(false);
    mResultLayout.SetVisible(false);
    mFiber.Start(std::tr1::bind(&CYouWin::Run, this), 0x8000);
    return true;
}
}
