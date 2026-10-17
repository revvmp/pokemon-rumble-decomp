#ifndef NW4R_G3D_TRANSFORM_H
#define NW4R_G3D_TRANSFORM_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {
namespace detail {

bool CalcViewNrmMtx(math::MTX33 *pOut, const math::MTX34 *pW);
bool CalcViewTexMtx(math::MTX34 *pOut, const math::MTX34 *pW);
bool CalcInvWorldMtx(math::MTX34 *pOut, const math::MTX34 *pW);

}
}
}

#endif
