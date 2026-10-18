#include <nw4r/ef/drawstrategy/ef_drawsmoothstripestrategy.h>
#include <nw4r/ef/ef_drawinfo.h>
#include <nw4r/ef/ef_effect.h>
#include <nw4r/ef/ef_effectsystem.h>
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/ef/ef_resource.h>
#include <nw4r/ef/ef_util.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <revolution/GX.h>
#include <revolution/gx/GXTransform.h>
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

static inline void BeginTubeSide(GXCullMode cullMode, bool flushGP,
                                 int flags) {
    GXSetCullMode(cullMode);

    if (flushGP) {
        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, 8);

        for (int i = 0; i < 8; i++) {
            GXPosition3f32(0.0f, 0.0f, 0.0f);
            if (flags & DrawSmoothStripeStrategy::DRAWFLAG_TEXCOORD) {
                GXTexCoord2f32(0.0f, 0.0f);
            }
        }

        GXEnd();
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

inline DrawSmoothStripeStrategy::Vertex::Vertex() {}

void DrawSmoothStripeStrategy::DrawSmoothStripe_BSpline_ConnectRing(
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
        texScale = 0.5f;
    } else {
        texScale = 0.5f / numParticle;
    }

    int texIndex = -1;
    int texStep = 2;
    if (drawOrder != 0) {
        texIndex = numParticle * 2 + 1;
        texStep = -2;
    }

    Vertex vertex[5];
    math::VEC3 edge0;
    math::VEC3 edge1;
    int prev2;
    int prev;
    int curr;

    Particle* pParticle = pGetFirstFunc(pManager);
    CalcVertex(&vertex[3], pContext, flags, pParticle, pCalcAheadFunc,
               pParticle->mParameter.mPosition, rEdge0, rEdge1, pivot,
               texScale * texIndex);
    pParticle = pGetNextFunc(pManager, pParticle);
    texIndex += texStep;

    CalcVertex(&vertex[4], pContext, flags, pParticle, pCalcAheadFunc,
               pParticle->mParameter.mPosition, rEdge0, rEdge1, pivot,
               texScale * texIndex);
    pParticle = pGetNextFunc(pManager, pParticle);
    texIndex += texStep;

    prev = 3;
    curr = 4;

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

    vertex[3].texT = texScale * texIndex;
    DrawSection_BSpline(numDivide, vertex[prev], vertex[curr], vertex[3],
                        flags);

    texIndex += texStep;
    vertex[4].texT = texScale * texIndex;
    DrawSection_BSpline(numDivide, vertex[curr], vertex[3], vertex[4], flags);
    DrawSectionEnd_BSpline(vertex[curr], vertex[3], vertex[4], flags);
    GXEnd();
}

void DrawSmoothStripeStrategy::DrawSmoothStripe_BSpline_ConnectEmitter(
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

    f32 pivot = 0.01f * rSetting.pivotX;

    Particle* pYoungest =
        GetYoungestDrawParticle(pContext->mCommon.mParticleManager);
    math::VEC3 offset =
        pContext->mCommon.mEmitterCenter - pYoungest->mParameter.mPosition;

    int numParticle = GetNumDrawParticle(pManager);

    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0,
            ((numParticle + 1) * numDivide + 1) * 2);

    f32 texScale;
    if (GetStripeTexmapType(rSetting) == STRIPE_TEXMAP_REPEAT) {
        texScale = 1.0f;
    } else {
        texScale = math::FInv(numParticle);
    }

    int texIndex = 0;
    int texStep = 1;
    if (drawOrder != 0) {
        texIndex = numParticle;
        texStep = -1;
    }

    Vertex vertex[3];
    int prev2 = 0;
    int prev = 0;
    int curr = 0;
    math::VEC3 edge0;
    math::VEC3 edge1;

    Particle* pParticle = pGetFirstFunc(pManager);
    CalcVertex(&vertex[0], pContext, flags, pParticle, pCalcAheadFunc,
               pParticle->mParameter.mPosition, rEdge0, rEdge1, pivot,
               texScale * texIndex);
    texIndex += texStep;

    if (drawOrder == 0) {
        curr = 1;
        vertex[1] = vertex[0];
        vertex[1].texT = texScale * texIndex;
        vertex[0].pos0 += offset;
        vertex[0].pos1 += offset;
        texIndex += texStep;

        DrawSection_BSpline(numDivide, vertex[0], vertex[0], vertex[1],
                            flags);
    }

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

    if (drawOrder != 0) {
        prev2 = prev;
        prev = curr;
        curr = (curr + 1) % 3;

        vertex[curr] = Vertex(vertex[prev].pos0 + offset,
                              vertex[prev].pos1 + offset, texScale * texIndex);
        DrawSection_BSpline(numDivide, vertex[prev2], vertex[prev],
                            vertex[curr], flags);
    }

    DrawSection_BSpline(numDivide, vertex[prev], vertex[curr], vertex[curr],
                        flags);
    DrawSectionEnd_BSpline(vertex[prev], vertex[curr], vertex[curr], flags);
    GXEnd();
}

