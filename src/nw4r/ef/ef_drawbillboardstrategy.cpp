#include <nw4r/ef/drawstrategy/ef_drawbillboardstrategy.h>
#include <nw4r/ef/ef_drawinfo.h>
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/ef/ef_resource.h>
#include <nw4r/ef/ef_util.h>
#include <nw4r/math/math_arithmetic.h>
#include <revolution/GX.h>
#include <revolution/gx/GXVert.h>

namespace nw4r {
namespace ef {

static inline void GXPosition(const math::VEC3& rPos) {
    GXPosition3f32(rPos.x, rPos.y, rPos.z);
}

static inline void DispPolygon(const math::VEC3& rP, const math::VEC3& rD0,
                               const math::VEC3& rD1, bool texCoord) {
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GXPosition(rP - rD0);
        if (texCoord) {
            GXTexCoord1x8(0);
        }

        GXPosition(rP - rD1);
        if (texCoord) {
            GXTexCoord1x8(1);
        }

        GXPosition(rP + rD0);
        if (texCoord) {
            GXTexCoord1x8(2);
        }

        GXPosition(rP + rD1);
        if (texCoord) {
            GXTexCoord1x8(3);
        }
    }
    GXEnd();
}

static inline void DispPolygon(const math::VEC3* pVtx, bool texCoord) {
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GXPosition(pVtx[0]);
        if (texCoord) {
            GXTexCoord1x8(0);
        }

        GXPosition(pVtx[1]);
        if (texCoord) {
            GXTexCoord1x8(1);
        }

        GXPosition(pVtx[2]);
        if (texCoord) {
            GXTexCoord1x8(2);
        }

        GXPosition(pVtx[3]);
        if (texCoord) {
            GXTexCoord1x8(3);
        }
    }
    GXEnd();
}

static inline void CalcNormalPolygon(register math::VEC3* pVtx,
                                     register const Particle* pParticle,
                                     register const math::MTX34* pMtx,
                                     register const math::VEC2* pPivot,
                                     register f32 vx, register f32 vy,
                                     register f32 rc, register f32 rs,
                                     register f32 sx, register f32 sy) {
    register f32 work0, work1, work2, work3, work4, work5, work6, work7;

    asm {
        psq_l      f0, Particle.mParameter.mPosition.x(pParticle), 0, 0
        psq_l      f2, 0x00(pMtx), 0, 0
        psq_l      f1, Particle.mParameter.mPosition.z(pParticle), 1, 0
        ps_mul     f4, f2, f0
        psq_l      f3, 0x08(pMtx), 0, 0
        ps_madd    f5, f3, f1, f4
        psq_l      f8, 0x10(pMtx), 0, 0
        ps_sum0    f6, f5, f6, f5
        psq_l      f9, 0x18(pMtx), 0, 0
        ps_mul     f10, f8, f0
        ps_madd    f11, f9, f1, f10
        psq_l      f2, 0x20(pMtx), 0, 0
        ps_sum0    f12, f11, f12, f11
        psq_l      f3, 0x28(pMtx), 0, 0
        ps_mul     f4, f2, f0
        ps_merge00 work2, f6, f12
        ps_madd    f5, f3, f1, f4

        ps_merge00 work1, rs, rc
        ps_merge00 work0, sx, sy
        ps_muls0   work5, work1, vx
        ps_merge10 work5, work5, work5
        ps_muls0   work4, work1, vy
        ps_neg     work3, work4
        ps_merge01 work4, work4, work3

        psq_l      work1, 0(pPivot), 0, 0
        ps_nmsub   work6, work0, work1, work1
        ps_madds0  work3, work5, work6, work2
        ps_madds1  work7, work4, work6, work3

        ps_muls0   work1, work5, work0
        ps_muls1   work0, work4, work0
        ps_add     work2, work1, work0
        ps_neg     work0, work0
        ps_add     work1, work1, work0

        ps_sum0    work0, f5, f6, f5
        psq_st     work0, 0x08(pVtx), 1, 0
        psq_st     work0, 0x14(pVtx), 1, 0
        psq_st     work0, 0x20(pVtx), 1, 0
        psq_st     work0, 0x2C(pVtx), 1, 0

        ps_sub     work3, work7, work1
        psq_st     work3, 0x00(pVtx), 0, 0
        ps_sub     work3, work7, work2
        psq_st     work3, 0x0C(pVtx), 0, 0
        ps_add     work3, work7, work1
        psq_st     work3, 0x18(pVtx), 0, 0
        ps_add     work3, work7, work2
        psq_st     work3, 0x24(pVtx), 0, 0
    }
}

