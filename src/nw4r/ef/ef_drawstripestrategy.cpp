#include <nw4r/ef/drawstrategy/ef_drawstripestrategy.h>
#include <nw4r/ef/ef_drawinfo.h>
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/ef_effect.h>
#include <nw4r/ef/ef_effectsystem.h>
#include <nw4r/ef/ef_resource.h>
#include <nw4r/ef/ef_util.h>
#include <revolution/gx/GXTransform.h>
#include <revolution/gx/GXVert.h>
#include <nw4r/math/math_triangular.h>

#include <revolution/GX.h>

namespace nw4r {
namespace ef {
static inline void GXPosition(const math::VEC3& rPos) {
    GXPosition3f32(rPos.x, rPos.y, rPos.z);
}

static inline void DrawVertexPair(const math::VEC3& rFirst,
                                  const math::VEC3& rSecond,
                                  f32 texCoord, int flags) {
    GXPosition(rFirst);
    if (flags & 1) {
        GXTexCoord2f32(1.0f, texCoord);
    }
    GXPosition(rSecond);
    if (flags & 1) {
        GXTexCoord2f32(0.0f, texCoord);
    }
}

DrawStripeStrategy::DrawStripeStrategy() {}
inline math::MTX34
DrawStripeStrategy::CalcAxisMtx(AheadContextStripe* pContext, int flags, Particle* pParticle,
                               const math::VEC3& rDir, const math::VEC3& rPos) {
    math::VEC3 axisX;
    math::VEC3 axisZ;

    if (!(flags & 8)) {
        math::VEC3Cross(&axisX, &rDir, &pParticle->mPrevAxis);
    } else {
        math::VEC3Cross(&axisX, &rDir, &pContext->mViewAxisZ);
    }

    if (!Normalize(&axisX)) {
        axisX = pContext->mEmitterAxisX;
    }

    math::VEC3Cross(&axisZ, &axisX, &rDir);
    Normalize(&axisZ);

    pParticle->mPrevAxis = axisZ;


    return math::MTX34(
        axisX.x, rDir.x, axisZ.x, rPos.x,
        axisX.y, rDir.y, axisZ.y, rPos.y,
        axisX.z, rDir.z, axisZ.z, rPos.z);

}


inline math::MTX34 DrawStripeStrategy::CalcRotateMtx(Particle* pParticle,
                                                  f32 pivot) {
    math::VEC3 rot;
    pParticle->Draw_GetRotate(&rot);

    f32 sx = pParticle->Draw_GetSizeX();

    f32 c, s;
    PSSinCosRad(&s, &c, rot.y);
    s *= sx;
    c *= sx;


    return math::MTX34(
           c, 0.0f,   -s, pivot - c * pivot,
        0.0f, 1.0f, 0.0f, 0.0f,
           s, 0.0f,    c, -s * pivot);

}

void DrawStripeStrategy::DrawStripe(AheadContextStripe* pContext, int flags,
                                    const math::VEC3& rRight,
                                    const math::VEC3& rLeft) {

    ParticleManager* pManager = pContext->mCommon.mParticleManager;
    const EmitterDrawSetting& rSetting = *pManager->mResource->GetEmitterDrawSetting();

    CalcAheadStripeFunc pCalcAheadFunc = reinterpret_cast<CalcAheadStripeFunc>(GetCalcAheadFunc(pManager));

    int drawOrder = rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER;
    GetFirstDrawParticleFunc pGetFirstFunc =
        GetGetFirstDrawParticleFunc(drawOrder);
    GetNextDrawParticleFunc pGetNextFunc = GetGetNextDrawParticleFunc(drawOrder);

    int num = GetNumDrawParticle(pManager);
    f32 pivot = 0.01f * rSetting.pivotX;


    bool loop = (rSetting.typeOption2 & 7) == EmitterDrawSetting::ASSIST_ST_CROSS;
    bool center =
        (rSetting.typeOption2 & 7) == EmitterDrawSetting::ASSIST_ST_BILLBOARD;

    if (loop || center) {
        num++;
    }


    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, num * 2);

