#ifndef NW4R_EF_CREATION_QUEUE_H
#define NW4R_EF_CREATION_QUEUE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_emitter.h>
#include <nw4r/ef/ef_types.h>

namespace nw4r {
namespace ef {

struct CreationQueue {
private:
    struct CreationQueueData {
        enum Type { TYPE_PARTICLE, TYPE_EMITTER };

        u8 mType;
        u8 mFlags;
        u16 mCalcRemain;
        EmitterInheritSetting mInheritSetting;
        Particle* mReferenceParticle;
        EmitterResource* mEmitterResource;
        math::VEC3 mTranslate;
        math::VEC3 mExtraTranslate;

        enum Flag {
            FLAG_TRANSLATE = (1 << 0),
            FLAG_EXTRA_TRANSLATE = (1 << 1),
        };
    };

private:
    static const int QUEUE_SIZE =
        (NW4R_EF_MAX_PARTICLE > NW4R_EF_MAX_EMITTER ? NW4R_EF_MAX_PARTICLE : NW4R_EF_MAX_EMITTER);

private:
    int mNumItem;
    CreationQueueData mQueueData[QUEUE_SIZE];

public:
    CreationQueue();

    void AddParticleCreation(const EmitterInheritSetting* pSetting,
                             Particle* pParticle, EmitterResource* pResource,
                             u16 calcRemain,
                             const math::VEC3* pTranslate,
                             const math::VEC3* pExtraTranslate);

    void AddEmitterCreation(const EmitterInheritSetting* pSetting,
                            Particle* pParticle, EmitterResource* pResource,
                            u16 calcRemain,
                            const math::VEC3* pTranslate,
                            const math::VEC3* pExtraTranslate);

    void Execute();

    bool IsEmpty() {
        return mNumItem == 0;
    }
};

}
}

#endif
