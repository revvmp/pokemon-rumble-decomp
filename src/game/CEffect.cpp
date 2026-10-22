#include <game/CEffect.h>
#include <game/BModelObj.h>
#include <game/BWorld.h>
#include <game/BWorldObj.h>
#include <game/EffectGroupID.h>
#include <game/MathUtil.h>
#include <game/Memory.h>
#include <game/Random.h>
#include <nw4r/ef/ef_effect.h>
#include <nw4r/ef/ef_effectsystem.h>
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/emform/ef_emform.h>
#include <nw4r/ef/ef_memorymanager.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <nw4r/ut/ut_list.h>
#include <new>
#include <string.h>
#include <vector>

namespace {

inline bool IsEqual(const char* pName, const char* pPrefix, size_t length) {
    return strncmp(pName, pPrefix, length) == 0;
}

inline u32 AlignHeap(u32 value) {
    return (value + 31) & ~31;
}

class EmitterWithParam : public nw4r::ef::Emitter {
public:
    EmitterWithParam() {
        mParam.mInfo = nullptr;
        mParam.mEnable = false;
    }

    virtual bool Initialize(nw4r::ef::Effect* pParent, nw4r::ef::EmitterResource* pResource, u8 drawWeight) {
        bool result = nw4r::ef::Emitter::Initialize(pParent, pResource, drawWeight);
        mParam.mInfo = nullptr;
        mParam.mEnable = false;
        return result;
    }

    virtual nw4r::ef::Emitter* CreateEmitter(nw4r::ef::EmitterResource* pResource,
                                             nw4r::ef::EmitterInheritSetting* pSetting, nw4r::ef::Particle* pParticle,
                                             u16 calcRemain, nw4r::math::VEC3* pTranslate) {
        nw4r::ef::Emitter* pEmitter =
            nw4r::ef::Emitter::CreateEmitter(pResource, pSetting, pParticle, calcRemain, pTranslate);
        if (pEmitter != nullptr && mParam.mEnable) {
            switch (mParam.mInfo->mType) {
            case CEffect::TYPE_LIFE: {
                if (pEmitter->mResource->GetEmitterDesc()->GetFormType() != nw4r::ef::EMITFORMTYPE_CUBE) {
                    break;
                }

                nw4r::math::VEC3 scale(static_cast<const nw4r::math::_VEC3&>(pEmitter->mParameter.mScale));
                const char* pName = pEmitter->mResource->GetName();
                if (IsEqual(pName, "main", 4)) {
                    pEmitter->mParameter.mParams[0] = mParam.mFrames.mHeight * 0.5f / scale.x;
                    pEmitter->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
                    pEmitter->SetMtxDirty();
                    pEmitter->mParameter.mVelDiffusionSpecDir = 0.0f;
                    pEmitter->mParameter.mVelPowerSpecDir = mParam.mFrames.mRadius / scale.x;
                    pEmitter->mWaitTime = 1;
                    pEmitter->mParameter.mEmitSpan = mParam.mFrames.mEmitSpan;
                    static_cast<EmitterWithParam*>(pEmitter)->mParam = mParam;
                } else if (IsEqual(pName, "sub", 3)) {
                    pEmitter->mWaitTime = mParam.mFrames.mLife + 1;
                    pEmitter->mParameter.mParams[0] = mParam.mFrames.mHeight * 0.5f / scale.x;
                    pEmitter->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, mParam.mFrames.mRadius * mParam.mFrames.mLife));
                    pEmitter->SetMtxDirty();
                }
                break;
            }
            case CEffect::TYPE_LIFE_FOLLOW: {
                if (pEmitter->mResource->GetEmitterDesc()->GetFormType() != nw4r::ef::EMITFORMTYPE_CYLINDER) {
                    break;
                }

                pEmitter->mParameter.mParams[2] = NW4R_MATH_PI + -mParam.mFrames.mHeight * 0.5f;
                pEmitter->mParameter.mParams[3] = NW4R_MATH_PI + mParam.mFrames.mHeight * 0.5f;
                nw4r::math::VEC3 scale(static_cast<const nw4r::math::_VEC3&>(pEmitter->mParameter.mScale));
                pEmitter->SetTranslate(nw4r::math::VEC3(0.0f, 0.0f, 0.0f));
                pEmitter->SetMtxDirty();

                const char* pName = pEmitter->mResource->GetName();
                if (IsEqual(pName, "main", 4)) {
                    pEmitter->mParameter.mParams[0] = pEmitter->mParameter.mParams[5] = 0.01f;
                    pEmitter->mParameter.mVelPowerRadiationDir = mParam.mFrames.mRadius / scale.x;
                    pEmitter->mWaitTime = 1;
                    pEmitter->mParameter.mEmitSpan = mParam.mFrames.mEmitSpan;
                    static_cast<EmitterWithParam*>(pEmitter)->mParam = mParam;
                } else if (IsEqual(pName, "sub", 3)) {
                    pEmitter->mWaitTime = mParam.mFrames.mLife + 1;
                    f32 radius = mParam.mFrames.mRadius * mParam.mFrames.mLife;
                    pEmitter->mParameter.mParams[0] = radius / scale.x;
                    pEmitter->mParameter.mParams[5] = radius / scale.z;
                }
                break;
            }
            }
        }

