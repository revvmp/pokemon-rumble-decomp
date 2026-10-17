#include <MSL_C/math.h>

double fmod(double x, double y) {
    return __ieee754_fmod(x, y);
}
