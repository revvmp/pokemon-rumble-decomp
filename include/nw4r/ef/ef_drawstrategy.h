#ifndef NW4R_EF_DRAW_STRATEGY_H
#define NW4R_EF_DRAW_STRATEGY_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace ef {

class DrawInfo;
class ParticleManager;

class DrawStrategy {
public:
    DrawStrategy() {}
    virtual ~DrawStrategy() {}

    virtual void Draw(const DrawInfo& rInfo, ParticleManager* pManager) = 0;
};

}
}

#endif
