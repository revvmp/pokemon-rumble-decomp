#ifndef NW4R_LYT_TEXMAP_H
#define NW4R_LYT_TEXMAP_H
#include <nw4r/lyt/lyt_common.h>
#include <revolution/TPL.h>

#include <revolution/GX.h>

namespace nw4r {
namespace lyt {
class TexMap {
public:
    TexMap(TPLPalette *pTPLPalette, u32 id) {
        Set(pTPLPalette, id);
    }

    TexMap() {
        mpImage = nullptr;
        mWidth = 0;
        mHeight = 0;

        mBits.textureFormat = 0;
        mBits.mipmap = 0;

        mBits.wrapS = 0;
        mBits.wrapT = 0;
        mBits.minFilter = 1;
        mBits.magFilter = 1;

        mMinLOD = 0.0f;
        mMaxLOD = 0.0f;
        mLODBias = 0;
        mBits.biasClampEnable = 0;
        mBits.edgeLODEnable = 0;

        mpPalette = nullptr;
        mBits.anisotropy = 0;
        mBits.paletteFormat = 0;
        mPaletteEntryNum = 0;
    }

    void Get(GXTexObj *) const;
    void Get(GXTlutObj *) const;

    void Set(const TexMap &t) {
        *this = t;
    }

    void Set(const GXTexObj &);
    void Set(TPLPalette *, u32);
    void Set(const TPLDescriptor *);

    void ReplaceImage(TPLPalette *, u32);
    void ReplaceImage(const TPLDescriptor *);

    void *GetImage() const {
        return mpImage;
    }
    void SetImage(void *img) {
        mpImage = img;
    }

    void *GetPalette() const {
        return mpPalette;
    }
    void SetPalette(void *pal) {
        mpPalette = pal;
    }

    u16 GetWidth() const {
        return mWidth;
    }
    u16 GetHeight() const {
        return mHeight;
    }
    void SetSize(u16 w, u16 h) {
        mWidth = w;
        mHeight = h;
    }

    f32 GetMinLOD() const {
        return mMinLOD;
    }
    f32 GetMaxLOD() const {
        return mMaxLOD;
    }
    void SetLOD(f32 min, f32 max) {
        mMinLOD = min;
        mMaxLOD = max;
    }

    f32 GetLODBias() const {
        return mLODBias / 256.0f;
    }
    void SetLODBias(f32 val) {
        mLODBias = u16(val * 256.0f);
    }

    u16 GetPaletteEntryNum() const {
        return mPaletteEntryNum;
    }
    void SetPaletteEntryNum(u16 num) {
        mPaletteEntryNum = num;
    }

    GXTexFmt GetTexelFormat() const {
        return (GXTexFmt)mBits.textureFormat;
    }
    void SetTexelFormat(GXTexFmt fmt) {
        mBits.textureFormat = fmt;
    }

    bool IsMipMap() const {
        return mBits.mipmap;
    }
    void SetMipMap(bool b) {
        mBits.mipmap = b;
    }

    GXTexWrapMode GetWrapModeS() const {
        return (GXTexWrapMode)mBits.wrapS;
    }
    GXTexWrapMode GetWrapModeT() const {
        return (GXTexWrapMode)mBits.wrapT;
    }
    void SetWrapMode(GXTexWrapMode wrapS, GXTexWrapMode wrapT) {
        mBits.wrapS = wrapS;
        mBits.wrapT = wrapT;
    }

    GXTexFilter GetMinFilter() const {
        return (GXTexFilter)mBits.minFilter;
    }
    GXTexFilter GetMagFilter() const {
        return (GXTexFilter)mBits.magFilter;
    }
    void SetFilter(GXTexFilter minFlt, GXTexFilter magFlt) {
        mBits.minFilter = minFlt;
        mBits.magFilter = magFlt;
    }

    bool IsBiasClampEnable() const {
        return mBits.biasClampEnable;
    }
    void SetBiasClampEnable(bool b) {
        mBits.biasClampEnable = b;
    }

    bool IsEdgeLODEnable() const {
        return mBits.edgeLODEnable;
    }
    void SetEdgeLODEnable(bool b) {
        mBits.edgeLODEnable = b;
    }

    GXAnisotropy GetAnisotropy() const {
        return (GXAnisotropy)mBits.anisotropy;
    }
    void SetAnisotropy(GXAnisotropy a) {
        mBits.anisotropy = a;
    }

    GXTlutFmt GetPaletteFormat() const {
        return (GXTlutFmt)mBits.paletteFormat;
    }
    void SetPaletteFormat(GXTlutFmt fmt) {
        mBits.paletteFormat = fmt;
    }

private:
    void *mpImage;
    void *mpPalette;
    u16 mWidth;
    u16 mHeight;
    f32 mMinLOD;
    f32 mMaxLOD;
    u16 mLODBias;
    u16 mPaletteEntryNum;
    struct {
        u32 textureFormat : 4;
        u32 mipmap : 1;
        u32 wrapS : 2;
        u32 wrapT : 2;
        u32 minFilter : 3;
        u32 magFilter : 1;
        u32 biasClampEnable : 1;
        u32 edgeLODEnable : 1;
        u32 anisotropy : 2;
        u32 paletteFormat : 2;
    } mBits;
};
}
}

#endif
