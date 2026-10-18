#ifndef NW4R_EF_POST_FIELD_H
#define NW4R_EF_POST_FIELD_H

#include <nw4r/math/math_types.h>
#include <nw4r/ef/ef_emitter.h>

namespace nw4r {
namespace ef {

struct PostFieldInfo {
    math::VEC3 mScale;
    math::VEC3 mRotate;
    math::VEC3 mTranslate;
    f32 mSpeedLimit;
    u8 mSpeedMode;
    u8 mShape;
    u8 mOption;
    u8 mCollisionMode;
    u16 mFlags;
    u16 mStartAge;
    math::VEC3 mCollisionScale;
    EmitterInheritSetting mInheritSetting;
};

int GetTypePositionSphere(const math::VEC3& rPosition, u8 option);
int GetTypePositionCube(const math::VEC3& rPosition, u8 option);
int GetTypePositionCylinder(const math::VEC3& rPosition, u8 option);
int GetTypePositionCircle(const math::VEC3& rPosition, u8 option);
int GetTypePositionRectangle(const math::VEC3& rPosition, u8 option);
int GetTypePositionPlane(const math::VEC3& rPosition, u8 option);

f32 crossCheckPlane(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit);
f32 crossCheckRectangle(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit);
f32 crossCheckCircle(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit);

f32 crossCheckSphere(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit);
f32 crossCheckCylinder(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit);
f32 crossCheckCube(const math::MTX34& rMtx, const PostFieldInfo* pInfo,
                    const math::VEC3& rCurrent, const math::VEC3& rPrevious,
                    math::VEC3* pDirection, math::VEC3* pVelocity,
                    math::VEC3* pHit, math::VEC3* pWorldHit);

int crossCheck(const math::MTX34& rMtx, const math::MTX34& rInverseMtx,
               const PostFieldInfo* pInfo, EmitterResource* pResource,
               Particle* pParticle, const math::VEC3& rCurrent,
               const math::VEC3& rPrevious, math::VEC3 positionOffset,
               bool* pKilled, math::VEC3* pDirection, math::VEC3* pVelocity,
               math::VEC3* pHit, math::VEC3* pWorldHit);

}
}

#endif
