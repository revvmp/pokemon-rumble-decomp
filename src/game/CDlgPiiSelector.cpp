#include <game/IDlgPiiSelector.h>

#include <game/CLayout.h>
#include <game/ControllerTable.h>
#include <game/CPlayer.h>
#include <game/CRemoteMemory.h>
#include <game/IDlgMsgWin.h>
#include <game/IGameSession.h>
#include <memory>

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

class CDataPerPlayer {
public:
    virtual ~CDataPerPlayer();
    void CalcMtx();
    void CalcView();
    bool IsShown();
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
    inline virtual void Calc();

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

    inline virtual void Draw();
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
