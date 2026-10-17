#ifndef _MATH_H
#define _MATH_H

#ifdef __cplusplus
extern "C" {
#endif

double pow(double x, double y);

static inline float powf(float x, float y) {
    return (float)pow(x, y);
}

#ifdef __cplusplus
}
#endif

#endif
