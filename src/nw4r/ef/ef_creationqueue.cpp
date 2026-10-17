#include <nw4r/ef/ef_creationqueue.h>
#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/ef_particle.h>
#include <nw4r/ef/ef_particlemanager.h>

namespace nw4r {
namespace ef {

CreationQueue::CreationQueue() : mNumItem(0) {}

void CreationQueue::AddParticleCreation(const EmitterInheritSetting* pSetting,
                                        Particle* pParticle,
                                        EmitterResource* pResource,
                                        u16 calcRemain,
                                        const math::VEC3* pTranslate,
                                        const math::VEC3* pExtraTranslate) {
    if (mNumItem >= QUEUE_SIZE) {
        return;
    }

    mQueueData[mNumItem].mType = CreationQueueData::TYPE_PARTICLE;
    mQueueData[mNumItem].mFlags = 0;
    mQueueData[mNumItem].mCalcRemain = calcRemain;
    mQueueData[mNumItem].mInheritSetting = *pSetting;
    mQueueData[mNumItem].mReferenceParticle = pParticle;

    pParticle->Ref();

    mQueueData[mNumItem].mEmitterResource = pResource;

    if (pTranslate != nullptr) {
        mQueueData[mNumItem].mFlags |= CreationQueueData::FLAG_TRANSLATE;
        mQueueData[mNumItem].mTranslate = *pTranslate;
    }

    if (pExtraTranslate != nullptr) {
        mQueueData[mNumItem].mFlags |= CreationQueueData::FLAG_EXTRA_TRANSLATE;
        mQueueData[mNumItem].mExtraTranslate = *pExtraTranslate;
    }

    mNumItem++;
}

void CreationQueue::AddEmitterCreation(const EmitterInheritSetting* pSetting,
                                       Particle* pParticle,
                                       EmitterResource* pResource,
                                       u16 calcRemain,
                                       const math::VEC3* pTranslate,
                                       const math::VEC3* pExtraTranslate) {
    if (mNumItem >= QUEUE_SIZE) {
        return;
    }

    mQueueData[mNumItem].mType = CreationQueueData::TYPE_EMITTER;
    mQueueData[mNumItem].mFlags = 0;
    mQueueData[mNumItem].mCalcRemain = calcRemain;
    mQueueData[mNumItem].mInheritSetting = *pSetting;
    mQueueData[mNumItem].mReferenceParticle = pParticle;

    pParticle->Ref();

    mQueueData[mNumItem].mEmitterResource = pResource;

    if (pTranslate != nullptr) {
        mQueueData[mNumItem].mFlags |= CreationQueueData::FLAG_TRANSLATE;
        mQueueData[mNumItem].mTranslate = *pTranslate;
    }

    if (pExtraTranslate != nullptr) {
        mQueueData[mNumItem].mFlags |= CreationQueueData::FLAG_EXTRA_TRANSLATE;
        mQueueData[mNumItem].mExtraTranslate = *pExtraTranslate;
    }

    mNumItem++;
}

void CreationQueue::Execute() {
    for (int i = 0; i < mNumItem; i++) {
        CreationQueueData& rData = mQueueData[i];

        math::VEC3* pTranslate = nullptr;
        if (rData.mFlags & CreationQueueData::FLAG_TRANSLATE) {
            pTranslate = &rData.mTranslate;
        }

        switch (rData.mType) {
        case CreationQueueData::TYPE_PARTICLE: {
            rData.mReferenceParticle->mParticleManager->mManagerEM
                ->CreateEmitterTmp(rData.mEmitterResource,
                                   &rData.mInheritSetting,
                                   rData.mReferenceParticle, rData.mCalcRemain,
                                   pTranslate);

            rData.mReferenceParticle->UnRef();
            break;
        }

        case CreationQueueData::TYPE_EMITTER: {
            rData.mReferenceParticle->mParticleManager->mManagerEM
                ->CreateEmitter(rData.mEmitterResource, &rData.mInheritSetting,
                                rData.mReferenceParticle, rData.mCalcRemain,
                                pTranslate);

            rData.mReferenceParticle->UnRef();
            break;
        }
        }
    }

    mNumItem = 0;
}

}
}
