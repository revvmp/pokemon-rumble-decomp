#include "MSL_C/math.h"

int __fpclassifyf(float value) {
    switch ((*(int*)&value) & 0x7F800000) {
        case 0x7F800000:
            if ((*(int*)&value) & 0x007FFFFF) {
                return FP_NAN;
            }

            return FP_INFINITE;
        case 0:
            return ((*(int*)&value) & 0x007FFFFF) != 0 ? FP_SUBNORMAL : FP_ZERO;
    }

    return FP_NORMAL;
}

int __signbitd(double value) {
    return __HI(value) & 0x80000000;
}

int __fpclassifyd(double value) {
    switch (__HI(value) & 0x7FF00000) {
        case 0x7FF00000:
            if ((__HI(value) & 0x000FFFFF) || __LO(value)) {
                return FP_NAN;
            }

            return FP_INFINITE;
        case 0:
            if ((__HI(value) & 0x000FFFFF) || __LO(value)) {
                return FP_SUBNORMAL;
            }

            return FP_ZERO;
    }

    return FP_NORMAL;
}
