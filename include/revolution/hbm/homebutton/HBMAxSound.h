#ifndef HBM_AX_SOUND_H
#define HBM_AX_SOUND_H

#include "revolution/types.h"

extern const char* SOUND_FILENAME[];

namespace homebutton {

void PlaySeq(int num);
void InitAxSound(const void* pSoundData, void* pMem, u32 memSize);
void ShutdownAxSound();
void AxSoundMain();
void StopAllSeq();
void SetVolumeAllSeq(f32 gain);
void SetSoundMode(u32 mode);

}

#endif
