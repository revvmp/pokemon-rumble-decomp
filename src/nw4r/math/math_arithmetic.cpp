#include <nw4r/math/math_arithmetic.h>

namespace nw4r {
namespace math {

f32 FrSqrt(register f32 x) {
    register f32 rsqrt;
    register f32 c_half = 0.5f, c_three = 3.0f;
    register f32 work0, work1;

    asm (
        frsqrte rsqrt, x

        fmuls work0, rsqrt, rsqrt
        fmuls work1, rsqrt, c_half
        fnmsubs work0, work0, x, c_three
        fmuls work1, work0, work1
    )

    return work1;
}

u32 CntBit1(u32 x) {
    u32 n = x - ((x >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    n = (n + (n >> 4)) & 0x0F0F0F0F;
    n += n >> 8;
    n += n >> 16;
    return n & 0x3F;
}

}
}
