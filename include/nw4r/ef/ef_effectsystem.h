#ifndef NW4R_EF_EFFECT_SYSTEM_H
#define NW4R_EF_EFFECT_SYSTEM_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_creationqueue.h>
#include <nw4r/ef/ef_random.h>
#include <nw4r/math/math_types.h>

namespace nw4r {
namespace ef {

class ActivityList;
class DrawInfo;
class DrawOrderBase;
class DrawStrategyBuilder;
class EmitFormBuilder;
class Emitter;
class MemoryManagerBase;
struct EmitterResource;

class EffectSystem {
private:
    MemoryManagerBase* mMemoryManager;

public:
    DrawOrderBase* mDrawOrderFunc;
    DrawStrategyBuilder* mDrawStrategyBuilder;
    EmitFormBuilder* mEmitFormBuilder;
    CreationQueue mCreationQueue;
    u32 mMaxGroupID;
    ActivityList* mActivityList;
    Random mRandom;
    math::VEC3 mProcessCameraPos;
    math::MTX34 mProcessCameraMtx;
    f32 mProcessCameraFar;
    f32 mProcessCameraNear;
    bool mXFFlushSafe;
    Emitter* mTmpEmitter;

    static s32 mDisplayVersion;
    static EffectSystem instance;
    static DrawInfo mSortEffectOperatorZDrawInfo;

public:
    static EffectSystem* GetInstance();

    EffectSystem();
    ~EffectSystem();

    bool Initialize(u32 maxGroupID);
    bool Closing(Effect* pEffect);

    Effect* CreateEffect(const char* pName, u32 groupID, u16 calcRemain);
    Effect* CreateEffect(EmitterResource* pResource, u32 groupID,
                         u16 calcRemain);
    u32 RetireEffect(Effect* pEffect);

    u32 RetireEffectAll(u32 groupID);
    u32 RetireEmitterAll(u32 groupID);
    u32 RetireParticleAll(u32 groupID);

    void Calc(u32 groupID, u32 calcFlags);
    void Draw(const DrawInfo& rInfo, u32 groupID);

    void SetProcessCamera(const math::VEC3& rPos, const math::MTX34& rMtx,
                          f32 near, f32 far);

    MemoryManagerBase* GetMemoryManager() const {
        return mMemoryManager;
    }
    void SetMemoryManager(MemoryManagerBase* pManager, u32 maxGroupID) {
        mMemoryManager = pManager;

        if (mMemoryManager != NULL) {
            Initialize(maxGroupID);
        } else {
            mMaxGroupID = 0;
            mActivityList = NULL;
        }
    }

    void SetXFFlushSafe(bool safe) {
        mXFFlushSafe = safe;
    }
};

}
}

#endif
