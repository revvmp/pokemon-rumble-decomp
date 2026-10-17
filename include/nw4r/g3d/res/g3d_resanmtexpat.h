#ifndef NW4R_G3D_RES_RES_ANM_TEX_PAT_H
#define NW4R_G3D_RES_RES_ANM_TEX_PAT_H

#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resdict.h>
#include <nw4r/g3d/res/g3d_resmat.h>
#include <nw4r/g3d/res/g3d_respltt.h>
#include <nw4r/g3d/res/g3d_restex.h>

namespace nw4r {
namespace g3d {

struct TexPatAnmResult {
    static const int NUM_OF_ANMS = 8;

    u8 bTexExist;
    u8 bPlttExist;
    u8 PADDING_0x2[0x4 - 0x2];
    ResTex tex[NUM_OF_ANMS];
    ResPltt pltt[NUM_OF_ANMS];
};

struct ResAnmTexPatFrmData {
    f32 frame;
    u16 texIndex;
    u16 plttIndex;
};

struct ResAnmTexPatAnmData {
    u16 numFrameValues;
    u8 PADDING_0x2[0x4 - 0x2];
    f32 invKeyFrameRange;
    ResAnmTexPatFrmData frameValues[1];
};

struct ResAnmTexPatMatData {
    enum Flag {
        FLAG_ANM_EXISTS = (1 << 0),
        FLAG_ANM_CONST = (1 << 1),
        FLAG_ANM_TEX = (1 << 2),
        FLAG_ANM_PLTT = (1 << 3),

        NUM_OF_FLAGS = 4
    };

    union AnmData {
        s32 toResAnmTexPatAnmData;

        struct {
            u16 texIndex;
            u16 plttIndex;
        } constValue;
    };

    s32 name;
    u32 flags;
    AnmData anms[1];
};

struct ResAnmTexPatInfoData {
    u16 numFrame;
    u16 numMaterial;
    u16 numTexture;
    u16 numPalette;
    AnmPolicy policy;
};

struct ResAnmTexPatData {
    ResBlockHeaderData header;
    u32 revision;
    s32 toResFileData;
    s32 toTexPatDataDic;
    s32 toTexNameArray;
    s32 toPlttNameArray;
    s32 toResTexArray;
    s32 toResPlttArray;
    s32 toResUserData;
    s32 name;
    s32 original_path;
    ResAnmTexPatInfoData info;
};

class ResAnmTexPat : public ResCommon<ResAnmTexPatData> {
public:
    static const u32 SIGNATURE = 'PAT0';
    static const int REVISION = 4;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmTexPat);

    bool Bind(const ResFile file);
    void Release();

    ResFile GetParent() const;

    u32 GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const {
        return GetRevision() == REVISION;
    }

    void GetAnmResult(TexPatAnmResult *pResult, u32 idx, f32 frame) const;

    const ResAnmTexPatMatData *GetMatAnm(int idx) const {
        return static_cast<ResAnmTexPatMatData *>(ofs_to_obj<ResDic>(ref().toTexPatDataDic)[idx]);
    }
    const ResAnmTexPatMatData *GetMatAnm(u32 idx) const {
        return static_cast<ResAnmTexPatMatData *>(ofs_to_obj<ResDic>(ref().toTexPatDataDic)[idx]);
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
