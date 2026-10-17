#include <nw4r/ef/drawstrategy/ef_drawsmoothstripestrategy.h>
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

typedef DrawSmoothStripeStrategy::Vertex Vertex;
typedef DrawSmoothStripeStrategy::VertexTube VertexTube;
typedef DrawSmoothStripeStrategy::Trigonometric Trigonometric;

static inline math::VEC3 CalcBSplineWeight(f32 t) {
    f32 t2 = t * t;

    return math::VEC3(0.5f + (0.5f * t2 - t), 0.5f + (-1.0f * t2 + t), 0.5f * t2);
}

static inline Vertex BlendVertex(const Vertex& rV0, const Vertex& rV1,
                                 const Vertex& rV2, const math::VEC3& rWeight) {
    Vertex vertex;
    vertex.pos0 = rV0.pos0 * rWeight.x + rV1.pos0 * rWeight.y +
                  rV2.pos0 * rWeight.z;
    vertex.pos1 = rV0.pos1 * rWeight.x + rV1.pos1 * rWeight.y +
                  rV2.pos1 * rWeight.z;
    vertex.texT = rV2.texT * rWeight.z +
                  (rV0.texT * rWeight.x + rV1.texT * rWeight.y);
    return vertex;
}

static inline VertexTube BlendVertexTube(const VertexTube& rV0,
                                         const VertexTube& rV1,
                                         const VertexTube& rV2,
                                         const math::VEC3& rWeight) {
    VertexTube vertex;
    vertex.pos = rV0.pos * rWeight.x + rV1.pos * rWeight.y +
                 rV2.pos * rWeight.z;
    vertex.axisX = rV0.axisX * rWeight.x + rV1.axisX * rWeight.y +
                   rV2.axisX * rWeight.z;
    vertex.axisZ = rV0.axisZ * rWeight.x + rV1.axisZ * rWeight.y +
                   rV2.axisZ * rWeight.z;
    vertex.texT = rV2.texT * rWeight.z +
                  (rV0.texT * rWeight.x + rV1.texT * rWeight.y);
    return vertex;
}

static inline void GXPosition(const math::VEC3& rPos) {
    GXPosition3f32(rPos.x, rPos.y, rPos.z);
}

static inline void DrawVertexPair(const math::VEC3& rPos0,
                                  const math::VEC3& rPos1, f32 texT,
                                  int flags) {
    GXPosition(rPos0);
    if (flags & DrawSmoothStripeStrategy::DRAWFLAG_TEXCOORD) {
        GXTexCoord2f32(1.0f, texT);
    }

    GXPosition(rPos1);
    if (flags & DrawSmoothStripeStrategy::DRAWFLAG_TEXCOORD) {
        GXTexCoord2f32(0.0f, texT);
    }
}

DrawSmoothStripeStrategy::DrawSmoothStripeStrategy() {}

math::MTX34 DrawSmoothStripeStrategy::CalcAxisMtx(
    AheadContextStripe* pContext, int flags, Particle* pParticle,
    const math::VEC3& rAxisY, const math::VEC3& rPos) {

    math::VEC3 axisX;
    math::VEC3 axisZ;

    if (!(flags & DRAWFLAG_VIEW_AXIS)) {
        math::VEC3Cross(&axisX, &rAxisY, &pParticle->mPrevAxis);
    } else {
        math::VEC3Cross(&axisX, &rAxisY, &pContext->mViewAxisZ);
    }

    if (!Normalize(&axisX)) {
        axisX = pContext->mEmitterAxisX;
    }

    math::VEC3Cross(&axisZ, &axisX, &rAxisY);
    Normalize(&axisZ);
    pParticle->mPrevAxis = axisZ;

    return math::MTX34(axisX.x, rAxisY.x, axisZ.x, rPos.x,
                       axisX.y, rAxisY.y, axisZ.y, rPos.y,
                       axisX.z, rAxisY.z, axisZ.z, rPos.z);
}

math::MTX34 DrawSmoothStripeStrategy::CalcRotateMtx(Particle* pParticle,
                                                    f32 pivot) {
    math::VEC3 rot;
    pParticle->Draw_GetRotate(&rot);

    f32 size = pParticle->Draw_GetSizeX();

    f32 c, s;
    PSSinCosRad(&s, &c, rot.y);
    c *= size;
    s *= size;

    return math::MTX34(c, 0.0f, -s, pivot - c * pivot,
                       0.0f, 1.0f, 0.0f, 0.0f,
                       s, 0.0f, c, -s * pivot);
}