    f32 step;
    if (GetStripeTexmapType(rSetting) == 0x40) {
        step = 1.0f;
    } else {
        step = math::FInv(num - 1);
    }

    int idx;
    int dir;

    switch (rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER) {
    case 0: {
        idx = 0;
        dir = 1;
        break;
    }

    case 1:
    default: {
        idx = num - 1;
        dir = -1;
        break;
    }
    }

    if (center && drawOrder == 0) {
        DrawParticle(pContext, flags, nullptr, pCalcAheadFunc,
                   pContext->mCommon.mEmitterCenter, rRight, rLeft,
                   pivot, step * idx);
        idx += dir;
    }

    for (Particle* pIt = pGetFirstFunc(pManager); pIt != nullptr;
         pIt = pGetNextFunc(pManager, pIt), idx += dir) {

        DrawParticle(pContext, flags, pIt, pCalcAheadFunc, pIt->mParameter.mPosition,
                   rRight, rLeft, pivot, step * idx);
    }

    if (loop) {
        Particle* pFirst = pGetFirstFunc(pManager);

        DrawParticle(pContext, flags, pFirst, pCalcAheadFunc,
                   pFirst->mParameter.mPosition, rRight, rLeft,
                   pivot, step * idx);
    }

    if (center && drawOrder != 0) {
        DrawParticle(pContext, flags, nullptr, pCalcAheadFunc,
                   pContext->mCommon.mEmitterCenter, rRight, rLeft,
                   pivot, step * idx);
    }

    GXEnd();
}

void DrawStripeStrategy::DrawParticle(
    AheadContextStripe* pContext, int flags, Particle* pParticle, CalcAheadStripeFunc pCalcAheadFunc,
    const math::VEC3& rPos, const math::VEC3& rRight, const math::VEC3& rLeft,
    f32 pivot, f32 texV) {

    if (pParticle == nullptr) {
        pParticle = GetYoungestDrawParticle(pContext->mCommon.mParticleManager);
    }

    math::MTX34 drawMtx;
    math::VEC3 dir;
    math::VEC3 p0;
    math::VEC3 p1;

    pCalcAheadFunc(&dir, pContext, pParticle);

    math::MTX34 dirMtx = CalcAxisMtx(pContext, flags, pParticle, dir, rPos);
    math::MTX34 locMtx = CalcRotateMtx(pParticle, pivot);

    math::MTX34Mult(&drawMtx, &dirMtx, &locMtx);

    math::VEC3Transform(&p0, &drawMtx, &rRight);
    math::VEC3Transform(&p1, &drawMtx, &rLeft);

    DrawVertexPair(p0, p1, texV, flags);
}


void DrawStripeStrategy::CalcVertexTube(VertexTube* pDst,
    AheadContextStripe* pContext, int flags, Particle* pParticle,
    CalcAheadStripeFunc pCalcAheadFunc, f32 pivotX, f32 pivotY, f32 texCoord) {
    math::VEC3 ahead;
    pCalcAheadFunc(&ahead, pContext, pParticle);
    math::MTX34 frame = CalcAxisMtx(pContext, flags, pParticle,
                                     ahead, pParticle->mParameter.mPosition);
    math::VEC3 rotate;
    pParticle->Draw_GetRotate(&rotate);
    f32 scaleX = pParticle->Draw_GetSizeX();
    f32 scaleY = pParticle->Draw_GetSizeY();
    f32 s, c;
    PSSinCosRad(&s, &c, rotate.y);
    math::MTX34 local(c * scaleX, 0.0f, -s * scaleY,
        (pivotX - pivotX * (c * scaleX)) + pivotY * (s * scaleY),
        0.0f, 1.0f, 0.0f, 0.0f,
        s * scaleX, 0.0f, c * scaleY,
        (pivotY - pivotX * (s * scaleX)) - pivotY * (c * scaleY));
    math::MTX34 mtx;
    math::MTX34Mult(&mtx, &frame, &local);
    pDst->mCenter = math::VEC3(mtx._03, mtx._13, mtx._23);
    math::VEC3TransformNormal(&pDst->mAxisX, &mtx, &GetXUnitVec());
    math::VEC3TransformNormal(&pDst->mAxisZ, &mtx, &GetZUnitVec());
    pDst->mTexCoord = texCoord;
}

void DrawStripeStrategy::DrawTubeSegment(const VertexTube& rFirst,
    const VertexTube& rSecond, int flags, int divide, const f32* pTrig) {
    f32 inv = math::FInv(divide);
    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, divide * 2 + 2);
    if (flags & 16) {
        for (int i = 0; i <= divide; pTrig += 2, i++) {
            f32 s = pTrig[0];
            f32 c = pTrig[1];
            GXPosition(rFirst.mAxisX * c + rFirst.mAxisZ * s + rFirst.mCenter);
            if (flags & 1) {
                GXTexCoord2f32(inv * i, rFirst.mTexCoord);
            }
            GXPosition(rSecond.mAxisX * c + rSecond.mAxisZ * s + rSecond.mCenter);
            if (flags & 1) {
                GXTexCoord2f32(inv * i, rSecond.mTexCoord);
            }
        }
    } else {
        for (int i = 0; i <= divide; pTrig += 2, i++) {
            f32 s = pTrig[0];
            f32 c = pTrig[1];
            GXPosition(rSecond.mAxisX * c + rSecond.mAxisZ * s + rSecond.mCenter);
            if (flags & 1) {
                GXTexCoord2f32(inv * i, rSecond.mTexCoord);
            }
            GXPosition(rFirst.mAxisX * c + rFirst.mAxisZ * s + rFirst.mCenter);
            if (flags & 1) {
                GXTexCoord2f32(inv * i, rFirst.mTexCoord);
            }
        }
    }
    GXEnd();
}

