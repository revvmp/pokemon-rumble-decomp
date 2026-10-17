#ifndef NW4R_SND_SOUND_ACTOR_H
#define NW4R_SND_SOUND_ACTOR_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_ExternalSoundPlayer.h>
#include <nw4r/snd/snd_SoundStartable.h>

namespace nw4r {
namespace snd {

class SoundArchivePlayer;

class SoundActor : public SoundStartable {
public:
    static const int ACTOR_PLAYER_COUNT = 4;

public:
    explicit SoundActor(SoundArchivePlayer& rPlayer);
    virtual ~SoundActor();

    virtual StartResult detail_SetupSound(SoundHandle* pHandle, u32 id,
                                          bool hold,
                                          const StartInfo* pStartInfo);
    virtual u32 detail_ConvertLabelStringToSoundId(const char* pLabel);
    virtual StartResult SetupSound(SoundHandle* pHandle, u32 id,
                                   const StartInfo* pStartInfo, void* pArg);
    virtual StartResult detail_SetupSoundWithAmbientInfo(
        SoundHandle* pHandle, u32 id, const StartInfo* pStartInfo,
        detail::BasicSound::AmbientInfo* pAmbientInfo, void* pArg);

    void StopAllSound(int frames);
    void PauseAllSound(bool flag, int frames);
    int GetPlayingSoundCount(int actorPlayerId) const;
    void SetPlayableSoundCount(int actorPlayerId, int count);

    template <typename TForEachFunc>
    TForEachFunc ForEachSound(TForEachFunc pFunc) {
        for (int i = 0; i < ACTOR_PLAYER_COUNT; i++) {
            mActorPlayer[i].ForEachSound(pFunc);
        }

        return pFunc;
    }

    detail::ExternalSoundPlayer* detail_GetActorPlayer(int actorPlayerId) {
        if (actorPlayerId < 0 || actorPlayerId >= ACTOR_PLAYER_COUNT) {
            return nullptr;
        }

        return &mActorPlayer[actorPlayerId];
    }

    const detail::SoundActorParam& detail_GetActorParam() const {
        return mActorParam;
    }

    f32 GetVolume() const {
        return mActorParam.volume;
    }
    void SetVolume(f32 volume) {
        mActorParam.volume = volume;
    }

private:
    SoundArchivePlayer& mSoundArchivePlayer;
    detail::ExternalSoundPlayer mActorPlayer[ACTOR_PLAYER_COUNT];
    detail::SoundActorParam mActorParam;
};

}
}

#endif
