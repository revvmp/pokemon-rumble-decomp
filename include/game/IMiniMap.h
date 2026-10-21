#ifndef GAME_IMINIMAP_H
#define GAME_IMINIMAP_H

#include <game/ICalcDraw.h>
#include <memory>
#include <revolution/types.h>

struct MapDesc;
class BWorld;

class IMiniMap : public ICalcDraw {
public:
    virtual ~IMiniMap() {}

    static std::tr1::shared_ptr<IMiniMap> Create(MapDesc* pMapDesc, BWorld* pWorld, f32 scale, bool flag);
};

#endif
