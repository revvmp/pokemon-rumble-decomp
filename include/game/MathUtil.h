#ifndef GAME_MATHUTIL_H
#define GAME_MATHUTIL_H

#include <algorithm>
#include <revolution/types.h>
#include <MSL_C/math.h>
#include <nw4r/math/math_types.h>

inline nw4r::math::VEC3* TransformPoint(nw4r::math::VEC3* pOut, const nw4r::math::MTX34* pMtx, const nw4r::math::VEC3* pPoint) {
    PSMTXMultVec(*pMtx, *pPoint, *pOut);
    return pOut;
}

f32 NormalizeSignedAngle(f32 degrees);
f32 NormalizeAngle(f32 degrees);
f32 IntegerPower(f32 value, int exponent);

inline nw4r::math::VEC3 Transform(const nw4r::math::MTX34& rMtx, const nw4r::math::VEC3& rVec) {
    nw4r::math::VEC3 result;
    TransformPoint(&result, &rMtx, &rVec);
    return result;
}

inline nw4r::math::VEC3 Lerp(f32 t, const nw4r::math::VEC3& rFrom, const nw4r::math::VEC3& rTo) {
    return (rTo - rFrom) * t + rFrom;
}

inline bool Inverse(nw4r::math::MTX34* pOut, const nw4r::math::MTX34* pIn) {
    return nw4r::math::MTX34Inv(pOut, pIn);
}

inline nw4r::math::VEC3 Cross(const nw4r::math::VEC3& rA, const nw4r::math::VEC3& rB) {
    nw4r::math::VEC3 result;
    nw4r::math::VEC3Cross(&result, &rA, &rB);
    return result;
}

inline bool SafeNormalize(nw4r::math::VEC3* pOut, const nw4r::math::VEC3* pIn) {
    if (pIn->x == 0.0f && pIn->y == 0.0f && pIn->z == 0.0f) {
        return false;
    }

    nw4r::math::VEC3 result;
    nw4r::math::VEC3Normalize(&result, pIn);
    if (isfinite(result.x) && isfinite(result.y) && isfinite(result.z)) {
        *pOut = result;
        return true;
    }

    return false;
}

template <typename T> inline T Max(T a, T b) {
    return (a < b) ? b : a;
}

template <typename T> inline T Min(T a, T b) {
    return (b < a) ? b : a;
}

template <typename T> T Clamp(T value, T low, T high) {
    return std::min(std::max(low, value), high);
}

#endif
