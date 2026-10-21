#ifndef GAME_VIDEOTIMING_H
#define GAME_VIDEOTIMING_H

#include <revolution/types.h>

u32 GetVideoRetraceCount();
void WaitForVideoRetrace();
int GetVideoRetraceTicks();
int GetNominalVideoRetraceTicks();

extern bool gFrameTimingReset;

#endif
