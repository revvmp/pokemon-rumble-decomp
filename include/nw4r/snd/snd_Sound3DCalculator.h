#ifndef NW4R_SND_SOUND_3D_CALCULATOR_H
#define NW4R_SND_SOUND_3D_CALCULATOR_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math/math_constant.h>
#include <nw4r/math/math_types.h>

namespace nw4r {
namespace snd {

class Sound3DListener;
class Sound3DManager;
struct Sound3DParam;

class Sound3DCalculator {
public:
    struct CalcPanParam {
        CalcPanParam()
            : speakerAngleStereo(NW4R_MATH_PI / 4.0f),
              frontSpeakerAngleDpl2(NW4R_MATH_PI / 6.0f),
              rearSpeakerAngleDpl2(2.0f * NW4R_MATH_PI / 3.0f),
              initPan(0.0f) {}

        f32 speakerAngleStereo;
        f32 frontSpeakerAngleDpl2;
        f32 rearSpeakerAngleDpl2;
        f32 initPan;
    };

    static void CalcVolumeAndPriority(const Sound3DManager& rManager,
                                      const Sound3DListener& rListener,
                                      const Sound3DParam& rParam,
                                      f32* pVolume, int* pPriority);
    static void CalcPan(const Sound3DManager& rManager,
                        const Sound3DListener& rListener,
                        const Sound3DParam& rParam,
                        const CalcPanParam& rPanParam, f32* pPan,
                        f32* pSurroundPan);
    static void CalcPitch(const Sound3DManager& rManager,
                          const Sound3DListener& rListener,
                          const Sound3DParam& rParam, f32* pPitch);
    static void CalcBiquadFilterValue(const Sound3DManager& rManager,
                                      const Sound3DListener& rListener,
                                      const Sound3DParam& rParam,
                                      f32* pValue);

    static void CalcPanDpl2(const math::VEC3& rPos, f32 interiorSize,
                            f32 distance, f32 panRange,
                            f32 frontSpeakerAngle, f32 rearSpeakerAngle,
                            f32 initPan, f32* pPan, f32* pSurroundPan);
    static void CalcPanStereo(const math::VEC3& rPos, f32 interiorSize,
                              f32 distance, f32 panRange, f32 speakerAngle,
                              f32* pPan, f32* pSurroundPan);
    static void CalcAngleAndDistance(const math::VEC3& rPos, f32 distance,
                                     f32 interiorSize, f32* pAngle,
                                     f32* pDistance);
};

}
}

#endif
