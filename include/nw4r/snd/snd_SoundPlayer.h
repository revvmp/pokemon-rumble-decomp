#ifndef NW4R_SND_SOUND_PLAYER_H
#define NW4R_SND_SOUND_PLAYER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_PlayerHeap.h>
#include <nw4r/snd/snd_Types.h>

#include <revolution/wpad.h>

namespace nw4r {
namespace snd {

class SoundPlayer {
public:
    SoundPlayer();
    ~SoundPlayer();

    void Update();
    void StopAllSound(int frames);
    void PauseAllSound(bool flag, int frames);

    int GetPlayingSoundCount() const {
        return mSoundList.GetSize();
    }

    int GetPlayableSoundCount() const {
        return mPlayableCount;
    }
    void SetPlayableSoundCount(int count);

    f32 GetVolume() const {
        return mVolume;
    }
    void SetVolume(f32 volume);

    f32 GetLpfFreq() const {
        return mLpfFreq;
    }
    void SetLpfFreq(f32 freq);

    int GetDefaultOutputLine() const {
        return mOutputLineFlag;
    }

    f32 GetMainOutVolume() const {
        return mMainOutVolume;
    }

    int GetBiquadFilterType() const {
        return mBiquadType;
    }
    f32 GetBiquadFilterValue() const {
        return mBiquadValue;
    }

    f32 GetRemoteOutVolume(int remote) const;

    f32 GetMainSend() const {
        return mMainSend;
    }

    f32 GetFxSend(int bus) const {
        return mFxSend[bus];
    }
    void SetFxSend(AuxBus bus, f32 send);

    detail::BasicSound* GetLowestPrioritySound() {
        return &mPriorityList.GetFront();
    }

    void RemoveSoundList(detail::BasicSound* pSound);
    void InsertPriorityList(detail::BasicSound* pSound);
    void RemovePriorityList(detail::BasicSound* pSound);

    bool detail_AppendSound(detail::BasicSound* pSound);
    void detail_RemoveSound(detail::BasicSound* pSound);
    void detail_SetPlayableSoundLimit(int limit);
    bool detail_CanPlaySound(int startPriority);
    void detail_SortPriorityList(detail::BasicSound* pSound);
    void detail_SortPriorityList();

    detail::PlayerHeap* detail_AllocPlayerHeap(detail::BasicSound* pSound);
    void detail_FreePlayerHeap(detail::BasicSound* pSound);
    void detail_AppendPlayerHeap(detail::PlayerHeap* pHeap);

private:
    detail::BasicSoundPlayerPlayList mSoundList;
    detail::BasicSoundPlayerPrioList mPriorityList;
    detail::PlayerHeapList mHeapList;
    int mPlayableCount;
    int mPlayableLimit;
    f32 mVolume;
    f32 mLpfFreq;
    int mOutputLineFlag;
    f32 mMainOutVolume;
    int mBiquadType;
    f32 mBiquadValue;
    f32 mRemoteOutVolume[WPAD_MAX_CONTROLLERS];
    f32 mMainSend;
    f32 mFxSend[AUX_BUS_NUM];
};

}
}

#endif
