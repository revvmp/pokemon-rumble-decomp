#include <nw4r/g3d/g3d_workmem.h>
namespace nw4r {
namespace g3d {
namespace detail {
namespace workmem {
namespace {

union {
    u8 mem[WORKMEM_SIZE];
    struct {
        math::_VEC3 tmpScale[WORKMEM_NUMTMPSCALE];
        u32 mtxID[WORKMEM_NUMMTXID];
    };
    u8 byteCode[WORKMEM_NUMBYTECODE];
    MdlZ mdlZ[WORKMEM_NUMMDLZ];
    math::_MTX34 skinningMtx[WORKMEM_NUMSKINNINGMTX];
    math::_MTX34 bbMtx[WORKMEM_NUMBBMTX];
    u8 shpAnmResultBuf[WORKMEM_NUMSHPANMRESULT];
} sTemp __attribute__((aligned(128)));

}

math::VEC3 *GetScaleTemporary() {
    return static_cast<math::VEC3 *>(sTemp.tmpScale);
}

u32 *GetMtxIDTemporary() {
    return sTemp.mtxID;
}

MdlZ *GetMdlZTemporary() {
    return sTemp.mdlZ;
}

math::MTX34 *GetSkinningMtxTemporary() {
    return static_cast<math::MTX34 *>(sTemp.skinningMtx);
}

math::MTX34 *GetBillboardMtxTemporary() {
    return static_cast<math::MTX34 *>(sTemp.bbMtx);
}

ShpAnmResultBuf *GetShpAnmResultBufTemporary() {
    return reinterpret_cast<ShpAnmResultBuf *>(sTemp.shpAnmResultBuf);
}

}
}
}
}
