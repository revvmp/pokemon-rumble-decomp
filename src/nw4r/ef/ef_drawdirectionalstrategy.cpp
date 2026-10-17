#include <nw4r/db/db_assert.h>
#include <nw4r/ef/drawstrategy/ef_drawdirectionalstrategy.h>
#include <nw4r/ef/ef_drawinfo.h>
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/ef/ef_resource.h>
#include <nw4r/ef/ef_util.h>
#include <nw4r/math/math_arithmetic.h>
#include <revolution/GX.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/gx/GXVert.h>

namespace nw4r {
namespace ef {

static u8 directional_tex0_u8[] = {0x00, 0x01, 0x00, 0x00,
                                   0x01, 0x00, 0x01, 0x01};

static inline void GXPosition(const math::VEC3& rPos) {
    GXPosition3f32(rPos.x, rPos.y, rPos.z);
}

DrawDirectionalStrategy::DrawDirectionalStrategy() {}

static void DrawQuad(const math::MTX34& rMtx, const math::_VEC3* pPosArray,
                     int texCoord) {
    math::VEC3 p0;
    math::VEC3 p1;
    math::VEC3 p2;
    math::VEC3 p3;

    math::VEC3Transform(&p0, &rMtx,
                        static_cast<const math::VEC3*>(&pPosArray[0]));
    math::VEC3Transform(&p1, &rMtx,
                        static_cast<const math::VEC3*>(&pPosArray[1]));
    math::VEC3Transform(&p2, &rMtx,
                        static_cast<const math::VEC3*>(&pPosArray[2]));
    math::VEC3Transform(&p3, &rMtx,
                        static_cast<const math::VEC3*>(&pPosArray[3]));

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GXPosition(p0);
        if (texCoord & 1) {
            GXTexCoord1x8(0);
        }

        GXPosition(p1);
        if (texCoord & 1) {
            GXTexCoord1x8(1);
        }

        GXPosition(p2);
        if (texCoord & 1) {
            GXTexCoord1x8(2);
        }

        GXPosition(p3);
        if (texCoord & 1) {
            GXTexCoord1x8(3);
        }
    }
    GXEnd();
}

static void DrawQuad(const math::VEC3& rCenter, const math::VEC3& rD0,
                     const math::VEC3& rD1, int texCoord) {
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GXPosition(rCenter - rD0);
        if (texCoord & 1) {
            GXTexCoord1x8(0);
        }

        GXPosition(rCenter - rD1);
        if (texCoord & 1) {
            GXTexCoord1x8(1);
        }

        GXPosition(rCenter + rD0);
        if (texCoord & 1) {
            GXTexCoord1x8(2);
        }

        GXPosition(rCenter + rD1);
        if (texCoord & 1) {
            GXTexCoord1x8(3);
        }
    }
    GXEnd();
}

void DrawDirectionalStrategy::Draw(const DrawInfo& rInfo,
                                   ParticleManager* pManager) {
    InitGraphics(rInfo, pManager);

    if ((pManager->mResource->GetEmitterDrawSetting()->typeOption2 & 3) != 1) {
        DrawDirectional(rInfo, pManager);
    } else {
        DrawDirectionalBillboard(rInfo, pManager);
    }
}

