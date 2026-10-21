#ifndef NW4R_SND_SOUND_3D_MANAGER_H
#define NW4R_SND_SOUND_3D_MANAGER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_InstancePool.h>
#include <nw4r/snd/snd_Sound3DListener.h>

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace snd {

class Sound3DEngine;
class SoundArchive;

struct Sound3DParam {
    enum ParamControlFlag {
        PARAM_CTRL_FLAG_NO_VOLUME = (1 << 0),
        PARAM_CTRL_FLAG_NO_PAN = (1 << 1),
        PARAM_CTRL_FLAG_NO_SURROUND_PAN = (1 << 2),
        PARAM_CTRL_FLAG_NO_PRIORITY = (1 << 3),
        PARAM_CTRL_FLAG_BIQUAD_FILTER = (1 << 4)
    };

    enum DecayCurve {
        DECAY_CURVE_LOGARITHMIC = 1,
        DECAY_CURVE_LINEAR = 2
    };

    Sound3DParam();

    math::VEC3 position;
    math::VEC3 velocity;
    u32 ctrlFlag;
    u8 decayCurve;
    u8 decayRatio;
    u8 dopplerFactor;
    u32 actorUserParam;
    u32 soundUserParam;
};

class Sound3DManager
    : public detail::BasicSound::AmbientParamUpdateCallback,
      public detail::BasicSound::AmbientArgAllocaterCallback {
public:
    Sound3DManager();

    virtual void detail_UpdateAmbientParam(const void* pArg, u32 soundId,
                                           int voiceOutCount,
                                           SoundAmbientParam* pParam);
    virtual int detail_GetAmbientPriority(const void* pArg, u32 soundId);
    virtual int detail_GetRequiredVoiceOutCount(const void* pArg,
                                                u32 soundId);

    virtual void* detail_AllocAmbientArg(u32 size);
    virtual void detail_FreeAmbientArg(void* pArg,
                                       const detail::BasicSound* pSound);

    u32 GetRequiredMemSize(const SoundArchive* pArchive);
    bool Setup(const SoundArchive* pArchive, void* pBuffer, u32 size);

    void AddListener(Sound3DListener* pListener) {
        mListenerList.PushBack(pListener);
    }

    Sound3DListenerList& GetListenerList() {
        return mListenerList;
    }
    const Sound3DListenerList& GetListenerList() const {
        return mListenerList;
    }

    int GetMaxPriorityReduction() const {
        return mMaxPriorityReduction;
    }
    void SetMaxPriorityReduction(int reduction) {
        mMaxPriorityReduction = reduction;
    }

    f32 GetPanRange() const {
        return mPanRange;
    }
    void SetPanRange(f32 range) {
        mPanRange = range;
    }

    f32 GetSonicVelocity() const {
        return mSonicVelocity;
    }
    void SetSonicVelocity(f32 velocity) {
        mSonicVelocity = velocity;
    }

    int GetBiquadFilterType() const {
        return mBiquadFilterType;
    }
    void SetBiquadFilterType(int type) {
        mBiquadFilterType = type;
    }

private:
    detail::InstancePool<Sound3DParam> mParamPool;
    Sound3DListenerList mListenerList;
    Sound3DEngine* mpEngine;
    int mMaxPriorityReduction;
    f32 mPanRange;
    f32 mSonicVelocity;
    int mBiquadFilterType;
};

}
}

#endif