static inline void DispParticle_Normal(Particle* pParticle,
                                       const math::MTX34& rMtx, f32 vx,
                                       f32 vy, f32 rc, f32 rs, f32 sx, f32 sy,
                                       const math::VEC2& rPivot,
                                       bool texCoord) {
    math::VEC3 vtx[4];
    math::VEC3* pVtx = vtx;

    math::VEC3 rot;
    pParticle->Draw_GetRotate(&rot);

    if (rot.z != 0.0f) {
        math::VEC3 pos;
        math::VEC3 p0;
        math::VEC3 d0;
        math::VEC3 d1;

        math::VEC3Transform(&pos, &rMtx, &pParticle->mParameter.mPosition);

        f32 px = rPivot.x;
        f32 py = rPivot.y;

        f32 vx_rc = vx * rc;
        f32 vx_rs = vx * rs;
        f32 vy_rc = vy * rc;
        f32 vy_rs = vy * rs;

        f32 cr, sr;
        PSSinCosRad(&sr, &cr, -rot.z);

        f32 cr_sx = cr * sx;
        f32 sr_sx = sr * sx;
        f32 cr_sy = cr * sy;
        f32 sr_sy = sr * sy;

        f32 ox = px - cr_sx * px - sr_sy * py;
        f32 oy = py + sr_sx * px - cr_sy * py;

        p0.x = pos.x + (vx_rc * ox + vy_rs * oy);
        p0.y = pos.y + (vx_rs * ox - vy_rc * oy);
        p0.z = pos.z;

        f32 vx_rc_cr_sx = vx_rc * cr_sx;
        f32 vx_rc_sr_sy = vx_rc * sr_sy;
        f32 vy_rs_sr_sx = vy_rs * sr_sx;
        f32 vy_rs_cr_sy = vy_rs * cr_sy;
        f32 vx_rs_cr_sx = vx_rs * cr_sx;
        f32 vx_rs_sr_sy = vx_rs * sr_sy;
        f32 vy_rc_sr_sx = vy_rc * sr_sx;
        f32 vy_rc_cr_sy = vy_rc * cr_sy;

        d0.x = vx_rc_cr_sx - vx_rc_sr_sy - vy_rs_sr_sx - vy_rs_cr_sy;
        d0.y = vy_rc_cr_sy + (vy_rc_sr_sx + (vx_rs_cr_sx - vx_rs_sr_sy));
        d0.z = 0.0f;

        d1.x = vy_rs_cr_sy + (vx_rc_cr_sx + vx_rc_sr_sy - vy_rs_sr_sx);
        d1.y = (vy_rc_sr_sx + (vx_rs_cr_sx + vx_rs_sr_sy)) - vy_rc_cr_sy;
        d1.z = 0.0f;

        DispPolygon(p0, d0, d1, texCoord);
    } else {
        CalcNormalPolygon(pVtx, pParticle, &rMtx, &rPivot, vx, vy, rc, rs, sx,
                          sy);
        DispPolygon(pVtx, texCoord);
    }
}

static inline void DispParticle_YBillboard(Particle* pParticle,
                                           const math::MTX34& rMtx, f32 vx,
                                           f32 vy, f32 rc, f32 rs, f32 sx,
                                           f32 sy, const math::VEC2& rPivot,
                                           bool texCoord) {
    math::VEC3 rot;
    pParticle->Draw_GetRotate(&rot);

    math::VEC3 pos;
    math::VEC3Transform(&pos, &rMtx, &pParticle->mParameter.mPosition);

    f32 vy_rs = vy * rs;
    f32 vy_rc = vy * rc;

    f32 px = rPivot.x;
    f32 py = rPivot.y;

    math::VEC3 p0;
    math::VEC3 d0;
    math::VEC3 d1;

    if (rot.z != 0.0f) {
        f32 sr, cr;
        PSSinCosRad(&sr, &cr, -rot.z);

        f32 cr_sx = cr * sx;
        f32 sr_sx = sr * sx;
        f32 cr_sy = cr * sy;
        f32 sr_sy = sr * sy;

        f32 oy = -py - sr_sx * px + cr_sy * py;
        f32 ox = px - cr_sx * px - sr_sy * py;

        p0.x = pos.x + vx * ox;
        p0.y = pos.y + vy_rs * oy;
        p0.z = pos.z + vy_rc * oy;

        d0.x = vx * (cr_sx - sr_sy);
        d0.y = vy_rs * (sr_sx + cr_sy);
        d0.z = vy_rc * (sr_sx + cr_sy);

        d1.x = vx * (cr_sx + sr_sy);
        d1.y = vy_rs * (sr_sx - cr_sy);
        d1.z = vy_rc * (sr_sx - cr_sy);
    } else {
        p0.x = pos.x + vx * (px - px * sx);
        p0.y = pos.y + vy_rs * (py * sy - py);
        p0.z = pos.z + vy_rc * (py * sy - py);

        d0.x = vx * sx;
        d0.y = vy_rs * sy;
        d0.z = vy_rc * sy;

        d1.x = vx * sx;
        d1.y = -vy_rs * sy;
        d1.z = -vy_rc * sy;
    }

    DispPolygon(p0, d0, d1, texCoord);
}

DrawBillboardStrategy::DrawBillboardStrategy() {}

