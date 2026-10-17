#ifndef NW4R_EF_DRAW_STRATEGY_DRAWSTRIPESTRATEGY_H
#define NW4R_EF_DRAW_STRATEGY_DRAWSTRIPESTRATEGY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r {
namespace ef {

class DrawStripeStrategy : public DrawStrategyImpl {
public:
    DrawStripeStrategy();

    virtual void Draw(const DrawInfo& rInfo, ParticleManager* pManager);
    virtual CalcAheadFunc GetCalcAheadFunc(ParticleManager* pManager);
};

}
}

#endif
