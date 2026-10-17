#include "revolution/mtx.h"

asm void PSVECAdd
(
    const register Vec *vec1,
    const register Vec *vec2,
          register Vec *dst
)
{
    nofralloc

    psq_l       fp2, 0(vec1), 0, 0
    psq_l       fp4, 0(vec2), 0, 0
    ps_add      fp6, fp2, fp4
    psq_st      fp6, 0(dst), 0, 0
    psq_l       fp3, 8(vec1), 1, 0
    psq_l       fp5, 8(vec2), 1, 0
    ps_add      fp7, fp3, fp5
    psq_st      fp7, 8(dst), 1, 0

    blr
}

void PSVECNormalize
(
    const register Vec *vec1,
          register Vec *dst
)
{
    register f32 c_half  = 0.5f;
    register f32 c_three = 3.0f;
    register f32 v1_xy, v1_z;
    register f32 xx_zz, xx_yy;
    register f32 sqsum;
    register f32 rsqrt;
    register f32 nwork0, nwork1;

    asm
    {
        psq_l       v1_xy, 0(vec1), 0, 0
        ps_mul      xx_yy, v1_xy, v1_xy
        psq_l       v1_z, 8(vec1), 1, 0
        ps_madd     xx_zz, v1_z, v1_z, xx_yy
        ps_sum0     sqsum, xx_zz, v1_z, xx_yy
        frsqrte     rsqrt, sqsum
        fmuls       nwork0, rsqrt, rsqrt
        fmuls       nwork1, rsqrt, c_half
        fnmsubs     nwork0, nwork0, sqsum, c_three
        fmuls       rsqrt, nwork0, nwork1
        ps_muls0    v1_xy, v1_xy, rsqrt
        psq_st      v1_xy, 0(dst), 0, 0
        ps_muls0    v1_z, v1_z, rsqrt
        psq_st      v1_z, 8(dst), 1, 0
    }
}

f32 PSVECMag(const register Vec *v)
{
    register f32 vxy, vzz, sqmag;
    register f32 rmag, nwork0, nwork1;
    register f32 c_three, c_half, c_zero;

    c_half = 0.5f;

    asm
    {
        psq_l       vxy, 0(v), 0, 0
        ps_mul      vxy, vxy, vxy
        lfs         vzz, 8(v)
        fsubs       c_zero, c_half, c_half
        ps_madd     sqmag, vzz, vzz, vxy
        ps_sum0     sqmag, sqmag, vxy, vxy
        fcmpu       cr0, sqmag, c_zero
        beq-        __exit
        frsqrte     rmag, sqmag
    }

    c_three = 3.0f;

    asm
    {
        fmuls       nwork0, rmag, rmag
        fmuls       nwork1, rmag, c_half
        fnmsubs     nwork0, nwork0, sqmag, c_three
        fmuls       rmag, nwork0, nwork1
        fmuls       sqmag, sqmag, rmag
    __exit:
    }

    return sqmag;
}

asm f32 PSVECDotProduct(const register Vec *vec1, const register Vec *vec2)
{
    nofralloc

    psq_l       fp2, 4(vec1), 0, 0
    psq_l       fp3, 4(vec2), 0, 0
    ps_mul      fp2, fp2, fp3
    psq_l       fp5, 0(vec1), 0, 0
    psq_l       fp4, 0(vec2), 0, 0
    ps_madd     fp3, fp5, fp4, fp2
    ps_sum0     fp1, fp3, fp2, fp2

    blr
}

asm void PSVECCrossProduct
(
    const register Vec *vec1,
    const register Vec *vec2,
          register Vec *dst
)
{
    nofralloc

    psq_l       fp1, 0(vec2), 0, 0
    lfs         fp2, 8(vec1)
    psq_l       fp0, 0(vec1), 0, 0
    ps_merge10  fp6, fp1, fp1
    lfs         fp3, 8(vec2)
    ps_mul      fp4, fp1, fp2
    ps_muls0    fp7, fp1, fp0
    ps_msub     fp5, fp0, fp3, fp4
    ps_msub     fp8, fp0, fp6, fp7
    ps_merge11  fp9, fp5, fp5
    ps_merge01  fp10, fp5, fp8
    psq_st      fp9, 0(dst), 1, 0
    ps_neg      fp10, fp10
    psq_st      fp10, 4(dst), 0, 0

    blr
}

void C_VECHalfAngle(const Vec *a, const Vec *b, Vec *half)
{
    Vec aTmp;
    Vec bTmp;
    Vec hTmp;

    aTmp.x = -a->x;
    aTmp.y = -a->y;
    aTmp.z = -a->z;
    bTmp.x = -b->x;
    bTmp.y = -b->y;
    bTmp.z = -b->z;

    PSVECNormalize(&aTmp, &aTmp);
    PSVECNormalize(&bTmp, &bTmp);
    PSVECAdd(&aTmp, &bTmp, &hTmp);

    if (PSVECDotProduct(&hTmp, &hTmp) > 0.0f) {
        PSVECNormalize(&hTmp, half);
    } else {
        *half = hTmp;
    }
}

asm f32 PSVECSquareDistance(const register Vec *a, const register Vec *b)
{
    nofralloc

    psq_l       fp0, 4(a), 0, 0
    psq_l       fp1, 4(b), 0, 0
    psq_l       fp2, 0(a), 0, 0
    ps_sub      fp3, fp0, fp1
    psq_l       fp0, 0(b), 0, 0
    ps_sub      fp0, fp2, fp0
    ps_mul      fp3, fp3, fp3
    ps_madd     fp1, fp0, fp0, fp3
    ps_sum0     fp1, fp1, fp3, fp3

    blr
}
