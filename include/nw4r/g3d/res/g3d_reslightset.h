#ifndef NW4R_G3D_RES_RES_LIGHT_SET_H
#define NW4R_G3D_RES_RES_LIGHT_SET_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/g3d_state.h>
#include <nw4r/g3d/res/g3d_rescommon.h>

namespace nw4r {
namespace g3d {

struct ResLightSetData {
    static const u32 INVALID_ID = 0xFFFF;

    u32 size;
    s32 toResAnmScnData;
    s32 name;
    u32 id;
    u32 refNumber;
    s32 ambLightName;
    u16 ambLightId;
    u8 numLight;
    u8 PADDING_0x1B[0x1C - 0x1B];
    s32 lightNames[G3DState::NUM_LIGHT_IN_LIGHT_SET];
    u16 lightId[G3DState::NUM_LIGHT_IN_LIGHT_SET];
};

class ResLightSet : public ResCommon<ResLightSetData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResLightSet);

    bool Bind(const ResAnmScn scene);

    bool HasAmbLight() const {
        return ref().ambLightName != 0;
    }

    ResName GetAmbLightResName() const {
        const ResLightSetData &r = ref();

        if (r.ambLightName != 0) {
            return NW4R_G3D_OFS_TO_RESNAME(&r, r.ambLightName);
        }

        return ResName(NULL);
    }

    u32 GetAmbLightID() const {
        return ref().ambLightId;
    }

    u32 GetLightID(u32 index) const {
        return ref().lightId[index];
    }

    u32 GetNumLight() const {
        return ref().numLight;
    }

    bool IsAmbLightBound() const {
        return !HasAmbLight() || GetAmbLightID() != ResLightSetData::INVALID_ID;
    }

    ResName GetLightResName(u32 idx) const {
        const ResLightSetData &r = ref();

        const s32 *pNames = r.lightNames;
        return NW4R_G3D_OFS_TO_RESNAME(pNames, pNames[idx]);
    }
};

}
}

#endif