void DrawStripeStrategy::DrawStripeTube_ConnectNone(AheadContextStripe* pContext, int flags) {
    ParticleManager* pManager = pContext->mCommon.mParticleManager;
    const EmitterDrawSetting& rSetting = *pManager->mResource->GetEmitterDrawSetting();
    CalcAheadStripeFunc pCalcAheadFunc = reinterpret_cast<CalcAheadStripeFunc>(GetCalcAheadFunc(pManager));
    int drawOrder = rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER;
    GetFirstDrawParticleFunc pGetFirst = GetGetFirstDrawParticleFunc(drawOrder);
    GetNextDrawParticleFunc pGetNext = GetGetNextDrawParticleFunc(drawOrder);
    f32 pivotX = 0.01f * rSetting.pivotX;
    f32 pivotY = 0.01f * rSetting.pivotY;
    int count = GetNumDrawParticle(pManager);
    f32 step;
    if (GetStripeTexmapType(rSetting) != 0x40) {
        step = math::FInv(count - 1);
    } else {
        step = 1.0f;
    }
    int index = 0;
    int dir = 1;
    if (drawOrder != 0) {
        index = count - 1;
        dir = -1;
    }
    VertexTube current;
    VertexTube previous;
    Particle* pParticle = pGetFirst(pManager);
    CalcVertexTube(&current, pContext, flags, pParticle, pCalcAheadFunc,
                   pivotX, pivotY, step * index);
    index += dir;
    for (pParticle = pGetNext(pManager, pParticle); pParticle != nullptr;
         pParticle = pGetNext(pManager, pParticle), index += dir) {
        previous = current;
        CalcVertexTube(&current, pContext, flags, pParticle, pCalcAheadFunc,
                       pivotX, pivotY, step * index);
        DrawTubeSegment(previous, current, flags, GetStripeTubeDivide(rSetting), pContext->mTrigTable);
    }
}

