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
class MemoryManagerBase;

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

    static bool mDisplayVersion;
    static EffectSystem instance;

public:
    static EffectSystem* GetInstance();

    EffectSystem();
    ~EffectSystem();

    bool Initialize(u32 maxGroupID);
    bool Closing(Effect* pEffect);

    Effect* CreateEffect(const char* pName, u32 groupID, u16 calcRemain);
    u32 RetireEffect(Effect* pEffect);

    u32 RetireEffectAll(u32 groupID);
    u32 RetireEmitterAll(u32 groupID);
    u32 RetireParticleAll(u32 groupID);

    void Calc(u32 groupID, bool onlyBillboard);
    void Draw(const DrawInfo& rInfo, u32 groupID);

    MemoryManagerBase* GetMemoryManager() const {
        return mMemoryManager;
    }
    void SetMemoryManager(MemoryManagerBase* pManager, u32 maxGroupID) {
        mMemoryManager = pManager;

        if (mMemoryManager != NULL) {
            Initialize(maxGroupID);
        }
    }

    void SetXFFlushSafe(bool safe) {
        mXFFlushSafe = safe;
    }
};

}
}

#endif
