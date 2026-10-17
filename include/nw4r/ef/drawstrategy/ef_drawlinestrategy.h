#ifndef NW4R_EF_DRAW_STRATEGY_DRAWLINESTRATEGY_H
#define NW4R_EF_DRAW_STRATEGY_DRAWLINESTRATEGY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r {
namespace ef {

class DrawLineStrategy : public DrawStrategyImpl {
public:
    DrawLineStrategy();

    virtual void Draw(const DrawInfo& rInfo, ParticleManager* pManager);

private:
    void InitGraphics(const DrawInfo& rInfo, ParticleManager* pManager);
};

}
}

#endif