void DrawSmoothStripeStrategy::DrawSmoothStripeTube_BSpline_ConnectNone(
    AheadContextStripe* pContext, int numDivide, int flags) {

    ParticleManager* pManager = pContext->mCommon.mParticleManager;
    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    CalcAheadStripeFunc pCalcAheadFunc =
        reinterpret_cast<CalcAheadStripeFunc>(GetCalcAheadFunc(pManager));

    int drawOrder = GetDrawOrder(rSetting);
    GetFirstDrawParticleFunc pGetFirstFunc =
        GetGetFirstDrawParticleFunc(drawOrder);
    GetNextDrawParticleFunc pGetNextFunc = GetGetNextDrawParticleFunc(drawOrder);

    f32 pivotX = 0.01f * rSetting.pivotX;
    f32 pivotY = 0.01f * rSetting.pivotY;

    int numParticle = GetNumDrawParticle(pManager);

    f32 texScale;
    if (GetStripeTexmapType(rSetting) != STRIPE_TEXMAP_REPEAT) {
        texScale = math::FInv(numParticle - 1);
    } else {
        texScale = 1.0f;
    }

    int texIndex = 0;
    int texStep = 1;
    if (drawOrder != 0) {
        texIndex = numParticle - 1;
        texStep = -1;
    }

    VertexTube vertex[3];
    int prev2 = 0;
    int prev = 0;
    int curr = 0;

    Particle* pParticle = pGetFirstFunc(pManager);
    CalcVertexTube(&vertex[curr], pContext, flags, pParticle, pCalcAheadFunc,
                   pivotX, pivotY, texScale * texIndex);
    texIndex += texStep;
    pParticle = pGetNextFunc(pManager, pParticle);

    while (pParticle != nullptr) {
        prev2 = prev;
        prev = curr;
        curr = (curr + 1) % 3;

        CalcVertexTube(&vertex[curr], pContext, flags, pParticle,
                       pCalcAheadFunc, pivotX, pivotY, texScale * texIndex);
        DrawSectionTube_BSpline(numDivide, GetTubeDivide(rSetting),
                                vertex[prev2], vertex[prev], vertex[curr],
                                flags, pContext->mTrigonometric);

        pParticle = pGetNextFunc(pManager, pParticle);
        texIndex += texStep;
    }

    DrawSectionTube_BSpline(numDivide, GetTubeDivide(rSetting), vertex[prev],
                            vertex[curr], vertex[curr], flags,
                            pContext->mTrigonometric);
}

