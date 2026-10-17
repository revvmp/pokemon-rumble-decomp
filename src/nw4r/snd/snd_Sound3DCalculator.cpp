#include <nw4r/snd/snd_Sound3DCalculator.h>

#include <nw4r/snd/snd_AxManager.h>
#include <nw4r/snd/snd_Sound3DListener.h>
#include <nw4r/snd/snd_Sound3DManager.h>

#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_types.h>

#include <math.h>

namespace nw4r {
namespace snd {

namespace {

inline f32 SolveLinerFunction(f32 x, f32 x1, f32 x0, f32 y1, f32 y0) {
    if (x1 == x0) {
        return (y1 + y0) / 2.0f;
    }

    f32 dx = x1 - x0;
    return x * (y0 - y1) / dx + (x1 * y1 - x0 * y0) / dx;
}

inline void CalcPanByOutputMode(const math::VEC3& rPos, f32 speakerAngleStereo,
                                f32 interiorSize, f32 distance, f32 panRange,
                                f32 frontSpeakerAngleDpl2,
                                f32 rearSpeakerAngleDpl2, f32 initPan,
                                f32* pPan, f32* pSurroundPan) {
    switch (detail::AxManager::GetInstance().GetOutputMode()) {
    case OUTPUT_MODE_SURROUND:
    case OUTPUT_MODE_DPL2:
        Sound3DCalculator::CalcPanDpl2(rPos, interiorSize, distance, panRange,
                                       frontSpeakerAngleDpl2,
                                       rearSpeakerAngleDpl2, initPan, pPan,
                                       pSurroundPan);
        break;

    case OUTPUT_MODE_STEREO:
        Sound3DCalculator::CalcPanStereo(rPos, interiorSize, distance,
                                         panRange, speakerAngleStereo, pPan,
                                         pSurroundPan);
        break;

    default:
        *pPan = 0.0f;
        *pSurroundPan = 0.0f;
        break;
    }
}

}

void Sound3DCalculator::CalcVolumeAndPriority(const Sound3DManager& rManager,
                                              const Sound3DListener& rListener,
                                              const Sound3DParam& rParam,
                                              f32* pVolume, int* pPriority) {
    math::VEC3 relativePos;
    math::VEC3Sub(&relativePos, &rParam.position, &rListener.GetPosition());

    f32 distance = math::VEC3Len(&relativePos);
    f32 decayRatio = rParam.decayRatio / 256.0f;
    f32 maxVolumeDistance = rListener.GetMaxVolumeDistance();
    f32 unitDistance = rListener.GetUnitDistance();
    int maxPriorityReduction = rManager.GetMaxPriorityReduction();
    u8 decayCurve = rParam.decayCurve;

    f32 volume = 1.0f;

    if (distance > maxVolumeDistance) {
        switch (decayCurve) {
        case Sound3DParam::DECAY_CURVE_LOGARITHMIC: {
            f32 units = (distance - maxVolumeDistance) / unitDistance;
            volume = pow(decayRatio, units);
            break;
        }

        case Sound3DParam::DECAY_CURVE_LINEAR: {
            f32 units = (distance - maxVolumeDistance) / unitDistance *
                        (1.0f - decayRatio);
            volume = 1.0f - units;

            if (volume < 0.0f) {
                volume = 0.0f;
            }
            break;
        }
        }
    }

    *pVolume = volume;
    *pPriority = -static_cast<int>((1.0f - volume) * maxPriorityReduction);
}

void Sound3DCalculator::CalcPan(const Sound3DManager& rManager,
                                const Sound3DListener& rListener,
                                const Sound3DParam& rParam,
                                const CalcPanParam& rPanParam, f32* pPan,
                                f32* pSurroundPan) {
    math::VEC3 relativePos;
    math::VEC3Transform(&relativePos, &rListener.GetMatrix(), &rParam.position);
    f32 distance = math::VEC3Len(&relativePos);

    CalcPanByOutputMode(relativePos, rPanParam.speakerAngleStereo,
                        rListener.GetInteriorSize(), distance,
                        rManager.GetPanRange(), rPanParam.frontSpeakerAngleDpl2,
                        rPanParam.rearSpeakerAngleDpl2, rPanParam.initPan, pPan,
                        pSurroundPan);
}

void Sound3DCalculator::CalcPitch(const Sound3DManager& rManager,
                                  const Sound3DListener& rListener,
                                  const Sound3DParam& rParam, f32* pPitch) {
    f32 sonicVelocity = rManager.GetSonicVelocity();

    if (sonicVelocity == 0.0f) {
        *pPitch = 1.0f;
        return;
    }

    math::VEC3 relativePos;
    math::VEC3Sub(&relativePos, &rParam.position, &rListener.GetPosition());
    f32 distance = math::VEC3Len(&relativePos);

    if (distance > 0.0f) {
        relativePos /= distance;
    }

    f32 dopplerFactor = rParam.dopplerFactor / 32.0f;
    f32 sourceVelocity;
    f32 listenerVelocity;

    if (distance > 0.0f) {
        sourceVelocity = -math::VEC3Dot(&relativePos, &rParam.velocity);
        listenerVelocity =
            -math::VEC3Dot(&relativePos, &rListener.GetVelocity());
    } else {
        sourceVelocity = -math::VEC3Len(&rParam.velocity);
        listenerVelocity = math::VEC3Len(&rListener.GetVelocity());
    }

    sourceVelocity *= dopplerFactor;
    listenerVelocity *= dopplerFactor;

    f32 pitch;
    if (listenerVelocity > sonicVelocity) {
        pitch = 0.0f;
    } else if (sourceVelocity >= sonicVelocity) {
        pitch = 65535.0f;
    } else {
        pitch = (sonicVelocity - listenerVelocity) /
                (sonicVelocity - sourceVelocity);
    }

    *pPitch = pitch;
}

void Sound3DCalculator::CalcBiquadFilterValue(const Sound3DManager& rManager,
                                              const Sound3DListener& rListener,
                                              const Sound3DParam& rParam,
                                              f32* pValue) {
    math::VEC3 relativePos;
    math::VEC3Sub(&relativePos, &rParam.position, &rListener.GetPosition());
    f32 distance = math::VEC3Len(&relativePos);

    f32 value = 0.0f;
    f32 maxVolumeDistance = rListener.GetMaxVolumeDistance();
    f32 maxValue = rListener.GetUnitBiquadFilterMax();

    if (distance > maxVolumeDistance) {
        value = (distance - maxVolumeDistance) / rListener.GetUnitDistance() *
                rListener.GetUnitBiquadFilterValue();

        if (value > maxValue) {
            value = maxValue;
        }
    }

    *pValue = value;
}

void Sound3DCalculator::CalcPanDpl2(const math::VEC3& rPos, f32 interiorSize,
                                    f32 distance, f32 panRange,
                                    f32 frontSpeakerAngle,
                                    f32 rearSpeakerAngle, f32 initPan,
                                    f32* pPan, f32* pSurroundPan) {
    f32 angle;
    f32 distanceRatio;
    CalcAngleAndDistance(rPos, distance, interiorSize, &angle, &distanceRatio);

    f32 rearLeftAngle = -rearSpeakerAngle;
    f32 frontLeftAngle = -frontSpeakerAngle;
    f32 frontRightAngle = frontSpeakerAngle;
    f32 rearRightAngle = rearSpeakerAngle;

    f32 pan;
    f32 surroundPan;

    if (angle < rearLeftAngle) {
        pan = SolveLinerFunction(angle, -NW4R_MATH_PI, rearLeftAngle, -1.0f,
                                 0.0f);
        surroundPan = 1.0f;
    } else if (angle < -NW4R_MATH_PI / 2.0f) {
        pan = -1.0f;
        surroundPan = SolveLinerFunction(angle, rearLeftAngle,
                                         -NW4R_MATH_PI / 2.0f, 0.0f, 1.0f);
    } else if (angle < frontLeftAngle) {
        pan = -1.0f;
        surroundPan = SolveLinerFunction(angle, -NW4R_MATH_PI / 2.0f,
                                         frontLeftAngle, -1.0f, 0.0f);
    } else if (angle < frontRightAngle) {
        pan = SolveLinerFunction(angle, frontLeftAngle, frontRightAngle, 1.0f,
                                 -1.0f);
        surroundPan = -1.0f;
    } else if (angle < NW4R_MATH_PI / 2.0f) {
        pan = 1.0f;
        surroundPan = SolveLinerFunction(angle, frontRightAngle,
                                         NW4R_MATH_PI / 2.0f, 0.0f, -1.0f);
    } else if (angle < rearRightAngle) {
        pan = 1.0f;
        surroundPan = SolveLinerFunction(angle, NW4R_MATH_PI / 2.0f,
                                         rearRightAngle, 1.0f, 0.0f);
    } else {
        pan = SolveLinerFunction(angle, rearRightAngle, NW4R_MATH_PI, 0.0f,
                                 1.0f);
        surroundPan = 1.0f;
    }

    f32 centerCos = (cosf(frontSpeakerAngle) + cosf(rearSpeakerAngle)) / 2.0f;
    f32 centerSurroundPan = centerCos / (centerCos + -cosf(rearSpeakerAngle));

    pan *= panRange;
    surroundPan *= panRange;

    *pPan = pan * distanceRatio;
    *pSurroundPan =
        initPan + (1.0f + (surroundPan * distanceRatio +
                           centerSurroundPan * (1.0f - distanceRatio)));
}

void Sound3DCalculator::CalcPanStereo(const math::VEC3& rPos,
                                      f32 interiorSize, f32 distance,
                                      f32 panRange, f32 speakerAngle,
                                      f32* pPan, f32* pSurroundPan) {
    f32 angle;
    f32 distanceRatio;
    CalcAngleAndDistance(rPos, distance, interiorSize, &angle, &distanceRatio);

    f32 rearLeftAngle = -NW4R_MATH_PI + speakerAngle;
    f32 frontLeftAngle = -speakerAngle;
    f32 frontRightAngle = speakerAngle;
    f32 rearRightAngle = NW4R_MATH_PI - speakerAngle;

    f32 pan;

    if (angle < rearLeftAngle) {
        pan = SolveLinerFunction(angle, -NW4R_MATH_PI, rearLeftAngle, -1.0f,
                                 0.0f);
    } else if (angle < frontLeftAngle) {
        pan = -1.0f;
    } else if (angle < frontRightAngle) {
        pan = SolveLinerFunction(angle, frontLeftAngle, frontRightAngle, 1.0f,
                                 -1.0f);
    } else if (angle < rearRightAngle) {
        pan = 1.0f;
    } else {
        pan = SolveLinerFunction(angle, rearRightAngle, NW4R_MATH_PI, 0.0f,
                                 1.0f);
    }

    pan *= panRange;

    *pPan = pan * distanceRatio;
    *pSurroundPan = 0.0f;
}

void Sound3DCalculator::CalcAngleAndDistance(const math::VEC3& rPos,
                                             f32 distance, f32 interiorSize,
                                             f32* pAngle, f32* pDistance) {
    math::VEC3 pos;

    if (0.0f == distance) {
        pos.z = 0.0f;
        pos.y = 0.0f;
        pos.x = 0.0f;
    } else {
        math::VEC3 horizontal(rPos.x, 0.0f, rPos.z);

        f32 horizontalDistance = math::VEC3Len(&horizontal);
        if (horizontalDistance > interiorSize) {
            horizontal.x *= interiorSize / horizontalDistance;
            horizontal.z *= interiorSize / horizontalDistance;
        }

        f32 scale = math::VEC3Len(&horizontal);
        pos.x = rPos.x * scale / distance;
        pos.y = 0.0f;
        pos.z = rPos.z * scale / distance;
    }

    *pAngle = atan2(pos.x, -pos.z);
    *pDistance = math::VEC3Len(&pos) / interiorSize;
}

}
}
