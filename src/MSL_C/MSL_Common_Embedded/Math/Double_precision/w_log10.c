#include <MSL_C/math.h>

#pragma push
#pragma force_active on
double log10(double x) {
    return __ieee754_log10(x);
}
#pragma pop
