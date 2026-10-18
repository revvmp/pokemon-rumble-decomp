#ifndef NW4R_EF_DRAW_STRATEGY_DRAWSMOOTHSTRIPESTRATEGY_H
#define NW4R_EF_DRAW_STRATEGY_DRAWSMOOTHSTRIPESTRATEGY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r {
namespace ef {

class DrawSmoothStripeStrategy : public DrawStrategyImpl {
public:
    enum DrawFlag {
        DRAWFLAG_TEXCOORD = (1 << 0),
        DRAWFLAG_VIEW_AXIS = (1 << 3),
        DRAWFLAG_REVERSE = (1 << 4)
    };

    enum StripeConnectType {
        STRIPE_CONNECT_NONE,
        STRIPE_CONNECT_RING,
        STRIPE_CONNECT_EMITTER
    };

    enum PrevAxisType {
        PREVAXIS_Y = (0 << 3),
        PREVAXIS_X = (1 << 3),
        PREVAXIS_Z = (2 << 3),
        PREVAXIS_XYZ = (3 << 3)
    };

    enum StripeTexmapType {
        STRIPE_TEXMAP_STRETCH = (0 << 6),
        STRIPE_TEXMAP_REPEAT = (1 << 6)
    };

    struct Trigonometric {
        f32 cos;
        f32 sin;
    };

    struct AheadContextStripe : AheadContext {
        math::VEC3 mEmitterAxisX;
        math::VEC3 mViewAxisZ;
        const Trigonometric* mTrigonometric;

        AheadContextStripe(const math::MTX34& rViewMtx,
                           ParticleManager* pManager)
            : AheadContext(rViewMtx, pManager) {

            math::VEC3 axisX(mCommon.mEmitterMtx._00, mCommon.mEmitterMtx._10,
                             mCommon.mEmitterMtx._20);

            math::VEC3TransformNormal(&mEmitterAxisX,
                                      &mCommon.mParticleManagerMtxInv, &axisX);
        }
    };

    typedef void (*CalcAheadStripeFunc)(math::VEC3* pAxisY,
                                        AheadContextStripe* pContext,
                                        Particle* pParticle);

    struct Vertex {
        math::VEC3 pos0;
        math::VEC3 pos1;
        f32 texT;

        Vertex();
        Vertex(const math::VEC3& rPos0, const math::VEC3& rPos1, f32 _texT)
            : pos0(rPos0), pos1(rPos1), texT(_texT) {}
    };

    struct VertexTube {
        math::VEC3 pos;
        math::VEC3 axisX;
        math::VEC3 axisZ;
        f32 texT;

        VertexTube() {}
        VertexTube(const math::VEC3& rPos, const math::VEC3& rAxisX,
                   const math::VEC3& rAxisZ, f32 _texT)
            : pos(rPos), axisX(rAxisX), axisZ(rAxisZ), texT(_texT) {}
    };

public:
    DrawSmoothStripeStrategy();

    virtual void Draw(const DrawInfo& rInfo, ParticleManager* pManager);
    virtual CalcAheadFunc GetCalcAheadFunc(ParticleManager* pManager);

private:
    inline void InitGraphics(const DrawInfo& rInfo, ParticleManager* pManager);

    inline math::MTX34 CalcAxisMtx(AheadContextStripe* pContext, int flags,
                                   Particle* pParticle,
                                   const math::VEC3& rAxisY,
                                   const math::VEC3& rPos);

    inline math::MTX34 CalcRotateMtx(Particle* pParticle, f32 pivot);

    void CalcVertex(Vertex* pVertex, AheadContextStripe* pContext, int flags,
                    Particle* pParticle, CalcAheadStripeFunc pCalcAheadFunc,
                    const math::VEC3& rPos, const math::VEC3& rEdge0,
                    const math::VEC3& rEdge1, f32 pivot, f32 texT);

    void CalcVertexTube(VertexTube* pVertex, AheadContextStripe* pContext,
                        int flags, Particle* pParticle,
                        CalcAheadStripeFunc pCalcAheadFunc, f32 pivotX,
                        f32 pivotY, f32 texT);

    void DrawSection_BSpline(int numDivide, const Vertex& rV0,
                             const Vertex& rV1, const Vertex& rV2,
                             int flags);

    void DrawSectionEnd_BSpline(const Vertex& rV0, const Vertex& rV1,
                                const Vertex& rV2, int flags);

    void DrawSectionTube_BSpline(int numDivide, int numTubeDivide,
                                 const VertexTube& rV0, const VertexTube& rV1,
                                 const VertexTube& rV2, int flags,
                                 const Trigonometric* pTrig);

    void DrawSmoothStripe_BSpline_ConnectNone(AheadContextStripe* pContext,
                                              int numDivide, int flags,
                                              const math::VEC3& rEdge0,
                                              const math::VEC3& rEdge1);

    void DrawSmoothStripe_BSpline_ConnectRing(AheadContextStripe* pContext,
                                              int numDivide, int flags,
                                              const math::VEC3& rEdge0,
                                              const math::VEC3& rEdge1);

    void DrawSmoothStripe_BSpline_ConnectEmitter(AheadContextStripe* pContext,
                                                 int numDivide, int flags,
                                                 const math::VEC3& rEdge0,
                                                 const math::VEC3& rEdge1);

    void DrawSmoothStripeTube_BSpline_ConnectNone(AheadContextStripe* pContext,
                                                  int numDivide, int flags);

    void DrawSmoothStripeTube_BSpline_ConnectRing(AheadContextStripe* pContext,
                                                  int numDivide, int flags);

    void DrawSmoothStripeTube_BSpline_ConnectEmitter(
        AheadContextStripe* pContext, int numDivide, int flags);

    void DrawSmoothStripeTube(AheadContextStripe* pContext, int numDivide,
                              int flags, bool flushGP);

    inline void DrawSmoothStripe(AheadContextStripe* pContext, int numDivide,
                                 int flags, const math::VEC3& rEdge0,
                                 const math::VEC3& rEdge1);

    math::VEC3 GetInitialPrevAxis(const EmitterDrawSetting& rSetting,
                                  AheadContextStripe* pContext);

    static void CalcAhead_Particle_Stripe(math::VEC3* pAxisY,
                                          AheadContext* pContext,
                                          Particle* pParticle);

    static void CalcAhead_ParticleBoth_Stripe(math::VEC3* pAxisY,
                                              AheadContext* pContext,
                                              Particle* pParticle);

    static void CalcAhead_ParticleBoth_Ring(math::VEC3* pAxisY,
                                            AheadContext* pContext,
                                            Particle* pParticle);

    static void CalcAhead_ParticleBoth_Origin(math::VEC3* pAxisY,
                                              AheadContext* pContext,
                                              Particle* pParticle);

    int GetDrawOrder(const EmitterDrawSetting& rSetting) const {
        return rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER;
    }

    int GetStripeTexmapType(const EmitterDrawSetting& rSetting) const {
        return rSetting.typeOption2 & 0xC0;
    }

    int GetTubeDivide(const EmitterDrawSetting& rSetting) const {
        return rSetting.typeOption0;
    }

    int GetStripeConnectType(const EmitterDrawSetting& rSetting) const {
        return rSetting.typeOption2 & 7;
    }

    int GetPrevAxisType(const EmitterDrawSetting& rSetting) const {
        return rSetting.typeOption2 & 0x38;
    }
};

}
}

#endif