static inline math::MTX34 CalcRotate(Particle* pParticle, u8 axis) {
    math::VEC3 rot;
    pParticle->Draw_GetRotate(&rot);

    f32 sinX = 0.0f, cosX = 1.0f;
    f32 sinY = 0.0f, cosY = 1.0f;
    f32 sinZ = 0.0f, cosZ = 1.0f;

    switch (axis) {
    case 0: {
        if (rot.x != 0.0f) {
            PSSinCosRad(&sinX, &cosX, -rot.x);
        }

        return math::MTX34(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cosX, -sinX, 0.0f,
                           0.0f, sinX, cosX, 0.0f);
    }

    case 1: {
        if (rot.y != 0.0f) {
            PSSinCosRad(&sinY, &cosY, -rot.y);
        }

        return math::MTX34(cosY, 0.0f, sinY, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                           -sinY, 0.0f, cosY, 0.0f);
    }

    case 2: {
        if (rot.z != 0.0f) {
            PSSinCosRad(&sinZ, &cosZ, -rot.z);
        }

        return math::MTX34(cosZ, -sinZ, 0.0f, 0.0f, sinZ, cosZ, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f);
    }

    default: {
        if (rot.x != 0.0f) {
            PSSinCosRad(&sinX, &cosX, -rot.x);
        }

        if (rot.y != 0.0f) {
            PSSinCosRad(&sinY, &cosY, -rot.y);
        }

        if (rot.z != 0.0f) {
            PSSinCosRad(&sinZ, &cosZ, -rot.z);
        }

        f32 sx_sy = sinX * sinY;
        f32 cx_sy = cosX * sinY;

        return math::MTX34(cosY * cosZ, sx_sy * cosZ - cosX * sinZ,
                           cx_sy * cosZ + sinX * sinZ, 0.0f, cosY * sinZ,
                           sx_sy * sinZ + cosX * cosZ,
                           cx_sy * sinZ - sinX * cosZ, 0.0f, -sinY,
                           cosY * sinX, cosX * cosY, 0.0f);
    }
    }
}

static inline math::MTX34 CalcLocalTransform(u8 option, f32 px, f32 py,
                                             f32 sx, f32 sy, f32 stretch,
                                             const math::MTX34& rRot) {
    switch (option) {
    case 0: {
        f32 sx_px = sx * px;
        f32 sy_st = sy * stretch;
        f32 sy_py = sy * (py + stretch - 1.0f);

        return math::MTX34(
            rRot._00 * sx, rRot._01 * sy_st, rRot._02 * sx,
            px + (-rRot._00 * sx_px - rRot._01 * sy_py), rRot._10 * sx,
            rRot._11 * sy_st, rRot._12 * sx,
            py + (-rRot._10 * sx_px - rRot._11 * sy_py), rRot._20 * sx,
            rRot._21 * sy_st, rRot._22 * sx,
            -rRot._20 * sx_px - rRot._21 * sy_py);
    }

    default: {
        f32 sx_px = sx * px;
        f32 sy_py = sy * py;

        return math::MTX34(
            rRot._00 * sx, rRot._02 * sy, rRot._01 * sx,
            px + (-rRot._00 * sx_px - rRot._02 * sy_py), rRot._10 * sx,
            rRot._12 * sy, rRot._11 * sx, -rRot._10 * sx_px - rRot._12 * sy_py,
            rRot._20 * sx, rRot._22 * sy, rRot._21 * sx,
            (-rRot._20 * sx_px - rRot._22 * sy_py) - py);
    }
    }
}

