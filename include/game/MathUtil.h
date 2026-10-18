#ifndef GAME_MATHUTIL_H
#define GAME_MATHUTIL_H

#include <revolution/types.h>

f32 NormalizeSignedAngle(f32 degrees);

template <typename T> inline T Max(T a, T b) {
    return (a < b) ? b : a;
}

template <typename T> inline T Min(T a, T b) {
    return (b < a) ? b : a;
}

#endif
