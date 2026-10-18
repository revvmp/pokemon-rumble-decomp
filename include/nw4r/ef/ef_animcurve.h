#ifndef NW4R_EF_ANIM_CURVE_H
#define NW4R_EF_ANIM_CURVE_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace ef {

class Particle;

f32 AnimCurveCalcTime(u8* pCmdList, u32 tick, u32 life);
u16 AnimCurveCalcFrameStep(u8* pCmdList, u32 tick, u32 life);

void AnimCurveExecuteU8(u8* pCmdList, u8* pTarget, u32 tick, u16 seed,
                        u32 life);

void AnimCurveExecuteF32(u8* pCmdList, f32* pTarget, u32 tick, u16 seed,
                         u32 life);

void AnimCurveExecuteRotate(u8* pCmdList, f32* pTarget, u32 tick, u16 seed,
                            u32 life);

void AnimCurveExecuteTexture(u8* pCmdList, Particle* pParticle, u32 tick,
                             u16 seed, u32 life);

void AnimCurveExecuteChild(u8* pCmdList, Particle* pParticle, u32 tick,
                           u16 seed, u32 life);

void AnimCurveExecuteBakedU8(u8* pCmdList, u8* pTarget, u32 tick, u32 life);
void AnimCurveExecuteBakedF32(u8* pCmdList, f32* pTarget, u32 tick, u32 life);

}
}

#endif