void DrawStripeStrategy::DrawStripeTube_ConnectRing(AheadContextStripe* pContext, int flags) {
    ParticleManager* pManager = pContext->mCommon.mParticleManager;
    const EmitterDrawSetting& rSetting = *pManager->mResource->GetEmitterDrawSetting();
    CalcAheadStripeFunc pCalcAheadFunc = reinterpret_cast<CalcAheadStripeFunc>(GetCalcAheadFunc(pManager));
    int drawOrder = rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER;
    GetFirstDrawParticleFunc pGetFirst = GetGetFirstDrawParticleFunc(drawOrder);
    GetNextDrawParticleFunc pGetNext = GetGetNextDrawParticleFunc(drawOrder);
    f32 pivotX = 0.01f * rSetting.pivotX;
    f32 pivotY = 0.01f * rSetting.pivotY;
    int count = GetNumDrawParticle(pManager);
    f32 step;
    if (GetStripeTexmapType(rSetting) != 0x40) {
        step = math::FInv(count);
    } else {
        step = 1.0f;
    }
    int index = 0;
    int dir = 1;
    if (drawOrder != 0) {
        index = count;
        dir = -1;
    }
    VertexTube current;
    VertexTube previous;
    VertexTube first;
    Particle* pParticle = pGetFirst(pManager);
    CalcVertexTube(&first, pContext, flags, pParticle, pCalcAheadFunc,
                   pivotX, pivotY, step * index);
    current = first;
    index += dir;
    for (pParticle = pGetNext(pManager, pParticle); pParticle != nullptr;
         pParticle = pGetNext(pManager, pParticle), index += dir) {
        previous = current;
        CalcVertexTube(&current, pContext, flags, pParticle, pCalcAheadFunc,
                       pivotX, pivotY, step * index);
        DrawTubeSegment(previous, current, flags, GetStripeTubeDivide(rSetting), pContext->mTrigTable);
    }
    first.mTexCoord = step * index;
    DrawTubeSegment(current, first, flags, GetStripeTubeDivide(rSetting), pContext->mTrigTable);
}

void DrawStripeStrategy::DrawStripeTube_ConnectEmitter(AheadContextStripe* pContext, int flags) {
    ParticleManager* pManager = pContext->mCommon.mParticleManager;
    const EmitterDrawSetting& rSetting = *pManager->mResource->GetEmitterDrawSetting();
    CalcAheadStripeFunc pCalcAheadFunc = reinterpret_cast<CalcAheadStripeFunc>(GetCalcAheadFunc(pManager));
    int drawOrder = rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER;
    GetFirstDrawParticleFunc pGetFirst = GetGetFirstDrawParticleFunc(drawOrder);
    GetNextDrawParticleFunc pGetNext = GetGetNextDrawParticleFunc(drawOrder);
    f32 pivotX = 0.01f * rSetting.pivotX;
    f32 pivotY = 0.01f * rSetting.pivotY;
    int count = GetNumDrawParticle(pManager);
    f32 step;
    if (GetStripeTexmapType(rSetting) != 0x40) {
        step = math::FInv(count);
    } else {
        step = 1.0f;
    }
    int index = 0;
    int dir = 1;
    if (drawOrder != 0) {
        index = count;
        dir = -1;
    }
    VertexTube current;
    VertexTube previous;
    Particle* pParticle = pGetFirst(pManager);
    CalcVertexTube(&previous, pContext, flags, pParticle, pCalcAheadFunc,
                   pivotX, pivotY, step * index);
    index += dir;
    current = previous;
    current.mTexCoord = step * index;
    Particle* pYoungest = GetYoungestDrawParticle(pManager);
    math::VEC3 offset = pContext->mCommon.mEmitterCenter - pYoungest->mParameter.mPosition;
    previous.mCenter += offset;
    index += dir;
    DrawTubeSegment(previous, current, flags, GetStripeTubeDivide(rSetting), pContext->mTrigTable);
    for (pParticle = pGetNext(pManager, pParticle); pParticle != nullptr;
         pParticle = pGetNext(pManager, pParticle), index += dir) {
        previous = current;
        CalcVertexTube(&current, pContext, flags, pParticle, pCalcAheadFunc,
                       pivotX, pivotY, step * index);
        DrawTubeSegment(previous, current, flags, GetStripeTubeDivide(rSetting), pContext->mTrigTable);
    }
}

inline void DrawStripeStrategy::BeginDrawSide(GXCullMode cull, bool flush, int flags) {
    GXSetCullMode(cull);
    if (flush) {
        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, 8);
        for (int i = 0; i < 8; i++) {
            GXPosition3f32(0.0f, 0.0f, 0.0f);
            if (flags & 1) {
                GXTexCoord2f32(0.0f, 0.0f);
            }
        }
        GXEnd();
    }
}

