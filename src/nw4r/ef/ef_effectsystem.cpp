#include <nw4r/ef/drawstrategy/ef_drawstrategybuilder.h>
#include <nw4r/ef/ef_activitylist.h>
#include <nw4r/ef/ef_drawinfo.h>
#include <nw4r/ef/ef_draworder.h>
#include <nw4r/ef/ef_effect.h>
#include <nw4r/ef/ef_effectsystem.h>
#include <nw4r/ef/ef_memorymanager.h>
#include <nw4r/ef/ef_resource.h>
#include <nw4r/ef/ef_util.h>
#include <nw4r/ef/emform/ef_emform.h>

#include <revolution/os.h>

inline void* operator new[](size_t, void* p) {
    return p;
}

namespace {

NW4R_LIB_VERSION(EF, "Sep 24 2009", "16:53:09", "0x4302_151");

}

namespace nw4r {
namespace ef {

static DrawOrder gBasicDrawOrder;
static DrawStrategyBuilder gBasicDrawStrategyBuilder;
static EmitFormBuilder gBasicEmitFormBuilder;

DrawInfo EffectSystem::mSortEffectOperatorZDrawInfo;

s32 EffectSystem::mDisplayVersion;
EffectSystem EffectSystem::instance;

EffectSystem::EffectSystem() {
    if (mDisplayVersion == 0) {
        mDisplayVersion = 1;
        OSRegisterVersion(NW4R_EF_Version_);
    }

    mMemoryManager = NULL;
    mMaxGroupID = 0;
    mTmpEmitter = nullptr;
    mXFFlushSafe = false;
}

EffectSystem::~EffectSystem() {
    for (u32 i = 0; i < mMaxGroupID; i++) {
        RetireEffectAll(i);
    }
}

EffectSystem* EffectSystem::GetInstance() {
    return &instance;
}

bool EffectSystem::Initialize(u32 maxGroupID) {
    mMaxGroupID = maxGroupID;

    mActivityList =
        new (mMemoryManager->AllocHeap(mMaxGroupID * sizeof(ActivityList) + 32))
            ActivityList[mMaxGroupID];

    for (int i = 0; i < mMaxGroupID; i++) {
        mActivityList[i].SetOffset(offsetof(Effect, mActivityLink));
        mActivityList[i].Initialize();
    }

    mRandom.Srand(0);

    mDrawOrderFunc = &gBasicDrawOrder;
    mDrawStrategyBuilder = &gBasicDrawStrategyBuilder;
    mEmitFormBuilder = &gBasicEmitFormBuilder;

    return true;
}

bool EffectSystem::Closing(Effect* pEffect) {
    mActivityList[pEffect->mGroupID].ToClosing(pEffect);
    pEffect->mLifeStatus = ReferencedObject::NW4R_EF_LS_CLOSING;

    return true;
}

Effect* EffectSystem::CreateEffect(const char* pName, u32 groupID, u16 calcRemain) {
    EmitterResource* pResource = Resource::GetInstance()->_FindEmitter(pName, NULL);

    return CreateEffect(pResource, groupID, calcRemain);
}

Effect* EffectSystem::CreateEffect(EmitterResource* pResource, u32 groupID, u16 calcRemain) {
    if (pResource == NULL) {
        return NULL;
    }

    if (!(pResource->GetEmitterDesc()->commonFlag & EmitterDesc::CMN_FLAG_RELOCATED)) {
        return NULL;
    }

    Effect* pEffect = GetMemoryManager()->AllocEffect();
    if (pEffect == NULL) {
        return NULL;
    }

    if (!pEffect->Initialize(this, pResource, calcRemain)) {
        GetMemoryManager()->FreeEffect(pEffect);
        return NULL;
    }

    pEffect->mGroupID = groupID;

    mActivityList[groupID].ToActive(pEffect);
    pEffect->mLifeStatus = ReferencedObject::NW4R_EF_LS_ACTIVE;

    mActivityList[groupID].ToWait(pEffect);
    pEffect->mLifeStatus = ReferencedObject::NW4R_EF_LS_WAIT;

    return pEffect;
}

u32 EffectSystem::RetireEffect(Effect* pEffect) {
    if (pEffect->mLifeStatus != ReferencedObject::NW4R_EF_LS_ACTIVE) {
        return 0;
    }

    mActivityList[pEffect->mGroupID].ToWait(pEffect);
    pEffect->Destroy();
    return 1;
}

u32 EffectSystem::RetireEffectAll(u32 groupID) {
    u32 num = 0;
    void* pArray[NW4R_EF_MAX_EFFECT];

    u16 size = UtlistToArray(&mActivityList[groupID].mActiveList, pArray,
                             UtlistSize(&mActivityList[groupID].mActiveList));

    for (u16 i = 0; i < size; i++) {
        Effect* pEffect = static_cast<Effect*>(pArray[i]);

        if (pEffect->mLifeStatus == ReferencedObject::NW4R_EF_LS_ACTIVE) {
            num += RetireEffect(pEffect);
        }
    }

    return num;
}

u32 EffectSystem::RetireEmitterAll(u32 groupID) {
    u32 num = 0;
    void* pArray[NW4R_EF_MAX_EFFECT];

    u16 size = UtlistToArray(&mActivityList[groupID].mActiveList, pArray,
                             UtlistSize(&mActivityList[groupID].mActiveList));

    for (u16 i = 0; i < size; i++) {
        Effect* pEffect = static_cast<Effect*>(pArray[i]);
        num += pEffect->RetireEmitterAll();
    }

    return num;
}

u32 EffectSystem::RetireParticleAll(u32 groupID) {
    u32 num = 0;
    void* pArray[NW4R_EF_MAX_EFFECT];

    u16 size = UtlistToArray(&mActivityList[groupID].mActiveList, pArray,
                             UtlistSize(&mActivityList[groupID].mActiveList));

    for (u16 i = 0; i < size; i++) {
        Effect* pEffect = static_cast<Effect*>(pArray[i]);
        num += pEffect->RetireParticleAll();
    }

    return num;
}

void EffectSystem::Calc(u32 groupID, u32 calcFlags) {
    void* pArray[NW4R_EF_MAX_EFFECT];

    u16 size = UtlistToArray(&mActivityList[groupID].mActiveList, pArray,
                             UtlistSize(&mActivityList[groupID].mActiveList));

    for (u16 i = 0; i < size; i++) {
        Effect* pEffect = static_cast<Effect*>(pArray[i]);
        pEffect->Calc(calcFlags);
    }

    mMemoryManager->GarbageCollection();
}

void EffectSystem::Draw(const DrawInfo& rInfo, u32 groupID) {
    NW4R_UT_LIST_FOREACH(Effect, it, mActivityList[groupID].mActiveList, { it->Draw(rInfo); });
}

void EffectSystem::SetProcessCamera(const math::VEC3& rPos, const math::MTX34& rMtx, f32 near,
                                    f32 far) {
    mProcessCameraPos = rPos;
    PSMTXCopy(rMtx, mProcessCameraMtx);
    mProcessCameraNear = near;
    mProcessCameraFar = far;
}

}
}
