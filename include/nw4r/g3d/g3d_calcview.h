#ifndef NW4R_G3D_CALC_VIEW_H
#define NW4R_G3D_CALC_VIEW_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resmdl.h>

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {
namespace detail {

struct MtxCacheMap {
    u8 currDmaArray[32 * sizeof(math::MTX44)];
    u8 currViewArray[32 * sizeof(math::MTX44)];
    u8 currNrmArray[32 * sizeof(math::MTX44)];
    u8 currTexArray[32 * sizeof(math::MTX44)];

    u8 prevDmaArray[32 * sizeof(math::MTX44)];
    u8 prevViewArray[32 * sizeof(math::MTX44)];
    u8 prevNrmArray[32 * sizeof(math::MTX44)];
    u8 prevTexArray[32 * sizeof(math::MTX44)];
};

}

void CalcView(
    math::MTX34 *pViewPosArray, math::MTX33 *pViewNrmArray, const math::MTX34 *pModelMtxArray,
    const u32 *pModelMtxAttribArray, u32 numMtx, const math::MTX34 *pViewMtx, const ResMdl mdl,
    math::MTX34 *pViewTexMtxArray
);

void CalcView_LC(
    math::MTX34 *pViewPosArray, math::MTX33 *pViewNrmArray, const math::MTX34 *pModelMtxArray,
    const u32 *pModelMtxAttribArray, u32 numMtx, const math::MTX34 *pViewMtx, const ResMdl mdl,
    math::MTX34 *pViewTexMtxArray
);

void CalcView_LC_DMA_ModelMtx(
    math::MTX34 *pViewPosArray, math::MTX33 *pViewNrmArray, const math::MTX34 *pModelMtxArray,
    const u32 *pModelMtxAttribArray, u32 numMtx, const math::MTX34 *pViewMtx, const ResMdl mdl,
    math::MTX34 *pViewTexMtxArray
);

}
}

#endif
