#ifndef NW4R_LYT_RESOURCES_H
#define NW4R_LYT_RESOURCES_H
#include <nw4r/lyt/lyt_types.h>
#include <nw4r/math/math_types.h>
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
struct FontList;
struct Material;
struct MaterialList {
    DataBlockHeader blockHeader;
    u16 materialNum;
    u8 padding[2];
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
