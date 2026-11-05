#ifndef GAME_RANDOM_H
#define GAME_RANDOM_H

#include <revolution/types.h>

void InitRandom();
void ShutdownRandom();
u32 Rand32();
int RandInt(int min, int max);
f32 RandReal(f32 min, f32 max);
bool RandBool(f32 probability);

#endif
