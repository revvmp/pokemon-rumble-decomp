#include <nw4r/snd/snd_SoundHandle.h>

namespace nw4r {
namespace snd {

void SoundHandle::detail_AttachSoundAsTempHandle(detail::BasicSound* pSound) {
    mSound = pSound;

    if (pSound->IsAttachedTempGeneralHandle()) {
        mSound->DetachTempGeneralHandle();
    }


    mSound->mTempGeneralHandle = this;
}

void SoundHandle::detail_AttachSound(detail::BasicSound* pSound) {
    mSound = pSound;

    if (pSound->IsAttachedGeneralHandle()) {
        mSound->DetachGeneralHandle();
    }

    mSound->mGeneralHandle = this;
}

void SoundHandle::DetachSound() {
    if (IsAttachedSound()) {
        if (mSound->mGeneralHandle == this) {
            mSound->mGeneralHandle = nullptr;
        }

        if (mSound->mTempGeneralHandle == this) {
            mSound->mTempGeneralHandle = nullptr;
        }
    }

    if (mSound != nullptr) {
        mSound = nullptr;
    }
}

}
}
