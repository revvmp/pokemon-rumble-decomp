#ifndef NW4R_G3D_FOG_H
#define NW4R_G3D_FOG_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>

#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_Color.h>

#include <revolution/GX.h>

namespace nw4r {
namespace g3d {

struct FogData {
    GXFogType type;
    f32 startz;
    f32 endz;
    f32 nearz;
    f32 farz;
    GXColor color;
    GXBool adjEnable;
    u8 PADDING_0x19;
    u16 adjCenter;
    GXFogAdjTable adjTable;
};

class Fog : public ResCommon<FogData> {
public:
    explicit Fog(FogData *pData);

    void Init();
    Fog CopyTo(void *pDst) const;

    void SetFogRangeAdjParam(u16 width, u16 center, const math::MTX44 &rProjMtx);
    void SetGP() const;

    void SetFogType(GXFogType type) {
        if (!IsValid()) {
            return;
        }

        ref().type = type;
    }

    void SetZ(f32 startZ, f32 endZ) {
        if (!IsValid()) {
            return;
        }

        FogData &r = ref();

        r.startz = startZ;
        r.endz = endZ;
    }

    void SetNearFar(f32 nearZ, f32 farZ) {
        if (!IsValid()) {
            return;
        }

        FogData &r = ref();

        r.nearz = nearZ;
        r.farz = farZ;
    }

    void SetFogColor(GXColor color) {
        if (!IsValid()) {
            return;
        }

        ref().color = color;
    }

    bool IsFogRangeAdjEnable() const {
        return IsValid() && ref().adjEnable == TRUE;
    }

    void SetFogRangeAdjEnable(bool enable) {
        if (!IsValid()) {
            return;
        }
        ref().adjEnable = enable;
    }
};

}
}

#endif
