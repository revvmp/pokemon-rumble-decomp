#include <game/audio/Sound.h>

#include <functional>
#include <game/io/CFileData.h>
#include <game/core/ResetCallback.h>
#include <memory>
#include <nw4r/snd/snd_MemorySoundArchive.h>
#include <nw4r/snd/snd_FxChorus.h>
#include <nw4r/snd/snd_FxDelay.h>
#include <nw4r/snd/snd_FxReverbStd.h>
#include <nw4r/snd/snd_Sound3DListener.h>
#include <nw4r/snd/snd_Sound3DManager.h>
#include <nw4r/snd/snd_SoundActor.h>
#include <nw4r/snd/snd_SoundArchivePlayer.h>
#include <nw4r/snd/snd_SoundHandle.h>
#include <nw4r/snd/snd_SoundSystem.h>
#include <nw4r/snd/snd_debug.h>
#include <revolution/ai.h>
#include <revolution/ax/AX.h>

#include <game/core/Memory.h>

namespace {

class SoundWork {
public:
    std::tr1::shared_ptr<u8> mSystemMemory;
    nw4r::snd::FxReverbStd mFxReverb;
    nw4r::snd::FxChorus mFxChorus;
    nw4r::snd::FxDelay mFxDelay;
    std::tr1::shared_ptr<u8> mFxReverbMemory;
    std::tr1::shared_ptr<u8> mFxChorusMemory;
    std::tr1::shared_ptr<u8> mFxDelayMemory;
    std::tr1::shared_ptr<CFileData> mArchiveFile;
    nw4r::snd::MemorySoundArchive mArchive;
    nw4r::snd::SoundArchivePlayer mPlayer;
    std::tr1::shared_ptr<u8> mPlayerMemory;
    std::tr1::shared_ptr<u8> mStrmBuffer;
    nw4r::snd::Sound3DManager m3DManager;
    nw4r::snd::Sound3DListener m3DListener;
    std::tr1::shared_ptr<u8> m3DManagerMemory;
    nw4r::snd::SoundHandle mSeHandle;
    nw4r::snd::SoundHandle mBgmHandles[2];
    nw4r::snd::SoundHandle mSpareHandle;
};

int sBgmIndex;
std::tr1::shared_ptr<SoundWork> sSoundWork;
bool sBgmDisabled;

const f32 DEFAULT_FADE_TIME = 3.0f;

int SecondsToFrames(f32 seconds) {
    return 60.0f * seconds;
}

nw4r::snd::detail::BasicSound* GetBgmSound() {
    return sSoundWork->mBgmHandles[sBgmIndex].detail_GetAttachedSound();
}

void OnReset() {
    nw4r::snd::SoundSystem::PrepareReset();
    nw4r::snd::SoundSystem::WaitForResetReady();
}

}

void InitSound(u32 reverbSize, u32 chorusSize, u32 delaySize) {
    nw4r::snd::Debug_SetWarningFlag(nw4r::snd::DEBUG_WARNING_NOT_ENOUGH_INSTANCE, true);
    sSoundWork.reset(new SoundWork);

    AIInit(nullptr);
    AXInit();

    nw4r::snd::SoundSystem::SoundSystemParam param;
    u32 systemSize = nw4r::snd::SoundSystem::GetRequiredMemSize(param);
    sSoundWork->mSystemMemory.reset(static_cast<u8*>(GameMemory::Allocate(systemSize)), GameMemory::Free);
    nw4r::snd::SoundSystem::InitSoundSystem(param, sSoundWork->mSystemMemory.get(), systemSize);

    if (reverbSize == 0) {
        reverbSize = sSoundWork->mFxReverb.GetRequiredMemSize();
    }
    sSoundWork->mFxReverbMemory.reset(static_cast<u8*>(GameMemory::Allocate(reverbSize)), GameMemory::Free);
    sSoundWork->mFxReverb.AssignWorkBuffer(sSoundWork->mFxReverbMemory.get(), reverbSize);

    if (chorusSize == 0) {
        chorusSize = sSoundWork->mFxChorus.GetRequiredMemSize();
    }
    sSoundWork->mFxChorusMemory.reset(static_cast<u8*>(GameMemory::Allocate(chorusSize)), GameMemory::Free);
    sSoundWork->mFxChorus.AssignWorkBuffer(sSoundWork->mFxChorusMemory.get(), chorusSize);

    if (delaySize == 0) {
        delaySize = sSoundWork->mFxDelay.GetRequiredMemSize();
    }
    sSoundWork->mFxDelayMemory.reset(static_cast<u8*>(GameMemory::Allocate(delaySize)), GameMemory::Free);
    sSoundWork->mFxDelay.AssignWorkBuffer(sSoundWork->mFxDelayMemory.get(), delaySize);

    if (nw4r::snd::SoundSystem::GetOutputMode() == nw4r::snd::OUTPUT_MODE_DPL2) {
        nw4r::snd::SoundSystem::SetOutputMode(nw4r::snd::OUTPUT_MODE_SURROUND);
    }

    RegisterResetCallback(OnReset, 0);
}

void ShutdownSound() {
    sSoundWork->mPlayer.Shutdown();
    nw4r::snd::SoundSystem::ShutdownSoundSystem();
    sSoundWork.reset();
}

void UpdateSound() {
    sSoundWork->mPlayer.Update();
}

