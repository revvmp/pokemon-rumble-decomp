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

    enum StripeTexmapType {
        STRIPE_TEXMAP_STRETCH = (0 << 6),
        STRIPE_TEXMAP_REPEAT = (1 << 6)
    };

    struct AheadContextStripe : AheadContext {
        math::VEC3 mEmitterAxisX;
        math::VEC3 mViewAxisZ;
    };

    typedef void (*CalcAheadStripeFunc)(math::VEC3* pAxisY,
                                        AheadContextStripe* pContext,
                                        Particle* pParticle);

    struct Vertex {
        math::VEC3 pos0;
        math::VEC3 pos1;
        f32 texT;

        Vertex() {}
        Vertex(const math::VEC3& rPos0, const math::VEC3& rPos1, f32 _texT)
            : pos0(rPos0), pos1(rPos1), texT(_texT) {}
    };

    struct VertexTube {
        math::VEC3 pos;
        math::VEC3 axisX;
        math::VEC3 axisZ;
        f32 texT;

        VertexTube() {}
    };

    struct Trigonometric {
        f32 cos;
        f32 sin;
    };

public:
    DrawSmoothStripeStrategy();

    virtual void Draw(const DrawInfo& rInfo, ParticleManager* pManager);
    virtual CalcAheadFunc GetCalcAheadFunc(ParticleManager* pManager);

private:
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

    int GetDrawOrder(const EmitterDrawSetting& rSetting) const {
        return rSetting.mFlags & EmitterDrawSetting::FLAG_DRAW_ORDER;
    }

    int GetStripeTexmapType(const EmitterDrawSetting& rSetting) const {
        return rSetting.typeOption2 & 0xC0;
    }
};

}
}

#endif