void DrawDirectionalStrategy::DrawDirectional(const DrawInfo& rInfo,
                                              ParticleManager* pManager) {
    InitGraphics(rInfo, pManager);

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    math::MTX34 viewMtx;
    rInfo.GetViewMtxCooked(&viewMtx);

    AheadContext context(viewMtx, pManager);
    CalcAheadFunc pCalcAheadFunc = GetCalcAheadFunc(pManager);

    int texCoord;
    if (mNumTexmap > 0) {
        texCoord = 1;
    } else {
        texCoord = 0;
    }

    math::VEC3 emAxisY;
    math::VEC3 emAxisZ;

    if (rSetting.typeDir == EmitterDrawSetting::DIR_7) {
        math::MTX34 posMtx;
        math::MTX34Mult(&posMtx, &viewMtx,
                        &context.mCommon.mParticleManagerMtx);
        GXLoadPosMtxImm(posMtx, GX_PNMTX0);

        const math::MTX34& rInvMtx = context.mCommon.mParticleManagerMtxInv;
        emAxisY = math::VEC3(rInvMtx._01, rInvMtx._11, rInvMtx._21);
        emAxisZ = math::VEC3(rInvMtx._02, rInvMtx._12, rInvMtx._22);
        Normalize(&emAxisZ);
    } else {
        math::MTX34 posMtx;
        math::MTX34Mult(&posMtx, &viewMtx,
                        &context.mCommon.mParticleManagerMtx);
        GXLoadPosMtxImm(posMtx, GX_PNMTX0);

        math::MTX34 emMtx;
        pManager->mManagerEM->CalcGlobalMtx(&emMtx);
        math::MTX34Mult(&emMtx, &context.mCommon.mParticleManagerMtxInv,
                        &emMtx);

        emAxisY = math::VEC3(emMtx._01, emMtx._11, emMtx._21);
        emAxisZ = math::VEC3(emMtx._02, emMtx._12, emMtx._22);
        Normalize(&emAxisZ);
    }

    for (Particle* pIt = GetYoungestParticle(pManager);
         pIt != nullptr && pIt->mPrevAxis.x > 1.0f;
         pIt = GetElderParticle(pManager, pIt)) {
        pIt->mPrevAxis = emAxisY;
    }

    f32 px = rSetting.pivotX / 100.0f;
    f32 py = rSetting.pivotY / 100.0f;

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

        f32 stretch = 1.0f;
        if (rSetting.typeOption0 != 0) {
            math::VEC3 move;
            pParticle->GetMoveDir(&move);

            stretch += 0.5f * math::FSqrt(math::VEC3Dot(&move, &move)) / sy;
        }

        SetupGP(pParticle, rSetting, rInfo, first, false);
        first = false;

        math::MTX34 rotMtx = CalcRotate(pParticle, rSetting.typeAxis);
        math::MTX34 locMtx = CalcLocalTransform(rSetting.typeOption1, px, py,
                                                sx, sy, stretch, rotMtx);

        math::VEC3 axisX;
        math::VEC3 axisY;
        math::VEC3 axisZ;

        pCalcAheadFunc(&axisY, &context, pParticle);
        math::VEC3Cross(&axisX, &pParticle->mPrevAxis, &axisY);

        if (!Normalize(&axisX)) {
            axisX = emAxisZ;
        }

        math::VEC3Cross(&axisZ, &axisY, &axisX);

        pParticle->mPrevAxis = axisZ;

        math::VEC3 pos = pParticle->mParameter.mPosition;

        math::MTX34 dirMtx(axisZ.x, axisY.x, axisX.x, pos.x, axisZ.y, axisY.y,
                           axisX.y, pos.y, axisZ.z, axisY.z, axisX.z, pos.z);

        math::MTX34 drawMtx;
        math::MTX34Mult(&drawMtx, &dirMtx, &locMtx);

        static const math::_VEC3 quad[4] = {
            {-1.0f, -1.0f, 0.0f},
            {-1.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {1.0f, -1.0f, 0.0f},
        };

        DrawQuad(drawMtx, quad, texCoord);

        if (rSetting.typeOption == EmitterDrawSetting::TYPE_CMN_CROSS) {
            static const math::_VEC3 cross[4] = {
                {0.0f, -1.0f, 1.0f},
                {0.0f, 1.0f, 1.0f},
                {0.0f, 1.0f, -1.0f},
                {0.0f, -1.0f, -1.0f},
            };

            DrawQuad(drawMtx, cross, texCoord);
        }
    }
}

void DrawDirectionalStrategy::InitGraphics(const DrawInfo& rInfo,
                                           ParticleManager* pManager) {
    NW4R_ASSERT_VALID_PTR(pManager);

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    Initialize(pManager, rSetting, rInfo);

    GXEnableTexOffsets(GX_TEXCOORD0, TRUE, TRUE);

    GXSetArray(GX_VA_TEX0, directional_tex0_u8, 2);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);

    if (mNumTexmap > 0) {
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
    }

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);

    GXSetCurrentMtx(GX_PNMTX0);
}

}
}
