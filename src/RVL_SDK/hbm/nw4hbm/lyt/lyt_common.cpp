#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_layout.h"

#include "revolution/gx/GXBump.h"
#include "revolution/gx/GXTexture.h"
#include "revolution/gx/GXVert.h"

#include <string.h>

namespace nw4hbm {
namespace lyt {
namespace detail {

bool EqualsPaneName(const char* pLhs, const char* pRhs) {
    return strncmp(pLhs, pRhs, NW4R_LYT_RES_NAME_LEN) == 0;
}

bool EqualsMaterialName(const char* pLhs, const char* pRhs) {
    return strncmp(pLhs, pRhs, NW4R_LYT_MATERIAL_NAME_LEN) == 0;
}

bool TestFileHeader(const res::BinaryFileHeader& rHeader) {
    return rHeader.byteOrder == 0xFEFF && rHeader.version == 8;
}

bool TestFileHeader(const res::BinaryFileHeader& rHeader, u32 signature) {
    return static_cast<u32>(GetSignatureInt(rHeader.signature)) == signature && TestFileHeader(rHeader);
}

TexCoordAry::TexCoordAry() : mCap(0), mNum(0), mpData(nullptr) {}

void TexCoordAry::Free() {
    if (mpData) {
        Layout::FreeMemory(mpData);
        mpData = nullptr;
        mCap = 0;
        mNum = 0;
    }
}

void TexCoordAry::Reserve(u8 num) {
    if (mCap < num) {
        Free();
        mpData = static_cast<TexCoord*>(Layout::AllocMemory(sizeof(*mpData) * num));

        if (mpData) {
            mCap = num;
        }
    }
}

void TexCoordAry::SetSize(u8 num) {
    if (mpData == nullptr) {
        return;
    }

    if (num > mCap) {
        return;
    }

    static TexCoord texCoords = {
        math::VEC2(0.0f, 0.0f),
        math::VEC2(1.0f, 0.0f),
        math::VEC2(0.0f, 1.0f),
        math::VEC2(1.0f, 1.0f),
    };

    for (int j = mNum; j < num; j++) {
        for (int i = 0; i < VERTEXCOLOR_MAX; i++) {
            mpData[j][i] = texCoords[i];
        }
    }

    mNum = num;
}

void TexCoordAry::Copy(const void* pSrc, u8 num) {
    mNum = ut::Max(mNum, num);
    const TexCoord* pSrcCoords = static_cast<const TexCoord*>(pSrc);

    for (int j = 0; j < num; j++) {
        for (int i = 0; i < VERTEXCOLOR_MAX; i++) {
            mpData[j][i] = pSrcCoords[j][i];
        }
    }
}

bool IsModulateVertexColor(ut::Color* pColors, u8 glbAlpha) {
    if (glbAlpha != 0xFF) {
        return true;
    }

    if (pColors) {
        if (pColors[0] != 0xFFFFFFFF || pColors[1] != 0xFFFFFFFF || pColors[2] != 0xFFFFFFFF || pColors[3] != 0xFFFFFFFF) {
            return true;
        }
    }

    return false;
}

ut::Color MultipleAlpha(const ut::Color color, u8 alpha) {
    ut::Color ret = color;

    if (alpha != 0xFF) {
        ret.a = color.a * alpha / 255;
    }

    return ret;
}

void MultipleAlpha(ut::Color* pDst, const ut::Color* pSrc, u8 alpha) {
    for (int i = 0; i < VERTEXCOLOR_MAX; i++) {
        pDst[i] = MultipleAlpha(pSrc[i], alpha);
    }
}

void SetVertexFormat(bool modulate, u8 numCoord) {
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);

    if (modulate) {
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    }

    for (int i = 0; i < numCoord; i++) {
        GXSetVtxDesc(static_cast<GXAttr>(GX_VA_TEX0 + i), GX_DIRECT);
    }

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_CLR_RGB, GX_F32, 0);

    if (modulate) {
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    }

    for (int i = 0; i < numCoord; i++) {
        GXSetVtxAttrFmt(GX_VTXFMT0, static_cast<GXAttr>(GX_VA_TEX0 + i), GX_CLR_RGBA, GX_F32, 0);
    }
}

void DrawQuad(const math::VEC2& rBase, const Size& rSize, u8 num, const TexCoord* pCoords, const ut::Color* pColors) {
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);

