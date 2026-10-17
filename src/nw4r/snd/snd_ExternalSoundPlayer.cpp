#include <nw4r/snd/snd_ExternalSoundPlayer.h>

#include <nw4r/snd/snd_SoundThread.h>

namespace nw4r {
namespace snd {
namespace detail {

ExternalSoundPlayer::ExternalSoundPlayer() : mPlayableCount(1) {}

ExternalSoundPlayer::~ExternalSoundPlayer() {
    for (BasicSoundExtPlayList::Iterator it = mSoundList.GetBeginIter();
         it != mSoundList.GetEndIter();) {
        BasicSoundExtPlayList::Iterator curr = it++;
        curr->DetachExternalSoundPlayer(this);
    }
}

void ExternalSoundPlayer::StopAllSound(int frames) {
    for (BasicSoundExtPlayList::Iterator it = mSoundList.GetBeginIter();
         it != mSoundList.GetEndIter();) {
        BasicSoundExtPlayList::Iterator curr = it++;
        curr->Stop(frames);
    }
}

void ExternalSoundPlayer::PauseAllSound(bool flag, int frames) {
    for (BasicSoundExtPlayList::Iterator it = mSoundList.GetBeginIter();
         it != mSoundList.GetEndIter();) {
        BasicSoundExtPlayList::Iterator curr = it++;
        curr->Pause(flag, frames);
    }
}

void ExternalSoundPlayer::DetachSoundActorAll(SoundActor* pActor) {
    for (BasicSoundExtPlayList::Iterator it = mSoundList.GetBeginIter();
         it != mSoundList.GetEndIter();) {
        BasicSoundExtPlayList::Iterator curr = it++;
        curr->DetachSoundActor(pActor);
    }
}

bool ExternalSoundPlayer::AppendSound(BasicSound* pSound) {
    SoundThread::AutoLock lock;

    int priority = pSound->CalcCurrentPlayerPriority();

    if (GetPlayableSoundCount() == 0) {
        return false;
    }

    while (GetPlayingSoundCount() >= GetPlayableSoundCount()) {
        BasicSound* pDropSound = GetLowestPrioritySound();
        if (pDropSound == nullptr) {
            return false;
        }

        if (priority < pDropSound->CalcCurrentPlayerPriority()) {
            return false;
        }

        pDropSound->Shutdown();
    }

    mSoundList.PushBack(pSound);
    pSound->AttachExternalSoundPlayer(this);

    return true;
}

void ExternalSoundPlayer::SetPlayableSoundCount(int count) {
    mPlayableCount = count;

    while (GetPlayingSoundCount() > GetPlayableSoundCount()) {
        GetLowestPrioritySound()->Shutdown();
    }
}

void ExternalSoundPlayer::RemoveSound(BasicSound* pSound) {
    mSoundList.Erase(pSound);
    pSound->DetachExternalSoundPlayer(this);
}

bool ExternalSoundPlayer::detail_CanPlaySound(int startPriority) {
    if (GetPlayableSoundCount() == 0) {
        return false;
    }

    if (GetPlayingSoundCount() >= GetPlayableSoundCount()) {
        BasicSound* pDropSound = GetLowestPrioritySound();
        if (pDropSound == nullptr) {
            return false;
        }

        if (startPriority < pDropSound->CalcCurrentPlayerPriority()) {
            return false;
        }
    }

    return true;
}

BasicSound* ExternalSoundPlayer::GetLowestPrioritySound() {
    int lowestPriority = BasicSound::PRIORITY_MAX + 1;
    BasicSound* pLowest = nullptr;

    for (BasicSoundExtPlayList::Iterator it = mSoundList.GetBeginIter();
         it != mSoundList.GetEndIter(); ++it) {
        int priority = it->CalcCurrentPlayerPriority();

        if (lowestPriority > priority) {
            pLowest = &*it;
            lowestPriority = priority;
        }
    }

    return pLowest;
}

}
}
}
