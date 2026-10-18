#ifndef NW4R_EF_RES_TEXTURE_H
#define NW4R_EF_RES_TEXTURE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ef/ef_types.h>

namespace nw4r {
namespace ef {

struct TextureData {
    char* name;
    u16 width;
    u16 height;
    u32 dataSize;
    u8 format;
    u8 tlutFormat;
    u16 tlutEntries;
    u32 tlutSize;
    u8 mipmap;
    u8 min_filt;
    u8 mag_filt;
    u8 reserved[1];
    f32 lod_bias;
    u8* texture;
    u8* tlut;
    u32 reserve[7];
};

class ResTexture {
private:
    TextureData* mData;

public:
    explicit ResTexture(TextureData* pData = NULL) : mData(pData) {}
    explicit ResTexture(const ResTexture& rOther) : mData(rOther.mData) {}

    bool IsValid() const {
        return mData != NULL;
    }

    TextureData* ptr() {
        return mData;
    }

    const TextureData* ptr() const {
        return mData;
    }

    void SetWidth(u16 width) {
        mData->width = width;
    }

    void SetHeight(u16 height) {
        mData->height = height;
    }

    void SetFormat(u8 format) {
        mData->format = format;
    }

    void SetMipmap(u8 mipmap) {
        mData->mipmap = mipmap;
    }

    void SetTexture(u8* pTexture) {
        mData->texture = pTexture;
    }

    void SetTlutFormat(u8 format) {
        mData->tlutFormat = format;
    }

    void SetTlutEntries(u16 entries) {
        mData->tlutEntries = entries;
    }

    void SetTlut(u8* pTlut) {
        mData->tlut = pTlut;
    }
};

}
}

#endif
