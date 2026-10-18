#ifndef NW4R_G3D_RES_RES_USER_H
#define NW4R_G3D_RES_RES_USER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resdict.h>

namespace nw4r {
namespace g3d {

struct ResUserDataEntryData {
    u32 size;
    s32 toData;
    u32 numData;
    u32 type;
};

class ResUserDataEntry : public ResCommon<ResUserDataEntryData> {
public:
    enum Type {
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_STRING
    };

    NW4R_G3D_RESOURCE_FUNC_DEF(ResUserDataEntry);

    u32 GetType() const {
        return ref().type;
    }

    s32 GetInt() const {
        return *ofs_to_ptr<s32>(ref().toData);
    }
};

struct ResUserDataData {
    u32 size;
    ResDicData dict;
};

class ResUserData : public ResCommon<ResUserDataData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResUserData);

    ResUserDataEntry GetEntry(const char *pName) const {
        return ResUserDataEntry(ResDic(const_cast<ResDicData *>(&ref().dict))[pName]);
    }
};

}
}

#endif
