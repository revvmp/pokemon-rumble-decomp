#ifndef GAME_SOUND_H
#define GAME_SOUND_H

#include <revolution/types.h>

namespace nw4r {
namespace snd {
class FxChorus;
class FxDelay;
class FxReverbStd;
class Sound3DListener;
class Sound3DManager;
class SoundActor;
class SoundArchivePlayer;
class SoundHandle;
class SoundStartable;
}
}

void InitSound(u32 reverbSize, u32 chorusSize, u32 delaySize);
void ShutdownSound();
void UpdateSound();
void LoadSoundArchive(const char* pPath);

void SetBGMVolume(f32 volume, f32 fadeTime);
void StopBGM(f32 fadeTime);
u32 GetBGMID();
void ContinueBGM(u32 id, f32 fadeTime);
void PlayBGM(u32 id, f32 fadeTime, bool resume);

void PlaySound(u32 id, nw4r::snd::SoundHandle* pHandle = nullptr, nw4r::snd::SoundStartable* pStartable = nullptr);
void StopSound(nw4r::snd::SoundHandle* pHandle, nw4r::snd::SoundActor* pActor, f32 fadeTime);

nw4r::snd::SoundArchivePlayer& GetSoundArchivePlayer();
nw4r::snd::Sound3DManager& GetSound3DManager();
nw4r::snd::Sound3DListener& GetSound3DListener();
nw4r::snd::FxReverbStd& GetFxReverb();
nw4r::snd::FxDelay& GetFxDelay();
nw4r::snd::FxChorus& GetFxChorus();

#endif