        return pEmitter;
    }

    CEffect::Param mParam;
};

void PrevEmissionLife(nw4r::ef::Emitter* pEmitter, nw4r::ef::ParticleManager* pManager, int* pCount, u32* pFlags,
                      f32 (*pParams)[nw4r::ef::NUM_PARAMS], u16* pLife, f32* pLifeRnd, nw4r::math::MTX34* pSpace);
void PrevEmissionLifeFollow(nw4r::ef::Emitter* pEmitter, nw4r::ef::ParticleManager* pManager, int* pCount, u32* pFlags,
                            f32 (*pParams)[nw4r::ef::NUM_PARAMS], u16* pLife, f32* pLifeRnd,
                            nw4r::math::MTX34* pSpace);

const nw4r::math::_VEC3 sExtendEmissionOffsets[] = {
    {0.5f, 0.7f, 0.3f},   {0.25f, 0.525f, 0.475f},  {0.0f, 0.35f, 0.65f}, {-0.25f, 0.175f, 0.825f},
    {-0.5f, 0.0f, 1.0f},  {-0.5f, 0.7f, 0.3f},      {-0.25f, 0.525f, 0.475f}, {0.0f, 0.35f, 0.65f},
    {0.25f, 0.175f, 0.825f}, {0.5f, 0.0f, 1.0f},
};

std::tr1::shared_ptr<std::vector<bool> > sUsedEffectGroupID;
int sNumUsedEffectGroupID;

}

namespace nw4r {
namespace ef {

inline void ReleaseFromParent(ParticleManager* pManager);
inline void ReleaseFromParent(Emitter* pEmitter);
inline void ReleaseFromParent(Effect* pEffect);

template <typename T> class MemoryManagerTmp {
public:
    MemoryManagerTmp(int num, T* pBuffer) {
        ut::List_Init(&mFreeList, 0);
        ut::List_Init(&mActiveList, 0);
        mObjectSize = sizeof(T);
        mBuffer = pBuffer;

        for (u32 i = 0; i < num; i++) {
            pBuffer[i].mObjectID = i;
            ut::List_Append(&mFreeList, &pBuffer[i]);
        }
    }

    void GarbageCollection() {
        T* pObject;
        T* pNext;
        pObject = static_cast<T*>(ut::List_GetFirst(&mActiveList));
        if (pObject == nullptr) return;
        pNext = static_cast<T*>(ut::List_GetNext(&mActiveList, pObject));
        while (pObject != nullptr) {
            if (pObject->GetLifeStatus() == ReferencedObject::NW4R_EF_LS_CLOSING && pObject->GetRefCount() == 0) {
                ReleaseFromParent(pObject);
                Free(pObject);
            }
            pObject = pNext;
            pNext = static_cast<T*>(ut::List_GetNext(&mActiveList, pNext));
        }
    }

    T* Alloc() {
        T* pObject = static_cast<T*>(ut::List_GetFirst(&mFreeList));
        if (pObject == nullptr) {
            return nullptr;
        }

        UpdateObjectID(pObject);
        ut::List_Remove(&mFreeList, pObject);
        ut::List_Append(&mActiveList, pObject);
        return pObject;
    }

    void Free(void* pObject) {
        ut::List_Remove(&mActiveList, pObject);
        ut::List_Append(&mFreeList, pObject);
    }

    u32 GetNumAlloc() const { return mActiveList.numObjects + mFreeList.numObjects; }
    u32 GetNumActive() const { return mActiveList.numObjects; }
    u32 GetNumFree() const { return mFreeList.numObjects; }

    T* mBuffer;
    ut::List mFreeList;
    ut::List mActiveList;
    u32 mObjectSize;

    virtual void UpdateObjectID(T* pObject) { pObject->mObjectID += 0x10000; }
};

inline void ReleaseFromParent(ParticleManager* pManager) {
    pManager->mManagerEM->mActivityList.ToFree(pManager);
}

inline void ReleaseFromParent(Emitter* pEmitter) {
    pEmitter->mManagerEF->mActivityList.ToFree(pEmitter);
}

inline void ReleaseFromParent(Effect* pEffect) {
    pEffect->mManagerES->mActivityList[pEffect->mGroupID].ToFree(pEffect);
}

class MemoryManager : public MemoryManagerBase {
public:
    struct HeapBlock {
        HeapBlock* mPrev;
        HeapBlock* mNext;
        HeapBlock* mPrevPhys;
        HeapBlock* mNextPhys;
        u32 mSize;
        bool mUsed;
    };

