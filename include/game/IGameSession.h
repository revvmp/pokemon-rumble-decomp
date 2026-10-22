#ifndef GAME_IGAMESESSION_H
#define GAME_IGAMESESSION_H

#include <game/ISession.h>
#include <game/WazaDescData.h>
#include <iosfwd>
#include <memory>
#include <revolution/types.h>

struct MapIndex;
struct MapDesc;
struct StageDesc;
struct PiiPropData;
class CBattleRoyalBox;
class IController;
class IDisconMsg;
class CPiiBox;
class CPiiCollectionBox;
class CPlayer;
class CRemoteMemory;
class CStatistic;
class IController;
class IGameStage;
class COyaInfo;
class CPiiPersonalData;

namespace NPpu {
class CEnvironment;
}

class IGameSession : public ISession {
public:
    virtual ~IGameSession() {}
    virtual WazaDescData* GetWazaDescData() = 0;
    virtual PiiPropData* GetPiiPropData() = 0;
    virtual CPiiCollectionBox* GetPiiCollectionBox() = 0;
    virtual int GetCollectionType(u16 monsno) = 0;
    virtual CBattleRoyalBox* GetBattleRoyalBox() = 0;
    virtual CStatistic* GetStatistic() = 0;
    virtual std::wstring GetWazaName(int waza) = 0;
    virtual StageDesc* GetStageDesc(MapIndex index) = 0;
    virtual MapDesc* GetMapDesc(MapIndex index) = 0;
    virtual MapDesc* CreateMapDesc(const MapIndex& rIndex) = 0;
    virtual int MovePlayerPiisToTeamPiiBox() = 0;
    virtual int GetMoney() = 0;
    virtual void SetMoney(int money) = 0;
    virtual void AddMoney(int money) = 0;
    virtual IGameStage* GetGameStage() = 0;
    virtual bool IsAllPiiBoxCapacityFull() = 0;
    virtual CPiiBox* GetTeamPiiBox() = 0;
    virtual CPlayer* GetPlayer(int player) = 0;
    virtual CRemoteMemory* GetRemoteMemory(int player) = 0;
    virtual COyaInfo* GetOyaInfo() = 0;
    virtual std::tr1::shared_ptr<NPpu::CEnvironment> GetEnvironment() = 0;
    virtual int AddPPDToPlayerPiiBox(int player, const std::tr1::shared_ptr<CPiiPersonalData>& rPPD) = 0;
    virtual bool CanAddPPDToPlayerPiiBox(int player, const std::tr1::shared_ptr<CPiiPersonalData>& rPPD) = 0;
    virtual bool IsReplacePiiAvailable(int player) = 0;
    virtual f32 GetPlayTimeSeconds() = 0;
    virtual int GetMapChangeType() = 0;
    virtual void SetFade(int type, int frames) = 0;
    virtual int GetFadeFrames() = 0;
    virtual void RequestMapChange(MapIndex map, int type, int frames) = 0;
    virtual void RequestSceneChange(int type, int frames) = 0;
    virtual void StartMapTransitionOut() = 0;
    virtual void StartMapTransitionIn() = 0;
    virtual void UpdateMapTransition() = 0;
    virtual bool IsMapChangeFinished() = 0;
    virtual IDisconMsg* GetDisconMsg() = 0;
    virtual bool IsControllerAssigned(const IController* pController) = 0;
    virtual void RegisterTeamPiiCollection() = 0;

    static IGameSession* GetInstance();
    static std::tr1::shared_ptr<IGameSession> Create(IDisconMsg* pDisconMsg);
};

IGameSession* GetGameSession();

#endif
