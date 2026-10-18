#ifndef GAME_SOUND_H
#define GAME_SOUND_H

#include <revolution/types.h>

namespace nw4r {
namespace snd {
class SoundHandle;
class SoundStartable;
}
}

void PlaySound(u32 id, nw4r::snd::SoundHandle* pHandle = nullptr, nw4r::snd::SoundStartable* pStartable = nullptr);

#endif
