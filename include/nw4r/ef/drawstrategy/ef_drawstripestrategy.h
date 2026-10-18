#ifndef NW4R_EF_DRAW_STRATEGY_DRAWSTRIPESTRATEGY_H
#define NW4R_EF_DRAW_STRATEGY_DRAWSTRIPESTRATEGY_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/drawstrategy/ef_drawstrategyimpl.h>

namespace nw4r {
namespace ef {

class DrawStripeStrategy : public DrawStrategyImpl {
public:
    struct AheadContextStripe : AheadContext {
        math::VEC3 mEmitterAxisX;
        math::VEC3 mViewAxisZ;
        f32* mTrigTable;
        AheadContextStripe(const math::MTX34& rViewMtx, ParticleManager* pManager)
            : AheadContext(rViewMtx, pManager), mTrigTable(nullptr) {
            math::VEC3 axisX(mCommon.mEmitterMtx._00, mCommon.mEmitterMtx._10,
                             mCommon.mEmitterMtx._20);
            math::VEC3TransformNormal(&mEmitterAxisX, &mCommon.mParticleManagerMtxInv, &axisX);
        }
    };
    typedef void (*CalcAheadStripeFunc)(math::VEC3*, AheadContextStripe*, Particle*);
    struct VertexTube {
        math::VEC3 mCenter;
        math::VEC3 mAxisX;
        math::VEC3 mAxisZ;
        f32 mTexCoord;
    };
    DrawStripeStrategy();
    virtual void Draw(const DrawInfo& rInfo, ParticleManager* pManager);
    virtual CalcAheadFunc GetCalcAheadFunc(ParticleManager* pManager);
    void DrawStripe(AheadContextStripe*, int, const math::VEC3&, const math::VEC3&);
    inline void DrawParticle(AheadContextStripe*, int, Particle*, CalcAheadStripeFunc,
                      const math::VEC3&, const math::VEC3&, const math::VEC3&, f32, f32);
    void CalcVertexTube(VertexTube*, AheadContextStripe*, int, Particle*,
                        CalcAheadStripeFunc, f32, f32, f32);
    void DrawStripeTube_ConnectNone(AheadContextStripe*, int);
    void DrawStripeTube_ConnectRing(AheadContextStripe*, int);
    void DrawStripeTube_ConnectEmitter(AheadContextStripe*, int);
    void DrawStripeTube(AheadContextStripe*, int, bool);
    static inline void BeginDrawSide(GXCullMode, bool, int);
    math::VEC3 GetInitialPrevAxis(const EmitterDrawSetting&, const AheadContextStripe&);
    inline void InitGraphics(const DrawInfo&, ParticleManager*);
    void DrawTubeSegment(const VertexTube&, const VertexTube&, int, int, const f32*);
    int GetStripeTubeDivide(const EmitterDrawSetting& rSetting) const {
        return rSetting.typeOption0;
    }
    int GetStripeTexmapType(const EmitterDrawSetting& rSetting) const {
        return rSetting.typeOption2 & 0xC0;
    }
    inline math::MTX34 CalcAxisMtx(AheadContextStripe*, int, Particle*,
                                            const math::VEC3&, const math::VEC3&);
    inline math::MTX34 CalcRotateMtx(Particle*, f32);
    static void CalcAhead_Particle_Stripe(math::VEC3*, AheadContext*, Particle*);
    static void CalcAhead_ParticleBoth_Stripe(math::VEC3*, AheadContext*, Particle*);
    static void CalcAhead_ParticleBoth_Ring(math::VEC3*, AheadContext*, Particle*);
    static void CalcAhead_ParticleBoth_Origin(math::VEC3*, AheadContext*, Particle*);
};

}
}

#endif