    MemoryManager(void* pBuffer, u32 size, u16 numEffect, u16 numEmitter, u16 numParticleManager, u16 numParticle) {
        mNumEffect = numEffect;
        mNumEmitter = numEmitter;
        mNumParticleManager = numParticleManager;
        mNumParticle = numParticle;
        mEffectPool = nullptr;
        mEmitterPool = nullptr;
        mParticleManagerPool = nullptr;
        mParticlePool = nullptr;
        mUsedList = nullptr;
        mLastFreed = nullptr;
        mFirstBlock = nullptr;
        mHeapEnd = static_cast<u8*>(pBuffer) + size;
        mHeapStart = reinterpret_cast<u8*>(AlignHeap(reinterpret_cast<u32>(pBuffer)));
        mFreeList = reinterpret_cast<HeapBlock*>(mHeapStart);
        mFreeList->mPrev = nullptr;
        mFreeList->mNext = nullptr;
        mFreeList->mPrevPhys = nullptr;
        mFreeList->mNextPhys = nullptr;
        mFreeList->mUsed = false;
        mFreeList->mSize = mHeapEnd - mHeapStart - AlignHeap(sizeof(HeapBlock));
        mFirstBlock = mFreeList;
        mUsedList = nullptr;

        int numEffects = mNumEffect;
        Effect* pEffects = nullptr;
        if (numEffects > 0) {
            pEffects = new (AllocHeap(sizeof(Effect) * numEffects + 0x20)) Effect[numEffects];
        }
        mEffectPool = new (AllocHeap(sizeof(MemoryManagerTmp<Effect>))) MemoryManagerTmp<Effect>(mNumEffect, pEffects);

        int numEmitters = mNumEmitter;
        EmitterWithParam* pEmitters = nullptr;
        if (numEmitters > 0) {
            pEmitters = new (AllocHeap(sizeof(EmitterWithParam) * numEmitters + 0x20)) EmitterWithParam[numEmitters];
        }
        mEmitterPool = new (AllocHeap(sizeof(MemoryManagerTmp<EmitterWithParam>)))
            MemoryManagerTmp<EmitterWithParam>(mNumEmitter, pEmitters);

        int numParticleManagers = mNumParticleManager;
        ParticleManager* pParticleManagers = nullptr;
        if (numParticleManagers > 0) {
            pParticleManagers =
                new (AllocHeap(sizeof(ParticleManager) * numParticleManagers + 0x20)) ParticleManager[numParticleManagers];
        }
        mParticleManagerPool = new (AllocHeap(sizeof(MemoryManagerTmp<ParticleManager>)))
            MemoryManagerTmp<ParticleManager>(mNumParticleManager, pParticleManagers);

        int numParticles = mNumParticle;
        Particle* pParticles = nullptr;
        if (numParticles > 0) {
            pParticles = new (AllocHeap(sizeof(Particle) * numParticles + 0x20)) Particle[numParticles];
        }
        mParticlePool =
            new (AllocHeap(sizeof(MemoryManagerTmp<Particle>))) MemoryManagerTmp<Particle>(mNumParticle, pParticles);
    }

    virtual ~MemoryManager() {}

    virtual void GarbageCollection() {
        NW4R_UT_LIST_FOREACH_SAFE(ParticleManager, pManager, mParticleManagerPool->mActiveList, {
            Particle* pParticle;
            Particle* pNext;
            for (pParticle = static_cast<Particle*>(pManager->mActivityList.mClosingList.headObject);
                 pParticle != nullptr; pParticle = pNext) {
                pNext = static_cast<Particle*>(
                    NW4R_UT_LIST_GET_LINK(pManager->mActivityList.mClosingList, pParticle)->nextObject);
                pParticle->mParticleManager->mActivityList.ToFree(pParticle);
                ut::List_Remove(&mParticlePool->mActiveList, pParticle);
                ut::List_Append(&mParticlePool->mFreeList, pParticle);
            }
        });

        if (mParticleManagerPool != nullptr) mParticleManagerPool->GarbageCollection();
        if (mEmitterPool != nullptr) mEmitterPool->GarbageCollection();
        if (mEffectPool != nullptr) mEffectPool->GarbageCollection();
    }

    virtual Effect* AllocEffect() {
        if (mEffectPool == nullptr) {
            return nullptr;
        }
        return mEffectPool->Alloc();
    }
    virtual void FreeEffect(void* pObject) {
        if (mEffectPool != nullptr) {
            mEffectPool->Free(pObject);
        }
    }
    virtual u32 GetNumAllocEffect() const {
        if (mEffectPool == nullptr) {
            return 0;
        }
        return mEffectPool->GetNumAlloc();
    }
    virtual u32 GetNumActiveEffect() const {
        if (mEffectPool == nullptr) {
            return 0;
        }
        return mEffectPool->GetNumActive();
    }
    virtual u32 GetNumFreeEffect() const {
        if (mEffectPool == nullptr) {
            return 0;
        }
        return mEffectPool->GetNumFree();
    }

    virtual Emitter* AllocEmitter() {
        if (mEmitterPool == nullptr) {
            return nullptr;
        }
        return mEmitterPool->Alloc();
    }
    virtual void FreeEmitter(void* pObject) {
        if (mEmitterPool != nullptr) {
            mEmitterPool->Free(pObject);
        }
    }
    virtual u32 GetNumAllocEmitter() const {
        if (mEmitterPool == nullptr) {
            return 0;
        }
        return mEmitterPool->GetNumAlloc();
    }
    virtual u32 GetNumActiveEmitter() const {
        if (mEmitterPool == nullptr) {
            return 0;
        }
        return mEmitterPool->GetNumActive();
    }
    virtual u32 GetNumFreeEmitter() const {
        if (mEmitterPool == nullptr) {
            return 0;
        }
        return mEmitterPool->GetNumFree();
    }

