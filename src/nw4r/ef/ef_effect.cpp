#include <nw4r/ef/ef_effect.h>
#include <nw4r/ef/ef_effectsystem.h>
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/ef_memorymanager.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_particlemanager.h>
#include <nw4r/ef/ef_util.h>

namespace nw4r {
namespace ef {

Effect::Effect() : mActivityList(offsetof(Emitter, mActivityLink)) {
    ut::List_Init(&mParticleManager, offsetof(ParticleManager, mDrawOrderLink));
}

Effect::~Effect() {
    RetireEmitterAll();
}

bool Effect::Initialize(EffectSystem* pSystem, EmitterResource* pResource, u16 calcRemain) {
    ReferencedObject::Initialize();
    mActivityList.Initialize();

    mDrawOrderFunc = pSystem->mDrawOrderFunc;
    math::MTX34Identity(&mRootMtx);
    mManagerES = pSystem;

    mCallBack.mPrevEmission = nullptr;
    mCallBack.mPrevPtclCalc = nullptr;
    mCallBack.mPostPtclCalc = nullptr;

    mFlags = 0;

    mVelocity.x = 0.0f;
    mVelocity.y = 0.0f;
    mVelocity.z = 0.0f;

    Emitter* pEmitter = pSystem->GetMemoryManager()->AllocEmitter();
    if (pEmitter == nullptr) {
        return false;
    }

    if (!pEmitter->Initialize(this, pResource, 128)) {
        pSystem->GetMemoryManager()->FreeEmitter(pEmitter);
        return false;
    }

    mActivityList.ToActive(pEmitter);
    pEmitter->mLifeStatus = NW4R_EF_LS_ACTIVE;
    pEmitter->mCalcRemain += calcRemain;

    return true;
}

void Effect::SendClosing() {
    mManagerES->Closing(this);
}

void Effect::DestroyFunc() {
    RetireEmitterAll();
}

bool Effect::Closing(Emitter* pEmitter) {
    if (pEmitter->mParent != nullptr) {
        pEmitter->mParent->UnRef();
    }

    if (pEmitter->mpReferenceParticle != nullptr) {
        pEmitter->mpReferenceParticle->UnRef();
        pEmitter->mpReferenceParticle = nullptr;
    }

    pEmitter->mManagerEF->UnRef();

    mActivityList.ToClosing(pEmitter);
    pEmitter->mLifeStatus = NW4R_EF_LS_CLOSING;

    return true;
}

Emitter* Effect::CreateEmitter(EmitterResource* pResource, u8 drawWeight, u16 calcRemain) {
    if (!(pResource->GetEmitterDesc()->commonFlag & EmitterDesc::CMN_FLAG_RELOCATED)) {
        return nullptr;
    }

    Emitter* pEmitter = mManagerES->GetMemoryManager()->AllocEmitter();
    if (pEmitter == nullptr) {
        return nullptr;
    }

    if (!pEmitter->Initialize(this, pResource, drawWeight)) {
        mManagerES->GetMemoryManager()->FreeEmitter(pEmitter);
        return nullptr;
    }

    mActivityList.ToActive(pEmitter);
    pEmitter->mLifeStatus = NW4R_EF_LS_ACTIVE;
    pEmitter->mCalcRemain += calcRemain;

    return pEmitter;
}

u32 Effect::RetireEmitter(Emitter* pEmitter) {
    if (pEmitter->mLifeStatus != NW4R_EF_LS_ACTIVE) {
        return 0;
    }

    mActivityList.ToWait(pEmitter);
    pEmitter->Destroy();
    return 1;
}

u32 Effect::RetireEmitterAll() {
    u32 count = 0;
    void* emitters[NW4R_EF_MAX_EMITTER];
    u16 size = UtlistToArray(&mActivityList.mActiveList, emitters, UtlistSize(&mActivityList.mActiveList));

    for (u16 i = 0; i < size; i++) {
        Emitter* pEmitter = static_cast<Emitter*>(emitters[i]);

        if (pEmitter->mLifeStatus == NW4R_EF_LS_ACTIVE) {
            count += RetireEmitter(pEmitter);
        }
    }

    return count;
}

u32 Effect::RetireParticleAll() {
    u32 count = 0;
    void* emitters[NW4R_EF_MAX_EMITTER];
    u16 size = UtlistToArray(&mActivityList.mActiveList, emitters, UtlistSize(&mActivityList.mActiveList));

    for (u16 i = 0; i < size; i++) {
        count += static_cast<Emitter*>(emitters[i])->RetireParticleAll();
    }

    return count;
}

void Effect::Calc(u32 calcFlags) {
    if (calcFlags & CALC_FLAG_ONLY_BILLBOARD) {
        Emitter* pEmitter = nullptr;

        while ((pEmitter = static_cast<Emitter*>(ut::List_GetNext(&mActivityList.mActiveList, pEmitter))) != nullptr) {
            if (pEmitter->mLifeStatus == NW4R_EF_LS_ACTIVE || pEmitter->mLifeStatus == NW4R_EF_LS_WAIT) {
                if (!(pEmitter->mParameter.mEmitFlags & EmitterDesc::EMIT_FLAG_16) &&
                    !(pEmitter->mParameter.mEmitFlags & EmitterDesc::EMIT_FLAG_15)) {
                    continue;
                }

                pEmitter->CalcBillboard();
            }
        }

        return;
    }

    if (GetFlagDisableCalc()) {
        return;
    }

    if (!(calcFlags & CALC_FLAG_SKIP_PREPARE)) {
        Emitter* pEmitter = nullptr;

        while ((pEmitter = static_cast<Emitter*>(ut::List_GetNext(&mActivityList.mActiveList, pEmitter))) != nullptr) {
            if (pEmitter->mLifeStatus == NW4R_EF_LS_ACTIVE || pEmitter->mLifeStatus == NW4R_EF_LS_WAIT) {
                if (pEmitter->mEvalStatus == NW4R_EF_ES_DONE) {
                    pEmitter->mEvalStatus = NW4R_EF_ES_WAIT;
                }
            }

            pEmitter->mPrevGlobalTranslate = pEmitter->mGlobalTranslate;

            ParticleManager* pManager = nullptr;

            while ((pManager = static_cast<ParticleManager*>(ut::List_GetNext(&pEmitter->mActivityList.mActiveList, pManager))) != nullptr) {
                pManager->BeginCalc(false);
            }
        }
    }

    SetFlagExistCalcRemain(true);

    while (GetFlagExistCalcRemain()) {
        void* emitters[NW4R_EF_MAX_EMITTER];

        SetFlagExistCalcRemain(false);

        while (true) {
            {
                u16 size = UtlistToArray(&mActivityList.mActiveList, emitters, UtlistSize(&mActivityList.mActiveList));

                for (u16 i = 0; i < size; i++) {
                    static_cast<Emitter*>(emitters[i])->CalcEmitter();
                }
            }

            {
                Emitter* pEmitter = nullptr;

                while ((pEmitter = static_cast<Emitter*>(ut::List_GetNext(&mActivityList.mActiveList, pEmitter))) != nullptr) {
                    if (pEmitter->mLifeStatus == NW4R_EF_LS_ACTIVE || pEmitter->mLifeStatus == NW4R_EF_LS_WAIT) {
                        if (!(pEmitter->mParameter.mEmitFlags & EmitterDesc::EMIT_FLAG_16) &&
                            !(pEmitter->mParameter.mEmitFlags & EmitterDesc::EMIT_FLAG_15)) {
                            continue;
                        }

                        pEmitter->CalcBillboard();
                    }
                }
            }

            {
                u16 size = UtlistToArray(&mActivityList.mActiveList, emitters, UtlistSize(&mActivityList.mActiveList));

                for (u16 i = 0; i < size; i++) {
                    Emitter* pEmitter = static_cast<Emitter*>(emitters[i]);

                    if (pEmitter->mEvalStatus == NW4R_EF_ES_WAIT) {
                        pEmitter->mTick++;
                    }

                    pEmitter->CalcParticle();

                    if (pEmitter->mEvalStatus == NW4R_EF_ES_WAIT) {
                        pEmitter->mTick--;
                    }
                }
            }

            {
                u16 size = UtlistToArray(&mActivityList.mActiveList, emitters, UtlistSize(&mActivityList.mActiveList));

                for (u16 i = 0; i < size; i++) {
                    static_cast<Emitter*>(emitters[i])->CalcEmission();
                }
            }

            {
                u16 size = UtlistToArray(&mActivityList.mActiveList, emitters, UtlistSize(&mActivityList.mActiveList));

                for (u16 i = 0; i < size; i++) {
                    static_cast<Emitter*>(emitters[i])->CalcParticle();
                }
            }

            if (mManagerES->mCreationQueue.IsEmpty()) {
                break;
            }

            mManagerES->mCreationQueue.Execute();
        }

        if (GetFlagExistCalcRemain()) {
            Emitter* pEmitter = nullptr;

            while ((pEmitter = static_cast<Emitter*>(ut::List_GetNext(&mActivityList.mActiveList, pEmitter))) != nullptr) {
                if (pEmitter->mLifeStatus == NW4R_EF_LS_ACTIVE && pEmitter->mCalcRemain != 0) {
                    if (pEmitter->mCalcRemain != 0) {
                        pEmitter->mCalcRemain--;
                    }

                    if (pEmitter->mEvalStatus == NW4R_EF_ES_DONE) {
                        pEmitter->mEvalStatus = NW4R_EF_ES_WAIT;
                    }
                }

                ParticleManager* pManager = nullptr;

                while ((pManager = static_cast<ParticleManager*>(ut::List_GetNext(&pEmitter->mActivityList.mActiveList, pManager))) != nullptr) {
                    pManager->BeginCalc(true);
                }
            }
        }
    }

    Emitter* pEmitter = nullptr;

    while ((pEmitter = static_cast<Emitter*>(ut::List_GetNext(&mActivityList.mActiveList, pEmitter))) != nullptr) {
        if (pEmitter->mLifeStatus == NW4R_EF_LS_ACTIVE || pEmitter->mLifeStatus == NW4R_EF_LS_WAIT) {
            if (pEmitter->mEvalStatus == NW4R_EF_ES_SKIP) {
                pEmitter->mEvalStatus = NW4R_EF_ES_DONE;
            }
        }

        ParticleManager* pManager = nullptr;

        while ((pManager = static_cast<ParticleManager*>(ut::List_GetNext(&pEmitter->mActivityList.mActiveList, pManager))) != nullptr) {
            pManager->EndCalc();
        }

        math::MTX34 globalMtx;
        pEmitter->CalcGlobalMtx(&globalMtx);
        pEmitter->mGlobalTranslate.x = globalMtx._03;
        pEmitter->mGlobalTranslate.y = globalMtx._13;
        pEmitter->mGlobalTranslate.z = globalMtx._23;
    }
}

void Effect::Draw(const DrawInfo& rInfo) {
    if (GetFlagDisableDraw()) {
        return;
    }

    mDrawOrderFunc->Draw(this, rInfo);
}

Emitter* Effect::GetFirstEmitter() {
    return static_cast<Emitter*>(mActivityList.mActiveList.headObject);
}

void Effect::SetRootMtx(const math::MTX34& rMtx) {
    math::MTX34Copy(&mRootMtx, &rMtx);

    Emitter* pEmitter = nullptr;

    while ((pEmitter = static_cast<Emitter*>(ut::List_GetNext(&mActivityList.mActiveList, pEmitter))) != nullptr) {
        if (pEmitter->mParent == nullptr) {
            pEmitter->SetMtxDirty();
        }
    }
}

}
}
