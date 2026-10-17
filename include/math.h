#ifndef _MATH_H
#define _MATH_H

#ifdef __cplusplus
extern "C" {
#endif

double pow(double x, double y);
double acos(double x);
double ceil(double x);
double floor(double x);
double asin(double x);
double atan2(double y, double x);
double fmod(double x, double y);
double log(double x);
double log10(double x);
double ldexp(double x, int exponent);
double modf(double x, double* integral);
double sin(double x);
double tan(double x);
double sqrt(double x);
double nan(const char* tag);

static inline float powf(float x, float y) {
    return (float)pow(x, y);
}

#ifdef __cplusplus
}
#endif

#endif
