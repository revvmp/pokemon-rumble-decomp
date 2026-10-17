#include <nw4r/ut/ut_CharWriter.h>
#include <nw4r/ut/ut_Font.h>
#include <revolution/gx.h>
#include <revolution/gx/GXBump.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/gx/GXVert.h>

namespace {

static void SetupGXCommon() {
    static const nw4r::ut::Color fog = 0;

    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fog);
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumIndStages(0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
}

}

namespace nw4r {
namespace ut {

CharWriter::CharWriter() : mAlpha(255), mIsWidthFixed(false), mFixedWidth(0.0f), mFont(nullptr) {
    mLoadingTexture.Reset();
    ResetColorMapping();
    SetGradationMode(GRADMODE_NONE);
    SetTextColor(Color(0xFFFFFFFF));
    SetScale(1.0f, 1.0f);
    SetCursor(0.0f, 0.0f, 0.0f);
    EnableLinearFilter(true, true);
}

CharWriter::~CharWriter() {}

void CharWriter::SetupGX() {
    ResetTextureCache();

    if (mColorMapping.min != 0x00000000 || mColorMapping.max != 0xFFFFFFFF) {
        SetupGXWithColorMapping(mColorMapping.min, mColorMapping.max);
    } else if (mFont != nullptr) {
        switch (mFont->GetTextureFormat()) {
            case GX_TF_I4:
            case GX_TF_I8:     SetupGXForI(); break;
            case GX_TF_IA4:
            case GX_TF_IA8:    SetupGXDefault(); break;
            case GX_TF_RGB565:
            case GX_TF_RGB5A3:
            case GX_TF_RGBA8:  SetupGXForRGBA(); break;
            default:           SetupGXDefault(); break;
        }
    } else {
        SetupGXDefault();
    }
}

void CharWriter::SetupGXForDisplayList() {
    ResetTextureCache();
    if (mColorMapping.min != 0x00000000 || mColorMapping.max != 0xFFFFFFFF) {
        GXSetTevColor(GX_TEVREG0, mTextColor.start);
        GXSetTevKColor(GX_KCOLOR0, mTextColor.end);
        GXSetTevColor(GX_TEVREG1, mColorMapping.min);
        GXSetTevKColor(GX_KCOLOR1, mColorMapping.max);
        SetupGXForDisplayListWithColorMapping();
    } else {
        GXSetTevColor(GX_TEVREG0, mTextColor.start);
        GXSetTevColor(GX_TEVREG1, mTextColor.end);
        if (mFont != nullptr) {
            switch (mFont->GetTextureFormat()) {
            case GX_TF_I4:
            case GX_TF_I8:
                SetupGXForDisplayListForI();
                break;
            case GX_TF_IA4:
            case GX_TF_IA8:
                SetupGXForDisplayListDefault();
                break;
            case GX_TF_RGB565:
            case GX_TF_RGB5A3:
            case GX_TF_RGBA8:
                SetupGXForDisplayListForRGBA();
                break;
            default:
                SetupGXForDisplayListDefault();
                break;
            }
        } else {
            SetupGXForDisplayListDefault();
        }
    }
}


void CharWriter::SetFontSize(f32 width, f32 height) {
    SetScale(width / mFont->GetWidth(), height / mFont->GetHeight());
}

void CharWriter::SetFontSize(f32 height) {
    f32 scale = height / mFont->GetHeight();
    SetScale(scale, scale);
}

f32 CharWriter::GetFontWidth() const {
    return mScale.x * mFont->GetWidth();
}

f32 CharWriter::GetFontHeight() const {
    return mScale.y * mFont->GetHeight();
}

f32 CharWriter::GetFontAscent() const {
    return mScale.y * mFont->GetAscent();
}

f32 CharWriter::GetFontDescent() const {
    return mScale.y * mFont->GetDescent();
}

void CharWriter::EnableLinearFilter(bool atSmall, bool atLarge) {
    mFilter.atSmall = atSmall ? GX_LINEAR : GX_NEAR;
    mFilter.atLarge = atLarge ? GX_LINEAR : GX_NEAR;
}

void CharWriter::UpdateVertexColor() {

    mVertexColor.lu = mTextColor.start;
    mVertexColor.ru = mTextColor.gradationMode != GRADMODE_H    ? mTextColor.start : mTextColor.end;
    mVertexColor.ld = mTextColor.gradationMode != GRADMODE_V    ? mTextColor.start : mTextColor.end;
    mVertexColor.rd = mTextColor.gradationMode == GRADMODE_NONE ? mTextColor.start : mTextColor.end;

    mVertexColor.lu.a = (mVertexColor.lu.a * mAlpha) / 255, mVertexColor.ru.a = (mVertexColor.ru.a * mAlpha) / 255;
    mVertexColor.ld.a = (mVertexColor.ld.a * mAlpha) / 255;
    mVertexColor.rd.a = (mVertexColor.rd.a * mAlpha) / 255;
}

void CharWriter::SetupVertexFormat() {
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U16, 15);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}

void CharWriter::SetupGXDefault() {
    SetupGXCommon();

    GXSetNumTevStages(1);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);

    SetupVertexFormat();
}

void CharWriter::SetupGXWithColorMapping(Color min, Color max) {
    SetupGXCommon();

    GXSetNumTevStages(2);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevDirect(GX_TEVSTAGE1);

    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);

    GXSetTevColor(GX_TEVREG0, min);
    GXSetTevColor(GX_TEVREG1, max);

    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A0, GX_CA_A1, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

    SetupVertexFormat();
}

void CharWriter::SetupGXForDisplayListDefault() {
    SetupGXCommon();
    GXSetNumTevStages(2);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevDirect(GX_TEVSTAGE1);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_C1, GX_CC_RASC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A0, GX_CA_A1, GX_CA_RASA, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevOp(GX_TEVSTAGE1, GX_MODULATE);
    SetupVertexFormat();
}

void CharWriter::SetupGXForDisplayListWithColorMapping() {
    SetupGXCommon();
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K1);
    GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K1_A);
    GXSetNumTevStages(3);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevDirect(GX_TEVSTAGE1);
    GXSetTevDirect(GX_TEVSTAGE2);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_KONST, GX_CC_RASC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A0, GX_CA_KONST, GX_CA_RASA, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_A1, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C2, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_A2, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    SetupVertexFormat();
}

void CharWriter::SetupGXForDisplayListForI() {
    SetupGXCommon();
    GXSetNumTevStages(2);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevDirect(GX_TEVSTAGE1);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_C1, GX_CC_RASC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A0, GX_CA_A1, GX_CA_RASA, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    SetupVertexFormat();
}

void CharWriter::SetupGXForDisplayListForRGBA() {
    SetupGXForDisplayListDefault();
}

void CharWriter::SetupGXForI() {
    SetupGXCommon();

    GXSetNumTevStages(1);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

    SetupVertexFormat();
}

void CharWriter::SetupGXForRGBA() {
    SetupGXDefault();
}

CharWriter::LoadingTexture CharWriter::mLoadingTexture;

}
}
