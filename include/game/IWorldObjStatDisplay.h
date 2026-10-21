#ifndef GAME_IWORLDOBJSTATDISPLAY_H
#define GAME_IWORLDOBJSTATDISPLAY_H

#include <game/ICalcDraw.h>
#include <memory>

class BWorld;

class IWorldObjStatDisplay : public ICalcDraw {
public:
    virtual ~IWorldObjStatDisplay() {}

    static std::tr1::shared_ptr<IWorldObjStatDisplay> Create(BWorld* pWorld);
};

#endif
