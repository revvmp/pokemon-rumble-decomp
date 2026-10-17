#include <nw4r/ef/ef_util.h>
#include <nw4r/math/math_constant.h>
#include <math.h>

namespace nw4r {
namespace ef {

static inline f32 Atan2(f32 y, f32 x) {
    return atan2(y, x);
}


static f32 sRsqrtCoefficients[2] = {3.0f, 0.5f};
static const f32 sFIdxRange[2] = {65536.0f, 65536.0f};
static const f32 sRadToFIdx[1] = {40.7436638f};


u16 UtlistToArray(const ut::List* pList, void** ppArray, int maxlen) {
    u16 num = 0;

    NW4R_UT_LIST_FOREACH (void, it, *pList, {
        ppArray[num++] = it;

        if (num >= maxlen) {
            break;
        }
    });

    return num;
}

inline void MtxSetColVec(math::MTX34* pMtx, int col, const math::VEC3* pVec) {
    pMtx->m[0][col] = pVec->x;
    pMtx->m[1][col] = pVec->y;
    pMtx->m[2][col] = pVec->z;
}

inline math::VEC3* MtxColVec(const math::MTX34* pMtx, int col, math::VEC3* pVec) {
    pVec->x = pMtx->m[0][col];
    pVec->y = pMtx->m[1][col];
    pVec->z = pMtx->m[2][col];

    return pVec;
}

void GetDirMtxY(math::MTX34* pMtx, const math::VEC3& rVec) {
    f32 sx, cx;
    f32 sz, cz;

    sx = rVec.z;

    if (1.0f - math::FAbs(sx) < NW4R_MATH_FLT_EPSILON) {
        cx = 0.0f;
        sz = 0.0f;
        cz = 1.0f;
    } else {
        cx = 1.0f - sx * sx;
        cx = math::FSqrt(cx);
        cz = rVec.y / cx;
        sz = rVec.x / -cx;
    }

    pMtx->_00 = cz;
    pMtx->_01 = rVec.x;
    pMtx->_02 = sx * sz;
    pMtx->_03 = 0.0f;

    pMtx->_10 = sz;
    pMtx->_11 = rVec.y;
    pMtx->_12 = -cz * sx;
    pMtx->_13 = 0.0f;

    pMtx->_20 = 0.0f;
    pMtx->_21 = rVec.z;
    pMtx->_22 = cx;
    pMtx->_23 = 0.0f;
}

void MtxGetRotationMtx(const math::MTX34& rMtx, math::MTX34* pDst) {
    math::VEC3 x, y, z;

    MtxColVec(&rMtx, 0, &x);
    if (!Normalize(&x, &x)) {
        x.x = 1.0f;
    }

    MtxColVec(&rMtx, 1, &y);
    if (!Normalize(&y, &y)) {
        y.y = 1.0f;
    }

    math::VEC3Cross(&z, &x, &y);
    math::VEC3Cross(&y, &z, &x);

    MtxSetColVec(pDst, 0, &x);
    MtxSetColVec(pDst, 1, &y);
    MtxSetColVec(pDst, 2, &z);

    pDst->_03 = 0.0f;
    pDst->_13 = 0.0f;
    pDst->_23 = 0.0f;
}

void MtxGetRotation(const math::MTX34& rMtx, math::VEC3* pRot) {
    f32 C;
    f32 sx, sy, sz;
    f32 f;

    if (!((sx = MTXColLen(&rMtx, 0)) < NW4R_MATH_FLT_MIN) &&
        !((sy = MTXColLen(&rMtx, 1)) < NW4R_MATH_FLT_MIN) &&
        !((sz = MTXColLen(&rMtx, 2)) < NW4R_MATH_FLT_MIN)) {

        f = -rMtx._20 / sx;

        if (f > 1.0f) {
            f = 1.0f;
        }

        if (f < -1.0f) {
            f = -1.0f;
        }

        pRot->y = static_cast<f32>(asin(f));
        C = math::CosFIdx(sRadToFIdx[0] * pRot->y);

        if (C >= NW4R_MATH_FLT_MIN) {
            pRot->x = Atan2(rMtx._21 / sy, rMtx._22 / sz);
            pRot->z = Atan2(rMtx._10, rMtx._00);
        } else {
            pRot->x = Atan2(rMtx._01, rMtx._11);
            pRot->z = 0.0f;
        }
    } else {
        pRot->x = 0.0f;
        pRot->y = 0.0f;
        pRot->z = 0.0f;
    }
}

void MtxGetTranslate(const math::MTX34& rMtx, math::VEC3* pTrans) {
    pTrans->x = rMtx._03;
    pTrans->y = rMtx._13;
    pTrans->z = rMtx._23;
}

void MtxGetScale(const math::MTX34& rMtx, math::VEC3* pScale) {
    f32 sh[3];
    math::VEC3 v0, v1, v2, v;
    f32 mag;

    MtxColVec(&rMtx, 0, &v0);
    mag = math::VEC3LenSq(&v0);

    if (mag > NW4R_MATH_FLT_EPSILON) {
        mag = math::FrSqrt(mag);
        pScale->x = math::FInv(mag);

        math::VEC3Scale(&v0, &v0, mag);
        MtxColVec(&rMtx, 1, &v1);

        sh[0] = math::VEC3Dot(&v0, &v1);
        math::VEC3Scale(&v, &v0, sh[0]);
        math::VEC3Sub(&v1, &v1, &v);

        mag = math::VEC3LenSq(&v1);

        if (mag > NW4R_MATH_FLT_EPSILON) {
            mag = math::FrSqrt(mag);
            pScale->y = math::FInv(mag);

            sh[0] *= mag;
            math::VEC3Scale(&v1, &v1, mag);
            MtxColVec(&rMtx, 2, &v2);

            sh[2] = math::VEC3Dot(&v1, &v2);
            math::VEC3Scale(&v, &v1, sh[2]);
            math::VEC3Sub(&v2, &v2, &v);

            sh[1] = math::VEC3Dot(&v0, &v2);
            math::VEC3Scale(&v, &v0, sh[1]);
            math::VEC3Sub(&v2, &v2, &v);

            mag = math::VEC3LenSq(&v2);

            if (mag > NW4R_MATH_FLT_EPSILON) {
                pScale->z = math::FSqrt(mag);
                math::VEC3Cross(&v, &v1, &v2);

                if (math::VEC3Dot(&v0, &v) < 0.0f) {
                    
                    pScale->x *= -1.0;
                    pScale->y *= -1.0;
                    pScale->z *= -1.0;
                }
            } else {
                pScale->z = 0.0f;
            }
        } else {
            pScale->y = 0.0f;
            MtxColVec(&rMtx, 2, &v2);

            sh[1] = math::VEC3Dot(&v0, &v2);
            math::VEC3Scale(&v, &v0, sh[1]);
            math::VEC3Sub(&v2, &v2, &v);

            mag = math::VEC3LenSq(&v2);

            if (mag > NW4R_MATH_FLT_EPSILON) {
                pScale->z = math::FSqrt(mag);
            } else {
                pScale->z = 0.0f;
            }
        }

    } else {
        pScale->x = 0.0f;

        MtxColVec(&rMtx, 1, &v1);
        mag = math::VEC3LenSq(&v1);

        if (mag > NW4R_MATH_FLT_EPSILON) {
            mag = math::FrSqrt(mag);
            pScale->y = math::FInv(mag);

            math::VEC3Scale(&v1, &v1, mag);
            MtxColVec(&rMtx, 2, &v2);

            sh[2] = math::VEC3Dot(&v1, &v2);
            math::VEC3Scale(&v, &v1, sh[2]);
            math::VEC3Sub(&v2, &v2, &v);
            pScale->z = math::VEC3Len(&v2);
        } else {
            pScale->y = 0.0f;

            MtxColVec(&rMtx, 2, &v2);
            pScale->z = math::VEC3Len(&v2);
        }
    }
}


static const f32 sHalf[1] = {0.5f};
static const f32 sThree[1] = {3.0f};
static const f32 sAngleRange[1] = {65536.0f};
static const f32 sMinScale[1] = {0.00001f};
extern const f32 sScalarMinScale[1];

using math::detail::gSinCosTbl;

asm bool Normalize(math::VEC3* pOut, const math::VEC3* pIn) {
    nofralloc
    psq_l f3, 0(r4), 0, 0
    lfs f0, sHalf(r2)
    ps_mul f6, f3, f3
    psq_l f4, 8(r4), 1, 0
    fsubs f2, f0, f0
    lfs f1, sThree(r2)
    ps_madd f5, f4, f4, f6
    ps_sum0 f5, f5, f4, f6
    fcmpu cr0, f5, f2
    beq zeroLength
    frsqrte f2, f5
    fmuls f6, f2, f2
    fmuls f0, f2, f0
    fnmsubs f6, f6, f5, f1
    fmuls f2, f6, f0
    ps_muls0 f3, f3, f2
    ps_muls0 f4, f4, f2
    psq_st f3, 0(r3), 0, 0
    psq_st f4, 8(r3), 1, 0
    li r3, 1
    blr
zeroLength:
    psq_st f3, 0(r3), 0, 0
    psq_st f4, 8(r3), 1, 0
    li r3, 0
    blr
}

asm void _PSSinCosRad(f32* pSinCos, f32 rad) {
    nofralloc
    lfs f0, sRadToFIdx(r2)
    lis r4, gSinCosTbl@ha
    lfs f2, sAngleRange(r2)
    addi r4, r4, gSinCosTbl@l
    fmuls f0, f1, f0
    fabs f1, f0
    psq_st f1, 0(r3), 1, 3
    fcmpu cr0, f1, f2
    ble lookup
    nop
reduce:
    fsubs f1, f1, f2
    fcmpu cr0, f1, f2
    bge reduce
    psq_st f1, 0(r3), 1, 3
lookup:
    lhz r0, 0(r3)
    fsubs f4, f2, f2
    rlwinm r0, r0, 4, 20, 27
    add r4, r4, r0
    psq_l f2, 0(r3), 1, 3
    fcmpu cr0, f0, f4
    psq_l f3, 0(r4), 0, 0
    fsubs f2, f1, f2
    psq_l f0, 8(r4), 0, 0
    ps_madds0 f1, f0, f2, f3
    bge store
    ps_neg f0, f1
    ps_merge01 f1, f0, f1
store:
    psq_st f1, 0(r3), 0, 0
    blr
}

asm void PSSinCosRad(f32* pSin, f32* pCos, f32 rad) {
    nofralloc
    lfs f0, sRadToFIdx(r2)
    lis r5, gSinCosTbl@ha
    lfs f2, sAngleRange(r2)
    addi r5, r5, gSinCosTbl@l
    fmuls f0, f1, f0
    fabs f1, f0
    psq_st f1, 0(r3), 1, 3
    fcmpu cr0, f1, f2
    ble lookup
    nop
reduce:
    fsubs f1, f1, f2
    fcmpu cr0, f1, f2
    bge reduce
    psq_st f1, 0(r3), 1, 3
lookup:
    lhz r0, 0(r3)
    fsubs f4, f2, f2
    rlwinm r0, r0, 4, 20, 27
    add r5, r5, r0
    psq_l f2, 0(r3), 1, 3
    fcmpu cr0, f0, f4
    psq_l f3, 0(r5), 0, 0
    fsubs f2, f1, f2
    psq_l f0, 8(r5), 0, 0
    ps_madds0 f0, f0, f2, f3
    ps_merge10 f2, f0, f0
    psq_st f2, 0(r4), 1, 0
    bge store
    ps_neg f0, f0
store:
    psq_st f0, 0(r3), 1, 0
    blr
}

asm math::VEC3& Rotation2VecY(const math::VEC3& rRot, math::VEC3* pVec) {
    nofralloc
    lfs f0, 0(r3)
    la r0, sFIdxRange(r2)
    lfs f1, 4(r3)
    lis r5, gSinCosTbl@ha
    psq_lx f2, r0, r0, 0, 0
    addi r5, r5, gSinCosTbl@l
    ps_merge00 f6, f0, f1
    lfs f3, sRadToFIdx(r2)
    ps_merge00 f2, f2, f2
    stwu r1, -16(r1)
    lfs f0, 8(r3)
    ps_muls0 f6, f6, f3
    ps_neg f1, f2
    addi r7, r1, 8
    ps_sub f4, f2, f2
    ps_abs f5, f6
    ps_cmpu0 cr0, f5, f2
    ble reduceY
reduceX:
    ps_sum0 f5, f5, f5, f1
    ps_cmpu0 cr0, f5, f2
    bge reduceX
reduceY:
    ps_cmpu1 cr0, f5, f2
    ble quantizeXY
    ps_merge10 f5, f5, f5
reduceYLoop:
    ps_sum0 f5, f5, f5, f1
    ps_cmpu0 cr0, f5, f2
    bge reduceYLoop
    ps_merge10 f5, f5, f5
quantizeXY:
    psq_st f5, 0(r7), 0, 3
    fmuls f7, f0, f3
    psq_l f8, 0(r7), 0, 3
    fabs f3, f7
    lwz r0, 8(r1)
    fcmpu cr0, f3, f2
    ble quantizeZ
    nop
reduceZ:
    fsubs f3, f3, f2
    fcmpu cr0, f3, f2
    bge reduceZ
quantizeZ:
    psq_st f3, 0(r7), 1, 3
    rlwinm r3, r0, 20, 20, 27
    add r3, r5, r3
    ps_sub f8, f5, f8
    psq_l f2, 0(r3), 0, 0
    rlwinm r6, r0, 4, 20, 27
    psq_l f5, 8(r3), 0, 0
    ps_cmpu0 cr0, f6, f4
    add r6, r5, r6
    ps_madds0 f0, f5, f8, f2
    psq_l f2, 0(r6), 0, 0
    psq_l f5, 8(r6), 0, 0
    lhz r0, 8(r1)
    bge signY
    ps_neg f9, f0
    ps_merge01 f0, f9, f0
signY:
    ps_madds1 f1, f5, f8, f2
    psq_l f8, 0(r7), 1, 3
    rlwinm r0, r0, 4, 20, 27
    ps_cmpu1 cr0, f6, f4
    add r3, r5, r0
    fsubs f8, f3, f8
    psq_l f2, 0(r3), 0, 0
    psq_l f5, 8(r3), 0, 0
    bge signZ
    ps_neg f9, f1
    ps_merge01 f1, f9, f1
signZ:
    fcmpu cr0, f7, f4
    ps_madds0 f2, f5, f8, f2
    bge build
    ps_neg f9, f2
    ps_merge01 f2, f9, f2
build:
    ps_muls1 f3, f2, f0
    mr r3, r4
    ps_muls0 f1, f1, f0
    ps_merge10 f2, f2, f2
    ps_neg f0, f3
    ps_merge01 f3, f0, f3
    ps_madds0 f2, f2, f1, f3
    ps_merge10 f1, f1, f1
    psq_st f2, 0(r4), 0, 0
    psq_st f1, 8(r4), 1, 0
    addi r1, r1, 16
    blr
}

asm void PSMTX34RotXYZRad(math::MTX34* pMtx, f32 x, f32 y, f32 z) {
    nofralloc
    ps_merge00 f6, f1, f2
    lfs f2, sRadToFIdx(r2)
    la r0, sFIdxRange(r2)
    stwu r1, -16(r1)
    psq_lx f0, r0, r0, 0, 0
    lis r4, gSinCosTbl@ha
    ps_muls0 f6, f6, f2
    addi r4, r4, gSinCosTbl@l
    ps_merge00 f0, f0, f0
    addi r7, r1, 8
    ps_abs f5, f6
    ps_neg f1, f0
    ps_sub f4, f0, f0
    ps_cmpu0 cr0, f5, f0
    ble reduceY
    nop
reduceX:
    ps_sum0 f5, f5, f5, f1
    ps_cmpu0 cr0, f5, f0
    bge reduceX
reduceY:
    ps_cmpu1 cr0, f5, f0
    ble quantizeXY
    ps_merge10 f5, f5, f5
reduceYLoop:
    ps_sum0 f5, f5, f5, f1
    ps_cmpu0 cr0, f5, f0
    bge reduceYLoop
    ps_merge10 f5, f5, f5
quantizeXY:
    psq_st f5, 0(r7), 0, 3
    fmuls f3, f3, f2
    psq_l f7, 0(r7), 0, 3
    fabs f2, f3
    lwz r0, 8(r1)
    fcmpu cr0, f2, f0
    ble quantizeZ
    nop
reduceZ:
    fsubs f2, f2, f0
    fcmpu cr0, f2, f0
    bge reduceZ
quantizeZ:
    psq_st f2, 0(r7), 1, 3
    rlwinm r5, r0, 20, 20, 27
    add r5, r4, r5
    ps_sub f7, f5, f7
    psq_l f5, 0(r5), 0, 0
    rlwinm r6, r0, 4, 20, 27
    psq_l f8, 8(r5), 0, 0
    ps_cmpu0 cr0, f6, f4
    add r6, r4, r6
    ps_madds0 f0, f8, f7, f5
    psq_l f5, 0(r6), 0, 0
    psq_l f8, 8(r6), 0, 0
    lhz r0, 8(r1)
    bge signY
    ps_neg f9, f0
    ps_merge01 f0, f9, f0
signY:
    ps_madds1 f1, f8, f7, f5
    psq_l f7, 0(r7), 1, 3
    rlwinm r0, r0, 4, 20, 27
    ps_cmpu1 cr0, f6, f4
    add r5, r4, r0
    fsubs f7, f2, f7
    psq_l f5, 0(r5), 0, 0
    psq_l f8, 8(r5), 0, 0
    bge signZ
    ps_neg f9, f1
    ps_merge01 f1, f9, f1
signZ:
    fcmpu cr0, f3, f4
    ps_madds0 f2, f8, f7, f5
    bge build
    ps_neg f9, f2
    ps_merge01 f2, f9, f2
build:
    ps_neg f3, f0
    ps_muls1 f5, f2, f1
    ps_sub f7, f0, f0
    ps_merge10 f3, f3, f0
    ps_merge10 f6, f5, f5
    psq_st f7, 44(r3), 1, 0
    ps_muls0 f4, f0, f2
    psq_st f6, 0(r3), 1, 0
    ps_muls1 f6, f0, f2
    ps_muls0 f8, f3, f2
    ps_muls1 f2, f3, f2
    ps_madds0 f6, f6, f1, f8
    ps_neg f2, f2
    psq_st f6, 4(r3), 0, 0
    ps_merge00 f6, f7, f5
    psq_st f6, 12(r3), 0, 0
    ps_madds0 f6, f4, f1, f2
    psq_st f6, 20(r3), 0, 0
    ps_neg f6, f1
    ps_merge00 f6, f7, f6
    psq_st f6, 28(r3), 0, 0
    ps_muls1 f6, f0, f1
    psq_st f6, 36(r3), 0, 0
    addi r1, r1, 16
    blr
}

asm math::MTX34* MTX34ScaleNonZero(math::MTX34* pOut, const math::MTX34* pIn, const math::VEC3* pScale) {
    nofralloc
    psq_l f0, 0(r5), 0, 0
    lfs f8, sMinScale(r2)
    ps_sub f6, f0, f0
    psq_l f1, 8(r5), 1, 0
    psq_l f2, 0(r4), 0, 0
    psq_l f3, 8(r4), 0, 0
    ps_cmpu0 cr0, f0, f6
    bne scaleY
    ps_merge01 f0, f8, f0
scaleY:
    ps_cmpu1 cr0, f0, f6
    psq_l f4, 16(r4), 0, 0
    psq_l f5, 24(r4), 0, 0
    bne scaleZ
    ps_merge00 f0, f0, f8
scaleZ:
    ps_cmpu0 cr0, f1, f6
    psq_l f6, 32(r4), 0, 0
    psq_l f7, 40(r4), 0, 0
    bne multiply
    ps_merge01 f1, f8, f1
multiply:
    ps_mul f2, f2, f0
    ps_mul f4, f4, f0
    ps_mul f6, f6, f0
    psq_st f2, 0(r3), 0, 0
    ps_mul f3, f3, f1
    ps_mul f5, f5, f1
    psq_st f4, 16(r3), 0, 0
    ps_mul f7, f7, f1
    psq_st f3, 8(r3), 0, 0
    psq_st f6, 32(r3), 0, 0
    psq_st f5, 24(r3), 0, 0
    psq_st f7, 40(r3), 0, 0
    blr
}

asm math::MTX34* MTX34ScaleNonZero(math::MTX34* pOut, const math::VEC3* pScale, const math::MTX34* pIn) {
    nofralloc
    lis r6, sScalarMinScale@ha
    lfs f9, 0(r4)
    lfs f12, sScalarMinScale@l(r6)
    lfs f10, 4(r4)
    fsubs f13, f12, f12
    lfs f11, 8(r4)
    psq_l f4, 0(r5), 0, 0
    psq_l f5, 8(r5), 0, 0
    fcmpu cr0, f9, f13
    bne scaleY
    fmr f9, f12
scaleY:
    fcmpu cr0, f10, f13
    psq_l f6, 16(r5), 0, 0
    psq_l f7, 24(r5), 0, 0
    bne scaleZ
    fmr f10, f12
scaleZ:
    fcmpu cr0, f11, f13
    psq_l f8, 32(r5), 0, 0
    psq_l f2, 40(r5), 0, 0
    bne multiply
    fmr f11, f12
multiply:
    ps_muls0 f4, f4, f9
    ps_muls0 f5, f5, f9
    ps_muls0 f6, f6, f10
    psq_st f4, 0(r3), 0, 0
    ps_muls0 f7, f7, f10
    ps_muls0 f8, f8, f11
    psq_st f5, 8(r3), 0, 0
    ps_muls0 f2, f2, f11
    psq_st f6, 16(r3), 0, 0
    psq_st f7, 24(r3), 0, 0
    psq_st f8, 32(r3), 0, 0
    psq_st f2, 40(r3), 0, 0
    blr
}

math::MTX34* MTX34ScaleInv(math::MTX34* pOut, const math::MTX34* pIn, const math::VEC3* pScale) {
    math::VEC3 inv;
    inv.x = pScale->x != 0.0f ? math::FInv(pScale->x) : 100000.0f;
    inv.y = pScale->y != 0.0f ? math::FInv(pScale->y) : 100000.0f;
    inv.z = pScale->z != 0.0f ? math::FInv(pScale->z) : 100000.0f;
    return math::MTX34Scale(pOut, pIn, &inv);
}

const f32 sScalarMinScale[1] = {0.00001f};

asm f32 MTXColLen(const math::MTX34* pMtx, int col) {
    nofralloc
    slwi r4, r4, 2
    add r4, r3, r4
    psq_l f0, 0(r4), 1, 0
    psq_l f1, 16(r4), 1, 0
    lfs f2, 32(r4)
    ps_merge00 f0, f0, f1
    ps_mul f0, f0, f0
    ps_madd f1, f2, f2, f0
    fsubs f2, f0, f0
    ps_sum0 f1, f1, f0, f0
    fcmpu cr0, f1, f2
    beqlr
    frsqrte f0, f1
    lis r6, sRsqrtCoefficients@ha
    addi r6, r6, sRsqrtCoefficients@l
    psq_l f3, 0(r6), 0, 0
    fmuls f2, f0, f0
    ps_muls1 f0, f0, f3
    fnmsubs f2, f2, f1, f3
    fmuls f0, f2, f0
    fmuls f1, f1, f0
    blr
}

}
}