    virtual ParticleManager* AllocParticleManager() {
        if (mParticleManagerPool == nullptr) {
            return nullptr;
        }
        return mParticleManagerPool->Alloc();
    }
    virtual void FreeParticleManager(void* pObject) {
        if (mParticleManagerPool != nullptr) {
            mParticleManagerPool->Free(pObject);
        }
    }
    virtual u32 GetNumAllocParticleManager() const {
        if (mParticleManagerPool == nullptr) {
            return 0;
        }
        return mParticleManagerPool->GetNumAlloc();
    }
    virtual u32 GetNumActiveParticleManager() const {
        if (mParticleManagerPool == nullptr) {
            return 0;
        }
        return mParticleManagerPool->GetNumActive();
    }
    virtual u32 GetNumFreeParticleManager() const {
        if (mParticleManagerPool == nullptr) {
            return 0;
        }
        return mParticleManagerPool->GetNumFree();
    }

    virtual Particle* AllocParticle() {
        if (mParticlePool == nullptr) {
            return nullptr;
        }
        return mParticlePool->Alloc();
    }
    virtual void FreeParticle(void* pObject) {
        if (mParticlePool != nullptr) {
            mParticlePool->Free(pObject);
        }
    }
    virtual u32 GetNumAllocParticle() const {
        if (mParticlePool == nullptr) {
            return 0;
        }
        return mParticlePool->GetNumAlloc();
    }
    virtual u32 GetNumActiveParticle() const {
        if (mParticlePool == nullptr) {
            return 0;
        }
        return mParticlePool->GetNumActive();
    }
    virtual u32 GetNumFreeParticle() const {
        if (mParticlePool == nullptr) {
            return 0;
        }
        return mParticlePool->GetNumFree();
    }

    virtual void* AllocHeap(u32 size);
    virtual void FreeHeap(void* pPtr) {
        u32 headerSize = AlignHeap(sizeof(HeapBlock));
        HeapBlock* pInfo = reinterpret_cast<HeapBlock*>(static_cast<u8*>(pPtr) - headerSize);

        if (pInfo->mPrev != nullptr) {
            pInfo->mPrev->mNext = pInfo->mNext;
        } else {
            mUsedList = pInfo->mNext;
        }

        if (pInfo->mNext != nullptr) {
            pInfo->mNext->mPrev = pInfo->mPrev;
        }

        HeapBlock* pPrev = nullptr;
        if (pInfo->mPrevPhys != nullptr && !pInfo->mPrevPhys->mUsed) {
            pPrev = pInfo->mPrevPhys;
        }

        HeapBlock* pNext = nullptr;
        if (pInfo->mNextPhys != nullptr && !pInfo->mNextPhys->mUsed) {
            pNext = pInfo->mNextPhys;
        }

        if (pPrev != nullptr) {
            if (pNext != nullptr) {
                if (pNext->mPrev != nullptr) {
                    pNext->mPrev->mNext = pNext->mNext;
                } else {
                    mFreeList = pNext->mNext;
                }

                if (pNext->mNext != nullptr) {
                    pNext->mNext->mPrev = pNext->mPrev;
                }

                pPrev->mSize += pNext->mSize + pInfo->mSize + headerSize + headerSize;

                pPrev->mNextPhys = pNext->mNextPhys;

                if (pNext->mNextPhys != nullptr) {
                    pNext->mNextPhys->mPrevPhys = pPrev;
                }
            } else {
                pPrev->mSize += pInfo->mSize + headerSize;
                pPrev->mNextPhys = pInfo->mNextPhys;

                if (pInfo->mNextPhys != nullptr) {
                    pInfo->mNextPhys->mPrevPhys = pPrev;
                }
            }
        } else if (pNext != nullptr) {
            if (pNext->mPrev != nullptr) {
                pNext->mPrev->mNext = pInfo;
            } else {
                mFreeList = pInfo;
            }

            if (pNext->mNext != nullptr) {
                pNext->mNext->mPrev = pInfo;
            }

            pInfo->mPrev = pNext->mPrev;
            pInfo->mNext = pNext->mNext;
            pInfo->mSize += pNext->mSize + headerSize;
            pInfo->mUsed = false;
            pInfo->mNextPhys = pNext->mNextPhys;

            if (pNext->mNextPhys != nullptr) {
                pNext->mNextPhys->mPrevPhys = pPrev;
            }
        } else {
            if (mFreeList != nullptr) {
                mFreeList->mPrev = pInfo;
            }

            pInfo->mNext = mFreeList;
            pInfo->mPrev = nullptr;
            pInfo->mUsed = false;
            mFreeList = pInfo;
        }
    }

    int mNumEffect;
    MemoryManagerTmp<Effect>* mEffectPool;
    int mNumEmitter;
    MemoryManagerTmp<EmitterWithParam>* mEmitterPool;
    int mNumParticleManager;
    MemoryManagerTmp<ParticleManager>* mParticleManagerPool;
    int mNumParticle;
    MemoryManagerTmp<Particle>* mParticlePool;
    u8* mHeapStart;
    u8* mHeapEnd;
    HeapBlock* mUsedList;
    HeapBlock* mFreeList;
    HeapBlock* mLastFreed;
    HeapBlock* mFirstBlock;
};

}
}