void DrawStripeStrategy::DrawStripeTube(AheadContextStripe* pContext, int flags, bool flush) {
    const EmitterDrawSetting& rSetting = *pContext->mCommon.mParticleManager->mResource->GetEmitterDrawSetting();
    f32* pPair;
    f32* pTrig;
    int divide = GetStripeTubeDivide(rSetting);
    if (divide >= 3) {
        pTrig = static_cast<f32*>(__alloca((divide + 1) * sizeof(f32) * 2));
        if (pTrig != nullptr) {
            f32 step = 360.0f / divide;
            pTrig[1] = 1.0f;
            pTrig[0] = 0.0f;
            int i;
            for (pPair = pTrig + 2, i = 1; i < divide; pPair += 2, i++) {
                math::SinCosDeg(&pPair[0], &pPair[1], step * i);
            }
            (pTrig + 2 * divide)[1] = 1.0f;
            (pTrig + 2 * divide)[0] = 0.0f;
            pContext->mTrigTable = pTrig;
            switch (rSetting.typeOption2 & 7) {
            default:
                BeginDrawSide(GX_CULL_FRONT, flush, flags);
                DrawStripeTube_ConnectNone(pContext, flags);
                BeginDrawSide(GX_CULL_BACK, flush, flags);
                DrawStripeTube_ConnectNone(pContext, flags);
                break;
            case 1:
                BeginDrawSide(GX_CULL_FRONT, flush, flags);
                DrawStripeTube_ConnectRing(pContext, flags);
                BeginDrawSide(GX_CULL_BACK, flush, flags);
                DrawStripeTube_ConnectRing(pContext, flags);
                break;
            case 2:
                BeginDrawSide(GX_CULL_FRONT, flush, flags);
                DrawStripeTube_ConnectEmitter(pContext, flags);
                BeginDrawSide(GX_CULL_BACK, flush, flags);
                DrawStripeTube_ConnectEmitter(pContext, flags);
                break;
            }
        }
    }
}


void DrawStripeStrategy::Draw(const DrawInfo& rInfo, ParticleManager* pManager) {
    const EmitterDrawSetting& rSetting = *pManager->mResource->GetEmitterDrawSetting();
    int count = GetNumDrawParticle(pManager);
    if (count != 0) {
        math::MTX34 viewMtx;
        rInfo.GetViewMtxCooked(&viewMtx);
        AheadContextStripe context(viewMtx, pManager);
        math::VEC3 initialAxis = GetInitialPrevAxis(rSetting, context);
        for (Particle* pIt = GetYoungestParticle(pManager); pIt != nullptr;
             pIt = GetElderParticle(pManager, pIt)) {
            if (pIt->mPrevAxis.x <= 1.0f) {
                break;
            }
            pIt->mPrevAxis = initialAxis;
        }
        int flags = (rSetting.typeOption == 2 ? 8 : 0)
                  | ((rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER) ? 16 : 0);
        math::MTX34 posMtx;
        rInfo.GetViewMtxCooked(&posMtx);
        math::MTX34Mult(&posMtx, &posMtx, &context.mCommon.mParticleManagerMtx);
        if (flags & 8) {
            math::MTX34 inv;
            if (math::MTX34Inv(&inv, &posMtx)) {
                context.mViewAxisZ = math::VEC3(inv._02, inv._12, inv._22);
            } else {
                context.mViewAxisZ = math::VEC3(0.0f, 0.0f, 1.0f);
            }
        }
        if (((rSetting.typeOption2 & 7) == 1 && count < 3)
            || ((rSetting.typeOption2 & 7) == 0 && count < 2)) {
            int drawOrder = rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER;
            CalcAheadStripeFunc pCalcAhead = reinterpret_cast<CalcAheadStripeFunc>(GetCalcAheadFunc(pManager));
            GetFirstDrawParticleFunc pGetFirst = GetGetFirstDrawParticleFunc(drawOrder);
            GetNextDrawParticleFunc pGetNext = GetGetNextDrawParticleFunc(drawOrder);
            for (Particle* pIt = pGetFirst(pManager); pIt != nullptr; pIt = pGetNext(pManager, pIt)) {
                math::VEC3 ahead;
                pCalcAhead(&ahead, &context, pIt);
                math::MTX34 frame = CalcAxisMtx(&context, flags, pIt, ahead, pIt->mParameter.mPosition);
            }
            return;
        }
        InitGraphics(rInfo, pManager);
        GXLoadPosMtxImm(posMtx, GX_PNMTX0);
        flags |= mNumTexmap != 0;
        Particle* pFirst = GetGetFirstDrawParticleFunc(rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER)(pManager);
        SetupGP(pFirst, rSetting, rInfo, true, false);
        switch (rSetting.typeOption) {
        default:
            DrawStripe(&context, flags, math::VEC3(1.0f, 0.0f, 0.0f), math::VEC3(-1.0f, 0.0f, 0.0f));
            if (rSetting.typeOption == 1) {
                DrawStripe(&context, flags, math::VEC3(0.0f, 0.0f, -1.0f), math::VEC3(0.0f, 0.0f, 1.0f));
            }
            break;
        case 3:
            bool flush = false;
            if ((rSetting.mFlags & EmitterDrawSetting::FLAG_CLIP_DISABLE)
                || !pFirst->mParticleManager->mManagerEM->mManagerEF->mManagerES->mXFFlushSafe) {
                flush = true;
            }
            DrawStripeTube(&context, flags, flush);
            break;
        }
    }
}