void DrawSmoothStripeStrategy::CalcVertex(
    Vertex* pVertex, AheadContextStripe* pContext, int flags,
    Particle* pParticle, CalcAheadStripeFunc pCalcAheadFunc,
    const math::VEC3& rPos, const math::VEC3& rEdge0, const math::VEC3& rEdge1,
    f32 pivot, f32 texT) {

    math::VEC3 axisY;
    math::MTX34 mtx;

    pCalcAheadFunc(&axisY, pContext, pParticle);

    math::MTX34 axisMtx = CalcAxisMtx(pContext, flags, pParticle, axisY, rPos);
    math::MTX34 rotMtx = CalcRotateMtx(pParticle, pivot);
    math::MTX34Mult(&mtx, &axisMtx, &rotMtx);

    math::VEC3Transform(&pVertex->pos0, &mtx, &rEdge0);
    math::VEC3Transform(&pVertex->pos1, &mtx, &rEdge1);
    pVertex->texT = texT;
}

void DrawSmoothStripeStrategy::CalcVertexTube(
    VertexTube* pVertex, AheadContextStripe* pContext, int flags,
    Particle* pParticle, CalcAheadStripeFunc pCalcAheadFunc, f32 pivotX,
    f32 pivotY, f32 texT) {

    math::VEC3 axisY;
    pCalcAheadFunc(&axisY, pContext, pParticle);

    math::MTX34 axisMtx = CalcAxisMtx(pContext, flags, pParticle, axisY,
                                      pParticle->mParameter.mPosition);

    math::VEC3 rot;
    pParticle->Draw_GetRotate(&rot);

    f32 sx = pParticle->Draw_GetSizeX();
    f32 sy = pParticle->Draw_GetSizeY();

    f32 s, c;
    PSSinCosRad(&s, &c, rot.y);

    math::MTX34 rotMtx(
        c * sx, 0.0f, -s * sy, (pivotX - pivotX * (c * sx)) + pivotY * (s * sy),
        0.0f, 1.0f, 0.0f, 0.0f,
        s * sx, 0.0f, c * sy, (pivotY - pivotX * (s * sx)) - pivotY * (c * sy));

    math::MTX34 mtx;
    math::MTX34Mult(&mtx, &axisMtx, &rotMtx);

    pVertex->pos = math::VEC3(mtx._03, mtx._13, mtx._23);
    math::VEC3TransformNormal(&pVertex->axisX, &mtx, &GetXUnitVec());
    math::VEC3TransformNormal(&pVertex->axisZ, &mtx, &GetZUnitVec());
    pVertex->texT = texT;
}

void DrawSmoothStripeStrategy::DrawSection_BSpline(int numDivide,
                                                   const Vertex& rV0,
                                                   const Vertex& rV1,
                                                   const Vertex& rV2,
                                                   int flags) {
    f32 invDivide = math::FInv(numDivide);

    for (int i = 0; i < numDivide; i++) {
        f32 t = i * invDivide;
        math::VEC3 weight = CalcBSplineWeight(t);
        Vertex vertex = BlendVertex(rV0, rV1, rV2, weight);
        DrawVertexPair(vertex.pos0, vertex.pos1, vertex.texT, flags);
    }
}

void DrawSmoothStripeStrategy::DrawSectionEnd_BSpline(const Vertex& rV0,
                                                      const Vertex& rV1,
                                                      const Vertex& rV2,
                                                      int flags) {
    math::VEC3 weight = CalcBSplineWeight(1.0f);
    Vertex vertex = BlendVertex(rV0, rV1, rV2, weight);
    DrawVertexPair(vertex.pos0, vertex.pos1, vertex.texT, flags);
}