void InitializeEffect(u32 numEffect, u32 numEmitter, u32 numParticleManager, u32 numParticle, u32 maxGroupID) {
    u32 size = 0x220 + AlignHeap(sizeof(nw4r::ef::Effect) * numEffect + 0x20) +
               AlignHeap(sizeof(EmitterWithParam) * numEmitter + 0x20) +
               AlignHeap(sizeof(nw4r::ef::ParticleManager) * numParticleManager + 0x20) +
               AlignHeap(sizeof(nw4r::ef::Particle) * numParticle + 0x20) +
               AlignHeap(sizeof(nw4r::ef::ActivityList) * maxGroupID + 0x20);
    void* pBuffer = GameMemory::Allocate(size);

    nw4r::ef::EffectSystem::GetInstance()->SetMemoryManager(
        new nw4r::ef::MemoryManager(pBuffer, size, numEffect, numEmitter, numParticleManager, numParticle), maxGroupID);
}

void* nw4r::ef::MemoryManager::AllocHeap(u32 size) {
    u32 blockSize = AlignHeap(size);

    for (HeapBlock* pBlock = mFreeList; pBlock != nullptr; pBlock = pBlock->mNext) {
        if (pBlock->mSize < blockSize) {
            continue;
        }

        if (pBlock->mSize < blockSize + 0x40) {
            if (pBlock->mPrev != nullptr) {
                pBlock->mPrev->mNext = pBlock->mNext;
            } else {
                mFreeList = pBlock->mNext;
            }
            if (pBlock->mNext != nullptr) {
                pBlock->mNext->mPrev = pBlock->mPrev;
            }

            if (mUsedList != nullptr) {
                mUsedList->mPrev = pBlock;
            }
            pBlock->mNext = mUsedList;
            pBlock->mPrev = nullptr;
            pBlock->mUsed = true;
            mUsedList = pBlock;
        } else {
            HeapBlock* pRest = reinterpret_cast<HeapBlock*>(reinterpret_cast<u8*>(pBlock) + AlignHeap(sizeof(HeapBlock)) + blockSize);
            pRest->mPrev = pBlock->mPrev;
            pRest->mNext = pBlock->mNext;
            pRest->mSize = pBlock->mSize - blockSize - AlignHeap(sizeof(HeapBlock));
            pRest->mUsed = false;

            pRest->mNextPhys = pBlock->mNextPhys;
            if (pBlock->mNextPhys != nullptr) {
                pBlock->mNextPhys->mPrevPhys = pRest;
            }
            pRest->mPrevPhys = pBlock;
            pBlock->mNextPhys = pRest;

            if (pBlock->mPrev != nullptr) {
                pBlock->mPrev->mNext = pRest;
            } else {
                mFreeList = pRest;
            }
            if (pBlock->mNext != nullptr) {
                pRest->mNext->mPrev = pRest;
            }

            if (mUsedList != nullptr) {
                mUsedList->mPrev = pBlock;
            }
            pBlock->mNext = mUsedList;
            pBlock->mPrev = nullptr;
            pBlock->mSize = blockSize;
            pBlock->mUsed = true;
            mUsedList = pBlock;
        }

        return reinterpret_cast<u8*>(pBlock) + AlignHeap(sizeof(HeapBlock));
    }

    return nullptr;
}

void FinalizeEffect() {}

u32 AllocEffectGroupID() {
    nw4r::ef::EffectSystem* pSystem = nw4r::ef::EffectSystem::GetInstance();

    if (sNumUsedEffectGroupID == 0) {
        sUsedEffectGroupID.reset(new std::vector<bool>(pSystem->mMaxGroupID, false));
        (*sUsedEffectGroupID)[0] = true;
        sNumUsedEffectGroupID++;
        return 0;
    }

    for (u32 i = 0; i < sUsedEffectGroupID->size(); i++) {
        if (!(*sUsedEffectGroupID)[i]) {
            (*sUsedEffectGroupID)[i] = true;
            sNumUsedEffectGroupID++;
            return i;
        }
    }

    return EFFECT_GROUP_INVALID;
}

void FreeEffectGroupID(u32 id) {
    if (sNumUsedEffectGroupID == 0) {
        return;
    }
    if (id < sUsedEffectGroupID->size()) {
        if ((*sUsedEffectGroupID)[id]) {
            (*sUsedEffectGroupID)[id] = false;
            if (--sNumUsedEffectGroupID == 0) {
                sUsedEffectGroupID.reset();
            }
        }
    }
}

CEffect::CEffect() : mFlags(0), mWorld(nullptr), mHandle(0) {
    mParam.mInfo = nullptr;
}

