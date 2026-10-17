#ifndef NW4R_G3D_RES_RES_TEX_H
#define NW4R_G3D_RES_RES_TEX_H
#include <nw4r/types_nw4r.h>

#include <nw4r/g3d/res/g3d_rescommon.h>

#include <revolution/GX.h>

namespace nw4r {
namespace g3d {

struct ResTexData {
    enum Flag {
        FLAG_CIFMT = (1 << 0),
    };

    ResBlockHeaderData header;
    u32 revision;
    s32 toResFileData;
    s32 toTexData;
    s32 name;
    u32 flag;
    u16 width;
    u16 height;
    union {
        GXTexFmt fmt;
        GXCITexFmt cifmt;
    };
    u32 mipmap_level;
    f32 min_lod;
    f32 max_lod;
    s32 original_path;
    s32 toResUserData;
};

class ResTex : public ResCommon<ResTexData> {
public:
    static const u32 SIGNATURE = 'TEX0';
    static const int REVISION = 3;

public:
    NW4R_G3D_RESOURCE_FUNC_DEF(ResTex);

    void Init();

    u32 GetRevision() const {
        return ref().revision;
    }

    bool CheckRevision() const;

    bool GetTexObjParam(
        void **ppTexData, u16 *pWidth, u16 *pHeight, GXTexFmt *pFormat, f32 *pMinLod, f32 *pMaxLod, GXBool *pMipMap
    ) const;

    bool GetTexObjCIParam(
        void **ppTexData, u16 *pWidth, u16 *pHeight, GXCITexFmt *pFormatCI, f32 *pMinLod, f32 *pMaxLod, GXBool *pMipMap
    ) const;

    bool IsCIFmt() const {
        return ref().flag & ResTexData::FLAG_CIFMT;
    }

    u16 GetWidth() const {
        return ref().width;
    }
    u16 GetHeight() const {
        return ref().height;
    }

    const void *GetTexData() const {
        return ofs_to_ptr<void>(ref().toTexData);
    }
};

}
}

#endif
