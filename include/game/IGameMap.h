#ifndef GAME_IGAMEMAP_H
#define GAME_IGAMEMAP_H

#include <game/ICalcDraw.h>
#include <memory>

class BWorld;
struct MapDesc;

namespace NPpu {
class CEnvironment;
}

class IWorldObjStatDisplay;
class IMiniMap;
class IDlgPiiSelector;
class IDlgPiiList;
class IDlgCollection;
class IDlgInfo;
class IDlgMsgWin;
class INoticeDisplay;
class IMoneyLifeDisplay;
class IBattleRoyalDisplay;
class IComboDisplay;
class ILayoutManager;
class IPauseMenu;
class ITutorialMsg;
class IMsgBalloon;
class IEnemyInfo;
class INumericInput;
class IPostEffect;
class IDlg;

class IGameMap : public ICalcDraw, public std::tr1::enable_shared_from_this<IGameMap> {
public:
    struct Param {
        Param() : mMapDesc(nullptr) {}

        MapDesc* mMapDesc;
        std::tr1::shared_ptr<NPpu::CEnvironment> mEnvironment;
    };

    virtual ~IGameMap() {}
    virtual void RunAction() = 0;
    virtual const MapDesc& GetMapDesc() const = 0;
    virtual BWorld* GetWorld() const = 0;
    virtual IWorldObjStatDisplay* GetWorldObjStatDisplay() { return nullptr; }
    virtual IMiniMap* GetMiniMap() { return nullptr; }
    virtual IDlgPiiSelector* GetDlgPiiSelector() { return nullptr; }
    virtual IDlgPiiList* GetDlgPiiList() { return nullptr; }
    virtual IDlgCollection* GetDlgCollection() { return nullptr; }
    virtual IDlgInfo* GetDlgInfo() { return nullptr; }
    virtual IDlgMsgWin* GetDlgMsgWin() { return nullptr; }
    virtual INoticeDisplay* GetNoticeDisplay() { return nullptr; }
    virtual IMoneyLifeDisplay* GetMoneyLifeDisplay() { return nullptr; }
    virtual IBattleRoyalDisplay* GetBattleRoyalDisplay() { return nullptr; }
    virtual IComboDisplay* GetComboDisplay() { return nullptr; }
    virtual ILayoutManager* GetLayoutManager() { return nullptr; }
    virtual IPauseMenu* GetPauseMenu() { return nullptr; }
    virtual ITutorialMsg* GetTutorialMsg() { return nullptr; }
    virtual IMsgBalloon* GetMsgBalloon() { return nullptr; }
    virtual IEnemyInfo* GetEnemyInfo() { return nullptr; }
    virtual INumericInput* GetNumericInput() { return nullptr; }
    virtual IPostEffect* GetPostEffect() { return nullptr; }
    virtual IDlg* GetCurrentDialog() { return nullptr; }
    virtual bool RegisterDialog(const std::tr1::shared_ptr<IDlg>& rDialog) { return false; }
    virtual void UnregisterDialog(const std::tr1::shared_ptr<IDlg>& rDialog) {}
    virtual void SetPausing(bool pausing) {}
    virtual bool IsPausing() { return false; }
    virtual void Hitstop(int frames) {}
    virtual bool IsHitstopping() { return false; }
    virtual void EnterEvent() {}
    virtual void ExitEvent() {}
    virtual bool IsInEvent() { return false; }
    virtual void StartTimer(int frames, int step) {}
    virtual void AddTimer(int frames) {}
    virtual void StopTimer() {}
    virtual bool IsTimerWorking() { return false; }
    virtual int GetTimerFrame() { return 0; }
    virtual void DisappearModelObjs() {}
    virtual void AppearModelObjs() {}

    static std::tr1::shared_ptr<IGameMap> Create(const Param& rParam);
};

IGameMap* GetCurrentGameMap();
IGameMap* GetActiveGameMap();

#endif
