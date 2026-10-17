#ifndef NW4R_G3D_RES_RES_MDL_H
#define NW4R_G3D_RES_RES_MDL_H

#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resdict.h>
#include <nw4r/g3d/res/g3d_resmat.h>
#include <nw4r/g3d/res/g3d_resnode.h>
#include <nw4r/g3d/res/g3d_resshp.h>
#include <nw4r/g3d/res/g3d_resvtx.h>

#include <nw4r/math/math_types.h>

namespace nw4r {
namespace g3d {

struct ResMdlInfoDataTypedef {
    enum TexMatrixMode {
        TEXMATRIXMODE_MAYA,
        TEXMATRIXMODE_XSI,
        TEXMATRIXMODE_3DSMAX
    };

    enum EnvelopeMatrixMode {
        EVPMATRIXMODE_NORMAL,
        EVPMATRIXMODE_APPROX,
        EVPMATRIXMODE_EXACT
    };
};

enum ScalingRule {
    SCALINGRULE_STANDARD,
    SCALINGRULE_SOFTIMAGE,
    SCALINGRULE_MAYA
};

struct ResMtxIDToNodeIDData {
    u32 numMtxID;
};

struct ResMdlInfoData : ResMdlInfoDataTypedef {
    u32 size;
    s32 toResMdlData;
    ScalingRule scaling_rule;
    TexMatrixMode tex_mtx_mode;
    s32 vertex_size;
    s32 triangle_size;
    s32 original_path;
    u32 numViewMtx;
    bool need_nrm_mtx_array;
    bool need_tex_mtx_array;
    bool is_valid_volume;
    u8 envelope_mtx_mode;
    s32 toMtxIDToNodeID;
    math::_VEC3 volume_min;
    math::_VEC3 volume_max;
};

class ResMdlInfo : public ResCommon<ResMdlInfoData>, public ResMdlInfoDataTypedef {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResMdlInfo);

    ScalingRule GetScalingRule() const {
        return ref().scaling_rule;
    }

    u32 GetNumViewMtx() const {
        return ref().numViewMtx;
    }

    EnvelopeMatrixMode GetEnvelopeMatrixMode() const {
        return static_cast<EnvelopeMatrixMode>(ref().envelope_mtx_mode);
    }

    u32 GetNumPosNrmMtx() const {
        const ResMtxIDToNodeIDData *pData = reinterpret_cast<const ResMtxIDToNodeIDData *>(
            reinterpret_cast<const u8 *>(&ref()) + ref().toMtxIDToNodeID
        );

        return pData->numMtxID;
    }

    s32 GetNodeIDFromMtxID(u32 id) const {
        const s32 *pArray = reinterpret_cast<const s32 *>(
            reinterpret_cast<const u8 *>(&ref()) + ref().toMtxIDToNodeID + sizeof(ResMtxIDToNodeIDData)
        );

        return pArray[id];
    }
};

struct ResMdlData {
    ResBlockHeaderData header;
    u32 revision;
    s32 toResFileData;
    s32 toResByteCodeDic;
    s32 toResNodeDic;
    s32 toResVtxPosDic;
    s32 toResVtxNrmDic;
    s32 toResVtxClrDic;
    s32 toResVtxTexCoordDic;
    s32 toResVtxFurVecDic;
    s32 toResVtxFurPosDic;
    s32 toResMatDic;
    s32 toResTevDic;
    s32 toResShpDic;
    s32 toResTexNameToTexPlttInfoDic;
    s32 toResPlttNameToTexPlttInfoDic;
    s32 toResUserData;
    s32 name;
    ResMdlInfoData info;
};

class ResMdl : public ResCommon<ResMdlData> {
public:
    static const u32 SIGNATURE = 'MDL0';
    static const int REVISION = 11;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResMdl);

    void Init();
    void Terminate();

    bool Bind(const ResFile file);
    void Release();

    u32 GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const {
        return GetRevision() == REVISION;
    }

    const char *GetName() const {
        return ofs_to_ptr<const char>(ref().name);
    }

    ResFile GetParent() const;

    const u8 *GetResByteCode(const char *pName) const;
    u32 GetResByteCodeNumEntries() const;

    ResNode GetResNode(const char *pName) const;
    ResNode GetResNode(const ResName name) const;
    ResNode GetResNode(int idx) const;
    ResNode GetResNode(u32 idx) const;
    u32 GetResNodeNumEntries() const;

    ResVtxPos GetResVtxPos(const ResName name) const;
    ResVtxPos GetResVtxPos(int idx) const;
    ResVtxPos GetResVtxPos(u32 idx) const;
    u32 GetResVtxPosNumEntries() const;

    ResVtxNrm GetResVtxNrm(const ResName name) const;
    ResVtxNrm GetResVtxNrm(int idx) const;
    ResVtxNrm GetResVtxNrm(u32 idx) const;
    u32 GetResVtxNrmNumEntries() const;

    ResVtxClr GetResVtxClr(const ResName name) const;
    ResVtxClr GetResVtxClr(int idx) const;
    ResVtxClr GetResVtxClr(u32 idx) const;
    u32 GetResVtxClrNumEntries() const;

    ResVtxTexCoord GetResVtxTexCoord(int idx) const;
    u32 GetResVtxTexCoordNumEntries() const;

    ResVtxFurPos GetResVtxFurPos(int idx) const;
    u32 GetResVtxFurPosNumEntries() const;

    ResVtxFurVec GetResVtxFurVec(int idx) const;
    u32 GetResVtxFurVecNumEntries() const;

    ResMat GetResMat(const char *pName) const;
    ResMat GetResMat(const ResName name) const;
    ResMat GetResMat(int idx) const;
    ResMat GetResMat(u32 idx) const;
    u32 GetResMatNumEntries() const;

    ResShp GetResShp(const char *pName) const;
    ResShp GetResShp(int idx) const;
    ResShp GetResShp(u32 idx) const;
    u32 GetResShpNumEntries() const;

    ResTexPlttInfoOffset GetResTexPlttInfoOffsetFromTexName(int idx) const;
    u32 GetResTexPlttInfoOffsetFromTexNameNumEntries() const;

    ResMdlInfo GetResMdlInfo() {
        return ResMdlInfo(&ref().info);
    }
    ResMdlInfo GetResMdlInfo() const {
        return ResMdlInfo(const_cast<ResMdlInfoData *>(&ref().info));
    }
};

}
}

#endif
