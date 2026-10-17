#ifndef NW4R_EF_DRAW_STRATEGY_DRAWPOINTSTRATEGY_H
#define NW4R_EF_DRAW_STRATEGY_DRAWPOINTSTRATEGY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r {
namespace ef {

class DrawPointStrategy : public DrawStrategyImpl {
public:
    DrawPointStrategy();

    virtual void Draw(const DrawInfo& rInfo, ParticleManager* pManager);

private:
    void InitGraphics(const DrawInfo& rInfo, ParticleManager* pManager);
};

}
}

#endif
