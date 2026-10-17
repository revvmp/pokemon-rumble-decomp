#ifndef NW4R_G3D_RES_RES_ANM_CHR_H
#define NW4R_G3D_RES_RES_ANM_CHR_H

#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_resanm.h>
#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resmdl.h>

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {

struct ChrAnmResult {
    enum Flag {
        FLAG_ANM_EXISTS = (1 << 0),
        FLAG_MTX_IDENT = (1 << 1),
        FLAG_ROT_TRANS_ZERO = (1 << 2),
        FLAG_SCALE_ONE = (1 << 3),
        FLAG_SCALE_UNIFORM = (1 << 4),
        FLAG_ROT_ZERO = (1 << 5),
        FLAG_TRANS_ZERO = (1 << 6),
        FLAG_PATCH_SCALE = (1 << 7),
        FLAG_PATCH_ROT = (1 << 8),
        FLAG_PATCH_TRANS = (1 << 9),

        FLAG_SSC_APPLY = (1 << 10),
        FLAG_SSC_PARENT = (1 << 11),

        FLAG_XSI_SCALING = (1 << 12),

        FLAG_ROT_RAW_FMT = (1 << 31)
    };

    u32 flags;
    math::VEC3 s;
    math::VEC3 rawR;
    math::MTX34 rt;

    void GetScale(math::VEC3 *pScale) const;
    void GetMtx(math::MTX34 *pMtx) const;
    bool GetRotateDeg(math::VEC3 *pRotate) const;
    void GetTranslate(math::VEC3 *pTranslate) const;
    void GetRotTrans(math::MTX34 *pRotTrans) const;

    void SetScale(const math::VEC3 *pScale);
    void SetMtx(const math::MTX34 *pMtx);
    void SetRotateDeg(const math::VEC3 *pRotate);
    void SetTranslate(const math::VEC3 *pTranslate);
    void SetRotTrans(const math::MTX34 *pRotTrans);
};

struct ResAnmChrFrm32Data {
    union {
        u32 fvsU32;

        struct {
            u8 frame;
            u8 vs[3];
        } fvs;
    };
};
struct ResAnmChrFVS32Data {
    f32 scale;
    f32 offset;
    ResAnmChrFrm32Data frameValues[1];
};

struct ResAnmChrFrm48Data {
    s16 frame;
    u16 value;
    s16 slope;
};
struct ResAnmChrFVS48Data {
    f32 scale;
    f32 offset;
    ResAnmChrFrm48Data frameValues[1];
};

struct ResAnmChrFrm96Data {
    f32 frame;
    f32 value;
    f32 slope;
};
struct ResAnmChrFVS96Data {
    ResAnmChrFrm96Data frameValues[1];
};

struct ResAnmChrFVSData {
    u16 numFrameValues;
    u8 PADDING_0x2[0x4 - 0x2];
    f32 invKeyFrameRange;

    union {
        ResAnmChrFVS32Data fvs32;
        ResAnmChrFVS48Data fvs48;
        ResAnmChrFVS96Data fvs96;
    };
};

struct ResAnmChrCV8Data {
    f32 scale;
    f32 offset;
    u8 values[1];
};
struct ResAnmChrCV16Data {
    f32 scale;
    f32 offset;
    u16 values[1];
};
struct ResAnmChrCV32Data {
    f32 values[1];
};

struct ResAnmChrCVData {
    union {
        ResAnmChrCV8Data cv8;
        ResAnmChrCV16Data cv16;
        ResAnmChrCV32Data cv32;
    };
};

struct ResAnmChrAnmData {
    union {
        ResAnmChrFVSData fvs;
        ResAnmChrCVData cv;
    };
};

struct ResAnmChrNodeData {
    enum Flag {
        FLAG_ANM_EXISTS = (1 << 0),
        FLAG_MTX_IDENT = (1 << 1),
        FLAG_ROT_TRANS_ZERO = (1 << 2),
        FLAG_SCALE_ONE = (1 << 3),
        FLAG_SCALE_UNIFORM = (1 << 4),
        FLAG_ROT_ZERO = (1 << 5),
        FLAG_TRANS_ZERO = (1 << 6),
        FLAG_PATCH_SCALE = (1 << 7),
        FLAG_PATCH_ROT = (1 << 8),
        FLAG_PATCH_TRANS = (1 << 9),

        FLAG_SSC_APPLY = (1 << 10),
        FLAG_SSC_PARENT = (1 << 11),

        FLAG_XSI_SCALING = (1 << 12),

        FLAG_SCALE_X_CONST = (1 << 13),
        FLAG_SCALE_Y_CONST = (1 << 14),
        FLAG_SCALE_Z_CONST = (1 << 15),

        FLAG_ROT_X_CONST = (1 << 16),
        FLAG_ROT_Y_CONST = (1 << 17),
        FLAG_ROT_Z_CONST = (1 << 18),

        FLAG_TRANS_X_CONST = (1 << 19),
        FLAG_TRANS_Y_CONST = (1 << 20),
        FLAG_TRANS_Z_CONST = (1 << 21),

        FLAG_HAS_SCALE = (1 << 22),
        FLAG_HAS_ROT = (1 << 23),
        FLAG_HAS_TRANS = (1 << 24),

        FLAG_SCALE_FVS32_FMT = (1 << 25),
        FLAG_SCALE_FVS48_FMT = (1 << 26),
        FLAG_SCALE_FVS96_FMT = FLAG_SCALE_FVS32_FMT | FLAG_SCALE_FVS48_FMT,

        FLAG_ROT_FVS32_FMT = (1 << 27),
        FLAG_ROT_FVS48_FMT = (1 << 28),
        FLAG_ROT_FVS96_FMT = FLAG_ROT_FVS32_FMT | FLAG_ROT_FVS48_FMT,

        FLAG_ROT_CV8_FMT = (1 << 29),
        FLAG_ROT_CV16_FMT = (1 << 27) | (1 << 29),
        FLAG_ROT_CV32_FMT = (1 << 28) | (1 << 29),

        FLAG_TRANS_FVS32_FMT = (1 << 30),
        FLAG_TRANS_FVS48_FMT = (1 << 31),
        FLAG_TRANS_FVS96_FMT = FLAG_TRANS_FVS32_FMT | FLAG_TRANS_FVS48_FMT,

        FLAG_HAS_SRT_MASK = FLAG_HAS_SCALE | FLAG_HAS_ROT | FLAG_HAS_TRANS,

        FLAG_SCALE_FMT_MASK = FLAG_SCALE_FVS32_FMT | FLAG_SCALE_FVS48_FMT | FLAG_SCALE_FVS96_FMT,

        FLAG_ROT_FMT_MASK = FLAG_ROT_FVS32_FMT | FLAG_ROT_FVS48_FMT | FLAG_ROT_FVS96_FMT | FLAG_ROT_CV8_FMT |
                            FLAG_ROT_CV16_FMT | FLAG_ROT_CV32_FMT,

        FLAG_TRANS_FMT_MASK = FLAG_TRANS_FVS32_FMT | FLAG_TRANS_FVS48_FMT | FLAG_TRANS_FVS96_FMT
    };

    union AnmData {
        s32 toResAnmChrAnmData;
        f32 constValue;
    };

    s32 name;
    u32 flags;
    AnmData anms[1];
};

struct ResAnmChrInfoData {
    u16 numFrame;
    u16 numNode;
    AnmPolicy policy;
    ScalingRule scalingRule;
};

struct ResAnmChrData {
    ResBlockHeaderData header;
    u32 revision;
    s32 toResFileData;
    s32 toChrDataDic;
    s32 toResUserData;
    s32 name;
    s32 original_path;
    ResAnmChrInfoData info;
};

class ResAnmChr : public ResCommon<ResAnmChrData> {
public:
    static const u32 SIGNATURE = 'CHR0';
    static const int REVISION = 5;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResAnmChr);

    u32 GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const {
        return GetRevision() == REVISION;
    }

    void GetAnmResult(ChrAnmResult *pResult, u32 idx, f32 frame) const;

    const ResAnmChrNodeData *GetNodeAnm(int idx) const {
        return static_cast<ResAnmChrNodeData *>(ofs_to_obj<ResDic>(ref().toChrDataDic)[idx]);
    }
    const ResAnmChrNodeData *GetNodeAnm(u32 idx) const {
        return static_cast<ResAnmChrNodeData *>(ofs_to_obj<ResDic>(ref().toChrDataDic)[idx]);
    }

    s32 GetNodeAnmIndex(const ResName name) const {
        return ofs_to_obj<ResDic>(ref().toChrDataDic).GetIndex(name);
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

#endif
