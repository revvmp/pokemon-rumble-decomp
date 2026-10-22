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
#include <game/MapIndex.h>
#include <game/CPiiBox.h>
#include <game/CPiiCollectionBox.h>
#include <list>
#include <game/MathUtil.h>
#include <game/Pad.h>
#include <game/QuarterTurn.h>
#include <game/SaveData.h>
#include <game/CItem.h>
#include <game/CPii.h>
#include <game/PiiSpeciesData.h>
#include <game/Message.h>
#include <game/Sound.h>
#include <game/CStatistic.h>
#include <game/StringFormat.h>
#include <memory>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <nw4r/math/math_types.h>
#include <vector>

u32 GetPokemonVoiceId(u16 species, u16 form);

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
bool LoadEntranceItems(PiiItemVector& rItems, int player);
bool IsCurrentMapStage7Even();
int CheckPadErrors(int mask);
int CheckRemoteMemoryErrors();

struct SwapRequest {
    std::tr1::shared_ptr<CPiiPersonalData> mPii;
    nw4r::math::VEC3 mPosition;
    f32 mRotation;
    nw4r::math::VEC3 mModelScale;
    int mOutlineType;
};

template <typename T> T Lerp(const T& rFrom, const T& rTo, f32 t) {
    return rFrom + (rTo - rFrom) * t;
}

const char* const kMotionPaneNames[] = {"PIISPEC_ANM", "LIST_ANM", "PLAYERID_ANM", "HELP_ANM"};

class CDataPerPlayer {
public:
    enum Part {
        PART_SPEC,
        PART_LIST,
        PART_PLAYER_ID,
        PART_HELP,
        PART_COUNT
    };

