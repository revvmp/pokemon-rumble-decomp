#ifndef NW4R_SND_SOUND_ARCHIVE_PLAYER_H
#define NW4R_SND_SOUND_ARCHIVE_PLAYER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_BasicSound.h>
#include <nw4r/snd/snd_DisposeCallback.h>
#include <nw4r/snd/snd_SoundArchive.h>
#include <nw4r/snd/snd_SoundStartable.h>

namespace nw4r {
namespace snd {

class SoundActor;
class SoundHandle;
class SoundPlayer;

class SoundArchivePlayer_FileManager {
public:
    virtual const void* GetFileAddress(u32 fileId) = 0;
    virtual const void* GetFileWaveDataAddress(u32 fileId) = 0;
};

class SoundArchivePlayer : public detail::DisposeCallback,
                           public SoundStartable {
public:
    SoundArchivePlayer();
    virtual ~SoundArchivePlayer();

    virtual void InvalidateData(const void* pStart, const void* pEnd);
    virtual void InvalidateWaveData(const void* pStart, const void* pEnd);

    virtual u32 detail_ConvertLabelStringToSoundId(const char* pLabel) {
        return mSoundArchive->ConvertLabelStringToSoundId(pLabel);
    }
    virtual StartResult detail_SetupSound(SoundHandle* pHandle, u32 soundId,
                                          bool hold,
                                          const StartInfo* pStartInfo);

    const SoundArchive& GetSoundArchive() const;

    StartResult detail_SetupSoundImpl(
        SoundHandle* pHandle, u32 soundId,
        detail::BasicSound::AmbientInfo* pAmbientInfo, SoundActor* pActor,
        bool hold, const StartInfo* pStartInfo);

private:
    const SoundArchive* mSoundArchive;
};

}
}

#endif
