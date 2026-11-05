#ifndef GAME_CEFFECT_H
#define GAME_CEFFECT_H

#include <game/graphics/EffectInfo.h>
#include <game/core/ICalc.h>
#include <memory>
#include <nw4r/ef/ef_handle.h>
#include <nw4r/math/math_triangular.h>
#include <nw4r/math/math_types.h>

class BWorld;
class BWorldObj;

namespace nw4r {
namespace ef {
class Effect;
}
}

class CEffect : public ICalc, public std::tr1::enable_shared_from_this<CEffect> {
public:
    enum Flag {
        FLAG_FINALIZED = 1 << 0,
        FLAG_LOCAL_MTX = 1 << 1,
        FLAG_FOLLOW_ROTATION = 1 << 2,
        FLAG_FOLLOW = 1 << 3,
        FLAG_TARGET = 1 << 4,
        FLAG_PERSISTENT = 1 << 5
    };

    enum Type {
        TYPE_NORMAL,
        TYPE_STOP_EMISSION,
        TYPE_RADIUS,
        TYPE_EXTEND_EMISSION,
        TYPE_LIFE,
        TYPE_LIFE_FOLLOW,
        TYPE_SCALE
    };

    struct Frames {
        u16 mEmitSpan;
        u16 mLife;
        f32 mRadius;
        f32 mHeight;
    };

    struct Param {
        const EffectInfo* mInfo;
        bool mEnable;
        union {
            Frames mFrames;
            f32 mScale;
        };
    };

    CEffect();
    virtual ~CEffect();

    virtual void Calc();
    virtual void CalcMtx();

    bool Create(BWorld* pWorld, const Param& rParam);
    bool Create(BWorld* pWorld, const Param& rParam, const nw4r::math::VEC3& rPosition, f32 rotation);
    bool Create(BWorld* pWorld, const Param& rParam, BWorldObj* pFollow);
    bool Create(BWorld* pWorld, const Param& rParam, BWorldObj* pFollow, f32 rotation);
    bool CreateFollowRotation(BWorld* pWorld, const Param& rParam, BWorldObj* pFollow, f32 rotation);
    bool Create(BWorld* pWorld, const Param& rParam, BWorldObj* pFollow, BWorldObj* pTarget);
    void Finalize(BOOL retireParticles);

    u32 GetHandle() const { return mHandle; }
    void SetHandle(u32 handle) { mHandle = handle; }
    bool IsFinished() const { return (mFlags & FLAG_FINALIZED) || mParam.mInfo == nullptr; }
    bool IsPersistent() const { return mFlags & FLAG_PERSISTENT; }

private:
    void SetRotation(f32 rotation) {
        f32 sin;
        f32 cos;
        nw4r::math::SinCosDeg(&sin, &cos, rotation);
        mLocalMtx = nw4r::math::MTX34(cos, 0.0f, sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sin, 0.0f, cos, 0.0f);
    }

    u32 mFlags;
    BWorld* mWorld;
    u32 mHandle;
    Param mParam;
    nw4r::ef::Handle<nw4r::ef::Effect> mEffect;
    nw4r::math::MTX34 mLocalMtx;
    nw4r::math::VEC3 mPosition;
    std::tr1::weak_ptr<BWorldObj> mFollow;
    nw4r::math::VEC3 mTargetPosition;
    std::tr1::weak_ptr<BWorldObj> mTarget;
};

void InitializeEffect(u32 numEffect, u16 numEmitter, u16 numParticleManager, u32 numParticle, u32 maxGroupID);
void FinalizeEffect();

#endif
