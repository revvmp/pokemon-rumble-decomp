#ifndef NW4R_G3D_RES_RES_ANM_FOG_H
#define NW4R_G3D_RES_RES_ANM_FOG_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>

#include <nw4r/ut/ut_Color.h>

#include <revolution/GX.h>

namespace nw4r {
namespace g3d {

struct FogAnmResult {
    GXFogType type;
    f32 startz;
    f32 endz;
    ut::Color color;
};

struct ResAnmFogData {
    enum Flag {
        FLAG_START_CONST = (1 << 29),
        FLAG_END_CONST = (1 << 30),
        FLAG_COLOR_CONST = (1 << 31)
    };

    u32 size;
    s32 toResAnmScnData;
    s32 name;
    u32 id;
    u32 refNumber;
    u32 flags;
    GXFogType type;
    ResAnmData startz;
    ResAnmData endz;
    ResColorAnmData color;
};

class ResAnmFog : public ResCommon<ResAnmFogData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmFog);

    void GetAnmResult(FogAnmResult *pResult, f32 frame) const;

    u32 GetID() const {
        return ref().id;
    }
};

}
}

#endif
