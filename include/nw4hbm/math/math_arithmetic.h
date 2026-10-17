#ifndef NW4HBM_MATH_ARITHMETIC_H
#define NW4HBM_MATH_ARITHMETIC_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/math/math_constant.h"

#include "revolution/os/OSFastCast.h"

namespace nw4hbm {
namespace math {

namespace detail {

f32 FExp(f32 x);
f32 FLog(f32 x);

}

f32 FrSqrt(f32 x);

inline f32 FAbs(register f32 x) {
    register f32 ax;

    asm {
        fabs ax, x
    }

    return ax;
}

inline f32 FExp(f32 x) {
    return detail::FExp(x);
}

inline f32 FInv(register f32 x) {
    register f32 work0, work1, work2, work3;

    asm {
        fmr  work1, x
        fres work0, work1

        ps_add   work2, work0, work0
        ps_mul   work3, work0, work0
        ps_nmsub work0, work1, work3, work2
    }

    return work0;
}

inline f32 FSqrt(f32 x) {
    return x <= 0.0f ? 0.0f : x * FrSqrt(x);
}

inline f32 FLog(f32 x) {
    if (x > 0.0f) {
        return detail::FLog(x);
    }

    return NW4R_MATH_QNAN;
}

inline f32 FSelect(register f32 value, register f32 ge_zero,
                   register f32 lt_zero) {
    register f32 ret;

    asm {
        fsel ret, value, ge_zero, lt_zero
    }

    return ret;
}

inline f32 U16ToF32(u16 arg) {
    f32 ret;
    OSu16tof32(&arg, &ret);
    return ret;
}
inline u16 F32ToU16(f32 arg) {
    u16 ret;
    OSf32tou16(&arg, &ret);
    return ret;
}

inline f32 S16ToF32(s16 arg) {
    f32 ret;
    OSs16tof32(&arg, &ret);
    return ret;
}
inline s16 F32ToS16(f32 arg) {
    s16 ret;
    OSf32tos16(&arg, &ret);
    return ret;
}

inline u32 F32AsU32(f32 arg) {
    return *reinterpret_cast<u32*>(&arg);
}
inline f32 U32AsF32(u32 arg) {
    return *reinterpret_cast<f32*>(&arg);
}

inline s32 FGetExpPart(f32 x) {
    s32 s = F32AsU32(x);
    return ((s >> 23) & 0xFF) - 127;
}
inline f32 FGetMantPart(f32 x) {
    u32 u = F32AsU32(x);
    return U32AsF32((u & 0x807FFFFF) | 0x3F800000);
}

}
}

#endif
