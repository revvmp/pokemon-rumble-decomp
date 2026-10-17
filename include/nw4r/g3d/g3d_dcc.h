#ifndef NW4R_G3D_DCC_H
#define NW4R_G3D_DCC_H
#include <nw4r/types_nw4r.h>

#include "nw4r/g3d/res/g3d_resanmtexsrt.h"
#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {

void CalcTexMtx(math::MTX34 *pMtx, bool set, const TexSrt &rSrt, TexSrt::Flag flag);
void CalcTexMtx(math::MTX34 *pMtx, bool set, const TexSrt &rSrt, TexSrt::Flag flag, TexSrt::TexMatrixMode mode);

namespace detail {

class WorldMtxAttr {
public:
    enum Attr {
        ATTR_S_UNIFORM = (1 << 28),
        ATTR_ALL_S_UNIFORM = (1 << 29),
        ATTR_S_ONE = (1 << 30),
        ATTR_ALL_S_ONE = (1 << 31)
    };

    static u32 AnmScaleUniform(u32 attr) {
        return attr | ATTR_S_UNIFORM;
    }

    static u32 AnmNotScaleUniform(u32 attr) {
        return attr & ~(ATTR_S_UNIFORM | ATTR_ALL_S_UNIFORM | ATTR_S_ONE | ATTR_ALL_S_ONE);
    }

    static u32 AnmScaleOne(u32 attr) {
        return attr | ATTR_S_ONE;
    }

    static u32 AnmNotScaleOne(u32 attr) {
        return attr & ~(ATTR_S_ONE | ATTR_ALL_S_ONE);
    }

    static bool IsAllScaleOne(u32 attr) {
        return (attr & ATTR_ALL_S_ONE) ? true : false;
    }
};

}

}
}

#endif
