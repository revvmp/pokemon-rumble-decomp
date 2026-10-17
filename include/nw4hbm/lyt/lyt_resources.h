#ifndef NW4HBM_LYT_RESOURCES_H
#define NW4HBM_LYT_RESOURCES_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_types.h"

namespace nw4hbm {
namespace lyt {

class ResourceAccessor;

namespace res {

struct BinaryFileHeader {
    char signature[4];
    u16 byteOrder;
    u16 version;
    u32 fileSize;
    u16 headerSize;
    u16 dataBlocks;
};

struct DataBlockHeader {
    char kind[4];
    u32 size;
};

struct Texture {
    u32 nameStrOffset;
    u8 type;
    u8 PADDING_0x5[0x8 - 0x5];
};

struct TextureList {
    res::DataBlockHeader blockHeader;
    u16 texNum;
    u8 PADDING_0xA[0xC - 0xA];
};

struct Font {
    u32 nameStrOffset;
    u8 type;
    u8 PADDING_0x5[0x8 - 0x5];
};

struct FontList {
    res::DataBlockHeader blockHeader;
    u16 fontNum;
    u8 PADDING_0xA[0xC - 0xA];
};

struct MaterialList {
    res::DataBlockHeader blockHeader;
    u16 materialNum;
    u8 PADDING_0xA[0xC - 0xA];
};

struct TexMap {
    u16 texIdx;
    u8 wrapS;
    u8 wrapT;
};

}

struct ResBlockSet {
    const res::TextureList* pTextureList;
    const res::FontList* pFontList;
    const res::MaterialList* pMaterialList;
    ResourceAccessor* pResAccessor;
};

struct MaterialResourceNum {
    u32 bits;

    u8 GetTexMapNum() const {
        return detail::GetBits(bits, 0, 4);
    }
    u8 GetTexSRTNum() const {
        return detail::GetBits(bits, 4, 4);
    }
    u8 GetTexCoordGenNum() const {
        return detail::GetBits(bits, 8, 4);
    }
    bool HasTevSwapTable() const {
        return detail::TestBit(bits, 12);
    }
    u8 GetIndTexSRTNum() const {
        return detail::GetBits(bits, 13, 2);
    }
    u8 GetIndTexStageNum() const {
        return detail::GetBits(bits, 15, 3);
    }
    u8 GetTevStageNum() const {
        return detail::GetBits(bits, 18, 5);
    }
    bool HasAlphaCompare() const {
        return detail::TestBit(bits, 23);
    }
    bool HasBlendMode() const {
        return detail::TestBit(bits, 24);
    }
    u8 GetChanCtrlNum() const {
        return detail::GetBits(bits, 25, 1);
    }
    u8 GetMatColNum() const {
        return detail::GetBits(bits, 27, 1);
    }
};

}
}

#endif