math::VEC3 DrawStripeStrategy::GetInitialPrevAxis(const EmitterDrawSetting& rSetting,
    const AheadContextStripe& rContext) {
    math::VEC3 axis;
    if (rSetting.typeDir == 7) {
        const math::MTX34& rMtx = rContext.mCommon.mParticleManagerMtxInv;
        switch (rSetting.typeOption2 & 0x38) {
        case 8:
            axis = math::VEC3(rMtx._00, rMtx._10, rMtx._20);
            break;
        default:
            axis = math::VEC3(rMtx._01, rMtx._11, rMtx._21);
            break;
        case 16:
            axis = math::VEC3(rMtx._02, rMtx._12, rMtx._22);
            break;
        case 24:
            axis = math::VEC3(rMtx._00, rMtx._10, rMtx._20)
                 + math::VEC3(rMtx._01, rMtx._11, rMtx._21)
                 + math::VEC3(rMtx._02, rMtx._12, rMtx._22);
            break;
        }
    } else {
        switch (rSetting.typeOption2 & 0x38) {
        case 8:
            math::VEC3TransformNormal(&axis, &rContext.mCommon.mEmitterMtx, &GetXUnitVec());
            math::VEC3TransformNormal(&axis, &rContext.mCommon.mParticleManagerMtxInv, &axis);
            break;
        default:
            axis = rContext.mCommon.mEmitterAxisY;
            return axis;
        case 16:
            math::VEC3TransformNormal(&axis, &rContext.mCommon.mEmitterMtx, &GetZUnitVec());
            math::VEC3TransformNormal(&axis, &rContext.mCommon.mParticleManagerMtxInv, &axis);
            break;
        case 24:
            math::VEC3 diagonal(1.0f, 1.0f, 1.0f);
            math::VEC3TransformNormal(&axis, &rContext.mCommon.mEmitterMtx, &diagonal);
            math::VEC3TransformNormal(&axis, &rContext.mCommon.mParticleManagerMtxInv, &axis);
            break;
        }
    }
    if (!Normalize(&axis)) {
        axis = rContext.mCommon.mEmitterAxisY;
    }
    return axis;
}

