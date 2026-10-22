#include <game/IDlgPiiSelector.h>

#include <game/ButtonMessage.h>
#include <game/CColorBtnTagProc.h>
#include <game/CFiber.h>
#include <game/CLayout.h>
#include <game/CNoticeDisplay.h>
#include <game/ControllerTable.h>
#include <game/CPiiListFilterSort.h>
#include <game/CPlayer.h>
#include <game/CPlayerPii.h>
#include <game/BWorld.h>
#include <game/CRemoteMemory.h>
#include <game/CUIManager.h>
#include <game/CUIPiiList.h>
#include <game/CUIPiiListItem.h>
#include <game/CUIPiiSpec.h>
#include <game/CUIPlayerID.h>
#include <game/IDisconMsg.h>
#include <game/IDlgMsgWin.h>
#include <game/IGameMap.h>
#include <game/IGameSession.h>
#include <game/LayoutResources.h>
#include <game/MapDesc.h>
#include <game/MathUtil.h>
#include <game/Message.h>
#include <game/Sound.h>
#include <game/CStatistic.h>
#include <game/StringFormat.h>
#include <memory>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <nw4r/math/math_types.h>
#include <vector>

#pragma dont_inline on

namespace {

int sMaxPlayerNum = 4;

class CControllerList {
public:
    CControllerList() {
        mCount = 0;
        for (int i = 0; i < 4; i++) {
            mControllers[i] = nullptr;
        }
    }

    void Collect() {
        mCount = 0;
        for (int channel = 0; channel < 4; channel++) {
            IController* pController = GetController(IController::TYPE_GC_PAD, channel);
            mControllers[channel] = pController;
            if (pController != nullptr && pController->IsConnected()) {
                mCount++;
            } else {
                mControllers[channel] = nullptr;
            }
        }
        if (mCount < 4 && !AddType(IController::TYPE_CLASSIC) && !AddType(IController::TYPE_WII_REMOTE)) {
            AddType(IController::TYPE_NUNCHAKU);
        }
        if (mCount == 0) {
            mControllers[0] = GetController(IController::TYPE_WII_REMOTE, 0);
            mCount++;
        }
    }

    bool AddType(IController::Type type) {
        for (int channel = 0; channel < 4; channel++) {
            if (Add(GetController(type, channel))) {
                return true;
            }
        }
        return false;
    }

    bool Add(IController* pController) {
        if (mCount == 4) {
            return true;
        }
        if (pController == nullptr || !pController->IsConnected()) {
            return false;
        }
        for (int i = 0; i < 4; i++) {
            if (mControllers[i] == nullptr) {
                mControllers[i] = pController;
                return ++mCount == 4;
            }
        }
        return false;
    }

    IController* Get(int index) { return mControllers[index]; }

private:
    int mCount;
    IController* mControllers[4];
};

class IDlgPiiSelectorInternal;

typedef std::vector<std::tr1::shared_ptr<CUIPiiListItem> > PiiItemVector;

bool LoadItems(PiiItemVector& rItems, int player);
bool IsCurrentMapStage7Even();

struct SwapRequest {
    std::tr1::shared_ptr<CPiiPersonalData> mPii;
    nw4r::math::VEC3 mFrom;
    nw4r::math::VEC3 mTo;
};

class CDataPerPlayer {
public:
    CDataPerPlayer(IDlgPiiSelectorInternal* pDlg, int state, int player)
        : mpDlg(pDlg), mState(state), mPlayer(player), mMode(0), mFavoriteMode(false), mReservedPosition(0.0f, 0.0f),
          mReservedTargetPosition(0.0f, 0.0f), mOpen(false), mSilent(false), mInputDisabled(false),
          mReserved6C(0), mReserved70(0), mReserved74(0), mReserved78(""), mReserved7C(""), mReserved80(""),
          mUIManager(new CUIManager), mRootLayout(new BUILayout), mListLayout(new BUILayout),
          mPiiSpec(new CUIPiiSpec), mPiiList(new CUIPiiList), mPlayerID(new CUIPlayerID), mpController(nullptr),
          mDisconnectSerial(0), mDisconnected(false), mEntranceResult(0), mFiber("CDataPerPlayer") {}

    void Draw() { mUIManager->Draw(); }
    void CalcMtx() { mUIManager->CalcMtx(); }
    void CalcView() { mUIManager->CalcView(); }

