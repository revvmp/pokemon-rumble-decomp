
#include "nw4r/lyt/lyt_material.h"

#include "nw4r/lyt/lyt_layout.h"
#include <nw4r/lyt/lyt_resourceAccessor.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/gx/GXTev.h>
#include <revolution/gx/GXBump.h>
#include <revolution/gx/GXLighting.h>
#include <revolution/gx/GXPixel.h>
#include <revolution/gx/GXTransform.h>
#include "string.h"

static const GXColorS10 DefaultBlackColor = {0, 0, 0, 0};
enum { WhiteColorComponent = 0xFF };

namespace nw4r {
namespace lyt {
namespace {

bool operator==(const GXColorS10 &a, const GXColorS10 &b) {
    return (a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a);
}

bool operator!=(const GXColorS10 &a, const GXColorS10 &b) {
    return !(a == b);
}

void SetWhiteColor(GXColorS10& rColor) {
    rColor.r = WhiteColorComponent;
    rColor.g = WhiteColorComponent;
    rColor.b = WhiteColorComponent;
    rColor.a = WhiteColorComponent;
}

bool IsWhiteColor(const GXColorS10& rColor) {
    return rColor.r == WhiteColorComponent && rColor.g == WhiteColorComponent &&
           rColor.b == WhiteColorComponent && rColor.a == WhiteColorComponent;
}

void CalcTextureMtx(math::MTX34 *pMtx, const TexSRT &texSRT) {
    math::VEC2 center(0.5f, 0.5f);
    f32 cosR = math::CosDeg(texSRT.rotate);
    f32 sinR = math::SinDeg(texSRT.rotate);
    f32 a0, a1;

    a0 = cosR * texSRT.scale.x;
    a1 = -sinR * texSRT.scale.y;
    pMtx->_00 = a0;
    pMtx->_01 = a1;
    pMtx->_02 = 0.0f;
    pMtx->_03 = texSRT.translate.x + center.x + a0 * -center.x + a1 * -center.y;

    a0 = sinR * texSRT.scale.x;
    a1 = cosR * texSRT.scale.y;
    pMtx->_10 = a0;
    pMtx->_11 = a1;
    pMtx->_12 = 0.0f;
    pMtx->_13 = texSRT.translate.y + center.y + a0 * -center.x + a1 * -center.y;

    pMtx->_20 = 0.0f;
    pMtx->_21 = 0.0f;
    pMtx->_22 = 1.0f;
    pMtx->_23 = 0.0f;
}

void CalcIndTexMtx(f32 (*mtx)[3], const TexSRT &texSRT) {
    f32 cosR = math::CosDeg(texSRT.rotate);
    f32 sinR = math::SinDeg(texSRT.rotate);

    mtx[0][0] = cosR * texSRT.scale.x;
    mtx[0][1] = -sinR * texSRT.scale.y;
    mtx[0][2] = texSRT.translate.x;
    mtx[1][0] = sinR * texSRT.scale.x;
    mtx[1][1] = cosR * texSRT.scale.y;
    mtx[1][2] = texSRT.translate.y;
}

void SetColorComponentValue(ut::Color *pCol, u32 compIdx, s16 value) {
    u8 u8Val = ut::Min<s16>(ut::Max<s16>(value, 0), 0xFF);
    switch (compIdx % 4) {
        case 0:  pCol->r = u8Val; break;
        case 1:  pCol->g = u8Val; break;
        case 2:  pCol->b = u8Val; break;
        case 3:  pCol->a = u8Val; break;
        default: break;
    }
}

void SetIndTexMtx(GXIndTexMtxID id, const f32 (*mtx)[3]) {
    f32 m00 = mtx[0][0];
    f32 m01 = mtx[0][1];
    f32 m02 = mtx[0][2];
    f32 m10 = mtx[1][0];
    f32 m11 = mtx[1][1];
    f32 m12 = mtx[1][2];
    f32 a00 = math::FAbs(m00);
    f32 a01 = math::FAbs(m01);
    f32 a02 = math::FAbs(m02);
    f32 a10 = math::FAbs(m10);
    f32 a11 = math::FAbs(m11);
    f32 a12 = math::FAbs(m12);
    s8 scaleExp = 0;

    if ((a00 >= 1.0f || a01 >= 1.0f || a02 >= 1.0f || a10 >= 1.0f || a11 >= 1.0f || a12 >= 1.0f)) {
        do {
            if (scaleExp >= 46) {
                break;
            }
            m00 *= 0.5f;
            m01 *= 0.5f;
            m02 *= 0.5f;
            m10 *= 0.5f;
            m11 *= 0.5f;
            m12 *= 0.5f;
            a00 *= 0.5f;
            a01 *= 0.5f;
            a02 *= 0.5f;
            a10 *= 0.5f;
            a11 *= 0.5f;
            a12 *= 0.5f;
            scaleExp++;
        } while ((a00 >= 1.0f || a01 >= 1.0f || a02 >= 1.0f || a10 >= 1.0f || a11 >= 1.0f || a12 >= 1.0f));
    } else if (a00 < 0.5f && a01 < 0.5f && a02 < 0.5f && a10 < 0.5f && a11 < 0.5f && a12 < 0.5f) {
        do {
            a00 *= 2.0f;
            m00 *= 2.0f;
            m01 *= 2.0f;
            m02 *= 2.0f;
            m10 *= 2.0f;
            m11 *= 2.0f;
            m12 *= 2.0f;
            a01 *= 2.0f;
            a02 *= 2.0f;
            a10 *= 2.0f;
            a11 *= 2.0f;
            a12 *= 2.0f;
            scaleExp--;
        } while (a00 < 0.5f && a01 < 0.5f && a02 < 0.5f && a10 < 0.5f && a11 < 0.5f && a12 < 0.5f && scaleExp > -17);
    }
    f32 outMtx[2][3] = {m00, m01, m02, m10, m11, m12};
    GXSetIndTexMtx(id, outMtx, scaleExp);
}

u32 GetTexMtx(u32 texMtxIdx) {
    return 30 + 3 * texMtxIdx;
}

u32 GetTexMtxIdx(u32 texMtx) {
    return (texMtx - 30) / 3;
}

void InitTexSRT(TexSRT *texSRTs, u32 num) {
    for (u32 i = 0; i < num; i++) {
        texSRTs[i].translate = math::VEC2(0.0f, 0.0f);
        texSRTs[i].rotate = 0.0f;
        texSRTs[i].scale = math::VEC2(1.0f, 1.0f);
    }
}

u32 CalcOffsetTexSRTAry(const detail::BitGXNums &bitGXNums) {
    return bitGXNums.texMap * sizeof(TexMap);
}

u32 CalcOffsetTexCoordGenAry(const detail::BitGXNums &bitGXNums) {
    return bitGXNums.texSRT * RoundUp(sizeof(TexSRT), 4) + CalcOffsetTexSRTAry(bitGXNums);
}

u32 CalcOffsetChanCtrlAry(const detail::BitGXNums &bitGXNums) {
    return bitGXNums.texCoordGen * RoundUp(sizeof(TexCoordGen), 4) + CalcOffsetTexCoordGenAry(bitGXNums);
}

u32 CalcOffsetMatColAry(const detail::BitGXNums &bitGXNums) {
    return bitGXNums.chanCtrl * RoundUp(sizeof(ChanCtrl), 4) + CalcOffsetChanCtrlAry(bitGXNums);
}

u32 CalcOffsetTevSwapAry(const detail::BitGXNums &bitGXNums) {
    return bitGXNums.matCol * RoundUp(sizeof(ut::Color), 4) + CalcOffsetMatColAry(bitGXNums);
}

u32 CalcOffsetGetAlphaCompare(const detail::BitGXNums &bitGXNums) {

    return bitGXNums.tevSwap * RoundUp(sizeof(TevSwapMode), 4) + CalcOffsetTevSwapAry(bitGXNums);
}

u32 CalcOffsetBlendMode(const detail::BitGXNums &bitGXNums) {
    return bitGXNums.alpComp * RoundUp(sizeof(AlphaCompare), 4) + CalcOffsetGetAlphaCompare(bitGXNums);
}

u32 CalcOffsetIndirectStageAry(const detail::BitGXNums &bitGXNums) {
    return bitGXNums.blendMode * RoundUp(sizeof(BlendMode), 4) + CalcOffsetBlendMode(bitGXNums);
}

u32 CalcOffsetIndTexSRTAry(const detail::BitGXNums &bitGXNums) {
    return bitGXNums.indStage * RoundUp(sizeof(IndirectStage), 4) + CalcOffsetIndirectStageAry(bitGXNums);
}

u32 CalcOffsetTevStageAry(const detail::BitGXNums &bitGXNums) {
    return bitGXNums.indSRT * RoundUp(sizeof(TexSRT), 4) + CalcOffsetIndTexSRTAry(bitGXNums);
}

}

Material::Material() {
    Init();
    memset(mName, 0, sizeof(mName));
}

#pragma dont_inline on

Material::Material(const res::Material *pRes, const ResBlockSet &resBlockSet) : mAnimList(), mTevKCols() {
    Init();
    SetName(pRes->name);
    for (int i = 0; i < 3; i++) {
        mTevCols[i] = pRes->tevCols[i];
    }
    for (int i = 0; i < 4; i++) {
        mTevKCols[i] = pRes->tevKCols[i];
    }

    u32 resOffs = sizeof(res::Material);
    const res::TexMap *pResTexMap = detail::ConvertOffsToPtr<res::TexMap>(pRes, resOffs);
    resOffs += pRes->resNum.GetTexMapNum() * sizeof(res::TexMap);

    const TexSRT *pResTexSRTs = detail::ConvertOffsToPtr<TexSRT>(pRes, resOffs);
    resOffs += pRes->resNum.GetTexSRTNum() * sizeof(TexSRT);

    const TexCoordGen *resTexCoordGen = detail::ConvertOffsToPtr<TexCoordGen>(pRes, resOffs);
    resOffs += pRes->resNum.GetTexCoordGenNum() * sizeof(TexCoordGen);

    u8 texMapNum = ut::Min<u8>(pRes->resNum.GetTexMapNum(), 8);
    u8 texSRTNum = ut::Min<u8>(pRes->resNum.GetTexSRTNum(), 10);
    u8 texCoordGenNum = ut::Min<u8>(pRes->resNum.GetTexCoordGenNum(), 8);
    bool allocChanCtrl = pRes->resNum.GetChanCtrlNum() != 0;
    bool allocMatCol = pRes->resNum.GetMatColNum() != 0;
    bool allocTevSwap = pRes->resNum.HasTevSwapTable();
    bool allocAlpComp = pRes->resNum.HasAlphaCompare();
    bool allocBlendMode = pRes->resNum.HasBlendMode();
    u8 indTexSRTNum = ut::Min<u8>(pRes->resNum.GetIndTexSRTNum(), 3);
    u8 indStageNum = ut::Min<u8>(pRes->resNum.GetIndTexStageNum(), 4);
    u8 tevStageNum = ut::Min<u8>(pRes->resNum.GetTevStageNum(), 16);

    ReserveGXMem(
        texMapNum, texSRTNum, texCoordGenNum, tevStageNum, allocTevSwap, indStageNum, indTexSRTNum, allocChanCtrl,
        allocMatCol, allocAlpComp, allocBlendMode
    );

    if (mpGXMem) {
        SetTextureNum(texMapNum);
        if (texMapNum != 0) {
            const res::Texture *textures =
                detail::ConvertOffsToPtr<res::Texture>(resBlockSet.pTextureList, sizeof(res::TextureList));
            TexMap *texMaps = GetTexMapAry();
            u8 di = 0;
            for (u8 si = 0; si < texMapNum; di++, si++) {
                const char *fileName =
                    detail::ConvertOffsToPtr<char>(textures, textures[pResTexMap[si].texIdx].nameStrOffset);
                void *pTplRes = resBlockSet.pResAccessor->GetResource('timg', fileName, nullptr);
                texMaps[di].ReplaceImage((TPLPalette *)pTplRes, 0);
                texMaps[di].SetWrapMode(pResTexMap[si].GetWarpModeS(), pResTexMap[si].GetWarpModeT());
                texMaps[di].SetFilter(pResTexMap[si].GetMinFilter(), pResTexMap[si].GetMagFilter());
            }
            SetTextureNum(di);
        }
        TexSRT *texSRTs = GetTexSRTAry();
        for (int i = 0; i < texSRTNum; i++) {
            texSRTs[i].translate = pResTexSRTs[i].translate;
            texSRTs[i].rotate = pResTexSRTs[i].rotate;
            texSRTs[i].scale = pResTexSRTs[i].scale;
        }

        TexCoordGen *texCoordGens = GetTexCoordGenAry();
        SetTexCoordGenNum(texCoordGenNum);
        for (int i = 0; i < mGXMemNum.texCoordGen; i++) {
            texCoordGens[i] = resTexCoordGen[i];
        }

        if (allocChanCtrl) {
            const ChanCtrl *pResChanCtrl = detail::ConvertOffsToPtr<ChanCtrl>(pRes, resOffs);
            GetChanCtrlAry()[0] = *pResChanCtrl;
            resOffs += sizeof(ChanCtrl);
        }

        if (allocMatCol) {
            const ut::Color *pResMatCol = detail::ConvertOffsToPtr<ut::Color>(pRes, resOffs);
            GetMatColAry()[0] = *pResMatCol;
            resOffs += sizeof(ut::Color);
        }

        if (allocTevSwap) {
            const TevSwapMode *tevSwaps = detail::ConvertOffsToPtr<TevSwapMode>(pRes, resOffs);
            TevSwapMode *pResTevSwap = GetTevSwapAry();
            for (int i = 0; i < 4; i++) {
                pResTevSwap[i] = tevSwaps[i];
            }
            resOffs += 4 * sizeof(TevSwapMode);
        }

        if (indTexSRTNum != 0) {
            TexSRT *indTexSRTs = GetIndTexSRTAry();
            const TexSRT *pResIndMtx = detail::ConvertOffsToPtr<TexSRT>(pRes, resOffs);
            for (int i = 0; i < indTexSRTNum; i++) {
                indTexSRTs[i] = pResIndMtx[i];
            }
        }
        resOffs += pRes->resNum.GetIndTexSRTNum() * sizeof(TexSRT);

        if (indStageNum != 0) {
            SetIndStageNum(indStageNum);
            IndirectStage *indirectStages = GetIndirectStageAry();
            const IndirectStage *pResIndStg = detail::ConvertOffsToPtr<IndirectStage>(pRes, resOffs);
            for (int i = 0; i < indStageNum; i++) {
                indirectStages[i] = pResIndStg[i];
            }
        }
        resOffs += pRes->resNum.GetIndTexStageNum() * sizeof(IndirectStage);

        if (tevStageNum != 0) {
            SetTevStageNum(tevStageNum);
            TevStage *tevStages = GetTevStageAry();
            const TevStage *pResTevStg = detail::ConvertOffsToPtr<TevStage>(pRes, resOffs);
            for (int i = 0; i < tevStageNum; i++) {
                tevStages[i] = pResTevStg[i];
            }
        }
        resOffs += pRes->resNum.GetTevStageNum() * sizeof(TevStage);

        if (allocAlpComp) {
            const AlphaCompare *pResAlphaCompare = detail::ConvertOffsToPtr<AlphaCompare>(pRes, resOffs);
            *GetAlphaComparePtr() = *pResAlphaCompare;
            resOffs += sizeof(AlphaCompare);
        }
        if (allocBlendMode) {
            const BlendMode *pResBlendMode = detail::ConvertOffsToPtr<BlendMode>(pRes, resOffs);
            *GetBlendModePtr() = *pResBlendMode;
        }
    }
}
#pragma dont_inline reset

void Material::Init() {
    mTevCols[0] = DefaultBlackColor;
    SetWhiteColor(mTevCols[1]);
    SetWhiteColor(mTevCols[2]);
    InitBitGXNums(&mGXMemCap);
    InitBitGXNums(&mGXMemNum);
    mbUserAllocated = false;
    mpGXMem = nullptr;
}

Material::~Material() {
    UnbindAllAnimation();
    if (mpGXMem) {
        Layout::FreeMemory(mpGXMem);
        mpGXMem = nullptr;
    }
}

void Material::InitBitGXNums(detail::BitGXNums *ptr) {
    ptr->texMap = 0;
    ptr->texSRT = 0;
    ptr->texCoordGen = 0;
    ptr->indSRT = 0;
    ptr->indStage = 0;
    ptr->tevSwap = 0;
    ptr->tevStage = 0;
    ptr->chanCtrl = 0;
    ptr->matCol = 0;
    ptr->alpComp = 0;
    ptr->blendMode = 0;
}

void Material::ReserveGXMem(
    u8 texMapNum, u8 texSRTNum, u8 texCoordGenNum, u8 tevStageNum, bool allocTevSwap, u8 indStageNum, u8 indSRTNum,
    bool allocChanCtrl, bool allocMatCol, bool allocAlpComp, bool allocBlendMode
) {
    int tevSwapNum = allocTevSwap;
    int chanCtrlNum = allocChanCtrl;
    int matColNum = allocMatCol;
    int alpCompNum = allocAlpComp;
    int blendModeNum = allocBlendMode;
    if (mGXMemCap.texMap < texMapNum || mGXMemCap.texSRT < texSRTNum || mGXMemCap.texCoordGen < texCoordGenNum ||
        mGXMemCap.tevStage < tevStageNum || mGXMemCap.tevSwap < tevSwapNum || mGXMemCap.indStage < indStageNum ||
        mGXMemCap.indSRT < indSRTNum || mGXMemCap.chanCtrl < chanCtrlNum || mGXMemCap.matCol < matColNum ||
        mGXMemCap.alpComp < alpCompNum || mGXMemCap.blendMode < blendModeNum) {
        if (mpGXMem) {
            Layout::FreeMemory(mpGXMem);
            mpGXMem = nullptr;
            InitBitGXNums(&mGXMemCap);
            InitBitGXNums(&mGXMemNum);
        }
        mpGXMem = Layout::AllocMemory(
            (texCoordGenNum + chanCtrlNum + matColNum + tevSwapNum + alpCompNum + blendModeNum + indStageNum) * 4 +
            texMapNum * sizeof(TexMap) + tevStageNum * sizeof(TevStage) + texSRTNum * sizeof(TexSRT) +
            indSRTNum * sizeof(TexSRT)
        );

        if (mpGXMem) {
            mGXMemCap.texMap = texMapNum;
            mGXMemCap.texSRT = texSRTNum;
            mGXMemCap.texCoordGen = texCoordGenNum;
            mGXMemCap.indSRT = indSRTNum;
            mGXMemCap.indStage = indStageNum;
            mGXMemCap.tevSwap = tevSwapNum;
            mGXMemCap.tevStage = tevStageNum;
            mGXMemCap.chanCtrl = chanCtrlNum;
            mGXMemCap.matCol = matColNum;
            mGXMemCap.alpComp = alpCompNum;
            mGXMemCap.blendMode = blendModeNum;

            mGXMemNum.texSRT = mGXMemCap.texSRT;
            InitTexSRT(GetTexSRTAry(), mGXMemNum.texSRT);

            mGXMemNum.indSRT = mGXMemCap.indSRT;
            InitTexSRT(GetIndTexSRTAry(), mGXMemNum.indSRT);

            mGXMemNum.chanCtrl = mGXMemCap.chanCtrl;
            if (mGXMemNum.chanCtrl != 0) {
                GetChanCtrlAry()[0] = ChanCtrl();
            }

            mGXMemNum.matCol = mGXMemCap.matCol;
            if (mGXMemNum.matCol != 0) {
                GetMatColAry()[0] = 0xFFFFFFFF;
            }

            mGXMemNum.tevSwap = mGXMemCap.tevSwap;
            if (mGXMemNum.tevSwap != 0) {
                TevSwapMode *tevSwaps = GetTevSwapAry();
                tevSwaps[0].Set(GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
                tevSwaps[1].Set(GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
                tevSwaps[2].Set(GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
                tevSwaps[3].Set(GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
            }

            mGXMemNum.alpComp = mGXMemCap.alpComp;
            if (mGXMemNum.alpComp != 0) {
                *GetAlphaComparePtr() = AlphaCompare();
            }

            mGXMemNum.blendMode = mGXMemCap.blendMode;
            if (mGXMemNum.blendMode != 0) {
                *GetBlendModePtr() = BlendMode();
            }
        }
    }
}

const TexMap *Material::GetTexMapAry() const {
    return reinterpret_cast<TexMap*>(static_cast<u8*>(mpGXMem) + 0);
}

TexMap *Material::GetTexMapAry() {
    return (reinterpret_cast<TexMap*>(static_cast<u8*>(mpGXMem) + 0));
}

const TexSRT *Material::GetTexSRTAry() const {
    return detail::ConvertOffsToPtr<TexSRT>(mpGXMem, CalcOffsetTexSRTAry(mGXMemCap));
}

TexSRT *Material::GetTexSRTAry() {
    return detail::ConvertOffsToPtr<TexSRT>(mpGXMem, CalcOffsetTexSRTAry(mGXMemCap));
}

TexCoordGen *Material::GetTexCoordGenAry() {
    return reinterpret_cast<TexCoordGen*>(static_cast<u8*>(mpGXMem) + CalcOffsetTexCoordGenAry(mGXMemCap));
}

ChanCtrl *Material::GetChanCtrlAry() {
    return reinterpret_cast<ChanCtrl*>(static_cast<u8*>(mpGXMem) + CalcOffsetChanCtrlAry(mGXMemCap));
}

ut::Color *Material::GetMatColAry() {
    return reinterpret_cast<ut::Color*>(static_cast<u8*>(mpGXMem) + CalcOffsetMatColAry(mGXMemCap));
}

TevSwapMode *Material::GetTevSwapAry() {
    return reinterpret_cast<TevSwapMode*>(static_cast<u8*>(mpGXMem) + CalcOffsetTevSwapAry(mGXMemCap));
}

AlphaCompare *Material::GetAlphaComparePtr() {
    return reinterpret_cast<AlphaCompare*>(static_cast<u8*>(mpGXMem) + CalcOffsetGetAlphaCompare(mGXMemCap));
}

BlendMode *Material::GetBlendModePtr() {
    return reinterpret_cast<BlendMode*>(static_cast<u8*>(mpGXMem) + CalcOffsetBlendMode(mGXMemCap));
}

IndirectStage *Material::GetIndirectStageAry() {
    return reinterpret_cast<IndirectStage*>(static_cast<u8*>(mpGXMem) + CalcOffsetIndirectStageAry(mGXMemCap));
}

TexSRT *Material::GetIndTexSRTAry() {
    return reinterpret_cast<TexSRT*>(static_cast<u8*>(mpGXMem) + CalcOffsetIndTexSRTAry(mGXMemCap));
}

TevStage *Material::GetTevStageAry() {
    return reinterpret_cast<TevStage*>(static_cast<u8*>(mpGXMem) + CalcOffsetTevStageAry(mGXMemCap));
}

void Material::SetName(const char *name) {
    strncpy(mName, name, 20);
    mName[20] = '\0';
}

void Material::SetTextureNum(u8 num) {
    if (num != 0) {
        TexMap *texMaps = GetTexMapAry();
        for (u32 i = mGXMemNum.texMap; i < num; i++) {
            new (&texMaps[i]) TexMap();

        }
    }
    mGXMemNum.texMap = num;
}

void Material::SetTexCoordGenNum(u8 num) {
    if (num != 0) {
        TexCoordGen *texCoordGens = GetTexCoordGenAry();
        for (u32 i = mGXMemNum.texCoordGen; i < num; i++) {
            new (&texCoordGens[i]) TexCoordGen();
        }
    }
    mGXMemNum.texCoordGen = num;
}

void Material::SetTevStageNum(u8 num) {
    if (num != 0) {
        TevStage *tevStages = GetTevStageAry();
        for (u32 i = mGXMemNum.tevStage; i < num; i++) {
            new (&tevStages[i]) TevStage();
        }
    }
    mGXMemNum.tevStage = num;
}

void Material::SetIndStageNum(u8 num) {
    if (num != 0) {
        IndirectStage *indStages = GetIndirectStageAry();
        for (u32 i = mGXMemNum.indStage; i < num; i++) {
            new (&indStages[i]) IndirectStage();
        }
    }
    mGXMemNum.indStage = num;
}

void Material::SetColorElement(u32 colorType, s16 value) {

    switch (colorType) {
    case 0: case 1: case 2: case 3: {
        if (mGXMemNum.matCol < 1 ) {
            break;
        }
        ut::Color *matCols = GetMatColAry();
        SetColorComponentValue(matCols, colorType , value);
        break;
    }
    case  4: case  5: case  6: case  7: case  8: case  9:
    case 10: case 11: case 12: case 13: case 14: case 15: {
        u32 regIdx = (colorType - 4) / 4;
        switch ((colorType - 4) % 4) {
        case 0:
            mTevCols[regIdx].r = value;
            break;
        case 1:
            mTevCols[regIdx].g = value;
            break;
        case 2:
            mTevCols[regIdx].b = value;
            break;
        case 3:
            mTevCols[regIdx].a = value;
            break;
        }
    } break;
    case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23:
    case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: {
        u32 regIdx = (colorType - 16) / 4;
        SetColorComponentValue(&mTevKCols[regIdx], (colorType - 16) , value);
    } break;
    }

}

bool Material::SetupGX(bool bModVtxCol, u8 alpha) {
    bool bUseVtxCol = true;
    bool bUseMatCol = false;
    GXSetNumChans(1);
    if (IsChanCtrlCap()) {
        const ChanCtrl *chanCtrls = GetChanCtrlAry();
        GXSetChanCtrl(GX_COLOR0, false, GX_SRC_REG, chanCtrls->GetColorSrc(), GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
        GXSetChanCtrl(GX_ALPHA0, false, GX_SRC_REG, chanCtrls->GetAlphaSrc(), GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);

        bUseVtxCol = chanCtrls->GetColorSrc() == GX_SRC_VTX || chanCtrls->GetAlphaSrc() == GX_SRC_VTX;
        bUseMatCol = chanCtrls->GetColorSrc() == GX_SRC_REG || chanCtrls->GetAlphaSrc() == GX_SRC_REG;
    } else {
        GXSetChanCtrl(GX_COLOR0A0, FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    }

    bool bUseRasStage = false;
    if (bUseVtxCol) {
        bUseRasStage = bUseRasStage || bModVtxCol;
    }

    if (bUseMatCol) {
        ut::Color matCol = 0xFFFFFFFF;
        if (IsMatColorCap()) {
            matCol = GetMatColAry()[0];
        }
        matCol = detail::MultipleAlpha(matCol, alpha);
        GXSetChanMatColor(GX_COLOR0A0, matCol);
        bUseRasStage = bUseRasStage || matCol != 0xFFFFFFFF;
    }
    bool bSetTexMtx = false;
    bool bUseTexMtx[10];
    for (int i = 0; i < 10; i++) {
        bUseTexMtx[i] = false;
    }
    GXSetNumTexGens(mGXMemNum.texCoordGen);
    if (mGXMemNum.texCoordGen != 0) {
        const TexCoordGen *texCoordGens = GetTexCoordGenAry();
        for (int i = 0; i < mGXMemNum.texCoordGen; i++) {
            u32 texMtx = texCoordGens[i].GetTexMtx();
            if (texCoordGens[i].GetTexGenType() == GX_TEXMAP1 && texMtx != 0x3C) {
                bUseTexMtx[GetTexMtxIdx(texMtx)] = true;
                bSetTexMtx = true;
            }
            GXSetTexCoordGen2(
                (GXTexCoordID)i, texCoordGens[i].GetTexGenType(), texCoordGens[i].GetTexGenSrc(), texMtx, FALSE, 0x7D
            );
        }
    }
    if (bSetTexMtx) {
        const TexSRT *texSRTs = GetTexSRTAry();

        for (u8 i = 0; i < mGXMemNum.texSRT; i++) {
            if (bUseTexMtx[i]) {
                math::MTX34 texMtx;
                CalcTextureMtx(&texMtx, texSRTs[i]);
                GXLoadTexMtxImm(texMtx, GetTexMtx(i), GX_MTX2x4);
            }
        }
    }

    if (mGXMemNum.texMap) {
        u32 tlutID = 0;
        u32 bigTlutID = 16;

        const TexMap *texMaps = GetTexMapAry();
        for (int i = 0; i < mGXMemNum.texMap; i++) {
            const TexMap &texMap = texMaps[i];
            GXTexObj texObj;
            texMap.Get(&texObj);
            if (detail::IsCITexelFormat(texMap.GetTexelFormat())) {
                u32 tlutName;
                if (texMap.GetTexelFormat() == 10) {
                    tlutName = bigTlutID++;
                } else {
                    tlutName = tlutID++;
                }
                GXInitTexObjTlut(&texObj, tlutName);
                GXTlutObj tlutObj;
                texMap.Get(&tlutObj);
                GXLoadTlut(&tlutObj, tlutName);
            }
            GXLoadTexObj(&texObj, (GXTexMapID)i);
        }
    }

    GXSetTevColorS10(GX_TEVREG0, mTevCols[0]);
    GXSetTevColorS10(GX_TEVREG1, mTevCols[1]);
    GXSetTevColorS10(GX_TEVREG2, mTevCols[2]);
    GXSetTevKColor(GX_KCOLOR0, mTevKCols[0]);
    GXSetTevKColor(GX_KCOLOR1, mTevKCols[1]);
    GXSetTevKColor(GX_KCOLOR2, mTevKCols[2]);
    GXSetTevKColor(GX_KCOLOR3, mTevKCols[3]);

    if (IsTevSwapCap()) {
        const TevSwapMode *tevSwaps = GetTevSwapAry();
        for (int i = 0; i < 4; i++) {
            GXSetTevSwapModeTable(
                (GXTevSwapSel)i, tevSwaps[i].GetR(), tevSwaps[i].GetG(), tevSwaps[i].GetB(), tevSwaps[i].GetA()
            );
        }
    } else {
        GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
    }

    bool bSetIndTexMtx = false;
    bool bUseIndTexMtx[3];
    for (int i = 0; i < 3; i++) {
        bUseIndTexMtx[i] = false;
    }
    if (mGXMemNum.tevStage != 0) {
        GXSetNumTevStages(mGXMemNum.tevStage);
        const TevStage *tevStages = GetTevStageAry();
        for (int i = 0; i < mGXMemNum.tevStage; i++) {
            GXTevStageID tevStage = (GXTevStageID)i;
            GXSetTevOrder(
                tevStage, tevStages[i].GetTexCoordGen(), tevStages[i].GetTexMap(), tevStages[i].GetColorChan()
            );
            GXSetTevSwapMode(tevStage, tevStages[i].GetRasSwapSel(), tevStages[i].GetTexSwapSel());
            GXSetTevColorIn(
                tevStage, tevStages[i].GetColorInA(), tevStages[i].GetColorInB(), tevStages[i].GetColorInC(),
                tevStages[i].GetColorInD()
            );
            GXSetTevColorOp(
                tevStage, tevStages[i].GetColorOp(), tevStages[i].GetColorBias(), tevStages[i].GetColorScale(),
                tevStages[i].IsColorClamp(), tevStages[i].GetColorOutReg()
            );
            GXSetTevKColorSel(tevStage, tevStages[i].GetKColorSel());
            GXSetTevAlphaIn(
                tevStage, tevStages[i].GetAlphaInA(), tevStages[i].GetAlphaInB(), tevStages[i].GetAlphaInC(),
                tevStages[i].GetAlphaInD()
            );
            GXSetTevAlphaOp(
                tevStage, tevStages[i].GetAlphaOp(), tevStages[i].GetAlphaBias(), tevStages[i].GetAlphaScale(),
                tevStages[i].IsAlphaClamp(), tevStages[i].GetAlphaOutReg()
            );
            GXSetTevKAlphaSel(tevStage, tevStages[i].GetKAlphaSel());
            GXIndTexMtxID indMtxSel = tevStages[i].GetIndMtxSel();
            GXSetTevIndirect(
                tevStage, tevStages[i].GetIndStage(), tevStages[i].GetIndFormat(), tevStages[i].GetIndBiasSel(),
                indMtxSel, tevStages[i].GetIndWrapS(), tevStages[i].GetIndWrapT(), tevStages[i].IsIndAddPrev(),
                tevStages[i].IsIndUtcLod(), tevStages[i].GetIndAlphaSel()
            );
            if (1 <= indMtxSel && indMtxSel <= 3) {
                bUseIndTexMtx[indMtxSel - 1] = true;
                bSetIndTexMtx = true;
            }
        }
        bUseRasStage = true;
    } else {
        static GXTevKColorSel kColSels[8] = {
            GX_TEV_KCSEL_K3_A, GX_TEV_KCSEL_K3_B, GX_TEV_KCSEL_K3_G, GX_TEV_KCSEL_K3_R,
            GX_TEV_KCSEL_K2_A, GX_TEV_KCSEL_K2_B, GX_TEV_KCSEL_K2_G, GX_TEV_KCSEL_K2_R,
        };
        static GXTevKAlphaSel kAlpSels[8] = {
            GX_TEV_KASEL_K3_A, GX_TEV_KASEL_K3_B, GX_TEV_KASEL_K3_G, GX_TEV_KASEL_K3_R,
            GX_TEV_KASEL_K2_A, GX_TEV_KASEL_K2_B, GX_TEV_KASEL_K2_G, GX_TEV_KASEL_K2_R,
        };
        u8 tevStageID = 0;
        if (mGXMemNum.texMap == 0) {
            GXTevStageID tevStage = GX_TEVSTAGE0;
            GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
            GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_C1, GX_CC_RASC, GX_CC_ZERO);
            GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_A1, GX_CA_RASA, GX_CA_ZERO);
            bUseRasStage = true;
            tevStageID = 1;
        } else {
            if (mGXMemNum.texMap == 1) {
                GXTevStageID tevStage = GX_TEVSTAGE0;
                GXSetTevOrder(tevStage, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
                GXSetTevColorIn(tevStage, GX_CC_C0, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
                GXSetTevAlphaIn(tevStage, GX_CA_A0, GX_CA_A1, GX_CA_TEXA, GX_CA_ZERO);
                tevStageID = 1;
            } else {
                if (mGXMemNum.texMap == 2) {
                    GXTevStageID tevStage = GX_TEVSTAGE0;
                    GXSetTevOrder(tevStage, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
                    GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
                    GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);

                    tevStage = GX_TEVSTAGE1;
                    GXSetTevOrder(tevStage, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
                    GXSetTevColorIn(tevStage, GX_CC_TEXC, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
                    GXSetTevAlphaIn(tevStage, GX_CA_TEXA, GX_CA_APREV, GX_CA_KONST, GX_CA_ZERO);
                    GXSetTevKColorSel(tevStage, kColSels[0]);
                    GXSetTevKAlphaSel(tevStage, kAlpSels[0]);
                    tevStageID = 2;

                } else {
                    for (int i = 0; i < mGXMemNum.texMap; i++) {
                        GXTevStageID tevStage = (GXTevStageID)tevStageID;
                        GXSetTevOrder(tevStage, (GXTexCoordID)i, (GXTexMapID)i, GX_COLOR_NULL);
                        GXTevColorArg colDIn = i == 0 ? GX_CC_ZERO : GX_CC_CPREV;
                        GXTevAlphaArg alpDIn = i == 0 ? GX_CA_ZERO : GX_CA_APREV;
                        GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, colDIn);
                        GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, alpDIn);
                        GXSetTevKColorSel(tevStage, kColSels[i]);
                        GXSetTevKAlphaSel(tevStage, kAlpSels[i]);
                        tevStageID++;
                    }
                }
                if (mTevCols[0] != DefaultBlackColor || !IsWhiteColor(mTevCols[1])) {
                    GXTevStageID tevStage = (GXTevStageID)tevStageID;
                    GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
                    GXSetTevColorIn(tevStage, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
                    GXSetTevAlphaIn(tevStage, GX_CA_A0, GX_CA_A1, GX_CA_APREV, GX_CA_ZERO);
                    tevStageID++;
                }
            }
            if (bUseRasStage) {
                GXTevStageID tevStage = (GXTevStageID)tevStageID;
                GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
                GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
                GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
                tevStageID++;
            }
        }

        const u8 tevStageNum = tevStageID;

        for (u8 id = 0; id < tevStageNum; id++) {
            GXTevStageID tevStage = (GXTevStageID)id;
            GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
            GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, TRUE, GX_TEVPREV);
            GXSetTevDirect(tevStage);
            GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
        }
        GXSetNumTevStages(tevStageNum);
    }
    if (bSetIndTexMtx) {
        const TexSRT *indTexSRTs = GetIndTexSRTAry();
        for (int i = 0; i < mGXMemNum.indSRT; i++) {
            if (bUseIndTexMtx[i]) {
                f32 mtx[2][3];
                CalcIndTexMtx(mtx, indTexSRTs[i]);
                SetIndTexMtx((GXIndTexMtxID)(i + 1), mtx);
            }
        }
    }

    GXSetNumIndStages(mGXMemNum.indStage);
    if (mGXMemNum.indStage != 0) {
        const IndirectStage *indirectStages = GetIndirectStageAry();
        for (int i = 0; i < mGXMemNum.indStage; i++) {
            GXIndTexStageID indStage = (GXIndTexStageID)i;
            GXSetIndTexOrder(indStage, indirectStages[i].GetTexCoordGen(), indirectStages[i].GetTexMap());
            GXSetIndTexCoordScale(indStage, indirectStages[i].GetScaleS(), indirectStages[i].GetScaleT());
        }
    }
    if (IsAlphaCompareCap()) {
        const AlphaCompare *pAlpComp = GetAlphaComparePtr();
        GXSetAlphaCompare(
            pAlpComp->GetComp0(), pAlpComp->GetRef0(), pAlpComp->GetOp(), pAlpComp->GetComp1(), pAlpComp->GetRef1()
        );
    } else {
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    }
    if (IsBlendModeCap()) {
        const BlendMode *pBlendMode = GetBlendModePtr();
        GXSetBlendMode(
            pBlendMode->GetType(), pBlendMode->GetSrcFactor(), pBlendMode->GetDstFactor(), pBlendMode->GetOp()
        );

    } else {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    }
    return bUseRasStage && bUseVtxCol;
}

void Material::BindAnimation(AnimTransform *pAnimTrans) {
    pAnimTrans->Bind(this, false);
}

void Material::UnbindAnimation(AnimTransform *pAnimTrans) {
    detail::UnbindAnimationLink(GetAnimationList(), pAnimTrans);
}

void Material::UnbindAllAnimation() {
    UnbindAnimation(nullptr);
}

void Material::Animate() {
    for (ut::LinkList<AnimationLink, 0>::Iterator it = mAnimList.GetBeginIter(); it != mAnimList.GetEndIter(); it++) {
        if (it->IsEnable()) {
            AnimTransform *pAnimTrans = it->GetAnimTransform();
            pAnimTrans->Animate(it->GetIndex(), this);
        }
    }
}

void Material::AddAnimationLink(AnimationLink *pAnimationLink) {
    mAnimList.PushBack(pAnimationLink);
}

AnimationLink *Material::FindAnimationLink(AnimTransform *pAnimTrans) {
    return detail::FindAnimationLink(&mAnimList, pAnimTrans);
}

AnimationLink *Material::FindAnimationLink(const AnimResource &animRes) {
    return detail::FindAnimationLink(&mAnimList, animRes);
}

void Material::SetAnimationEnable(AnimTransform *pAnimTrans, bool bEnable) {
    AnimationLink *pAnimLink = FindAnimationLink(pAnimTrans);
    if (pAnimLink) {
        pAnimLink->SetEnable(bEnable);
    }
}

void Material::SetAnimationEnable(const AnimResource &animRes, bool bEnable) {
    AnimationLink *pAnimLink = FindAnimationLink(animRes);
    if (pAnimLink) {
        pAnimLink->SetEnable(bEnable);
    }
}

namespace detail {

Size GetTextureSize(Material *pMaterial, u8 texMapIdx) {
    if (texMapIdx >= pMaterial->GetTextureNum()) {
        return Size();
    }
    const TexMap &texMap = pMaterial->GetTexMapAry()[texMapIdx];
    return Size(texMap.GetWidth(), texMap.GetHeight());
}

}

}

}
