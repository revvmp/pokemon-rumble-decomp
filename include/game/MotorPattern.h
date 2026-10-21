#ifndef GAME_MOTORPATTERN_H
#define GAME_MOTORPATTERN_H

#include <game/Pad.h>

struct MotorPattern {
    int mIndex;
    const RumbleStep* mpWiiPattern;
    const RumbleStep* mpGCPattern;
};

#endif
