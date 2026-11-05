#ifndef GAME_GAMEUTIL_H
#define GAME_GAMEUTIL_H

#include <game/world/MapIndex.h>
#include <revolution/types.h>

bool IsStage7OddMap(MapIndex index);
int GetTypeEfficacy(int attackType, int defendType);

void ChangeBGM(u32 id);
void ContinueGameBGM(u32 id);
void PauseGameAudio();
void ResumeGameAudio();
void ClearGameAudioEffects();
void RestoreGameAudioEffects();

#endif
