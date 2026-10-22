#include <game/IDlgPiiList.h>

#include <game/ButtonMessage.h>
#include <game/CColorBtnTagProc.h>
#include <game/CFiber.h>
#include <game/CLayout.h>
#include <game/CPiiListFilterSort.h>
#include <game/COyaInfo.h>
#include <game/CPlayer.h>
#include <game/CPpuEngine.h>
#include <game/CPiiBox.h>
#include <game/CPiiCollectionBox.h>
#include <game/CRemoteMemory.h>
#include <game/CScopedActivity.h>
#include <game/IDisconMsg.h>
#include <game/Random.h>
#include <list>
#include <game/CUIManager.h>
#include <game/CUIPiiList.h>
#include <game/CUIPiiListItem.h>
#include <game/CUIPiiSpec.h>
#include <game/CUIPlayerID.h>
#include <game/IController.h>
#include <game/IDlgMsgWin.h>
#include <game/IGameMap.h>
#include <game/IGameSession.h>
#include <game/LayoutResources.h>
#include <game/IDlgPiiSelector.h>
#include <game/Message.h>
#include <game/MessageFormat.h>
#include <game/Sound.h>
#include <algorithm>
#include <memory>
#include <nw4r/lyt/lyt_pane.h>
#include <nw4r/lyt/lyt_textBox.h>
#include <vector>
#include <game/HomeButton.h>
#include <revolution/os/OSTime.h>

namespace std {

template <class charT, class traits, class Allocator>
basic_string<charT, traits, Allocator> operator+(const basic_string<charT, traits, Allocator>& lhs, const charT* rhs)
{
    basic_string<charT, traits, Allocator> result;
    size_t rhsLength = traits::length(rhs);
    size_t lhsLength;
    const charT* lhsData;
    lhs.get_buffer(lhsData, lhsLength);
    result.reserve(lhsLength + rhsLength);
    result.replace(0, 0, lhsData, lhsData + lhsLength).replace(lhsLength, 0, rhs, rhs + rhsLength);
    return result;
}

}

std::wstring GetButtonMessage(int player, u32 button, int messageId) {
    IController* pController = IGameSession::GetInstance()->GetPlayer(player)->GetController();
    if (pController == nullptr) {
        return L"";
    }
    const wchar_t* pSymbol = pController->GetButtonSymbol(button);
    if (pSymbol == nullptr || *pSymbol == 0) {
        return L"";
    }
    return FormatMessage(GetMessageString(messageId, nullptr), pSymbol) + L"\n";
}


CPlayer* FindPlayerHoldingPii(std::tr1::shared_ptr<CPiiPersonalData> pii, int exceptPlayer) {
    for (int i = 0; i < 4; i++) {
        if (i == exceptPlayer) {
            continue;
        }
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(i);
        if (pPlayer->IsJoined() && (pPlayer->GetPersonalData() == pii || pPlayer->GetPendingPersonalData() == pii)) {
            return pPlayer;
        }
    }
    return nullptr;
}

template std::tr1::shared_ptr<CPiiPersonalData>::shared_ptr(const std::tr1::shared_ptr<CPiiPersonalData>&);

#pragma scheduling off
std::tr1::shared_ptr<CPiiPersonalData> CPlayer::GetPersonalData() const {
    return mState.mPersonalData;
}

std::tr1::shared_ptr<CPiiPersonalData> CPlayer::GetPendingPersonalData() const {
    return mState.mPendingPersonalData;
}
#pragma scheduling reset

namespace {

typedef std::vector<std::tr1::shared_ptr<CUIPiiListItem> > PiiItemVector;

int sMinUnmarkedCount = 1;
int sMaxRemoteMarkedCount = 12;


class CDlgPiiListImpl {
public:
    CDlgPiiListImpl(int player, int type);

    void AppendItems(const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rList);
    void Setup(bool background);
    void OnSelectionChanged(BUI* pUI);
    void OnAccept(BUI* pUI);
    void OnSortChanged(const CPiiListFilterSort* pFilterSort);
    void RunRemoteTransfer();
    void RunSell();
    void Run();
    void RunList();
    void Finish();
    void ShowMessage(const wchar_t* pText);
    bool AskYesNo(const wchar_t* pText);
    int CountRemoteMarked();
    int CountRemoteUnmarked();
    int CountSellUnmarked();
    int GetMinUnmarkedCount();

