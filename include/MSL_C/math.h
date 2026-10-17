#ifndef MSL_C_MATH_H
#define MSL_C_MATH_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

int __fpclassifyd(double value);
double copysign(double value, double sign);
double __ieee754_acos(double value);
double __ieee754_asin(double value);
double __ieee754_atan2(double y, double x);
double __ieee754_fmod(double value, double divisor);
double __ieee754_log(double value);
double __ieee754_log10(double value);
double __ieee754_pow(double value, double exponent);
double __ieee754_sqrt(double value);
double __kernel_sin(double value, double tail, int hasTail);
double __kernel_cos(double value, double tail);
double __kernel_tan(double value, double tail, int reciprocal);
int __ieee754_rem_pio2(double value, double* remainder);

extern int __float_nan[];

#define NAN (*(float*)__float_nan)
#define __HI(x) *(int*)&x
#define __LO(x) *(1 + (int*)&x)
#define __HIp(x) *(int*)x
#define __LOp(x) *(1 + (int*)x)

#ifdef __cplusplus
}
#endif

#endif