bool CEffect::Create(BWorld* pWorld, const Param& rParam) {
    mParam = rParam;
    mWorld = pWorld;

    u32 groupID = mParam.mInfo->mUseCaptureGroup ? pWorld->GetCaptureEffectGroupID() : pWorld->GetEffectGroupID();

    nw4r::ef::EffectSystem* pSystem;
    while (true) {
        pSystem = nw4r::ef::EffectSystem::GetInstance();
        mEffect = pSystem->CreateEffect(mParam.mInfo->mName, groupID, 0);
        if (mEffect.IsValid()) {
            break;
        }

        if (!pWorld->RemoveOldestEffect(false)) {
            mFlags |= FLAG_FINALIZED;
            return false;
        }

        pSystem->GetMemoryManager()->GarbageCollection();
    }

    if (std::tr1::shared_ptr<BWorldObj> pFollow = Lock(mFollow)) {
        mFlags |= FLAG_FOLLOW;
    }

    if (std::tr1::shared_ptr<BWorldObj> pTarget = Lock(mTarget)) {
        mFlags |= FLAG_TARGET;
    }

    if (mParam.mEnable) {
        mParam.mEnable = false;

        nw4r::ef::Emitter* pEmitter = mEffect.GetPtr()->GetFirstEmitter();
        if (pEmitter != nullptr) {
            nw4r::ef::EmitterResource* pResource = pEmitter->mResource;

            switch (mParam.mInfo->mType) {
            case TYPE_STOP_EMISSION: {
                if (pResource->GetEmitterDesc()->GetFormType() != nw4r::ef::EMITFORMTYPE_DISC) {
                    break;
                }

                for (u16 i = pResource->NumEmitInitTrack(); i < pResource->NumEmitTrack(); i++) {
                    nw4r::ef::AnimCurveHeader* pCurve =
                        reinterpret_cast<nw4r::ef::AnimCurveHeader*>(pResource->GetEmitTrack(i));
                    if (pCurve->kindType == 0x2C || pCurve->kindType == 0x3C) {
                        pCurve->processFlag |= nw4r::ef::AnimCurveHeader::PROC_FLAG_STOP;
                    }
                }

                pEmitter->mParameter.mEmitSpan = mParam.mFrames.mEmitSpan;
                mParam.mEnable = true;
                break;
            }
            case TYPE_RADIUS: {
                if (pResource->GetEmitterDesc()->GetFormType() != nw4r::ef::EMITFORMTYPE_SPHERE) {
                    break;
                }

                nw4r::math::VEC3 scale(static_cast<const nw4r::math::_VEC3&>(pEmitter->mParameter.mScale));
                pEmitter->mParameter.mParams[0] = mParam.mFrames.mRadius / scale.x;
                pEmitter->mParameter.mParams[5] = mParam.mFrames.mRadius / scale.z;
                pEmitter->mParameter.mEmitSpan = mParam.mFrames.mEmitSpan;
                mParam.mEnable = true;
                break;
            }
            case TYPE_LIFE: {
                EmitterWithParam* pWithParam = dynamic_cast<EmitterWithParam*>(pEmitter);
                mParam.mEnable = true;
                pWithParam->mParam = mParam;
                mEffect.GetPtr()->mCallBack.mPrevEmission = PrevEmissionLife;
                break;
            }
            case TYPE_LIFE_FOLLOW: {
                EmitterWithParam* pWithParam = dynamic_cast<EmitterWithParam*>(pEmitter);
                mParam.mEnable = true;
                pWithParam->mParam = mParam;
                mEffect.GetPtr()->mCallBack.mPrevEmission = PrevEmissionLifeFollow;
                break;
            }
            case TYPE_SCALE: {
                pEmitter->SetScale(nw4r::math::VEC3(mParam.mScale, mParam.mScale, mParam.mScale));
                pEmitter->SetMtxDirty();
                mParam.mEnable = false;
                break;
            }
            case TYPE_EXTEND_EMISSION: {
                for (u16 i = pResource->NumEmitInitTrack(); i < pResource->NumEmitTrack(); i++) {
                    nw4r::ef::AnimCurveHeader* pCurve =
                        reinterpret_cast<nw4r::ef::AnimCurveHeader*>(pResource->GetEmitTrack(i));
                    if (pCurve->kindType == 0x70) {
                        pCurve->processFlag |= nw4r::ef::AnimCurveHeader::PROC_FLAG_STOP;
                    }
                }

                if (pEmitter->mParameter.mEmitSpan < mParam.mFrames.mEmitSpan) {
                    pEmitter->mParameter.mEmitSpan = mParam.mFrames.mEmitSpan;
                }
                mParam.mEnable = true;
                break;
            }
            }
        }
    }

    return true;
}

namespace {

void PrevEmissionLife(nw4r::ef::Emitter* pEmitter, nw4r::ef::ParticleManager* pManager, int* pCount, u32* pFlags,
                      f32 (*pParams)[nw4r::ef::NUM_PARAMS], u16* pLife, f32* pLifeRnd, nw4r::math::MTX34* pSpace) {
    EmitterWithParam* pWithParam = dynamic_cast<EmitterWithParam*>(pEmitter);
    if (pWithParam != nullptr && pWithParam->mParam.mEnable) {
        *pLife = pWithParam->mParam.mFrames.mLife;
    }
}

void PrevEmissionLifeFollow(nw4r::ef::Emitter* pEmitter, nw4r::ef::ParticleManager* pManager, int* pCount, u32* pFlags,
                            f32 (*pParams)[nw4r::ef::NUM_PARAMS], u16* pLife, f32* pLifeRnd,
                            nw4r::math::MTX34* pSpace) {
    EmitterWithParam* pWithParam = dynamic_cast<EmitterWithParam*>(pEmitter);
    if (pWithParam != nullptr && pWithParam->mParam.mEnable) {
        *pLife = pWithParam->mParam.mFrames.mLife;
    }
}

inline nw4r::math::MTX34 MakeTranslationMtx(const nw4r::math::VEC3& rTranslate) {
    return nw4r::math::MTX34(1.0f, 0.0f, 0.0f, rTranslate.x, 0.0f, 1.0f, 0.0f, rTranslate.y, 0.0f, 0.0f, 1.0f,
                             rTranslate.z);
}

inline nw4r::math::MTX34 MakeRotationMtx(f32 rotation) {
    f32 sin;
    f32 cos;
    nw4r::math::SinCosDeg(&sin, &cos, rotation);
    return nw4r::math::MTX34(cos, 0.0f, sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sin, 0.0f, cos, 0.0f);
}

inline nw4r::math::MTX34 Mult(const nw4r::math::MTX34& rA, const nw4r::math::MTX34& rB) {
    nw4r::math::MTX34 result;
    nw4r::math::MTX34Mult(&result, &rA, &rB);
    return result;
}

}