    GXPosition2f32(rBase.x, rBase.y);

    if (pColors) {
        GXColor1u32(pColors[0]);
    }

    for (int i = 0; i < num; i++) {
        GXTexCoord2f32(pCoords[i][0].x, pCoords[i][0].y);
    }

    GXPosition2f32(rBase.x + rSize.width, rBase.y);

    if (pColors) {
        GXColor1u32(pColors[1]);
    }

    for (int i = 0; i < num; i++) {
        GXTexCoord2f32(pCoords[i][1].x, pCoords[i][1].y);
    }

    GXPosition2f32(rBase.x + rSize.width, rBase.y + rSize.height);

    if (pColors) {
        GXColor1u32(pColors[3]);
    }

    for (int i = 0; i < num; i++) {
        GXTexCoord2f32(pCoords[i][3].x, pCoords[i][3].y);
    }

    GXPosition2f32(rBase.x, rBase.y + rSize.height);

    if (pColors) {
        GXColor1u32(pColors[2]);
    }

    for (int i = 0; i < num; i++) {
        GXTexCoord2f32(pCoords[i][2].x, pCoords[i][2].y);
    }

    GXEnd();
}

void DrawQuad(const math::VEC2& rBase, const Size& rSize, u8 num, const TexCoord* pCoords, const ut::Color* pColors, u8 alpha) {
    ut::Color colors[VERTEXCOLOR_MAX];

    if (pColors) {
        MultipleAlpha(colors, pColors, alpha);
    }

    DrawQuad(rBase, rSize, num, pCoords, pColors ? colors : nullptr);
}

void DrawLine(const math::VEC2& rPos, const Size& rSize, ut::Color color) {
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanMatColor(GX_COLOR0A0, color);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetNumIndStages(0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    GXSetLineWidth(6, GX_TO_ZERO);

    GXBegin(GX_LINESTRIP, GX_VTXFMT0, 5);
    GXPosition2f32(rPos.x, rPos.y);
    GXPosition2f32(rPos.x + rSize.width, rPos.y);
    GXPosition2f32(rPos.x + rSize.width, rPos.y + rSize.height);
    GXPosition2f32(rPos.x, rPos.y + rSize.height);
    GXPosition2f32(rPos.x, rPos.y);
    GXEnd();
}

void InitGXTexObjFromTPL(GXTexObj* pTexObj, TPLPalette* pPalette, u32 id) {
    if (pPalette->descriptorArray < reinterpret_cast<TPLDescriptor*>(0x80000000)) {
        TPLBind(pPalette);
    }

    TPLDescriptor* pDesc = TPLGet(pPalette, id);
    GXBool mipMap = pDesc->textureHeader->minLOD != pDesc->textureHeader->maxLOD ? GX_TRUE : GX_FALSE;

    if (pDesc->CLUTHeader) {
        GXInitTexObjCI(pTexObj, pDesc->textureHeader->data, pDesc->textureHeader->width, pDesc->textureHeader->height,
                       static_cast<GXCITexFmt>(pDesc->textureHeader->format), pDesc->textureHeader->wrapS,
                       pDesc->textureHeader->wrapT, mipMap, 0);
        GXInitTexObjUserData(pTexObj, pDesc->CLUTHeader);
    } else {
        GXInitTexObj(pTexObj, pDesc->textureHeader->data, pDesc->textureHeader->width, pDesc->textureHeader->height,
                     static_cast<GXTexFmt>(pDesc->textureHeader->format), pDesc->textureHeader->wrapS,
                     pDesc->textureHeader->wrapT, mipMap);
    }

    GXInitTexObjLOD(pTexObj, pDesc->textureHeader->minFilter, pDesc->textureHeader->magFilter, pDesc->textureHeader->minLOD,
                    pDesc->textureHeader->maxLOD, pDesc->textureHeader->LODBias, GX_FALSE, pDesc->textureHeader->edgeLODEnable,
                    GX_ANISO_1);
}

}
}
}
