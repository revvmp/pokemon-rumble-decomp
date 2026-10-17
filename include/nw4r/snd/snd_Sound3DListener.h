#ifndef NW4R_SND_SOUND_3D_LISTENER_H
#define NW4R_SND_SOUND_3D_LISTENER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_LinkList.h>

namespace nw4r {
namespace snd {

class Sound3DListener {
public:
    Sound3DListener();

    const math::MTX34& GetMatrix() const {
        return mMtx;
    }
    void SetMatrix(const math::MTX34& rMtx);

    const math::VEC3& GetPosition() const {
        return mPosition;
    }
    const math::VEC3& GetVelocity() const {
        return mVelocity;
    }

    f32 GetInteriorSize() const {
        return mInteriorSize;
    }
    void SetInteriorSize(f32 size);

    f32 GetMaxVolumeDistance() const {
        return mMaxVolumeDistance;
    }
    void SetMaxVolumeDistance(f32 distance);

    f32 GetUnitDistance() const {
        return mUnitDistance;
    }
    void SetUnitDistance(f32 distance);

    u32 GetUserParam() const {
        return mUserParam;
    }
    void SetUserParam(u32 param) {
        mUserParam = param;
    }

    f32 GetUnitBiquadFilterValue() const {
        return mUnitBiquadFilterValue;
    }
    f32 GetUnitBiquadFilterMax() const {
        return mUnitBiquadFilterMax;
    }

private:
    math::MTX34 mMtx;
    math::VEC3 mPosition;
    math::VEC3 mVelocity;
    f32 mInteriorSize;
    f32 mMaxVolumeDistance;
    f32 mUnitDistance;
    u32 mUserParam;
    bool mResetMatrixFlag;
    f32 mUnitBiquadFilterValue;
    f32 mUnitBiquadFilterMax;

public:
    NW4R_UT_LINKLIST_NODE_DECL();
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(Sound3DListener);

}
}

#endif