void LoadSoundArchive(const char* pPath) {
    sSoundWork->mArchiveFile = LoadFile(pPath, 0, true);
    sSoundWork->mArchive.Setup(sSoundWork->mArchiveFile->GetData());

    u32 playerSize = sSoundWork->mPlayer.GetRequiredMemSize(&sSoundWork->mArchive);
    u32 strmSize = sSoundWork->mPlayer.GetRequiredStrmBufferSize(&sSoundWork->mArchive);
    sSoundWork->mPlayerMemory.reset(static_cast<u8*>(GameMemory::Allocate(playerSize)), GameMemory::Free);
    sSoundWork->mStrmBuffer.reset(static_cast<u8*>(GameMemory::Allocate(strmSize)), GameMemory::Free);
    sSoundWork->mPlayer.Setup(&sSoundWork->mArchive, sSoundWork->mPlayerMemory.get(), playerSize,
                              sSoundWork->mStrmBuffer.get(), strmSize);

    u32 managerSize = sSoundWork->m3DManager.GetRequiredMemSize(&sSoundWork->mArchive);
    sSoundWork->m3DManagerMemory.reset(static_cast<u8*>(GameMemory::Allocate(managerSize)), GameMemory::Free);
    sSoundWork->m3DManager.Setup(&sSoundWork->mArchive, sSoundWork->m3DManagerMemory.get(), managerSize);
    sSoundWork->m3DManager.SetMaxPriorityReduction(32);
    sSoundWork->m3DManager.AddListener(&sSoundWork->m3DListener);
    sSoundWork->m3DListener.SetMaxVolumeDistance(12.0f);
    sSoundWork->m3DListener.SetUnitDistance(6.0f);
    sSoundWork->m3DListener.SetInteriorSize(20.0f);
}

void SetBGMVolume(f32 volume, f32 fadeTime) {
    if (sBgmDisabled) {
        return;
    }
    nw4r::snd::detail::BasicSound* pSound = GetBgmSound();
    if (pSound != nullptr) {
        pSound->SetVolume(volume, SecondsToFrames(fadeTime));
    }
}

void StopBGM(f32 fadeTime) {
    if (sBgmDisabled) {
        return;
    }
    if (fadeTime == -1.0f) {
        fadeTime = DEFAULT_FADE_TIME;
    }
    nw4r::snd::detail::BasicSound* pSound = GetBgmSound();
    if (pSound != nullptr) {
        pSound->Pause(true, SecondsToFrames(fadeTime));
    }
}

u32 GetBGMID() {
    if (sBgmDisabled) {
        return nw4r::snd::detail::BasicSound::INVALID_ID;
    }
    nw4r::snd::detail::BasicSound* pSound = GetBgmSound();
    if (pSound != nullptr) {
        return pSound->GetId();
    }
    return nw4r::snd::detail::BasicSound::INVALID_ID;
}

void ContinueBGM(u32 id, f32 fadeTime) {
    PlayBGM(id, fadeTime, true);
}

void PlayBGM(u32 id, f32 fadeTime, bool resume) {
    if (sBgmDisabled) {
        return;
    }

    StopBGM(fadeTime);
    sBgmIndex = (sBgmIndex + 1) % 2;
    if (resume && GetBGMID() != id) {
        resume = false;
    }

    if (fadeTime == -1.0f) {
        fadeTime = DEFAULT_FADE_TIME;
    }

    if (!resume) {
        nw4r::snd::detail::BasicSound* pSound = GetBgmSound();
        if (pSound != nullptr) {
            pSound->Stop(0);
        }
        pSound = GetBgmSound();
        if (pSound != nullptr) {
            pSound->FadeIn(SecondsToFrames(fadeTime));
        }
        sSoundWork->mPlayer.detail_StartSound(&sSoundWork->mBgmHandles[sBgmIndex], id, nullptr);
    } else {
        nw4r::snd::detail::BasicSound* pSound = GetBgmSound();
        if (pSound != nullptr) {
            pSound->Pause(false, SecondsToFrames(fadeTime));
        }
    }
}

void PlaySound(u32 id, nw4r::snd::SoundHandle* pHandle, nw4r::snd::SoundStartable* pStartable) {
    if (pHandle == nullptr) {
        pHandle = &sSoundWork->mSeHandle;
    }
    if (pStartable == nullptr) {
        sSoundWork->mPlayer.detail_StartSound(pHandle, id, nullptr);
    } else {
        pStartable->detail_StartSound(pHandle, id, nullptr);
    }
}

void StopSound(nw4r::snd::SoundHandle* pHandle, nw4r::snd::SoundActor* pActor, f32 fadeTime) {
    if (pActor != nullptr) {
        pActor->StopAllSound(SecondsToFrames(fadeTime));
        return;
    }
    if (pHandle == nullptr) {
        pHandle = &sSoundWork->mSeHandle;
    }
    nw4r::snd::detail::BasicSound* pSound = pHandle->detail_GetAttachedSound();
    if (pSound != nullptr) {
        pSound->Stop(SecondsToFrames(fadeTime));
    }
}

nw4r::snd::SoundArchivePlayer& GetSoundArchivePlayer() {
    return sSoundWork->mPlayer;
}

nw4r::snd::Sound3DManager& GetSound3DManager() {
    return sSoundWork->m3DManager;
}

nw4r::snd::Sound3DListener& GetSound3DListener() {
    return sSoundWork->m3DListener;
}

nw4r::snd::FxReverbStd& GetFxReverb() {
    return sSoundWork->mFxReverb;
}

nw4r::snd::FxDelay& GetFxDelay() {
    return sSoundWork->mFxDelay;
}

nw4r::snd::FxChorus& GetFxChorus() {
    return sSoundWork->mFxChorus;
}
