#ifndef NW4R_G3D_DCC_XSI_H
#define NW4R_G3D_DCC_XSI_H

#include <nw4r/g3d/g3d_dcc.h>

namespace nw4r {
namespace g3d {
namespace detail {
namespace dcc {

bool CalcTexMtx_Xsi(math::MTX34* pMtx, bool set, const TexSrt& rSrt, TexSrt::Flag flag);
u32 CalcWorldMtx_Xsi(math::MTX34* pWorld, math::VEC3* pScale, const math::MTX34* pParentWorld, const math::VEC3* pParentScale, u32 attr, const ChrAnmResult* pResult);

}
}
}
}

#endif
