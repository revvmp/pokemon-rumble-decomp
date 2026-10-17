#ifndef NW4R_G3D_WORK_MEM_H
#define NW4R_G3D_WORK_MEM_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanmshp.h>

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {
namespace detail {
namespace workmem {

struct MdlZ {
    f32 Z;
    u16 priority;
    u16 nodeID;
    u16 matID;
    u16 shpID;
};

struct ShpAnmResultBuf {
    ShpAnmResult resultBuf;
    const ShpAnmResult *pResult;
    f32 weight;
};

static const int WORKMEM_SIZE = 0x18000;

static const int WORKMEM_NUMTMPSCALE = 2048;
static const int WORKMEM_NUMMTXID = 2048;
static const int WORKMEM_NUMBYTECODE = WORKMEM_SIZE;
static const int WORKMEM_NUMMDLZ = 2048;
static const int WORKMEM_NUMSKINNINGMTX = 2048;
static const int WORKMEM_NUMBBMTX = 2048;
static const int WORKMEM_NUMSHPANMRESULT = WORKMEM_SIZE / sizeof(ShpAnmResultBuf);

math::VEC3 *GetScaleTemporary();
u32 *GetMtxIDTemporary();
MdlZ *GetMdlZTemporary();
math::MTX34 *GetSkinningMtxTemporary();
math::MTX34 *GetBillboardMtxTemporary();
ShpAnmResultBuf *GetShpAnmResultBufTemporary();

}
}
}
}

#endif
