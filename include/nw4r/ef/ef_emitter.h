#ifndef NW4R_EF_EMITTER_H
#define NW4R_EF_EMITTER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_activitylist.h>
#include <nw4r/ef/ef_animcurve.h>
#include <nw4r/ef/ef_random.h>
#include <nw4r/ef/ef_referencedobject.h>
#include <nw4r/ef/ef_res_emitter.h>
#include <nw4r/ef/ef_types.h>
#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_list.h>

namespace nw4r {
namespace ef {

class Effect;
class Emitter;
class EmitterForm;
class Particle;
class ParticleManager;
struct EmitterResource;

struct EmitterInheritSetting {
    enum Flag {
        FLAG_FOLLOW_EMIT = (1 << 0),
        FLAG_INHERIT_ROT = (1 << 1),
    };

    s16 speed;
    u8 scale;
    u8 alpha;
    u8 color;
    u8 weight;
    u8 type;
    u8 flag;
    u8 alphaFlagPri;
    u8 alphaFlagSec;

    enum AlphaFlag {
        ALPHA_FLAG_PRIMARY = (1 << 0),
        ALPHA_FLAG_SECONDARY = (1 << 1),
        ALPHA_FLAG_FLICK = (1 << 2),
    };
};

class EmitterParameter {
public:
    enum InheritFlag {
        INHERIT_FLAG_SCALE = (1 << 0),
        INHERIT_FLAG_ROT = (1 << 1)
    };

public:
    u32 mComFlags;
    u32 mEmitFlags;
    f32 mEmitRatio;
    f32 mEmitRandom;
    u16 mEmitInterval;
    u16 mEmitEmitDiv;
    f32 mEmitIntervalRandom;
    f32 mEmitCount;
    u16 mEmitSpan;
    f32 mLODNear;
    f32 mLODFar;
    f32 mLODMinEmit;
    f32 mParams[NUM_PARAMS];
    u8 mInherit;
    s8 mInheritTranslate;
    s8 mVelInitVelocityRandom;
    s8 mVelMomentumRandom;
    f32 mVelPowerRadiationDir;
    f32 mVelPowerYAxis;
    f32 mVelPowerRandomDir;
    f32 mVelPowerNormalDir;
    f32 mVelDiffusionEmitterNormal;
    f32 mVelPowerSpecDir;
    f32 mVelDiffusionSpecDir;
    math::VEC3 mVelSpecDir;
    math::VEC3 mTranslate;
    math::VEC3 mScale;
    math::VEC3 mRotate;
};

class Emitter : public ReferencedObject {
    friend class Effect;

public:
    EmitterParameter mParameter;
    EvaluateStatus mEvalStatus;
    EmitterResource* mResource;
    Effect* mManagerEF;
    ActivityList mActivityList;
    bool mIsFirstEmission;
    u16 mWaitTime;
    u16 mEmitIntervalWait;
    u32 mTick;
    u16 mCalcRemain;
    u16 mRandSeed;
    Random mRandom;
    EmitterForm* mForm;
    Emitter* mParent;
    Particle* mpReferenceParticle;
    EmitterInheritSetting mInheritSetting;

protected:
    bool mMtxDirty;

public:
    math::VEC3 mGlobalTranslate;
    math::VEC3 mPrevGlobalTranslate;

public:
    math::MTX34 mMtx;

public:
    Emitter();
    ~Emitter();

    virtual void SendClosing();
    virtual void DestroyFunc();

    virtual bool Initialize(Effect* pParent, EmitterResource* pResource,
                            u8 drawWeight);

    virtual Emitter* CreateEmitter(EmitterResource* pResource,
                                   EmitterInheritSetting* pSetting,
                                   Particle* pParticle,
                                   u16 calcRemain);

    virtual void CalcEmitter();
    virtual void CalcParticle();
    virtual void CalcEmission();
    virtual void CalcBillboard();

    bool Closing(ParticleManager* pManager);

    u32 RetireParticleAll();
    u32 RetireParticleManager(ParticleManager* pManager);
    u32 RetireParticleManagerAll();

    void UpdateDatas(EmitterResource* pResource);
    bool InitializeDatas(EmitterResource* pResource, Effect* pEffect);

    void CreateEmitterTmp(EmitterResource* pResource,
                          EmitterInheritSetting* pSetting, Particle* pParticle,
                          u16 calcRemain);

    ParticleManager* FindParticleManager(EmitterResource* pResource,
                                         bool inheritS, bool inheritR,
                                         s8 inheritT, u8 weight);

    void Emission(ParticleManager* pManager, const math::MTX34* pSpace);

    math::MTX34* CalcGlobalMtx(math::MTX34* pResult);
    void SetMtxDirty();

    u16 GetNumParticleManager() const;
    ParticleManager* GetParticleManager(u16 idx);

    u32 ForeachEmitter(ForEachFunc pFunc, ForEachParam param,
                       bool ignoreLifeStatus);
    u32 ForeachParticleManager(ForEachFunc pFunc, ForEachParam param,
                               bool ignoreLifeStatus, bool propogate);

    static math::MTX34* RestructMatrix(math::MTX34* pResult, math::MTX34* pOrig,
                                       bool inheritS, bool inheritR,
                                       s8 inheritT);

    bool GetFlagDisableCalc() const {
        return mParameter.mComFlags & EmitterDesc::CMN_FLAG_DISABLE_CALC;
    }

    void SetTranslate(const math::VEC3& rTrans) {
        mParameter.mTranslate = rTrans;
    }
    void SetScale(const math::VEC3& rScale) {
        mParameter.mScale = rScale;
    }
};

}
}

#endif
