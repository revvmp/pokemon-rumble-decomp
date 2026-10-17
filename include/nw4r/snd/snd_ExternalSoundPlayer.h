#ifndef NW4R_SND_EXTERNAL_SOUND_PLAYER_H
#define NW4R_SND_EXTERNAL_SOUND_PLAYER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_SoundHandle.h>

namespace nw4r {
namespace snd {

class SoundActor;

namespace detail {

class ExternalSoundPlayer {
public:
    ExternalSoundPlayer();
    ~ExternalSoundPlayer();

    int GetPlayingSoundCount() const {
        return mSoundList.GetSize();
    }

    int GetPlayableSoundCount() const {
        return mPlayableCount;
    }
    void SetPlayableSoundCount(int count);

    void StopAllSound(int frames);
    void PauseAllSound(bool flag, int frames);
    void DetachSoundActorAll(SoundActor* pActor);

    bool AppendSound(BasicSound* pSound);
    void RemoveSound(BasicSound* pSound);

    bool detail_CanPlaySound(int startPriority);

    template <typename TForEachFunc>
    TForEachFunc ForEachSound(TForEachFunc pFunc) {
        BasicSoundExtPlayList::Iterator it = mSoundList.GetBeginIter();

        while (it != mSoundList.GetEndIter()) {
            BasicSoundExtPlayList::Iterator current = it++;

            SoundHandle handle;
            handle.detail_AttachSound(&*current);
            pFunc(handle);
        }

        return pFunc;
    }

private:
    BasicSound* GetLowestPrioritySound();

private:
    BasicSoundExtPlayList mSoundList;
    int mPlayableCount;
};

}
}
}

#endif