void DrawSmoothStripeStrategy::DrawSmoothStripeTube_BSpline_ConnectRing(
    AheadContextStripe* pContext, int numDivide, int flags) {

    ParticleManager* pManager = pContext->mCommon.mParticleManager;
    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    CalcAheadStripeFunc pCalcAheadFunc =
        reinterpret_cast<CalcAheadStripeFunc>(GetCalcAheadFunc(pManager));

    int drawOrder = GetDrawOrder(rSetting);
    GetFirstDrawParticleFunc pGetFirstFunc =
        GetGetFirstDrawParticleFunc(drawOrder);
    GetNextDrawParticleFunc pGetNextFunc = GetGetNextDrawParticleFunc(drawOrder);

    f32 pivotX = 0.01f * rSetting.pivotX;
    f32 pivotY = 0.01f * rSetting.pivotY;

    int numParticle = GetNumDrawParticle(pManager);

    f32 texScale;
    if (GetStripeTexmapType(rSetting) == STRIPE_TEXMAP_REPEAT) {
        texScale = 0.5f;
    } else {
        texScale = 0.5f / numParticle;
    }

    int texIndex = -1;
    int texStep = 2;
    if (drawOrder != 0) {
        texIndex = numParticle * 2 + 1;
        texStep = -2;
    }

    VertexTube vertex[5];
    int prev2;
    int prev;
    int curr;

    Particle* pParticle = pGetFirstFunc(pManager);
    CalcVertexTube(&vertex[3], pContext, flags, pParticle, pCalcAheadFunc,
                   pivotX, pivotY, texScale * texIndex);
    pParticle = pGetNextFunc(pManager, pParticle);
    texIndex += texStep;

    CalcVertexTube(&vertex[4], pContext, flags, pParticle, pCalcAheadFunc,
                   pivotX, pivotY, texScale * texIndex);
    pParticle = pGetNextFunc(pManager, pParticle);
    texIndex += texStep;

    prev = 3;
    curr = 4;

    while (pParticle != nullptr) {
        prev2 = prev;
        prev = curr;
        curr = (curr + 1) % 3;

        CalcVertexTube(&vertex[curr], pContext, flags, pParticle,
                       pCalcAheadFunc, pivotX, pivotY, texScale * texIndex);
        DrawSectionTube_BSpline(numDivide, GetTubeDivide(rSetting),
                                vertex[prev2], vertex[prev], vertex[curr],
                                flags, pContext->mTrigonometric);

        pParticle = pGetNextFunc(pManager, pParticle);
        texIndex += texStep;
    }

    vertex[3].texT = texScale * texIndex;
    DrawSectionTube_BSpline(numDivide, GetTubeDivide(rSetting), vertex[prev],
                            vertex[curr], vertex[3], flags,
                            pContext->mTrigonometric);

    texIndex += texStep;
    vertex[4].texT = texScale * texIndex;
    DrawSectionTube_BSpline(numDivide, GetTubeDivide(rSetting), vertex[curr],
                            vertex[3], vertex[4], flags,
                            pContext->mTrigonometric);
}

void DrawSmoothStripeStrategy::DrawSmoothStripeTube_BSpline_ConnectEmitter(
    AheadContextStripe* pContext, int numDivide, int flags) {

    ParticleManager* pManager = pContext->mCommon.mParticleManager;
    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();

    CalcAheadStripeFunc pCalcAheadFunc =
        reinterpret_cast<CalcAheadStripeFunc>(GetCalcAheadFunc(pManager));

    int drawOrder = GetDrawOrder(rSetting);
    GetFirstDrawParticleFunc pGetFirstFunc =
        GetGetFirstDrawParticleFunc(drawOrder);
    GetNextDrawParticleFunc pGetNextFunc = GetGetNextDrawParticleFunc(drawOrder);

    f32 pivotX = 0.01f * rSetting.pivotX;
    f32 pivotY = 0.01f * rSetting.pivotY;

    Particle* pYoungest =
        GetYoungestDrawParticle(pContext->mCommon.mParticleManager);
    math::VEC3 offset =
        pContext->mCommon.mEmitterCenter - pYoungest->mParameter.mPosition;

    int numParticle = GetNumDrawParticle(pManager);

    f32 texScale;
    if (GetStripeTexmapType(rSetting) != STRIPE_TEXMAP_REPEAT) {
        texScale = math::FInv(numParticle);
    } else {
        texScale = 1.0f;
    }

    int texIndex = 0;
    int texStep = 1;
    if (drawOrder != 0) {
        texIndex = numParticle;
        texStep = -1;
    }

    VertexTube vertex[3];
    int prev2 = 0;
    int prev = 0;
    int curr = 0;

    Particle* pParticle = pGetFirstFunc(pManager);
    CalcVertexTube(&vertex[0], pContext, flags, pParticle, pCalcAheadFunc,
                   pivotX, pivotY, texScale * texIndex);
    texIndex += texStep;

    if (drawOrder == 0) {
        curr = 1;
        vertex[1] = VertexTube(vertex[0].pos, vertex[0].axisX,
                               vertex[0].axisZ, texScale * texIndex);
        vertex[0].pos += offset;
        texIndex += texStep;

        DrawSectionTube_BSpline(numDivide, GetTubeDivide(rSetting), vertex[0],
                                vertex[0], vertex[1], flags,
                                pContext->mTrigonometric);
    }

    pParticle = pGetNextFunc(pManager, pParticle);

    while (pParticle != nullptr) {
        prev2 = prev;
        prev = curr;
        curr = (curr + 1) % 3;

        CalcVertexTube(&vertex[curr], pContext, flags, pParticle,
                       pCalcAheadFunc, pivotX, pivotY, texScale * texIndex);
        DrawSectionTube_BSpline(numDivide, GetTubeDivide(rSetting),
                                vertex[prev2], vertex[prev], vertex[curr],
                                flags, pContext->mTrigonometric);

        pParticle = pGetNextFunc(pManager, pParticle);
        texIndex += texStep;
    }

    if (drawOrder != 0) {
        prev2 = prev;
        prev = curr;
        curr = (curr + 1) % 3;

        vertex[curr] = VertexTube(vertex[prev].pos + offset,
                                  vertex[prev].axisX, vertex[prev].axisZ,
                                  texScale * texIndex);
        DrawSectionTube_BSpline(numDivide, GetTubeDivide(rSetting),
                                vertex[prev2], vertex[prev], vertex[curr],
                                flags, pContext->mTrigonometric);
    }

    DrawSectionTube_BSpline(numDivide, GetTubeDivide(rSetting), vertex[prev],
                            vertex[curr], vertex[curr], flags,
                            pContext->mTrigonometric);
}

