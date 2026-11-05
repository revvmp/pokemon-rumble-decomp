#include <game/core/QuarterTurn.h>
#include <nw4r/math/math_triangular.h>

f32 SinQuarterTurn(f32 value) {
    return nw4r::math::SinDeg(90.0f * value);
}

f32 OneMinusCosQuarterTurn(f32 value) {
    return 1.0f - nw4r::math::CosDeg(90.0f * value);
}