void DrawBillboardStrategy::Draw(const DrawInfo& rInfo,
                                 ParticleManager* pManager) {
    switch (pManager->mResource->GetEmitterDrawSetting()->typeOption) {
    case EmitterDrawSetting::ASSIST_BB_NORMAL:
    case EmitterDrawSetting::ASSIST_BB_NOROLL: {
        DrawNormalBillboard(rInfo, pManager);
        break;
    }

    case EmitterDrawSetting::ASSIST_BB_Y: {
        DrawYBillboard(rInfo, pManager);
        break;
    }

    case EmitterDrawSetting::ASSIST_BB_DIRECTIONAL: {
        DrawDirectionalBillboard(rInfo, pManager);
        break;
    }
    }
}

void DrawBillboardStrategy::DrawNormalBillboard(const DrawInfo& rInfo,
                                                ParticleManager* pManager) {
    math::MTX34 viewMtx;
    rInfo.GetViewMtxCooked(&viewMtx);

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    InitGraphics(rInfo, pManager);

    bool texCoord = mNumTexmap > 0;

    math::VEC2 pivot;
    pivot.x = rSetting.pivotX / 100.0f;
    pivot.y = rSetting.pivotY / 100.0f;

    math::MTX34 mtx;
    pManager->CalcGlobalMtx(&mtx);
    math::MTX34Mult(&mtx, &viewMtx, &mtx);

    f32 vx = MTXColLen(&mtx, 0);
    f32 vy = MTXColLen(&mtx, 1);

    f32 rc, rs;
    f32 mag =
        math::FSqrt(viewMtx._01 * viewMtx._01 + viewMtx._11 * viewMtx._11);

    if (rSetting.typeOption == EmitterDrawSetting::ASSIST_BB_NOROLL ||
        mag == 0.0f) {
        rs = 0.0f;
        rc = 1.0f;
    } else {
        f32 inv = math::FInv(mag);
        rs = -viewMtx._01 * inv;
        rc = viewMtx._11 * inv;
    }

    GetFirstDrawParticleFunc pGetFirstFunc = GetGetFirstDrawParticleFunc(
        rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER);

    GetNextDrawParticleFunc pGetNextFunc = GetGetNextDrawParticleFunc(
        rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER);

    bool first = true;

    for (Particle* pParticle = pGetFirstFunc(pManager); pParticle != nullptr;
         pParticle = pGetNextFunc(pManager, pParticle)) {
        f32 sx = pParticle->Draw_GetSizeX();
        if (sx < NW4R_MATH_FLT_EPSILON) {
            continue;
        }

        f32 sy = pParticle->Draw_GetSizeY();
        if (sy < NW4R_MATH_FLT_EPSILON) {
            continue;
        }

        SetupGP(pParticle, rSetting, rInfo, first, false);
        first = false;

        DispParticle_Normal(pParticle, mtx, vx, vy, rc, rs, sx, sy, pivot,
                            texCoord);
    }
}

void DrawBillboardStrategy::DrawYBillboard(const DrawInfo& rInfo,
                                           ParticleManager* pManager) {
    math::MTX34 viewMtx;
    rInfo.GetViewMtxCooked(&viewMtx);

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    InitGraphics(rInfo, pManager);

    bool texCoord = mNumTexmap > 0;

    math::VEC2 pivot(rSetting.pivotX / 100.0f, rSetting.pivotY / 100.0f);

    math::MTX34 mtx;
    pManager->CalcGlobalMtx(&mtx);
    math::MTX34Mult(&mtx, &viewMtx, &mtx);

    f32 vx = MTXColLen(&mtx, 0);
    f32 vy = MTXColLen(&mtx, 1);

    f32 rs, rc;
    f32 mag =
        math::FSqrt(viewMtx._11 * viewMtx._11 + viewMtx._21 * viewMtx._21);

    if (mag == 0.0f) {
        rs = 0.0f;
        rc = 1.0f;
    } else {
        f32 inv = math::FInv(mag);
        rc = viewMtx._21 * inv;
        rs = viewMtx._11 * inv;
    }

    GetFirstDrawParticleFunc pGetFirstFunc = GetGetFirstDrawParticleFunc(
        rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER);

    GetNextDrawParticleFunc pGetNextFunc = GetGetNextDrawParticleFunc(
        rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER);

    bool first = true;

    for (Particle* pParticle = pGetFirstFunc(pManager); pParticle != nullptr;
         pParticle = pGetNextFunc(pManager, pParticle)) {
        if (pParticle->GetLifeStatus() != ReferencedObject::NW4R_EF_LS_ACTIVE) {
            continue;
        }

        f32 sx = pParticle->Draw_GetSizeX();
        if (sx < NW4R_MATH_FLT_EPSILON) {
            continue;
        }

        f32 sy = pParticle->Draw_GetSizeY();
        if (sy < NW4R_MATH_FLT_EPSILON) {
            continue;
        }

        SetupGP(pParticle, rSetting, rInfo, first, false);
        first = false;

        DispParticle_YBillboard(pParticle, mtx, vx, vy, rc, rs, sx, sy, pivot,
                                texCoord);
    }
}

}
}
