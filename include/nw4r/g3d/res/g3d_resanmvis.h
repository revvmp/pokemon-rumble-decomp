#ifndef NW4R_G3D_RES_RES_ANM_VIS_H
#define NW4R_G3D_RES_RES_ANM_VIS_H

#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resdict.h>

namespace nw4r {
namespace g3d {

struct ResAnmVisAnmData {
    enum Flag {
        FLAG_ENABLE = (1 << 0),
        FLAG_CONST = (1 << 1)
    };

    s32 name;
    u32 flags;
    ResBoolAnmFramesData visibility;
};

struct ResAnmVisInfoData {
    u16 numFrame;
    u16 numNode;
    AnmPolicy policy;
};

struct ResAnmVisData {
    ResBlockHeaderData header;
    u32 revision;
    s32 toResFileData;
    s32 toVisDataDic;
    s32 toResUserData;
    s32 name;
    s32 original_path;
    ResAnmVisInfoData info;
};

#if defined(CMODEL_EMISSION_LAYOUT)
#pragma section code_type ".text.model_vis_assignment"
#endif
class ResAnmVis : public ResCommon<ResAnmVisData> {
public:
    static const u32 SIGNATURE = 'VIS0';
    static const int REVISION = 4;

public:
#if defined(CMODEL_EMISSION_LAYOUT)
    __declspec(section ".text.model_update")
#endif
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmVis);

    u32 GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const {
        return GetRevision() == REVISION;
    }

    bool GetAnmResult(u32 idx, f32 frame) const;

    const ResAnmVisAnmData *GetNodeAnm(int idx) const {
        return static_cast<ResAnmVisAnmData *>(ofs_to_obj<ResDic>(ref().toVisDataDic)[idx]);
    }
    const ResAnmVisAnmData *GetNodeAnm(u32 idx) const {
        return static_cast<ResAnmVisAnmData *>(ofs_to_obj<ResDic>(ref().toVisDataDic)[idx]);
    }

    int GetNumFrame() const {
        return ref().info.numFrame;
    }

    int GetNumNode() const {
        return ref().info.numNode;
    }

    AnmPolicy GetAnmPolicy() const {
        return ref().info.policy;
    }
};

}
}

#if defined(CMODEL_EMISSION_LAYOUT)
#pragma section code_type ".text.model_prefix"
#endif

#endif
