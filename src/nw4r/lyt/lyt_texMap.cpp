#include <nw4r/lyt/lyt_texMap.h>
#include <revolution/gx/GXGet.h>
#include <revolution/gx/GXTexture.h>

namespace nw4r {
namespace lyt {

void TexMap::Get(GXTexObj *pTexObj) const {
    if (detail::IsCITexelFormat(GetTexelFormat())) {
        u32 tlutName = GXGetTexObjTlut(pTexObj);
        GXInitTexObjCI(
            pTexObj, GetImage(), GetWidth(), GetHeight(), (GXCITexFmt)GetTexelFormat(), GetWrapModeS(), GetWrapModeT(),
            IsMipMap(), tlutName
        );
    } else {
        GXInitTexObj(
            pTexObj, GetImage(), GetWidth(), GetHeight(), GetTexelFormat(), GetWrapModeS(), GetWrapModeT(), IsMipMap()
        );
    }

    GXInitTexObjLOD(
        pTexObj, GetMinFilter(), GetMagFilter(), GetMinLOD(), GetMaxLOD(), GetLODBias(), IsBiasClampEnable(),
        IsEdgeLODEnable(), GetAnisotropy()
    );
}

void TexMap::Get(GXTlutObj *pTLUTObj) const {
    GXInitTlutObj(pTLUTObj, GetPalette(), GetPaletteFormat(), GetPaletteEntryNum());
}

void TexMap::Set(const GXTexObj &pTexObj) {
    void *pImage;
    u16 width, height;
    GXTexFmt format;
    GXTexWrapMode wrapS, wrapT;
    u8 mipmap;
    GXGetTexObjAll(&pTexObj, &pImage, &width, &height, &format, &wrapS, &wrapT, &mipmap);
    SetImage(pImage);
    SetSize(width, height);
    SetTexelFormat(format);
    SetWrapMode(wrapS, wrapT);
    SetMipMap(mipmap);

    GXTexFilter minFilter, magFilter;
    f32 minLOD, maxLOD, LODBias;
    u8 biasClampEnable, edgeLODEnable;
    GXAnisotropy anisotropy;
    GXGetTexObjLODAll(
        &pTexObj, &minFilter, &magFilter, &minLOD, &maxLOD, &LODBias, &biasClampEnable, &edgeLODEnable, &anisotropy
    );
    SetFilter(minFilter, magFilter);
    SetLOD(minLOD, maxLOD);
    SetLODBias(LODBias);
    SetBiasClampEnable(biasClampEnable);
    SetEdgeLODEnable(edgeLODEnable);
    SetAnisotropy(anisotropy);
}

void TexMap::Set(TPLPalette *pal, u32 id) {

    if ((unsigned int)pal->descriptorArray < 0x80000000) {
        TPLBind(pal);
    }

    Set(TPLGet(pal, id));
}

void TexMap::Set(const TPLDescriptor *pTPLDesc) {
    TPLHeader *tplHead = pTPLDesc->textureHeader;
    SetImage(tplHead->data);
    SetSize(tplHead->width, tplHead->height);
    SetTexelFormat(static_cast<GXTexFmt>(tplHead->format));
    SetWrapMode(tplHead->wrapS, tplHead->wrapT);

    bool bMipMap = (tplHead->minLOD != tplHead->maxLOD);
    SetMipMap(bMipMap);

    SetFilter(tplHead->minFilter, tplHead->magFilter);
    SetLOD(tplHead->minLOD, tplHead->maxLOD);
    SetLODBias(tplHead->LODBias);
    SetEdgeLODEnable(tplHead->edgeLODEnable);

    TPLClutHeader *pTPLCLUTHeader = pTPLDesc->CLUTHeader;
    if (pTPLCLUTHeader != nullptr) {
        SetPalette(pTPLCLUTHeader->data);
        SetPaletteFormat(pTPLCLUTHeader->format);
        SetPaletteEntryNum(pTPLCLUTHeader->numEntries);
    } else {
        SetPalette(nullptr);
        SetPaletteFormat(GX_TL_IA8);
        SetPaletteEntryNum(0);
    }
}

void TexMap::ReplaceImage(TPLPalette *pal, u32 id) {

    if ((unsigned int)pal->descriptorArray < 0x80000000) {
        TPLBind(pal);
    }

    ReplaceImage(TPLGet(pal, id));
}

void TexMap::ReplaceImage(const TPLDescriptor *pTPLDesc) {
    TPLHeader *tplHead = pTPLDesc->textureHeader;
    SetImage(tplHead->data);
    SetSize(tplHead->width, tplHead->height);
    SetTexelFormat(static_cast<GXTexFmt>(tplHead->format));

    TPLClutHeader *pTPLCLUTHeader = pTPLDesc->CLUTHeader;
    if (pTPLCLUTHeader != nullptr) {
        SetPalette(pTPLCLUTHeader->data);
        SetPaletteFormat(pTPLCLUTHeader->format);
        SetPaletteEntryNum(pTPLCLUTHeader->numEntries);
    } else {
        SetPalette(nullptr);
        SetPaletteFormat(GX_TL_IA8);
        SetPaletteEntryNum(0);
    }
}

}
}
