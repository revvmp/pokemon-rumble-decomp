#include <nw4r/snd/snd_Sound3DActor.h>

#include <nw4r/snd/snd_Sound3DManager.h>
#include <nw4r/snd/snd_SoundArchive.h>
#include <nw4r/snd/snd_SoundArchivePlayer.h>
#include <nw4r/snd/snd_SoundHandle.h>

namespace nw4r {
namespace snd {

Sound3DActor::Sound3DActor(SoundArchivePlayer& rPlayer,
                           Sound3DManager& rManager)
    : SoundActor(rPlayer),
      m3DManager(rManager),
      mSoundArchivePlayer(&rPlayer),
      mUserParam(0),
      mPosition(0.0f, 0.0f, 0.0f),
      mVelocity(0.0f, 0.0f, 0.0f),
      mResetPositionFlag(true) {}

Sound3DActor::~Sound3DActor() {
    ForEachSound(ClearUpdateCallback);
}

SoundStartable::StartResult
Sound3DActor::SetupSound(SoundHandle* pHandle, u32 soundId,
                         const StartInfo* pStartInfo, void* pArg) {
    Sound3DParam param;
    param.position = mPosition;
    param.velocity = mVelocity;
    param.actorUserParam = mUserParam;

    if (mSoundArchivePlayer != nullptr) {
        const SoundArchive& rArchive = mSoundArchivePlayer->GetSoundArchive();

        SoundArchive::Sound3DParam archiveParam;
        if (rArchive.detail_ReadSound3DParam(soundId, &archiveParam)) {
            param.ctrlFlag = archiveParam.flags;
            param.decayRatio = archiveParam.decayRatio;
            param.dopplerFactor = archiveParam.spatialParameter;

            switch (archiveParam.decayCurve) {
            case Sound3DParam::DECAY_CURVE_LOGARITHMIC:
                param.decayCurve = Sound3DParam::DECAY_CURVE_LOGARITHMIC;
                break;

            case Sound3DParam::DECAY_CURVE_LINEAR:
                param.decayCurve = Sound3DParam::DECAY_CURVE_LINEAR;
                break;

            default:
                param.decayCurve = Sound3DParam::DECAY_CURVE_LOGARITHMIC;
                break;
            }
        }

        param.soundUserParam = rArchive.GetSoundUserParam(soundId);
    }

    detail::BasicSound::AmbientInfo ambientInfo = {
        &m3DManager, this, &m3DManager, &param, sizeof(Sound3DParam)};

    StartResult result = SoundActor::detail_SetupSoundWithAmbientInfo(
        pHandle, soundId, pStartInfo, &ambientInfo, pArg);

    if (pHandle->detail_GetAttachedSound() != nullptr) {
        pHandle->detail_GetAttachedSound()->SetPanCurve(detail::PAN_CURVE_SINCOS);
    }

    return result;
}

void Sound3DActor::SetPosition(const math::VEC3& rPosition) {
    if (!mResetPositionFlag) {
        math::VEC3Sub(&mVelocity, &rPosition, &mPosition);
    }

    mPosition = rPosition;
    mResetPositionFlag = false;
}

void Sound3DActor::detail_UpdateAmbientArg(void* pArg,
                                           const detail::BasicSound* pSound) {
    Sound3DParam* p3DParam = static_cast<Sound3DParam*>(pArg);

    p3DParam->position = mPosition;
    p3DParam->velocity = mVelocity;
    p3DParam->actorUserParam = mUserParam;
}

void Sound3DActor::ClearUpdateCallback(SoundHandle& rHandle) {
    if (rHandle.IsAttachedSound()) {
        rHandle.detail_GetAttachedSound()->ClearAmbientArgUpdateCallback();
    }
}

}
}
