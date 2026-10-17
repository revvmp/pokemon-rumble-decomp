#ifndef NW4R_G3D_RES_RES_ANM_AMB_LIGHT_H
#define NW4R_G3D_RES_RES_ANM_AMB_LIGHT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>

namespace nw4r {
namespace g3d {

struct AmbLightAnmResult {
    enum Flag {
        FLAG_COLOR_ENABLE = (1 << 0),
        FLAG_ALPHA_ENABLE = (1 << 1),
    };

    u32 flags;
    u32 color;
};

struct ResAnmAmbLightData {
    enum Flag {
        FLAG_COLOR_ENABLE = (1 << 0),
        FLAG_ALPHA_ENABLE = (1 << 1),

        FLAG_CONST = (1 << 31)
    };

    u32 size;
    s32 toResAnmScnData;
    s32 name;
    u32 id;
    u32 refNumber;
    u32 flags;
    ResColorAnmData color;
};

class ResAnmAmbLight : public ResCommon<ResAnmAmbLightData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmAmbLight);

    void GetAnmResult(AmbLightAnmResult *pResult, f32 frame) const;

    u32 GetID() const {
        return ref().id;
    }

    u32 GetRefNumber() const {
        return ref().refNumber;
    }

    const char *GetName() const {
        const ResAnmAmbLightData &r = ref();

        if (r.name != 0) {
            return NW4R_G3D_OFS_TO_RESNAME(&r, r.name).GetName();
        }

        return NULL;
    }
};

}
}

#endif
