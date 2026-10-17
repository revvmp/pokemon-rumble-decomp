#include <nw4r/snd/snd_Sound3DManager.h>

#include <nw4r/snd/snd_Sound3DEngine.h>
#include <nw4r/snd/snd_SoundArchive.h>

#include <new>

namespace nw4r {
namespace snd {

namespace {

Sound3DEngine sDefaultEngine;

}

Sound3DManager::Sound3DManager()
    : mpEngine(&sDefaultEngine),
      mMaxPriorityReduction(32),
      mPanRange(0.9f),
      mSonicVelocity(0.0f),
      mBiquadFilterType(0) {}

u32 Sound3DManager::GetRequiredMemSize(const SoundArchive* pArchive) {
    u32 paramCount = 0;

    SoundArchive::SoundArchivePlayerInfo playerInfo;
    if (pArchive->ReadSoundArchivePlayerInfo(&playerInfo)) {
        paramCount += playerInfo.seqSoundCount;
        paramCount += playerInfo.strmSoundCount;
        paramCount += playerInfo.waveSoundCount;
    }

    return paramCount * sizeof(Sound3DParam);
}

bool Sound3DManager::Setup(const SoundArchive* pArchive, void* pBuffer,
                           u32 size) {
    mParamPool.Create(pBuffer, size);
    return true;
}

void Sound3DManager::detail_UpdateAmbientParam(const void* pArg, u32 soundId,
                                               int voiceOutCount,
                                               SoundAmbientParam* pParam) {
    const Sound3DParam* p3DParam = static_cast<const Sound3DParam*>(pArg);

    if (mpEngine != nullptr) {
        mpEngine->UpdateAmbientParam(this, p3DParam, soundId, voiceOutCount,
                                     pParam);
    }
}

int Sound3DManager::detail_GetAmbientPriority(const void* pArg, u32 soundId) {
    const Sound3DParam* p3DParam = static_cast<const Sound3DParam*>(pArg);
    int priority = 0;

    if (mpEngine != nullptr) {
        priority = mpEngine->GetAmbientPriority(this, p3DParam, soundId);
    }

    return priority;
}

int Sound3DManager::detail_GetRequiredVoiceOutCount(const void* pArg,
                                                    u32 soundId) {
    const Sound3DParam* p3DParam = static_cast<const Sound3DParam*>(pArg);
    int count = 1;

    if (mpEngine != nullptr) {
        count = mpEngine->GetRequiredVoiceOutCount(this, p3DParam, soundId);
    }

    return count;
}

void* Sound3DManager::detail_AllocAmbientArg(u32 size) {
    if (size != sizeof(Sound3DParam)) {
        return nullptr;
    }

    return mParamPool.Alloc();
}

void Sound3DManager::detail_FreeAmbientArg(void* pArg,
                                           const detail::BasicSound* pSound) {
    mParamPool.Free(static_cast<Sound3DParam*>(pArg));
}

Sound3DParam::Sound3DParam()
    : ctrlFlag(0),
      decayCurve(DECAY_CURVE_LOGARITHMIC),
      decayRatio(128),
      dopplerFactor(0),
      actorUserParam(0),
      soundUserParam(0) {}

}
}
