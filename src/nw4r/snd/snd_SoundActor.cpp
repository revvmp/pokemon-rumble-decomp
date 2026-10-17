#include <nw4r/snd/snd_SoundActor.h>

#include <nw4r/snd/snd_SoundArchivePlayer.h>

#include <limits.h>

namespace nw4r {
namespace snd {

SoundActor::SoundActor(SoundArchivePlayer& rPlayer)
    : mSoundArchivePlayer(rPlayer) {
    for (int i = 0; i < ACTOR_PLAYER_COUNT; i++) {
        mActorPlayer[i].SetPlayableSoundCount(i == 0 ? INT_MAX : 1);
    }
}

SoundActor::~SoundActor() {
    for (int i = 0; i < ACTOR_PLAYER_COUNT; i++) {
        mActorPlayer[i].DetachSoundActorAll(this);
    }
}

void SoundActor::StopAllSound(int frames) {
    for (int i = 0; i < ACTOR_PLAYER_COUNT; i++) {
        mActorPlayer[i].StopAllSound(frames);
    }
}

void SoundActor::SetPlayableSoundCount(int actorPlayerId, int count) {
    mActorPlayer[actorPlayerId].SetPlayableSoundCount(count);
}

SoundStartable::StartResult
SoundActor::SetupSound(SoundHandle* pHandle, u32 soundId,
                       const StartInfo* pStartInfo, void* pArg) {
    return mSoundArchivePlayer.detail_SetupSoundImpl(
        pHandle, soundId, nullptr, this, *static_cast<bool*>(pArg),
        pStartInfo);
}

SoundStartable::StartResult SoundActor::detail_SetupSoundWithAmbientInfo(
    SoundHandle* pHandle, u32 soundId, const StartInfo* pStartInfo,
    detail::BasicSound::AmbientInfo* pAmbientInfo, void* pArg) {
    return mSoundArchivePlayer.detail_SetupSoundImpl(
        pHandle, soundId, pAmbientInfo, this, *static_cast<bool*>(pArg),
        pStartInfo);
}

SoundStartable::StartResult
SoundActor::detail_SetupSound(SoundHandle* pHandle, u32 soundId, bool hold,
                              const StartInfo* pStartInfo) {
    return SetupSound(pHandle, soundId, pStartInfo, &hold);
}

u32 SoundActor::detail_ConvertLabelStringToSoundId(const char* pLabel) {
    return mSoundArchivePlayer.detail_ConvertLabelStringToSoundId(pLabel);
}

}
}
