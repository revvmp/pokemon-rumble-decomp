#ifndef NW4R_G3D_RES_RES_ANM_SHP_H
#define NW4R_G3D_RES_RES_ANM_SHP_H

#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resdict.h>
#include <nw4r/g3d/res/g3d_resvtx.h>

namespace nw4r {
namespace g3d {

struct ShpAnmVtxSet {
    ResVtxPos resVtxPos;
    ResVtxNrm resVtxNrm;
    ResVtxClr resVtxClr;

    friend bool operator==(const ShpAnmVtxSet &rLhs, const ShpAnmVtxSet &rRhs);
};

inline bool operator==(const ShpAnmVtxSet &rLhs, const ShpAnmVtxSet &rRhs) {
    return rLhs.resVtxPos == rRhs.resVtxPos && rLhs.resVtxNrm == rRhs.resVtxNrm && rLhs.resVtxClr == rRhs.resVtxClr;
}

struct BlendVtx {
    ShpAnmVtxSet vtxSet;
    f32 weight;
};

struct ShpAnmResult {
    enum Flag {
        FLAG_ANM_EXISTS = (1 << 0),
        FLAG_ANM_VTXPOS = (1 << 1),
        FLAG_ANM_VTXNRM = (1 << 2),
        FLAG_ANM_VTXCLR = (1 << 3),
    };

    static const int MAX_KEY_SHAPE = 32;

    u32 flags;
    u32 numKeyShape;
    ShpAnmVtxSet baseShapeVtxSet;
    f32 baseShapeWeight;
    BlendVtx keyShape[MAX_KEY_SHAPE];
};

struct ResAnmShpAnmData {
    enum Flag {
        FLAG_ANM_EXISTS = (1 << 0),
        FLAG_ANM_VTXPOS = (1 << 1),
        FLAG_ANM_VTXNRM = (1 << 2),
        FLAG_ANM_VTXCLR = (1 << 3),
    };

    u32 flags;
    s32 name;
    u16 baseShapeVtxIdx;
    u16 numKeyShape;
    u32 constFlags;
    s32 toAnmIdxToVtxIdxTable;
    ResAnmData anms[1];
};

struct ResAnmShpInfoData {
    u16 numFrame;
    u16 numVtxName;
    AnmPolicy policy;
};

struct ResAnmShpData {
    ResBlockHeaderData header;
    u32 revision;
    s32 toResFileData;
    s32 toShpDataDic;
    s32 toVtxNameArray;
    s32 toResUserData;
    s32 name;
    s32 original_path;
    ResAnmShpInfoData info;
};

class ResAnmShp : public ResCommon<ResAnmShpData> {
public:
    static const u32 SIGNATURE = 'SHP0';
    static const int REVISION = 4;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmShp);

    u32 GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const {
        return GetRevision() == REVISION;
    }

    void GetAnmResult(ShpAnmResult *pResult, u32 idx, f32 frame, const ShpAnmVtxSet *pShapeArray) const;

    const ResAnmShpAnmData *GetShapeAnm(int idx) const {
        return static_cast<ResAnmShpAnmData *>(ofs_to_obj<ResDic>(ref().toShpDataDic)[idx]);
    }
    const ResAnmShpAnmData *GetShapeAnm(u32 idx) const {
        return static_cast<ResAnmShpAnmData *>(ofs_to_obj<ResDic>(ref().toShpDataDic)[idx]);
    }

    u32 GetShapeAnmNumEntries() const {
        return ofs_to_obj<ResDic>(ref().toShpDataDic).GetNumData();
    }

    const s32 *GetVtxNameArray() const {
        return ofs_to_ptr<s32>(ref().toVtxNameArray);
    }

    int GetNumFrame() const {
        return ref().info.numFrame;
    }

    int GetNumVtxNames() const {
        return ref().info.numVtxName;
    }

    AnmPolicy GetAnmPolicy() const {
        return ref().info.policy;
    }
};

}
}

#endif
