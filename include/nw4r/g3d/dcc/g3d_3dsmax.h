#ifndef NW4R_G3D_DCC_3DSMAX_H
#define NW4R_G3D_DCC_3DSMAX_H

#include <nw4r/g3d/g3d_dcc.h>

namespace nw4r {
namespace g3d {
namespace detail {
namespace dcc {

bool CalcTexMtx_3dsmax(math::MTX34* pMtx, bool set, const TexSrt& rSrt, TexSrt::Flag flag);

}
}
}
}

#endif
