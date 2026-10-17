#ifndef GAME_MATHUTIL_H
#define GAME_MATHUTIL_H

template <typename T> inline T Max(T a, T b) {
    return (a < b) ? b : a;
}

template <typename T> inline T Min(T a, T b) {
    return (b < a) ? b : a;
}

#endif
