#ifndef NW4R_G3D_RES_RES_SHP_H
#define NW4R_G3D_RES_RES_SHP_H

#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>
#include <nw4r/g3d/res/g3d_resvtx.h>

#include <revolution/GX.h>

namespace nw4r {
namespace g3d {

struct ResPrePrimDL {
    union {
        struct {
            u8 cullMode[10];
            u8 vtxDescv[21];
            u8 PADDING_0x1F;
            u8 vtxFmtv[GX_CP_CMD_SZ * 3];
            u8 array[GX_POS_MTX_ARRAY - GX_VA_POS][GX_CP_CMD_SZ * 2];
            u8 PADDING_0xC2[0xE0 - 0xC2];
        } dl;

        u8 data[0xE0];
    };
};

class ResShpPrePrim : public ResCommon<ResPrePrimDL> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF_EX(ResShpPrePrim, ResPrePrimDL);

    void DCStore(bool sync);
};

struct ResCacheVtxDescv {
    union {
        u32 data_u32[0xC / sizeof(u32)];
        u8 data[0xC / sizeof(u8)];
    };

    void Clear() {
        data_u32[0] = data_u32[1] = data_u32[2] = 0;
    }

    bool operator==(const ResCacheVtxDescv &rRhs) const {
        return data_u32[0] == rRhs.data_u32[0] && data_u32[1] == rRhs.data_u32[1] && data_u32[2] == rRhs.data_u32[2];
    }
};

struct ResMtxSetUsed {
    u32 numMtxID;
    u16 vecMtxID[2];
};

struct ResShpData {
    enum IDFlag {
        ID_FLAG_ENVELOPE = (1 << 31),
    };

    enum Flag {
        FLAG_INVISIBLE = (1 << 1),
    };

    u32 size;
    s32 toResMdlData;
    s32 curMtxIdx;
    ResCacheVtxDescv cache;
    ResTagDLData tagPrePrimDL;
    ResTagDLData tagPrimDL;
    u32 vcdBitmap;
    u32 flag;
    s32 name;
    u32 id;
    u32 numVtx;
    u32 numPolygon;
    s16 idVtxPosition;
    s16 idVtxNormal;
    s16 idVtxColor[GX_VA_TEX0 - GX_VA_CLR0];
    s16 idVtxTexCoord[GX_POS_MTX_ARRAY - GX_VA_TEX0];
    s16 idVtxFurVec;
    s16 idVtxFurPos;
    s32 toMtxSetUsed;
    ResMtxSetUsed msu;
};

class ResShp : public ResCommon<ResShpData> {
public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResShp);

    void Init();
    void Terminate();

    ResMdl GetParent() const;

    bool GXGetVtxDescv(GXVtxDescList *pList) const;
    bool GXGetVtxAttrFmtv(GXVtxAttrFmtList *pList) const;
    void GXSetArray(GXAttr attr, const void *pBase, u8 stride);
    void DisableSetArray(GXAttr attr);

    ResVtxPos GetResVtxPos() const;
    ResVtxNrm GetResVtxNrm() const;
    ResVtxClr GetResVtxClr(u32 idx) const;
    ResVtxTexCoord GetResVtxTexCoord(u32 idx) const;
    ResVtxClr GetResVtxFurVec(u32 idx) const;
    ResVtxFurPos GetResVtxFurPos() const;

    void CallPrePrimitiveDisplayList(bool sync, bool cacheIsSame) const;
    void CallPrimitiveDisplayList(bool sync) const;

    ResTagDL GetPrePrimDLTag() {
        return ResTagDL(&ref().tagPrePrimDL);
    }
    ResTagDL GetPrePrimDLTag() const {
        return ResTagDL(const_cast<ResTagDLData *>(&ref().tagPrePrimDL));
    }

    ResTagDL GetPrimDLTag() {
        return ResTagDL(&ref().tagPrimDL);
    }
    ResTagDL GetPrimDLTag() const {
        return ResTagDL(const_cast<ResTagDLData *>(&ref().tagPrimDL));
    }

    ResShpPrePrim GetResShpPrePrim() {
        return ResShpPrePrim(GetPrePrimDLTag().GetDL());
    }
    ResShpPrePrim GetResShpPrePrim() const {
        return ResShpPrePrim(GetPrePrimDLTag().GetDL());
    }

    bool ExistVtxDesc(GXAttr attr) const {
        return ref().vcdBitmap & (1 << attr);
    }

    bool IsVisible() const {
        return !(ref().flag & ResShpData::FLAG_INVISIBLE);
    }

    void SetVisibility(bool visible) {
        if (visible) {
            ptr()->flag &= ~ResShpData::FLAG_INVISIBLE;
        } else {
            ptr()->flag |= ResShpData::FLAG_INVISIBLE;
        }
    }

    void DCStore(bool sync);
};

}
}

#endif