void DrawSmoothStripeStrategy::DrawSectionTube_BSpline(
    int numDivide, int numTubeDivide, const VertexTube& rV0,
    const VertexTube& rV1, const VertexTube& rV2, int flags,
    const Trigonometric* pTrig) {

    f32 invDivide = math::FInv(numDivide);
    f32 invTubeDivide = math::FInv(numTubeDivide);

    math::VEC3 weight = CalcBSplineWeight(0.0f);
    VertexTube prev;
    VertexTube curr = BlendVertexTube(rV0, rV1, rV2, weight);

    for (int i = 1; i <= numDivide; i++) {
        prev = curr;
        f32 t = i * invDivide;
        weight = CalcBSplineWeight(t);
        curr = BlendVertexTube(rV0, rV1, rV2, weight);

        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, numTubeDivide * 2 + 2);

        if (flags & DRAWFLAG_REVERSE) {
            for (int j = 0; j <= numTubeDivide; j++) {
                f32 s = pTrig[j].sin;
                f32 c = pTrig[j].cos;

                GXPosition(prev.axisX * c + prev.axisZ * s + prev.pos);
                if (flags & DRAWFLAG_TEXCOORD) {
                    GXTexCoord2f32(invTubeDivide * j, prev.texT);
                }

                GXPosition(curr.axisX * c + curr.axisZ * s + curr.pos);
                if (flags & DRAWFLAG_TEXCOORD) {
                    GXTexCoord2f32(invTubeDivide * j, curr.texT);
                }
            }
        } else {
            for (int j = 0; j <= numTubeDivide; j++) {
                f32 s = pTrig[j].sin;
                f32 c = pTrig[j].cos;

                GXPosition(curr.axisX * c + curr.axisZ * s + curr.pos);
                if (flags & DRAWFLAG_TEXCOORD) {
                    GXTexCoord2f32(invTubeDivide * j, curr.texT);
                }

                GXPosition(prev.axisX * c + prev.axisZ * s + prev.pos);
                if (flags & DRAWFLAG_TEXCOORD) {
                    GXTexCoord2f32(invTubeDivide * j, prev.texT);
                }
            }
        }

        GXEnd();
    }
}

void DrawSmoothStripeStrategy::DrawSmoothStripe_BSpline_ConnectNone(
    AheadContextStripe* pContext, int numDivide, int flags,
    const math::VEC3& rEdge0, const math::VEC3& rEdge1) {

    ParticleManager* pManager = pContext->mCommon.mParticleManager;
    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    CalcAheadStripeFunc pCalcAheadFunc =
        reinterpret_cast<CalcAheadStripeFunc>(GetCalcAheadFunc(pManager));

    int drawOrder = GetDrawOrder(rSetting);
    GetFirstDrawParticleFunc pGetFirstFunc =
        GetGetFirstDrawParticleFunc(drawOrder);
    GetNextDrawParticleFunc pGetNextFunc = GetGetNextDrawParticleFunc(drawOrder);

    int numParticle = GetNumDrawParticle(pManager);
    f32 pivot = 0.01f * rSetting.pivotX;

    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, (numParticle * numDivide + 1) * 2);

    f32 texScale;
    if (GetStripeTexmapType(rSetting) == STRIPE_TEXMAP_REPEAT) {
        texScale = 1.0f;
    } else {
        texScale = math::FInv(numParticle - 1);
    }

    int texIndex = 0;
    int texStep = 1;
    if (drawOrder != 0) {
        texIndex = numParticle - 1;
        texStep = -1;
    }

    Vertex vertex[3];
    int prev2 = 0;
    int prev = 0;
    int curr = 0;

    Particle* pParticle = pGetFirstFunc(pManager);
    CalcVertex(&vertex[curr], pContext, flags, pParticle, pCalcAheadFunc,
               pParticle->mParameter.mPosition, rEdge0, rEdge1, pivot,
               texScale * texIndex);
    texIndex += texStep;
    pParticle = pGetNextFunc(pManager, pParticle);

    while (pParticle != nullptr) {
        prev2 = prev;
        prev = curr;
        curr = (curr + 1) % 3;

        CalcVertex(&vertex[curr], pContext, flags, pParticle,
                   pCalcAheadFunc, pParticle->mParameter.mPosition, rEdge0,
                   rEdge1, pivot, texScale * texIndex);
        DrawSection_BSpline(numDivide, vertex[prev2], vertex[prev],
                            vertex[curr], flags);

        pParticle = pGetNextFunc(pManager, pParticle);
        texIndex += texStep;
    }

    DrawSection_BSpline(numDivide, vertex[prev], vertex[curr], vertex[curr],
                        flags);
    DrawSectionEnd_BSpline(vertex[prev], vertex[curr], vertex[curr], flags);
    GXEnd();
}

}
}
