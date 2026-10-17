#ifndef NW4R_G3D_RES_RES_ANM_CAMERA_H
#define NW4R_G3D_RES_RES_ANM_CAMERA_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>

#include <nw4r/math/math_types.h>

#include <revolution/GX.h>

namespace nw4r {
namespace g3d {

struct ResAnmCameraDataTypedef {
    enum CameraType {
        CAMERATYPE_ROTATE,
        CAMERATYPE_AIM,

        CAMERATYPE_MAX
    };
};

struct CameraAnmResult : ResAnmCameraDataTypedef {
    enum Flag {
        FLAG_CAMERA_TYPE_MASK = (1 << 0),
        FLAG_ANM_EXISTS = (1 << 1)
    };

    u32 flags;
    GXProjectionType projType;
    math::VEC3 pos;
    f32 aspect;
    f32 near;
    f32 far;

    union {
        struct {
            math::_VEC3 rot;
        } rotate;

        struct {
            math::_VEC3 aim;
            f32 twist;
        } aim;
    };

    union {
        f32 perspFovy;
        f32 orthoHeight;
    };
};

struct ResAnmCameraData : ResAnmCameraDataTypedef {
    enum Flag {
        FLAG_CAMERA_TYPE_MASK = (1 << 0),
        FLAG_ANM_EXISTS = (1 << 1),

        FLAG_POS_X_CONST = (1 << 17),
        FLAG_POS_Y_CONST = (1 << 18),
        FLAG_POS_Z_CONST = (1 << 19),

        FLAG_ASPECT_CONST = (1 << 20),
        FLAG_NEAR_CONST = (1 << 21),
        FLAG_FAR_CONST = (1 << 22),

        FLAG_PERSP_FOVY_CONST = (1 << 23),
        FLAG_ORTHO_HEIGHT_CONST = (1 << 24),

        FLAG_AIM_X_CONST = (1 << 25),
        FLAG_AIM_Y_CONST = (1 << 26),
        FLAG_AIM_Z_CONST = (1 << 27),
        FLAG_TWIST_CONST = (1 << 28),

        FLAG_ROT_X_CONST = (1 << 29),
        FLAG_ROT_Y_CONST = (1 << 30),
        FLAG_ROT_Z_CONST = (1 << 31)
    };

    u32 size;
    s32 toResAnmScnData;
    s32 name;
    u32 id;
    u32 refNumber;
    GXProjectionType projType;
    u32 flags;
    s32 toResUserData;
    ResAnmData posX;
    ResAnmData posY;
    ResAnmData posZ;
    ResAnmData aspect;
    ResAnmData near;
    ResAnmData far;
    ResAnmData rotX;
    ResAnmData rotY;
    ResAnmData rotZ;
    ResAnmData aimX;
    ResAnmData aimY;
    ResAnmData aimZ;
    ResAnmData twist;
    ResAnmData perspFovy;
    ResAnmData orthoHeight;
};

class ResAnmCamera : public ResCommon<ResAnmCameraData>, public ResAnmCameraDataTypedef {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmCamera);

    void GetAnmResult(CameraAnmResult *pResult, f32 frame) const;

    u32 GetID() const {
        return ref().id;
    }
};

}
}

#endif
