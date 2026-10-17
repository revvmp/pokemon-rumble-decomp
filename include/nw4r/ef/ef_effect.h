#ifndef NW4R_EF_EFFECT_H
#define NW4R_EF_EFFECT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_activitylist.h>
#include <nw4r/ef/ef_draworder.h>
#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/ef/ef_referencedobject.h>
#include <nw4r/ef/ef_res_emitter.h>
#include <nw4r/ef/ef_types.h>
#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_list.h>

namespace nw4r {
namespace ef {

class Emitter;
class ParticleManager;
class Particle;
class EffectSystem;
class DrawOrderBase;

class Effect : public ReferencedObject {
    friend class EffectSystem;
    friend class DrawOrderBase;

private:
public:
    enum CalcFlag {
        CALC_FLAG_ONLY_BILLBOARD = (1 << 0),
        CALC_FLAG_SKIP_PREPARE = (1 << 1),
    };

private:
    enum Flag {
        FLAG_DISABLE_CALC = (1 << 0),
        FLAG_DISABLE_DRAW = (1 << 1),
        FLAG_EXIST_CALC_REMAIN = (1 << 16),
    };

    struct CallBack {
        typedef void (*PrevEmissionFunc)(Emitter* pEmitter,
                                         ParticleManager* pManager, int* pCount,
                                         u32* pFlags,
                                         f32 (*pParams)[NUM_PARAMS], u16* pLife,
                                         f32* pLifeRnd, math::MTX34* pSpace);

        typedef void (*PtclCalcFunc)(ParticleManager* pManager, ut::List* pList,
                                     Particle* pParticle);

        PrevEmissionFunc mPrevEmission;
        PtclCalcFunc mPrevPtclCalc;
        PtclCalcFunc mPostPtclCalc;
    };

public:
    EffectSystem* mManagerES;
    ActivityList mActivityList;
    u32 mGroupID;
    CallBack mCallBack;

protected:
    u32 mFlags;
    math::MTX34 mRootMtx;
    math::VEC3 mVelocity;
    ut::List mParticleManager;
    DrawOrderBase* mDrawOrderFunc;

public:
    Effect();
    ~Effect();

    virtual void SendClosing();
    virtual void DestroyFunc();

    virtual bool Initialize(EffectSystem* pSystem, EmitterResource* pResource,
                            u16 calcRemain);
    virtual Emitter* CreateEmitter(ResEmitter res, u8 drawWeight,
                                   u16 calcRemain) {
        return CreateEmitter(res.ptr(), drawWeight, calcRemain);
    }

    virtual void Calc(u32 calcFlags);
    virtual void Draw(const DrawInfo& rInfo);

    bool Closing(Emitter* pEmitter);

    Emitter* CreateEmitter(EmitterResource* pResource, u8 drawWeight,
                           u16 calcRemain);
    u32 RetireEmitter(Emitter* pEmitter);

    u32 RetireEmitterAll();
    u32 RetireParticleAll();

    u16 GetNumEmitter() const;
    Emitter* GetFirstEmitter();
    Emitter* GetEmitter(u16 idx);

    u32 ForeachParticleManager(ForEachFunc pFunc, ForEachParam param,
                               bool ignoreLifeStatus);
    u32 ForeachEmitterFrom(ForEachFunc pFunc, ForEachParam param,
                           bool ignoreLifeStatus, Emitter* pEmitter);

    void ParticleManagerAdd(ParticleManager* pManager) {
        mDrawOrderFunc->Add(this, pManager);
    }
    void ParticleManagerRemove(ParticleManager* pManager) {
        mDrawOrderFunc->Remove(this, pManager);
    }

    void Modifier_SetSimpleLightType(u8 type, bool ignoreLifeStatus = false) {
        ForeachParticleManager(
            ParticleManager::ModifierTravFunc_SetSimpleLightType,
            static_cast<u32>(type), ignoreLifeStatus);
    }

    void Modifier_SetSimpleLightAmbient(const GXColor& rColor,
                                        bool ignoreLifeStatus = false) {
        ForeachParticleManager(
            ParticleManager::ModifierTravFunc_SetSimpleLightAmbient,
            reinterpret_cast<u32>(&rColor), ignoreLifeStatus);
    }

    void Modifier_SetScale(math::VEC2& rScale, bool ignoreLifeStatus = false) {
        ForeachParticleManager(ParticleManager::ModifierTravFunc_SetScale,
                               reinterpret_cast<u32>(&rScale),
                               ignoreLifeStatus);
    }

    void Modifier_SetRotate(const math::VEC3& rRot,
                            bool ignoreLifeStatus = false) {
        ForeachParticleManager(ParticleManager::ModifierTravFunc_SetRotate,
                               reinterpret_cast<u32>(&rRot), ignoreLifeStatus);
    }

    bool GetFlagDisableCalc() const {
        return mFlags & FLAG_DISABLE_CALC;
    }
    void SetFlagDisableCalc(bool disable) {
        if (disable) {
            mFlags |= FLAG_DISABLE_CALC;
        } else {
            mFlags &= ~FLAG_DISABLE_CALC;
        }
    }

    bool GetFlagDisableDraw() const {
        return mFlags & FLAG_DISABLE_DRAW;
    }
    void SetFlagDisableDraw(bool disable) {
        if (disable) {
            mFlags |= FLAG_DISABLE_DRAW;
        } else {
            mFlags &= ~FLAG_DISABLE_DRAW;
        }
    }

    bool GetFlagExistCalcRemain() const {
        return mFlags & FLAG_EXIST_CALC_REMAIN;
    }
    void SetFlagExistCalcRemain(bool exist) {
        if (exist) {
            mFlags |= FLAG_EXIST_CALC_REMAIN;
        } else {
            mFlags &= ~FLAG_EXIST_CALC_REMAIN;
        }
    }

    const math::MTX34* GetRootMtx() const {
        return &mRootMtx;
    }

    void SetRootMtx(const math::MTX34& rMtx);

    const math::VEC3* GetVelocity() const {
        return &mVelocity;
    }

    void SetVelocity(const nw4r::math::VEC3& rVelocity) {
        mVelocity = rVelocity;
    }
};

}
}

#endif
