#ifndef NW4R_EF_UTIL_H
#define NW4R_EF_UTIL_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_list.h>

namespace nw4r {
namespace ef {

u16 UtlistToArray(const ut::List* pList, void** ppArray, int maxlen);

math::VEC3& Rotation2VecY(const math::VEC3& rRot, math::VEC3* pVec);
void GetDirMtxY(math::MTX34* pMtx, const math::VEC3& rVec);

void MtxGetRotationMtx(const math::MTX34& rMtx, math::MTX34* pDst);

void MtxSetColVec(math::MTX34* pMtx, int col, const math::VEC3* pVec);
math::VEC3* MtxColVec(const math::MTX34* pMtx, int col, math::VEC3* pVec);

void MtxGetRotation(const math::MTX34& rMtx, math::VEC3* pRot);
void MtxGetTranslate(const math::MTX34& rMtx, math::VEC3* pTrans);
void MtxGetScale(const math::MTX34& rMtx, math::VEC3* pScale);

bool Normalize(math::VEC3* pOut, const math::VEC3* pIn);

void _PSSinCosRad(f32* pSinCos, f32 rad);
void _PSSinCosRad3(f32* pSinCos, f32 x, f32 y, f32 z);
void PSSinCosRad(f32* pSin, f32* pCos, f32 rad);
void PSMTX34RotXYZRad(math::MTX34* pMtx, f32 x, f32 y, f32 z);

math::MTX34* MTX34ScaleNonZero(math::MTX34* pOut, const math::MTX34* pIn,
                               const math::VEC3* pScale);
math::MTX34* MTX34ScaleNonZero(math::MTX34* pOut, const math::VEC3* pScale,
                               const math::MTX34* pIn);
math::MTX34* MTX34ScaleInv(math::MTX34* pOut, const math::MTX34* pIn,
                           const math::VEC3* pScale);
f32 MTXColLen(const math::MTX34* pMtx, int col);

}
}

#endif
