#include "revolution/tpl.h"
#include "revolution/gx/GXTexture.h"
#include "revolution/os.h"

void TPLBind(TPLPalettePtr pal) {
    u16 i;

    if (pal->versionNumber != 2142000) {
        OSPanic(__FILE__, 25, "invalid version number for texture palette");
    }

    pal->descriptorArray = (TPLDescriptorPtr)((u32)pal->descriptorArray + (u32)pal);

    for (i = 0; i < pal->numDescriptors; i++) {
        if (pal->descriptorArray[i].textureHeader) {
            pal->descriptorArray[i].textureHeader =
                (TPLHeaderPtr)((u32)pal->descriptorArray[i].textureHeader + (u32)pal);

            if (!pal->descriptorArray[i].textureHeader->unpacked) {
                pal->descriptorArray[i].textureHeader->data =
                    (char*)((u32)pal->descriptorArray[i].textureHeader->data + (u32)pal);
                pal->descriptorArray[i].textureHeader->unpacked = TRUE;
            }
        }

        if (pal->descriptorArray[i].CLUTHeader) {
            pal->descriptorArray[i].CLUTHeader =
                (TPLClutHeaderPtr)((u32)pal->descriptorArray[i].CLUTHeader + (u32)pal);

            if (!pal->descriptorArray[i].CLUTHeader->unpacked) {
                pal->descriptorArray[i].CLUTHeader->data =
                    (char*)((u32)pal->descriptorArray[i].CLUTHeader->data + (u32)pal);
                pal->descriptorArray[i].CLUTHeader->unpacked = TRUE;
            }
        }
    }
}

TPLDescriptorPtr TPLGet(TPLPalettePtr pal, u32 id) {
    id %= pal->numDescriptors;
    return &pal->descriptorArray[id];
}

void TPLGetGXTexObjFromPalette(TPLPalettePtr pal, GXTexObj* texObj, u32 id) {
    TPLDescriptorPtr desc = TPLGet(pal, id);
    GXBool mipmap = desc->textureHeader->minLOD != desc->textureHeader->maxLOD ? GX_TRUE : GX_FALSE;

    GXInitTexObj(texObj, desc->textureHeader->data, desc->textureHeader->width, desc->textureHeader->height,
                 (GXTexFmt)desc->textureHeader->format, desc->textureHeader->wrapS, desc->textureHeader->wrapT, mipmap);

    GXInitTexObjLOD(texObj, desc->textureHeader->minFilter, desc->textureHeader->magFilter, desc->textureHeader->minLOD,
                    desc->textureHeader->maxLOD, desc->textureHeader->LODBias, GX_FALSE,
                    desc->textureHeader->edgeLODEnable, GX_ANISO_1);
}