void DrawSmoothStripeStrategy::DrawSmoothStripeTube(
    AheadContextStripe* pContext, int numDivide, int flags, bool flushGP) {

    const EmitterDrawSetting& rSetting =
        *pContext->mCommon.mParticleManager->mResource->GetEmitterDrawSetting();

    int numTubeDivide = GetTubeDivide(rSetting);
    if (numTubeDivide < 3) {
        return;
    }

    Trigonometric* pTrig = static_cast<Trigonometric*>(
        __alloca((numTubeDivide + 1) * sizeof(Trigonometric)));

    if (pTrig == nullptr) {
        return;
    }

    f32 step = 360.0f / numTubeDivide;

    pTrig[0].cos = 1.0f;
    pTrig[0].sin = 0.0f;

    for (int i = 1; i < numTubeDivide; i++) {
        math::SinCosDeg(&pTrig[i].sin, &pTrig[i].cos, step * i);
    }

    pTrig[numTubeDivide].cos = 1.0f;
    pTrig[numTubeDivide].sin = 0.0f;

    pContext->mTrigonometric = pTrig;

    switch (GetStripeConnectType(rSetting)) {
    case STRIPE_CONNECT_NONE:
    default: {
        BeginTubeSide(GX_CULL_FRONT, flushGP, flags);
        DrawSmoothStripeTube_BSpline_ConnectNone(pContext, numDivide, flags);
        BeginTubeSide(GX_CULL_BACK, flushGP, flags);
        DrawSmoothStripeTube_BSpline_ConnectNone(pContext, numDivide, flags);
        break;
    }

    case STRIPE_CONNECT_RING: {
        BeginTubeSide(GX_CULL_FRONT, flushGP, flags);
        DrawSmoothStripeTube_BSpline_ConnectRing(pContext, numDivide, flags);
        BeginTubeSide(GX_CULL_BACK, flushGP, flags);
        DrawSmoothStripeTube_BSpline_ConnectRing(pContext, numDivide, flags);
        break;
    }

    case STRIPE_CONNECT_EMITTER: {
        BeginTubeSide(GX_CULL_FRONT, flushGP, flags);
        DrawSmoothStripeTube_BSpline_ConnectEmitter(pContext, numDivide,
                                                    flags);
        BeginTubeSide(GX_CULL_BACK, flushGP, flags);
        DrawSmoothStripeTube_BSpline_ConnectEmitter(pContext, numDivide,
                                                    flags);
        break;
    }
    }
}

void DrawSmoothStripeStrategy::DrawSmoothStripe(AheadContextStripe* pContext,
                                                int numDivide, int flags,
                                                const math::VEC3& rEdge0,
                                                const math::VEC3& rEdge1) {
    switch (GetStripeConnectType(*pContext->mCommon.mParticleManager->mResource
                                      ->GetEmitterDrawSetting())) {
    case STRIPE_CONNECT_NONE:
    default: {
        DrawSmoothStripe_BSpline_ConnectNone(pContext, numDivide, flags,
                                             rEdge0, rEdge1);
        break;
    }

    case STRIPE_CONNECT_RING: {
        DrawSmoothStripe_BSpline_ConnectRing(pContext, numDivide, flags,
                                             rEdge0, rEdge1);
        break;
    }

    case STRIPE_CONNECT_EMITTER: {
        DrawSmoothStripe_BSpline_ConnectEmitter(pContext, numDivide, flags,
                                                rEdge0, rEdge1);
        break;
    }
    }
}

