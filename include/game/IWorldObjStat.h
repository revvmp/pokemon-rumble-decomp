#ifndef GAME_IWORLDOBJSTAT_H
#define GAME_IWORLDOBJSTAT_H

#include <nw4r/math/math_types.h>
#include <revolution/types.h>

class BWorldObj;

class IWorldObjStat {
public:
    virtual ~IWorldObjStat() {}
    virtual void Update(BWorldObj* pObj) = 0;
    virtual void CalcView(const nw4r::math::MTX34& rView,
                          const nw4r::math::MTX44& rProjection) = 0;
    virtual bool IsVisible() const = 0;
    virtual void Draw() = 0;
    virtual u32 GetType() const = 0;
};

#endif
