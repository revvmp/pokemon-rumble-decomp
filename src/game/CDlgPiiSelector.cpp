#include <game/IDlgPiiSelector.h>

#include <game/CFiber.h>
#include <game/CLayout.h>
#include <game/CNoticeDisplay.h>
#include <game/ControllerTable.h>
#include <game/CPiiListFilterSort.h>
#include <game/CPlayer.h>
#include <game/CRemoteMemory.h>
#include <game/CUIManager.h>
#include <game/CUIPiiList.h>
#include <game/CUIPiiListItem.h>
#include <game/CUIPiiSpec.h>
#include <game/CUIPlayerID.h>
#include <game/IDlgMsgWin.h>
#include <game/IGameMap.h>
#include <game/IGameSession.h>
#include <memory>
#include <nw4r/math/math_types.h>
#include <vector>

#pragma dont_inline on

namespace {

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

class CDataPerPlayer {
public:
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

    bool IsDisconnected() { return mDisconnected; }
    bool IsFinished();
    bool IsShown();

private:
    IDlgPiiSelectorInternal* mpDlg;
    int mState;
    int mPlayer;
    int mReservedMode;
    bool mFavoriteMode;
    std::vector<std::tr1::shared_ptr<CUIPiiListItem> > mItems;
    CPiiListFilterSort mFilterSort;
    nw4r::math::VEC2 mReservedPosition;
    nw4r::math::VEC2 mReservedTargetPosition;
    bool mReservedFlag50;
    bool mSilent;
    bool mReservedFlag52;
    std::tr1::shared_ptr<BUI> mReservedUI54;
    std::tr1::shared_ptr<BUI> mReservedUI5C;
    std::tr1::shared_ptr<BUI> mReservedUI64;
    int mReserved6C;
    int mReserved70;
    int mReserved74;
    int mReserved78;
    int mReserved7C;
    int mReserved80;
    std::tr1::shared_ptr<CUIManager> mUIManager;
    std::tr1::shared_ptr<BUILayout> mReservedLayout8C;
    std::tr1::shared_ptr<BUILayout> mReservedLayout94;
    std::tr1::shared_ptr<CUIPiiSpec> mPiiSpec;
    std::tr1::shared_ptr<CUIPiiList> mPiiList;
    std::tr1::shared_ptr<CUIPlayerID> mPlayerID;
    std::tr1::shared_ptr<BUILayout> mReservedLayoutB4;
    nw4r::math::VEC2 mReservedVectorBC;
    IController* mpController;
    u32 mDisconnectSerial;
    bool mDisconnected;
    int mReservedD0;
    CFiber mFiber;

public:
    virtual ~CDataPerPlayer();
};

class IDlgPiiSelectorInternal : public IDlgPiiSelector {
public:
    virtual ~IDlgPiiSelectorInternal() {}
    virtual std::tr1::shared_ptr<CPiiPersonalData> GetEntrancePPD(IController* pController) = 0;
    virtual void UpdateDlgOpenState() = 0;
};

class CDlgPiiSelector : public IDlgPiiSelectorInternal, public std::tr1::enable_shared_from_this<CDlgPiiSelector> {
public:
    struct MultiPlayerData {
        int mState;
        std::tr1::shared_ptr<CPiiPersonalData> mEntrancePPD[4][4];
        int mPlayerNum;
        CLayout mLayout;
        std::tr1::shared_ptr<IDlgMsgWin> mMsgWin;
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

    inline virtual void Show(int player, bool flag);
    virtual bool IsMultiPlayerEntranceShown() { return mMultiPlayerData; }
    inline virtual void ShowMultiPlayerEntrance();

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
