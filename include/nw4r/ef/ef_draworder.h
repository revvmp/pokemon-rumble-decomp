#ifndef NW4R_EF_DRAW_ORDER_H
#define NW4R_EF_DRAW_ORDER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_list.h>

namespace nw4r {
namespace ef {

class Effect;
class ParticleManager;
class DrawInfo;

class DrawOrderBase {
public:
    DrawOrderBase() {}

    virtual void Add(Effect* pEffect, ParticleManager* pManager) = 0;
    virtual void Remove(Effect* pEffect,
                        ParticleManager* pManager) = 0;
    virtual void Draw(Effect* pEffect, const DrawInfo& rInfo) = 0;

    static ut::List* GetPMList(Effect* pEffect);
};

class DrawOrder : public DrawOrderBase {
public:
    virtual void Add(Effect* pEffect, ParticleManager* pManager);
    virtual void Remove(Effect* pEffect,
                        ParticleManager* pManager);
    virtual void Draw(Effect* pEffect, const DrawInfo& rInfo);
};

}
}

#endif
