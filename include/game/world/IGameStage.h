#ifndef GAME_IGAMESTAGE_H
#define GAME_IGAMESTAGE_H

#include <game/core/ICalcDraw.h>
#include <game/world/MapIndex.h>
#include <memory>

class CPpuEnv;

class IGameMap;

namespace NPpu {
class CEnvironment;
}

class IGameStage : public ICalcDraw, public std::tr1::enable_shared_from_this<IGameStage> {
public:
    struct Param {
        MapIndex mMapIndex;
        std::tr1::shared_ptr<CPpuEnv> mEnvironment;
        ~Param() {}
    };

    virtual ~IGameStage() {}
    virtual void Run() = 0;
    virtual IGameMap* GetGameMap() = 0;
    virtual int GetLife() = 0;
    virtual void SetLife(int life) = 0;
    virtual void ChangeMap(MapIndex index) = 0;
    virtual void RequestState(int kind) = 0;
    virtual void SetEnvSE(int id) = 0;
    virtual void StopEnvSE() = 0;

    static std::tr1::shared_ptr<IGameStage> Create(const Param& rParam);
};

IGameStage* GetGameStage();
IGameStage* GetCurrentGameStage();

#endif
