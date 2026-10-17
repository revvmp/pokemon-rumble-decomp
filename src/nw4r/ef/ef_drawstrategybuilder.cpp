#include <nw4r/ef/drawstrategy/ef_drawbillboardstrategy.h>
#include <nw4r/ef/drawstrategy/ef_drawdirectionalstrategy.h>
#include <nw4r/ef/drawstrategy/ef_drawfreestrategy.h>
#include <nw4r/ef/drawstrategy/ef_drawlinestrategy.h>
#include <nw4r/ef/drawstrategy/ef_drawpointstrategy.h>
#include <nw4r/ef/drawstrategy/ef_drawsmoothstripestrategy.h>
#include <nw4r/ef/drawstrategy/ef_drawstrategybuilder.h>
#include <nw4r/ef/drawstrategy/ef_drawstripestrategy.h>

namespace nw4r {
namespace ef {

DrawStrategy* DrawStrategyBuilder::Create(u32 type) {
    static DrawBillboardStrategy billboardStrategy;
    static DrawDirectionalStrategy directionalStrategy;
    static DrawFreeStrategy freeStrategy;
    static DrawLineStrategy lineStrategy;
    static DrawPointStrategy pointStrategy;
    static DrawStripeStrategy stripeStrategy;
    static DrawSmoothStripeStrategy smoothStripeStrategy;

    switch (type) {
    case DRAWSTRATEGY_BILLBOARD: {
        return &billboardStrategy;
    }

    case DRAWSTRATEGY_DIRECTIONAL: {
        return &directionalStrategy;
    }

    case DRAWSTRATEGY_FREE: {
        return &freeStrategy;
    }

    case DRAWSTRATEGY_LINE: {
        return &lineStrategy;
    }

    case DRAWSTRATEGY_POINT: {
        return &pointStrategy;
    }

    case DRAWSTRATEGY_STRIPE: {
        return &stripeStrategy;
    }

    case DRAWSTRATEGY_SMOOTHSTRIPE: {
        return &smoothStripeStrategy;
    }

    default: {
        return &billboardStrategy;
    }
    }
}

}
}
