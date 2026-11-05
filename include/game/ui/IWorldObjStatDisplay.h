#ifndef GAME_IWORLDOBJSTATDISPLAY_H
#define GAME_IWORLDOBJSTATDISPLAY_H

#include <game/core/ICalcDraw.h>
#include <memory>

class BWorld;
class BWorldObj;

class IWorldObjStatDisplay : public ICalcDraw {
public:
    virtual ~IWorldObjStatDisplay() {}
    virtual void Add(BWorldObj* pObj) = 0;

    static std::tr1::shared_ptr<IWorldObjStatDisplay> Create(BWorld* pWorld);
};

#endif
