#ifndef GAME_IMULTIRANKING_H
#define GAME_IMULTIRANKING_H

#include <game/IGameMap.h>

class IMultiRanking : public IGameMap {
public:
    static std::tr1::shared_ptr<IMultiRanking> Create(const MapDesc* pDesc, bool background);
};

#endif
