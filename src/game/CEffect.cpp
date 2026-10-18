#include <game/CEffect.h>
#include <game/BWorld.h>
#include <game/BWorldObj.h>
#include <game/Memory.h>
#include <game/Random.h>
#include <nw4r/ef/ef_effect.h>
#include <nw4r/ef/ef_effectsystem.h>
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/emform/ef_emform.h>
#include <nw4r/ef/ef_memorymanager.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/math/math_triangular.h>
#include <nw4r/ut/ut_list.h>
#include <new>

namespace {

inline u32 AlignHeap(u32 value) {
    return (value + 31) & ~31;
}

class EmitterWithParam : public nw4r::ef::Emitter {
public:
    EmitterWithParam() {
        mParam.mInfo = nullptr;
        mParam.mEnable = false;
    }

    CEffect::Param mParam;
};

void PrevEmissionLife(nw4r::ef::Emitter* pEmitter, nw4r::ef::ParticleManager* pManager, int* pCount, u32* pFlags,
                      f32 (*pParams)[nw4r::ef::NUM_PARAMS], u16* pLife, f32* pLifeRnd, nw4r::math::MTX34* pSpace);
void PrevEmissionLifeFollow(nw4r::ef::Emitter* pEmitter, nw4r::ef::ParticleManager* pManager, int* pCount, u32* pFlags,
                            f32 (*pParams)[nw4r::ef::NUM_PARAMS], u16* pLife, f32* pLifeRnd,
                            nw4r::math::MTX34* pSpace);

}

namespace nw4r {
namespace ef {

template <typename T> class MemoryManagerTmp {
public:
    MemoryManagerTmp(T* pBuffer, u32 num) {
        ut::List_Init(&mFreeList, 0);
        ut::List_Init(&mActiveList, 0);
        mObjectSize = sizeof(T);
        mBuffer = pBuffer;

        for (u32 i = 0; i < num; i++) {
            pBuffer->mObjectID = i;
            ut::List_Append(&mFreeList, pBuffer);
            pBuffer++;
        }
    }

    T* mBuffer;
    ut::List mFreeList;
    ut::List mActiveList;
    u32 mObjectSize;

    virtual ~MemoryManagerTmp() {}
};

class MemoryManager : public MemoryManagerBase {
public:
    struct HeapBlock {
        HeapBlock* mPrev;
        HeapBlock* mNext;
        HeapBlock* mPrevPhys;
        HeapBlock* mNextPhys;
        u32 mSize;
        bool mUsed;
    } __attribute__((aligned(32)));

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
        mFreeList->mSize = mHeapEnd - mHeapStart - sizeof(HeapBlock);
        mFirstBlock = mFreeList;
        mUsedList = nullptr;

        Effect* pEffects = nullptr;
        if (mNumEffect > 0) {
            pEffects = new (AllocHeap(sizeof(Effect) * mNumEffect + 0x20)) Effect[mNumEffect];
        }
        mEffectPool = new (AllocHeap(sizeof(MemoryManagerTmp<Effect>))) MemoryManagerTmp<Effect>(pEffects, mNumEffect);

        EmitterWithParam* pEmitters = nullptr;
        if (mNumEmitter > 0) {
            pEmitters = new (AllocHeap(sizeof(EmitterWithParam) * mNumEmitter + 0x20)) EmitterWithParam[mNumEmitter];
        }
        mEmitterPool = new (AllocHeap(sizeof(MemoryManagerTmp<EmitterWithParam>)))
            MemoryManagerTmp<EmitterWithParam>(pEmitters, mNumEmitter);

        ParticleManager* pParticleManagers = nullptr;
        if (mNumParticleManager > 0) {
            pParticleManagers =
                new (AllocHeap(sizeof(ParticleManager) * mNumParticleManager + 0x20)) ParticleManager[mNumParticleManager];
        }
        mParticleManagerPool = new (AllocHeap(sizeof(MemoryManagerTmp<ParticleManager>)))
            MemoryManagerTmp<ParticleManager>(pParticleManagers, mNumParticleManager);

        Particle* pParticles = nullptr;
        if (mNumParticle > 0) {
            pParticles = new (AllocHeap(sizeof(Particle) * mNumParticle + 0x20)) Particle[mNumParticle];
        }
        mParticlePool =
            new (AllocHeap(sizeof(MemoryManagerTmp<Particle>))) MemoryManagerTmp<Particle>(pParticles, mNumParticle);
    }

    virtual void GarbageCollection();

    virtual Effect* AllocEffect();
    virtual void FreeEffect(void* pObject);
    virtual u32 GetNumAllocEffect() const;
    virtual u32 GetNumActiveEffect() const;
    virtual u32 GetNumFreeEffect() const;

    virtual Emitter* AllocEmitter();
    virtual void FreeEmitter(void* pObject);
    virtual u32 GetNumAllocEmitter() const;
    virtual u32 GetNumActiveEmitter() const;
    virtual u32 GetNumFreeEmitter() const;

    virtual ParticleManager* AllocParticleManager();
    virtual void FreeParticleManager(void* pObject);
    virtual u32 GetNumAllocParticleManager() const;
    virtual u32 GetNumActiveParticleManager() const;
    virtual u32 GetNumFreeParticleManager() const;

    virtual Particle* AllocParticle();
    virtual void FreeParticle(void* pObject);
    virtual u32 GetNumAllocParticle() const;
    virtual u32 GetNumActiveParticle() const;
    virtual u32 GetNumFreeParticle() const;

    virtual void* AllocHeap(u32 size);
    virtual void FreeHeap(void* pPtr);

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
            HeapBlock* pRest = reinterpret_cast<HeapBlock*>(reinterpret_cast<u8*>(pBlock + 1) + blockSize);
            pRest->mPrev = pBlock->mPrev;
            pRest->mNext = pBlock->mNext;
            pRest->mSize = pBlock->mSize - blockSize - sizeof(HeapBlock);
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

        return pBlock + 1;
    }

    return nullptr;
}

void FinalizeEffect() {}

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

}

bool CEffect::Create(BWorld* pWorld, const Param& rParam, const nw4r::math::VEC3& rPosition, f32 rotation) {
    mPosition = rPosition;
    mFlags |= FLAG_LOCAL_MTX;
    SetRotation(rotation);
    return Create(pWorld, rParam);
}

bool CEffect::Create(BWorld* pWorld, const Param& rParam, BWorldObj* pFollow) {
    if (pFollow != nullptr) {
        mFollow = pFollow->shared_from_this();
    }
    mFlags |= FLAG_LOCAL_MTX;
    SetRotation(RandReal(0.0f, 360.0f));
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