bool CEffect::Create(BWorld* pWorld, const Param& rParam, BWorldObj* pFollow) {
    if (pFollow != nullptr) {
        mFollow = pFollow->shared_from_this();
    }
    mFlags |= FLAG_LOCAL_MTX;
    SetRotation(RandReal(0.0f, 360.0f));
    return Create(pWorld, rParam);
}

bool CEffect::Create(BWorld* pWorld, const Param& rParam, const nw4r::math::VEC3& rPosition, f32 rotation) {
    mPosition = rPosition;
    mFlags |= FLAG_LOCAL_MTX;
    SetRotation(rotation);
    return Create(pWorld, rParam);
}

bool CEffect::Create(BWorld* pWorld, const Param& rParam, BWorldObj* pFollow, f32 rotation) {
    if (pFollow != nullptr) {
        mFollow = pFollow->shared_from_this();
    }
    mFlags |= FLAG_LOCAL_MTX;
    SetRotation(rotation);
    return Create(pWorld, rParam);
}

bool CEffect::CreateFollowRotation(BWorld* pWorld, const Param& rParam, BWorldObj* pFollow, f32 rotation) {
    if (pFollow != nullptr) {
        mFollow = pFollow->shared_from_this();
    }
    mFlags |= FLAG_LOCAL_MTX | FLAG_FOLLOW_ROTATION;
    SetRotation(rotation);
    return Create(pWorld, rParam);
}

bool CEffect::Create(BWorld* pWorld, const Param& rParam, BWorldObj* pFollow, BWorldObj* pTarget) {
    if (pFollow != nullptr) {
        mFollow = pFollow->shared_from_this();
    }
    if (pTarget != nullptr) {
        mTarget = pTarget->shared_from_this();
    }
    return Create(pWorld, rParam);
}

CEffect::~CEffect() {
    Finalize(FALSE);
}

void CEffect::Finalize(BOOL retireParticles) {
    if (mFlags & FLAG_FINALIZED) {
        return;
    }

    if (mEffect.IsAlive()) {
        nw4r::ef::Effect* pEffect = mEffect.GetPtr();
        if (retireParticles == TRUE) {
            pEffect->RetireParticleAll();
        }
        if (!mParam.mEnable || (mParam.mInfo->mType != TYPE_LIFE && mParam.mInfo->mType != TYPE_LIFE_FOLLOW)) {
            pEffect->RetireEmitterAll();
        }
        nw4r::ef::EffectSystem::GetInstance()->RetireEffect(pEffect);
    }

    mEffect = nw4r::ef::Handle<nw4r::ef::Effect>();
    mFlags |= FLAG_FINALIZED;
}

void CEffect::Calc() {}