    void Calc() {
        if (mState == 2) {
            IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
            if (pController == nullptr || mpController != pController ||
                mDisconnectSerial != pController->GetDisconnectSerial()) {
                mDisconnected = true;
            }
            if (!IsDisconnected()) {
                mPlayerID->Update();
            }
        }
        mPiiList->UpdateSelection();
        {
            std::tr1::shared_ptr<IUIListItem> selected = mPiiList->GetSelectedItem();
            if (selected) {
                std::tr1::shared_ptr<CUIPiiListItem> item = std::tr1::static_pointer_cast<CUIPiiListItem>(selected);
                mPiiSpec->SetPii(item->GetPPD(), mPlayer);
            }
        }
        mUIManager->Calc();
        mFiber.Resume();
    }

    void Setup() { Setup(false); }
    void Setup(bool background);
    void RequestClose() { mMode = 5; }
    bool IsDisconnected() { return mDisconnected; }
    bool IsFinished();
    bool IsShown();
    void OnSelectionChanged(BUI* pUI);
    void OnAccept(BUI* pUI) { mMode = 3; }
    void OnSortChanged(const CPiiListFilterSort* pFilterSort);
    void Run();
    void RunDialog();
    void RunSelection();
    void RunEntrance();
    void RunEntranceController();
    void FlushRemoteMemory();
    void WaitForRemoteMemory();
    void SetOpen(bool open);
    int TakeMode() {
        int mode = mMode;
        mMode = 0;
        return mode;
    }
    void UpdateList(bool selectFirst);
    void SelectEntrancePii();
    std::tr1::shared_ptr<CLayout::CMotionProgram> PlayMotion(std::tr1::shared_ptr<BUILayout> pLayout, bool shown);
    int ValidateAccept();
    void ApplyAccept();
    void ProcessEntranceAccept();
    void FinishAccept();
    bool CanSwap();
    void PrepareSwap(SwapRequest& rRequest);
    void ExecuteSwap(const SwapRequest& rRequest);

private:
    IDlgPiiSelectorInternal* mpDlg;
    int mState;
    int mPlayer;
    int mMode;
    bool mFavoriteMode;
    PiiItemVector mItems;
    CPiiListFilterSort mFilterSort;
    nw4r::math::VEC2 mReservedPosition;
    nw4r::math::VEC2 mReservedTargetPosition;
    bool mOpen;
    bool mSilent;
    bool mInputDisabled;
    std::tr1::weak_ptr<IUIListItem> mReservedWeak54;
    std::tr1::weak_ptr<BUI> mReservedWeak5C;
    std::tr1::weak_ptr<BUI> mReservedWeak64;
    int mReserved6C;
    int mReserved70;
    int mReserved74;
    const char* mReserved78;
    const char* mReserved7C;
    const char* mReserved80;
    std::tr1::shared_ptr<CUIManager> mUIManager;
    std::tr1::shared_ptr<BUILayout> mRootLayout;
    std::tr1::shared_ptr<BUILayout> mListLayout;
    std::tr1::shared_ptr<CUIPiiSpec> mPiiSpec;
    std::tr1::shared_ptr<CUIPiiList> mPiiList;
    std::tr1::shared_ptr<CUIPlayerID> mPlayerID;
    std::tr1::shared_ptr<BUILayout> mHelpLayout;
    bool mSpecShown;
    bool mListShown;
    bool mPlayerIDShown;
    bool mHelpShown;
    bool mReservedFlagC0;
    IController* mpController;
    u32 mDisconnectSerial;
    bool mDisconnected;
    int mEntranceResult;
    CFiber mFiber;

public:
    virtual ~CDataPerPlayer();
};

void CDataPerPlayer::Setup(bool background) {
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    const char* const* pArchives = gSharedLayoutArchives;
    if (mState == 1) {
        mHelpLayout.reset(new BUILayout);
        mRootLayout->Load(pArchives, "dlgpiiselt_root", fonts, background);
        mListLayout->Load(pArchives, "dlgpiilist_root", fonts, background);
        mPiiSpec->Load(pArchives, "piispec2", fonts, background);
        mPiiList->Load(pArchives, "dlgpiilist_list", fonts, background);
        mPlayerID->Setup(mPlayer, 0, pArchives, "playerid", fonts, background);
        mHelpLayout->Load(pArchives, "dlgpiilist_help", fonts, background);
        mUIManager->AddChild(mRootLayout);
        mRootLayout->AttachChild("DPL_ROOT", mListLayout);
        mListLayout->AttachChild("PIISPEC", mPiiSpec);
        mListLayout->AttachChild("LIST", mPiiList);
        mListLayout->AttachChild("PLAYERID", mPlayerID);
        mListLayout->AttachChild("HELP", mHelpLayout);
        CColorBtnTagProc::Attach(mHelpLayout->GetLayout()->SetText(
            "HELP", (GetButtonMessage(mPlayer, 8, 0x600A4) + GetButtonMessage(mPlayer, 0x20, 0x600A2) +
                     GetButtonMessage(mPlayer, 0x10, 0x600A0) + GetButtonMessage(mPlayer, 0x100, 0x600A6) +
                     GetButtonMessage(mPlayer, 0x800, 0x600A5)).c_str()));
    } else {
        if (mState == 0) {
            int joined = 0;
            for (int player = 0; player < 4; player++) {
                if (IGameSession::GetInstance()->GetPlayer(player)->IsJoined()) {
                    joined++;
                }
            }
            if (joined == 1) {
                mHelpLayout.reset(new BUILayout);
            }
        }
        mRootLayout->Load(pArchives, "dlgpiisel_root", fonts, background);
        mListLayout->Load(pArchives, "dlgpiisel_p_root", fonts, background);
        mPiiSpec->Load(pArchives, "piispec", fonts, background);
        mPiiList->Load(pArchives, "dlgpiisel_list", fonts, background);
        if (mHelpLayout) {
            mHelpLayout->Load(pArchives, "dlgpiisel_help", fonts, background);
        }
        if (mState == 2) {
            mPlayerID->Setup(mPlayer, 1, pArchives, "playerid", fonts, background);
        } else {
            mPlayerID->Setup(mPlayer, 0, pArchives, "playerid", fonts, background);
        }
        mUIManager->AddChild(mRootLayout);
        mRootLayout->AttachChild(FormatString("P%d", mPlayer).c_str(), mListLayout);
        mListLayout->AttachChild("PIISPEC", mPiiSpec);
        mListLayout->AttachChild("LIST", mPiiList);
        mListLayout->AttachChild("PLAYERID", mPlayerID);
        if (mHelpLayout) {
            std::wstring leave = GetButtonMessage(mPlayer, 0x10, 0x600A0);
            CPlayerPii* pPii = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetPii();
            if (pPii != nullptr) {
                if (pPii->GetPersonalData()->IsFainted()) {
                    leave.clear();
                }
            }
            mListLayout->AttachChild(FormatString("HELP%d", mPlayer).c_str(), mHelpLayout);
            CColorBtnTagProc::Attach(mHelpLayout->GetLayout()->SetText(
                "HELP", (GetButtonMessage(mPlayer, 8, 0x600A4) + GetButtonMessage(mPlayer, 0x20, 0x600A2) + leave +
                         GetButtonMessage(mPlayer, 0x800, 0x600A5)).c_str()));
        }
    }
    mPiiList->GetSelectionCallbacks().Add(std::tr1::bind(&CDataPerPlayer::OnSelectionChanged, this, std::tr1::placeholders::_1));
    mPiiList->GetAcceptCallbacks().Add(std::tr1::bind(&CDataPerPlayer::OnAccept, this, std::tr1::placeholders::_1));
    mPiiList->SetScrollMode(1);
    mPiiList->SetSelectionInputEnabled(true);
    mPiiSpec->SetVisible(false);
    mPiiList->SetVisible(false);
    mPlayerID->SetVisible(false);
    mSpecShown = false;
    mListShown = false;
    mPlayerIDShown = false;
    mHelpShown = false;
    mReservedFlagC0 = false;
    CLayout* pLayout = mListLayout->GetLayout();
    pLayout->CalculateMtx(true);
    nw4r::math::VEC3 position;
    position = Transform(pLayout->FindPane("INSIDE")->GetGlobalMtx(), nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
    mReservedTargetPosition = pLayout->NormalizeViewPosition(nw4r::math::VEC2(position.x, position.y));
    position = Transform(pLayout->FindPane("OUTSIDE")->GetGlobalMtx(), nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
    mReservedPosition = pLayout->NormalizeViewPosition(nw4r::math::VEC2(position.x, position.y));
    mFiber.Start(std::tr1::bind(&CDataPerPlayer::Run, this), 0x8000);
}

class IDlgPiiSelectorInternal : public IDlgPiiSelector {
public:
    virtual ~IDlgPiiSelectorInternal() {}
    virtual std::tr1::shared_ptr<CPiiPersonalData> GetEntrancePPD(IController* pController) = 0;
    virtual void UpdateDlgOpenState() = 0;
};

void CDataPerPlayer::Run() {
    switch (mState) {
    case 0:
    case 1:
        YieldFiberStarted();
        RunDialog();
        break;
    case 2:
        RunEntrance();
        break;
    }
    SetOpen(false);
}

void CDataPerPlayer::RunDialog() {
    WaitForRemoteMemory();
    mpController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
    mDisconnectSerial = mpController != nullptr ? mpController->GetDisconnectSerial() : 0;
    if (LoadItems(mItems, mPlayer)) {
        SetOpen(true);
        RunSelection();
    }
    PlayMotion(mPiiSpec, false);
    PlayMotion(mPlayerID, false);
    PlayMotion(mHelpLayout, false);
    std::tr1::shared_ptr<CLayout::CMotionProgram> motion = PlayMotion(mPiiList, false);
    if (motion) {
        motion->Wait();
    }
}

void CDataPerPlayer::WaitForRemoteMemory() {
    IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
    IController::Type type = pController->GetType();
    if (type >= IController::TYPE_WII_REMOTE && type <= IController::TYPE_NUNCHAKU) {
        IGameSession::GetInstance()->GetRemoteMemory(pController->GetChannel())->WaitUntilIdle(true);
    }
}

void CDataPerPlayer::SetOpen(bool open) {
    mOpen = open;
    mpDlg->UpdateDlgOpenState();
}

void CDataPerPlayer::RunSelection() {
    mSilent = true;
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
    UpdateList(false);
    mPlayerID->Update();
    SelectEntrancePii();
    PlaySound(40);
    PlayMotion(mPiiSpec, true);
    PlayMotion(mPlayerID, true);
    PlayMotion(mHelpLayout, true);
    {
        std::tr1::shared_ptr<CLayout::CMotionProgram> motion = PlayMotion(mPiiList, true);
        if (motion) {
            motion->Wait();
        }
    }
    mUIManager->SetFocusedUI(mPiiList.get());
    mSilent = false;
    for (;;) {
        IController* pController = pPlayer->GetController();
        if (IsDisconnected()) {
            if (!IsCurrentMapStage7Even()) {
                mMode = 4;
            }
        } else if (pController == nullptr || !pController->IsConnected()) {
            if (mState == 2) {
                mMode = 4;
            }
        } else if (!mInputDisabled) {
            if (pController->GetTrig() & 0x90) {
                do {
                    if (IsCurrentMapStage7Even()) {
                        break;
                    }
                    CPlayerPii* pPii = pPlayer->GetPii();
                    if (pPii != nullptr) {
                        if (pPii->GetPersonalData()->IsFainted()) {
                            break;
                        }
                    }
                    mpDlg->SetEntrancePPD(pController, 0);
                    mMode = 4;
                } while (false);
                PlaySound(90);
            } else if (pController->GetTrig() & 0x800) {
                mMode = 6;
            } else if (pController->GetTrig() & 0x100) {
                if (mState == 1) {
                    mMode = 7;
                }
            } else {
                mUIManager->DispatchFocusedInput(pController);
            }
        }
        switch (TakeMode()) {
        case 5:
            IGameSession::GetInstance()->GetStatistic()->Add(-99, 1);
            PlaySound(41);
            return;
        case 4:
            if (!IsCurrentMapStage7Even()) {
                PlaySound(41);
                return;
            }
            break;
        case 7:
            YieldFiber();
            mFilterSort.ShowSortMenu(mUIManager, mPlayer, mItems,
                                     std::tr1::bind(&CDataPerPlayer::OnSortChanged, this, std::tr1::placeholders::_1));
            break;
        case 6: {
            std::tr1::shared_ptr<IUIListItem> selected = mPiiList->GetSelectedItem();
            if (selected) {
                std::tr1::shared_ptr<CUIPiiListItem> item = std::tr1::static_pointer_cast<CUIPiiListItem>(selected);
                bool favorite = !item->GetPPD()->IsFavorite();
                item->GetPPD()->SetFavorite(favorite);
                if (favorite) {
                    PlaySound(9);
                } else {
                    PlaySound(90);
                }
                mFavoriteMode = favorite;
            }
            break;
        }
        case 3: {
            switch (ValidateAccept()) {
            case 2:
                PlaySound(41);
                return;
            case 0:
                ApplyAccept();
                if (mState == 2) {
                    ProcessEntranceAccept();
                    if (mEntranceResult != 2) {
                        if (!IsDisconnected()) {
                            PlayMotion(mPiiList, true);
                        }
                        FinishAccept();
                        break;
                    }
                }
                if (CanSwap()) {
                    SwapRequest request;
                    PrepareSwap(request);
                    SetOpen(false);
                    ExecuteSwap(request);
                }
                SetOpen(false);
                PlayMotion(mPiiSpec, false);
                PlayMotion(mPiiList, false);
                PlayMotion(mPlayerID, false);
                PlayMotion(mHelpLayout, false);
                FinishAccept();
                return;
            }
            break;
        }
        }
        YieldFiber();
    }
}

void CDataPerPlayer::UpdateList(bool selectFirst) {
    int player = mState == 2 ? -1 : mPlayer;
    mFilterSort.Filter(mPiiList, mItems, player);
    if (selectFirst) {
        mPiiList->SelectIndex(0);
    }
}

void CDataPerPlayer::SelectEntrancePii() {
    IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
    {
        std::tr1::shared_ptr<CPiiPersonalData> entrance = mpDlg->GetEntrancePPD(pController);
        if (entrance) {
            int index = 0;
            for (PiiItemVector::iterator it = mItems.begin(); it != mItems.end(); ++it) {
                if ((*it)->GetPPD() == entrance) {
                    mPiiList->SetSelectedIndex(index);
                    return;
                }
                index++;
            }
        }
    }
    mPiiList->SelectIndex(0);
}

void CDataPerPlayer::RunEntrance() {
    SetOpen(true);
    mEntranceResult = 0;
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
    mItems.clear();
    PlayMotion(mPiiSpec, false);
    PlayMotion(mPiiList, false);
    PlayMotion(mPlayerID, false);
    YieldFiberStarted();
    for (;;) {
        IController* pController = pPlayer->GetController();
        if (pController != nullptr && pController->IsConnected() && !mInputDisabled) {
            mpController = pController;
            mDisconnectSerial = pController->GetDisconnectSerial();
            mDisconnected = false;
            RunEntranceController();
        }
        if (mEntranceResult == 2) {
            PlayMotion(mPiiSpec, false);
            PlayMotion(mPiiList, false);
            PlayMotion(mPlayerID, false);
            break;
        }
        YieldFiber();
    }
}

void CDataPerPlayer::RunEntranceController() {
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
    IController* pController;
    PlayMotion(mPlayerID, true);
    {
        std::tr1::shared_ptr<CLayout::CMotionProgram> motion = PlayMotion(mPlayerID, true);
        if (motion) {
            motion->Wait();
        }
    }
    FlushRemoteMemory();
    if (!IsDisconnected()) {
        mItems.resize(mPlayer);
        for (;;) {
            pController = pPlayer->GetController();
            if (mEntranceResult == 2) {
                mMode = 4;
            } else if (IsDisconnected()) {
                mMode = 4;
            } else if (pController != nullptr && pController->IsConnected()) {
                if (!mInputDisabled) {
                    if (pController->GetTrig() & 0x20) {
                        mMode = 2;
                        PlaySound(9);
                    } else {
                        bool hasEntrance = mpDlg->GetEntrancePPD(pController);
                        if (hasEntrance) {
                            mMode = 2;
                        }
                    }
                }
            } else {
                mMode = 4;
            }
            int mode = TakeMode();
            if (static_cast<u32>(mode - 4) <= 1) {
                break;
            }
            if (mode == 2) {
                pPlayer->Join();
                RunSelection();
                if (mEntranceResult == 2) {
                    return;
                }
                pPlayer->Leave();
                PlayMotion(mPiiSpec, false);
                std::tr1::shared_ptr<CLayout::CMotionProgram> motion = PlayMotion(mPiiList, false);
                if (motion) {
                    motion->Wait();
                }
            }
            YieldFiber();
        }
    }
    std::tr1::shared_ptr<CLayout::CMotionProgram> motion = PlayMotion(mPlayerID, false);
    if (motion) {
        motion->Wait();
    }
}

void CDataPerPlayer::FlushRemoteMemory() {
    IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
    IController::Type type = pController->GetType();
    if (type >= IController::TYPE_WII_REMOTE && type <= IController::TYPE_NUNCHAKU) {
        CRemoteMemory* pRemoteMemory = IGameSession::GetInstance()->GetRemoteMemory(pController->GetChannel());
        while (pRemoteMemory->IsBusy()) {
            YieldFiber();
            if (IsDisconnected()) {
                return;
            }
        }
        pRemoteMemory->Update();
        while (pRemoteMemory->IsBusy()) {
            YieldFiber();
            if (IsDisconnected()) {
                return;
            }
        }
    }
}

class CDlgPiiSelector : public IDlgPiiSelectorInternal, public std::tr1::enable_shared_from_this<CDlgPiiSelector> {
public:
    struct MultiPlayerData {
        int mState;
        std::tr1::shared_ptr<CPiiPersonalData> mEntrancePPD[4][4];
        int mPlayerNum;
        CLayout mLayout;
        std::tr1::shared_ptr<IDlgMsgWin> mMsgWin;
        std::tr1::shared_ptr<IDisconMsg::CDisable> mDisconMsgDisable;
    };

    CDlgPiiSelector() : mIsPausing(false), mInputLocked(false), mErrorState(0), mError(0) {}
    virtual ~CDlgPiiSelector() {}
    virtual void Calc() {
        if (mMultiPlayerData) {
            CalcEntrance();
        }
        for (int player = 0; player < 4; player++) {
            if (mDataPerPlayer[player]) {
                mDataPerPlayer[player]->Calc();
                if (mDataPerPlayer[player]->IsFinished()) {
                    mDataPerPlayer[player].reset();
                    if (mIsPausing) {
                        mIsPausing = false;
                        GetCurrentGameMap()->SetPausing(false);
                    }
                    UpdateDlgOpenState();
                }
            }
        }
        if (mMultiPlayerData) {
            UpdateEntrance();
            if (mMultiPlayerData) {
                mMultiPlayerData->mLayout.Calc();
                mMultiPlayerData->mMsgWin->Calc();
            }
        }
    }

    virtual void CalcMtx() {
        for (int player = 0; player < 4; player++) {
            if (mDataPerPlayer[player]) {
                mDataPerPlayer[player]->CalcMtx();
            }
        }
        if (mMultiPlayerData) {
            mMultiPlayerData->mMsgWin->CalcMtx();
        }
    }

    virtual void CalcView() {
        for (int player = 0; player < 4; player++) {
            if (mDataPerPlayer[player]) {
                mDataPerPlayer[player]->CalcView();
            }
        }
        if (mMultiPlayerData) {
            mMultiPlayerData->mMsgWin->CalcView();
        }
    }

    virtual void Draw() {
        if (GetCurrentGameMap()->GetNoticeDisplay()->IsShown()) {
            return;
        }
        for (int player = 0; player < 4; player++) {
            if (mDataPerPlayer[player]) {
                mDataPerPlayer[player]->Draw();
            }
        }
        if (mMultiPlayerData) {
            mMultiPlayerData->mLayout.Draw();
            mMultiPlayerData->mMsgWin->Draw();
        }
    }

    virtual bool IsClosable() { return false; }
    virtual bool Close() { return false; }
    virtual bool IsShown(int player) {
        if (mIsPausing) {
            return true;
        }
        return !mMultiPlayerData && mDataPerPlayer[player] && mDataPerPlayer[player]->IsShown();
    }

    virtual void Show(int player, bool flag) {
        if (mMultiPlayerData) {
            return;
        }
        if (flag) {
            if (GetCurrentGameMap()->GetMapDesc().mIndex.mStage == 0) {
                if (IsAnyShown()) {
                    return;
                }
                flag = true;
                GetCurrentGameMap()->SetPausing(true);
                mIsPausing = true;
            } else {
                flag = false;
            }
            if (!mDataPerPlayer[player]) {
                mDataPerPlayer[player].reset(new CDataPerPlayer(this, flag, player));
                mDataPerPlayer[player]->Setup();
            }
        } else if (mDataPerPlayer[player]) {
            mDataPerPlayer[player]->RequestClose();
        }
    }

    virtual bool IsMultiPlayerEntranceShown() { return mMultiPlayerData; }
    virtual void ShowMultiPlayerEntrance() {
        if (mMultiPlayerData) {
            return;
        }
        for (int player = 0; player < 4; player++) {
            mDataPerPlayer[player].reset();
        }
        GetCurrentGameMap()->SetPausing(true);
        mMultiPlayerData.reset(new MultiPlayerData);
        mMultiPlayerData->mState = 0;
        mMultiPlayerData->mDisconMsgDisable.reset(new IDisconMsg::CDisable(IGameSession::GetInstance()->GetDisconMsg()));
        mMultiPlayerData->mMsgWin = IDlgMsgWin::Create(false, false);
        for (int player = 0; player < 4; player++) {
            CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
            IController* pController = pPlayer->GetController();
            if (pController != nullptr) {
                mMultiPlayerData->mEntrancePPD[pController->GetType()][pController->GetChannel()] =
                    pPlayer->GetPersonalData();
            }
            CPlayerPii* pPii = pPlayer->GetPii();
            if (pPii != nullptr) {
                GetCurrentGameMap()->GetWorld()->RemoveObject(pPii->GetSharedPtr());
            }
            pPlayer->Leave();
            pPlayer->SetController(nullptr);
        }
        mMultiPlayerData->mPlayerNum =
            std::min(static_cast<int>(IGameSession::GetInstance()->GetTeamPiiBox()->GetPersonalDataList().size()), sMaxPlayerNum);
        std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
        mMultiPlayerData->mLayout.Load(gSharedLayoutArchives, "dlgpiisel_confirm", fonts);
        CColorBtnTagProc::Attach(mMultiPlayerData->mLayout.SetText(
            "TEXT", GetMessageString(0x6008B, L"+かSTARTでけってい!")));
        for (int player = 0; player < mMultiPlayerData->mPlayerNum; player++) {
            mDataPerPlayer[player].reset(new CDataPerPlayer(this, 2, player));
            mDataPerPlayer[player]->Setup();
        }
        AssignControllers();
    }

    virtual void SetEntrancePPD(IController* pController, const std::tr1::shared_ptr<CPiiPersonalData>& rPPD) {
        if (pController != nullptr && mMultiPlayerData) {
            mMultiPlayerData->mEntrancePPD[pController->GetType()][pController->GetChannel()] = rPPD;
        }
    }

    virtual std::tr1::shared_ptr<CPiiPersonalData> GetEntrancePPD(IController* pController) {
        return pController != nullptr && mMultiPlayerData
                   ? mMultiPlayerData->mEntrancePPD[pController->GetType()][pController->GetChannel()]
                   : std::tr1::shared_ptr<CPiiPersonalData>();
    }

    inline virtual void UpdateDlgOpenState();

    void Init(bool flag) {}

    void CalcEntrance() {
        switch (mMultiPlayerData->mState) {
        case 0:
            AssignControllers();
            break;
        case 1:
            AssignControllers();
            break;
        }
    }

    void UpdateEntrance();
    bool IsAnyShown();

    void AssignControllers() {
        CControllerList controllers;
        controllers.Collect();
        for (int player = 0; player < 4; player++) {
            CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
            IController* pController = nullptr;
            if (player < mMultiPlayerData->mPlayerNum) {
                pController = controllers.Get(player);
            }
            pPlayer->SetController(pController);
        }
    }

private:
    std::tr1::shared_ptr<CDataPerPlayer> mDataPerPlayer[4];
    std::tr1::shared_ptr<MultiPlayerData> mMultiPlayerData;
    bool mIsPausing;
    bool mInputLocked;
    int mErrorState;
    int mError;
};

}

std::tr1::shared_ptr<IDlgPiiSelector> IDlgPiiSelector::Create(bool flag) {
    for (int player = 0; player < 4; player++) {
        IGameSession::GetInstance()->GetRemoteMemory(player)->WaitUntilIdle(flag);
    }
    std::tr1::shared_ptr<CDlgPiiSelector> dlg(new CDlgPiiSelector);
    dlg->Init(flag);
    return dlg;
}
