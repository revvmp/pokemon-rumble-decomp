#ifndef _MATH_H
#define _MATH_H

#ifdef __cplusplus
extern "C" {
#endif

double pow(double x, double y);
double ceil(double x);
double floor(double x);
double asin(double x);
double atan(double x);
double atan2(double y, double x);
double cos(double x);
double exp(double x);
double fmod(double x, double y);
double log(double x);
double log10(double x);
double ldexp(double x, int exponent);
double frexp(double x, int* exponent);
double modf(double x, double* integral);
double scalbn(double x, int exponent);
float roundf(float x);
double sin(double x);
double tan(double x);
double sqrt(double x);
double nan(const char* tag);

static inline float sqrtf(float x) {
    return (float)sqrt(x);
}

static inline float cosf(float x) {
    return (float)cos(x);
}

static inline float tanf(float x) {
    return (float)tan(x);
}

static inline float powf(float x, float y) {
    return (float)pow(x, y);
}

static inline float log10f(float x) {
    return (float)log10(x);
}

#ifdef __cplusplus
}
#endif

#endif
