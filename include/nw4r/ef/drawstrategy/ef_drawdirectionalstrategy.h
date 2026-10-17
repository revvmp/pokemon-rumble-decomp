#ifndef NW4R_EF_DRAW_STRATEGY_DRAWDIRECTIONALSTRATEGY_H
#define NW4R_EF_DRAW_STRATEGY_DRAWDIRECTIONALSTRATEGY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r {
namespace ef {

class DrawDirectionalStrategy : public DrawStrategyImpl {
public:
    DrawDirectionalStrategy();

    virtual void Draw(const DrawInfo& rInfo, ParticleManager* pManager);
    virtual CalcAheadFunc GetCalcAheadFunc(ParticleManager* pManager);

private:
    void DrawDirectional(const DrawInfo& rInfo, ParticleManager* pManager);
};

}
}

#endif