    int mPlayer;
    int mType;
    int mResult;
    bool mHolding;
    bool mFavoriteMode;
    PiiItemVector mItems;
    CPiiListFilterSort mFilterSort;
    std::tr1::shared_ptr<CUIManager> mUIManager;
    std::tr1::shared_ptr<BUILayout> mRootLayout;
    std::tr1::shared_ptr<CUIPiiSpec> mPiiSpec;
    std::tr1::shared_ptr<CUIPiiList> mPiiList;
    std::tr1::shared_ptr<CUIPlayerID> mPlayerID;
    std::tr1::shared_ptr<BUILayout> mHelpLayout;
    std::tr1::shared_ptr<BUILayout> mTitleLayout;
    std::tr1::shared_ptr<BUILayout> mRemoteMemoryLayout;
    std::tr1::shared_ptr<BUILayout> mSavingLayout;
    std::tr1::shared_ptr<IDlgMsgWin> mMsgWin;
    CFiber mFiber;
    bool mFlag400;
};

#pragma dont_inline on

typedef std::list<std::tr1::shared_ptr<CPiiPersonalData> > PiiList;

inline void CDlgPiiListImpl::RunRemoteTransfer() {
    NResetMgr::CDisable resetDisable;
    CDisableHBM homeButtonDisable;
    bool changed = false;
    for (PiiItemVector::const_iterator it = mItems.begin(); it != mItems.end(); ++it) {
        if ((*it)->IsRemoteMarked() != (*it)->GetPPD()->IsInWiiRemote()) {
            changed = true;
            break;
        }
    }
    if (!changed) {
        PlaySound(0x5A);
        return;
    }
    PlaySound(9);
    if (!AskYesNo(GetMessageString(0x60099, nullptr))) {
        return;
    }
    CRemoteMemory* pRemote = IGameSession::GetInstance()->GetRemoteMemory(
        IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController()->GetChannel());
    pRemote->Update();
    pRemote->WaitUntilIdle(true);
    if (pRemote->ConsumeFatalError()) {
        ShowMessage(GetMessageString(0x1000F, nullptr));
        return;
    }
    if (pRemote->IsAborted() || pRemote->HasFailed()) {
        ShowMessage(GetMessageString(0x6009E, nullptr));
        GetCurrentGameMap()->GetDlgPiiSelector()->ShowMultiPlayerEntrance();
        return;
    }
    std::tr1::shared_ptr<IDisconMsg::CDisable> disconDisable(new IDisconMsg::CDisable(IGameSession::GetInstance()->GetDisconMsg()));
    mRemoteMemoryLayout->GetLayout()->CreateMotion("begin");
    mRemoteMemoryLayout->GetLayout()->AppendMotion("loop");
    mRemoteMemoryLayout->GetLayout()->SetText("text", GetMessageString(0x600EC, L"ここで Wiiリモコンに\nかきこみ"));
    mRemoteMemoryLayout->GetLayout()->SetText("caution", GetMessageString(0x600ED, nullptr));
    mSavingLayout->GetLayout()->CreateMotion("begin");
    mSavingLayout->GetLayout()->AppendMotion("loop");
    OSTick start = OSGetTick();
    u32 minimumTicks = OS_MSEC_TO_TICKS(RandInt(0, 7000));
    while (OSGetTick() - start < minimumTicks) {
        YieldFiber();
    }
    PiiList fromRemote;
    PiiList toRemote;
    PiiItemVector::iterator it;
    for (it = mItems.begin(); it != mItems.end(); ++it) {
        std::tr1::shared_ptr<CPiiPersonalData> ppd = (*it)->GetPPD();
        if ((*it)->IsRemoteMarked() == ppd->IsInWiiRemote()) {
            continue;
        }
        if ((*it)->IsRemoteMarked()) {
            pRemote->QueueAdd(ppd);
            toRemote.push_back(ppd);
        } else {
            pRemote->QueueRemove(ppd);
            fromRemote.push_back(ppd);
        }
    }
    CPiiCollectionBox* pCollection = IGameSession::GetInstance()->GetPiiCollectionBox();
    pCollection->SaveSnapshot();
    for (PiiList::iterator it = fromRemote.begin(); it != fromRemote.end(); ++it) {
        std::tr1::shared_ptr<CPiiPersonalData> ppd = *it;
        IGameSession::GetInstance()->GetTeamPiiBox()->AddPersonalData(ppd);
        int species = ppd->GetSpeciesData()->GetSpecies();
        int form = ppd->GetSpeciesData()->GetFormNumber();
        bool rare = ppd->IsRareColor();
        pCollection->RecordCollection(species, form, 3, rare);
    }
    for (PiiList::iterator it = toRemote.begin(); it != toRemote.end(); ++it) {
        IGameSession::GetInstance()->GetTeamPiiBox()->RemovePersonalData(*it);
    }
    IGameSession::GetInstance()->StartMapTransitionIn();
    pRemote->PrepareWrite(1);
    while (!IGameSession::GetInstance()->IsMapChangeFinished()) {
        YieldFiber();
    }
    pRemote->WaitUntilIdle(true);
    bool hasBlockError = pRemote->IsBlockInvalid();
    pRemote->HasFailed();
    bool failed = false;
    if (pRemote->HasFailed() || hasBlockError) {
        failed = true;
    }
    bool restoreMap = false;
    if (!failed) {
        if (RandBool(0.5f)) {
            pRemote->PrepareWrite(2);
            pRemote->WaitUntilIdle(true);
            failed = pRemote->HasFailed();
            if (!failed) {
                IGameSession::GetInstance()->UpdateMapTransition();
                while (!IGameSession::GetInstance()->IsMapChangeFinished()) {
                    YieldFiber();
                }
            }
        } else {
            IGameSession::GetInstance()->UpdateMapTransition();
            while (!IGameSession::GetInstance()->IsMapChangeFinished()) {
                YieldFiber();
            }
            pRemote->PrepareWrite(2);
            pRemote->WaitUntilIdle(true);
            failed = pRemote->HasFailed();
            restoreMap = failed;
        }
    }
    if (failed) {
        CPiiBox* pBox = pRemote->GetPiiBox();
        for (PiiList::iterator it = fromRemote.begin(); it != fromRemote.end(); ++it) {
            pBox->RemovePersonalData(*it);
            pBox->AddPersonalData(*it);
            pRemote->QueueAdd(*it);
        }
        for (PiiList::iterator it = toRemote.begin(); it != toRemote.end(); ++it) {
            pRemote->QueueRemove(*it);
        }
        IGameSession::GetInstance()->GetPiiCollectionBox()->RestoreSnapshot();
        for (PiiList::iterator it = fromRemote.begin(); it != fromRemote.end(); ++it) {
            IGameSession::GetInstance()->GetTeamPiiBox()->RemovePersonalData(*it);
        }
        for (PiiList::iterator it = toRemote.begin(); it != toRemote.end(); ++it) {
            IGameSession::GetInstance()->GetTeamPiiBox()->AddPersonalData(*it);
        }
        if (restoreMap) {
            IGameSession::GetInstance()->StartMapTransitionOut();
            while (!IGameSession::GetInstance()->IsMapChangeFinished()) {
                YieldFiber();
            }
        }
    }
    while (OSGetTick() - start < OS_SEC_TO_TICKS(10)) {
        YieldFiber();
    }
    std::tr1::shared_ptr<CLayout::CMotionProgram> endMotion = mSavingLayout->GetLayout()->QueueMotion("end");
    endMotion->WaitStart();
    mRemoteMemoryLayout->GetLayout()->CreateMotion("end")->Wait();
    endMotion->Wait();
    disconDisable.reset();
    if (pRemote->HasFailed() || hasBlockError) {
        if (pRemote->ConsumeFatalError()) {
            ShowMessage(GetMessageString(0x1000F, nullptr));
        } else {
            ShowMessage(GetMessageString(0x6009C, nullptr));
        }
    }
    pRemote->GetPiiBox();
}

#pragma dont_inline reset

inline CDlgPiiListImpl::CDlgPiiListImpl(int player, int type)
    : mPlayer(player), mType(type), mResult(0), mHolding(false), mFavoriteMode(false), mUIManager(new CUIManager),
      mRootLayout(new BUILayout), mPiiSpec(new CUIPiiSpec), mPiiList(new CUIPiiList), mPlayerID(new CUIPlayerID),
      mHelpLayout(new BUILayout), mTitleLayout(new BUILayout), mRemoteMemoryLayout(new BUILayout),
      mSavingLayout(new BUILayout), mFlag400(false) {
    mItems.clear();
    AppendItems(IGameSession::GetInstance()->GetTeamPiiBox()->GetPersonalDataList());
    if (mType == 1) {
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
        if (pPlayer->IsJoined()) {
            IController* pController = pPlayer->GetController();
            if (pController != nullptr && pController->IsConnected() && pController->GetType() == IController::TYPE_WII_REMOTE) {
                AppendItems(IGameSession::GetInstance()->GetRemoteMemory(pController->GetChannel())->GetPiiBox()->GetPersonalDataList());
            }
        }
    }
    Setup(false);
    mFiber.Start(std::tr1::bind(&CDlgPiiListImpl::Run, this), 0x8000);
}

inline void CDlgPiiListImpl::ShowMessage(const wchar_t* pText) {
    if (!mMsgWin) {
        mMsgWin = IDlgMsgWin::Create(false, true);
        mMsgWin->SetControllerSlot(mPlayer);
    }
    mMsgWin->ShowMessage(pText);
    mMsgWin->WaitUntilClosed();
}

inline bool CDlgPiiListImpl::AskYesNo(const wchar_t* pText) {
    if (!mMsgWin) {
        mMsgWin = IDlgMsgWin::Create(false, true);
        mMsgWin->SetControllerSlot(mPlayer);
    }
    bool yes = mMsgWin->AskYesNo(pText);
    mMsgWin->WaitUntilClosed();
    return yes;
}

inline void CDlgPiiListImpl::AppendItems(const std::list<std::tr1::shared_ptr<CPiiPersonalData> >& rList) {
    for (std::list<std::tr1::shared_ptr<CPiiPersonalData> >::const_iterator it = rList.begin(); it != rList.end(); ++it) {
        std::tr1::shared_ptr<CUIPiiListItem> item(new CUIPiiListItem(std::tr1::shared_ptr<CPiiPersonalData>(*it), mPlayer));
        mItems.push_back(item);
    }
}

inline void CDlgPiiListImpl::Setup(bool background) {
    std::tr1::shared_ptr<CLayout::FontMap> fonts = GetDefaultFontMap();
    const char* const* pArchives = gSharedLayoutArchives;
    mRootLayout->Load(pArchives, "dlgpiilist_root", fonts, background);
    mPiiSpec->Load(pArchives, "piispec2", fonts, background);
    mPiiList->Load(pArchives, "dlgpiilist_list", fonts, background);
    mPlayerID->Setup(mPlayer, 0, pArchives, "playerid", fonts, background);
    mHelpLayout->Load(pArchives, "dlgpiilist_help", fonts, background);
    mTitleLayout->Load(pArchives, "dlgpiilist_title", fonts, background);
    mRemoteMemoryLayout->Load(pArchives, "remote_memory", fonts, background);
    mRemoteMemoryLayout->GetLayout()->CreateMotion("init");
    mSavingLayout->Load(pArchives, "saving", fonts, background);
    mUIManager->AddChild(mRootLayout);
    mRootLayout->AttachChild("PIISPEC", mPiiSpec);
    mRootLayout->AttachChild("LIST", mPiiList);
    mRootLayout->AttachChild("PLAYERID", mPlayerID);
    mRootLayout->AttachChild("HELP", mHelpLayout);
    mRootLayout->AttachChild("TITLE", mTitleLayout);
    mRootLayout->AddChild(mRemoteMemoryLayout);
    mRootLayout->AddChild(mSavingLayout);
    mPiiList->SetScrollRange(2, mPiiList->GetLayoutCount() - 2);
    mPiiList->GetSelectionCallbacks().Add(std::tr1::bind(&CDlgPiiListImpl::OnSelectionChanged, this, std::tr1::placeholders::_1));
    mPiiList->GetAcceptCallbacks().Add(std::tr1::bind(&CDlgPiiListImpl::OnAccept, this, std::tr1::placeholders::_1));
    mPiiList->SetScrollMode(1);
    mPiiList->SetSelectionInputEnabled(true);
    mPiiList->SetActivateOnHold(true);
    mPlayerID->Update();
    switch (mType) {
    case 1:
        mTitleLayout->GetLayout()->SetText("TITLE", GetMessageString(0x6008D, nullptr));
        CColorBtnTagProc::Attach(mHelpLayout->GetLayout()->SetText(
            "HELP", (GetButtonMessage(mPlayer, 8, 0x600A4) + GetButtonMessage(mPlayer, 0x20, 0x600A1) +
                     GetButtonMessage(mPlayer, 0x10, 0x6009F) + GetButtonMessage(mPlayer, 0x100, 0x600A6) +
                     GetButtonMessage(mPlayer, 0x800, 0x600A5)).c_str()));
        break;
    case 2:
        mTitleLayout->GetLayout()->SetText("TITLE", GetMessageString(0x6008E, nullptr));
        CColorBtnTagProc::Attach(mHelpLayout->GetLayout()->SetText(
            "HELP", (GetButtonMessage(mPlayer, 8, 0x600A4) + GetButtonMessage(mPlayer, 0x20, 0x600A3) +
                     GetButtonMessage(mPlayer, 0x10, 0x6009F) + GetButtonMessage(mPlayer, 0x100, 0x600A6) +
                     GetButtonMessage(mPlayer, 0x800, 0x600A5)).c_str()));
        break;
    }
    mUIManager->SetFocusedUI(mPiiList.get());
}

inline void CDlgPiiListImpl::OnSelectionChanged(BUI* pUI) {
    if (!mFlag400) {
        mFlag400 = true;
    } else {
        PlaySound(0x54);
    }
    std::tr1::shared_ptr<IUIListItem> selected = mPiiList->GetSelectedItem();
    if (!selected) {
        return;
    }
    std::tr1::shared_ptr<CUIPiiListItem> item = std::tr1::static_pointer_cast<CUIPiiListItem>(selected);
    if (item->GetPPD()->IsSpecHidden()) {
        item->GetPPD()->SetNewlyDisclosed(true);
        item->GetPPD()->SetSpecHidden(false);
        mPiiList->UpdateSelection();
    }
    IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
    if (pController != nullptr && (pController->GetHold() & 0x800) && !(pController->GetTrig() & 0x800)) {
        if (item->GetPPD()->IsFavorite()) {
            if (!mFavoriteMode) {
                item->GetPPD()->SetFavorite(false);
                PlaySound(0x5A);
            }
        } else if (mFavoriteMode) {
            item->GetPPD()->SetFavorite(true);
            PlaySound(9);
        }
    }
    mPiiSpec->SetPii(item->GetPPD(), mPlayer);
}

inline int CDlgPiiListImpl::CountRemoteMarked() {
    int count = 0;
    for (PiiItemVector::const_iterator it = mItems.begin(); it != mItems.end(); ++it) {
        if ((*it)->IsRemoteMarked()) {
            count++;
        }
    }
    return count;
}

inline int CDlgPiiListImpl::CountRemoteUnmarked() {
    int count = 0;
    for (PiiItemVector::const_iterator it = mItems.begin(); it != mItems.end(); ++it) {
        if (!(*it)->IsRemoteMarked()) {
            count++;
        }
    }
    return count;
}

inline int CDlgPiiListImpl::CountSellUnmarked() {
    int count = 0;
    for (PiiItemVector::const_iterator it = mItems.begin(); it != mItems.end(); ++it) {
        if (!(*it)->IsSellMarked()) {
            count++;
        }
    }
    return count;
}

inline int CDlgPiiListImpl::GetMinUnmarkedCount() {
    int joined = 0;
    for (int player = 0; player < 4; player++) {
        if (IGameSession::GetInstance()->GetPlayer(player)->IsJoined()) {
            joined++;
        }
    }
    return std::max(joined, sMinUnmarkedCount);
}

inline void CDlgPiiListImpl::OnAccept(BUI* pUI) {
    IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
    bool holdMode = false;
    if (pController != nullptr && (pController->GetTrig() & 0x20)) {
        holdMode = true;
    }
    if (holdMode) {
        mHolding = false;
    }
    std::tr1::shared_ptr<IUIListItem> selected = mPiiList->GetSelectedItem();
    if (selected) {
    std::tr1::shared_ptr<CUIPiiListItem> item = std::tr1::static_pointer_cast<CUIPiiListItem>(selected);
    int sound = 0;
    switch (mType) {
    case 1:
        if (IGameSession::GetInstance()->GetOyaInfo()->GetCount() != item->GetPPD()->GetOriginalTrainerId() &&
            item->GetPPD()->GetCappedLevel() < item->GetPPD()->GetLevel()) {
            mResult = 0x11;
            break;
        }
        if ((holdMode && item->IsRemoteMarked()) || (!holdMode && !mHolding)) {
            if (CountRemoteUnmarked() < 1000) {
                sound = 0;
                if (item->IsRemoteMarked()) {
                    sound = 2;
                }
                item->SetRemoteMarked(false);
            } else {
                mResult = 0xF;
            }
        } else {
            CPlayer* pHolder;
            pHolder = FindPlayerHoldingPii(item->GetPPD(), mPlayer);
            if (pHolder != nullptr) {
                mResult = pHolder->GetPlayerNo() + 10;
                break;
            }
            if (CountRemoteMarked() + 1 > 12) {
                mResult = 0xE;
                break;
            }
            if (CountRemoteUnmarked() <= GetMinUnmarkedCount() && !item->IsRemoteMarked()) {
                mResult = 0x10;
                break;
            }
            sound = item->IsRemoteMarked() ^ 1;
            item->SetRemoteMarked(true);
            if (holdMode) {
                mHolding = true;
            }
        }
        break;
    case 2:
        if (item->GetPPD()->IsFavorite()) {
            mResult = 8;
            break;
        }
        if ((holdMode && item->IsSellMarked()) || (!holdMode && !mHolding)) {
            sound = 0;
            if (item->IsSellMarked()) {
                sound = 2;
            }
            item->SetSellMarked(false);
        } else {
            CPlayer* pHolder;
            pHolder = FindPlayerHoldingPii(item->GetPPD(), -1);
            if (pHolder != nullptr) {
                mResult = pHolder->GetPlayerNo() + 4;
                break;
            }
            if (CountSellUnmarked() <= GetMinUnmarkedCount() && !item->IsSellMarked()) {
                mResult = 9;
                break;
            }
            sound = item->IsSellMarked() ^ 1;
            item->SetSellMarked(true);
            if (holdMode) {
                mHolding = true;
            }
        }
        break;
    }
    switch (sound) {
    case 2:
        PlaySound(0x5A);
        break;
    case 1:
        PlaySound(9);
        break;
    }
    }
}

inline void CDlgPiiListImpl::OnSortChanged(const CPiiListFilterSort* pFilterSort) {
    const_cast<CPiiListFilterSort*>(pFilterSort)->Filter(mPiiList, mItems, mPlayer);
    mPiiList->SelectIndex(0);
}

inline void CDlgPiiListImpl::Run() {
    YieldFiberStarted();
    if (mType == 1) {
        IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
        if (pController == nullptr || !pController->IsConnected() || pController->GetType() != IController::TYPE_WII_REMOTE) {
            ShowMessage(GetMessageString(0x6008F, nullptr));
            YieldFiber();
            return;
        }
        CRemoteMemory* pRemote = IGameSession::GetInstance()->GetRemoteMemory(pController->GetChannel());
        pRemote->WaitUntilIdle(true);
        if (pRemote->ConsumeFatalError()) {
            ShowMessage(GetMessageString(0x1000F, nullptr));
            return;
        }
        pRemote->Update();
        pRemote->WaitUntilIdle(true);
        if (pRemote->ConsumeFatalError()) {
            ShowMessage(GetMessageString(0x1000F, nullptr));
            return;
        }
        if (pRemote->IsAborted() || pRemote->HasFailed()) {
            ShowMessage(GetMessageString(0x6009E, nullptr));
            GetCurrentGameMap()->GetDlgPiiSelector()->ShowMultiPlayerEntrance();
            return;
        }
        if (pRemote->IsRecoveryPending()) {
            if (!AskYesNo(GetMessageString(0x6009A, nullptr))) {
                return;
            }
        }
        if (pRemote->IsStorageLocked()) {
            ShowMessage(GetMessageString(0x6009B, nullptr));
        }
    }
    RunList();
}

inline void CDlgPiiListImpl::Finish() {
    switch (mType) {
    case 1:
        RunRemoteTransfer();
        break;
    case 2:
        RunSell();
        break;
    }
}

inline void CDlgPiiListImpl::RunList() {
    mFilterSort.Filter(mPiiList, mItems, mPlayer);
    mPiiList->SelectIndex(0);
    mRootLayout->GetLayout()->CreateMotion("begin")->Wait();
    mResult = 0;
    for (;;) {
        IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
        if (pController == nullptr) {
            mResult = 1;
        } else if (pController->GetTrig() & 0x10) {
            mResult = 1;
        } else if (pController->GetTrig() & 0x100) {
            mResult = 2;
        } else if (pController->GetTrig() & 0x800) {
            mResult = 3;
        } else {
            mUIManager->DispatchFocusedInput(pController);
        }
        int result = mResult;
        mResult = 0;
        switch (result) {
        case 2:
            YieldFiber();
            mFilterSort.ShowSortMenu(mUIManager, mPlayer, mItems,
                                     std::tr1::bind(&CDlgPiiListImpl::OnSortChanged, this, std::tr1::placeholders::_1));
            break;
        case 3: {
            std::tr1::shared_ptr<IUIListItem> selected = mPiiList->GetSelectedItem();
            if (selected) {
                std::tr1::shared_ptr<CUIPiiListItem> item = std::tr1::static_pointer_cast<CUIPiiListItem>(selected);
                bool favorite = !item->GetPPD()->IsFavorite();
                item->GetPPD()->SetFavorite(favorite);
                if (favorite) {
                    PlaySound(9);
                } else {
                    PlaySound(0x5A);
                }
                mFavoriteMode = favorite;
            }
            break;
        }
        case 4:
        case 5:
        case 6:
        case 7:
            YieldFiber();
            ShowMessage(FormatMessage(GetMessageString(0x60090, nullptr), GetMessageString(0x1003E + result, nullptr)).c_str());
            break;
        case 8:
            YieldFiber();
            ShowMessage(GetMessageString(0x60091, nullptr));
            break;
        case 9: {
            YieldFiber();
            NIntl::basic_formatter<wchar_t> formatter(GetMessageString(0x60092, nullptr));
            formatter.format(L"%d") % GetMinUnmarkedCount();
            ShowMessage(formatter.c_str());
            break;
        }
        case 10:
        case 11:
        case 12:
        case 13: {
            YieldFiber();
            ShowMessage(FormatMessage(GetMessageString(0x60093, nullptr), GetMessageString(0x10038 + result, nullptr)).c_str());
            break;
        }
        case 14: {
            YieldFiber();
            NIntl::basic_formatter<wchar_t> formatter(GetMessageString(0x60094, nullptr));
            formatter.format(L"%d") % sMaxRemoteMarkedCount;
            ShowMessage(formatter.c_str());
            break;
        }
        case 15:
            YieldFiber();
            ShowMessage(GetMessageString(0x60095, nullptr));
            break;
        case 16: {
            YieldFiber();
            NIntl::basic_formatter<wchar_t> formatter(GetMessageString(0x60096, nullptr));
            formatter.format(L"%d") % GetMinUnmarkedCount();
            ShowMessage(formatter.c_str());
            break;
        }
        case 17:
            YieldFiber();
            ShowMessage(GetMessageString(0x60097, nullptr));
            break;
        case 1:
            goto finished;
        }
        YieldFiber();
    }
finished:
    YieldFiber();
    mRootLayout->GetLayout()->CreateMotion("end")->Wait();
    Finish();
}


inline void CDlgPiiListImpl::RunSell() {
    PiiList sellList;
    for (PiiItemVector::const_iterator it = mItems.begin(); it != mItems.end(); ++it) {
        if ((*it)->IsSellMarked()) {
            sellList.push_back((*it)->GetPPD());
        }
    }
    if (sellList.empty()) {
        PlaySound(0x5A);
        return;
    }
    PlaySound(9);
    if (!AskYesNo(GetMessageString(0x60098, nullptr))) {
        return;
    }
    for (PiiList::iterator it = sellList.begin(); it != sellList.end(); ++it) {
        (*it)->SetSelling(true);
    }
    std::tr1::shared_ptr<CPpuEngine> engine(new CPpuEngine);
    engine->Execute("ToyBoxRecipe.pkc", 1, IGameSession::GetInstance()->GetEnvironment());
    for (PiiList::iterator it = sellList.begin(); it != sellList.end(); ++it) {
        (*it)->SetSelling(false);
        IGameSession::GetInstance()->GetTeamPiiBox()->RemovePersonalData(*it);
    }
}


class CDlgPiiList : public IDlgPiiList, public std::tr1::enable_shared_from_this<CDlgPiiList> {
public:
    CDlgPiiList() {}
    virtual bool IsClosable() { return false; }
    virtual bool Close() {
        if (mImpl) {
            mImpl->mResult = 1;
        }
        GetCurrentGameMap()->UnregisterDialog(shared_from_this());
        return true;
    }
    virtual int GetType() {
        if (mImpl) {
            return mImpl->mType;
        }
        return 0;
    }
    virtual bool Show(int player, int type);
    virtual void Calc() {
        if (mImpl) {
            CDlgPiiListImpl* pImpl = mImpl.get();
            pImpl->mPiiList->UpdateSelection();
            pImpl->mUIManager->Calc();
            if (pImpl->mMsgWin) {
                pImpl->mMsgWin->Calc();
            }
            pImpl->mFiber.Resume();
            if (mImpl->mFiber.IsFinished()) {
                mImpl.reset();
                GetCurrentGameMap()->UnregisterDialog(shared_from_this());
            }
        }
    }
    virtual void CalcMtx() {
        if (mImpl) {
            CDlgPiiListImpl* pImpl = mImpl.get();
            pImpl->mUIManager->CalcMtx();
            if (pImpl->mMsgWin) {
                pImpl->mMsgWin->CalcMtx();
            }
        }
    }
    virtual void CalcView() {
        if (mImpl) {
            CDlgPiiListImpl* pImpl = mImpl.get();
            pImpl->mUIManager->CalcView();
            if (pImpl->mMsgWin) {
                pImpl->mMsgWin->CalcView();
            }
        }
    }
    virtual void Draw() {
        if (mImpl) {
            CDlgPiiListImpl* pImpl = mImpl.get();
            pImpl->mUIManager->Draw();
            if (pImpl->mMsgWin) {
                pImpl->mMsgWin->Draw();
            }
        }
    }


private:
    std::tr1::shared_ptr<CDlgPiiListImpl> mImpl;
};

}

std::tr1::shared_ptr<IDlgPiiList> IDlgPiiList::Create(bool) {
    return std::tr1::shared_ptr<CDlgPiiList>(new CDlgPiiList);
}

namespace {

inline bool CDlgPiiList::Show(int player, int type) {
    if (mImpl) {
        if (mImpl->mType != type) {
            Close();
            return false;
        }
        return true;
    }
    if (!GetCurrentGameMap()->RegisterDialog(shared_from_this())) {
        return false;
    }
    GetCurrentGameMap()->SetPausing(true);
    CDlgPiiListImpl* pImpl = new CDlgPiiListImpl(player, type);
    mImpl.reset(pImpl);
    return true;
}


}
