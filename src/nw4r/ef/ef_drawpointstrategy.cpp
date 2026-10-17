#include <nw4r/ef/drawstrategy/ef_drawpointstrategy.h>
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

static void DrawPoint(const math::VEC3& rPos, bool texCoord);
static void GXPosition(const math::VEC3& rPos);

DrawPointStrategy::DrawPointStrategy() {}

void DrawPointStrategy::Draw(const DrawInfo& rInfo, ParticleManager* pManager) {
    InitGraphics(rInfo, pManager);

    int texCoord;

    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    texCoord = mNumTexmap > 0;

    math::MTX34 glbMtx;
    math::MTX34 posMtx;

    pManager->CalcGlobalMtx(&glbMtx);
    rInfo.GetViewMtxCooked(&posMtx);
    math::MTX34Mult(&posMtx, &posMtx, &glbMtx);
    GXLoadPosMtxImm(posMtx, GX_PNMTX0);

    GetFirstDrawParticleFunc pGetFirstFunc = GetGetFirstDrawParticleFunc(
        rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER);

    GetNextDrawParticleFunc pGetNextFunc = GetGetNextDrawParticleFunc(
        rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER);

    u8 prevPointSize = 0;
    bool first = true;

    for (Particle* pParticle = pGetFirstFunc(pManager); pParticle != nullptr;
         pParticle = pGetNextFunc(pManager, pParticle)) {
        f32 sx = pParticle->Draw_GetSizeX();
        if (sx < NW4R_MATH_FLT_EPSILON) {
            continue;
        }

        u8 pointSize = static_cast<u8>(6.0f * Min(42.5f, sx));
        if (pointSize == 0) {
            continue;
        }

        if (prevPointSize != pointSize) {
            prevPointSize = pointSize;
            GXSetPointSize(pointSize, GX_TO_ONE);

            SetupGP(pParticle, rSetting, rInfo, first, true);
        } else {
            SetupGP(pParticle, rSetting, rInfo, first, false);
        }

        first = false;
        DrawPoint(pParticle->mParameter.mPosition, texCoord);
    }
}

static void DrawPoint(const math::VEC3& rPos, bool texCoord) {
    GXBegin(GX_POINTS, GX_VTXFMT0, 1);
    {
        GXPosition(rPos);

        if (texCoord) {
            GXTexCoord2f32(0.0f, 0.0f);
        }
    }
    GXEnd();
}

static void GXPosition(const math::VEC3& rPos) {
    GXPosition3f32(rPos.x, rPos.y, rPos.z);
}

void DrawPointStrategy::InitGraphics(const DrawInfo& rInfo,
                                     ParticleManager* pManager) {
    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    Initialize(pManager, rSetting, rInfo);

    GXEnableTexOffsets(GX_TEXCOORD0, TRUE, TRUE);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);

    if (mNumTexmap > 0) {
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    }

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    GXSetCurrentMtx(GX_PNMTX0);
}

}
}
