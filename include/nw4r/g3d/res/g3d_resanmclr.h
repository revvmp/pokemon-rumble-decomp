#ifndef NW4R_G3D_RES_RES_ANM_CLR_H
#define NW4R_G3D_RES_RES_ANM_CLR_H

#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resdict.h>

namespace nw4r {
namespace g3d {

struct ClrAnmResult {
    enum ColorAnim {
        CLA_CLR0,
        CLA_CLR1,
        CLA_AMB0,
        CLA_AMB1,

        CLA_TEV0,
        CLA_TEV1,
        CLA_TEV2,

        CLA_TEVK0,
        CLA_TEVK1,
        CLA_TEVK2,
        CLA_TEVK3,

        CLA_MAX
    };

    u32 bRgbaExist;
    u32 rgba[CLA_MAX];
    u32 rgbaMask[CLA_MAX];
};

struct ResAnmClrAnmData {
    u32 mask;
    ResColorAnmData color;
};

struct ResAnmClrMatData {
    enum Flag {
        FLAG_ANM_EXISTS = (1 << 0),
        FLAG_ANM_CONSTANT = (1 << 1),

        NUM_OF_FLAGS = 2
    };

    s32 name;
    u32 flags;
    ResAnmClrAnmData anms[1];
};

struct ResAnmClrInfoData {
    u16 numFrame;
    u16 numMaterial;
    AnmPolicy policy;
};

struct ResAnmClrData {
    ResBlockHeaderData header;
    u32 revision;
    s32 toResFileData;
    s32 toClrDataDic;
    s32 toResUserData;
    s32 name;
    s32 original_path;
    ResAnmClrInfoData info;
};

class ResAnmClr : public ResCommon<ResAnmClrData> {
public:
    static const u32 SIGNATURE = 'CLR0';
    static const int REVISION = 4;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmClr);

    u32 GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const {
        return GetRevision() == REVISION;
    }

    void GetAnmResult(ClrAnmResult *pResult, u32 idx, f32 frame) const;

    const ResAnmClrMatData *GetMatAnm(int idx) const {
        return static_cast<ResAnmClrMatData *>(ofs_to_obj<ResDic>(ref().toClrDataDic)[idx]);
    }
    const ResAnmClrMatData *GetMatAnm(u32 idx) const {
        return static_cast<ResAnmClrMatData *>(ofs_to_obj<ResDic>(ref().toClrDataDic)[idx]);
    }

    int GetNumFrame() const {
        return ref().info.numFrame;
    }

    int GetNumMaterial() const {
        return ref().info.numMaterial;
    }

    AnmPolicy GetAnmPolicy() const {
        return ref().info.policy;
    }
};

}
}

#endif