    CDataPerPlayer(IDlgPiiSelectorInternal* pDlg, int state, int player)
        : mpDlg(pDlg), mState(state), mPlayer(player), mMode(0), mFavoriteMode(false), mOutsidePosition(0.0f, 0.0f),
          mInsidePosition(0.0f, 0.0f), mOpen(false), mSilent(false), mInputDisabled(false),
          mEnterFrames(0), mSpinFrames(0), mExitFrames(0), mEnterMotion(""), mSpinMotion(""), mExitMotion(""),
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
    bool IsFinished() { return mFiber.IsFinished(); }
    bool IsShown() { return mOpen; }
    bool IsEntranceReady() { return mEntranceResult == 1; }
    void ConfirmEntrance() { mEntranceResult = 2; }
    void SetInputDisabled(bool disabled) { mInputDisabled = disabled; }
    void OnSelectionChanged(BUI* pUI);
    void OnAccept(BUI* pUI) { mMode = 3; }
    void OnSortChanged(const CPiiListFilterSort* pFilterSort) { UpdateList(true); }
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
    bool ExecuteSwap(const SwapRequest& rRequest);
    void SpawnPreview();
    void CalcPreviewPositions(nw4r::math::VEC3* pInside, nw4r::math::VEC3* pOutside);
    void CreateBall();
    nw4r::math::VEC3 GetOverheadPosition();
    nw4r::math::VEC3 GetInsidePosition();
    std::tr1::shared_ptr<CPii> MovePreviewPii(const nw4r::math::VEC3& rPosition);
    void MoveChangeCircle(const nw4r::math::VEC3& rPosition);
    void MoveBall(const nw4r::math::VEC3& rPosition);
    void RemovePreviewPii();
    void RemoveBall();
    void RemoveChangeCircle();
    bool IsSkipPressed();
    void GetPlayerPiiTransform(nw4r::math::VEC3* pPosition, f32* pRotation);

private:
    IDlgPiiSelectorInternal* mpDlg;
    int mState;
    int mPlayer;
    int mMode;
    bool mFavoriteMode;
    PiiItemVector mItems;
    CPiiListFilterSort mFilterSort;
    nw4r::math::VEC2 mOutsidePosition;
    nw4r::math::VEC2 mInsidePosition;
    bool mOpen;
    bool mSilent;
    bool mInputDisabled;
    std::tr1::weak_ptr<CPii> mPreviewPii;
    std::tr1::weak_ptr<CItem> mChangeCircle;
    std::tr1::weak_ptr<CItem> mBall;
    int mEnterFrames;
    int mSpinFrames;
    int mExitFrames;
    const char* mEnterMotion;
    const char* mSpinMotion;
    const char* mExitMotion;
    std::tr1::shared_ptr<CUIManager> mUIManager;
    std::tr1::shared_ptr<BUILayout> mRootLayout;
    std::tr1::shared_ptr<BUILayout> mListLayout;
    std::tr1::shared_ptr<CUIPiiSpec> mPiiSpec;
    std::tr1::shared_ptr<CUIPiiList> mPiiList;
    std::tr1::shared_ptr<CUIPlayerID> mPlayerID;
    std::tr1::shared_ptr<BUILayout> mHelpLayout;
    bool mShown[PART_COUNT];
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
    mShown[PART_SPEC] = false;
    mShown[PART_LIST] = false;
    mShown[PART_PLAYER_ID] = false;
    mShown[PART_HELP] = false;
    mReservedFlagC0 = false;
    CLayout* pLayout = mListLayout->GetLayout();
    pLayout->CalculateMtx(true);
    nw4r::math::VEC3 position;
    position = Transform(pLayout->FindPane("INSIDE")->GetGlobalMtx(), nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
    nw4r::math::_VEC2 normalized = pLayout->NormalizeViewPosition(nw4r::math::VEC2(position.x, position.y));
    mInsidePosition = nw4r::math::VEC2(normalized.x, normalized.y);
    position = Transform(pLayout->FindPane("OUTSIDE")->GetGlobalMtx(), nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
    normalized = pLayout->NormalizeViewPosition(nw4r::math::VEC2(position.x, position.y));
    mOutsidePosition = nw4r::math::VEC2(normalized.x, normalized.y);
    mFiber.Start(std::tr1::bind(&CDataPerPlayer::Run, this), 0x8000);
}

class IDlgPiiSelectorInternal : public IDlgPiiSelector {
public:
    virtual ~IDlgPiiSelectorInternal() {}
    virtual std::tr1::shared_ptr<CPiiPersonalData> GetEntrancePPD(IController* pController) = 0;
    virtual void UpdateDlgOpenState() = 0;
};

void CDataPerPlayer::OnSelectionChanged(BUI* pUI) {
    if (!mSilent) {
        PlaySound(0x54);
    }
    std::tr1::shared_ptr<IUIListItem> selected = mPiiList->GetSelectedItem();
    if (selected) {
        std::tr1::shared_ptr<CUIPiiListItem> item = std::tr1::static_pointer_cast<CUIPiiListItem>(selected);
        const CPiiPersonalData* pPPD = item->GetPPD().operator->();
        if (pPPD->IsSpecHidden()) {
            item->GetPPD()->SetNewlyDisclosed(true);
            item->GetPPD()->SetSpecHidden(false);
            mPiiList->UpdateSelection();
        }
        IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
        if (pController != nullptr && (pController->GetHold() & 0x800) && !(pController->GetTrig() & 0x800)) {
            if (item->GetPPD()->IsFavorite()) {
                if (!mFavoriteMode) {
                    item->GetPPD()->SetFavorite(false);
                    PlaySound(90);
                }
            } else if (mFavoriteMode) {
                item->GetPPD()->SetFavorite(true);
                PlaySound(9);
            }
        }
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
        mpDlg->SetEntrancePPD(pPlayer->GetController(), std::tr1::shared_ptr<CPiiPersonalData>(item->GetPPD()));
    }
}

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
    IController* pController;
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
    PlayMotion(mPlayerID, true);
    {
        std::tr1::shared_ptr<CLayout::CMotionProgram> motion = PlayMotion(mPlayerID, true);
        if (motion) {
            motion->Wait();
        }
    }
    FlushRemoteMemory();
    if (!IsDisconnected()) {
        LoadEntranceItems(mItems, mPlayer);
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

std::tr1::shared_ptr<CLayout::CMotionProgram> CDataPerPlayer::PlayMotion(std::tr1::shared_ptr<BUILayout> pLayout, bool shown) {
    if (!pLayout) {
        return std::tr1::shared_ptr<CLayout::CMotionProgram>();
    }
    int part;
    if (pLayout == mPiiSpec) {
        part = PART_SPEC;
    } else if (pLayout == mPiiList) {
        part = PART_LIST;
    } else if (pLayout == mPlayerID) {
        part = PART_PLAYER_ID;
    } else if (pLayout == mHelpLayout) {
        part = PART_HELP;
    } else {
        return std::tr1::shared_ptr<CLayout::CMotionProgram>();
    }
    if (shown == mShown[part]) {
        return std::tr1::shared_ptr<CLayout::CMotionProgram>();
    }
    mShown[part] = shown;
    pLayout->SetVisible(true);
    return mListLayout->GetLayout()->CreateMotion(shown ? "begin" : "end", kMotionPaneNames[part], 0.0f, 1.0f);
}

int CDataPerPlayer::ValidateAccept() {
    std::tr1::shared_ptr<IUIListItem> selected = mPiiList->GetSelectedItem();
    if (!selected) {
        return 1;
    }
    std::tr1::shared_ptr<CUIPiiListItem> item = std::tr1::static_pointer_cast<CUIPiiListItem>(selected);
    std::tr1::shared_ptr<CPiiPersonalData> ppd = item->GetPPD();
    if (!IGameSession::GetInstance()->CanAddPPDToPlayerPiiBox(mPlayer, std::tr1::shared_ptr<CPiiPersonalData>(ppd))) {
        if (!ppd->IsFainted()) {
            CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
            if (pPlayer->IsJoined() && pPlayer->GetPersonalData() == ppd) {
                return 2;
            }
        }
        return 1;
    }
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
    pPlayer->SetPendingPersonalData(std::tr1::shared_ptr<CPiiPersonalData>(ppd));
    PlaySound(9);
    mPiiList->GetSelectedLayout()->CreateMotion("selected")->Wait();
    return 0;
}

void CDataPerPlayer::ApplyAccept() {
    SpawnPreview();
    {
        std::tr1::shared_ptr<CPii> pii = mPreviewPii.lock();
        if (pii) {
            pii->PlayMotion(0x6E, 0.0f, 1.0f);
        }
    }
    if (mState == 2) {
        BWorld* pWorld = GetCurrentGameMap()->GetWorld();
        pWorld->RemoveObject(mBall.lock());
    } else {
        if (std::tr1::shared_ptr<CItem> ball = mBall.lock()) {
            mBall.lock()->Play(mEnterMotion, 0.0f, 1.0f);
        }
    }
    PlayMotion(mHelpLayout, false);
    std::tr1::shared_ptr<CLayout::CMotionProgram> motion = PlayMotion(mPiiList, false);
    int frames = mEnterFrames;
    for (int i = 0; i < frames; i++) {
        f32 t = Min(static_cast<f32>(i) / static_cast<f32>(frames), 1.0f);
        nw4r::math::VEC3 inside;
        nw4r::math::VEC3 outside;
        CalcPreviewPositions(&inside, &outside);
        nw4r::math::VEC3 position = Lerp(outside, inside, SinQuarterTurn(t));
        MovePreviewPii(position);
        MoveChangeCircle(position);
        MoveBall(GetOverheadPosition());
        YieldFiber();
    }
    if (motion) {
        motion->Wait();
    }
}

void CDataPerPlayer::SpawnPreview() {
    BWorld* pWorld = GetCurrentGameMap()->GetWorld();
    nw4r::math::VEC3 inside;
    nw4r::math::VEC3 outside;
    CalcPreviewPositions(&inside, &outside);
    std::tr1::shared_ptr<CPii> pii(
        new CPii(IGameSession::GetInstance()->GetPlayer(mPlayer)->GetPendingPersonalData(), 0x40));
    pii->Initialize();
    pii->SetVisible(true);
    pii->SetPosition(outside);
    pii->SetRotation(180.0f);
    pWorld->Add(pii);
    mPreviewPii = pii;
    std::tr1::shared_ptr<CItem> circle(new CItem(0x800));
    circle->Initialize();
    circle->LoadModel("change_circle.brres");
    circle->GetModel()->GetScnObj()->SetPriorityDrawXlu(200);
    circle->SetVisible(true);
    circle->SetPosition(pii->GetPositionRef());
    pWorld->Add(circle);
    mChangeCircle = circle;
    CreateBall();
}

void CDataPerPlayer::CalcPreviewPositions(nw4r::math::VEC3* pInside, nw4r::math::VEC3* pOutside) {
    nw4r::g3d::Camera camera = GetCurrentGameMap()->GetWorld()->GetG3dCamera();
    const nw4r::g3d::CameraData& rData = camera.ref();
    nw4r::math::MTX34 cameraMtx;
    nw4r::math::MTX34 inverseMtx;
    camera.GetCameraMtx(&cameraMtx);
    Inverse(&inverseMtx, &cameraMtx);
    f32 scale = 5.0f * nw4r::math::TanDeg(0.5f * rData.projFovy);
    nw4r::math::VEC3 position;
    position.x = rData.projAspect * (mInsidePosition.x * scale);
    position.y = -mInsidePosition.y * scale;
    position.z = -5.0f;
    position = Transform(inverseMtx, position);
    if (pInside != nullptr) {
        *pInside = position;
    }
    if (pOutside != nullptr) {
        nw4r::math::VEC3 outside;
        outside.z = -5.0f;
        outside.x = rData.projAspect * (mOutsidePosition.x * scale);
        outside.y = -mOutsidePosition.y * scale;
        outside = Transform(inverseMtx, outside);
        outside.x = position.x;
        *pOutside = outside;
    }
}

void CDataPerPlayer::CreateBall() {
    BWorld* pWorld = GetCurrentGameMap()->GetWorld();
    int rank = Clamp<int>(GetSaveData(-1)->mData.mFlags[0], 1, 4);
    mEnterMotion = "neji_EX__in";
    mSpinMotion = "neji_EX__screw";
    mExitMotion = "neji_EX__out";
    const char* pModelName = "neji_EX.brres";
    switch (GetSaveData(-1)->mData.mFlags[1]) {
    case 0:
        switch (rank) {
        case 1:
            mEnterMotion = "neji_C__in";
            mSpinMotion = "neji_C__screw";
            mExitMotion = "neji_C__out";
            pModelName = "neji_C.brres";
            break;
        case 2:
            mEnterMotion = "neji_B__in";
            mSpinMotion = "neji_B__screw";
            mExitMotion = "neji_B__out";
            pModelName = "neji_B.brres";
            break;
        case 3:
            mEnterMotion = "neji_A__in";
            mSpinMotion = "neji_A__screw";
            mExitMotion = "neji_A__out";
            pModelName = "neji_A.brres";
            break;
        case 4:
            mEnterMotion = "neji_S__in";
            mSpinMotion = "neji_S__screw";
            mExitMotion = "neji_S__out";
            pModelName = "neji_S.brres";
            break;
        }
        break;
    case 1:
        switch (rank) {
        case 1:
            mEnterMotion = "neji_another_C__in";
            mSpinMotion = "neji_another_C__screw";
            mExitMotion = "neji_another_C__out";
            pModelName = "neji_another_C.brres";
            break;
        case 2:
            mEnterMotion = "neji_another_B__in";
            mSpinMotion = "neji_another_B__screw";
            mExitMotion = "neji_another_B__out";
            pModelName = "neji_another_B.brres";
            break;
        case 3:
            mEnterMotion = "neji_another_A__in";
            mSpinMotion = "neji_another_A__screw";
            mExitMotion = "neji_another_A__out";
            pModelName = "neji_another_A.brres";
            break;
        case 4:
            mEnterMotion = "neji_another_S__in";
            mSpinMotion = "neji_another_S__screw";
            mExitMotion = "neji_another_S__out";
            pModelName = "neji_another_S.brres";
            break;
        }
        break;
    }
    std::tr1::shared_ptr<CItem> ball(new CItem(0x800));
    ball->Initialize();
    ball->LoadModel(pModelName);
    ball->GetModel()->GetScnObj()->SetPriorityDrawXlu(200);
    ball->SetVisible(true);
    ball->SetPosition(GetOverheadPosition());
    mEnterFrames = ball->GetModel()->GetMotionLength(mEnterMotion);
    mSpinFrames = ball->GetModel()->GetMotionLength(mSpinMotion);
    mExitFrames = ball->GetModel()->GetMotionLength(mExitMotion);
    mBall = ball;
    pWorld->Add(ball);
}

nw4r::math::VEC3 CDataPerPlayer::GetOverheadPosition() {
    nw4r::math::VEC3 position = GetInsidePosition();
    std::tr1::shared_ptr<CPii> pii = mPreviewPii.lock();
    if (pii) {
        position += nw4r::math::VEC3(0.0f, pii->GetModel()->GetBounds().max.y, 0.0f);
    }
    return position;
}

nw4r::math::VEC3 CDataPerPlayer::GetInsidePosition() {
    nw4r::math::VEC3 position;
    CalcPreviewPositions(&position, nullptr);
    return position;
}

std::tr1::shared_ptr<CPii> CDataPerPlayer::MovePreviewPii(const nw4r::math::VEC3& rPosition) {
    std::tr1::shared_ptr<CPii> pii = mPreviewPii.lock();
    if (pii) {
        pii->SetPosition(rPosition);
    }
    return pii;
}

void CDataPerPlayer::MoveChangeCircle(const nw4r::math::VEC3& rPosition) {
    std::tr1::shared_ptr<CItem> circle = mChangeCircle.lock();
    if (circle) {
        circle->SetPosition(rPosition);
    }
}

void CDataPerPlayer::MoveBall(const nw4r::math::VEC3& rPosition) {
    std::tr1::shared_ptr<CItem> ball = mBall.lock();
    if (ball) {
        ball->SetPosition(rPosition);
    }
}

void CDataPerPlayer::ProcessEntranceAccept() {
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
    mEntranceResult = 1;
    for (;;) {
        IController* pController = pPlayer->GetController();
        if (mEntranceResult == 2) {
            CreateBall();
            if (std::tr1::shared_ptr<CItem> ball = mBall.lock()) {
                std::tr1::shared_ptr<CModel::CMotionProgram> motion = mBall.lock()->Play(mEnterMotion, 0.0f, 1.0f);
                if (motion) {
                    motion->Wait();
                }
            }
            return;
        }
        if (IsDisconnected()) {
            mMode = 4;
        } else if (pController != nullptr && pController->IsConnected()) {
            if (!mInputDisabled && (pController->GetTrig() & 0x90)) {
                mMode = 4;
                PlaySound(90);
            }
        } else {
            mMode = 4;
        }
        if (static_cast<u32>(TakeMode() - 4) <= 1) {
            mEntranceResult = 0;
            pPlayer->ResetPendingPersonalData();
            return;
        }
        YieldFiber();
    }
}

void CDataPerPlayer::FinishAccept() {
    int frames = mEnterFrames;
    for (int i = 0; i < frames; i++) {
        f32 t = static_cast<f32>(i) / static_cast<f32>(frames);
        nw4r::math::VEC3 inside;
        nw4r::math::VEC3 outside;
        CalcPreviewPositions(&inside, &outside);
        nw4r::math::VEC3 position = Lerp(inside, outside, OneMinusCosQuarterTurn(t));
        MovePreviewPii(position);
        MoveChangeCircle(position);
        YieldFiber();
    }
    RemovePreviewPii();
    RemoveBall();
    RemoveChangeCircle();
}

void CDataPerPlayer::RemovePreviewPii() {
    std::tr1::shared_ptr<CPii> pii = mPreviewPii.lock();
    if (pii) {
        BWorld* pWorld = pii->GetWorld();
        if (pWorld != nullptr) {
            pWorld->RemoveObject(pii);
        }
        mPreviewPii.reset();
    }
}

void CDataPerPlayer::RemoveBall() {
    std::tr1::shared_ptr<CItem> ball = mBall.lock();
    if (ball) {
        BWorld* pWorld = ball->GetWorld();
        if (pWorld != nullptr) {
            pWorld->RemoveObject(ball);
        }
        mBall.reset();
    }
}

void CDataPerPlayer::RemoveChangeCircle() {
    std::tr1::shared_ptr<CItem> circle = mChangeCircle.lock();
    if (circle) {
        BWorld* pWorld = circle->GetWorld();
        if (pWorld != nullptr) {
            pWorld->RemoveObject(circle);
        }
        mChangeCircle.reset();
    }
}

CDataPerPlayer::~CDataPerPlayer() {
    RemovePreviewPii();
    RemoveBall();
    RemoveChangeCircle();
}

bool CDataPerPlayer::IsSkipPressed() {
    IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
    if (pController != nullptr && pController->IsConnected()) {
        return (pController->GetTrig() & 0x3F) != 0;
    }
    return false;
}

bool CDataPerPlayer::CanSwap() {
    {
        std::tr1::shared_ptr<CPii> pii = mPreviewPii.lock();
        if (pii) {
            pii->PlayMotion(0x6F, 0.0f, 1.0f);
        }
    }
    if (std::tr1::shared_ptr<CItem> ball = mBall.lock()) {
        mBall.lock()->Play(mSpinMotion, 0.0f, 1.0f);
    }
    {
        std::tr1::shared_ptr<CPii> pii = mPreviewPii.lock();
        if (pii) {
            pii->StartSE(0x39);
        }
    }
    f32 length = static_cast<f32>(mSpinFrames);
    int hold = 0;
    f32 rate = 1.0f;
    for (f32 frame = 0.0f; frame < length; frame += rate) {
        if (IsSkipPressed()) {
            rate = 2.0f;
            if (hold == 0) {
                {
                    std::tr1::shared_ptr<CPii> pii = mPreviewPii.lock();
                    if (pii) {
                        pii->PlayMotion(0x6F, frame, rate);
                        pii->SetPitch(2.0f);
                    }
                }
                if (std::tr1::shared_ptr<CItem> ball = mBall.lock()) {
                    mBall.lock()->Play(mSpinMotion, frame, rate);
                }
            }
            hold = 10;
        } else if (hold > 0 && --hold == 0) {
            rate = 1.0f;
            {
                std::tr1::shared_ptr<CPii> pii = mPreviewPii.lock();
                if (pii) {
                    pii->PlayMotion(0x6F, frame, rate);
                    pii->SetPitch(1.0f);
                }
            }
            if (std::tr1::shared_ptr<CItem> ball = mBall.lock()) {
                mBall.lock()->Play(mSpinMotion, frame, rate);
            }
        }
        nw4r::math::VEC3 inside = GetInsidePosition();
        MovePreviewPii(inside);
        MoveChangeCircle(inside);
        MoveBall(GetOverheadPosition());
        if (mState == 0 && !IsCurrentMapStage7Even()) {
            switch (TakeMode()) {
            case 5:
                IGameSession::GetInstance()->GetStatistic()->Add(-99, 1);
            case 4:
                if (std::tr1::shared_ptr<CItem> ball = mBall.lock()) {
                    mBall.lock()->Play(mExitMotion, 0.0f, 1.0f);
                    IGameSession::GetInstance()->GetPlayer(mPlayer)->ResetPendingPersonalData();
                }
                return false;
            }
        }
        YieldFiber();
    }
    return true;
}

void CDataPerPlayer::PrepareSwap(SwapRequest& rRequest) {
    {
        std::tr1::shared_ptr<CPii> pii = mPreviewPii.lock();
        if (pii) {
            pii->PlayMotion(1, 0.0f, 1.0f);
        }
    }
    if (std::tr1::shared_ptr<CItem> ball = mBall.lock()) {
        mBall.lock()->Play(mExitMotion, 0.0f, 1.0f);
    }
    PlaySound(7);
    {
        std::tr1::shared_ptr<CPii> pii = mPreviewPii.lock();
        if (pii) {
            const PiiSpeciesData* pSpeciesData = pii->GetPersonalData()->GetSpeciesData();
            PlaySound(GetPokemonVoiceId(pSpeciesData->GetSpecies(), static_cast<u8>(pSpeciesData->GetFormNumber())));
        }
    }
    int frames = mExitFrames;
    rRequest.mPosition = nw4r::math::VEC3(0.0f, 0.0f, 0.0f);
    rRequest.mRotation = 180.0f;
    if (mState == 2) {
        int joined = 0;
        int index = 0;
        for (int player = 0; player < 4; player++) {
            if (IGameSession::GetInstance()->GetPlayer(player)->IsJoined()) {
                if (player == mPlayer) {
                    index = joined;
                }
                joined++;
            }
        }
        rRequest.mPosition.x += 0.7f * (static_cast<f32>(-(joined - 1)) / 2.0f + static_cast<f32>(index));
    }
    for (int i = 0; i < frames; i++) {
        GetPlayerPiiTransform(&rRequest.mPosition, &rRequest.mRotation);
        f32 t = static_cast<f32>(i) / static_cast<f32>(frames);
        nw4r::math::VEC3 inside = GetInsidePosition();
        nw4r::math::VEC3 from(rRequest.mPosition);
        nw4r::math::VEC3 position;
        position.x = Lerp(inside.x, from.x, t);
        position.y = Lerp(inside.y, from.y, OneMinusCosQuarterTurn(t));
        position.z = Lerp(inside.z, from.z, t);
        MoveChangeCircle(inside);
        MoveBall(GetOverheadPosition());
        {
            std::tr1::shared_ptr<CPii> pii = MovePreviewPii(position);
            if (pii) {
                f32 startRotation = 180.0f;
                f32 endRotation = rRequest.mRotation;
                endRotation = startRotation + NormalizeSignedAngle(endRotation - startRotation);
                pii->SetRotation(Lerp(startRotation, endRotation, t));
            }
        }
        YieldFiber();
    }
    RemoveBall();
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
    std::tr1::shared_ptr<CPiiPersonalData> personalData;
    CPlayerPii* pPii = pPlayer->GetPii();
    if (pPii != nullptr) {
        rRequest.mPii = pPii->GetPersonalData();
        rRequest.mModelScale = pPii->GetModelScale();
        rRequest.mOutlineType = pPii->GetOutlineType();
    }
    pPlayer->RequestPiiChange();
    IGameSession::GetInstance()->GetStatistic()->Add(-100, 1);
    for (int i = 0; i < 60 && pPlayer->IsPiiChangeRequested(); i++) {
        GetPlayerPiiTransform(&rRequest.mPosition, &rRequest.mRotation);
        MoveChangeCircle(GetInsidePosition());
        {
            std::tr1::shared_ptr<CPii> pii = MovePreviewPii(rRequest.mPosition);
            if (pii) {
                pii->SetRotation(rRequest.mRotation);
            }
        }
        YieldFiber();
    }
    RemovePreviewPii();
}

void CDataPerPlayer::GetPlayerPiiTransform(nw4r::math::VEC3* pPosition, f32* pRotation) {
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
    if (pPlayer->GetPii()) {
        *pPosition = pPlayer->GetPii()->GetPositionRef();
        *pRotation = pPlayer->GetPii()->GetRotation();
    } else {
        for (int player = 0; player < 4; player++) {
            pPlayer = IGameSession::GetInstance()->GetPlayer(player);
            if (pPlayer->GetPii()) {
                *pPosition = pPlayer->GetPii()->GetPositionRef();
                *pRotation = pPlayer->GetPii()->GetRotation();
                break;
            }
        }
    }
}

bool CDataPerPlayer::ExecuteSwap(const SwapRequest& rRequest) {
    if (!rRequest.mPii) {
        return false;
    }
    std::tr1::shared_ptr<CPii> pii(new CPii(std::tr1::shared_ptr<CPiiPersonalData>(rRequest.mPii), 0x40));
    pii->Initialize();
    pii->SetVisible(true);
    pii->SetPosition(rRequest.mPosition);
    pii->SetRotation(rRequest.mRotation);
    pii->SetOutlineType(rRequest.mOutlineType);
    pii->SetModelScale(rRequest.mModelScale);
    if (rRequest.mPii->IsFainted()) {
        pii->PlayMotion(0x68, 0.0f, 1.0f);
    }
    BWorld* pWorld = GetCurrentGameMap()->GetWorld();
    pWorld->Add(pii);
    PlaySound(0x3A);
    mPreviewPii = pii;
    int frames = mExitFrames;
    for (int i = 0; i < frames; i++) {
        f32 t = static_cast<f32>(i) / static_cast<f32>(frames);
        nw4r::math::VEC3 from(rRequest.mPosition);
        nw4r::math::VEC3 inside = GetInsidePosition();
        nw4r::math::VEC3 position;
        position.x = Lerp(from.x, inside.x, t);
        position.y = Lerp(from.y, inside.y, SinQuarterTurn(t));
        position.z = Lerp(from.z, inside.z, t);
        pii = MovePreviewPii(position);
        MoveChangeCircle(inside);
        if (pii) {
            f32 startRotation = rRequest.mRotation;
            f32 endRotation = 0.0f;
            endRotation = startRotation + NormalizeSignedAngle(endRotation - startRotation);
            pii->SetRotation(Lerp(startRotation, endRotation, t));
        }
        YieldFiber();
    }
    return true;
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
    void UpdateErrors(int connected);
    void SetInputLocked(bool locked);
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

void CDlgPiiSelector::UpdateDlgOpenState() {
    if (IsAnyShown()) {
        GetCurrentGameMap()->RegisterDialog(shared_from_this());
    } else {
        GetCurrentGameMap()->UnregisterDialog(shared_from_this());
    }
}

bool CDlgPiiSelector::IsAnyShown() {
    if (IsMultiPlayerEntranceShown()) {
        return true;
    }
    for (int player = 0; player < 4; player++) {
        if (IsShown(player)) {
            return true;
        }
    }
    return false;
}

void CDlgPiiSelector::UpdateEntrance() {
    int joined = 0;
    int accepted = 0;
    int nunchaku = 0;
    int connected = 0;
    for (int player = 0; player < mMultiPlayerData->mPlayerNum; player++) {
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
        IController* pController = pPlayer->GetController();
        if (pController != nullptr && pController->IsConnected()) {
            connected++;
        }
        if (pPlayer->IsJoined()) {
            joined++;
            if (pController != nullptr && pController->GetType() == IController::TYPE_NUNCHAKU) {
                nunchaku++;
            }
            if (mDataPerPlayer[player] && mDataPerPlayer[player]->IsEntranceReady()) {
                accepted++;
            }
        }
    }
    if (mErrorState != 0) {
        UpdateErrors(connected);
        return;
    }
    switch (mMultiPlayerData->mState) {
    case 0:
        if (joined > 0 && joined == accepted && nunchaku < accepted) {
            mMultiPlayerData->mLayout.CreateMotion("begin");
            mMultiPlayerData->mState = 1;
        }
        break;
    case 1:
        if (joined > 0 && joined == accepted) {
            bool confirmed = false;
            for (int player = 0; player < mMultiPlayerData->mPlayerNum; player++) {
                CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
                if (pPlayer->IsJoined()) {
                    IController* pController = pPlayer->GetController();
                    if (pController != nullptr && pController->IsConnected() && !mInputLocked &&
                        (pController->GetTrig() & 0x100)) {
                        confirmed = true;
                        PlaySound(9);
                        break;
                    }
                }
            }
            if (confirmed) {
                mMultiPlayerData->mLayout.CreateMotion("end");
                mMultiPlayerData->mState = 2;
                for (int player = 0; player < 4; player++) {
                    if (mDataPerPlayer[player]) {
                        mDataPerPlayer[player]->ConfirmEntrance();
                    }
                }
                mMultiPlayerData->mDisconMsgDisable.reset();
            }
        } else {
            mMultiPlayerData->mLayout.CreateMotion("end");
            mMultiPlayerData->mState = 0;
        }
        break;
    case 2:
        for (int player = 0; player < 4; player++) {
            CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
            if (!pPlayer->IsJoined()) {
                pPlayer->SetController(nullptr);
            }
            if (mDataPerPlayer[player] && mDataPerPlayer[player]->IsShown()) {
                goto update;
            }
        }
        mMultiPlayerData.reset();
        GetCurrentGameMap()->SetPausing(false);
        return;
    }
update:
    UpdateErrors(connected);
}

void CDlgPiiSelector::UpdateErrors(int connected) {
    int previous;
    for (;;) {
        previous = mErrorState;
        switch (mErrorState) {
        case 0: {
            int mask = CheckRemoteMemoryErrors();
            if (mask != 0) {
                mError = mask;
                SetInputLocked(true);
                mMultiPlayerData->mMsgWin->OpenNotice(GetMessageString(0x1000F, nullptr));
                mErrorState = 1;
            } else {
                mask = CheckPadErrors(0);
                if (!IsExtensionControllerEnabled() && mask != 0) {
                    mError = mask;
                    SetInputLocked(true);
                    mMultiPlayerData->mMsgWin->OpenFootnote(GetMessageString(0x600E8, nullptr));
                    mErrorState = 2;
                } else if (mMultiPlayerData->mState == 0 && connected == 0) {
                    mMultiPlayerData->mMsgWin->OpenFootnote(GetMessageString(0x6008C, nullptr));
                    mErrorState = 3;
                }
            }
            break;
        }
        case 3:
            if (connected > 0) {
                mMultiPlayerData->mMsgWin->Close();
                mErrorState = 4;
            }
            break;
        case 2:
            mError = CheckPadErrors(mError);
            if (mError == 0) {
                mMultiPlayerData->mMsgWin->Close();
                mErrorState = 4;
                continue;
            }
            break;
        case 1: {
            bool finished = connected == 0;
            for (int channel = 0; channel < 4; channel++) {
                IController* pRemote = GetController(IController::TYPE_WII_REMOTE, channel);
                if (mError & (1 << channel)) {
                    bool extensionPressed = false;
                    if (!IGameSession::GetInstance()->IsControllerAssigned(pRemote) && IsExtensionControllerEnabled()) {
                        IController* pExtension = GetController(IController::TYPE_NUNCHAKU, channel);
                        if (!pExtension->IsConnected()) {
                            pExtension = GetController(IController::TYPE_CLASSIC, channel);
                        }
                        if (pExtension->IsConnected() && (pExtension->GetTrig() & 0x20)) {
                            extensionPressed = true;
                        }
                    }
                    if ((pRemote->GetTrig() & 0x20) || !pRemote->IsConnected() || extensionPressed) {
                        mError &= ~(1 << channel);
                        finished = true;
                    }
                    break;
                }
            }
            if (finished) {
                mMultiPlayerData->mMsgWin->Close();
                mErrorState = 4;
            }
            break;
        }
        case 4:
            if (mMultiPlayerData->mMsgWin->IsAnimationDone()) {
                SetInputLocked(false);
                mErrorState = 0;
            }
            break;
        }
        if (mErrorState == previous) {
            break;
        }
    }
}

int CheckRemoteMemoryErrors() {
    for (int player = 0; player < 4; player++) {
        CRemoteMemory* pRemoteMemory = IGameSession::GetInstance()->GetRemoteMemory(player);
        if (pRemoteMemory != nullptr && pRemoteMemory->ConsumeFatalError()) {
            return 1 << player;
        }
    }
    return 0;
}

int CheckPadErrors(int mask) {
    int result = mask;
    for (int channel = 0; channel < 4; channel++) {
        unsigned int device;
        switch (GetWiiPadDevice(channel, &device)) {
        case -1:
            result &= ~(1 << channel);
            break;
        case 0:
            if (device == 0 || device == 2) {
                result &= ~(1 << channel);
            } else {
                result |= 1 << channel;
            }
            break;
        }
    }
    return result;
}

void CDlgPiiSelector::SetInputLocked(bool locked) {
    if (locked != mInputLocked) {
        mInputLocked = locked;
        for (int player = 0; player < mMultiPlayerData->mPlayerNum; player++) {
            if (mDataPerPlayer[player]) {
                mDataPerPlayer[player]->SetInputDisabled(locked);
            }
        }
    }
}


typedef std::vector<std::tr1::shared_ptr<CPiiPersonalData> > PiiPersonalDataVector;

bool BuildItems(PiiItemVector& rItems, int player, PiiPersonalDataVector& rPiis) {
    const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rTeam = IGameSession::GetInstance()->GetTeamPiiBox()->GetPersonalDataList();
    rPiis.insert(rPiis.end(), rTeam.begin(), rTeam.end());
    IController* pController = IGameSession::GetInstance()->GetPlayer(player)->GetController();
    if (pController->GetType() == IController::TYPE_WII_REMOTE || pController->GetType() == IController::TYPE_NUNCHAKU ||
        pController->GetType() == IController::TYPE_CLASSIC) {
        const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rRemote =
            IGameSession::GetInstance()->GetRemoteMemory(pController->GetChannel())->GetPiiBox()->GetPersonalDataList();
        rPiis.insert(rPiis.end(), rRemote.begin(), rRemote.end());
    }
    if (rPiis.size() == 0) {
        return false;
    }
    PiiItemVector items;
    items.reserve(rPiis.size());
    for (PiiPersonalDataVector::iterator it = rPiis.begin(); it != rPiis.end(); ++it) {
        std::tr1::shared_ptr<CUIPiiListItem> item(new CUIPiiListItem(std::tr1::shared_ptr<CPiiPersonalData>(*it), player));
        items.push_back(item);
    }
    rItems.swap(items);
    return true;
}

bool LoadItems(PiiItemVector& rItems, int player) {
    CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(player);
    if (!pPlayer->IsJoined()) {
        return false;
    }
    const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rOwn = pPlayer->GetPiiBox().GetPersonalDataList();
    PiiPersonalDataVector piis;
    piis.assign(rOwn.begin(), rOwn.end());
    return BuildItems(rItems, player, piis);
}

bool LoadEntranceItems(PiiItemVector& rItems, int player) {
    PiiPersonalDataVector piis;
    return BuildItems(rItems, player, piis);
}

bool IsCurrentMapStage7Even() {
    return IsStage7EvenMap(GetCurrentGameMap()->GetMapDesc().mIndex);
}

}

std::tr1::shared_ptr<IDlgPiiSelector> IDlgPiiSelector::Create(bool flag) {
    for (int player = 0; player < 4; player++) {
        IGameSession::GetInstance()->GetRemoteMemory(player)->WaitUntilIdle(flag);
    }
    std::tr1::shared_ptr<CDlgPiiSelector> dlg(new CDlgPiiSelector);
    dlg->Init(flag);
    return dlg;
}