void DrawStripeStrategy::InitGraphics(const DrawInfo& rInfo, ParticleManager* pManager) {
    const EmitterDrawSetting& rSetting = *pManager->mResource->GetEmitterDrawSetting();
    Initialize(pManager, rSetting, rInfo);
    GXEnableTexOffsets(GX_TEXCOORD0, GX_TRUE, GX_TRUE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    if (mNumTexmap > 0) {
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    }
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetCurrentMtx(GX_PNMTX0);
}

DrawStrategyImpl::CalcAheadFunc
DrawStripeStrategy::GetCalcAheadFunc(ParticleManager* pManager) {
    EmitterDesc* pDesc = pManager->mResource->GetEmitterDesc();

    switch (pDesc->drawSetting.typeDir) {
    case 0: {
        return CalcAhead_Speed;
    }

    case 1: {
        return CalcAhead_EmitterCenter;
    }

    case 2: {
        return CalcAhead_EmitterDesign;
    }

    case 3: {
        return CalcAhead_Particle_Stripe;
    }

    case 5:
    case 7: {
        return CalcAhead_NoDesign;
    }

    case 6: {
        switch (pDesc->drawSetting.typeOption2 & 7) {
        case EmitterDrawSetting::ASSIST_ST_CROSS: {
            return CalcAhead_ParticleBoth_Ring;
        }

        case EmitterDrawSetting::ASSIST_ST_BILLBOARD: {
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

void DrawStripeStrategy::CalcAhead_Particle_Stripe(math::VEC3* pAxisY,
                                           AheadContext* pContext,
                                           Particle* pParticle) {

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

void DrawStripeStrategy::CalcAhead_ParticleBoth_Stripe(math::VEC3* pAxisY,
                                        AheadContext* pContext,
                                        Particle* pParticle) {

    Particle* pElder =
        GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    Particle* pYounger =
        GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    math::VEC3 elderPos(0.0f, 0.0f, 0.0f);

    if (pElder != nullptr) {
        math::VEC3Sub(&elderPos, &pElder->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);

        if (!Normalize(&elderPos)) {
            elderPos = math::VEC3(0.0f, 0.0f, 0.0f);
        }
    }

    math::VEC3 youngerPos(0.0f, 0.0f, 0.0f);

    if (pYounger != nullptr) {
        math::VEC3Sub(&youngerPos, &pYounger->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);

        if (!Normalize(&youngerPos)) {
            youngerPos = math::VEC3(0.0f, 0.0f, 0.0f);
        }
    }

    math::VEC3Sub(pAxisY, &elderPos, &youngerPos);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

void DrawStripeStrategy::CalcAhead_ParticleBoth_Ring(math::VEC3* pAxisY,
                                            AheadContext* pContext,
                                            Particle* pParticle) {

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

    math::VEC3 elderPos;
    math::VEC3Sub(&elderPos, &pElder->mParameter.mPosition,
                  &pParticle->mParameter.mPosition);

    if (!Normalize(&elderPos)) {
        elderPos = math::VEC3(0.0f, 0.0f, 0.0f);
    }

    math::VEC3 youngerPos;
    math::VEC3Sub(&youngerPos, &pYounger->mParameter.mPosition,
                  &pParticle->mParameter.mPosition);

    if (!Normalize(&youngerPos)) {
        youngerPos = math::VEC3(0.0f, 0.0f, 0.0f);
    }

    math::VEC3Sub(pAxisY, &elderPos, &youngerPos);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

void DrawStripeStrategy::CalcAhead_ParticleBoth_Origin(math::VEC3* pAxisY,
                                              AheadContext* pContext,
                                              Particle* pParticle) {

    Particle* pElder =
        GetElderDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    Particle* pYounger =
        GetYoungerDrawParticle(pContext->mCommon.mParticleManager, pParticle);

    math::VEC3 elderPos(0.0f, 0.0f, 0.0f);

    if (pElder != nullptr) {
        math::VEC3Sub(&elderPos, &pElder->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);
        Normalize(&elderPos);
    }

    math::VEC3 youngerPos;

    if (pYounger != nullptr) {
        math::VEC3Sub(&youngerPos, &pYounger->mParameter.mPosition,
                      &pParticle->mParameter.mPosition);
    } else {
        math::VEC3Sub(&youngerPos, &pContext->mCommon.mEmitterCenter,
                      &pParticle->mParameter.mPosition);
    }

    Normalize(&youngerPos);

    math::VEC3Sub(pAxisY, &elderPos, &youngerPos);

    if (!Normalize(pAxisY)) {
        *pAxisY = pContext->mCommon.mEmitterAxisY;
    }
}

}
}
