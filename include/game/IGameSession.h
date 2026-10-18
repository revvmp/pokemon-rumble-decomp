#ifndef GAME_IGAMESESSION_H
#define GAME_IGAMESESSION_H

#include <game/ISession.h>
#include <iosfwd>
#include <revolution/types.h>

struct MapIndex;
struct MapDesc;
struct StageDesc;
struct WazaDescData;
struct PiiPropData;
class CBattleRoyalData;
class CPiiBox;
class CPiiCollectionBox;
class CPlayer;
class CRemoteMemory;
class CStatistic;
class IGameStage;

class IGameSession : public ISession {
public:
    virtual ~IGameSession() {}
    virtual WazaDescData* GetWazaDescData() = 0;
    virtual PiiPropData* GetPiiPropData() = 0;
    virtual CPiiCollectionBox* GetPiiCollectionBox() = 0;
    virtual int GetCollectionType(u16 monsno) = 0;
    virtual CBattleRoyalData* GetBattleRoyalData() = 0;
    virtual CStatistic* GetStatistic() = 0;
    virtual std::wstring GetWazaName(int waza) = 0;
    virtual StageDesc* GetStageDesc(MapIndex index) = 0;
    virtual MapDesc* GetMapDesc(const MapIndex& rIndex) = 0;
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

    static IGameSession* GetInstance();
};

#endif
