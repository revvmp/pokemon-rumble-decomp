#include <game/IDlgPiiSelector.h>

#include <game/CRemoteMemory.h>
#include <game/IGameSession.h>
#include <memory>

#pragma dont_inline on

namespace {

class CDataPerPlayer {
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
    struct MultiPlayerData;

    CDlgPiiSelector() : mIsPausing(false), mInputLocked(false), mErrorState(0), mError(0) {}
    virtual ~CDlgPiiSelector();
    virtual void Calc();
    virtual void CalcMtx();
    virtual void CalcView();
    virtual void Draw();
    virtual bool IsClosable() { return false; }
    virtual bool Close() { return false; }
    virtual bool IsShown(int player);
    virtual void Show(int player, bool flag);
    virtual bool IsMultiPlayerEntranceShown();
    virtual void ShowMultiPlayerEntrance();
    virtual void SetEntrancePPD(IController* pController, const std::tr1::shared_ptr<CPiiPersonalData>& rPPD);
    virtual std::tr1::shared_ptr<CPiiPersonalData> GetEntrancePPD(IController* pController);
    virtual void UpdateDlgOpenState();

    void Init(bool flag) {}

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
