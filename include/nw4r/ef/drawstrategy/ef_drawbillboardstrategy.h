#ifndef NW4R_EF_DRAW_STRATEGY_DRAWBILLBOARDSTRATEGY_H
#define NW4R_EF_DRAW_STRATEGY_DRAWBILLBOARDSTRATEGY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r {
namespace ef {

class DrawBillboardStrategy : public DrawStrategyImpl {
public:
    DrawBillboardStrategy();

    virtual void Draw(const DrawInfo& rInfo, ParticleManager* pManager);
    virtual CalcAheadFunc GetCalcAheadFunc(ParticleManager* pManager);

private:
    void InitGraphics(const DrawInfo& rInfo, ParticleManager* pManager);
    void DrawNormalBillboard(const DrawInfo& rInfo, ParticleManager* pManager);
    void DrawYBillboard(const DrawInfo& rInfo, ParticleManager* pManager);
    void DrawDirectionalBillboard(const DrawInfo& rInfo,
                                  ParticleManager* pManager);
};

}
}

#endif