void DrawSmoothStripeStrategy::InitGraphics(const DrawInfo& rInfo,
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

void DrawSmoothStripeStrategy::Draw(const DrawInfo& rInfo,
                                    ParticleManager* pManager) {
    const EmitterDrawSetting& rSetting =
        *pManager->mResource->GetEmitterDrawSetting();
    int numParticle = GetNumDrawParticle(pManager);

    if (numParticle == 0) {
        return;
    }

    math::MTX34 viewMtx;
    rInfo.GetViewMtxCooked(&viewMtx);

    AheadContextStripe context(viewMtx, pManager);

    math::VEC3 prevAxis = GetInitialPrevAxis(rSetting, &context);

    for (Particle* pParticle = GetYoungestParticle(pManager);
         pParticle != nullptr;
         pParticle = GetElderParticle(pManager, pParticle)) {

        if (pParticle->mPrevAxis.x <= 1.0f) {
            break;
        }

        pParticle->mPrevAxis = prevAxis;
    }

    int flags = (GetDrawOrder(rSetting) != 0 ? DRAWFLAG_REVERSE : 0) |
                (rSetting.typeOption == EmitterDrawSetting::ASSIST_ST_BILLBOARD
                     ? DRAWFLAG_VIEW_AXIS
                     : 0);

    int drawOrder = GetDrawOrder(rSetting);

    math::MTX34 mtx;
    rInfo.GetViewMtxCooked(&mtx);
    math::MTX34Mult(&mtx, &mtx, &context.mCommon.mParticleManagerMtx);

    if (flags & DRAWFLAG_VIEW_AXIS) {
        math::MTX34 invMtx;

        if (math::MTX34Inv(&invMtx, &mtx)) {
            context.mViewAxisZ = math::VEC3(invMtx._02, invMtx._12, invMtx._22);
        } else {
            context.mViewAxisZ = math::VEC3(0.0f, 0.0f, 1.0f);
        }
    }

    int connectType = GetStripeConnectType(rSetting);

    if ((connectType == STRIPE_CONNECT_RING && numParticle < 3) ||
        (connectType == STRIPE_CONNECT_NONE && numParticle < 2)) {

        CalcAheadStripeFunc pCalcAheadFunc =
            reinterpret_cast<CalcAheadStripeFunc>(GetCalcAheadFunc(pManager));

        GetFirstDrawParticleFunc pGetFirstFunc =
            GetGetFirstDrawParticleFunc(drawOrder);
        GetNextDrawParticleFunc pGetNextFunc =
            GetGetNextDrawParticleFunc(drawOrder);

        for (Particle* pParticle = pGetFirstFunc(pManager);
             pParticle != nullptr;
             pParticle = pGetNextFunc(pManager, pParticle)) {

            math::VEC3 axisY;
            pCalcAheadFunc(&axisY, &context, pParticle);
            CalcAxisMtx(&context, flags, pParticle, axisY,
                        pParticle->mParameter.mPosition);
        }

        return;
    }

    InitGraphics(rInfo, pManager);
    GXLoadPosMtxImm(mtx, GX_PNMTX0);

    flags |= mNumTexmap != 0 ? DRAWFLAG_TEXCOORD : 0;

    Particle* pFirst =
        GetGetFirstDrawParticleFunc(GetDrawOrder(rSetting))(pManager);
    SetupGP(pFirst, rSetting, rInfo, true, false);

    int numDivide = rSetting.typeOption1;
    if (numDivide == 0) {
        numDivide = 1;
    }

    static const math::VEC3 sEdgeX0(1.0f, 0.0f, 0.0f);
    static const math::VEC3 sEdgeX1(-1.0f, 0.0f, 0.0f);
    static const math::VEC3 sEdgeZ0(0.0f, 0.0f, 1.0f);
    static const math::VEC3 sEdgeZ1(0.0f, 0.0f, -1.0f);

    switch (rSetting.typeOption) {
    default: {
        DrawSmoothStripe(&context, numDivide, flags, sEdgeX0, sEdgeX1);

        if (rSetting.typeOption == EmitterDrawSetting::ASSIST_ST_CROSS) {
            DrawSmoothStripe(&context, numDivide, flags, sEdgeZ0, sEdgeZ1);
        }
        break;
    }

    case EmitterDrawSetting::ASSIST_ST_TUBE: {
        bool flushGP = false;
        if ((rSetting.mFlags & EmitterDrawSetting::FLAG_CLIP_DISABLE) ||
            !pFirst->mParticleManager->mManagerEM->mManagerEF->mManagerES
                 ->mXFFlushSafe) {
            flushGP = true;
        }

        DrawSmoothStripeTube(&context, numDivide, flags, flushGP);
        break;
    }
    }
}

math::VEC3 DrawSmoothStripeStrategy::GetInitialPrevAxis(
    const EmitterDrawSetting& rSetting, AheadContextStripe* pContext) {

    math::VEC3 prevAxis;

    if (rSetting.typeDir == EmitterDrawSetting::DIR_7) {
        const math::MTX34& rInvMtx = pContext->mCommon.mParticleManagerMtxInv;

        switch (GetPrevAxisType(rSetting)) {
        case PREVAXIS_X: {
            prevAxis = math::VEC3(rInvMtx._00, rInvMtx._10, rInvMtx._20);
            break;
        }

        default: {
            prevAxis = math::VEC3(rInvMtx._01, rInvMtx._11, rInvMtx._21);
            break;
        }

        case PREVAXIS_Z: {
            prevAxis = math::VEC3(rInvMtx._02, rInvMtx._12, rInvMtx._22);
            break;
        }

        case PREVAXIS_XYZ: {
            prevAxis = math::VEC3(rInvMtx._00, rInvMtx._10, rInvMtx._20) +
                       math::VEC3(rInvMtx._01, rInvMtx._11, rInvMtx._21) +
                       math::VEC3(rInvMtx._02, rInvMtx._12, rInvMtx._22);
            break;
        }
        }
    } else {
        switch (GetPrevAxisType(rSetting)) {
        case PREVAXIS_X: {
            math::VEC3TransformNormal(&prevAxis, &pContext->mCommon.mEmitterMtx,
                                      &GetXUnitVec());
            math::VEC3TransformNormal(
                &prevAxis, &pContext->mCommon.mParticleManagerMtxInv,
                &prevAxis);
            break;
        }

        default: {
            prevAxis = pContext->mCommon.mEmitterAxisY;
            return prevAxis;
        }

        case PREVAXIS_Z: {
            math::VEC3TransformNormal(&prevAxis, &pContext->mCommon.mEmitterMtx,
                                      &GetZUnitVec());
            math::VEC3TransformNormal(
                &prevAxis, &pContext->mCommon.mParticleManagerMtxInv,
                &prevAxis);
            break;
        }

        case PREVAXIS_XYZ: {
            math::VEC3 axis(1.0f, 1.0f, 1.0f);
            math::VEC3TransformNormal(&prevAxis, &pContext->mCommon.mEmitterMtx,
                                      &axis);
            math::VEC3TransformNormal(
                &prevAxis, &pContext->mCommon.mParticleManagerMtxInv,
                &prevAxis);
            break;
        }
        }
    }

    if (!Normalize(&prevAxis)) {
        prevAxis = pContext->mCommon.mEmitterAxisY;
    }

    return prevAxis;
}

DrawStrategyImpl::CalcAheadFunc
DrawSmoothStripeStrategy::GetCalcAheadFunc(ParticleManager* pManager) {
    switch (pManager->mResource->GetEmitterDrawSetting()->typeDir) {
    case EmitterDrawSetting::AHEAD_ST_SPEED: {
        return CalcAhead_Speed;
    }

    case EmitterDrawSetting::AHEAD_ST_EMITTER_CENTER: {
        return CalcAhead_EmitterCenter;
    }

    case EmitterDrawSetting::AHEAD_ST_EMITTER_DESIGN: {
        return CalcAhead_EmitterDesign;
    }

    case EmitterDrawSetting::AHEAD_ST_PARTICLE: {
        return CalcAhead_Particle_Stripe;
    }

    case EmitterDrawSetting::AHEAD_CMN_NODESIGN:
    case EmitterDrawSetting::DIR_7: {
        return CalcAhead_NoDesign;
    }

    case EmitterDrawSetting::AHEAD_ST_PARTICLE_BOTH: {
        switch (GetStripeConnectType(
            *pManager->mResource->GetEmitterDrawSetting())) {
        case STRIPE_CONNECT_RING: {
            return CalcAhead_ParticleBoth_Ring;
        }

        case STRIPE_CONNECT_EMITTER: {
            return CalcAhead_ParticleBoth_Origin;
        }

        default: {
            return CalcAhead_ParticleBoth_Stripe;
        }
        }
    }

    default: {
        return CalcAhead_Speed;
    }
    }
}

void DrawSmoothStripeStrategy::CalcAhead_Particle_Stripe(
    math::VEC3* pAxisY, AheadContext* pContext, Particle* pParticle) {

    Particle* pYounger =
        GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    if (pYounger != nullptr) {
        math::VEC3Sub(pAxisY, &pParticle->mParameter.mPosition,
                      &pYounger->mParameter.mPosition);
    } else {
        math::VEC3Sub(pAxisY, &pParticle->mParameter.mPosition,
                      &pContext->mCommon.mEmitterCenter);
    }

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

void DrawSmoothStripeStrategy::CalcAhead_ParticleBoth_Stripe(
    math::VEC3* pAxisY, AheadContext* pContext, Particle* pParticle) {

    Particle* pElder =
        GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    Particle* pYounger =
        GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    math::VEC3 elderDir(0.0f, 0.0f, 0.0f);

    if (pElder != nullptr) {
        math::VEC3Sub(&elderDir, &pElder->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);

        if (!Normalize(&elderDir)) {
            elderDir = math::VEC3(0.0f, 0.0f, 0.0f);
        }
    }

    math::VEC3 youngerDir(0.0f, 0.0f, 0.0f);

    if (pYounger != nullptr) {
        math::VEC3Sub(&youngerDir, &pYounger->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);

        if (!Normalize(&youngerDir)) {
            youngerDir = math::VEC3(0.0f, 0.0f, 0.0f);
        }
    }

    math::VEC3Sub(pAxisY, &elderDir, &youngerDir);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

void DrawSmoothStripeStrategy::CalcAhead_ParticleBoth_Ring(
    math::VEC3* pAxisY, AheadContext* pContext, Particle* pParticle) {

    Particle* pElder =
        GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    if (pElder == nullptr) {
        pElder = GetYoungestDrawParticle(pContext->mCommon.mParticleManager);
    }

    Particle* pYounger =
        GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    if (pYounger == nullptr) {
        pYounger = GetOldestDrawParticle(pContext->mCommon.mParticleManager);
    }

    math::VEC3 elderDir;
    math::VEC3Sub(&elderDir, &pElder->mParameter.mPosition,
                  &pParticle->mParameter.mPosition);

    if (!Normalize(&elderDir)) {
        elderDir = math::VEC3(0.0f, 0.0f, 0.0f);
    }

    math::VEC3 youngerDir;
    math::VEC3Sub(&youngerDir, &pYounger->mParameter.mPosition,
                  &pParticle->mParameter.mPosition);

    if (!Normalize(&youngerDir)) {
        youngerDir = math::VEC3(0.0f, 0.0f, 0.0f);
    }

    math::VEC3Sub(pAxisY, &elderDir, &youngerDir);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

void DrawSmoothStripeStrategy::CalcAhead_ParticleBoth_Origin(
    math::VEC3* pAxisY, AheadContext* pContext, Particle* pParticle) {

    Particle* pElder =
        GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    Particle* pYounger =
        GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    math::VEC3 elderDir(0.0f, 0.0f, 0.0f);

    if (pElder != nullptr) {
        math::VEC3Sub(&elderDir, &pElder->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);
        Normalize(&elderDir);
    }

    math::VEC3 youngerDir;

    if (pYounger != nullptr) {
        math::VEC3Sub(&youngerDir, &pYounger->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);
    } else {
        math::VEC3Sub(&youngerDir, &pContext->mCommon.mEmitterCenter,
                      &pParticle->mParameter.mPosition);
    }

    Normalize(&youngerDir);

    math::VEC3Sub(pAxisY, &elderDir, &youngerDir);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

}
}