void CEffect::CalcMtx() {
    if (!mEffect.IsAlive()) {
        Finalize(FALSE);
        return;
    }

    f32 rotation = 0.0f;
    if (std::tr1::shared_ptr<BWorldObj> pFollow = Lock(mFollow)) {
        BModelObj* pModelObj = DynamicCast<BModelObj>(pFollow.get());
        if (pModelObj != nullptr && pModelObj->GetModel()) {
            u32 nodeID = 0xFFFFFFFF;
            nw4r::g3d::ScnMdlSimple* pScnMdl =
                static_cast<nw4r::g3d::ScnMdlSimple*>(pModelObj->GetModel()->GetScnObj());
            if (mParam.mInfo->mNodeName != nullptr) {
                nw4r::g3d::ResNode node = pScnMdl->GetResMdl().GetResNode(mParam.mInfo->mNodeName);
                if (node.IsValid()) {
                    nodeID = node.GetID();
                }
            }

            if (nodeID == 0xFFFFFFFF) {
                nw4r::math::MTX34 mtx;
                pScnMdl->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, 0);
                nw4r::math::VEC3 position(0.0f, pModelObj->GetSize().y, 0.0f);
                nw4r::math::VEC3Transform(&position, &mtx, &position);
                mPosition = position;
            } else {
                nw4r::math::MTX34 mtx;
                pScnMdl->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, nodeID);
                mPosition = nw4r::math::VEC3(mtx._03, mtx._13, mtx._23);
            }
        } else {
            mPosition = pFollow->GetPositionRef();
        }

        mEffect.GetPtr()->SetFlagDisableDraw(!pFollow->IsVisible());
        rotation = pFollow->GetRotation();
    } else {
        if (mFlags & FLAG_FOLLOW) {
            Finalize(FALSE);
            return;
        }
        mEffect.GetPtr()->SetFlagDisableDraw(false);
    }

    if (mFlags & FLAG_LOCAL_MTX) {
        if (mFlags & FLAG_FOLLOW_ROTATION) {
            mEffect.GetPtr()->SetRootMtx(Mult(Mult(MakeTranslationMtx(mPosition), MakeRotationMtx(rotation)), mLocalMtx));
        } else {
            mEffect.GetPtr()->SetRootMtx(Mult(MakeTranslationMtx(mPosition), mLocalMtx));
        }
    } else {
        if (std::tr1::shared_ptr<BWorldObj> pTarget = Lock(mTarget)) {
            BModelObj* pModelObj = DynamicCast<BModelObj>(pTarget.get());
            if (pModelObj != nullptr && pModelObj->GetModel()) {
                u32 nodeID = 0xFFFFFFFF;
                nw4r::g3d::ScnMdlSimple* pScnMdl =
                    static_cast<nw4r::g3d::ScnMdlSimple*>(pModelObj->GetModel()->GetScnObj());
                if (mParam.mInfo->mTargetNodeName != nullptr) {
                    nw4r::g3d::ResNode node = pScnMdl->GetResMdl().GetResNode(mParam.mInfo->mTargetNodeName);
                    if (node.IsValid()) {
                        nodeID = node.GetID();
                    }
                }

                if (nodeID == 0xFFFFFFFF) {
                    nw4r::math::MTX34 mtx;
                    pScnMdl->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, 0);
                    nw4r::math::VEC3 position(0.0f, pModelObj->GetSize().y, 0.0f);
                    nw4r::math::VEC3Transform(&position, &mtx, &position);
                    mTargetPosition = position;
                } else {
                    nw4r::math::MTX34 mtx;
                    pScnMdl->GetScnMtxPos(&mtx, nw4r::g3d::ScnObj::MTX_WORLD, nodeID);
                    mTargetPosition = nw4r::math::VEC3(mtx._03, mtx._13, mtx._23);
                }
            } else {
                mTargetPosition = pTarget->GetPositionRef();
            }
        } else if (mFlags & FLAG_TARGET) {
            Finalize(FALSE);
            return;
        }

        nw4r::math::VEC3 dir = mTargetPosition - mPosition;
        f32 lenSq = nw4r::math::VEC3LenSq(&dir);
        if (lenSq < 0.0001f) {
            mLocalMtx = nw4r::math::MTX34(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        } else {
            dir = dir * nw4r::math::FrSqrt(lenSq);
            nw4r::math::VEC3 side = Cross(nw4r::math::VEC3(0.0f, 1.0f, 0.0f), dir);
            lenSq = nw4r::math::VEC3LenSq(&side);
            if (0.0001f < lenSq) {
                side = side * nw4r::math::FrSqrt(lenSq);
                nw4r::math::VEC3 front = Cross(dir, side);
                mLocalMtx = nw4r::math::MTX34(side.x, front.x, dir.x, 0.0f, side.y, front.y, dir.y, 0.0f, side.z,
                                              front.z, dir.z, 0.0f);
            } else {
                mLocalMtx = nw4r::math::MTX34(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
            }
        }

        mEffect.GetPtr()->SetRootMtx(Mult(MakeTranslationMtx(mPosition), mLocalMtx));
    }

    if (!mParam.mEnable) {
        return;
    }

    switch (mParam.mInfo->mType) {
    case TYPE_STOP_EMISSION: {
        nw4r::ef::Emitter* pEmitter = mEffect.GetPtr()->GetFirstEmitter();
        if (pEmitter == nullptr) {
            mParam.mEnable = false;
            break;
        }

        u32 tick = pEmitter->mTick;
        if (mParam.mFrames.mLife <= tick) {
            pEmitter->mParameter.mParams[0] = pEmitter->mParameter.mParams[4] = mParam.mFrames.mRadius;
        } else {
            pEmitter->mParameter.mParams[0] = pEmitter->mParameter.mParams[4] =
                mParam.mFrames.mRadius * (static_cast<f32>(tick) / mParam.mFrames.mLife);
        }
        break;
    }
    case TYPE_EXTEND_EMISSION: {
        nw4r::ef::Emitter* pEmitter = mEffect.GetPtr()->GetFirstEmitter();
        if (pEmitter == nullptr) {
            mParam.mEnable = false;
            break;
        }

        f32 length = mParam.mFrames.mRadius * mParam.mFrames.mLife;
        u32 index = pEmitter->mTick % 10;
        f32 sin;
        f32 cos;
        nw4r::math::SinCosRad(&sin, &cos, mParam.mFrames.mHeight * sExtendEmissionOffsets[index].x);
        pEmitter->mParameter.mTranslate.x = (GetHandle() & 1) ? sin * length : -sin * length;
        pEmitter->mParameter.mTranslate.y = sExtendEmissionOffsets[index].y;
        pEmitter->mParameter.mTranslate.z = cos * length + (sExtendEmissionOffsets[index].z - 1.0f);
        pEmitter->SetMtxDirty();
        break;
    }
    }
}
