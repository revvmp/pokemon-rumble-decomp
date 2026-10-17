#include "nw4r/lyt/lyt_common.h"
#include <nw4r/lyt/lyt_layout.h>

#include <revolution/GX.h>

#include <string.h>

namespace nw4r {
namespace lyt {
namespace detail {

bool EqualsResName(const char* pLhs, const char* pRhs) {
    return strncmp(pLhs, pRhs, 16) == 0;
}

bool EqualsMaterialName(const char* pLhs, const char* pRhs) {
    return strncmp(pLhs, pRhs, 20) == 0;
}

bool TestFileHeader(const res::BinaryFileHeader& rHeader) {
    return rHeader.byteOrder == 0xFEFF;
}

bool TestFileHeader(const res::BinaryFileHeader& rHeader, u32 signature) {
    return GetSignatureInt(rHeader.signature) == signature &&
           TestFileHeader(rHeader);
}

TexCoordAry::TexCoordAry() : mCap(0), mNum(0), mpData(nullptr) {}

void TexCoordAry::Free() {
    if (mpData != nullptr) {
        Layout::DeleteObj(mpData);
        mpData = nullptr;
        mCap = 0;
        mNum = 0;
    }
}

void TexCoordAry::Reserve(u8 num) {
    if (mCap < num) {
        Free();

        void* pMem = Layout::AllocMemory(num * sizeof(TexCoord));
        mpData = pMem != nullptr ? static_cast<TexCoord*>(pMem) : nullptr;

        if (mpData != nullptr) {
            mCap = num;
        }
    }
}

void TexCoordAry::SetSize(u8 num) {
    if (mpData != nullptr && num <= mCap) {

        static TexCoord pCoords = {
            math::VEC2(0.0f, 0.0f),
            math::VEC2(1.0f, 0.0f),
            math::VEC2(0.0f, 1.0f),
            math::VEC2(1.0f, 1.0f)
        };

        for (int j = mNum; j < num; j++) {
            for (int i = 0; i < VERTEXCOLOR_MAX; i++) {
                mpData[j][i] = pCoords[i];
            }
        }

        mNum = num;
    }
}

void TexCoordAry::Copy(const void* pSrc, u8 num) {
    mNum = ut::Max<u8>(mNum, num);
    const TexCoord* pSrcCoords = static_cast<const TexCoord*>(pSrc);

    for (int j = 0; j < num; j++) {
        for (int i = 0; i < VERTEXCOLOR_MAX; i++) {
            mpData[j][i] = pSrcCoords[j][i];
        }
    }
}

bool IsModulateVertexColor(ut::Color* pColors, u8 glbAlpha) {
    if (glbAlpha != 255) {
        return true;
    }

    if (pColors != nullptr && (pColors[VERTEXCOLOR_LT] != ut::Color::WHITE ||
                            pColors[VERTEXCOLOR_RT] != ut::Color::WHITE ||
                            pColors[VERTEXCOLOR_LB] != ut::Color::WHITE ||
                            pColors[VERTEXCOLOR_RB] != ut::Color::WHITE)) {
        return true;
    }

    return false;
}

ut::Color MultipleAlpha(ut::Color color, u8 alpha) {
    ut::Color result = color;

    if (alpha != 255) {
        result.a = color.a * alpha / 255;
    }

    return result;
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

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);

    if (modulate) {
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    }

    for (int i = 0; i < numCoord; i++) {
        GXSetVtxAttrFmt(GX_VTXFMT0, static_cast<GXAttr>(GX_VA_TEX0 + i),
                        GX_TEX_ST, GX_F32, 0);
    }
}

void DrawQuad(const math::VEC2& rBase, const Size& rSize, u8 num,
              const TexCoord* pCoords, const ut::Color* pColors) {

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    {
        GXPosition2f32(rBase.x, rBase.y);
        if (pColors != nullptr) {
            GXColor1u32(pColors[VERTEXCOLOR_LT]);
        }
        for (int i = 0; i < num; i++) {
            GXTexCoord2f32(pCoords[i][VERTEXCOLOR_LT].x,
                           pCoords[i][VERTEXCOLOR_LT].y);
        }

        GXPosition2f32(rBase.x + rSize.width, rBase.y);
        if (pColors != nullptr) {
            GXColor1u32(pColors[VERTEXCOLOR_RT]);
        }
        for (int i = 0; i < num; i++) {
            GXTexCoord2f32(pCoords[i][VERTEXCOLOR_RT].x,
                           pCoords[i][VERTEXCOLOR_RT].y);
        }

        GXPosition2f32(rBase.x + rSize.width, rBase.y - rSize.height);
        if (pColors != nullptr) {
            GXColor1u32(pColors[VERTEXCOLOR_RB]);
        }
        for (int i = 0; i < num; i++) {
            GXTexCoord2f32(pCoords[i][VERTEXCOLOR_RB].x,
                           pCoords[i][VERTEXCOLOR_RB].y);
        }

        GXPosition2f32(rBase.x, rBase.y - rSize.height);
        if (pColors != nullptr) {
            GXColor1u32(pColors[VERTEXCOLOR_LB]);
        }
        for (int i = 0; i < num; i++) {
            GXTexCoord2f32(pCoords[i][VERTEXCOLOR_LB].x,
                           pCoords[i][VERTEXCOLOR_LB].y);
        }
    }
    GXEnd();
}

void DrawQuad(const math::VEC2& rBase, const Size& rSize, u8 num,
              const TexCoord* pCoords, const ut::Color* pColors, u8 alpha) {

    ut::Color colorWork[VERTEXCOLOR_MAX];

    if (pColors != nullptr) {
        MultipleAlpha(colorWork, pColors, alpha);
    }

    DrawQuad(rBase, rSize, num, pCoords, pColors ? colorWork : nullptr);
}

}
}
}
