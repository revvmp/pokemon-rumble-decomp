#include <math.h>
#include "MSL_C/math.h"

double scalbn(double x, int exponent) {
    int binaryExponent;
    double fraction = frexp(x, &binaryExponent);

    binaryExponent += exponent;
    return ldexp(fraction, binaryExponent);
}

float roundf(float x) {
    int negative = 0;
    float fraction;
    float result;

    if (!isfinite(x)) {
        return x;
    }

    if (x < 0.0) {
        negative = 1;
        x = -x;
    }

    fraction = x - (float)(int)x;
    result = x - fraction;

    if (fraction >= 0.5) {
        result += 1.0;
    }

    if (negative) {
        result = -result;
    }

    return result;
}
