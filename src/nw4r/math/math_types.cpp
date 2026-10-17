#include <nw4r/math/math_types.h>

#define FSEL_MAX(_fx, _fy)                                                     \
    fx = (_fx);                                                                \
    fy = (_fy);                                                                \
    dt = fx - fy;                                                              \
    asm ( fsel work, dt, fx, fy )

#define FSEL_MIN(_fx, _fy)                                                     \
    fx = (_fx);                                                                \
    fy = (_fy);                                                                \
    dt = fy - fx;                                                              \
    asm ( fsel work, dt, fx, fy )

namespace nw4r {
namespace math {

VEC3* VEC3Maximize(VEC3* pOut, const VEC3* pA, const VEC3* pB) {
    register f32 fx, fy;
    register f32 dt, work;

    FSEL_MAX(pA->x, pB->x);
    pOut->x = work;

    FSEL_MAX(pA->y, pB->y);
    pOut->y = work;

    FSEL_MAX(pA->z, pB->z);
    pOut->z = work;

    return pOut;
}

VEC3* VEC3Minimize(VEC3* pOut, const VEC3* pA, const VEC3* pB) {
    register f32 fx, fy;
    register f32 dt, work;

    FSEL_MIN(pA->x, pB->x);
    pOut->x = work;

    FSEL_MIN(pA->y, pB->y);
    pOut->y = work;

    FSEL_MIN(pA->z, pB->z);
    pOut->z = work;

    return pOut;
}

MTX33* MTX33Identity(register MTX33* pMtx) {
    register f32 c_00 = 0.0f, c_10 = 1.0f;
    register f32 c_11;

    asm (
        ps_merge00 c_11, c_10, c_00

        psq_st c_00, MTX33._02(pMtx), 0, 0
        psq_st c_00, MTX33._20(pMtx), 0, 0
        psq_st c_11, MTX33._00(pMtx), 0, 0
        psq_st c_11, MTX33._11(pMtx), 0, 0

        stfs c_10,   MTX33._22(pMtx)
    )

    return pMtx;
}

MTX33* MTX34ToMTX33(register MTX33* pOut, register const MTX34* pIn) {
    register f32 row0a, row0b, row1a, row1b, row2a, row2b;

    asm (
        psq_l row0a, MTX34._00(pIn), 0, 0
        psq_l row0b, MTX34._02(pIn), 0, 0
        psq_l row1a, MTX34._10(pIn), 0, 0
        psq_l row1b, MTX34._12(pIn), 0, 0
        psq_l row2a, MTX34._20(pIn), 0, 0
        psq_l row2b, MTX34._22(pIn), 0, 0

        psq_st row0a, MTX33._00(pOut), 0, 0
        psq_st row0b, MTX33._02(pOut), 1, 0
        psq_st row1a, MTX33._10(pOut), 0, 0
        psq_st row1b, MTX33._12(pOut), 1, 0
        psq_st row2a, MTX33._20(pOut), 0, 0
        psq_st row2b, MTX33._22(pOut), 1, 0
    )

    return pOut;
}

asm u32 MTX34InvTranspose(register MTX33* pOut, register const MTX34* pIn) {
    nofralloc

    psq_l      f0, MTX34._00(pIn), 1, 0
    psq_l      f1, MTX34._01(pIn), 0, 0
    psq_l      f2, MTX34._10(pIn), 1, 0
    ps_merge10 f6, f1, f0
    psq_l      f3, MTX34._11(pIn), 0, 0
    psq_l      f4, MTX34._20(pIn), 1, 0
    ps_merge10 f7, f3, f2
    psq_l      f5, MTX34._21(pIn), 0, 0

    ps_mul     f11, f3, f6
    ps_merge10 f8,  f5, f4
    ps_mul     f13, f5, f7
    ps_msub    f11, f1, f7, f11
    ps_mul     f12, f1, f8

    ps_msub f13, f3, f8, f13
    ps_msub f12, f5, f6, f12

    ps_mul  f10, f3, f4
    ps_mul  f9, f0, f5
    ps_mul  f8, f1, f2
    ps_msub f10, f2, f5, f10
    ps_msub f9, f1, f4, f9
    ps_msub f8, f0, f3, f8
    ps_mul  f7, f0, f13
    ps_sub  f1, f1, f1
    ps_madd f7, f2, f12, f7
    ps_madd f7, f4, f11, f7

    ps_cmpo0 cr0, f7, f1
    bne inverse_exists

    li r3, FALSE
    blr

inverse_exists:
    fres     f0, f7
    ps_add   f6, f0, f0
    ps_mul   f5, f0, f0
    ps_nmsub f0, f7, f5, f6
    ps_add   f6, f0, f0
    ps_mul   f5, f0, f0
    ps_nmsub f0, f7, f5, f6
    ps_muls0 f13, f13, f0
    ps_muls0 f12, f12, f0
    psq_st   f13, MTX33._00(pOut), 0, 0
    ps_muls0 f11, f11, f0
    psq_st   f12, MTX33._10(pOut), 0, 0
    ps_muls0 f10, f10, f0
    psq_st   f11, MTX33._20(pOut), 0, 0
    ps_muls0 f9, f9, f0
    psq_st   f10, MTX33._02(pOut), 1, 0
    ps_muls0 f8, f8, f0
    psq_st   f9, MTX33._12(pOut),  1, 0
    psq_st   f8, MTX33._22(pOut),  1, 0

    li r3, TRUE
    blr

}

MTX34* MTX34Zero(register MTX34* pMtx) {
    register f32 c_zero = 0.0f;

    asm (
        psq_st c_zero, MTX34._00(pMtx), 0, 0
        psq_st c_zero, MTX34._02(pMtx), 0, 0
        psq_st c_zero, MTX34._10(pMtx), 0, 0
        psq_st c_zero, MTX34._12(pMtx), 0, 0
        psq_st c_zero, MTX34._20(pMtx), 0, 0
        psq_st c_zero, MTX34._22(pMtx), 0, 0
    )

    return pMtx;
}

MTX34* MTX34Scale(register MTX34* pOut, register const MTX34* pIn,
                  register const VEC3* pScale) {
    register f32 xy, z;
    register f32 row0a, row0b;
    register f32 row1a, row1b;
    register f32 row2a, row2b;

    asm (
        psq_l xy, VEC3.x(pScale), 0, 0
        psq_l z,  VEC3.z(pScale), 1, 0

        psq_l row0a, MTX34._00(pIn), 0, 0
        psq_l row0b, MTX34._02(pIn), 0, 0
        psq_l row1a, MTX34._10(pIn), 0, 0
        psq_l row1b, MTX34._12(pIn), 0, 0
        psq_l row2a, MTX34._20(pIn), 0, 0
        psq_l row2b, MTX34._22(pIn), 0, 0

        ps_mul row0a, row0a, xy
        ps_mul row0b, row0b, z
        ps_mul row1a, row1a, xy
        ps_mul row1b, row1b, z
        ps_mul row2a, row2a, xy
        ps_mul row2b, row2b, z

        psq_st row0a, MTX34._00(pOut), 0, 0
        psq_st row0b, MTX34._02(pOut), 0, 0
        psq_st row1a, MTX34._10(pOut), 0, 0
        psq_st row1b, MTX34._12(pOut), 0, 0
        psq_st row2a, MTX34._20(pOut), 0, 0
        psq_st row2b, MTX34._22(pOut), 0, 0
    )

    return pOut;
}

MTX34* MTX34Trans(register MTX34* pOut, register const MTX34* pIn,
                  register const VEC3* pTrans) {
    register f32 xy, z;
    register f32 row0a, row0b;
    register f32 row1a, row1b;
    register f32 row2a, row2b;
    register f32 work0, work1, work2;

    asm (
        psq_l xy, VEC3.x(pTrans), 0, 0
        psq_l z,  VEC3.z(pTrans), 1, 0

        psq_l row0a, MTX34._00(pIn), 0, 0
        psq_l row0b, MTX34._02(pIn), 0, 0
        psq_l row1a, MTX34._10(pIn), 0, 0
        psq_l row1b, MTX34._12(pIn), 0, 0
        psq_l row2a, MTX34._20(pIn), 0, 0
        psq_l row2b, MTX34._22(pIn), 0, 0

        psq_st row0a, MTX34._00(pOut), 0, 0
        psq_st row0b, MTX34._02(pOut), 0, 0
        psq_st row1a, MTX34._10(pOut), 0, 0
        psq_st row1b, MTX34._12(pOut), 0, 0
        psq_st row2a, MTX34._20(pOut), 0, 0
        psq_st row2b, MTX34._22(pOut), 0, 0

        ps_mul  work0, row0a, xy
        ps_madd work1, row0b, z, work0
        ps_sum0 work2, work1, work2, work1
        psq_st  work2, MTX34._03(pOut), 1, 0

        ps_mul  work0, row1a, xy
        ps_madd work1, row1b, z, work0
        ps_sum0 work2, work1, work2, work1
        psq_st  work2, MTX34._13(pOut), 1, 0

        ps_mul  work0, row2a, xy
        ps_madd work1, row2b, z, work0
        ps_sum0 work2, work1, work2, work1
        psq_st  work2, MTX34._23(pOut), 1, 0
    )

    return pOut;
}

MTX34* MTX34RotAxisFIdx(MTX34* pMtx, const VEC3* pAxis, f32 fidx) {
    PSMTXRotAxisRad(*pMtx, *pAxis, NW4R_MATH_FIDX_TO_RAD(fidx));
    return pMtx;
}

using detail::gSinCosTbl;

static const f32 sFIdxRange[2] = {65536.0f, 65536.0f};

asm MTX34* MTX34RotXYZFIdx(register MTX34* pMtx, register f32 fx, register f32 fy, register f32 fz) {
    nofralloc
    ps_merge00 f6, fx, fy
    la r0, sFIdxRange(r2)
    psq_lx f0, r0, r0, 0, 0
    lis r4, gSinCosTbl@ha
    stwu r1, -0x10(r1)
    addi r4, r4, gSinCosTbl@l
    ps_merge00 f0, f0, f0
    addi r7, r1, 0x8
    ps_abs f4, f6
    ps_neg f1, f0
    ps_cmpu0 cr0, f4, f0
    ps_sub f2, f0, f0
    ble _x_done
    nop
_x_loop:
    ps_sum0 f4, f4, f4, f1
    ps_cmpu0 cr0, f4, f0
    bgt _x_loop
_x_done:
    ps_cmpu1 cr0, f4, f0
    ble _y_done
    ps_merge10 f4, f4, f4
_y_loop:
    ps_sum0 f4, f4, f4, f1
    ps_cmpu0 cr0, f4, f0
    bgt _y_loop
    ps_merge10 f4, f4, f4
_y_done:
    psq_st f4, 0(r7), 0, 3
    psq_l f7, 0(r7), 0, 3
    fabs f5, fz
    lwz r0, 8(r1)
    fcmpu cr0, f5, f0
    ble _z_done
_z_loop:
    fsubs f5, f5, f0
    fcmpu cr0, f5, f0
    bgt _z_loop
_z_done:
    psq_st f5, 0(r7), 1, 3
    rlwinm r5, r0, 20, 20, 27
    add r5, r4, r5
    ps_sub f7, f4, f7
    psq_l f4, 0(r5), 0, 0
    clrlslwi r6, r0, 24, 4
    psq_l f8, 8(r5), 0, 0
    ps_cmpu0 cr0, f6, f2
    add r6, r4, r6
    ps_madds0 f0, f8, f7, f4
    psq_l f4, 0(r6), 0, 0
    psq_l f8, 8(r6), 0, 0
    lhz r0, 8(r1)
    bge _x_pos
    ps_neg f9, f0
    ps_merge01 f0, f9, f0
_x_pos:
    ps_madds1 f1, f8, f7, f4
    psq_l f7, 0(r7), 1, 3
    clrlslwi r0, r0, 24, 4
    ps_cmpu1 cr0, f6, f2
    add r5, r4, r0
    fsubs f7, f5, f7
    psq_l f4, 0(r5), 0, 0
    psq_l f8, 8(r5), 0, 0
    bge _y_pos
    ps_neg f9, f1
    ps_merge01 f1, f9, f1
_y_pos:
    fcmpu cr0, fz, f2
    ps_madds0 f2, f8, f7, f4
    bge _z_pos
    ps_neg f9, f2
    ps_merge01 f2, f9, f2
_z_pos:
    ps_neg f3, f0
    ps_muls1 f5, f2, f1
    ps_sub f7, f0, f0
    ps_merge10 f3, f3, f0
    ps_merge10 f6, f5, f5
    psq_st f7, 0x2C(pMtx), 1, 0
    ps_muls0 f4, f0, f2
    psq_st f6, 0x0(pMtx), 1, 0
    ps_muls1 f6, f0, f2
    ps_muls0 f8, f3, f2
    ps_muls1 f2, f3, f2
    ps_madds0 f6, f6, f1, f8
    ps_neg f2, f2
    psq_st f6, 0x4(pMtx), 0, 0
    ps_merge00 f6, f7, f5
    psq_st f6, 0xC(pMtx), 0, 0
    ps_madds0 f6, f4, f1, f2
    psq_st f6, 0x14(pMtx), 0, 0
    ps_neg f6, f1
    ps_merge00 f6, f7, f6
    psq_st f6, 0x1C(pMtx), 0, 0
    ps_muls1 f6, f0, f1
    psq_st f6, 0x24(pMtx), 0, 0
    addi r1, r1, 0x10
    blr
}

VEC3* VEC3TransformNormal(VEC3* pOut, const MTX34* pMtx, const VEC3* pVec) {
    VEC3 tmp;
    tmp.x = pMtx->_00 * pVec->x + pMtx->_01 * pVec->y + pMtx->_02 * pVec->z;
    tmp.y = pMtx->_10 * pVec->x + pMtx->_11 * pVec->y + pMtx->_12 * pVec->z;
    tmp.z = pMtx->_20 * pVec->x + pMtx->_21 * pVec->y + pMtx->_22 * pVec->z;

    pOut->x = tmp.x;
    pOut->y = tmp.y;
    pOut->z = tmp.z;

    return pOut;
}

MTX44* MTX44Identity(register MTX44* pMtx) {
    register f32 c_zero = 0.0f, c_one = 1.0f;
    register f32 c_01, c_10;

    asm (
        ps_merge01 c_01, c_zero, c_one
        ps_merge10 c_10, c_one, c_zero

        psq_st c_zero, MTX44._02(pMtx), 0, 0
        psq_st c_zero, MTX44._12(pMtx), 0, 0
        psq_st c_zero, MTX44._20(pMtx), 0, 0
        psq_st c_01,   MTX44._10(pMtx), 0, 0
        psq_st c_10,   MTX44._00(pMtx), 0, 0
        psq_st c_10,   MTX44._22(pMtx), 0, 0
        psq_st c_zero, MTX44._30(pMtx), 0, 0
        psq_st c_01,   MTX44._32(pMtx), 0, 0
    )

    return pMtx;
}

MTX44* MTX44Copy(register MTX44* pDst, register const MTX44* pSrc) {
    register f32 row0a, row0b;
    register f32 row1a, row1b;
    register f32 row2a, row2b;
    register f32 row3a, row3b;

    asm (
        psq_l row0a, MTX44._00(pSrc), 0, 0
        psq_l row0b, MTX44._02(pSrc), 0, 0
        psq_l row1a, MTX44._10(pSrc), 0, 0
        psq_l row1b, MTX44._12(pSrc), 0, 0
        psq_l row2a, MTX44._20(pSrc), 0, 0
        psq_l row2b, MTX44._22(pSrc), 0, 0
        psq_l row3a, MTX44._30(pSrc), 0, 0
        psq_l row3b, MTX44._32(pSrc), 0, 0

        psq_st row0a, MTX44._00(pDst), 0, 0
        psq_st row0b, MTX44._02(pDst), 0, 0
        psq_st row1a, MTX44._10(pDst), 0, 0
        psq_st row1b, MTX44._12(pDst), 0, 0
        psq_st row2a, MTX44._20(pDst), 0, 0
        psq_st row2b, MTX44._22(pDst), 0, 0
        psq_st row3a, MTX44._30(pDst), 0, 0
        psq_st row3b, MTX44._32(pDst), 0, 0
    )

    return pDst;
}

VEC3* VEC3TransformCoord(VEC3* pOut, const MTX44* pMtx, const VEC3* pVec) {
    VEC3 tmp;
    tmp.x = pMtx->_00 * pVec->x + pMtx->_01 * pVec->y + pMtx->_02 * pVec->z + pMtx->_03;
    tmp.y = pMtx->_10 * pVec->x + pMtx->_11 * pVec->y + pMtx->_12 * pVec->z + pMtx->_13;
    tmp.z = pMtx->_20 * pVec->x + pMtx->_21 * pVec->y + pMtx->_22 * pVec->z + pMtx->_23;

    f32 w = pMtx->_30 * pVec->x + pMtx->_31 * pVec->y + pMtx->_32 * pVec->z + pMtx->_33;
    w = 1.0f / w;

    pOut->x = w * tmp.x;
    pOut->y = w * tmp.y;
    pOut->z = w * tmp.z;

    return pOut;
}

}
}
