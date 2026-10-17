#ifndef NW4R_LYT_RESOURCES_H
#define NW4R_LYT_RESOURCES_H
#include <nw4r/lyt/lyt_types.h>
#include <nw4r/math/math_types.h>
#include <revolution/GX.h>
namespace nw4r {
namespace lyt {
namespace res {
struct Pane {
    DataBlockHeader blockHeader;
    u8 flag;
    u8 basePosition;
    u8 alpha;
    u8 padding;
    char name[16];
    char userData[8];
    math::VEC3 translate;
    math::VEC3 rotate;
    math::VEC2 scale;
    Size size;
};
struct Group {
    DataBlockHeader blockHeader;
    char name[16];
    u16 paneNum;
    u8 padding[2];
};
struct ExtUserDataList {
    DataBlockHeader blockHeader;
    u16 num;
    u8 padding[2];
};
struct ExtUserData {
    u32 nameOffset;
    u32 dataOffset;
    u16 dataNum;
    u8 type;
    u8 padding;
    const char* GetName() const {
        return nameOffset != 0 ? reinterpret_cast<const char*>(this) + nameOffset : nullptr;
    }
};
struct TextureList;
struct TexMap {

    GXTexWrapMode GetWarpModeS() const {
        return (GXTexWrapMode)(wrapSflt & 3);
    }

    GXTexWrapMode GetWarpModeT() const {
        return (GXTexWrapMode)(wrapTflt & 3);
    }

    GXTexFilter GetMinFilter() const {
        u8 bitData = (wrapSflt >> 2) & 7;
        return (GXTexFilter)((bitData + 1) & 7);
    }

    GXTexFilter GetMagFilter() const {
        u8 bitData = (wrapTflt >> 2) & 1;
        return (GXTexFilter)((bitData + 1) & 1);
    }
    u16 texIdx;
    u8 wrapSflt;
    u8 wrapTflt;
};
struct MaterialResourceNum {

    u8 GetTevStageNum() const {
        return (bits >> 18) & 0x1F;
    }

    u8 GetIndTexStageNum() const {
        return (bits >> 15) & 0x7;
    }

    u8 GetIndTexSRTNum() const {
        return (bits >> 13) & 0x3;
    }

    bool HasBlendMode() const {
        return (bits >> 24) & 1;
    }

    bool HasAlphaCompare() const {
        return (bits >> 23) & 1;
    }

    bool HasTevSwapTable() const {
        return (bits >> 12) & 1;
    }

    u8 GetMatColNum() const {
        return (bits >> 27) & 1;
    }

    u8 GetChanCtrlNum() const {
        return (bits >> 25) & 1;
    }

    u8 GetTexCoordGenNum() const {
        return (bits >> 8) & 0xF;
    }

    u8 GetTexSRTNum() const {
        return (bits >> 4) & 0xF;
    }

    u8 GetTexMapNum() const {
        return (bits >> 0) & 0xF;
    }

    u32 bits;
};
struct Material {
    char name[20];
    GXColorS10 tevCols[3];
    GXColor tevKCols[4];
    MaterialResourceNum resNum;
};
struct Texture {
    u32 nameStrOffset;
    u8 type;
    u8 padding[3];
};
struct TextureList {
    DataBlockHeader blockHeader;
    u16 texNum;
    u8 padding[2];
};
struct FontList;
struct Material;
struct MaterialList {
    DataBlockHeader blockHeader;
    u16 materialNum;
    u8 padding[2];
};
struct InflationLRTB {
    f32 l;
    f32 r;
    f32 t;
    f32 b;
};
struct Window : Pane {
    InflationLRTB inflation;
    u8 frameNum;
    u8 padding[3];
    u32 contentOffset;
    u32 frameOffsetTableOffset;
};
struct WindowContent {
    u32 vtxCols[4];
    u16 materialIdx;
    u8 texCoordNum;
    u8 padding;
};
struct WindowFrame {
    u16 materialIdx;
    u8 textureFlip;
    u8 padding;
};
}
class ResourceAccessor;
struct ResBlockSet {
    const res::TextureList* pTextureList;
    const res::FontList* pFontList;
    const res::MaterialList* pMaterialList;
    ResourceAccessor* pResAccessor;
};
}
}
#endif
