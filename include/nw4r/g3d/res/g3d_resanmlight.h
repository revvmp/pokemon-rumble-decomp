#ifndef NW4R_G3D_RES_RES_ANM_LIGHT_H
#define NW4R_G3D_RES_RES_ANM_LIGHT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>

#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_Color.h>

#include <revolution/GX.h>

namespace nw4r {
namespace g3d {

struct LightAnmResult;

struct ResAnmLightDataTypedef {
    enum LightType {
        LIGHTTYPE_POINT,
        LIGHTTYPE_DIRECTIONAL,
        LIGHTTYPE_SPOT,

        LIGHTTYPE_MAX
    };
};

struct LightAnmResult : ResAnmLightDataTypedef {
    enum Flag {
        FLAG_LIGHT_TYPE_MASK = (1 << 0) | (1 << 1),
        FLAG_LIGHT_ENABLE = (1 << 2),
        FLAG_SPECULAR_ENABLE = (1 << 3),
        FLAG_COLOR_ENABLE = (1 << 4),
        FLAG_ALPHA_ENABLE = (1 << 5)
    };

    u32 specIdx;
    u32 flags;
    math::VEC3 pos;
    math::VEC3 aim;
    ut::Color color;
    GXDistAttnFn distFunc;
    f32 refDistance;
    f32 refBrightness;
    GXSpotFn spotFunc;
    f32 cutoff;
    ut::Color specColor;
    f32 shininess;
};

struct ResAnmLightData : ResAnmLightDataTypedef {
    enum Flag {
        FLAG_LIGHT_TYPE_MASK = (1 << 0) | (1 << 1),
        FLAG_LIGHT_ENABLE = (1 << 2),
        FLAG_SPECULAR_ENABLE = (1 << 3),
        FLAG_COLOR_ENABLE = (1 << 4),
        FLAG_ALPHA_ENABLE = (1 << 5),

        FLAG_POS_X_CONST = (1 << 19),
        FLAG_POS_Y_CONST = (1 << 20),
        FLAG_POS_Z_CONST = (1 << 21),

        FLAG_COLOR_CONST = (1 << 22),
        FLAG_ENABLE_CONST = (1 << 23),

        FLAG_AIM_X_CONST = (1 << 24),
        FLAG_AIM_Y_CONST = (1 << 25),
        FLAG_AIM_Z_CONST = (1 << 26),

        FLAG_CUTOFF_CONST = (1 << 27),
        FLAG_REF_DISTANCE_CONST = (1 << 28),
        FLAG_REF_BRIGHTNESS_CONST = (1 << 29),
        FLAG_SPEC_COLOR_CONST = (1 << 30),
        FLAG_SHININESS_CONST = (1 << 31)
    };

    u32 size;
    s32 toResAnmScnData;
    s32 name;
    u32 id;
    u32 refNumber;
    u32 specLightObjIdx;
    s32 toResUserData;
    u32 flags;
    ResBoolAnmData enable;
    ResAnmData posX;
    ResAnmData posY;
    ResAnmData posZ;
    ResColorAnmData color;
    ResAnmData aimX;
    ResAnmData aimY;
    ResAnmData aimZ;
    GXDistAttnFn distFunc;
    ResAnmData refDistance;
    ResAnmData refBrightness;
    GXSpotFn spotFunc;
    ResAnmData cutoff;
    ResColorAnmData specColor;
    ResAnmData shininess;
};

class ResAnmLight : public ResCommon<ResAnmLightData>, public ResAnmLightDataTypedef {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmLight);

    void GetAnmResult(LightAnmResult *pResult, f32 frame) const;

    u32 GetID() const {
        return ref().id;
    }

    u32 GetRefNumber() const {
        return ref().refNumber;
    }

    u32 GetSpecularLightIdx() const {
        return ref().specLightObjIdx;
    }

    bool HasSpecularLight() const {
        return ref().flags & ResAnmLightData::FLAG_SPECULAR_ENABLE;
    }
};

}
}

#endif
