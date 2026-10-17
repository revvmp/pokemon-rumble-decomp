#ifndef NW4R_SND_SOUND_HANDLE_H
#define NW4R_SND_SOUND_HANDLE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicSound.h>

#include <nw4r/ut/ut_NonCopyable.h>

namespace nw4r {
namespace snd {

class SoundHandle : private ut::NonCopyable {
public:
    SoundHandle() : mSound(nullptr) {}
    ~SoundHandle() {
        DetachSound();
    }

    void detail_AttachSound(detail::BasicSound* pSound);
    void detail_AttachSoundAsTempHandle(detail::BasicSound* pSound);
    void DetachSound();

    bool IsAttachedSound() const {
        return mSound != nullptr;
    }

    detail::BasicSound* detail_GetAttachedSound() {
        return mSound;
    }
    const detail::BasicSound* detail_GetAttachedSound() const {
        return mSound;
    }

    bool IsPrepared() const {
        return IsAttachedSound() && mSound->IsPrepared();
    }
    bool IsPause() const {
        return IsAttachedSound() && mSound->IsPause();
    }

    u32 GetId() const {
        if (IsAttachedSound()) {
            return mSound->GetId();
        }

        return detail::BasicSound::INVALID_ID;
    }

    int GetRemainingFadeFrames() const {
        if (IsAttachedSound()) {
            return mSound->GetRemainingFadeFrames();
        }

        return 0;
    }

    void StartPrepared() {
        if (IsAttachedSound()) {
            mSound->StartPrepared();
        }
    }

    void Stop(int frames) {
        if (IsAttachedSound()) {
            mSound->Stop(frames);
        }
    }

    void Pause(bool flag, int frames) {
        if (IsAttachedSound()) {
            mSound->Pause(flag, frames);
        }
    }

    void FadeIn(int frames) {
        if (IsAttachedSound()) {
            mSound->FadeIn(frames);
        }
    }

    void SetVolume(f32 volume, int frames) {
        if (IsAttachedSound()) {
            mSound->SetVolume(volume, frames);
        }
    }

    void SetPitch(f32 pitch) {
        if (IsAttachedSound()) {
            mSound->SetPitch(pitch);
        }
    }

    void SetPan(f32 pan) {
        if (IsAttachedSound()) {
            mSound->SetPan(pan);
        }
    }

    void SetLpfFreq(f32 freq) {
        if (IsAttachedSound()) {
            mSound->SetLpfFreq(freq);
        }
    }

    void SetOutputLineFlag(int flag) {
        if (IsAttachedSound()) {
            mSound->SetOutputLineFlag(flag);
        }
    }

    void SetFxSend(AuxBus bus, f32 send) {
        if (IsAttachedSound()) {
            mSound->SetFxSend(bus, send);
        }
    }

private:
    detail::BasicSound* mSound;
};

}
}

#endif
