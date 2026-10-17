#include <nw4r/snd/snd_Sound3DListener.h>

namespace nw4r {
namespace snd {

Sound3DListener::Sound3DListener()
    : mPosition(0.0f, 0.0f, 0.0f),
      mVelocity(0.0f, 0.0f, 0.0f),
      mInteriorSize(1.0f),
      mMaxVolumeDistance(1.0f),
      mUnitDistance(1.0f),
      mUserParam(0),
      mResetMatrixFlag(true),
      mUnitBiquadFilterValue(0.5f),
      mUnitBiquadFilterMax(1.0f) {
    math::MTX34Zero(&mMtx);
}

void Sound3DListener::SetMatrix(const math::MTX34& rMtx) {
    math::VEC3 prevPosition = mPosition;

    mPosition.x =
        -(rMtx._00 * rMtx._03 + rMtx._10 * rMtx._13 + rMtx._20 * rMtx._23);
    mPosition.y =
        -(rMtx._01 * rMtx._03 + rMtx._11 * rMtx._13 + rMtx._21 * rMtx._23);
    mPosition.z =
        -(rMtx._02 * rMtx._03 + rMtx._12 * rMtx._13 + rMtx._22 * rMtx._23);

    if (mResetMatrixFlag) {
        mMtx = rMtx;
        mResetMatrixFlag = false;
    } else {
        mMtx = rMtx;
        math::VEC3Sub(&mVelocity, &mPosition, &prevPosition);
    }
}

void Sound3DListener::SetInteriorSize(f32 size) {
    mInteriorSize = size;
}

void Sound3DListener::SetMaxVolumeDistance(f32 distance) {
    mMaxVolumeDistance = distance;
}

void Sound3DListener::SetUnitDistance(f32 distance) {
    mUnitDistance = distance;
}

}
}
