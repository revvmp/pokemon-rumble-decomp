#ifndef GAME_IGAMERESULT_H
#define GAME_IGAMERESULT_H

#include <game/world/IGameMap.h>

class IGameResult : public IGameMap {
public:
    virtual ~IGameResult() {}

    static std::tr1::shared_ptr<IGameResult> Create(const MapDesc* pDesc, bool background);
};

#endif
