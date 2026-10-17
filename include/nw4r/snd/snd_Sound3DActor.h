#ifndef NW4R_SND_SOUND_3D_ACTOR_H
#define NW4R_SND_SOUND_3D_ACTOR_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_SoundActor.h>

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace snd {

class Sound3DManager;
class SoundArchivePlayer;

class Sound3DActor : public SoundActor,
                     public detail::BasicSound::AmbientArgUpdateCallback {
public:
    Sound3DActor(SoundArchivePlayer& rPlayer, Sound3DManager& rManager);
    virtual ~Sound3DActor();

    virtual StartResult SetupSound(SoundHandle* pHandle, u32 soundId,
                                   const StartInfo* pStartInfo, void* pArg);

    virtual void detail_UpdateAmbientArg(void* pArg,
                                         const detail::BasicSound* pSound);

    void SetPosition(const math::VEC3& rPosition);
    const math::VEC3& GetPosition() const {
        return mPosition;
    }
    const math::VEC3& GetVelocity() const {
        return mVelocity;
    }

    void SetUserParam(u32 param) {
        mUserParam = param;
    }
    u32 GetUserParam() const {
        return mUserParam;
    }

private:
    static void ClearUpdateCallback(SoundHandle& rHandle);

private:
    Sound3DManager& m3DManager;
    SoundArchivePlayer* mSoundArchivePlayer;
    u32 mUserParam;
    math::VEC3 mPosition;
    math::VEC3 mVelocity;
    bool mResetPositionFlag;
};

}
}

#endif
